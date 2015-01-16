/// ==========================================================================================
/// StringStream.h - String stream interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// @author James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_STRINGSTREAM
#define GD_CORE_IO_STREAM_STRINGSTREAM

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>

GD_NAMESPACE_BEGIN

	/// Specifies read-only stream that provides reading from reference on data.
	template<typename CharacterType>
	class BaseStringInputStream final : public InputStream
	{
		static_assert(TypeTraits::IsCharacter<CharacterType>::Value, "'BaseStringInputStream<T>' error: T should be character type.");

	private /* Class members. */:
		UInt8 const* DataReference = nullptr;
		size_t    DataReferenceLength = 0;
		size_t    DataReferencePosition = 0;

	public:
		/// @brief					Initializes StringInputStream using pointer to some memory.
		/// @param Memory    Pointer to input stream initial memory.
		/// @param MemoryLength Length of specified data memory length.
		GDINL						BaseStringInputStream(CharacterType const* Memory, size_t const MemoryLength);

		/// @brief					Initializes StringInputStream using some String class instance.
		/// @param SomeString		InputStream data.
		GDINL explicit				BaseStringInputStream(BaseString<CharacterType> const& SomeString);

		/// @brief					Initializes StringInputStream using some StringBuilder class instance.
		/// @param SomeBuilder	InputStream data.
		GDINL explicit				BaseStringInputStream(BaseStringBuilder<CharacterType> const& SomeBuilder);

		/// @brief					Initializes StringInputStream using some Vector of characters class instance.
		/// @param SomeVector	InputStream data.
		GDINL explicit				BaseStringInputStream(Vector<CharacterType> const& SomeVector);

		/// @brief					Closes this stream.
		GDINL virtual				~BaseStringInputStream();
		
		/// @brief					Returns current position in stream.
		/// @returns				Current position in stream.
		GDINL virtual size_t		GetPosition() const override final;

		/// @brief					Returns size of data that stream handles.
		/// @returns				Size of data that stream handles.
		GDINL virtual size_t		GetLength() const override final;

		/// @brief					Closes this stream and releases all resources associated with this stream.
		GDINL virtual void			Close() override final;

		/// @brief					Reposition this stream to new specified position.
		/// @param Offset		Offset in bytes from specified origin.
		/// @param Origin		Defines origin from which point make offset.
		GDINL virtual void			Seek(ptrdiff_t const Offset, SeekOrigin const Origin = SeekOrigin::Current) override final;

		/// @brief					Reads next element from input stream. Full endian abstraction is provided.
		/// @returns				Read element from stream.
		GDINL virtual UInt8			Read() override final;

		/// @brief					Reads several elements from input stream.
		/// @param Array		Output memory to which data would be written.
		/// @param  Count		Length of one element
		/// @param  Length		Length of memory in elements.
		GDINL virtual void			Read(handle const Array, size_t const Count, size_t const Length) override final;
	};	// class BaseStringInputStream
	
	/// Specifies write-only stream that provides writing to reference on data (BaseStringBuilder).
	template<typename CharacterType>
	class BaseStringOutputStream final : public OutputStream
	{
		static_assert(TypeTraits::IsCharacter<CharacterType>::Value, "'BaseStringOutputStream<T>' error: T should be character type.");

	private:
		BaseStringBuilder<CharacterType>* Builder = nullptr;

	public:
		/// @brief					Initializes new string output stream that points to some string builder.
		/// @param Builder		Builder in which output would be stored.
		GDINL explicit				BaseStringOutputStream(BaseStringBuilder<CharacterType>& Builder);

		/// @brief					Closes this stream.
		GDINL virtual				~BaseStringOutputStream();

		/// @brief					Returns current position in stream.
		/// @returns				Current position in stream.
		GDINT virtual size_t		GetPosition() const override final;

		/// @brief					Returns size of data that stream handles.
		/// @returns				Size of data that stream handles.
		GDINT virtual size_t		GetLength() const override final;

		/// @brief					Closes this stream and releases all resources associated with this stream.
		GDINT virtual void			Close() override final;

		/// @brief					Writes all unsaved to the resource.
		GDINT virtual void			Flush() override final;

		/// @brief					Writes a byte into output.
		/// @param				Byte Byte that would be written to output.
		GDINT virtual void			Write(UInt8 const Byte) override final;

		/// @brief					Writes several elements to output.
		/// @param Array		Input elements that would be written.
		/// @param Count		Length of one element.
		/// @param Length		Length of memory in elements.
		GDINL virtual void			Write(chandle const Array, size_t const Count, size_t const Length) override final;
	};	// class StringOutputStream
	

	/// Shortcut for StringInputStream for Char type.
	typedef BaseStringInputStream<Char> StringInputStream;

	/// Shortcut for StringOutputStream for Char type.
	typedef BaseStringOutputStream<Char> StringOutputStream;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.inl>

#endif
