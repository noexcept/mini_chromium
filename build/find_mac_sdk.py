#!/usr/bin/env python
# coding: utf-8

# Copyright 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function

import argparse
import distutils.version
import os.path
import re
import subprocess
import sys

PLATFORM = 'macosx'


def _AsVersion(string):
  return distutils.version.StrictVersion(string)


def _RunXCRun(args, sdk=None):
  xcrun_args = ['xcrun']
  if sdk is not None:
    xcrun_args.extend(['--sdk', sdk])
  xcrun_args.extend(args)
  return subprocess.check_output(xcrun_args).decode('utf-8').rstrip()


def _SDKPath(sdk=None):
  return _RunXCRun(['--show-sdk-path'], sdk)


def _SDKVersion(sdk=None):
  return _AsVersion(_RunXCRun(['--show-sdk-version'], sdk))


def _DidNotMeetMinimum(params):
  raise Exception('no suitable SDK found', params)


def _FindSDKMinimumVersion(minimum_sdk_version_str):
  minimum_sdk_version = _AsVersion(minimum_sdk_version_str)

  # Try the SDKs that Xcode knows about.
  xcodebuild_showsdks_subprocess = subprocess.Popen(['xcodebuild', '-showsdks'],
                                                    stdout=subprocess.PIPE,
                                                    stderr=subprocess.PIPE)
  xcodebuild_showsdks_output = (
      xcodebuild_showsdks_subprocess.communicate()[0].decode('utf-8'))
  if xcodebuild_showsdks_subprocess.returncode == 0:
    # Collect strings instead of version objects to preserve the precise
    # format used to identify each SDK.
    sdk_version_strs = []
    for line in xcodebuild_showsdks_output.splitlines():
      match = re.match(
          '\t((Mac )?OS X|macOS) .+-sdk ' + re.escape(PLATFORM) + '(.+)$', line)
      if match:
        sdk_version_str = match.group(3)
        if _AsVersion(sdk_version_str) >= minimum_sdk_version:
          sdk_version_strs.append(sdk_version_str)

    if len(sdk_version_strs) == 0:
      _DidNotMeetMinimum({'minimum': minimum_sdk_version_str})
    sdk_version_str = sorted(sdk_version_strs, key=_AsVersion)[0]
    sdk_path = _SDKPath(PLATFORM + sdk_version_str)
    sdk_version = _AsVersion(sdk_version_str)
  else:
    # Xcode may not be installed. If the command-line tools are installed, use
    # the system’s default SDK if it meets the requirements.
    sdk_path = _SDKPath()
    sdk_version = _SDKVersion()
    if sdk_version < minimum_sdk_version:
      _DidNotMeetMinimum({'minimum': minimum_sdk_version_str})

  return (sdk_path, sdk_version)


def main(args):
  parser = argparse.ArgumentParser(description='Find an appropriate macOS SDK')
  parser.add_argument('--exact', help='an exact SDK version to find')
  parser.add_argument('--minimum', help='the minimum SDK version to find')
  parser.add_argument('--path', help='a known SDK path to validate')
  parsed = parser.parse_args(args)

  if parsed.path is not None:
    sdk_path = parsed.path
    sdk_version = _SDKVersion(sdk_path)
  elif parsed.exact is None and parsed.minimum is None:
    # Use the system’s default SDK.
    sdk_path = _SDKPath()
    sdk_version = _SDKVersion()
  elif parsed.exact is not None:
    sdk_path = _SDKPath(PLATFORM + parsed.exact)
    sdk_version = _SDKVersion(PLATFORM + parsed.exact)
  else:
    (sdk_path, sdk_version) = _FindSDKMinimumVersion(parsed.minimum)

  # This is redundant if _FindSDKMinimumVersion() was called, but it’s necessary
  # if --minimum was specified along with --exact or --path.
  if parsed.minimum is not None and sdk_version < _AsVersion(parsed.minimum):
    _DidNotMeetMinimum({'exact': parsed.exact,
                        'minimum': parsed.minimum,
                        'path': parsed.path})

  # Nobody wants trailing slashes. This is true even if “/” is the SDK: it’s
  # better to return an empty string, which will be interpreted as “no sysroot.”
  sdk_path.rstrip(os.path.sep)

  print(sdk_version)
  print(sdk_path)

  return 0


if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
