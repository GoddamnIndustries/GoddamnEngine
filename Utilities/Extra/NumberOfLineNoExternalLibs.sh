##########################################################################
# NumberOfLineNoExternalLibs.sh - computes number of code line in
# engine (not including source code of external libraries).
# Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
# 
# History:
#		* 04.07.2014 - Created by James Jhuighuy.
##########################################################################

./ThirdPartyExecutables/cloc-1.62 ./../../source/* --exclude-dir="Dependencies","DLLResources","Build"
read -rsp 'Press enter to continue...'
