cmake_minimum_required (VERSION 2.8)
project(Sample01)
include_directories(H:/GoddamnEngine/source/Projects/Samples/01_SimpleScene/Source
	H:/GoddamnEngine/Source/Projects/GoddamnCore/Source
	H:/GoddamnEngine/Source/Projects/GoddamnEngine/Source
	)
add_executable(Sample01
	H:/GoddamnEngine/source/Projects/Samples/01_SimpleScene/Source/demon_image.h
	H:/GoddamnEngine/source/Projects/Samples/01_SimpleScene/Source/dirent.h
	H:/GoddamnEngine/source/Projects/Samples/01_SimpleScene/Source/SimpleScene.cpp
	)
