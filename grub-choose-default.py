#!/usr/bin/python

# grub-choose-default
# Version 0.1
#
# by David Mohr <david@mcbf.net>
#
# Copyright 2007 David Mohr
# 
# Released under the GNU GPL v2
# This is free software; see the source for copying conditions. There is NO
# warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

import os

# Config

grub_menu = '/boot/grub/menu.lst'
grub_paths = [ os.path.join('grub','menu.lst'),
		os.path.join('boot','grub','menu.lst') ]


# Imports

import re
import sys
from Tkinter import *

# Functions

def get_win_menu():
	for i in range(ord('c'), ord('z')+1):
       		drive = chr(i) +":\\"

	 	if os.path.exists(drive):
			for p in grub_paths:
				m = os.path.join(drive,p)
				#print m
				if os.path.exists(m):
					return m
	raise IOError, "grub's menu.lst not found"

def init():
	global grub_menu, grub_default
	if sys.platform == "win32":
		grub_menu = get_win_menu()
	grub_default = os.path.join(os.path.split(grub_menu)[0], 'default')
	print "Using", grub_menu, "and", grub_default

def get_menu_entries():
    fmenu = open(grub_menu, 'r')
    pattern = "^[ \t]*title[ \t]+(.*)$"
    entries = []

    for line in fmenu.readlines():
        match = re.search(pattern, line)
        if match:
            #print "MATCH: ",match.group(1)
            entries.append(match.group(1))
    fmenu.close()

    #for entry in entries:
    #    print entry

    return entries

def get_grub_default():
    fdefault = open(grub_default, 'r')
    default_file = fdefault.readlines()
    fdefault.close()
    #print default_file
    match = re.search("^\d+", default_file[0])
    old_default = match.group()
    #print old_default, "was the old default"

    return (default_file, int(old_default))

def set_grub_default(i):
    print i, "(", grub_entries[i], ") is the new default"
    default_file[0] = "%d\n" % i 
    #print default_file

    fdefault = open(grub_default, "w")
    fdefault.writelines(default_file)
    fdefault.close()
    sys.exit()

init()
grub_entries = get_menu_entries()
(default_file, old_default) = get_grub_default()

tk_root = Tk()
tk_root.title('grub choose default')
lbl = Label(tk_root, text="Click on an entry below to make it the default for the next boot")
lbl.pack()
buttons = []
for i in range(len(grub_entries)):
    bt = Button(tk_root, 
        text=grub_entries[i], 
        #font=("Courier", 10, "bold"),
        command=(lambda df=i: set_grub_default(df)) )
    #print bt.cget("font")
    if i==old_default:
        #print i, "old default"
        bt.configure(bg="darkgreen", fg="white")
    bt.pack(expand=YES,fill=X)
    buttons.append(bt)

tk_root.mainloop()
