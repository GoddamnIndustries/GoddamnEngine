
GD_NAMESPACE_BEGIN

	ResourceLoader::ResourceLoader(ResourceLoaderFlags const resourceLoaderFlags) :
		Flags(resourceLoaderFlags)
	{
	}

	template<class ReadingElementType>
	GDINL ReadingElementType ResourceLoader::Read()
	{
		ReadingElementType readingElementType;
		self->Read(&readingElementType, sizeof(readingElementType));

		return readingElementType;
	}

	GDINL void ResourceLoader::ReadNullTerminatedString(String& destination)
	{
		Char character = self->Read<Char>();
		do 
		{
			destination.PushLast(character);
		} while (character != Char('\0'));
	}

	template<class ReadingHeaderType>
	GDINL void ResourceLoader::ReadSizedData(Vector<byte>& memory)
	{
		ReadingHeaderType const headerValue = self->Read<ReadingHeaderType>();
		memory.Resize(headerValue);
		
		self->Read(&memory[0], headerValue);
	}

	template<class WritingElementType>
	GDINL WritingElementType ResourceLoader::Write(WritingElementType const& writingElementType)
	{
		self->Write(&writingElementType, sizeof(writingElementType));
	}

	GDINL void ResourceLoader::WriteNullTerminatedString(String const& nullTerminatedString)
	{
		self->Write(((handle)(&nullTerminatedString[0])), 
			((sizeof(nullTerminatedString[0])) * (nullTerminatedString.GetSize() + 1)));
	}

	template<class ReadingHeaderType>
	GDINL void ResourceLoader::WriteSizedData(handle const memory, ReadingHeaderType const size)
	{
		self->Write<ReadingHeaderType>((ReadingHeaderType)size);
		self->Write(memory, size);
	}

GD_NAMESPACE_END
