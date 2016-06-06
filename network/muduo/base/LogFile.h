#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include "Mutex.h"
#include "Types.h"

#include <boost/noncopyable.hpp>
#include <memory>

namespace muduo {

    namespace FileUtil {
        class AppendFile;
    }

    class LogFile : boost::noncopyable {
    public:
        LogFile(const string& pathname,
                const string &basename,
                size_t rollSize,
                bool threadSafe = true,
                int flushInterval = 3,
                int checkEveryN = 1);

        ~LogFile();

        void append(const char *logline, int len);

        void flush();

    private:
        bool rollFile();

        void append_unlocked(const char *logline, int len);

        static string getLogFileName(const string &basename, time_t *now);

        const string pathname_; // the pathname of log file
        const string basename_; // the basename of log file
        const size_t rollSize_;
        const int flushInterval_;
        const int checkEveryN_;

        int count_;

        std::unique_ptr<MutexLock> mutex_;
        time_t startOfPeriod_;
        time_t lastRoll_;
        time_t lastFlush_;
        std::unique_ptr<FileUtil::AppendFile> file_;

        const static int kRollPerSeconds_ = 60 * 60 * 24;
    };

}
#endif  // MUDUO_BASE_LOGFILE_H
