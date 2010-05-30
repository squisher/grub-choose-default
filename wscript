#!/usr/bin/env python
#
# Copyright 2010  David Mohr <david@mcbf.net>
#
# Licensed under the EXPAT license.
#
# In parts based on midori's wscript

#from __future__ import print_function

import Options
import Utils
import os
import shutil

top = '.'
if Options.platform == 'win32':
  out = '_build.win32_'
else:
  out = '_build_'

APPNAME = 'grub-choose-default'
VERSION = '0.9'
IS_DEV=False


if IS_DEV:
  try:
    git = Utils.cmd_output (['git', 'rev-parse', '--short', 'HEAD'], silent=True)
    if git:
      VERSION = (VERSION + '-' + git).strip ()
  except:
    pass


def set_options (opt):
  opt.tool_options('gnu_dirs')

  opt.add_option ('--debug', action='store_true', default=False, help='Enable debugging code', dest='debug')
  opt.add_option ('--package', action='store_true', default=False, help='Create a source distribution, use with the \'build\' target', dest='package')

  if Options.platform == 'win32':
    def_direct = True
  else:
    def_direct = False
  opt.add_option ('--direct', action='store_true', default=def_direct, help='Directly work on grubenv, default under win32', dest='direct')

  opt.add_option ('--distribution', action='store', default=1, help='Sets index of the error message to use, see gchd-messages.h for values', dest='distro')


def configure (ctx):
  print "Configuring", APPNAME 

  ctx.check_tool ('gcc')
  ctx.check_tool ('gnu_dirs')
  ctx.check_cfg (package='gtk+-2.0', args='--cflags --libs', uselib_store='GTK', mandatory=True)
  if Options.platform != 'win32':
    if ctx.find_program ('docbook-to-man', var='DOCBOOKTOMAN'):
      manpage = 'yes'
    else:
      manpage = 'no'

  #print ctx.env
  if Options.platform == 'win32':
    ctx.check_cfg (package='gio-2.0', args='--cflags --libs', uselib_store='GIO', mandatory=True)
    if not Options.options.debug:
      ctx.env.append_value ('LINKFLAGS', '-mwindows'.split ())

  ctx.check (header_name='string.h', define_name='HAVE_STRING_H')
  ctx.check (header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')
  ctx.check (header_name='sys/types.h', define_name='HAVE_SYS_TYPES_H')
  ctx.check (header_name='unistd.h', define_name='HAVE_UNISTD_H')

  ctx.define ('VERSION', VERSION)
  ctx.define ('PACKAGE', APPNAME)
  ctx.define ('DIRECT', 1 if Options.options.direct else 0)
  ctx.define ('DEBUG', 1 if Options.options.debug else 0)
  # TODO: check distro range at this point, and not at runtime in gchd.c
  ctx.define ('DISTRIBUTION', int(Options.options.distro))


  ctx.write_config_header ('config.h')
  ctx.env.append_value ('CCDEFINES', ['HAVE_CONFIG_H=1'])

  if Options.options.debug:
    ctx.env.append_value ('CCFLAGS', '-Wall -O0 -g'.split ())


def build (ctx):
  #ctx.recurse ('src')
  ctx.add_subdirs ('src')
  ctx.add_subdirs ('icons')

  if Options.platform != 'win32':
    if ctx.env.DOCBOOKTOMAN:
      ctx(
          rule = ctx.env.DOCBOOKTOMAN + ' ${SRC} > ${TGT}',
          source = 'grub-choose-default.sgml',
          target = 'grub-choose-default.8',
          install_path = '${MANDIR}/man8',
          )
    ctx.install_files ("${DATADIR}/applications", "grub-choose-default.desktop");
    ctx.install_files ('${DOCDIR}/',
                       ctx.path.ant_glob('reboot/*'), relative_trick=True)

  ctx.install_files ('${DOCDIR}/',
                     'AUTHORS GPL-2 GPL-3 ChangeLog EXPAT README NEWS')

  if Options.options.package:
    #
    # this is all pretty hackish, but works
    #
    print "Creating package..."

    def glob (dirname, pattern):
      return dirname, ctx.path.ant_glob (dirname + '/' + '*.c', relative_trick=True) + ' '

    dirs = []
    files = ''

    d, fns  = glob ('src', '*.c')
    dirs.append(d)
    files += fns
    d, fns = glob ('src', '*.h')
    dirs.append(d)
    files += fns
    d, fns = glob ('reboot', '*')
    dirs.append(d)
    files += fns
    d, fns = glob ('icons', '**')
    dirs.append(d)
    files += fns
    d, fns = glob ('win32', '**')
    dirs.append(d)
    files += fns

    files += ctx.path.ant_glob ('README*', relative_trick=True) + ' '
    files += 'ChangeLog NEWS AUTHORS '
    files += 'EXPAT GPL-2 GPL-3 '
    files += 'update-changelog.py waf '
    files += 'wscript src/wscript_build '

    distdir = ('%(package)s-%(version)s') % { 'package': APPNAME, 'version': VERSION }
    tarname = distdir + '.tar.bz2'

    # copy all the files into the appropriate subdirectory
    os.mkdir (distdir)
    for d in [distdir + '/' + x for x in set(dirs)]:
      #print d
      os.mkdir (d)
    for f in files.split (' '):
      if f:
        shutil.copy (f, distdir + '/' + f)

    Utils.exec_command ('tar cjf ' + tarname + ' ' + distdir)
    Utils.exec_command ('sha1sum ' + tarname + ' > ' + tarname + '.SHA1')

    print "... " + tarname + " created"
    shutil.rmtree (distdir)
    return

