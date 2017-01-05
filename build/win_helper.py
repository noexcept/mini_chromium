#!/usr/bin/env python

# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import _winreg
import os
import re
import subprocess
import sys


def _RegistryGetValue(key, value):
  """Use the _winreg module to obtain the value of a registry key.

  Args:
    key: The registry key.
    value: The particular registry value to read.
  Return:
    contents of the registry key's value, or None on failure.
  """
  try:
    root, subkey = key.split('\\', 1)
    assert root == 'HKLM'  # Only need HKLM for now.
    with _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, subkey) as hkey:
      return _winreg.QueryValueEx(hkey, value)[0]
  except WindowsError:
    return None


def _ExtractImportantEnvironment(output_of_set):
  """Extracts environment variables required for the toolchain to run from
  a textual dump output by the cmd.exe 'set' command."""
  envvars_to_save = (
      'include',
      'lib',
      'libpath',
      'path',
      'pathext',
      'systemroot',
      'temp',
      'tmp',
      )
  env = {}
  for line in output_of_set.splitlines():
    for envvar in envvars_to_save:
      if re.match(envvar + '=', line.lower()):
        var, setting = line.split('=', 1)
        env[var.upper()] = setting
        break
  for required in ('SYSTEMROOT', 'TEMP', 'TMP'):
    if required not in env:
      raise Exception('Environment variable "%s" '
                      'required to be set to valid path' % required)
  return env


def _FormatAsEnvironmentBlock(envvar_dict):
  """Format as an 'environment block' directly suitable for CreateProcess.
  Briefly this is a list of key=value\0, terminated by an additional \0. See
  CreateProcess() documentation for more details."""
  block = ''
  nul = '\0'
  for key, value in envvar_dict.iteritems():
    block += key + '=' + value + nul
  block += nul
  return block


def _GenerateEnvironmentFiles(install_dir, out_dir):
  """It's not sufficient to have the absolute path to the compiler, linker, etc.
  on Windows, as those tools rely on .dlls being in the PATH. We also need to
  support both x86 and x64 compilers. Different architectures require a
  different compiler binary, and different supporting environment variables
  (INCLUDE, LIB, LIBPATH). So, we extract the environment here, wrap all
  invocations of compiler tools (cl, link, lib, rc, midl, etc.) to set up the
  environment, and then do not prefix the compiler with an absolute path,
  instead preferring something like "cl.exe" in the rule which will then run
  whichever the environment setup has put in the path."""
  archs = ('x86', 'amd64')
  result = []
  for arch in archs:
    # Extract environment variables for subprocesses.
    args = [os.path.join(install_dir, 'VC\\vcvarsall.bat')]
    args.extend((arch, '&&', 'set'))
    popen = subprocess.Popen(
        args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    variables, _ = popen.communicate()
    if popen.returncode != 0:
      raise Exception('"%s" failed with error %d' % (args, popen.returncode))
    env = _ExtractImportantEnvironment(variables)

    env_block = _FormatAsEnvironmentBlock(env)
    basename = 'environment.' + arch
    with open(os.path.join(out_dir, basename), 'wb') as f:
      f.write(env_block)
    result.append(basename)
  return result


def _GetEnvAsDict(self, arch):
  """Gets the saved environment from a file for a given architecture."""
  # The environment is saved as an "environment block" (see CreateProcess()
  # for details, which is the format required for ninja). We convert to a dict
  # here. Drop last 2 NULs, one for list terminator, one for trailing vs.
  # separator.
  pairs = open(arch).read()[:-2].split('\0')
  kvs = [item.split('=', 1) for item in pairs]
  return dict(kvs)


class WinTool(object):
  def Dispatch(self, args):
    """Dispatches a string command to a method."""
    if len(args) < 1:
      raise Exception("Not enough arguments")

    method = "Exec%s" % self._CommandifyName(args[0])
    return getattr(self, method)(*args[1:])

  def _CommandifyName(self, name_string):
    """Transforms a tool name like recursive-mirror to RecursiveMirror."""
    return name_string.title().replace('-', '')

  def ExecLinkWrapper(self, arch, *args):
    """Filter diagnostic output from link that looks like:
    '   Creating library ui.dll.lib and object ui.dll.exp'
    This happens when there are exports from the dll or exe.
    """
    env = _GetEnvAsDict(arch)
    args = list(args)  # *args is a tuple by default, which is read-only.
    args[0] = args[0].replace('/', '\\')
    link = subprocess.Popen(args, shell=True, env=env, stdout=subprocess.PIPE)
    out, _ = link.communicate()
    for line in out.splitlines():
      if (not line.startswith('   Creating library ') and
          not line.startswith('Generating code') and
          not line.startswith('Finished generating code')):
        print line
    return link.returncode

  def ExecStamp(self, path):
    """Simple stamp command."""
    open(path, 'w').close()
    return 0

  def _GetVisualStudioInstallDirOrDie(self):
    look_for = ('14.0', '12.0')
    result = ''
    for version in look_for:
      keys = [r'HKLM\Software\Microsoft\VisualStudio\%s' % version,
              r'HKLM\Software\Wow6432Node\Microsoft\VisualStudio\%s' % version]
      for key in keys:
        path = _RegistryGetValue(key, 'InstallDir')
        if not path:
          continue
        return os.path.normpath(os.path.join(path, '..', '..'))

    if not result:
      raise Exception('Visual Studio installation dir not found')

  def ExecGetVisualStudioData(self, outdir, *args):
    install_dir = self._GetVisualStudioInstallDirOrDie()
    x86_file, x64_file = _GenerateEnvironmentFiles(install_dir, outdir)
    result = '''install_dir = "%s"
x86_environment_file = "%s"
x64_environment_file = "%s"''' % (install_dir, x86_file, x64_file)
    print result
    return 0


if __name__ == '__main__':
  sys.exit(WinTool().Dispatch(sys.argv[1:]))
