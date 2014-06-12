#ifndef GD_RESOURCES_RESOURCE_LOADER_RESOURCE_LOADER
#define GD_RESOURCES_RESOURCE_LOADER_RESOURCE_LOADER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Containers/String/String.hh>

#include <limits.h>

GD_NAMESPACE_BEGIN

	/// Class makes abstraction for resource loading from different sources
	class ResourceLoader
	{
		GD_CLASS_UNASSIGNABLE(ResourceLoader);
		GD_CLASS_UNCOPIABLE(ResourceLoader);

	public:
		typedef UInt16 ResourceLoaderFlags;
		ResourceLoaderFlags const Flags;						///< Flags describing resource loader 
		enum : ResourceLoaderFlags
		{
			ResourceLoaderFlagsSupportsReading	= GD_BIT(0),		///< This resource loader supports reading from stream
			ResourceLoaderFlagsSupportsWriting	= GD_BIT(1),		///< This resource loader supports writing to stream
			ResourceLoaderFlagsReadingEnabled	= GD_BIT(2),		///< This resource loader is opened to reading
			ResourceLoaderFlagsRewritingEnabled	= GD_BIT(3),		///< This resource loader is opened to writing (rewriting)
			ResourceLoaderFlagsAppendingEnabled	= GD_BIT(4),		///< This resource loader is opened to writing (appending to end)
			ResourceLoaderFlagsWritingEnabled	= (ResourceLoaderFlagsRewritingEnabled | ResourceLoaderFlagsAppendingEnabled),	
			ResourceLoaderFlagsWritingAll		= USHRT_MAX,
			ResourceLoaderFlagsWritingNone		= 0	
		};

	public:
		GDINL ResourceLoader(ResourceLoaderFlags const resourceLoaderFlags = (ResourceLoaderFlagsSupportsReading | ResourceLoaderFlagsReadingEnabled));

		/// Determines if file is valid
		GDINT virtual bool IsValidResourcePath() const abstract;

		/// Returns size of resource in bytes
		GDINT virtual size_t GetResourceDataSize() const abstract;

		/// Reads data from loader stream to destination with specified size
		GDINT virtual void Read(handle const destination, size_t const size) abstract;

		/// Reads data one instance of specified type and returns it
		template<class ReadingElementType>
		GDINL ReadingElementType Read();

		/// Reads bytes into destination until '@c \0' is reached
		GDINL void ReadNullTerminatedString(String& destination);

		/// Reads data from resource stream with length stored in stream with type of ReadingHeaderType
		template<class ReadingHeaderType>
		GDINL void ReadSizedData(Vector<byte>& memory);

		/// Writes data from source to resource loading stream with specified size.
		GDINT virtual void Write(handle const source, size_t const size) abstract;

		/// Writes one instance of specified type
		template<class WritingElementType>
		GDINL WritingElementType Write(WritingElementType const& writingElementType);

		/// Writes string into file and terminating null into it
		GDINL void WriteNullTerminatedString(String const& nullTerminatedString);

		/// Writes size of memory and memory to resource loading stream
		template<class ReadingHeaderType>
		GDINL void WriteSizedData(handle const memory, ReadingHeaderType const size);

		/// @brief					Changes file pointer position in resource stream.
		/// @param seekFromCurrent	How much to seek from current position.
		/// @param seekFromStart	How much to seek from start of file. If is -1 then seeks from current position. 
		/// @returns				Pointer position in file after seeking
		GDINT virtual size_t Seek(long const seekFromCurrent, long const seekFromStart = -1) abstract;
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Resource/ResourceLoader/ResourceLoader.inl>

#endif