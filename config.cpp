//
// Created by root on 3/1/16.
//

#include <stdexcept>
#include <dirent.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <asm-generic/errno-base.h>

#include "config.h"

namespace ft {
    /*
     * 配置文件的全路径名
     */
    std::string Config::config_file_pathname_ = "/apps/yfq/server.conf";
    /*
     * 锁文件，用于确保本进程单一实例
     */
    std::string Config::singleton_lock_file_ = "/apps/yfq/lock";
    /*
     * TCP监听的普通端口，用于命令和短小数据传输
     */
    int Config::tcp_common_port_ = 8888;
    /*
     * soap监听的端口
     */
    int Config::soap_port_ = 9999;
    /*
     * soap的xml文件的全路径
     */
    std::string Config::soap_xml_filepath_ = "/apps/yfq/ns.wsdl";
    /*
     * 日志文件的存放路径，必须是绝对路径
     */
    std::string Config::log_file_path_ = "/apps/yfq/log";
    /*
     * 日志文件的basename
     */
    std::string Config::log_file_basename_ = "yfq";
    /*
     * 日志的输出级别
     */
    muduo::Logger::LogLevel Config::log_level_ = muduo::Logger::INFO;
    /*
     * 本程序使用的时区文件
     */
    muduo::TimeZone Config::timezone_("/usr/share/zoneinfo/Asia/Hong_Kong");
    /*
     * 通信部分使用的线程数
     */
    unsigned char Config::count_of_threads_for_communication_ = 1;
    /*
     * 业务部分使用的线程数
     */
    unsigned char Config::count_of_threads_for_business_ = 1;
    /*
     * whether log to stdout or stderr
     */
    bool Config::log_to_stdout_ = false;
    /*
     * whether log to stdout or stderr
     */
    bool Config::log_to_file_ = true;
    /*
     * the max size of per logfile, will switch a new, the unit of size is Mbyte.
     * if it's 0, then the size is unlimited
     */
    size_t Config::logfile_max_size_ = 0;
    /*
     * mysql服务器主机地址
     */
    std::string Config::mysql_hostname_ = "192.168.4.222";
    /*
     * mysql服务端端口
     */
    std::string Config::mysql_port_ = "3306";
    /*
     * the mysql schema we use
     */
    std::string Config::mysql_schema_ = "YFQ_DATABASE";
    /*
     * mysql连接池最大连接数
     */
    size_t Config::mysql_connection_pool_max_ = 2;
    /*
     * mysql连接池初始连接数
     */
    size_t Config::mysql_connection_pool_init_ = 1;
    /*
     * mysql用户名
     */
    std::string Config::mysql_username_ = "root";
    /*
     * mysql密码
     */
    std::string Config::mysql_password_ = "123456";
    /*
     * file for log
     */
    std::shared_ptr<muduo::LogFile> Config::logfile;
    /*
     * the process is daemon or not
     */
    bool Config::isDaemon_ = false;

    /**
	 * init log
	 */
    void Config::initlog() {
        // level of log
        muduo::Logger::setLogLevel(log_level_);
        // timezone of log
        muduo::Logger::setTimeZone(timezone_);
        // output file for log
        if (log_to_file()) {
            // make sure the $logpathname is existing
            ensureLogPathExist(log_file_path_.c_str());
            logfile.reset(
                    new muduo::LogFile(log_file_path_.c_str(), log_file_basename_.c_str(), logfile_max_size(), true));

            // to flush logfile before the process die
            ::atexit([] {
//                puts("flush logfile at exit");
                logfile->flush();
            });
        }
        /**
         * set the action of logout
         */
        if (log_to_stdout() && log_to_file()) {
            muduo::Logger::setOutput([](const char* msg, int len) {
                fprintf(stdout, "%s", msg);
                logfile->append(msg, len);
            });
            muduo::Logger::setFlush([] {
                fflush(stdout);
                logfile->flush();
            });
        } else if (log_to_stdout() && !log_to_file()) {
            muduo::Logger::setOutput([](const char* msg, int len) {
                fprintf(stdout, "%s", msg);
            });
            muduo::Logger::setFlush([] {
                fflush(stdout);
            });
        } else if (!log_to_stdout() && log_to_file()) {
            muduo::Logger::setOutput([](const char* msg, int len) {
                logfile->append(msg, len);
            });
            muduo::Logger::setFlush([] {
                logfile->flush();
            });
        }
    }

