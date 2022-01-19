// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_SCOPED_CLEAR_LAST_ERROR_H_
#define MINI_CHROMIUM_BASE_SCOPED_CLEAR_LAST_ERROR_H_

#include <errno.h>

#include "build/build_config.h"
#include "build/buildflag.h"

namespace base {

class ScopedClearLastErrorBase {
 public:
  ScopedClearLastErrorBase() : last_errno_(errno) { errno = 0; }

  ScopedClearLastErrorBase(const ScopedClearLastErrorBase&) = delete;
  ScopedClearLastErrorBase& operator=(const ScopedClearLastErrorBase&) = delete;

  ~ScopedClearLastErrorBase() { errno = last_errno_; }

 private:
  const int last_errno_;
};

#if BUILDFLAG(IS_WIN)

class ScopedClearLastError : public ScopedClearLastErrorBase {
 public:
  ScopedClearLastError();

  ScopedClearLastError(const ScopedClearLastError&) = delete;
  ScopedClearLastError& operator=(const ScopedClearLastError&) = delete;

  ~ScopedClearLastError();

 private:
  const unsigned long last_system_error_;
};

#elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)

using ScopedClearLastError = ScopedClearLastErrorBase;

#endif  // BUILDFLAG(IS_WIN)

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_SCOPED_CLEAR_LAST_ERROR_H_
