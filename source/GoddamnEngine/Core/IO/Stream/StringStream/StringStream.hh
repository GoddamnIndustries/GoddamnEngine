/// ==========================================================================================
/// StringStream.hh - String stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 18.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_STRINGSTREAM_STRINGSTREAM
#define GD_CORE_IO_STREAM_STRINGSTREAM_STRINGSTREAM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>


GD_NAMESPACE_BEGIN

	/// Specifies read-only stream that provides reading from reference on data.
	template<typename CharacterType>
	class BaseStringInputStream final : public InputStream
	{
		static_assert(TypeTraits::IsCharacter<CharacterType>::Value, "'BaseStringInputStream<T>' error: T should be character type.");

	private /* Class members. */:
		UInt8 const* DataReference = nullptr;
		size_t       DataReferenceLength = 0;
		size_t       DataReferencePosition = 0;

	public /*Implemented interface API*/:
		/// Initializes StringInputStream using pointer to some memory.
		/// @param SomeMemory       Pointer to input stream initial memory.
		/// @param SomeMemoryLength Length of specified data memory length.
		GDINL BaseStringInputStream(CharacterType const* SomeMemory, size_t const SomeMemoryLength);

		/// Initializes StringInputStream using some String class instance.
		/// @param SomeString InputStream data.
		GDINL explicit BaseStringInputStream(BaseString<CharacterType> const& SomeString);

		/// Initializes StringInputStream using some StringBuilder class instance.
		/// @param SomeStringBuilder InputStream data.
		GDINL explicit BaseStringInputStream(BaseStringBuilder<CharacterType> const& SomeStringBuilder);

		/// Initializes StringInputStream using some Vector of characters class instance.
		/// @param SomeVector InputStream data.
		GDINL explicit BaseStringInputStream(Vector<CharacterType> const& SomeVector);

		GDINL virtual ~BaseStringInputStream();
		
		/// @see BaseStream::GetPosition()
		GDINL virtual size_t GetPosition() const override final;

		/// @see BaseStream::GetSize()
		GDINL virtual size_t GetSize() const override final;

		/// @see InputStream::Close()
		GDINL virtual void Close() override final;

		/// @see InputStream::Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current)
		GDINL virtual void Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current) override final;

		/// @see InputStream::Read()
		GDINL virtual UInt8 Read() override final;

		/// @see InputStream::Read(UInt8* const Array, size_t const Count, size_t const Length)
		GDINL virtual void Read(UInt8* const Array, size_t const Count, size_t const Length) override final;
	};	// class BaseStringInputStream
	
	/// Shortcut for StringInputStream for Char type.
	typedef BaseStringInputStream<Char> StringInputStream;

	/// Specifies write-only stream that provides writing to reference on data (BaseStringBuilder).
	template<typename CharacterType>
	class BaseStringOutputStream final : public OutputStream
	{
		static_assert(TypeTraits::IsCharacter<CharacterType>::Value, "'BaseStringOutputStream<T>' error: T should be character type.");

	private /*Class members*/:
		BaseStringBuilder<CharacterType>* Builder = nullptr;

	public /*Implemented interface API*/:
		/// Initializes new string output stream that points to some string builder.
		/// @param Builder Builder in which output would be stored.
		GDINL explicit BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder);
		GDINL virtual ~BaseStringOutputStream();
		
		/// @see BaseStream::GetPosition()
		GDINL virtual size_t GetPosition() const override final;

		/// @see BaseStream::GetSize()
		GDINL virtual size_t GetSize() const override final;

		/// @see OutputStream::Close()
		GDINL virtual void Close() override final;

		/// @see OutputStream::Flush()
		GDINL virtual void Flush() override final;

		/// @see OutputStream::Write(UInt8 const Byte)
		GDINL virtual void Write(UInt8 const Byte) override final;

		/// @see OutputStream::Write(UInt8 const* const Array, size_t const Count, size_t const Length)
		GDINL virtual void Write(UInt8 const* const Array, size_t const Count, size_t const Length) override final;
	};	// class StringOutputStream
	
	/// Shortcut for StringOutputStream for Char type.
	typedef BaseStringOutputStream<Char> StringOutputStream;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.inl>

#endif
