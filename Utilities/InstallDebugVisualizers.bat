@echo off
rem =========================================================================================
rem InstallDebugVisualizers.bat - installs visualizers for Visual Studio.
rem Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
rem =========================================================================================

call ".\LocateVisualStudio.bat"

rem Setting up debug visualizers for Visual Studio..
mkdir "%UserProfile%\Documents\Visual Studio %VSCompilerYear%\Visualizers\"
copy /y ".\Natvis\GoddamnTemplateLibrary.natvis" "%UserProfile%\Documents\Visual Studio %VSCompilerYear%\Visualizers\"

pause
