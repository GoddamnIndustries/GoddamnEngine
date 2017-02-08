// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/DirectoryWindows.h
 * File contains Windows directory interface.
 */
#pragma once
#if !defined(GD_INSIDE_DIRECTORY_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/IO/Directory.h> instead.
#endif	// if !defined(GD_INSIDE_DIRECTORY_H)

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Windows directory visitor interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class DirectoryWindows : public DirectoryGeneric
	{
	private:
		WideCStr         m_DirPath;
		bool             m_DirFirstRead;
		HANDLE           m_DirHandle;
		WIN32_FIND_DATAW m_DirData;

	public:

		/*!
		 * Initializes a generic directory visitor.
		 * @param directoryPath Path to the directory.
		 */
		GDINT explicit DirectoryWindows(WideCStr const directoryPath)
			: DirectoryGeneric(directoryPath), m_DirPath(directoryPath), m_DirFirstRead(false), m_DirHandle(INVALID_HANDLE_VALUE)
		{
			Rewind();
		}

		GDINL ~DirectoryWindows()
		{
			Close();
		}

	public:

		/*!
		 * Returns true if this directory visitor is valid and ready for I/O operations.
		 */
		GDINL bool IsValid() const
		{
			return m_DirHandle != INVALID_HANDLE_VALUE;
		}

		/*!
		 * Closes this visitor and releases all resources associated with it.
		 */
		GDAPI void Close()
		{
			if (m_DirHandle != INVALID_HANDLE_VALUE)
			{
				GD_VERIFY(FindClose(m_DirHandle) == TRUE, "'FindClose' has failed.");
				m_DirHandle = INVALID_HANDLE_VALUE;
			}
		}

		/*!
		 * Reposition this visitor to the beginning of the directory.
		 */
		GDAPI void Rewind()
		{
			Close();
			if (WideCString::Strlen(m_DirPath) <= MAX_PATH - 3)
			{
				WCHAR directoryPathWin[MAX_PATH] = {};
				WideCString::Strcpy_s(directoryPathWin, GetLength(directoryPathWin), m_DirPath);
				WideCString::Strcat_s(directoryPathWin, GetLength(directoryPathWin), L"\\*");
				m_DirHandle = FindFirstFileW(directoryPathWin, &m_DirData);
				m_DirFirstRead = false;
			}
		}

		/*!
		 * Reads next entry from the directory.
		 * @returns Next entry byte from stream, or value with null pointer path on error.
		 */
		GDAPI DirectoryEntry ReadEntry()
		{
			GD_DEBUG_VERIFY(IsValid(), "Reading from invalid directory visitor.");
			if (m_DirFirstRead)
			{
				FindNextFileW(m_DirHandle, &m_DirData);
			}
			else
			{
				m_DirFirstRead = true;
			}
			return{ m_DirData.cFileName, (m_DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 };
		}

	};	// class DirectoryWindows

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Cross-platform directory visitor interface.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	using Directory = DirectoryWindows;

GD_NAMESPACE_END
