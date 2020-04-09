// Copyright (c) 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_SCOPED_CLEAR_LAST_ERROR_H_
#define MINI_CHROMIUM_BASE_SCOPED_CLEAR_LAST_ERROR_H_

#include <errno.h>

#include "base/macros.h"
#include "build/build_config.h"

namespace base {

// ScopedClearLastError stores and resets the value of thread local error codes
// (errno, GetLastError()), and restores them in the destructor. This is useful
// to avoid side effects on these values in instrumentation functions that
// interact with the OS.

// Common implementation of ScopedClearLastError for all platforms. Use
// ScopedClearLastError instead.
class  ScopedClearLastErrorBase {
 public:
  ScopedClearLastErrorBase() : last_errno_(errno) { errno = 0; }
  ~ScopedClearLastErrorBase() { errno = last_errno_; }

 private:
  const int last_errno_;

  DISALLOW_COPY_AND_ASSIGN(ScopedClearLastErrorBase);
};

#if defined(OS_WIN)

// Windows specific implementation of ScopedClearLastError.
class ScopedClearLastError : public ScopedClearLastErrorBase {
 public:
  ScopedClearLastError();
  ~ScopedClearLastError();

 private:
  unsigned int last_system_error_;

  DISALLOW_COPY_AND_ASSIGN(ScopedClearLastError);
};

#elif defined(OS_POSIX) || defined(OS_FUCHSIA)

using ScopedClearLastError = ScopedClearLastErrorBase;

#endif  // defined(OS_WIN)

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_SCOPED_CLEAR_LAST_ERROR_H_
