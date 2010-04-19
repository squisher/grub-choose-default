# grub-choose-default installer script
# based on: 
# grub-choose-default Windows installer script
# Version 0.4 28-Apr-2009

# Copyright (C) 2009-2010 by
#   Jesper Lund <mail@jesperlund.com>
#   Andrew Resch <andrewresch@gmail.com>
#   John Garland <johnnybg@gmail.com>
#   David Mohr <david@mcbf.net>

# You may redistribute it and/or modify it under the terms of the
# GNU General Public License, as published by the Free Software
# Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# grub-choose-default is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with grub-choose-default.    If not, write to:
# 	The Free Software Foundation, Inc.,
# 	51 Franklin Street, Fifth Floor
# 	Boston, MA    02110-1301, USA.
#

# Set default compressor
SetCompressor lzma

###
### --- The PROGRAM_VERSION !define need to be updated with new grub-choose-default versions ---
###

# Script version; displayed when running the installer
!define GCHD_INSTALLER_VERSION "0.4"

# grub-choose-default program information
!define PROGRAM_NAME "grub-choose-default"
!define PROGRAM_VERSION "0.6"
!define PROGRAM_WEB_SITE "http://ds.mcbf.net/wiki/GrubChooseDefault"

# Installer for GTK+ 2.12 runtime; will be downloaded from grub-choose-default-torrent.org
!define GCHD_GTK_DEPENDENCY "gtk2-runtime-2.16.6-2010-02-24-ash.exe"


# --- Interface settings ---

# Modern User Interface 2
!include MUI2.nsh

!include "MUI_EXTRAPAGES.nsh"

# Installer
!define MUI_ICON "grub-choose-default.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "grub-choose-default-logo.bmp"
#!define MUI_WELCOMEFINISHPAGE_BITMAP "installer-side.bmp"
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_ABORTWARNING

# Uninstaller
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_HEADERIMAGE_UNBITMAP "grub-choose-default-logo.bmp"
#!define MUI_WELCOMEFINISHPAGE_UNBITMAP "installer-side.bmp"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# --- Start of Modern User Interface ---

# Welcome page
!insertmacro MUI_PAGE_WELCOME

# License page
!insertmacro MUI_PAGE_LICENSE "..\GPL-2"

# README page
!insertmacro MUI_PAGE_README "../README.win32"

# Components page
!insertmacro MUI_PAGE_COMPONENTS

# Let the user select the installation directory
!insertmacro MUI_PAGE_DIRECTORY

# Run installation
!insertmacro MUI_PAGE_INSTFILES

# Display 'finished' page
!insertmacro MUI_PAGE_FINISH

# Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

# Language files
!insertmacro MUI_LANGUAGE "English"


# --- Functions ---

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd


# --- Installation sections ---

# Compare versions
!include "WordFunc.nsh"

!define PROGRAM_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}"
!define PROGRAM_UNINST_ROOT_KEY "HKLM"

# Branding text
BrandingText "grub-choose-default Windows Installer v${GCHD_INSTALLER_VERSION}"

Name "${PROGRAM_NAME} ${PROGRAM_VERSION}"
OutFile "..\_build.win32_\grub-choose-default-${PROGRAM_VERSION}-win32-setup.exe"

InstallDir "$PROGRAMFILES\grub-choose-default"

ShowInstDetails show
ShowUnInstDetails show

# Install main application
Section "grub-choose-default" Section1
  SectionIn RO

  # Clean up previous confusion between grub-choose-default.ico and grub-choose-default.ico (seems to matter on Vista registry settings?)
  Delete "$INSTDIR\grub-choose-default.ico"

  SetOverwrite ifnewer
  SetOutPath $INSTDIR
  File "..\GPL-2"
  File "..\_build.win32_\default\src\grub-choose-default.exe"
  #File "..\_build.win32_\default\src\gchd-test.exe"
  File "grub-choose-default.ico"

  SetOutPath $APPDATA\grub-choose-default
  SetOverwrite off
  File /oname=reboot.vbs "..\reboot\windows.vbs"
  SetOverwrite lastused

  # Create grub-choose-default.cmd file
#  fileOpen $0 "$INSTDIR\grub-choose-default.cmd" w
#  fileWrite $0 '@ECHO OFF$\r$\n'
#  fileWrite $0 'SET GCHDFOLDER="$INSTDIR"$\r$\n'
#  fileWrite $0 'SET STARTX_APP="$INSTDIR\StartX.exe"$\r$\n'
#  fileWrite $0 '$\r$\n'
#  fileWrite $0 'IF ""%1"" == """" ( $\r$\n'
#  fileWrite $0 '  %STARTX_APP% /B /D%GCHDFOLDER% "$INSTDIR\grub-choose-default-Python\grub-choose-default.exe"$\r$\n'
#  fileWrite $0 ') ELSE ( $\r$\n'
#  fileWrite $0 '  %STARTX_APP% /B /D%GCHDFOLDER% "$INSTDIR\grub-choose-default-Python\grub-choose-default.exe "%1" "%2" "%3" "%4""$\r$\n'
#  fileWrite $0 ')$\r$\n'
#  fileClose $0

