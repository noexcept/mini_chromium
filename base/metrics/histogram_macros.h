// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_METRICS_HISTOGRAM_MACROS_H_
#define BASE_METRICS_HISTOGRAM_MACROS_H_

// These are a subset of no-op stub macros with the same signatures as those in
// Chromium's base. This allows us to instrument Crashpad with histogram "tags"
// and have histograms be gathered when Crashpad is built in Chrome.

#define UMA_HISTOGRAM_MEMORY_KB(name, sample)

#endif  // BASE_METRICS_HISTOGRAM_MACROS_H_
