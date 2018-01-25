#!/bin/bash
# ==========================================================================================
# Copyright (C) Goddamn Industries 2018. All Rights Reserved.
# 
# This software or any its part is distributed under terms of Goddamn Industries End User
# License Agreement. By downloading or using this software or any its part you agree with 
# terms of Goddamn Industries End User License Agreement.
# ==========================================================================================

# -----------------------------------------------------------------------------------------
# Exit points.
# -----------------------------------------------------------------------------------------

# Arguments: none. Returns: none.
function ExitOnFailure {
	printf "Installer failed!\n"
	printf "Press any key to exit installer...\n"
	read
	exit 1
}

# Arguments: none. Returns: none.
function ExitOnSuccess {
	printf "Installer succeed!\n"
	printf "Press any key to exit installer...\n"
	read
	exit 0
}

# -----------------------------------------------------------------------------------------
# Installation process.
# -----------------------------------------------------------------------------------------

# Arguments: none. Returns: none.
function InstallGoddamnSDK {
	printf "Installing the GoddamnSDK..\n"
	printf "Please, do not close this window.\n"
	printf "\n"
	
	# Checking whether script if executed from the source directory.. 
	if [[ (! -d "./Source") || (! -d "Utilities") ||  (! -f "SetupGoddamnSDK.command") ]]; then 
		printf "Installation script should be located and executed directly from the SDK root path.\n"
		ExitOnFailure
	fi

	# Checking whether all external data (that does not come from GIT) is installed..
	#if [[ ! -d "./source/GoddamnEngine/Engine/Dependencies/mcpp" ]]; then
		#printf "Missing files that should be extracted from the extra archive that comes with SDK.\n"
		#ExitOnFailure
	#fi
	
	# Checking whether Mono is installed..
	mono --help > /dev/null
	if [[ $? != 0 ]]; then
		printf "Mono was not located on this machine. Is it installed?\n"
		ExitOnFailure
	fi

	# Checking whether MSBuild or XBuild is installed..
	msbuild /help > /dev/null
	if [[ $? == 0 ]]; then
		XMSBUILD=msbuild
	else
		xbuild /help > /dev/null
		if [[ $? == 0 ]]; then
			XMSBUILD=xbuild
		else
			printf "No MSBuild nor XBuild were not located on this machine. Is it installed?\n"
			ExitOnFailure
		fi
	fi
	
	# Building the GoddamnBuildSystem..
	$XMSBUILD /nologo /verbosity:quiet "./Source/Projects/GoddamnBuildSystem/GoddamnBuildSystem.csproj" /property:Configuration=Release /property:Platform=AnyCPU > /dev/null
	if [[ $? != 0 ]]; then
		printf "Failed to compile GoddamnBuildSystem.\n"
		ExitOnFailure
	fi
	
	# Generating solution files..
	mono "./bin/Tools/GoddamnBuildSystem.exe" $@ > /dev/null
	if [[ $? != 0 ]]; then
		printf "GoddamnBuildSystem failed.\n"
		ExitOnFailure
	fi
	
	ExitOnSuccess
}

# -----------------------------------------------------------------------------------------
# Removal process.
# -----------------------------------------------------------------------------------------

# Arguments: none. Returns: none.
function RemoveGoddamnSDK {
	printf "Removing the GoddamnSDK..\n"
	printf "Please, do not close this window.\n"
	
	#
	# Nothing to do here!
	#
	
	printf "Now you can simply delete this folder to completely remove the engine SDK.\n"
	ExitOnSuccess
}

# Parsing command-line arguments..
if [[ $1 == "---help" ]]; then 
	printf "Installation/Removal script for the Goddamn SDK [Linux/OS X Edition].\n"
	printf "Usage: SetupGoddamnSDK.sh [property] [other properties...]\n"
	printf "Property:\n"
	printf " ---help   - Prints this help.\n"
	printf " ---remove - Reverts the changes that were done to system by installer script.\n"
	printf "Other properties are passed directly to the GoddamnBuildSystem executable.\n"
	printf "If no properties were specified - just performs installation.\n"
else
	clear
	if [[ $1 == "---remove" ]]; then
		RemoveGoddamnSDK
	else
		InstallGoddamnSDK
	fi
fi