    void Config::applyConfigFile(const std::string& configfile) {
        readFromConfigFile(configfile.c_str());
//        initlog();
    }

    void Config::readFromConfigFile(const char* configfile) {
        char *addr;
        int fd;
        struct stat sb;

        if (-1 == (fd = ::open(configfile, O_RDONLY))) {
            ::fprintf(stderr, "failed to open config-file(\"%s\"): %s\n", config_file_pathname_.c_str(), ::strerror(errno));
            ::abort();
        }

        if (::fstat(fd, &sb) == -1) {          /* To obtain file size */
            ::fprintf(stderr, "failed to fstat config-file(\"%s\"): %s\n", config_file_pathname_.c_str(), ::strerror(errno));
            ::abort();
        }

        addr = reinterpret_cast<char *>(::mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
        if (addr == MAP_FAILED) {
            ::fprintf(stderr, "failed to mmap config-file(\"%s\"): %s\n", config_file_pathname_.c_str(), ::strerror(errno));
            ::abort();
        }
        if (0 != close(fd)) {
            ::fprintf(stderr, "failed to close config-file(\"%s\"): %s\n", config_file_pathname_.c_str(), ::strerror(errno));
            ::exit(1);
        }

        handfile(addr, sb.st_size);

        if (-1 == ::munmap(addr, sb.st_size)) {
            ::fprintf(stderr, "failed to munmap config-file(\"%s\"): %s\n", config_file_pathname_.c_str(), ::strerror(errno));
            ::abort();
        }
    }

    void Config::handfile(const char *buf, ssize_t len) {
        const char* begin = buf;
        const char* end = begin + len;

        std::string key;
        std::string value;
        std::string* ps = &key;
        bool annotated = false;

        while (begin != end) {
            char c = *(begin++);

            if ('#' == c) {
                annotated = true;
                continue;
            } else if (isblank(c)) {
                continue;
            } else if ('\r' == c || '\n' == c) {
                if (key.size() > 0 && value.size() > 0) {
                    if (!setParam(key, value)) {
                        ::fprintf(stderr, "invalid key and value: %s=%s\n", key.c_str(), value.c_str());
                        ::abort();
                    }
                } else if (key.size() > 0 && value.size() <= 0) {
                    ::fprintf(stderr, "the value of value(%s) can't be empty\n", key.c_str());
                    ::abort();
                }

                key.clear();
                value.clear();
                annotated = false;
                ps = &key;
                continue;
            } else if (!annotated && '=' == c) {
                ps = &value;
            } else if (!annotated) {
                ps->push_back(c);
            }
        }
    }

    bool Config::setParam(const std::string& key, const std::string& value) {
        try {
            if (key.compare("tcp_common_port") == 0) {
                tcp_common_port_ = std::stoi(value);
            } else if (key.compare("soap_port") == 0) {
                soap_port_ = std::stoi(value);
            } else if (key.compare("soap_xml_filepath") == 0) {
                soap_xml_filepath_ = std::move(value);
            } else if (key.compare("singleton_lock_file") == 0) {
                singleton_lock_file_ = std::move(value);
            } else if (key.compare("logfile_path") == 0) {
                log_file_path_ = std::move(value);
            } else if (key.compare("logfile_basename") == 0) {
                log_file_basename_ = std::move(value);
            } else if (key.compare("log_level") == 0) {
                log_level_ = getlevel(value);
            } else if (key.compare("timezone") == 0) {
                timezone_ = muduo::TimeZone(value.c_str());
                return timezone_.valid();
            } else if (key.compare("count_of_threads_for_communication") == 0) {
                count_of_threads_for_communication_ = static_cast<unsigned char>(std::stoi(value));
            } else if (key.compare("count_of_threads_for_business") == 0) {
                count_of_threads_for_business_ = static_cast<unsigned char>(std::stoi(value));
            } else if (key.compare("log_to_stdout") == 0) {
                log_to_stdout_ = value.compare("true") == 0;
            } else if (key.compare("log_to_file") == 0) {
                log_to_file_ = value.compare("true") == 0;
            } else if (key.compare("logfile_max_size") == 0) {
                logfile_max_size_ = static_cast<size_t>(std::stoul(value));
                logfile_max_size_ = (logfile_max_size_ == 0 ? 0 : (logfile_max_size_ << 20));
            } else if (key.compare("mysql_hostname") == 0) {
                mysql_hostname_ = std::move(value);
            } else if (key.compare("mysql_port") == 0) {
                mysql_port_ = std::move(value);
            } else if (key.compare("mysql_schema") == 0) {
                mysql_schema_ = std::move(value);
            } else if (key.compare("mysql_connection_pool_max") == 0) {
                mysql_connection_pool_max_ = static_cast<size_t>(std::stoul(value));
            } else if (key.compare("mysql_connection_pool_init") == 0) {
                mysql_connection_pool_init_ = static_cast<size_t>(std::stoul(value));
            } else if (key.compare("mysql_username") == 0) {
                mysql_username_ = std::move(value);
            }  else if (key.compare("mysql_password") == 0) {
                mysql_password_ = std::move(value);
            }
        } catch (std::invalid_argument& e) {
            return false;
        } catch (std::out_of_range& e) {
            return false;
        }

        return true;
    }

    muduo::Logger::LogLevel Config::getlevel(const std::string& level) {
        if (level.compare("info") == 0) {
            return muduo::Logger::INFO;
        } else if (level.compare("debug") == 0) {
            return muduo::Logger::DEBUG;
        } else if (level.compare("trace") == 0) {
            return muduo::Logger::TRACE;
        } else if (level.compare("warn") == 0) {
            return muduo::Logger::WARN;
        } else if (level.compare("error") == 0) {
            return muduo::Logger::ERROR;
        } else if (level.compare("fatal") == 0) {
            return muduo::Logger::FATAL;
        }
        return muduo::Logger::NUM_LOG_LEVELS;
    }

    void Config::ensureLogPathExist(const char *path) {
        DIR* dir = nullptr;
        if (nullptr == (dir = ::opendir(path))) {
            if (errno == ENOENT) {
                if (-1 == mkdir(path)) {
                    fprintf(stderr, "%s:%s:%d  --- failed to create director \"%s\": %s\n",
                            __FILE__, __func__, __LINE__, path, strerror(errno));
                    ::abort();
                }
            } else {
                fprintf(stderr, "%s:%s:%d  --- failed to open director \"%s\": %s\n",
                        __FILE__, __func__, __LINE__, path, strerror(errno));
                ::abort();
            }
        } else {
            ::closedir(dir);
        }
    }

    int Config::mkdir(const char *dir) {
        if (nullptr == dir) return -1;
        char tmp[1024] = {0};
        char *p = nullptr;

        ::snprintf(tmp, sizeof tmp, "%s", dir);
        size_t len = ::strlen(tmp);
        if (tmp[len - 1] == '/')
            tmp[len - 1] = 0;
        for (p = tmp + 1; *p; p++) {
            if (*p == '/') {
                *p = 0;
                if (0 != ::mkdir(tmp, S_IRWXU)) {
                    if (errno != EEXIST) return -1;
                }
                *p = '/';
            }
        }
        if (0 != ::mkdir(tmp, S_IRWXU)) {
            if (errno != EEXIST) return -1;
        }
        return 0;
    }
}
