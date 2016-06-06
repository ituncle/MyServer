#include "LogFile.h"

#include "FileUtil.h"
#include "ProcessInfo.h"

#include "TimeZone.h"

namespace muduo {
    TimeZone g_logTimeZone;
}

using namespace muduo;

LogFile::LogFile(const string& pathname,
                 const string& basename,
                 size_t rollSize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN)
  : pathname_(*(pathname.c_str() + pathname.length() - 1) == '/' ? pathname : pathname + string("/")),
    basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    checkEveryN_(checkEveryN),
    count_(0),
    mutex_(threadSafe ? new MutexLock() : nullptr),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)
{
  assert(basename.find('/') == string::npos);
//  auto pos = basename.find_last_of('/');
//  if (pos != string::npos) {
//    pathname_ = basename.substr(0, pos);
//    basename_ = basename.substr(pos + 1, basename.length() - pos - 1);
//
//  } else {
//    basename_ = basename;
//    pathname_ = "";
//  }

  rollFile();
}

LogFile::~LogFile()
{
}

void LogFile::append(const char* logline, int len)
{
  if (mutex_)
  {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
  }
  else
  {
    append_unlocked(logline, len);
  }
}

void LogFile::flush()
{
  if (mutex_)
  {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  }
  else
  {
    file_->flush();
  }
}

void LogFile::append_unlocked(const char* logline, int len)
{
  file_->append(logline, len);

  if (rollSize_ > 0 && file_->writtenBytes() > rollSize_)
  {
    rollFile();
  }
  else
  {
    ++count_;
    if (count_ >= checkEveryN_)
    {
      count_ = 0;
      time_t now = ::time(nullptr);
      time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
      if (thisPeriod_ != startOfPeriod_)
      {
        rollFile();
      }
      else if (now - lastFlush_ > flushInterval_)
      {
        lastFlush_ = now;
        file_->flush();
      }
    }
  }
}

bool LogFile::rollFile()
{
  time_t now = 0;
  string filename = getLogFileName(basename_, &now);
  time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

  if (now > lastRoll_)
  {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
    file_.reset(new FileUtil::AppendFile(pathname_ + filename));
    return true;
  }
  return false;
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
  string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  struct tm tm;

  int64_t microSecondsSinceEpoch = Timestamp::now().microSecondsSinceEpoch();
  *now = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
  tm = g_logTimeZone.toLocalTime(*now);
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);

//  *now = time(nullptr);
//  gmtime_r(now, &tm); // FIXME: localtime_r ?
//  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;

  filename += ProcessInfo::hostname();

  char pidbuf[32];
  snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
  filename += pidbuf;

  filename += ".log";

  return filename;
}

