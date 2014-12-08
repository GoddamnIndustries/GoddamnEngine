@echo off
rem =========================================================================================
rem SetupGoddamnSDK.bat - installs the Goddamn Software Development Kit [Windows Edition].
rem Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
rem
rem History:
rem		* 03.11.2014 - Created by James Jhuighuy.
rem =========================================================================================

rem Parsing command-line arguments..
if "%1" == "---help" (
	echo Installation/Removal script for the Goddamn SDK [Windows Edition].
	echo Usage: SetupGoddamnSDK.bat [property] [other properties...]
	echo Property:
	echo  ---help   - Prints this help.
	echo  ---remove - Removes the SDK.
	echo Other properties are passed directly to the GoddamnBuildSystem executable.
	goto ExitOnEndOfScript
) else (
	cls
	if "%1" == "---remove" ( 
		goto RemoveGoddamnSDK
	) else (
		goto InstallGoddamnSDK
	)
)

rem -----------------------------------------------------------------------------------------
rem Installation process.
rem -----------------------------------------------------------------------------------------
:InstallGoddamnSDK
echo Installing the GoddamnSDK..
echo Please, do not close this window.
echo.

rem Checking whether script if executed from the source directory.. 
if exist "source" if exist "utilities" if exist "SetupGoddamnSDK.bat" goto DirectoryTestPassed
echo Installation script should be located and executed directly from the SDK root path.
goto ExitOnFailure
:DirectoryTestPassed

rem Checking whether all external data (that does not come from GIT) is installed..
if exist "source\GoddamnEngine\Engine\_Dependencies\mcpp" goto ExternalDataInstallationTestPassed
echo Missing files that should be extracted from the extra archive that comes with SDK.
goto ExitOnFailure
:ExternalDataInstallationTestPassed

rem Checking whether Visual Studio 2014 / 2013 (with November CTP) is installed..
if not "%VS140COMNTOOLS%" == "" ( 
	set "GODDAMN_SDK_COMPILER_VS=2014"
	set "GODDAMN_SDK_COMPILER_VSN=14"
	set "GODDAMN_SDK_COMPILER_VST=%VS140COMNTOOLS%"
) else (
	if not "%VS120COMNTOOLS%" == "" (
		rem Visual Studio 2013 default compiler does not support C++11. We need the extended one.
		if exist "%VS120COMNTOOLS%\..\..\..\Microsoft Visual C++ Compiler Nov 2013 CTP" (
			set "GODDAMN_SDK_COMPILER_VS=2013"
			set "GODDAMN_SDK_COMPILER_VSN=12"
			set "GODDAMN_SDK_COMPILER_VST=%VS120COMNTOOLS%"
		) else (
			echo Found Visual Studio 2013 installation, but no "Visual C++ Compiler Nov 2013 CTP" was located. Is it installed?
			goto ExitOnFailure
		)
	) else (
		echo No suitable compiler/IDE was located on this machine. Is Visual Studio 2014/2013 installed?
		goto ExitOnFailure
	)
)

rem Setting up debug visualizers for Visual Studio..
copy /y "%GODDAMN_SDK%\utilities\Natvis\GoddamnTemplateLibrary.natvis" "%USERPROFILE%\My Documents\Visual Studio %GODDAMN_SDK_COMPILER_VS%\Visualizers\" 1>nul
if %ERRORLEVEL% == 0 (
	reg add "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\Debugger" /v "EnableNatvisDiagnostics" /t REG_DWORD /d "00000001" /f 1>nul
	if not %ERRORLEVEL% == 0 (
		echo Failed to enable debugging in Visualizers.
		goto ExitOnFailure
	)
) else (
	echo Failed to install debug visualizers for Visual Studio.
	goto ExitOnFailure
)

rem Setting up environment..
if "%GODDAMN_SDK%" == "" (
	setx GODDAMN_SDK %CD%\ 1>nul	
	if not %ERRORLEVEL% == 0 (
		echo Failed to setup the evronmental variable.
		goto ExitOnFailure
	)
	
	rem SETX result would be available since next script launch. We need to set it up manually for now.
	set GODDAMN_SDK=%CD%\
)

rem Building the GoddamnBuildSystem..
call "%GODDAMN_SDK_COMPILER_VST%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" 1>nul
msbuild /nologo /verbosity:quiet %GODDAMN_SDK%\source\ThirdPatry\GoddamnBuildSystem\GoddamnBuildSystem.csproj /property:Configuration=Release /property:Platform=AnyCPU 1>nul
if not %ERRORLEVEL% == 0 (
	echo Failed to compile GoddamnBuildSystem.
	goto ExitOnFailure
)

rem Generating solution files..
call "%GODDAMN_SDK%\bin\ThirdParty\GoddamnBuildSystem.exe" %* 1>nul
if not %ERRORLEVEL% == 0 (
	echo.
	echo GoddamnBuildSystem failed.
	goto ExitOnFailure
)

goto ExitOnSuccess

rem -----------------------------------------------------------------------------------------
rem Removal process.
rem -----------------------------------------------------------------------------------------
:RemoveGoddamnSDK
echo Removing the GoddamnSDK..
echo Please, do not close this window.

rem Removing the Visual Studio's visualizers..
if not "%VS140COMNTOOLS%" == "" ( 
	del "%USERPROFILE%\My Documents\Visual Studio 2014\Visualizers\GoddamnTemplateLibrary.natvis" 1>nul
	reg delete "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\14.0_Config\Debugger" /v "EnableNatvisDiagnostics" /f 1>nul
) else (
	if not "%VS120COMNTOOLS%" == "" (
		del "%USERPROFILE%\My Documents\Visual Studio 2013\Visualizers\GoddamnTemplateLibrary.natvis" 1>nul
		reg delete "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\Debugger" /v "EnableNatvisDiagnostics" /f 1>nul
	) 
)

rem Removing our environmental variables.
if "%GODDAMN_SDK%" == "" (
	reg delete "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v GODDAMN_SDK /f 1>nul
)

echo Now you can simply delete this folder to completely remove the engine SDK.
goto ExitOnSuccess

rem -----------------------------------------------------------------------------------------
rem Exit points.
rem -----------------------------------------------------------------------------------------

:ExitOnFailure
echo Installer failed!
echo Press any key to exit installer...
pause 1>nul
exit 1

:ExitOnSuccess
echo Installer succeed! 
echo Press any key to exit installer...
pause 1>nul
rem exit 0

:ExitOnEndOfScript
