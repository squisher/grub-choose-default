' //***************************************************************************
' // ***** Script Header *****
' // =======================================================
' // Based on:
' // Elevation PowerToys for Windows Vista v1.1 (04/29/2008)
' // =======================================================
' // 
' // This file is required to execute mountvol as it needs elevated
' // priviledges.
' //
' // Adjust the value of strArguments below:
' // - l: is the drive letter to mount the partition on
' // - the remainder is the volume name, you can list all
' //   available volumes by running "mountvol" in a cmd.exe window.
' //
' // For testing the volume can be removed with "mountvol l: /d"
' //
' // ***** End Header *****
' //***************************************************************************


Set objShell = CreateObject("Shell.Application")
strApplication = "mountvol"
strArguments = "l: \\?\Volume{fe0e4803-7a3f-11df-bef1-001676bc6680}\"


objShell.ShellExecute strApplication, strArguments, "", "runas"
' We need to sleep here because Windows takes a moment to
' make the drive available
WScript.Sleep 1500
