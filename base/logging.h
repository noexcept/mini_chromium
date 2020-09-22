// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_LOGGING_H_
#define MINI_CHROMIUM_BASE_LOGGING_H_

#include <assert.h>
#include <errno.h>

#include <limits>
#include <sstream>
#include <string>

#include "base/check.h"
#include "base/check_op.h"
#include "base/macros.h"
#include "base/notreached.h"
#include "build/build_config.h"

namespace logging {

#if defined(OS_WIN)
typedef wchar_t PathChar;
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
typedef char PathChar;
#endif

// A bitmask of potential logging destinations.
using LoggingDestination = uint32_t;

// Specifies where logs will be written. Multiple destinations can be specified
// with bitwise OR.
// Unless destination is LOG_NONE, all logs with severity ERROR and above will
// be written to stderr in addition to the specified destination.
enum : uint32_t {
  LOG_NONE = 0,
  LOG_TO_FILE = 1 << 0,
  LOG_TO_SYSTEM_DEBUG_LOG = 1 << 1,
  LOG_TO_STDERR = 1 << 2,

  LOG_TO_ALL = LOG_TO_FILE | LOG_TO_SYSTEM_DEBUG_LOG | LOG_TO_STDERR,

// On Windows, use a file next to the exe.
// On POSIX platforms, where it may not even be possible to locate the
// executable on disk, use stderr.
// On Fuchsia, use the Fuchsia logging service.
#if defined(OS_FUCHSIA)
  LOG_DEFAULT = LOG_TO_SYSTEM_DEBUG_LOG,
#elif defined(OS_WIN)
  LOG_DEFAULT = LOG_TO_FILE,
#elif defined(OS_POSIX)
  LOG_DEFAULT = LOG_TO_SYSTEM_DEBUG_LOG | LOG_TO_STDERR,
#endif
};

// Indicates that the log file should be locked when being written to.
// Unless there is only one single-threaded process that is logging to
// the log file, the file should be locked during writes to make each
// log output atomic. Other writers will block.
//
// All processes writing to the log file must have their locking set for it to
// work properly. Defaults to LOCK_LOG_FILE.
enum LogLockingState { LOCK_LOG_FILE, DONT_LOCK_LOG_FILE };

// On startup, should we delete or append to an existing log file (if any)?
// Defaults to APPEND_TO_OLD_LOG_FILE.
enum OldFileDeletionState { DELETE_OLD_LOG_FILE, APPEND_TO_OLD_LOG_FILE };

#if defined(OS_CHROMEOS)
// Defines the log message prefix format to use.
// LOG_FORMAT_SYSLOG indicates syslog-like message prefixes.
// LOG_FORMAT_CHROME indicates the normal Chrome format.
enum class LogFormat { LOG_FORMAT_CHROME, LOG_FORMAT_SYSLOG };
#endif

struct LoggingSettings {
  // Equivalent to logging destination enum, but allows for multiple
  // destinations.
  uint32_t logging_dest = LOG_DEFAULT;

