#!/bin/bash
# =========================================================================================
# SetupGoddamnSDK.bat - installs the Goddamn Software Development Kit [Linux/OS X Edition].
# Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
#
# History:
#		* 08.12.2014 - Created by James Jhuighuy.
# =========================================================================================

# -----------------------------------------------------------------------------------------
# Exit points.
# -----------------------------------------------------------------------------------------

function ExitOnFailure {
	printf "Installer failed!\n"
	printf "Press any key to exit installer...\n"
	read
	exit 1
}

function ExitOnSuccess {
	printf "Installer succeed!\n"
	printf "Press any key to exit installer...\n"
	read
	exit 0
}

# -----------------------------------------------------------------------------------------
# Installation process.
# -----------------------------------------------------------------------------------------
function InstallGoddamnSDK {
	printf "Installing the GoddamnSDK..\n"
	printf "Please, do not close this window.\n"
	printf "\n"
	
	# Checking whether script if executed from the source directory.. 
	if [[ (! -d "./source") || (! -d "utilities") ||  (! -f "SetupGoddamnSDK.sh") ]]; then 
		printf "Installation script should be located and executed directly from the SDK root path.\n"
		ExitOnFailure
	fi

	# Checking whether all external data (that does not come from GIT) is installed..
	if [[ ! -d "./source/GoddamnEngine/Engine/_Dependencies/mcpp" ]]; then
		printf "Missing files that should be extracted from the extra archive that comes with SDK.\n"
		ExitOnFailure
	fi
	
	# Determining current operating system..
	if [[ "$(uname)" == "Darwin" ]]; then
		OperatingSystem="OSX"
	elif [[ "$(expr substr $(uname -s) 1 5)" == "Linux" ]]; then
		OperatingSystem="Linux"
	else
		printf "Running under unknown operating system.\n"
		ExitOnFailure
	fi
	
	echo $OperatingSystem
	ExitOnSuccess
}

# -----------------------------------------------------------------------------------------
# Removal process.
# -----------------------------------------------------------------------------------------
function RemoveGoddamnSDK {
	printf "Removing the GoddamnSDK..\n"
	printf "Please, do not close this window.\n"
	printf "\n"
	
	printf "Now you can simply delete this folder to completely remove the engine SDK.\n"
	ExitOnSuccess
}

# Parsing command-line arguments..
if [[ $1 == "---help" ]]; then 
	printf "Installation/Removal script for the Goddamn SDK [Linux/OS X Edition].\n"
	printf "Usage: SetupGoddamnSDK.bat [property] [other properties...]\n"
	printf "Property:\n"
	printf " ---help   - Prints this help.\n"
	printf " ---remove - Removes the SDK.\n"
	printf "Other properties are passed directly to the GoddamnBuildSystem executable.\n"
else
	clear
	if [[ $1 == "---remove" ]]; then
		RemoveGoddamnSDK
	else
		InstallGoddamnSDK
	fi
fi
