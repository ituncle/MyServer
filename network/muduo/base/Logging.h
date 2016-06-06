#ifndef MUDUO_BASE_LOGGING_H
#define MUDUO_BASE_LOGGING_H

#include <memory>
#include <iostream>
#include "LogStream.h"
#include "Timestamp.h"

namespace muduo {

class TimeZone;

class Logger {
public:
  enum LogLevel {
    INFO,
    DEBUG,
    TRACE,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  // compile time calculation of basename of source file
  class SourceFile {
  public:
    template <size_t N>
    inline SourceFile(const char (&arr)[N])
    : data_(arr),
      size_(N - 1) {
      const char *slash = strrchr(data_, '/'); // builtin function
      if (slash) {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

    explicit SourceFile(const char *filename)
    : data_(filename) {
      const char *slash = strrchr(filename, '/');
      if (slash) {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

    const char *data_;
    size_t size_;
  };

  Logger(SourceFile file, int line);

  Logger(SourceFile file, int line, LogLevel level);

  Logger(SourceFile file, int line, LogLevel level, const char *func);

  Logger(SourceFile file, int line, bool toAbort);

  ~Logger();

  LogStream &stream() { return impl_.stream_; }

  static LogLevel logLevel();

  static void setLogLevel(LogLevel level);

//        typedef void (*OutputFunc)(const char *msg, int len);
  using OutputFunc = std::function<void(const char *msg, int len)>;

//        typedef void (*FlushFunc)();
  using FlushFunc = std::function<void(void)>;

  static void setOutput(OutputFunc);

  static void setFlush(FlushFunc);

  static void setTimeZone(const TimeZone &tz);

private:

  class Impl {
  public:
    typedef Logger::LogLevel LogLevel;

    Impl(LogLevel level, int old_errno, const SourceFile &file, int line);

    void formatTime();

    void finish();

    Timestamp time_;
    LogStream stream_;
    LogLevel level_;
    int line_;
    SourceFile basename_;
  };

  Impl impl_;

};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel() {
  return g_logLevel;
}

//
// CAUTION: do not write:
//
// if (good)
//   MUDUO_LOG_INFO << "Good news";
// else
//   MUDUO_LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//
#define MUDUO_LOG_TRACE if (muduo::Logger::logLevel() <= muduo::Logger::TRACE) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::TRACE, __func__).stream()
#define MUDUO_LOG_DEBUG if (muduo::Logger::logLevel() <= muduo::Logger::DEBUG) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::DEBUG, __func__).stream()
#define MUDUO_LOG_INFO if (muduo::Logger::logLevel() <= muduo::Logger::INFO) \
  muduo::Logger(__FILE__, __LINE__, muduo::Logger::INFO, __func__).stream()
#define MUDUO_LOG_WARN muduo::Logger(__FILE__, __LINE__, muduo::Logger::WARN, __func__).stream()
#define MUDUO_LOG_ERROR muduo::Logger(__FILE__, __LINE__, muduo::Logger::ERROR, __func__).stream()
#define MUDUO_LOG_FATAL muduo::Logger(__FILE__, __LINE__, muduo::Logger::FATAL, __func__).stream()
#define MUDUO_LOG_SYSERR muduo::Logger(__FILE__, __LINE__, false).stream()
#define MUDUO_LOG_SYSFATAL muduo::Logger(__FILE__, __LINE__, true).stream()

const char *strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non nullptr.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
  ::muduo::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non nullptr", (val))

// A small helper for CHECK_NOTNULL().
template<typename T>
T *CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr) {
  if (ptr == nullptr) {
    Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}


extern std::string printBytes(const char *buf, int offset, int count, int bytesPerLine = 10);

}

#endif  // MUDUO_BASE_LOGGING_H