  // The four settings below have an effect only when LOG_TO_FILE is
  // set in |logging_dest|.
  const PathChar* log_file_path = nullptr;
  LogLockingState lock_log = LOCK_LOG_FILE;
  OldFileDeletionState delete_old = APPEND_TO_OLD_LOG_FILE;
#if defined(OS_CHROMEOS)
  // Contains an optional file that logs should be written to. If present,
  // |log_file_path| will be ignored, and the logging system will take ownership
  // of the FILE. If there's an error writing to this file, no fallback paths
  // will be opened.
  FILE* log_file = nullptr;
  // ChromeOS uses the syslog log format by default.
  LogFormat log_format = LogFormat::LOG_FORMAT_SYSLOG;
#endif
};

// Sets the log file name and other global logging state. Calling this function
// is recommended, and is normally done at the beginning of application init.
// If you don't call it, all the flags will be initialized to their default
// values, and there is a race condition that may leak a critical section
// object if two threads try to do the first log at the same time.
// See the definition of the enums above for descriptions and default values.
//
// The default log file is initialized to "debug.log" in the application
// directory. You probably don't want this, especially since the program
// directory may not be writable on an enduser's system.
//
// This function may be called a second time to re-direct logging (e.g after
// loging in to a user partition), however it should never be called more than
// twice.
bool InitLogging(const LoggingSettings& settings);

typedef int LogSeverity;
const LogSeverity LOG_VERBOSE = -1;
const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;
const LogSeverity LOG_ERROR_REPORT = 3;
const LogSeverity LOG_FATAL = 4;
const LogSeverity LOG_NUM_SEVERITIES = 5;

#if defined(NDEBUG)
const LogSeverity LOG_DFATAL = LOG_ERROR;
#else
const LogSeverity LOG_DFATAL = LOG_FATAL;
#endif

typedef bool (*LogMessageHandlerFunction)(LogSeverity severity,
                                          const char* file_poath,
                                          int line,
                                          size_t message_start,
                                          const std::string& string);

void SetLogMessageHandler(LogMessageHandlerFunction log_message_handler);
LogMessageHandlerFunction GetLogMessageHandler();

static inline int GetMinLogLevel() {
  return LOG_INFO;
}

static inline int GetVlogLevel(const char*) {
  return std::numeric_limits<int>::max();
}

#if defined(OS_WIN)
// This is just ::GetLastError, but out-of-line to avoid including windows.h in
// such a widely used place.
unsigned long GetLastSystemErrorCode();
std::string SystemErrorCodeToString(unsigned long error_code);
#elif defined(OS_POSIX)
static inline int GetLastSystemErrorCode() {
  return errno;
}
#endif

class LogMessage {
 public:
  LogMessage(const char* function,
             const char* file_path,
             int line,
             LogSeverity severity);
  LogMessage(const char* function,
             const char* file_path,
             int line,
             std::string* result);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

 private:
  void Init(const char* function);

  std::ostringstream stream_;
  const char* file_path_;
  size_t message_start_;
  const int line_;
  LogSeverity severity_;

  DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

class LogMessageVoidify {
 public:
  LogMessageVoidify() {}

  void operator&(const std::ostream&) const {}
};

#if defined(OS_WIN)
class Win32ErrorLogMessage : public LogMessage {
 public:
  Win32ErrorLogMessage(const char* function,
                       const char* file_path,
                       int line,
                       LogSeverity severity,
                       unsigned long err);
  ~Win32ErrorLogMessage();

 private:
  unsigned long err_;

  DISALLOW_COPY_AND_ASSIGN(Win32ErrorLogMessage);
};
#elif defined(OS_POSIX)
class ErrnoLogMessage : public LogMessage {
 public:
  ErrnoLogMessage(const char* function,
                  const char* file_path,
                  int line,
                  LogSeverity severity,
                  int err);
  ~ErrnoLogMessage();

 private:
  int err_;

