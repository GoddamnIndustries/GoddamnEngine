//////////////////////////////////////////////////////////////////////////
/// Path.cc - Path management utility implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Core/IO/Path/Path.hh>

#if (defined(GD_PLATFORM_WINDOWS))
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif	// if (defined(GD_PLATFORM_WINDOWS))
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
			return Path::GetTemporaryPath() + TemporaryFileName;
		}
		
		extern String const& GetTemporaryPath()
		{
			String static const TemporaryPath([]() -> Char const* {
#if (defined(GD_PLATFORM_WINDOWS))
				char static TemporaryPath[MAX_PATH];
				DWORD const static Result = GetTempPathA(MAX_PATH, &TemporaryPath[0]);
				GD_ASSERT(Result != 0, "Failed to get temporary dir path");
				return TemporaryPath;
#else	// if (defined(GD_PLATFORM_WINDOWS))
				char const* const TemporaryPath = getenv("TMPDIR");
				if (TemporaryPath == 0)
					return "/tmp";
				return TemporaryPath;
#endif	// if (defined(GD_PLATFORM_WINDOWS))
			}());

			return TemporaryPath;
		}
	};	// namespace Path

GD_NAMESPACE_END
