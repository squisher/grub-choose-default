#!/usr/bin/env python
#
# grub-choose-default
#
# Version 0.2, 2007-07-28
#
# by David Mohr <david@mcbf.net>
#
# Copyright 2007 David Mohr
# 
# Released under the GNU GPL v2 or later
# This is free software; see the source for copying conditions. There is NO
# warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Config

import os

grub_menu_locs = [ 
        os.path.join('tmp','menu.lst'),
        os.path.join('grub','menu.lst'),
    	os.path.join('boot','grub','menu.lst'),
    	os.path.join('grub','grub.conf'),
    	os.path.join('boot','grub','grub.conf')
        ]
reboot_default = True   # default for the do reboot button:
                        # True => checked
                        # False => not checked
                        # _Note_: Under Linux this requires additional setup

reboot_cmd_win = ['shutdown',           # reboot in 2 seconds. Propbably 
                  '-r', '-t', '2']      # won't work for win9x etc, which
                                        # needs some dll call instead
grub_set_default_prog = 'grub-set-default'

# Imports

import re
import sys
from Tkinter import *

# the class tkSimpleDialog is a copy from some tutorial, forgot to save the URL
# credit goes to the now unknown author :-)
class tkSimpleDialog(Toplevel):
    def __init__(self, parent, title = None):
        Toplevel.__init__(self, parent)
        self.transient(parent)

        if title:
            self.title(title)

        self.parent = parent

        self.result = None

        body = Frame(self)
        self.initial_focus = self.body(body)
        body.pack(padx=5, pady=5)

        self.buttonbox()

        self.grab_set()

        if not self.initial_focus:
            self.initial_focus = self

        self.protocol("WM_DELETE_WINDOW", self.cancel)

        self.geometry("+%d+%d" % (parent.winfo_rootx()+50,
                                  parent.winfo_rooty()+50))

        self.initial_focus.focus_set()
        self.wait_window(self)

    #
    # construction hooks
    def body(self, master):
        # create dialog body.  return widget that should have
        # initial focus.  this method should be overridden
        pass

    def buttonbox(self):
        # add standard button box. override if you don't want the
        # standard buttons
        
        box = Frame(self)

        w = Button(box, text="OK", width=10, command=self.ok, default=ACTIVE)
        w.pack(side=LEFT, padx=5, pady=5)
        w = Button(box, text="Exit", width=10, command=self.cancel)
        w.pack(side=LEFT, padx=5, pady=5)

        self.bind("&lt;Return>", self.ok)
        self.bind("&lt;Escape>", self.cancel)

        box.pack()

    #
    # standard button semantics

    def ok(self, event=None):

        if not self.validate():
            self.initial_focus.focus_set() # put focus back
            return

        self.withdraw()
        self.update_idletasks()

        self.apply()

        self.cancel()

    def cancel(self, event=None):

        # put focus back to the parent window
        self.parent.focus_set()
        self.destroy()

    #
    # command hooks

    def validate(self):

        return 1 # override

    def apply(self):

        pass # override


class SettingsDialog(tkSimpleDialog):
    def __init__(self, parent, default):
        self.default = default
        tkSimpleDialog.__init__(self, parent)

    def body(self, master):
        Label(master, 
            text=
                "'default saved' needs to be set for this program to work.\n"
                "The current configuration is 'default "+self.default+"'.\n"
                "Should this be changed for you?\n"
                "(Otherwise there is no need to continue)"
            ).pack()

    def apply(self):
        self.result = True

def _delete_window():
    #print "Delete event..."
    tk_root.destroy()
    sys.exit(1)

grub_menu = None

# Functions

def get_menu(prefix):
    for p in grub_menu_locs:
        m = os.path.join(prefix,p)
        #print m
        if os.path.exists(m):
            return m
    return None


def get_win_menu():
    for i in range(ord('c'), ord('z')+1):
        drive = chr(i) +":\\"

     	if os.path.exists(drive):
            m = get_menu(drive)
            if m != None:
                return m
    raise IOError, "grub's menu.lst not found"

def init():
    global grub_menu, grub_default_file, grub_menu_locs
    if sys.platform == "win32":
    	grub_menu = get_win_menu()
    else:
    	grub_menu = get_menu("/")

                
    grub_default_file = os.path.join(os.path.split(grub_menu)[0], 'default')
    print >> sys.stderr, "Using", grub_menu, "and", grub_default_file

