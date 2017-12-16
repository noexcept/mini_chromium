#!/usr/bin/env python
# coding: utf-8

# Copyright 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function

import argparse
import distutils.version
import errno
import os
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


def _DidNotMeetCriteria(params):
  raise Exception('no suitable SDK found', params)


def _FindSDKWithMinimumVersion(minimum_sdk_version_str):
  minimum_sdk_version = _AsVersion(minimum_sdk_version_str)

  # Try the SDKs that Xcode knows about.
  xcodebuild_showsdks_subprocess = subprocess.Popen(
      ['xcodebuild', '-showsdks'],
      stdout=subprocess.PIPE,
      stderr=open(os.devnull, 'w'))
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
      _DidNotMeetCriteria({'minimum': minimum_sdk_version_str})
    sdk_version_str = sorted(sdk_version_strs, key=_AsVersion)[0]
    sdk_path = _SDKPath(PLATFORM + sdk_version_str)
    sdk_version = _AsVersion(sdk_version_str)
  else:
    # Xcode may not be installed. If the command-line tools are installed, use
    # the system’s default SDK if it meets the requirements.
    sdk_path = _SDKPath()
    sdk_version = _SDKVersion()
    if sdk_version < minimum_sdk_version:
      _DidNotMeetCriteria({'minimum': minimum_sdk_version_str,
                           'sdk_path': sdk_path,
                           'sdk_version': str(sdk_version)})

  return (sdk_version, sdk_path)


def main(args):
  parser = argparse.ArgumentParser(
      description='Find an appropriate macOS SDK',
      epilog='Two lines will be written to standard output: the version of the '
             'selected SDK, and its path.')
  parser.add_argument('--exact', help='an exact SDK version to find')
  parser.add_argument('--minimum', help='the minimum SDK version to find')
  parser.add_argument('--path', help='a known SDK path to validate')
  parsed = parser.parse_args(args)

  if subprocess.call(['xcode-select', '--print-path'],
                     stdout=open(os.devnull, 'w'),
                     stderr=open(os.devnull, 'w')) != 0:
    print(os.path.basename(sys.argv[0]) + """: No developer tools found.
Install Xcode and run "sudo xcodebuild -license", or install Command Line Tools
with "xcode-select --install". If necessary, run "xcode-select --switch" to
select an active developer tools installation.""",
          file=sys.stderr)
    return 1

  if parsed.path is not None:
    # _SDKVersion() doesn’t work with a pathname argument that’s a symbolic
    # link.
    resolved_sdk_path = parsed.path
    tries = 0
    while os.path.islink(resolved_sdk_path):
      tries += 1
      if tries > 32:  # MAXSYMLINKS
        raise IOError(errno.ELOOP, os.strerror(errno.ELOOP), parsed.path)
      resolved_sdk_path = resolved_sdk_path.rstrip('/')
      resolved_sdk_path = os.path.join(os.path.dirname(resolved_sdk_path),
                                       os.readlink(resolved_sdk_path))
    sdk_version = _SDKVersion(resolved_sdk_path)
    sdk_path = parsed.path
  elif parsed.exact is None and parsed.minimum is None:
    # Use the system’s default SDK.
    sdk_version = _SDKVersion()
    sdk_path = _SDKPath()
  elif parsed.exact is not None:
    sdk_version = _SDKVersion(PLATFORM + parsed.exact)
    sdk_path = _SDKPath(PLATFORM + parsed.exact)
  else:
    (sdk_version, sdk_path) = _FindSDKWithMinimumVersion(parsed.minimum)

  # These checks may be redundant depending on how the SDK was chosen.
  if ((parsed.exact is not None and sdk_version != _AsVersion(parsed.exact)) or
      (parsed.minimum is not None and
       sdk_version < _AsVersion(parsed.minimum))):
    _DidNotMeetCriteria({'exact': parsed.exact,
                         'minimum': parsed.minimum,
                         'path': parsed.path,
                         'sdk_path': sdk_path,
                         'sdk_version': str(sdk_version)})

  # Nobody wants trailing slashes. This is true even if “/” is the SDK: it’s
  # better to return an empty string, which will be interpreted as “no sysroot.”
  sdk_path = sdk_path.rstrip(os.path.sep)

  print(sdk_version)
  print(sdk_path)

  return 0


if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
