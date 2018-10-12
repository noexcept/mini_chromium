// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_metrics.h"

#include <windows.h>  // Must be in front of other Windows header files.

namespace base {
namespace {

// System pagesize. This value remains constant on x86/64 architectures.
const int PAGESIZE_KB = 4;

}  // namespace

size_t GetPageSize() {
  return PAGESIZE_KB * 1024;
}

}  // namespace base
