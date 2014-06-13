//////////////////////////////////////////////////////////////////////////
/// Stream.hh - Base stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_IO_STREAM
#define GD_CORE_IO_STREAM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Utility.hh>					
// #include <GoddamnEngine/Core/Threading/Task/Task.hh>		
#include <GoddamnEngine/Core/Text/String/String.hh>	
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>

GD_NAMESPACE_BEGIN

	template<typename SomeType> 
	struct Task { };

	/// Specifies the position in a stream to use for seeking.
	/// Streams are divided into Read-Only and Write-Only streams.
	enum SeekOrigin : UInt8
	{
		GD_SEEK_ORIGIN_BEGIN,	///< Specifies beginning of stream.
		GD_SEEK_ORIGIN_CURRENT,	///< Specifies current position of stream.
		GD_SEEK_ORIGIN_END		///< Specifies end of stream.
	};	// enum SeekOrigin

	/// Provides abstract view on sequence of bytes.
	class Stream
	{
	private:
		GD_CLASS_UNASSIGNABLE(Stream);
		GD_CLASS_UNSWAPPABLE (Stream);
		GD_CLASS_UNCOPIABLE  (Stream);
		GD_CLASS_UNMOVABLE   (Stream);

	protected:
		GDINL Stream() { }
	public:
		GDINL virtual ~Stream() { }

		/// Returns true if stream supports reading.
		/// Actually is true for Read-Only streams.
		GDAPI virtual      bool      CanRead () const abstract;
		/// Returns true if stream supports writing.
		/// Actually is true for Write-Only streams.
		GDAPI virtual      bool      CanWrite() const abstract;
		/// Returns true if stream supports seeking.
		/// Actually is true for both Read-Only and Write-Only streams.
		GDAPI virtual      bool      CanSeek () const abstract;

		/// Returns current position in stream.
		GDAPI virtual      size_t    GetPosition() const abstract;
		/// Returns current length of stream.
		GDAPI virtual      size_t    GetLength  () const abstract;
	
		/// Releases all resources used by this stream.
		GDAPI virtual      void      Dispose() abstract;

		/// Advances stream position from origin into specified value from specified origin.	
		/// @returns New position in stream.
		GDAPI virtual      size_t    Seek(ptrdiff_t const Offset, SeekOrigin const Origin) abstract;

		/// Reads sequence of bytes from current stream and advances reading position.
		/// @returns Total number of read bytes.
		GDAPI virtual      size_t    Read     (handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count) abstract;
		/// Asynchronously reads sequence of bytes from current stream and advances reading position.
		/// @returns Task that represents asynchronous read operation.
		GDAPI         Task<size_t>&& ReadAsync(handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count);
		/// Reads from stream into some object.
		template<typename ReadingElementType>
		GDINL ReadingElementType&&   Read();

		/// Writes sequence of bytes into current stream and advances writing position.
		/// @returns Total number of written bytes.
		GDAPI virtual      size_t    Write     (chandle const InputBuffer, ptrdiff_t const Offset, size_t const Count) abstract;
		/// Writes into specified stream some object.
		template<typename WritingElementType>
		GDINL void                   Write     (WritingElementType const& Element);
		/// Asynchronously writes sequence of bytes into current stream and advances writing position.
		/// @returns Task that represents asynchronous write operation.
		GDAPI         Task<size_t>&& WriteAsync(chandle const InputBuffer, ptrdiff_t const Offset, size_t const Count);

		/// Writes all unwritten data into underlying device and clears stream.
		/// This method is invocable for Write-Only streams.
		GDAPI virtual      void      Flush     () abstract;
		/// Asynchronously writes all unwritten data into underlying device and clears stream.
		/// @returns Task that represents flushing operation.
	 	GDAPI         Task<void>&&   FlushAsync();

		/// Copies content of this stream into specified one with specified length.
		GDAPI              void      CopyTo     (Stream* OtherStream, size_t const BufferSize);
		/// Copies content of this stream into specified one.
		GDINL              void      CopyTo     (Stream* OtherStream) { self->CopyTo(OtherStream, self->GetLength()); }
		/// Asynchronously copies content of this stream into specified one with specified length.
		/// @returns Task that represents copying operation.
		GDAPI         Task<void>&&   CopyToAsync(Stream* OtherStream, size_t const BufferSize);
		/// Asynchronously copies content of this stream into specified one.
		/// @returns Task that represents copying operation.
		GDAPI         Task<void>&&   CopyToAsync(Stream* OtherStream);
	};	// class Stream

	/// Specifies stream with read-only support.
	class InputStream : public Stream
	{
		GDINL virtual bool CanWrite() const final { return false; }
		GDINL virtual bool CanRead () const final { return true;  } 

		GDINL virtual void Flush   ()       final { GD_ASSERT_FALSE("Attempted flushing Read-Only stream"); }

		GDINL virtual size_t Write(chandle const InputBuffer, ptrdiff_t const Offset, size_t const Count) final 
		{
			GD_UNUSED(InputBuffer);
			GD_UNUSED(Offset);
			GD_UNUSED(Count);
			GD_ASSERT_FALSE("Writing unsupprted."); 
			return 0; 
		}
	};	// class InputStream

	/// Specifies stream with write-only support.
	class OutputStream : public Stream
	{
		GDINL virtual bool CanWrite() const final { return true;  }
		GDINL virtual bool CanRead () const final { return false; }
		GDINL virtual bool CanSeek () const final { return false; }

		GDINL virtual size_t Seek(ptrdiff_t const Offset, SeekOrigin const Origin) final 
		{ 
			GD_UNUSED(Offset);
			GD_UNUSED(Origin);
			GD_ASSERT_FALSE("Seeking unsupported.");
			return 0;
		}

		GDINL virtual size_t Read(handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count) final 
		{ 
			GD_UNUSED(OutputBuffer);
			GD_UNUSED(Offset);
			GD_UNUSED(Count);
			GD_ASSERT_FALSE("Reading unsupported.");
			return 0; 
		}
	};	// class OutputStream

	/// Specifies read-only stream that provides data from reference on data.
	class DataRefInputStream : public InputStream
	{
	private:
		UInt8 const* DataReference = nullptr;
		size_t const DataReferenceLength = 0;
		size_t DataReferencePosition = 0;

	public:
		template<typename SomeElementType>
		GDINL DataRefInputStream(Vector<SomeElementType> const& SomeVector) :
			DataReference(reinterpret_cast<UInt8 const*>(&SomeVector[0])),
			DataReferenceLength(sizeof(SomeElementType) * SomeVector.GetSize()) { }
		template<typename SomeElementType>
		GDINL DataRefInputStream(SomeElementType const* SomeMemory, size_t const SomeMemoryLength) :
			DataReference(reinterpret_cast<UInt8 const*>(SomeMemory)),
			DataReferenceLength(sizeof(SomeElementType) * SomeMemoryLength) { }
		GDINL DataRefInputStream(String const& SomeString) :
			DataReference(reinterpret_cast<UInt8 const*>(&SomeString[0])),
			DataReferenceLength(sizeof(DeclValue<String>()[0]) * SomeString.GetSize()) { }

		GDINL virtual bool   CanSeek() const final { return true; }

		GDINL virtual size_t GetPosition() const final { return self->DataReferencePosition; }
		GDINL virtual size_t GetLength() const final { return self->DataReferenceLength; }

		GDINL virtual void   Dispose() final { }
		GDINL virtual size_t Seek(ptrdiff_t const Offset, SeekOrigin const Origin) final
		{
			switch (Origin)
			{
			case GD_SEEK_ORIGIN_BEGIN  : return self->DataReferencePosition  = Offset; 
			case GD_SEEK_ORIGIN_CURRENT: return self->DataReferencePosition += Offset;
			case GD_SEEK_ORIGIN_END	   : return self->DataReferencePosition  = self->DataReferenceLength + Offset;
			default: GD_ASSERT_FALSE("Invalid origin specified");
			}

			return 0;
		}

		GDINL virtual size_t Read(handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count) final
		{ 
			if (self->Seek(Offset, GD_SEEK_ORIGIN_CURRENT) < self->GetLength())
			{	// Seeking this position on offset
				size_t const NumElmentsRead = (self->GetPosition() + Count) <= self->GetLength() ? Count : (self->GetLength() - self->GetPosition());
				memcpy(OutputBuffer, self->DataReference + self->GetPosition(), NumElmentsRead);
				self->Seek(NumElmentsRead, GD_SEEK_ORIGIN_CURRENT);
				return NumElmentsRead;
			}

			return 0;
		}
	};	// DataRefInputStream

	class StringRefOutputStream final : public InputStream
	{
	};

GD_NAMESPACE_END
#include <GoddamnEngine/Core/IO/Stream/Stream.inl>
#endif