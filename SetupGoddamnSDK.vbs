' -----------------------------------------------------------------------------------------
' SetupGoddamnSDK.bat - install Goddamn Software Development Kit.
' Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
' 
' History:
'		* 04.07.2014 - Created by James Jhuighuy.
' -----------------------------------------------------------------------------------------

Dim Shell
Set Shell = WScript.CreateObject("WScript.Shell")
Shell.Run  "SETX GODDAMN_SDK %CD%\\"
Set Shell = Nothing

'@echo on