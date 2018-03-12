#include "Streaming.h"

namespace Engine
{
	namespace Streaming
	{
		using namespace ::Engine::IO;

		void Stream::CopyTo(Stream* to, UInt64 length)
		{
			constexpr UInt64 buflen = 0x100000;
			UInt8* buffer = gd_new UInt8[buflen];
			if (!buffer)
				throw OutOfMemoryException();
			try
			{
				UInt64 pending = length;
				while (pending)
				{
					UInt32 amount = UInt32(min(buflen, pending));
					Read(buffer, amount);
					to->Write(buffer, amount);
					pending -= amount;
				}
			}
			catch (...)
			{
				gd_delete[] buffer;
				throw;
			}
			gd_delete[] buffer;
		}
		void Stream::CopyTo(Stream* to)
		{
			CopyTo(to, Length());
		}
		FileStream::FileStream(const string& path, FileAccess access, FileCreationMode mode)
		    : owned(true)
		{
			file = CreateFile(path, static_cast<IO::FileAccess>(access), static_cast<IO::FileCreationMode>(mode));
		}
		FileStream::FileStream(handle file_handle, bool take_control)
		    : owned(take_control)
		{
			file = file_handle;
		}
		void FileStream::Read(void* buffer, UInt32 length)
		{
			ReadFile(file, buffer, length);
		}
		void FileStream::Write(const void* data, UInt32 length)
		{
			WriteFile(file, data, length);
		}
		Int64 FileStream::Seek(Int64 position, SeekOrigin origin)
		{
			return IO::Seek(file, position, static_cast<IO::SeekOrigin>(origin));
		}
		UInt64 FileStream::Length()
		{
			return GetFileSize(file);
		}
		void FileStream::SetLength(UInt64 length)
		{
			SetFileSize(file, length);
		}
		void FileStream::Flush()
		{
			IO::Flush(file);
		}
		FileStream::~FileStream()
		{
			if (owned)
				CloseFile(file);
		}
		string FileStream::ToString() const
		{
			return L"FileStream";
		}

