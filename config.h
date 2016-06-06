//
// Created by root on 3/1/16.
//

#ifndef YFQSERVER_CONFIG_H
#define YFQSERVER_CONFIG_H

//#include <boost/noncopyable.hpp>
#include <string>
#include <memory>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>

#include "network/muduo/base/Logging.h"
#include "network/muduo/base/TimeZone.h"
#include "network/muduo/base/LogFile.h"

namespace ft {
    struct Config
    {
        static void applyConfigFile(const std::string &configfile = Config::config_file_pathname());
        static void initlog();

        static void set_config_file_pathname(const char *fullname) {
            config_file_pathname_ = fullname;
        }
        static const std::string &config_file_pathname() {
            return config_file_pathname_;
        }

        static void set_singleton_lock_file(const std::string& filepathname) {
            singleton_lock_file_ = filepathname;
        }
        static const std::string &singleton_lock_file() {
            return singleton_lock_file_;
        }

        static void set_tcp_common_port(int port) {
            tcp_common_port_ = port;
        }
        static int tcp_common_port() {
            return tcp_common_port_;
        }

        static void set_soap_port(int port) {
            soap_port_ = port;
        }
        static int soap_port() {
            return soap_port_;
        }

        static void set_soap_xml_filepath(const std::string& xmlpath) {
            soap_xml_filepath_ = xmlpath;
        }
        static const std::string& soap_xml_filepath() {
            return soap_xml_filepath_;
        }

        static const std::string& log_file_path() {
            return log_file_path_;
        }
        static void set_log_file_path(const std::string & path) {
            log_file_path_ = path;
        }

        static void set_log_file_basename(const std::string & basename) {
            log_file_basename_ = basename;
        }
        static const std::string &log_file_basename() {
            return log_file_basename_;
        }

        static void set_log_level(muduo::Logger::LogLevel level) {
            log_level_ = level;
        }
        static muduo::Logger::LogLevel log_level() {
            return log_level_;
        }

        static void set_timezone(const muduo::TimeZone & timezone) {
            timezone_ = timezone;
        }
        static const muduo::TimeZone &timezone() {
            return timezone_;
        }

        static void set_count_of_threads_for_communication(unsigned char threadsnum) {
            count_of_threads_for_communication_ = threadsnum;
        }
        static unsigned char count_of_threads_for_communication() {
            return count_of_threads_for_communication_;
        }

        static void set_count_of_threads_for_business(unsigned char threadsnum) {
            count_of_threads_for_business_ = threadsnum;
        }
        static unsigned char count_of_threads_for_business() {
            return count_of_threads_for_business_;
        }

        static void set_log_to_stdout(bool flag) {
            log_to_stdout_ = flag;
        }
        static bool log_to_stdout() {
            return log_to_stdout_ & !isDaemon_; // it's unnecessary for a daemon process to log to stdout
        }

        static void set_log_to_file(bool flag) {
            log_to_file_ = flag;
        }
        static bool log_to_file() {
            return log_to_file_;
        }

        static void set_logfile_max_size(size_t maxsize) {
            logfile_max_size_ = maxsize;
        }
        static size_t logfile_max_size() {
            return logfile_max_size_;
        }

        static void set_mysql_hostname(const std::string& host) {
            mysql_hostname_ = host;
        }
        static const std::string& mysql_hostname() {
            return mysql_hostname_;
        }

        static void set_mysql_port(const std::string& port) {
            mysql_port_ = port;
        }
        static const std::string& mysql_port() {
            return mysql_port_;
        }

        static void set_mysql_schema(const std::string& schema) {
            mysql_schema_ = schema;
        }
        static const std::string& mysql_schema() {
            return mysql_schema_;
        }

        static void mysql_connection_pool_max(size_t max_connections) {
            mysql_connection_pool_max_ = max_connections;
        }
        static size_t mysql_connection_pool_max() {
            return mysql_connection_pool_max_;
        }

        static void mysql_connection_pool_init(size_t init_connections) {
            mysql_connection_pool_init_ = init_connections;
        }
        static size_t mysql_connection_pool_init() {
            return mysql_connection_pool_init_;
        }

        static void set_mysql_username(const std::string& username) {
            mysql_username_ = username;
        }
        static const std::string& mysql_username() {
            return mysql_username_;
        }

        static void set_mysql_password(const std::string& pwd) {
            mysql_password_ = pwd;
        }
        static const std::string& mysql_password() {
            return mysql_password_;
        }

        static void set_daemon(bool flag) {
            isDaemon_ = flag;
        }
        static bool isDaemon() {
            return isDaemon_ ;
        }

    private:
        static void readFromConfigFile(const char *configfile = Config::config_file_pathname().c_str());

        static void handfile(const char *buf, ssize_t len);

        static bool setParam(const std::string &key, const std::string &value);

        static muduo::Logger::LogLevel getlevel(const std::string &level);

        static int mkdir(const char *dir);
        static void ensureLogPathExist(const char *dir);

    private:
        /*
         * 配置文件的全路径名
         */
        static std::string config_file_pathname_;
        /*
         * 锁文件，用于确保本进程单一实例
         */
        static std::string singleton_lock_file_;
        /*
         * TCP监听的普通端口，用于命令和短小数据传输
         */
        static int tcp_common_port_;
        /*
         * soap监听的端口
         */
        static int soap_port_;
        /*
         * soap的xml文件的全路径
         */
        static std::string soap_xml_filepath_;
        /*
         * 日志文件的存放路径，必须是绝对路径
         */
        static std::string log_file_path_;
        /*
         * 日志文件的basename
         */
        static std::string log_file_basename_;
        /*
         * 日志的输出级别
         */
        static muduo::Logger::LogLevel log_level_;
        /*
         * 本程序使用的时区文件
         */
        static muduo::TimeZone timezone_;
        /*
         * 通信部分使用的线程数
         */
        static unsigned char count_of_threads_for_communication_;
        /*
         * 业务部分使用的线程数
         */
        static unsigned char count_of_threads_for_business_;
        /*
         * whether log to stdout
         */
        static bool log_to_stdout_;
        /*
         * whether log to file
         */
        static bool log_to_file_;
        /*
         * the max size of per logfile, will switch a new
         */
        static size_t logfile_max_size_;
        /*
         * mysql服务器主机地址
         */
        static std::string mysql_hostname_;
        /*
         * mysql服务端端口
         */
        static std::string mysql_port_;
        /*
         * the mysql schema we use
         */
        static std::string mysql_schema_;
        /*
         * mysql连接池最大连接数
         */
        static size_t mysql_connection_pool_max_;
        /*
         * mysql连接池初始连接数
         */
        static size_t mysql_connection_pool_init_;
        /*
         * mysql用户名
         */
        static std::string mysql_username_;
        /*
         * mysql密码
         */
        static std::string mysql_password_;
        /*
         * file for log
         */
        static std::shared_ptr<muduo::LogFile> logfile;
        /*
         * the process is daemon or not
         */
        static bool isDaemon_;
    };
}
#endif //YFQSERVER_CONFIG_H
