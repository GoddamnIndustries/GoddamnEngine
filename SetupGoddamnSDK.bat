echo off
rem =========================================================================================
rem SetupGoddamnSDK.batinstalls Goddamn Software Development Kit.
rem Copyright (C) $(GODDAMN_DEV) 2011Present. All Rights Reserved.
rem
rem History:
rem		* 03.11.2014 - Created by James Jhuighuy.
rem =========================================================================================

cls
if "%1" == "--remove" ( 
	goto RemoveGoddamnSDK
) else (
	goto InstallGoddamnSDK
)

rem -----------------------------------------------------------------------------------------
rem Installation process.
rem -----------------------------------------------------------------------------------------
:InstallGoddamnSDK
echo Installing the GoddamnSDK..

rem Checking for Visual Studio 2014 / 2013 (with November CTP) been installed..
if not "%VS140COMNTOOLS%" == "" ( 
	set GODDAMN_SDK_COMPILER_VS=2014
	set GODDAMN_SDK_COMPILER_VST="%VS140COMNTOOLS%"
) else (
	if not "%VS120COMNTOOLS%" == "" (
		rem Visual Studio 2013 default compiler does not supports C++11. We need extended one.
		if exist "%VS120COMNTOOLS%/../../../Microsoft Visual C++ Compiler Nov 2013 CTP" (
			set GODDAMN_SDK_COMPILER_VS=2013
			set GODDAMN_SDK_COMPILER_VST="%VS120COMNTOOLS%"
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
copy /Y "%GODDAMN_SDK%utilities\Natvis\GoddamnTemplateLibrary.natvis" "%USERPROFILE%\My Documents\Visual Studio %GODDAMN_SDK_COMPILER_VS%\Visualizers\" 1>nul 2>nul
if %ERRORLEVEL% == 0 (
	reg add "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\Debugger" /V "EnableNatvisDiagnostics" /T REG_DWORD /D "00000001" 1>nul 2>nul
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
	setx GODDAMN_SDK %CD%\ 1>nul 2>nul	
	if not %ERRORLEVEL% == 0 (
		echo Failed to setup the evronmental variable.
		goto ExitOnFailure
	)
	
	rem SETX result would be available since next script launch. We need to set it up manually for now.
	set GODDAMN_SDK=%CD%\
)

echo Building the GoddamnBuildSystem..
call "%GODDAMN_SDK_COMPILER_VST%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" 1>nul 2>nul
msbuild /nologo /verbosity:quiet %GODDAMN_SDK%source\ThirdParty\GoddamnBuildSystem\GoddamnBuildSystem.csproj /property:Configuration=Release /property:Platform=AnyCPU
if not %ERRORLEVEL% == 0 (
	echo Failed to compile GoddamnBuildSystem.
	goto ExitOnFailure
)

echo Generating solution files..

echo.
echo Now you can open the generated solution file and compile the engine SDK.
goto ExitOnSuccess

rem -----------------------------------------------------------------------------------------
rem De-installation process.
rem -----------------------------------------------------------------------------------------
:RemoveGoddamnSDK
echo Removing the GoddamnSDK..

echo Not implemented.
goto ExitOnFailure

echo.
echo Now you can simply delete this folder to completely deinstall the engine SDK.
goto ExitOnSuccess

rem -----------------------------------------------------------------------------------------
rem Finalizers.
rem -----------------------------------------------------------------------------------------

:ExitOnFailure
echo.
echo Installation failed! Press any key to exit installer...
pause 1>nul 2>nul
exit 1

:ExitOnSuccess
echo.
echo Installation succeded! Press any key to exit installer...
pause 1>nul 2>nul
exit 0
