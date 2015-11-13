//! ==========================================================================================
//! StringStream.h - String stream interface.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_STRINGSTREAM
#define GD_CORE_IO_STREAM_STRINGSTREAM

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>

GD_NAMESPACE_BEGIN

	//! Specifies read-only stream that provides reading from reference on data.
	template<typename CharacterType>
	class BaseStringInputStream final : public InputStream
	{
		static_assert(TypeTraits::IsCharacter<CharacterType>::Value, "'BaseStringInputStream<T>' error: T should be character type.");

	private /* Class members. */:
		UInt8 const* DataReference = nullptr;
		SizeTp    DataReferenceLength = 0;
		SizeTp    DataReferencePosition = 0;

	public:
		//! 					Initializes StringInputStream using pointer to some memory.
		//! @param memory    pointer to input stream initial memory.
		//! @param MemoryLength m_Length of specified data memory m_Length.
		GDINL						BaseStringInputStream(CharacterType const* Memory, SizeTp const MemoryLength);

		////! 					Initializes StringInputStream using some String class instance.
		////! @param SomeString		InputStream data.
		//GDINL explicit				BaseStringInputStream(BaseString<CharacterType> const& SomeString);

		////! 					Initializes StringInputStream using some StringBuilder class instance.
		////! @param SomeBuilder	InputStream data.
		//GDINL explicit				BaseStringInputStream(BaseStringBuilder<CharacterType> const& SomeBuilder);

		////! 					Initializes StringInputStream using some Vector of characters class instance.
		////! @param SomeVector	InputStream data.
		//GDINL explicit				BaseStringInputStream(Vector<CharacterType> const& SomeVector);

		//! 					Closes this stream.
		GDINL virtual				~BaseStringInputStream();
		
		//! 					Returns current position in stream.
		//! @returns				Current position in stream.
		GDINL virtual SizeTp		GetPosition() const override final;

		//! 					Returns size of data that stream handles.
		//! @returns				Size of data that stream handles.
		GDINL virtual SizeTp		GetLength() const override final;

		//! 					Closes this stream and releases all resources associated with this stream.
		GDINL virtual void			Close() override final;

		//! 					Reposition this stream to new specified position.
		//! @param offset		offset in bytes from specified origin.
		//! @param origin		Defines origin from which point make offset.
		GDINL virtual void			Seek(ptrdiff_t const offset, SeekOrigin const origin = SeekOrigin::Current) override final;

		//! 					Reads next element from input stream. Full endian abstraction is provided.
		//! @returns				Read element from stream.
		GDINL virtual UInt8			Read() override final;

		//! 					Reads several elements from input stream.
		//! @param Array		Output memory to which data would be written.
		//! @param  count		m_Length of one element
		//! @param  m_Length		m_Length of memory in elements.
		GDINL virtual void			Read(Handle const Array, SizeTp const count, SizeTp const length) override final;
	};	// class BaseStringInputStream
	
	//! Specifies write-only stream that provides writing to reference on data (BaseStringBuilder).
	template<typename CharacterType>
	class BaseStringOutputStream final : public OutputStream
	{
		static_assert(TypeTraits::IsCharacter<CharacterType>::Value, "'BaseStringOutputStream<T>' error: T should be character type.");

	private:
		BaseStringBuilder<CharacterType>* Builder = nullptr;

	public:
		//! 					Initializes new string output stream that points to some string builder.
		//! @param Builder		Builder in which output would be stored.
		GDINL explicit				BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder);

		//! 					Closes this stream.
		GDINL virtual				~BaseStringOutputStream();

		//! 					Returns current position in stream.
		//! @returns				Current position in stream.
		GDINT virtual SizeTp		GetPosition() const override final;

		//! 					Returns size of data that stream handles.
		//! @returns				Size of data that stream handles.
		GDINT virtual SizeTp		GetLength() const override final;

		//! 					Closes this stream and releases all resources associated with this stream.
		GDINT virtual void			Close() override final;

		//! 					Writes all unsaved to the resource.
		GDINT virtual void			Flush() override final;

		//! 					Writes a byte into output.
		//! @param				Byte Byte that would be written to output.
		GDINT virtual void			Write(UInt8 const Byte) override final;

		//! 					Writes several elements to output.
		//! @param Array		Input elements that would be written.
		//! @param count		m_Length of one element.
		//! @param m_Length		m_Length of memory in elements.
		GDINL virtual void			Write(CHandle const Array, SizeTp const count, SizeTp const length) override final;
	};	// class StringOutputStream
	

	//! Shortcut for StringInputStream for Char type.
	typedef BaseStringInputStream<Char> StringInputStream;

	//! Shortcut for StringOutputStream for Char type.
	typedef BaseStringOutputStream<Char> StringOutputStream;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.inl>

#endif
