// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains routines for gathering resource statistics for processes
// running on the system.

#ifndef BASE_PROCESS_PROCESS_METRICS_H_
#define BASE_PROCESS_PROCESS_METRICS_H_

#include <stddef.h>

namespace base {

// Returns the number of bytes in a memory page. Do not use this to compute
// the number of pages in a block of memory for calling mincore(). On some
// platforms, e.g. iOS, mincore() uses a different page size from what is
// returned by GetPageSize().
size_t GetPageSize();

}  // namespace base

#endif  // BASE_PROCESS_PROCESS_METRICS_H_
