// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_FILES_FILE_UTIL_H_
#define MINI_CHROMIUM_BASE_FILES_FILE_UTIL_H_

#include "build/build_config.h"
#include "build/buildflag.h"

#if BUILDFLAG(IS_POSIX)

#include <sys/types.h>

namespace base {

bool ReadFromFD(int fd, char* buffer, size_t bytes);

}  // namespace base

#endif  // BUILDFLAG(IS_POSIX)

#endif  // MINI_CHROMIUM_BASE_FILES_FILE_UTIL_H_
