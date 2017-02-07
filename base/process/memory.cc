// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/alias.h"
#include "base/logging.h"
#include "base/process/memory.h"
#include "build/build_config.h"

namespace base {

bool UncheckedMalloc(size_t size, void** result) {
  *result = malloc(size);
  return *result != NULL;
}

bool UncheckedCalloc(size_t num_items, size_t size, void** result) {
  const size_t alloc_size = num_items * size;

  // Overflow check
  if (size && ((alloc_size / size) != num_items)) {
    *result = NULL;
    return false;
  }

  if (!UncheckedMalloc(alloc_size, result))
    return false;

  memset(*result, 0, alloc_size);
  return true;
}

}  // namespace base
