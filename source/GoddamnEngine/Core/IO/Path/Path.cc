//////////////////////////////////////////////////////////////////////////
/// Path.cc - Path management utility implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Core/IO/Path/Path.hh>

#include <utility>	// For Move
#include <cstdio>

GD_NAMESPACE_BEGIN

	namespace Path
	{
		extern String ChangeExtension(String const& SomePath, String const& NewExtension);
		extern String Combine(Vector<String> const& Paths);
		extern bool HasExtension(String const& SomePath);
		extern String GetDirectoryName(String const& SomePath);
		extern String GetExtension(String const& SomePath);
		extern String GetFileName(String const& SomePath);
		extern String GetFileNameWithoutExtension(String const& SomePath);
		
		extern String GetTemporaryFileName()
		{
			String TemporaryFileName(nullptr, L_tmpnam);
			GD_ASSERT(tmpnam(TemporaryFileName.CStr()) != nullptr, "Failed to create temporary file name.");
			return TemporaryFileName;
		}
		
		extern String const& GetTemporaryPath();
	};	// namespace Path

GD_NAMESPACE_END
