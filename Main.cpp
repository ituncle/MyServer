#include <memory>

#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

#include "log/ftlog.h"
#include "protocol/FtTcpServer.h"
#include "network/muduo/base/TimeZone.h"
#include "utils/Set.h"
#include "utils/Signal.h"
#include "network/muduo/base/ThreadPool.h"
#include "network/muduo/base/LogFile.h"
#include "config.h"
#include "dataAccess/db/connectionPool.h"
#include "protocol/yfq/yfqParser.h"
#include "protocol/yfq/yfqHandler.h"
#include "protocol/echo/EchoParser.h"
#include "protocol/echo/EchoHandler.h"
#include "protocol/echo/EchoRequest.h"
#include "projectinfo.h"


void *soapthreadpro(void *pArg);


class MainWork {
public:
  static MainWork* instance() {
    static MainWork mainWork;
    return &mainWork;
  }

  ~MainWork() {
    LogInfo << "MainWork::~MainWork()";

    delete echoServer_;
    echoServer_ = nullptr;

    delete threadPool_;
    threadPool_ = nullptr;

    delete loop_;
    loop_ = nullptr;
  }

  void stop() {
    loop_->quit();
  }

// initialize the process
  void init() {
    // handle signal Ctrl-c + SIGPIPE
    ft::signal(SIGINT, [](int signo) {
        puts("process is killed by ctrl-c");
        instance()->stop();
    });
    // handle signal Ctrl-c + SIGPIPE
    ft::signal(SIGTERM, [](int signo) {
        puts("process has to die due to 'SIGTERM'");
        instance()->stop();
    });
    // init db connection pool
    if (!ft::mysql::ConnectionPool::instance()->init(ft::Config::mysql_hostname(),
                                                     ft::Config::mysql_port(),
                                                     ft::Config::mysql_schema(),
                                                     ft::Config::mysql_username(),
                                                     ft::Config::mysql_password(),
                                                     ft::Config::mysql_connection_pool_max(),
                                                     ft::Config::mysql_connection_pool_init())) {
      LogFatal << "failed to init db connection pool";
    } else {
      LogInfo << "successed to open db connection pool";
    }
  }

  void start() {
    startTcpServer();
  }

private:
  MainWork()
  : loop_(new muduo::net::EventLoop()),
    threadPool_(new muduo::ThreadPool("wokers")) {
  }

  void startTcpServer() {
    const int threadsnumOfBussiness = ft::Config::count_of_threads_for_business();
    threadPool_->start(threadsnumOfBussiness);

//    // start tcp server for yifengqing
//    yfqServer_ = new ft::TcpServer<ft::proto::Request>(loop_,
//                     muduo::net::InetAddress(static_cast<uint16_t>(ft::Config::tcp_common_port())),
//                     "yifengqing",
//                     std::make_shared<ft::proto::YfqParser>(),
//                     std::make_shared<ft::proto::YfqHandler>(threadPool_, threadsnumOfBussiness));
//    yfqServer_->start();

    // start tcp server for echo
    uint16_t port = 9999;//static_cast<uint16_t>(ft::Config::tcp_common_port());
    echoServer_ = new ft::TcpServer<ft::proto::EchoRequest>(loop_,
                     muduo::net::InetAddress(port, false, false),
                     "echo",
                     std::make_shared<ft::proto::EchoParser>(),
                     std::make_shared<ft::proto::EchoHandler>(threadPool_, threadsnumOfBussiness));
    echoServer_->start();

    loop_->loop();
  }

private:
  int soapPort_ = ft::Config::soap_port();
  muduo::net::EventLoop *loop_;
  muduo::ThreadPool *threadPool_;    // handle requests received from client
//  ft::TcpServer<ft::proto::Request>* yfqServer_;       // tcp server for yifengqing
  ft::TcpServer<ft::proto::EchoRequest> *echoServer_;       // tcp server for echo
};


