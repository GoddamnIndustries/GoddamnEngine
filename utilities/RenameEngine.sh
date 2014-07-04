##########################################################################
# RenameEngine.sh - renames the whole motherfucken game engine.
# Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
# 
# History:
#		* 04.07.2014 - Created by James Jhuighuy.
##########################################################################

if [[ "$#" -ne 4 ]]; then
	printf '\nInvalid command line arguments specified' >&2
	printf '\nUsage:' >&2
	printf "\n\t$0 {OldName} {OldPrefix} {NewName} {NewPrefix}" >&2
	printf '\n' >&2
	printf '\n\tOldName   - Old name of the engine. E.g. "GoddamnEngine" > "Goddamn".' >&2
	printf '\n\tOldPrefix - Old prefix of the engine. E.g. "GoddamnEngine" > "GD".' >&2
	printf '\n\tNewName   - New name of the engine. E.g. "RenamedEngine" > "Renamed".' >&2
	printf '\n\tNewPrefix - New prefix of the engine. E.g. "RenamedEngine" > "RN".' >&2
	printf '\nExample:' >&2
	printf "\n\t$0 GoddamnEngine GD RenamedEngine RN" >&2
	exit 1
else
	OLD_NAME=$1
	OLD_PREFIX=$2
	NEW_NAME=$3
	NEW_PREFIX=$4
fi

./ThirdPartyExecutables/fart -q -p -- ./../ $OLD_NAME $NEW_NAME
./ThirdPartyExecutables/fart -q -p -- ./../ $OLD_PREFIX $NEW_PREFIX
