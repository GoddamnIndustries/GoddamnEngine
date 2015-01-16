/// ==========================================================================================
/// Path.cpp - Path management utility implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 27.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/IO/Path/Path.h>

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

		extern String GetExtension(String const& SomePath);

		extern size_t GetLastSlashLocation(String const& SomePath)
		{
			for (auto const& Character : Reverse(SomePath)) {
				if ((Character == Char('\\')) || (Character == Char('/'))) {
					return (&Character - &(*SomePath.Begin()) + 1);
				}
			}

			return SIZE_MAX;
		}

		extern String GetDirectoryName(String const& SomePath)
		{
			size_t const LastSlashLocation = Path::GetLastSlashLocation(SomePath);
			return ((LastSlashLocation != SIZE_MAX) ? SomePath.Substring(0, LastSlashLocation) : SomePath);
		}
		
		extern String GetFileName(String const& SomePath)
		{
			size_t const LastSlashLocation = Path::GetLastSlashLocation(SomePath);
			return ((LastSlashLocation != SIZE_MAX) ? SomePath.Substring(LastSlashLocation) : SomePath);
		}

		extern String GetDirectoryAndFileNameWithoutExtension(String const& SomePath)
		{
			size_t const Index = SomePath.ReverseFind('.');
			if (Index == SIZE_MAX) {
				return SomePath;
			}

			return SomePath.Substring(0, Index - 1);
		}

		extern String GetFileNameWithoutExtension(String const& SomePath)
		{
			return Path::GetDirectoryAndFileNameWithoutExtension(Path::GetFileName(SomePath));
		}

		extern String GetTemporaryFileName()
		{
			String TemporaryFileName(L_tmpnam, '\0');
			GD_ASSERT(tmpnam(TemporaryFileName.CStr()) != nullptr, "Failed to create temporary file name.");
			return Path::GetTemporaryPath() + TemporaryFileName;
		}
		
		extern String const& GetTemporaryPath()
		{
			String static const TemporaryPath([]() -> Char const* {
#if (defined(GD_PLATFORM_WINDOWS))
				char static TemporaryPath[MAX_PATH];
				DWORD const static Result = GetTempPathA(MAX_PATH, &TemporaryPath[0]);
				GD_ASSERT(Result != 0, "Failed to get temporary directory path");
				return TemporaryPath;
#else	// if (defined(GD_PLATFORM_WINDOWS))
				char const* const TemporaryPath = getenv("TMPDIR");
				if (TemporaryPath == nullptr) {
					return "/tmp";
				}
				return TemporaryPath;
#endif	// if (defined(GD_PLATFORM_WINDOWS))
			}());

			return TemporaryPath;
		}
	};	// namespace Path

GD_NAMESPACE_END
