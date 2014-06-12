#include <GoddamnEngine/Engine/Resource/ResourceLoader/FromFile/ResourceLoaderFromFile.hh>
#include <stdio.h>

#if (defined(GD_PLATFORM_DESKTOP) && ((!defined(GD_PLATFORM_OSX))/* || (defined(GD_))*/))

#if (defined(GD_PLATFORM_WINDOWS) && (defined(GD_PLATFORM_64BIT)))
#	define fseek _fseeki64
#	define ftell _ftelli64
#	define llong  __int64
#endif

GD_NAMESPACE_BEGIN

	struct ResourceLoaderFromFile::ResourceLoaderFromFileInternalFileHandle final : public FILE { };

	ResourceLoaderFromFile::ResourceLoaderFromFile(String const& resourceFilePath, 
		ResourceLoader::ResourceLoaderFlags const resourceLoaderFlags) :
		ResourceLoader(resourceLoaderFlags | (ResourceLoader::ResourceLoaderFlagsSupportsReading 
			| ResourceLoader::ResourceLoaderFlagsSupportsWriting)), fileHandle(nullptr), fileSize(0)
	{
		String resourceFileOpeningMode = "";
		switch (resourceLoaderFlags)
		{
		case (ResourceLoader::ResourceLoaderFlagsReadingEnabled)	: { resourceFileOpeningMode = "rb"; } break;
		case (ResourceLoader::ResourceLoaderFlagsRewritingEnabled)	: { resourceFileOpeningMode = "wb"; } break;
		case (ResourceLoader::ResourceLoaderFlagsAppendingEnabled)	: { resourceFileOpeningMode = "ab"; } break;
		case (ResourceLoader::ResourceLoaderFlagsReadingEnabled
			| ResourceLoader::ResourceLoaderFlagsRewritingEnabled)	: { resourceFileOpeningMode = "wb+"; } break;
		case (ResourceLoader::ResourceLoaderFlagsReadingEnabled
			| ResourceLoader::ResourceLoaderFlagsAppendingEnabled)	: { resourceFileOpeningMode = "ab+"; } break;
		default														: { GD_ASSERT(false, "Unsupported file opening mode"); } break;			
		}

		self->fileHandle = ((ResourceLoaderFromFileInternalFileHandle*)fopen((&resourceFilePath[0]), (&resourceFileOpeningMode[0])));
		self->fileSize = self->GetResourceDataSize();

		GD_ASSERT(self->fileHandle != nullptr, "");
	}

	ResourceLoaderFromFile::~ResourceLoaderFromFile()
	{
		if (self->IsValidResourcePath())
		{
			GD_ASSERT((fclose(self->fileHandle) != EOF),
				"Failed to close file input stream");
		}
	}

	size_t ResourceLoaderFromFile::GetResourceDataSize() const
	{
		if (((((self->Flags & ResourceLoader::ResourceLoaderFlagsWritingEnabled) != 0)
			|| (self->fileSize == 0)) && (self->IsValidResourcePath())))
		{
			llong const index = ftell(self->fileHandle);
			GD_ASSERT((index != -1), "internal error");
			GD_ASSERT((fseek(self->fileHandle, 0, SEEK_END) == 0), "internal error");

			self->fileSize = ((size_t)ftell(self->fileHandle));
			GD_ASSERT((self->fileSize != ((size_t)-1)), "internal error");
			GD_ASSERT((fseek(self->fileHandle, index, SEEK_SET) == 0), "internal error");
		}

		return self->fileSize;
	}

	bool ResourceLoaderFromFile::IsValidResourcePath() const
	{
		return (self->fileHandle != nullptr);
	}

	size_t ResourceLoaderFromFile::Seek(const long seekFromCurrent, const long seekFromStart)
	{
		GD_ASSERT((self->IsValidResourcePath()), 
			"File seeking failed");

		GD_ASSERT((fseek(self->fileHandle, 
			((seekFromStart != -1) ? (seekFromCurrent + seekFromStart) : seekFromCurrent), 
			((seekFromStart != -1) ? SEEK_SET : SEEK_CUR)) == 0), 
			"File seeking failed");

		return ((size_t)ftell(self->fileHandle));
	}

	void ResourceLoaderFromFile::Read(handle const destination, size_t const size)
	{
		GD_ASSERT(((size != 0) && (destination != nullptr)), "Invalid params specified");
		GD_ASSERT(((self->IsValidResourcePath())
			&& ((self->Flags & ResourceLoader::ResourceLoaderFlagsReadingEnabled) != 0)), 
			"File reading is unsupported");

		GD_ASSERT((fread(destination, size, 1, self->fileHandle) == 1), 
			"Reading data failed: read less than required");
	}

	void ResourceLoaderFromFile::Write(handle const source, size_t const size)
	{
		GD_ASSERT(((self->IsValidResourcePath()) 
			&& ((self->Flags & ResourceLoader::ResourceLoaderFlagsWritingEnabled) != 0)), 
			"File writing is unsupported");

		GD_ASSERT((fwrite(source, size, 1, self->fileHandle) == 1), 
			"Writing data failed: read less than required");
	}

GD_NAMESPACE_END

#endif
