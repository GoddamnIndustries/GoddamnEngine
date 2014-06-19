//////////////////////////////////////////////////////////////////////////
/// StringStream.hh - String stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_IO_STREAM_STRINGSTREAM_STRINGSTREAM
#define GD_CORE_IO_STREAM_STRINGSTREAM_STRINGSTREAM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>

#include <cstdio>

GD_NAMESPACE_BEGIN

	/// Specifies read-only stream that provides reading from reference on data.
	template<typename CharacterType, typename = typename EnableIf<TypeTraits::IsCharacter<CharacterType>::Value>::Type>
	class BaseStringInputStream final : public InputStream
	{
	private /* Class members. */:
		UInt8 const* DataReference = nullptr;
		size_t       DataReferenceLength = 0;
		size_t       DataReferencePosition = 0;

	public /* Constructors / Destructor */:
		GDINL explicit BaseStringInputStream(BaseString<CharacterType> const& SomeString)
			: DataReference      (reinterpret_cast<UInt8 const*>(SomeString.CStr()))
			, DataReferenceLength(sizeof(CharacterType)       *  SomeString.GetSize())
		{
		}

		GDINL explicit BaseStringInputStream(Vector<CharacterType> const& SomeVector)
			: DataReference      (reinterpret_cast<UInt8 const*>(SomeVector.CStr()))
			, DataReferenceLength(sizeof(CharacterType)       *  SomeVector.GetSize())
		{
		}

		GDINL explicit BaseStringInputStream(CharacterType const* SomeMemory, size_t const SomeMemoryLength)
			: DataReference      (reinterpret_cast<UInt8 const*>(SomeMemory))
			, DataReferenceLength(sizeof(CharacterType)       *  SomeMemoryLength)
		{
		}

		GDINL virtual ~BaseStringInputStream()
		{
			self->Dispose();
		}

	public /* Class implemented API */:
		GDINL virtual bool CanSeek() const final { return true; }

		GDINL virtual size_t GetPosition() const final { return self->DataReferencePosition; }
		GDINL virtual size_t GetLength  () const final { return self->DataReferenceLength; }

		GDINL virtual void Dispose() final 
		{
			self->DataReference         = nullptr;
			self->DataReferenceLength   = 0;
			self->DataReferencePosition = 0;
		}

		inline virtual size_t Seek(ptrdiff_t const Offset, SeekOrigin const Origin) final
		{
			switch (Origin)
			{
			case GD_SEEK_ORIGIN_BEGIN: 
				return self->DataReferencePosition = Offset;

			case GD_SEEK_ORIGIN_CURRENT: 
				return self->DataReferencePosition += Offset;

			case GD_SEEK_ORIGIN_END: 
				return self->DataReferencePosition = self->DataReferenceLength + Offset;

			default: GD_ASSERT_FALSE("Invalid origin specified");
			}

			return 0;
		}

		inline virtual size_t Read(handle const OutputBuffer, ptrdiff_t const Offset, size_t const Count) final
		{
			if (self->DataReference != nullptr)
			if (self->Seek(Offset, GD_SEEK_ORIGIN_CURRENT) < self->GetLength())
			{	// Seeking this position on offset
				size_t const NumCharactersRead = (self->GetPosition() + Count) <= self->GetLength() ? Count : (self->GetLength() - self->GetPosition());
				memcpy(OutputBuffer, self->DataReference + self->GetPosition(), NumCharactersRead);
				self->Seek(NumCharactersRead, GD_SEEK_ORIGIN_CURRENT);

				return NumCharactersRead;
			}

			return 0;
		}
	};	// class BaseStringInputStream
	typedef BaseStringInputStream<Char> StringInputStream;

	/// Specifies write-only stream that provides writing to reference on data (BaseStringBuilder).
	template<typename CharacterType, typename = typename EnableIf<TypeTraits::IsCharacter<CharacterType>::Value>::Type>
	class BaseStringOutputStream final : public OutputStream
	{
	private /* Class members. */:
		BaseStringBuilder<CharacterType>* Builder = nullptr;

	public /* Constructor / Destructor */:
		GDINL explicit BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder) : Builder(&Builder) { }
		GDINL virtual ~BaseStringOutputStream(                                         )                     { self->Dispose(); }

	public /* Class implemented API */:
		GDINL virtual size_t GetLength  () const final { return self->Builder->GetSize(); }
		GDINL virtual size_t GetPosition() const final { return self->Builder->GetSize(); }

		GDINL virtual void Flush  () final { }
		GDINL virtual void Dispose() final { self->Builder = nullptr; }
		
		inline virtual size_t Write(chandle const InputBuffer, ptrdiff_t const Offset, size_t const Count) final
		{
			if (self->Builder != nullptr)
			{
				CharacterType const* String = reinterpret_cast<CharacterType const*>(InputBuffer) + Offset;
				self->Builder->Append(String, Count);

				return Count;
			}

			return 0;
		}

	};	// class StringOutputStream
	typedef BaseStringOutputStream<Char> StringOutputStream;

GD_NAMESPACE_END

#endif
