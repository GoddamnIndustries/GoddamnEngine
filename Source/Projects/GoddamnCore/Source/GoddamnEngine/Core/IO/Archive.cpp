// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/Archive.cpp
 * File contains archive interfaces.
 */
#include <GoddamnEngine/Core/IO/Archive.h>
#include <GoddamnEngine/Core/IO/MemoryStream.h>
#include <GoddamnEngine/Core/IO/Paths.h>
#include <GoddamnEngine/Core/IO/FileUtilities.h>
#include <GoddamnEngine/Core/Misc/Endian.h>
#include <GoddamnEngine/Core/Concurrency/CriticalSection.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Map.h>

#include <zlib.h>

GD_NAMESPACE_BEGIN

	typedef Vector<Byte> ByteVector;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                              ZLib Compression API.                               ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GDINT static bool ZlibCompress(ByteVector& compressed, ByteVector const& uncompressed)
	{
		return false;
	}

	GDINT static bool ZlibUncompress(ByteVector const& compressed, ByteVector& uncompressed)
	{
		return false;
	}

	GDINL static CStr ZlibGetError(int result)
	{
		switch (result)
		{
			case Z_STREAM_ERROR:
				return "Z_STREAM_ERROR";
			case Z_DATA_ERROR:
				return "Z_DATA_ERROR";
			case Z_MEM_ERROR:
				return "Z_MEM_ERROR";
			case Z_BUF_ERROR:
				return "Z_BUF_ERROR";
			case Z_VERSION_ERROR:
				return "Z_VERSION_ERROR";
		}
		return "unknown";
	}

	GDINL static UInt32 ZLibCrc32(CHandle const data, SizeTp const dataLength)
	{
		return 0;
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                          ArchiveImplementation class.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	#define GD_ARCHIVE_PACK_PUSH __pragma(pack(push, 1))
	#define GD_ARCHIVE_PACK_POP  __pragma(pack(pop))

	typedef Byte ARCHIVE_HEADER_FORMAT[4];

	/*!
	 * Represents magic constant for header for all versions of archives.
	 * @warning Never modify value of this magic constant. It defines archive file format.
	 */
	GDINT static ARCHIVE_HEADER_FORMAT const ARCHIVE_HEADER_FORMAT_VALUE = { 'G', 'D', 'A', 'R' };

	/*!
	 * Represents header information for the archive file.
	 * @warning Never modify layout of this struct. It defines archive file format.
	 */
	GD_ARCHIVE_PACK_PUSH;
	struct ARCHIVE_HEADER
	{
		ARCHIVE_HEADER_FORMAT HeaderFormat;
		UInt32 HeaderMagic;
	};	// struct ARCHIVE_HEADER
	GD_ARCHIVE_PACK_POP;

	GDINL static bool ArchiveReadAndValidateHeader(ARCHIVE_HEADER* const archiveHeader, SharedPtr<InputStream> const archiveInputStream)
	{
		if (!archiveInputStream->Read(*archiveHeader))
		{
			Debug::LogError("Error while loading archive (v1): failed to read archive header.");
			return false;
		}

		/*if (union_cast<UInt32>(archiveHeader->HeaderFormat) != union_cast<UInt32>(ARCHIVE_HEADER_FORMAT_VALUE))
		{
			return false;
		}*/
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                              ArchiveVersion1 class.                              ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Represents magic constant for version 1 of archives.
	 * @warning Never modify value of this magic constant. It defines archive file format.
	 */
	GDINT static UInt32 const ARCHIVE_HEADER_MAGIC_VER1 = 0x53FEA701;

	GDINL static bool ArchiveReadAndValidateHeaderVer1(ARCHIVE_HEADER* const archiveHeader, SharedPtr<InputStream> const archiveInputStream)
	{
		if (ArchiveReadAndValidateHeader(archiveHeader, archiveInputStream))
		{
		}
		return false;
	}

	/*!
	 * Represents single file information for the archive file (version 1).
	 * @warning Never modify layout of this struct. It defines archive file format.
	 */
	GD_ARCHIVE_PACK_PUSH;
	struct ARCHIVE_FILE_VER1
	{
		HashCode FileNameHashCode;
		UInt32   FileLength;
		UInt32   FileOffset;
	};	// struct ARCHIVE_FILE_VER1
	GD_ARCHIVE_PACK_POP;

	/*!
	 * Represents dictionary information for the archive file (version 1).
	 * @warning Never modify layout of this struct. It defines archive file format.
	 */
	GD_ARCHIVE_PACK_PUSH;
	struct ARCHIVE_DICTIONARY_VER1
	{
		UInt32 DictionaryCrc32;
		UInt32 DictionaryLength;
		UInt32 DictionaryReserved[6];	// Reserved for future use (multiple dictionaries perhaps). Should be 0.
		ARCHIVE_FILE_VER1 Dictionary[256];
	};	// struct ARCHIVE_DICTIONARY_VER1
	GD_ARCHIVE_PACK_POP;

	// ------------------------------------------------------------------------------------------
	// Archive Readers.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Represents readers for version 1 of the Goddamn Archives.
	 * @warning Never modify behavior of this class. It defines archive using of archive file format.
	 */
	class ArchiveReaderVer1 final : public ArchiveReader
	{
		struct ArchiveReaderFile
		{
			UInt32     FileLength;
			UInt32     FileOffset;
			ByteVector FileData;
		};	// struct ArchiveReaderFile

	private:
		bool                             m_ArchiveIsValid;
		SharedPtr<InputStream>           m_ArchiveInputStream;
		Map<HashCode, ArchiveReaderFile> m_ArchiveFiles;
		CriticalSection                  m_ArchiveFilesLock;

	public:

		/*!
		 * Parses header and dictionary of the archive.
		 * @param archiveInputStream Input stream.
		 */
		GDINT explicit ArchiveReaderVer1(SharedPtr<InputStream> const archiveInputStream)
			: m_ArchiveInputStream(archiveInputStream)
		{
			GD_DEBUG_VERIFY(archiveInputStream != nullptr, "Null pointer input stream specified.");
			GD_DEBUG_VERIFY(archiveInputStream->IsValid(), "Invalid input stream specified.");

			// Reading and validating header of the archive.
			ARCHIVE_HEADER archiveHeader = {};
			if (!m_ArchiveInputStream->Read(archiveHeader))
			{
				Debug::LogError("Error while loading archive (v1): failed to read archive header.");
				m_ArchiveIsValid = false;
			}
			else
			{
				// No need to print correct header format or magic number on the error.
				// We must hide all the magic constants to prevent reverse-engineering. 
				if (/*archiveHeader.HeaderFormat != ARCHIVE_HEADER_FORMAT ||*/ archiveHeader.HeaderMagic != ~ARCHIVE_HEADER_MAGIC_VER1)
				{
					Debug::LogError("Error while loading archive (v1): corrupt archive header.");
					m_ArchiveIsValid = false;
				}
			}

			if (m_ArchiveIsValid)
			{
				// Reading and validating dictionary of the archive.
				ARCHIVE_DICTIONARY_VER1 archiveDictionary = {};
				if (!m_ArchiveInputStream->Read(archiveDictionary))
				{
					Debug::LogError("Error while loading archive (v1): failed to read archive dictionary.");
					m_ArchiveIsValid = false;
				}
				else
				{
					// Validating check sum of the dictionary.
					auto const archiveCrc32 = ZLibCrc32(archiveDictionary.Dictionary, sizeof archiveDictionary.Dictionary);
					if (archiveDictionary.DictionaryCrc32 != archiveCrc32)
					{
						Debug::LogError("Error while loading archive (v1): corrupt archive dictionary.");
						m_ArchiveIsValid = false;
					}
				}

				if (m_ArchiveIsValid)
				{
					// Registering files inside local table.
					for (UInt32 cnt = 0; cnt < archiveDictionary.DictionaryLength; ++cnt)
					{
						auto const& archiveDictionaryFile = archiveDictionary.Dictionary[cnt];
						m_ArchiveFiles.Insert(archiveDictionaryFile.FileNameHashCode, { archiveDictionaryFile.FileLength, archiveDictionaryFile.FileOffset });
					}
				}
			}
		}

	public:

		/*!
		 * Returns true of this archive is valid.
		 */
		GDINT virtual bool IsValid() const override final
		{
			return m_ArchiveIsValid;
		}

		/*!
		 * @brief Opens the input stream that can read from this archive.
		 * Each file could be opened for reading multiple times.
		 *
		 * @param fileName Name of the file being opened.
		 * @returns Pointer to opened stream or nullptr on error.
		 */
		GDINT virtual SharedPtr<InputStream> OpenInputStream(WideString const& fileName) override final
		{
			GD_DEBUG_VERIFY(IsValid(), "Reading from the invalid archive.");

			ScopedCriticalSection archiveFilesLock(m_ArchiveFilesLock);
			auto const& archiveFile = m_ArchiveFiles[fileName.GetHashCode()];
			if (archiveFile.FileData.IsEmpty())
			{
				
			}
		}

	};	// class ArchiveReaderVer1

GD_NAMESPACE_END
