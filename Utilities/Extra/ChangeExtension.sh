# ==========================================================================================
# ChangeExtension.sh - recursively changes extension of files.
# Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
# 
# History:
#		* 21.07.2014 - Created by James Jhuighuy
# ==========================================================================================

for X in `find . -name "*.$1"` 
do
	mv $X ${X/.$1/.$2}
done
