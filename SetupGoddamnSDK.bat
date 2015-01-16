@echo off
rem =========================================================================================
rem SetupGoddamnSDK.bat - installs the Goddamn Software Development Kit [Windows Edition].
rem Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
rem =========================================================================================

rem Parsing command-line arguments..
if "%1" == "---help" (
	echo Installation/Removal script for the Goddamn SDK [Windows Edition].
	echo Usage: SetupGoddamnSDK.bat [property] [other properties...]
	echo Property:
	echo  ---help   - Prints this help.
	echo  ---remove - Reverts the changes that were done to system by installer script.
	echo Other properties are passed directly to the GoddamnBuildSystem executable.
	echo If no properties were specified - just performs installation.
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
if exist "Source" if exist "Utilities" if exist "SetupGoddamnSDK.bat" set "DirectoryTestPassed=True"
if "%DirectoryTestPassed%" == "" (
	echo Installation script should be located and executed directly from the SDK root path.
	goto ExitOnFailure
)

rem Checking whether all external data (that does not come from GIT) is installed..
if exist "Source\Projects\GoddamnEngine\Dependencies\mcpp" set "ExternalDataInstallationTestPassed=True"
if "%DirectoryTestPassed%" == "" (
	echo Missing files that should be extracted from the extra archive that comes with SDK.
	goto ExitOnFailure
)

rem Checking whether Visual Studio 2015 / 2013 (with November CTP) is installed..
if not "%VS140ComnTools%" == "" ( 
	rem Visual Studio 2015 has a full support of C++11.
	set "VSCompilerYear=2015"
	set "VSCompilerVersion=14"
	set "VSCompilerTools=%VS140ComnTools%"
) else (
	if not "%VS120ComnTools%" == "" (
		rem Visual Studio 2013 default compiler does not support C++11. We need the extended one.
		if exist "%VS120ComnTools%\..\..\..\Microsoft Visual C++ Compiler Nov 2013 CTP" (
			set "VSCompilerYear=2013"
			set "VSCompilerVersion=12"
			set "VSCompilerTools=%VS120ComnTools%"
		) else (
			echo Found Visual Studio 2013 installation, but no "Visual C++ Compiler Nov 2013 CTP" was located. Is it installed?
			goto ExitOnFailure
		)
	) else (
		echo No suitable compiler/IDE was located on this machine. Is Visual Studio 2015 / 2013 installed?
		goto ExitOnFailure
	)
)

rem Setting up debug visualizers for Visual Studio..
mkdir "%UserProfile%\My Documents\Visual Studio %VSCompilerYear%\Visualizers\" 1>nul 2>nul
copy /y ".\utilities\Natvis\GoddamnTemplateLibrary.natvis" "%UserProfile%\My Documents\Visual Studio %VSCompilerYear%\Visualizers\" 1>nul 2>nul
if %ErrorLevel% == 0 (
	reg add "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\%VSCompilerVersion%.0_Config\Debugger" /v "EnableNatvisDiagnostics" /t REG_DWORD /d "00000001" /f 1>nul 2>nul
	if not %ErrorLevel% == 0 (
		echo Failed to enable debugging in Visualizers.
		goto ExitOnFailure
	)
) else (
	echo Failed to install debug visualizers for Visual Studio.
	goto ExitOnFailure
)

rem Building the GoddamnBuildSystem..
call "%VSCompilerTools%\..\..\VC\bin\x86_amd64\vcvarsx86_amd64.bat" 1>nul
msbuild /nologo /verbosity:quiet ".\source\Projects\GoddamnBuildSystem\GoddamnBuildSystem.csproj" /property:Configuration=Release /property:Platform=AnyCPU
if not %ErrorLevel% == 0 (
	echo Failed to compile GoddamnBuildSystem.
	goto ExitOnFailure
)

rem Generating solution files..
call ".\bin\ThirdParty\GoddamnBuildSystem.exe" %* 1>nul
if not %ErrorLevel% == 0 (
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
if not "%VS140ComnTools%" == "" ( 
	del "%UserProfile%\My Documents\Visual Studio 2014\Visualizers\GoddamnTemplateLibrary.natvis" 1>nul
	reg delete "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\14.0_Config\Debugger" /v "EnableNatvisDiagnostics" /f 1>nul
) else (
	if not "%VS120ComnTools%" == "" (
		del "%UserProfile%\My Documents\Visual Studio 2013\Visualizers\GoddamnTemplateLibrary.natvis" 1>nul
		reg delete "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\12.0_Config\Debugger" /v "EnableNatvisDiagnostics" /f 1>nul
	) 
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

:ExitOnEndOfScript
