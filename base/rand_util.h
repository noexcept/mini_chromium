// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_RAND_UTIL_H_
#define MINI_CHROMIUM_BASE_RAND_UTIL_H_

#include <stdint.h>

#include <string>

namespace base {

uint64_t RandUint64();

int RandInt(int min, int max);

uint64_t RandGenerator(uint64_t range);

double RandDouble();

void RandBytes(void* output, size_t output_length);
std::string RandBytesAsString(size_t length);

// An STL UniformRandomBitGenerator backed by RandUint64.
// TODO(tzik): Consider replacing this with a faster implementation.
class RandomBitGenerator {
 public:
  using result_type = uint64_t;
  static constexpr result_type min() { return 0; }
  static constexpr result_type max() { return UINT64_MAX; }
  result_type operator()() const { return RandUint64(); }

  RandomBitGenerator() = default;
  ~RandomBitGenerator() = default;
};

// Shuffles [first, last) randomly. Thread-safe.
template <typename Itr>
void RandomShuffle(Itr first, Itr last) {
  std::shuffle(first, last, RandomBitGenerator());
}

}  // namespace base

#endif  // MINI_CHROMIUM_BASE_RAND_UTIL_H_
