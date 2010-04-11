#!/usr/bin/env python

#from __future__ import print_function

top = '.'
out = '_build_'

APPNAME = 'grub-choose-default'
VERSION = '0.7'

def set_options (opt):
  opt.add_option ('--debug', action='store_true', default=False, help='Enable debugging code', dest='debug')

def configure (ctx):
  print "Configuring", APPNAME 

  ctx.check_tool ('gcc')
  ctx.check_cfg (package='gtk+-2.0', args='--cflags --libs', uselib_store='GTK', mandatory=True)

  ctx.check (header_name='string.h', define_name='HAVE_STRING_H')
  ctx.check (header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')
  ctx.check (header_name='sys/types.h', define_name='HAVE_SYS_TYPES_H')
  ctx.check (header_name='unistd.h', define_name='HAVE_UNISTD_H')

  ctx.define ('VERSION', VERSION)
  ctx.define ('PACKAGE', APPNAME)

  ctx.env.append_value ('CCFLAGS', '-DHAVE_CONFIG_H')

  ctx.write_config_header ('config.h')

def build (ctx):
  #ctx.recurse ('src')
  ctx.add_subdirs ('src')

  if Options.platform != 'win32':
    ctx(
        rule = 'docbook-to-man ${SRC} > ${TGT}',
        source = 'grub-choose-default.sgml',
        target = 'grub-choose-default.8',
        install_path = '${MANDIR}/man8',
        )
