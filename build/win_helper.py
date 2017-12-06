#!/usr/bin/env python

# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import subprocess
import sys


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

  def ExecLinkWrapper(self, *args):
    """Filter diagnostic output from link that looks like:
    '   Creating library ui.dll.lib and object ui.dll.exp'
    This happens when there are exports from the dll or exe.
    """
    args = list(args)  # *args is a tuple by default, which is read-only.
    args[0] = args[0].replace('/', '\\')
    link = subprocess.Popen(args, shell=True, stdout=subprocess.PIPE)
    out, _ = link.communicate()
    for line in out.splitlines():
      if (not line.startswith('   Creating library ') and
          not line.startswith('Generating code') and
          not line.startswith('Finished generating code')):
        print line
    return link.returncode

  def ExecRecursiveMirror(self, source, dest):
    """Emulation of rm -rf out && cp -af in out."""
    if os.path.exists(dest):
      if os.path.isdir(dest):
        def _on_error(fn, path, dummy_excinfo):
          # The operation failed, possibly because the file is set to
          # read-only. If that's why, make it writable and try the op again.
          if not os.access(path, os.W_OK):
            os.chmod(path, stat.S_IWRITE)
          fn(path)
        shutil.rmtree(dest, onerror=_on_error)
      else:
        if not os.access(dest, os.W_OK):
          # Attempt to make the file writable before deleting it.
          os.chmod(dest, stat.S_IWRITE)
        os.unlink(dest)

    if os.path.isdir(source):
      shutil.copytree(source, dest)
    else:
      shutil.copy2(source, dest)
      # Try to diagnose crbug.com/741603
      if not os.path.exists(dest):
        raise Exception("Copying of %s to %s failed" % (source, dest))


  def ExecStamp(self, path):
    """Simple stamp command."""
    open(path, 'w').close()
    return 0


if __name__ == '__main__':
  sys.exit(WinTool().Dispatch(sys.argv[1:]))
