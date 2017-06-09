# Copyright 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../build/common.gypi',
  ],

  'targets': [
    {
      'target_name': 'base',
      'type': 'static_library',
      'include_dirs': [
        '..',
      ],
      'link_settings': {
        'conditions': [
          ['OS=="mac"', {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/ApplicationServices.framework',
              '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
              '$(SDKROOT)/System/Library/Frameworks/IOKit.framework',
              '$(SDKROOT)/System/Library/Frameworks/Security.framework',
            ],
          }],
          ['OS=="win"', {
            'libraries': [
              '-ladvapi32.lib',
            ],
          }],
        ],
      },
      'sources': [
        'atomicops.h',
        'atomicops_internals_atomicword_compat.h',
        'atomicops_internals_mac.h',
        'atomicops_internals_portable.h',
        'atomicops_internals_x86_msvc.h',
        'auto_reset.h',
        'bit_cast.h',
        'compiler_specific.h',
        'debug/alias.cc',
        'debug/alias.h',
        'files/file_path.cc',
        'files/file_path.h',
        'files/file_util.h',
        'files/file_util_posix.cc',
        'files/scoped_file.cc',
        'files/scoped_file.h',
        'format_macros.h',
        'logging.cc',
        'logging.h',
        'mac/close_nocancel.cc',
        'mac/foundation_util.h',
        'mac/foundation_util.mm',
        'mac/mach_logging.h',
        'mac/mach_logging.cc',
        'mac/scoped_cftyperef.h',
        'mac/scoped_ioobject.h',
        'mac/scoped_launch_data.h',
        'mac/scoped_mach_port.cc',
        'mac/scoped_mach_port.h',
        'mac/scoped_mach_vm.cc',
        'mac/scoped_mach_vm.h',
        'mac/scoped_nsautorelease_pool.h',
        'mac/scoped_nsautorelease_pool.mm',
        'mac/scoped_nsobject.h',
        'mac/scoped_typeref.h',
        'macros.h',
        'memory/free_deleter.h',
        'memory/ptr_util.h',
        'memory/scoped_policy.h',
        'numerics/safe_conversions.h',
        'numerics/safe_conversions_impl.h',
        'numerics/safe_math.h',
        'numerics/safe_math_impl.h',
        'posix/eintr_wrapper.h',
        'posix/safe_strerror.cc',
        'posix/safe_strerror.h',
        'process/memory.cc',
        'process/memory.h',
        'rand_util.cc',
        'rand_util.h',
        'scoped_clear_errno.h',
        'scoped_generic.h',
        'strings/string16.cc',
        'strings/string16.h',
        'strings/string_number_conversions.cc',
        'strings/string_number_conversions.h',
        'strings/string_piece.h',
        'strings/string_util.h',
        'strings/string_util_posix.h',
        'strings/string_util_win.cc',
        'strings/string_util_win.h',
        'strings/stringprintf.cc',
        'strings/stringprintf.h',
        'strings/sys_string_conversions.h',
        'strings/sys_string_conversions_mac.mm',
        'strings/utf_string_conversion_utils.cc',
        'strings/utf_string_conversion_utils.h',
        'strings/utf_string_conversions.cc',
        'strings/utf_string_conversions.h',
        'synchronization/condition_variable.h',
        'synchronization/condition_variable_posix.cc',
        'synchronization/lock.cc',
        'synchronization/lock.h',
        'synchronization/lock_impl.h',
        'synchronization/lock_impl_posix.cc',
        'synchronization/lock_impl_win.cc',
        'sys_byteorder.h',
        'template_util.h',
        'third_party/icu/icu_utf.cc',
        'third_party/icu/icu_utf.h',
        'threading/thread_local_storage.cc',
        'threading/thread_local_storage.h',
        'threading/thread_local_storage_posix.cc',
        'threading/thread_local_storage_win.cc',
        '../build/build_config.h',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '..',
        ],
      },
    },
  ],
}
