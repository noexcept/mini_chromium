// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_NOTREACHED_H_
#define MINI_CHROMIUM_BASE_NOTREACHED_H_

#include "base/check.h"

// TODO(crbug.com/40580068): Redefine NOTREACHED() to be the [[noreturn]] once
// crashpad and chromium has migrated off of the non-noreturn version. This is
// easiest done by defining it as std::abort() as crashpad currently doesn't
// stream arguments to it. For a more complete implementation we should
// LOG(FATAL) but that doesn't work until mini_chromium's LOG(FATAL) is properly
// understood as [[noreturn]].
#define NOTREACHED() DCHECK(false)

// TODO(crbug.com/40580068): Remove this once the NotReachedIsFatal experiment
// has been rolled out in chromium.
#define NOTREACHED_IN_MIGRATION() DCHECK(false)

#endif  // MINI_CHROMIUM_BASE_NOTREACHED_H_
