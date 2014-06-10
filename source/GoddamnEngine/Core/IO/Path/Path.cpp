//////////////////////////////////////////////////////////////////////////
/// Path.cpp - Path management utility implementation.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Core/IO/Path/Path.h>
#include <utility>	// For Move

GD_NAMESPACE_BEGIN

	namespace Path
	{
		extern String&& ChangeExtension(String const& SomePath, String const& NewExtension);
		extern String&& Combine(Vector<String> const& Paths);
		extern bool HasExtension(String const& SomePath);
		extern String&& GetDirectoryName(String const& SomePath);
		extern String&& GetExtension(String const& SomePath);
		extern String&& GetFileName(String const& SomePath);
		extern String&& GetFileNameWithoutExtension(String const& SomePath);
		extern String&& GetTemporaryFileName();
		extern String const& GetTemporaryPath();
	};	// namespace Path

GD_NAMESPACE_END
