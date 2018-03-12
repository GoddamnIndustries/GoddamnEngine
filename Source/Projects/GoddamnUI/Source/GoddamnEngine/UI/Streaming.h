#pragma once

#include "EngineBase.h"
#include "PlatformDependent/FileApi.h"

namespace Engine
{
	namespace Streaming
	{
		enum FileAccess
		{
			AccessRead,
			AccessWrite,
			AccessReadWrite
		};
		enum FileCreationMode
		{
			CreateAlways,
			CreateNew,
			OpenAlways,
			OpenExisting,
			TruncateExisting
		};
		enum SeekOrigin
		{
			Begin,
			Current,
			End
		};

		class Stream : public Object
		{
		public:
			virtual void Read(void* buffer, UInt32 length) = 0;
			virtual void Write(const void* data, UInt32 length) = 0;
			virtual Int64 Seek(Int64 position, SeekOrigin origin) = 0;
			virtual UInt64 Length() = 0;
			virtual void SetLength(UInt64 length) = 0;
			virtual void Flush() = 0;
			void CopyTo(Stream* to, UInt64 length);
			void CopyTo(Stream* to);
		};

		class FileStream final : public Stream
		{
			handle file;
			bool owned;

		public:
			FileStream(const string& path, FileAccess access, FileCreationMode mode);
			FileStream(handle file_handle, bool take_control = false);
			virtual void Read(void* buffer, UInt32 length) override;
			virtual void Write(const void* data, UInt32 length) override;
			virtual Int64 Seek(Int64 position, SeekOrigin origin) override;
			virtual UInt64 Length() override;
			virtual void SetLength(UInt64 length) override;
			virtual void Flush() override;
			virtual ~FileStream() override;
			virtual string ToString() const override;
		};
		class MemoryStream final : public Stream
		{
			Array<UInt8> data;
			Int32 pointer;

		public:
			MemoryStream(const void* source, Int32 length);
			MemoryStream(const void* source, Int32 length, Int32 block);
			MemoryStream(Stream* source, Int32 length);
			MemoryStream(Stream* source, Int32 length, Int32 block);
			MemoryStream(Int32 block);
			virtual void Read(void* buffer, UInt32 length) override;
			virtual void Write(const void* data, UInt32 length) override;
			virtual Int64 Seek(Int64 position, SeekOrigin origin) override;
			virtual UInt64 Length() override;
			virtual void SetLength(UInt64 length) override;
			virtual void Flush() override;
			virtual ~MemoryStream() override;
			virtual string ToString() const override;
		};
		class FragmentStream final : public Stream
		{
			Stream* inner;
			UInt64 begin, end;
			Int64 pointer;

		public:
			FragmentStream(Stream* Inner, UInt64 From, UInt64 Length);
			virtual void Read(void* buffer, UInt32 length) override;
			virtual void Write(const void* data, UInt32 length) override;
			virtual Int64 Seek(Int64 position, SeekOrigin origin) override;
			virtual UInt64 Length() override;
			virtual void SetLength(UInt64 length) override;
			virtual void Flush() override;
			virtual ~FragmentStream() override;
			virtual string ToString() const override;
		};

		class TextWriter final : public Object
		{
			Stream* dest;
			Encoding coding;

		public:
			TextWriter(Stream* Dest);
			TextWriter(Stream* Dest, Encoding encoding);
			void Write(const string& text) const;
			void WriteLine(const string& text) const;
			void WriteEncodingSignature() const;
			virtual ~TextWriter() override;
			virtual string ToString() const override;

			TextWriter& operator<<(const string& text);
			const TextWriter& operator<<(const string& text) const;
		};
	}
}