def get_menu_settings():
    fmenu = open(grub_menu, 'r')
    entries_pattern = "^[ \t]*title[ \t]+(.*)$"
    default_pattern = "^[ \t]*default[ \t]+([A-Za-z0-9]+)"
    entries = []
    default = None

    for line in fmenu.readlines():
        entries_match = re.search(entries_pattern, line)
        default_match = re.search(default_pattern, line)
        if entries_match:
            entries.append(entries_match.group(1))
        if default_match:
            default = default_match.group(1)
    fmenu.close()

    #for entry in entries:
        #print "entry", entry
    #print "default", default    

    return (entries, default)

def get_grub_default():
    fdefault = open(grub_default_file, 'r')
    default_file = fdefault.readlines()
    fdefault.close()
    #print default_file
    match = re.search("^\d+", default_file[0])
    old_default = match.group()
    #print old_default, "was the old default"

    return (default_file, int(old_default))

def change_grub_config_default():
    print >> sys.stderr, "Updating configuration with 'default saved' (backup saved as "+grub_menu+"~)"

    pattern = "^[ \t]*default[ \t]+[a-zA-Z0-9]+(.*)$"
    menu_file = []
    fmenu = open(grub_menu, 'r')
    fbak = open(grub_menu+'~', 'w')
    for line in fmenu.readlines():
        fbak.write(line)
        match = re.search(pattern, line)
        if match:
            line = "default saved"+match.group(1)+"\n"
        menu_file.append(line)
    fmenu.close()
    fbak.close()

    fmenu = open(grub_menu, 'w')
    fmenu.writelines(menu_file)
    fmenu.close()


# this is invoked when someone clicks on one of the buttons
def set_grub_default(i):
    global grub_default_file, default_file, grub_set_default_prog
    print >> sys.stderr, i, "(", grub_entries[i], ") is the new default"
    succ = False
    if sys.platform == "linux2":
        r = os.system(grub_set_default_prog + " %d" % i)
        if r == 0:
            succ = True
        else:
            print >> sys.stderr, "Error running '%s', " \
                                 "modifying default file manually" \
                                 % grub_set_default_prog

    if not succ:
        if not os.path.exists(grub_default_file):
            print >> sys.stderr, "'default' file did not exist, will create", \
                                 grub_default_file, "from scratch"
            # format taken from the grub-set-default shell script
            default_file=[ "%d\n" % i,
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "#\n",
                        "# WARNING: If you want to edit this file directly, do not remove any line\n",
                        "# from this file, including this warning. Using,`grub-set-default\' is\n",
                        "# strongly recommended.\n" ]
        else:
            default_file[0] = "%d\n" % i 
        #print default_file

        fdefault = open(grub_default_file, 'w')
        fdefault.writelines(default_file)
        fdefault.close()

    if reboot_var.get() == 1:
        if sys.platform == "win32":
            os.execvp(reboot_cmd_win[0], reboot_cmd_win)
        else:
            print "User requested to reboot -- \n" \
                  "use a script that greps for 'reboot' in the output\n" \
                  "to actually perform the appropriate action."
    sys.exit(0)

#-----------------------------------------------------------------------------

if len(sys.argv) > 1 and sys.argv[1] == '-r':
    show_reboot = True
else:
    if sys.platform == "win32":
        show_reboot = True
    else:
        show_reboot = False
#print >> sys.stderr, "show_reboot", show_reboot

init()
print >> sys.stderr, "Getting entries"
(grub_entries, grub_default) = get_menu_settings()
print >> sys.stderr, "Getting default"
if not os.path.exists(grub_default_file):
    if grub_default.isdigit():
        old_default = int(grub_default)
    else:
        old_default = None
else:
    (default_file, old_default) = get_grub_default()

print >> sys.stderr, "Creating window, might take a second"
tk_root = Tk()
tk_root.title('grub choose default')
tk_root.protocol("WM_DELETE_WINDOW", _delete_window)
if grub_default != "saved":
    d = SettingsDialog(tk_root, grub_default)
    if d.result == True:
        print >> sys.stderr, "Changing default"
        change_grub_config_default()
    else:
        print >> sys.stderr, "Default is not saved, user chose not to change it -> exit"
        sys.exit(1)

print >> sys.stderr, "Adding label"
lbl = Label(tk_root, text="Click on an entry below to make it the default for the next boot")
print >> sys.stderr, "Packing"
lbl.pack()
buttons = []
for i in range(len(grub_entries)):
    print >> sys.stderr, "Adding button", i
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
reboot_var = IntVar()
reboot_var.set(0)
if show_reboot:
    checkbox = Checkbutton(tk_root, text="Reboot immediately", variable=reboot_var)
    if reboot_default:
        checkbox.select()
    checkbox.pack()

print >> sys.stderr, "Entering main loop"
tk_root.mainloop()