		MemoryStream::MemoryStream(const void* source, Int32 length)
		    : data(0x10000)
		    , pointer(0)
		{
			data.Resize(length);
			MemoryCopy(data.GetData(), source, length);
		}
		MemoryStream::MemoryStream(const void* source, Int32 length, Int32 block)
		    : data(block)
		    , pointer(0)
		{
			data.Resize(length);
			MemoryCopy(data.GetData(), source, length);
		}
		MemoryStream::MemoryStream(Stream* source, Int32 length)
		    : data(0x10000)
		    , pointer(0)
		{
			data.Resize(length);
			source->Read(data.GetData(), length);
		}
		MemoryStream::MemoryStream(Stream* source, Int32 length, Int32 block)
		    : data(block)
		    , pointer(0)
		{
			data.Resize(length);
			source->Read(data.GetData(), length);
		}
		MemoryStream::MemoryStream(Int32 block)
		    : data(block)
		    , pointer(0)
		{
		}
		void MemoryStream::Read(void* buffer, UInt32 length)
		{
			if (length > UInt32(data.GetLength() - pointer))
			{
				length = (pointer >= data.GetLength()) ? 0 : (UInt32(data.GetLength()) - pointer);
				MemoryCopy(buffer, data.GetData() + pointer, length);
				pointer += length;
				throw FileReadEndOfFileException(length);
			}
			MemoryCopy(buffer, data.GetData() + pointer, length);
			pointer += length;
		}
		void MemoryStream::Write(const void* _data, UInt32 length)
		{
			if (length > UInt32(data.GetLength() - pointer))
			{
				if (length > 0x7FFFFFFF)
					throw InvalidArgumentException();
				if (UInt32(pointer) + length > 0x7FFFFFFF)
					throw FileAccessException();
				data.Resize(pointer + Int32(length));
			}
			MemoryCopy(data.GetData() + pointer, _data, length);
			pointer += Int32(length);
		}
		Int64 MemoryStream::Seek(Int64 position, SeekOrigin origin)
		{
			Int64 newpos = position;
			if (origin == Current)
				newpos += Int64(pointer);
			else if (origin == End)
				newpos += Int64(data.GetLength());
			if (newpos < 0 || newpos > Int64(data.GetLength()))
				throw InvalidArgumentException();
			pointer = Int32(newpos);
			return pointer;
		}
		UInt64 MemoryStream::Length()
		{
			return UInt64(data.GetLength());
		}
		void MemoryStream::SetLength(UInt64 length)
		{
			if (length > 0x7FFFFFFF)
				throw InvalidArgumentException();
			data.Resize(Int32(length));
		}
		void MemoryStream::Flush()
		{
		}
		MemoryStream::~MemoryStream()
		{
		}
		string MemoryStream::ToString() const
		{
			return L"MemoryStream";
		}
		TextWriter::TextWriter(Stream* Dest)
		{
			dest = Dest;
			dest->AddRef();
			coding = TextFileEncoding;
		}
		TextWriter::TextWriter(Stream* Dest, Encoding encoding)
		{
			dest = Dest;
			dest->AddRef();
			coding = encoding;
		}
		void TextWriter::Write(const string& text) const
		{
			GD_NOT_IMPLEMENTED();
			// auto data = text.EncodeSequence(coding, false);
			// dest->Write(data->GetBuffer(), data->Length());
			// data->Release();
		}
		void TextWriter::WriteLine(const string& text) const
		{
			Write(text + NewLineChar);
		}
		void TextWriter::WriteEncodingSignature() const
		{
			if (coding == Encoding::ANSI)
			{
			}
			else if (coding == Encoding::UTF8)
			{
				dest->Write("\xEF\xBB\xBF", 3);
			}
			else if (coding == Encoding::UTF16)
			{
				dest->Write("\xFF\xFE", 2);
			}
			else if (coding == Encoding::UTF32)
			{
				dest->Write("\xFF\xFE\x00\x00", 4);
			}
		}
		TextWriter::~TextWriter()
		{
			dest->Release();
		}
		string TextWriter::ToString() const
		{
			return L"TextWriter";
		}
		TextWriter& TextWriter::operator<<(const string& text)
		{
			Write(text);
			return *this;
		}
		const TextWriter& TextWriter::operator<<(const string& text) const
		{
			Write(text);
			return *this;
		}
		FragmentStream::FragmentStream(Stream* Inner, UInt64 From, UInt64 Length)
		    : inner(Inner)
		    , begin(From)
		    , end(From + Length)
		    , pointer(From)
		{
			inner->AddRef();
		}
		void FragmentStream::Read(void* buffer, UInt32 length)
		{
			if (UInt64(length) > end - begin - pointer)
			{
				length = (UInt64(pointer) >= end - begin) ? 0 : UInt32(end - begin - pointer);
				auto pos = inner->Seek(0, Current);
				inner->Seek(begin + pointer, Begin);
				try
				{
					inner->Read(buffer, length);
				}
				catch (...)
				{
					inner->Seek(pos, Begin);
					throw;
				}
				inner->Seek(pos, Begin);
				pointer += length;
				throw FileReadEndOfFileException(length);
			}
			auto pos = inner->Seek(0, Current);
			inner->Seek(begin + pointer, Begin);
			try
			{
				inner->Read(buffer, length);
			}
			catch (...)
			{
				inner->Seek(pos, Begin);
				throw;
			}
			pointer += length;
		}
		void FragmentStream::Write(const void* data, UInt32 length)
		{
			throw FileAccessException();
		}
		Int64 FragmentStream::Seek(Int64 position, SeekOrigin origin)
		{
			Int64 newpos = position;
			if (origin == Current)
				newpos += pointer;
			else if (origin == End)
				newpos += end - begin;
			if (newpos < 0 || UInt64(newpos) > end - begin)
				throw InvalidArgumentException();
			pointer = newpos;
			return pointer;
		}
		UInt64 FragmentStream::Length()
		{
			return end - begin;
		}
		void FragmentStream::SetLength(UInt64 length)
		{
			throw FileAccessException();
		}
		void FragmentStream::Flush()
		{
		}
		FragmentStream::~FragmentStream()
		{
			inner->Release();
		}
		string FragmentStream::ToString() const
		{
			return L"FragmentStream";
		}
	}
}
