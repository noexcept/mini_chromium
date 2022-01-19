// Copyright 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MINI_CHROMIUM_BUILD_BUILD_CONFIG_H_
#define MINI_CHROMIUM_BUILD_BUILD_CONFIG_H_

#if defined(__APPLE__)
#include <TargetConditionals.h>
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_APPLE() 1
#if defined(TARGET_OS_OSX) && TARGET_OS_OSX
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_MAC() 1
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_IOS() 0
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_MAC() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_IOS() 1
#endif  // TARGET_OS_*
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_ANDROID() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_LINUX() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_CHROMEOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_WIN() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_FUCHSIA() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_POSIX() 1
#elif defined(__ANDROID__)
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_APPLE() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_MAC() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_IOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_ANDROID() 1
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_LINUX() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_CHROMEOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_WIN() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_FUCHSIA() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_POSIX() 1
#elif defined(__linux__)
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_APPLE() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_MAC() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_IOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_ANDROID() 0
#if !defined(OS_CHROMEOS)
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_LINUX() 1
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_CHROMEOS() 0
#else
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_LINUX() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_CHROMEOS() 1
#endif
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_WIN() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_FUCHSIA() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_POSIX() 1
#elif defined(_WIN32)
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_APPLE() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_MAC() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_IOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_ANDROID() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_LINUX() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_CHROMEOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_WIN() 1
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_FUCHSIA() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_POSIX() 0
#elif defined(__Fuchsia__)
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_APPLE() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_MAC() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_IOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_ANDROID() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_LINUX() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_CHROMEOS() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_WIN() 0
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_FUCHSIA() 1
#define MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_POSIX() 1
#else
#error Please add support for your platform in build/build_config.h
#endif

// Compiler detection.
#if defined(__GNUC__)
#define COMPILER_GCC 1
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#else
#error Please add support for your compiler in build/build_config.h
#endif

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86_64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86 1
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARMEL 1
#define ARCH_CPU_32_BITS 1
#elif defined(_M_ARM64) || defined(__aarch64__)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARM64 1
#define ARCH_CPU_64_BITS 1
#if defined(_M_ARM64)
#define ARCH_CPU_LITTLE_ENDIAN 1
#endif
#elif defined(__MIPSEL__)
#define ARCH_CPU_MIPS_FAMILY 1
#if !defined(__LP64__)
#define ARCH_CPU_MIPSEL 1
#define ARCH_CPU_32_BITS 1
#else
#define ARCH_CPU_MIPS64EL 1
#define ARCH_CPU_64_BITS 1
#endif
#else
#error Please add support for your architecture in build/build_config.h
#endif

#if !defined(ARCH_CPU_LITTLE_ENDIAN) && !defined(ARCH_CPU_BIG_ENDIAN)
#if defined(__LITTLE_ENDIAN__) || \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) || \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define ARCH_CPU_BIG_ENDIAN 1
#else
#error Please add support for your architecture in build/build_config.h
#endif
#endif

#if MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_POSIX() && \
    defined(COMPILER_GCC) && defined(__WCHAR_MAX__) &&   \
    (__WCHAR_MAX__ == 0x7fffffff || __WCHAR_MAX__ == 0xffffffff)
#define WCHAR_T_IS_UTF32
#elif MINI_CHROMIUM_INTERNAL_BUILDFLAG_VALUE_IS_WIN()
#define WCHAR_T_IS_UTF16
#else
#error Please add support for your compiler in build/build_config.h
#endif

#endif  // MINI_CHROMIUM_BUILD_BUILD_CONFIG_H_
