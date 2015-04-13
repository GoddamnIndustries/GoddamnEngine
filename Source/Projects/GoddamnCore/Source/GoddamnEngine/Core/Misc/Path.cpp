//! ==========================================================================================
//! Path.cpp - Path management utility implementation.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#include <GoddamnEngine/Core/Misc/Path.h>

#if GD_PLATFORM_WINDOWS
#	include <Windows.h>
#endif	// if (defined(GD_PLATFORM_WINDOWS))
#include <cstdio>

GD_NAMESPACE_BEGIN

	GDINT static String ChangeExtension(String const& SomePath, String const& NewExtension);
	GDINT static String Combine(Vector<String> const& Paths);
	GDINT static bool HasExtension(String const& SomePath);
	GDINT static String GetExtension(String const& SomePath);

	SizeTp GetLastSlashLocation(String const& SomePath)
	{
		for (auto const& Character : Reverse(SomePath)) {
			if ((Character == Char('\\')) || (Character == Char('/'))) {
				return (&Character - &(*SomePath.Begin()) + 1);
			}
		}

		return SizeTpMax;
	}

	String Path::GetDirectoryName(String const& SomePath)
	{
		SizeTp const LastSlashLocation = GetLastSlashLocation(SomePath);
		return ((LastSlashLocation != SizeTpMax) ? SomePath.Substring(0, LastSlashLocation) : SomePath);
	}
		
	String Path::GetFileName(String const& SomePath)
	{
		SizeTp const LastSlashLocation = GetLastSlashLocation(SomePath);
		return ((LastSlashLocation != SizeTpMax) ? SomePath.Substring(LastSlashLocation) : SomePath);
	}

	String Path::GetDirectoryAndFileNameWithoutExtension(String const& SomePath)
	{
		SizeTp const Index = SomePath.ReverseFind('.');
		if (Index == SizeTpMax) {
			return SomePath;
		}

		return SomePath.Substring(0, Index - 1);
	}

	String Path::GetFileNameWithoutExtension(String const& SomePath)
	{
		return Path::GetDirectoryAndFileNameWithoutExtension(Path::GetFileName(SomePath));
	}

	String Path::GetTemporaryFileName()
	{
		String TemporaryFileName(L_tmpnam, '\0');
		GD_ASSERT(tmpnam(TemporaryFileName.CStr()) != nullptr, "Failed to create temporary file name.");
		return Path::GetTemporaryPath() + TemporaryFileName;
	}
		
	String const& Path::GetTemporaryPath()
	{
		String static const TemporaryPath([]() -> Char const* {
#if GD_PLATFORM_WINDOWS
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

GD_NAMESPACE_END
