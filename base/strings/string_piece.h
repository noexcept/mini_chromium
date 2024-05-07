// Copyright 2006-2008 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_STRINGS_STRING_PIECE_H_
#define MINI_CHROMIUM_BASE_STRINGS_STRING_PIECE_H_

#include <algorithm>
#include <iterator>
#include <ostream>
#include <string>

namespace base {
  using StringPiece = std::string_view;
using StringPiece16 = std::u16string_view;

}  // namespace base;

#endif  // MINI_CHROMIUM_BASE_STRINGS_STRING_PIECE_H_