// handle arguments from cmd
void ParseArgs(int argc, char **argv) {
  bool flags = true;
  char opt;

  const char *help = "[-v(version)] [-h(help)] [-d isDaemon(0/1)] [-c full_pathname_of_configfile] "
  "[-l full_pathname_of_lockfile] [-L path_of_logfile] [-b basename_of_logfile]";

  while (flags && (opt = getopt(argc, argv, "hvd:c:l:L:b:")) != -1) {
    switch (opt) {
      case 'v':   // show version number
      case 'h':   // show usage
        fprintf(stdout, "%s:%d.%d.%d\n", argv[0], PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
        if (opt == 'h') fprintf(stderr, "usage: %s %s\n", argv[0], help);
        ::exit(EXIT_SUCCESS);
      case 'd':      // is daemon or not
        try {
          opt = std::stoi(optarg);
          printf("-d %d\n", opt);
          if (opt == 0)
            ft::Config::set_daemon(false);
          else if (opt == 1)
            ft::Config::set_daemon(true);
          else
            flags = false;
        } catch (std::invalid_argument &e) {
          flags = false;
        } catch (std::out_of_range &e) {
          flags = false;
        }
        break;
      case 'c':   // specify the full pathname of config file
//                printf("-c %s\n", optarg);
        ft::Config::set_config_file_pathname(optarg);
        break;
      case 'l':   //  specify the full pathname of lock file which ensure this process is singleton
//                printf("-l %s\n", optarg);
        ft::Config::set_singleton_lock_file(optarg);
        break;
      case 'L':   // specify the path where log files should live
//                printf("-L %s\n", optarg);
        ft::Config::set_log_file_path(optarg);
        break;
      case 'b':
//                printf("-b %s\n", optarg);
        ft::Config::set_log_file_basename(optarg);
        break;
      default: /* '?' */
        flags = false;
        break;
    }
  }

//    fprintf(stderr, "optind = %d\n", optind);

  if (!flags || optind > argc) {
    fprintf(stderr, "usage: %s %s\n", argv[0], help);
    ::exit(EXIT_FAILURE);
  }
}

// to guarantee this program is single on the system
bool singleton(const char *lock_file) {
  int fd = open(lock_file, O_WRONLY | O_CREAT);
  if (fd == -1) {
    fprintf(stderr, "failed to open/create lock_file(%s): %s\n", lock_file, strerror(errno));
  }

  if (0 != flock(fd, LOCK_EX | LOCK_NB)) {
    fprintf(stderr, "failed to lock lock_file(%s): %s\n", lock_file, strerror(errno));
    close(fd);
    return false;
  }
  return true;
}

//ft::mysql::DbHandler* myDbHandler = nullptr;

int main(int argc, char **argv) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ParseArgs(argc, argv);

  // read configuration file && apply it
  ft::Config::applyConfigFile();


//  // make sure this process is singleton on the entire system
//  if (!singleton(ft::Config::singleton_lock_file().c_str())) {
//    fprintf(stderr, "%s is already state right now!\n", argv[0]);
//    ::abort();
//  }

  // make this process to be a daemon process
  if (ft::Config::isDaemon()) {
    if (0 != daemon(0, 0)) {
      fprintf(stderr, "failed to call daemon(): %s\n", strerror(errno));
      ::abort();
    }
  }

  // init the action of logging
  ft::Config::initlog();

  MainWork::instance()->init();
//  {
//    ft::mysql::ConnectionGuard guard(ft::mysql::ConnectionPool::instance());
//    std::shared_ptr<ft::mysql::Connection>& conn = guard.connection();
//    const char* sql = "SELECT name, age, score FROM student ORDER BY score";
//    std::string error;
//    sql::ResultSet* rs = conn->executeQuery(sql, error);
//    if (nullptr == rs) {
//      LogError << "Failed to execute sql: \"" << sql << "\", error: " << error;
//    }
//    LogInfo << "name\tage\tscore";
//    while (rs->next()) {
//      LogInfo << rs->getString("name") << "\t" << rs->getInt("age") << "\t" << rs->getInt("score");
//    }
//  }

  MainWork::instance()->start();

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