SectionEnd

Section -StartMenu_Desktop_Links
  WriteIniStr "$INSTDIR\homepage.url" "InternetShortcut" "URL" "${PROGRAM_WEB_SITE}"

  CreateDirectory "$SMPROGRAMS\grub-choose-default"
  CreateShortCut "$SMPROGRAMS\grub-choose-default\grub-choose-default.lnk" "$INSTDIR\grub-choose-default.exe" "" "$INSTDIR\grub-choose-default.ico"
  CreateShortCut "$SMPROGRAMS\grub-choose-default\Project homepage.lnk" "$INSTDIR\Homepage.url"
  CreateShortCut "$SMPROGRAMS\grub-choose-default\Uninstall grub-choose-default.lnk" "$INSTDIR\grub-choose-default-uninst.exe"
SectionEnd

Section -Uninstaller
  WriteUninstaller "$INSTDIR\grub-choose-default-uninst.exe"
  WriteRegStr ${PROGRAM_UNINST_ROOT_KEY} "${PROGRAM_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PROGRAM_UNINST_ROOT_KEY} "${PROGRAM_UNINST_KEY}" "UninstallString" "$INSTDIR\grub-choose-default-uninst.exe"
  WriteRegStr ${PROGRAM_UNINST_ROOT_KEY} "${PROGRAM_UNINST_KEY}" "DisplayIcon" "$INSTDIR\grub-choose-default.ico"
SectionEnd

# Install GTK+ 2.16
Section "GTK+ 2.16 runtime" Section2
  GTK_install_start:
  MessageBox MB_OK "You will now download and run the installer for the GTK+ 2.16 runtime. \
    You must be connected to the internet before you press the OK button. \
    The GTK+ runtime can be installed in any location, \
    because the GTK+ installer adds the location to the global PATH variable. \
    Please note that the GTK+ 2.16 runtime is not removed by the grub-choose-default uninstaller. \
    You must use the GTK+ 2.16 uninstaller if you want to remove it together with grub-choose-default."

  # Download GTK+ installer to TEMP dir
  NSISdl::download http://dsp.mcbf.net/grub-choose-default/${GCHD_GTK_DEPENDENCY} "$TEMP\${GCHD_GTK_DEPENDENCY}"

  # Get return value (success, cancel, or string describing the network error)
  Pop $2
  StrCmp $2 "success" 0 GTK_download_error

  ExecWait '"$TEMP\${GCHD_GTK_DEPENDENCY}" /compatdlls=yes'
  Goto GTK_install_exit

  GTK_download_error:
  MessageBox MB_ICONEXCLAMATION|MB_OK "Download of GTK+ 2.16 installer failed (return code: $2). \
      You must install the GTK+ 2.16 runtime manually, or grub-choose-default will fail to run on your system."

  GTK_install_exit:
SectionEnd

LangString DESC_Section1 ${LANG_ENGLISH} "Install grub-choose-default"
LangString DESC_Section2 ${LANG_ENGLISH} "Download and install the GTK+ 2.16 runtime. \
  This is skipped automatically if GTK+ is already installed."

# Fields are: Title, subtitle, above text area, below text area
${ReadmeLanguage} "${LANG_ENGLISH}" \
    "Read Me" \
    "Please read the following important information." \
    "" \
    ""

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Section1} $(DESC_Section1)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section2} $(DESC_Section2)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


# --- Uninstallation section(s) ---

Section Uninstall
  Delete "$INSTDIR\grub-choose-default-uninst.exe"
  Delete "$INSTDIR\GPL-2"
  Delete "$INSTDIR\grub-choose-default.exe"
  #Delete "$INSTDIR\gchd-test.exe"
  Delete "$INSTDIR\Homepage.url"
  Delete "$INSTDIR\grub-choose-default.ico"

  Delete "$SMPROGRAMS\grub-choose-default\grub-choose-default.lnk"
  Delete "$SMPROGRAMS\grub-choose-default\Uninstall grub-choose-default.lnk"
  Delete "$SMPROGRAMS\grub-choose-default\Project homepage.lnk"
  Delete "$DESKTOP\grub-choose-default.lnk"

  RmDir "$SMPROGRAMS\grub-choose-default"
  RmDir "$INSTDIR"

  DeleteRegKey ${PROGRAM_UNINST_ROOT_KEY} "${PROGRAM_UNINST_KEY}"

  # This key is only used by grub-choose-default, so we should always delete it
  DeleteRegKey HKCR "grub-choose-default"
SectionEnd