  DISALLOW_COPY_AND_ASSIGN(ErrnoLogMessage);
};
#endif

}  // namespace logging

#if defined(COMPILER_MSVC)
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif

#define COMPACT_GOOGLE_LOG_EX_INFO(ClassName, ...) \
    logging::ClassName(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                       logging::LOG_INFO, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_WARNING(ClassName, ...) \
    logging::ClassName(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                       logging::LOG_WARNING, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_ERROR(ClassName, ...) \
    logging::ClassName(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                       logging::LOG_ERROR, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_ERROR_REPORT(ClassName, ...) \
    logging::ClassName(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                       logging::LOG_ERROR_REPORT, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_FATAL(ClassName, ...) \
    logging::ClassName(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                       logging::LOG_FATAL, ## __VA_ARGS__)
#define COMPACT_GOOGLE_LOG_EX_DFATAL(ClassName, ...) \
    logging::ClassName(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                       logging::LOG_DFATAL, ## __VA_ARGS__)

#define COMPACT_GOOGLE_LOG_INFO \
    COMPACT_GOOGLE_LOG_EX_INFO(LogMessage)
#define COMPACT_GOOGLE_LOG_WARNING \
    COMPACT_GOOGLE_LOG_EX_WARNING(LogMessage)
#define COMPACT_GOOGLE_LOG_ERROR \
    COMPACT_GOOGLE_LOG_EX_ERROR(LogMessage)
#define COMPACT_GOOGLE_LOG_ERROR_REPORT \
    COMPACT_GOOGLE_LOG_EX_ERROR_REPORT(LogMessage)
#define COMPACT_GOOGLE_LOG_FATAL \
    COMPACT_GOOGLE_LOG_EX_FATAL(LogMessage)
#define COMPACT_GOOGLE_LOG_DFATAL \
    COMPACT_GOOGLE_LOG_EX_DFATAL(LogMessage)

#if defined(OS_WIN)

// wingdi.h defines ERROR 0. We don't want to include windows.h here, and we
// want to allow "LOG(ERROR)", which will expand to LOG_0.

// This will not cause a warning if the RHS text is identical to that in
// wingdi.h (which it is).
#define ERROR 0

#define COMPACT_GOOGLE_LOG_EX_0(ClassName, ...) \
  COMPACT_GOOGLE_LOG_EX_ERROR(ClassName , ##__VA_ARGS__)
#define COMPACT_GOOGLE_LOG_0 COMPACT_GOOGLE_LOG_ERROR
namespace logging {
const LogSeverity LOG_0 = LOG_ERROR;
}  // namespace logging

#endif  // OS_WIN

#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void) 0 : ::logging::LogMessageVoidify() & (stream)

#define LOG_IS_ON(severity) \
    ((::logging::LOG_ ## severity) >= ::logging::GetMinLogLevel())
#define VLOG_IS_ON(verbose_level) \
    ((verbose_level) <= ::logging::GetVlogLevel(__FILE__))

#define LOG_STREAM(severity) COMPACT_GOOGLE_LOG_ ## severity.stream()
#define VLOG_STREAM(verbose_level) \
    logging::LogMessage(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                        -verbose_level).stream()

#if defined(OS_WIN)
#define PLOG_STREAM(severity) COMPACT_GOOGLE_LOG_EX_ ## severity( \
    Win32ErrorLogMessage, ::logging::GetLastSystemErrorCode()).stream()
#define VPLOG_STREAM(verbose_level)                                       \
    logging::Win32ErrorLogMessage(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                                  -verbose_level,                         \
                                  ::logging::GetLastSystemErrorCode()).stream()
#elif defined(OS_POSIX)
#define PLOG_STREAM(severity) COMPACT_GOOGLE_LOG_EX_ ## severity( \
    ErrnoLogMessage, ::logging::GetLastSystemErrorCode()).stream()
#define VPLOG_STREAM(verbose_level) \
    logging::ErrnoLogMessage(FUNCTION_SIGNATURE, __FILE__, __LINE__, \
                             -verbose_level, \
                             ::logging::GetLastSystemErrorCode()).stream()
#endif

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity) && (condition))
#define LOG_ASSERT(condition) \
    LOG_IF(FATAL, !(condition)) << "Assertion failed: " # condition ". "

#define VLOG(verbose_level) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), VLOG_IS_ON(verbose_level))
#define VLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), \
                VLOG_IS_ON(verbose_level) && (condition))

#define PLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity))
#define PLOG_IF(severity, condition) \
    LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

#define VPLOG(verbose_level) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), VLOG_IS_ON(verbose_level))
#define VPLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), \
                VLOG_IS_ON(verbose_level) && (condition))

#if defined(NDEBUG)
#define DLOG_IS_ON(severity) 0
#define DVLOG_IS_ON(verbose_level) 0
#define DCHECK_IS_ON() 0
#else
#define DLOG_IS_ON(severity) LOG_IS_ON(severity)
#define DVLOG_IS_ON(verbose_level) VLOG_IS_ON(verbose_level)
#define DCHECK_IS_ON() 1
#endif

#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))
#define DLOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity) && (condition))
#define DLOG_ASSERT(condition) \
    DLOG_IF(FATAL, !(condition)) << "Assertion failed: " # condition ". "

#define DVLOG(verbose_level) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), DVLOG_IS_ON(verbose_level))
#define DVLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VLOG_STREAM(verbose_level), \
                DVLOG_IS_ON(verbose_level) && (condition))

#define DPLOG(severity) LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity))
#define DPLOG_IF(severity, condition) \
    LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity) && (condition))

#define DVPLOG(verbose_level) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), DVLOG_IS_ON(verbose_level))
#define DVPLOG_IF(verbose_level, condition) \
    LAZY_STREAM(VPLOG_STREAM(verbose_level), \
                DVLOG_IS_ON(verbose_level) && (condition))

#undef assert
#define assert(condition) DLOG_ASSERT(condition)

#endif  // MINI_CHROMIUM_BASE_LOGGING_H_
