// Copyright 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BASE_STRINGS_STRING16_H_
#define MINI_CHROMIUM_BASE_STRINGS_STRING16_H_

#include <stdint.h>
#include <stdio.h>

#include <string>

#include "build/build_config.h"

namespace base {

#if defined(WCHAR_T_IS_UTF16)
typedef wchar_t char16;
typedef std::wstring string16;
#elif defined(WCHAR_T_IS_UTF32)
typedef char16_t char16;
typedef std::u16string string16;
#endif  // WCHAR_T_IS_UTF32

}  // namespace base

#if defined(WCHAR_T_IS_UTF32)

namespace std {
std::ostream& operator<<(std::ostream& out, const std::u16string& str);
}  // namespace std

#endif  // WCHAR_T_IS_UTF32

#endif  // MINI_CHROMIUM_BASE_STRINGS_STRING16_H_
