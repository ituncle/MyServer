/*
 * ftlog.h
 *
 *  Created on: Feb 19, 2016
 *      Author: root
 */

#ifndef LOG_FTLOG_H_
#define LOG_FTLOG_H_

#include "../network/muduo/base/Logging.h"


// Mainly useful to represent current progress of application.
#define LogInfo       MUDUO_LOG_INFO

// Only applicable if NDEBUG is not defined (for non-VC++) or _DEBUG is defined (for VC++).
#define LogDebug      MUDUO_LOG_DEBUG

// Information that can be useful to back-trace certain events - mostly useful than debug logs.
#define LogTrace      MUDUO_LOG_TRACE

// Information representing errors in application but application will keep running.
#define LogWarning    MUDUO_LOG_WARN
#define LogWarn       MUDUO_LOG_WARN

// Error information but will continue application to keep running.
#define LogError      MUDUO_LOG_ERROR

// Very severe error event that will presumably lead the application to abort.
#define LogFatal      MUDUO_LOG_FATAL

#define LogSysErr     MUDUO_LOG_SYSERR
#define LogSysFatal   MUDUO_LOG_SYSFATAL

#endif /* LOG_FTLOG_H_ */
