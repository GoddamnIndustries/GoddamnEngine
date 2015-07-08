@echo off
rem =========================================================================================
rem LocateVisualStudio.bat - locates installed instance of Visual Studio.
rem Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
rem =========================================================================================

rem Checking whether Visual Studio 2015 / 2013 (with November CTP) is installed..
if not "%VS141ComnTools%" == "" ( 
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
			exit 1
		)
	) else (
		echo No suitable compiler/IDE was located on this machine. Is Visual Studio 2015 / 2013 installed?
		exit 1
	)
)