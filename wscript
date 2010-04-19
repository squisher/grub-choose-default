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

top = '.'
if Options.platform == 'win32':
  out = '_build.win32_'
else:
  out = '_build_'

APPNAME = 'grub-choose-default'
VERSION = '0.6'
IS_DEV=True


if IS_DEV:
  try:
    git = Utils.cmd_output (['git', 'rev-parse', '--short', 'HEAD'], silent=True)
    if git:
      VERSION = (VERSION + '-' + git).strip ()
  except:
    pass


def set_options (opt):
  opt.add_option ('--debug', action='store_true', default=False, help='Enable debugging code', dest='debug')

  if Options.platform == 'win32':
    def_direct = True
  else:
    def_direct = False
  opt.add_option ('--direct', action='store_true', default=def_direct, help='Directly work on grubenv, default under win32', dest='direct')

def configure (ctx):
  print "Configuring", APPNAME 

  ctx.check_tool ('gcc')
  ctx.check_cfg (package='gtk+-2.0', args='--cflags --libs', uselib_store='GTK', mandatory=True)
  if Options.platform != 'win32':
    if ctx.find_program ('docbook-to-man', var='DOCBOOKTOMAN'):
      manpage = 'yes'
    else:
      manpage = 'no'

  if Options.platform == 'win32':
    ctx.check_cfg (package='gio-2.0', args='--cflags --libs', uselib_store='GIO', mandatory=True)

  ctx.check (header_name='string.h', define_name='HAVE_STRING_H')
  ctx.check (header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')
  ctx.check (header_name='sys/types.h', define_name='HAVE_SYS_TYPES_H')
  ctx.check (header_name='unistd.h', define_name='HAVE_UNISTD_H')

  ctx.define ('VERSION', VERSION)
  ctx.define ('PACKAGE', APPNAME)
  ctx.define ('DIRECT', 1 if Options.options.direct else 0)
  ctx.define ('DEBUG', 1 if Options.options.debug else 0)

  ctx.write_config_header ('config.h')
  ctx.env.append_value ('CCDEFINES', ['HAVE_CONFIG_H=1'])

  if Options.options.debug:
    ctx.env.append_value ('CCFLAGS', '-Wall -O0 -g'.split ())

def build (ctx):
  #ctx.recurse ('src')
  ctx.add_subdirs ('src')
  ctx.add_subdirs ('icons')

  if Options.platform != 'win32' and ctx.env.DOCBOOKTOMAN:
    ctx(
        rule = ctx.env.DOCBOOKTOMAN + ' ${SRC} > ${TGT}',
        source = 'grub-choose-default.sgml',
        target = 'grub-choose-default.8',
        install_path = '${PREFIX}/usr/share/man/man8',
        )
