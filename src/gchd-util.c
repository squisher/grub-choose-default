/*
 * Copyright (c) 2009     David Mohr <david@mcbf.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <config.h>

#include <glib.h>
#include <stdarg.h>
#include <stdio.h>

#include "gchd-util.h"

void
gchd_debug_print (char * file, int line, char * fmt, ...)
{
#if DEBUG>0
  va_list args;
  gchar * fullfmt;

  /* waf compiles files from within the build directory,
   * introducing ../ to every file name. Let's cut that off
   */
  fullfmt = g_strdup_printf ("%s:%d %s\n", file+3, line, fmt);

  va_start (args, fmt);
  vfprintf (stderr, fullfmt, args);
  va_end (args);

  g_free (fullfmt);
#endif
}
