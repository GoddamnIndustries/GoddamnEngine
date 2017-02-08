// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/Stream.h
 * File contains IO stream interfaces.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Represents I/O stream basic functionality.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class BaseStream : public ReferenceTarget
	{
	public:

		/*!
		 * Returns true if this stream is valid and ready for I/O operations.
		 */
		GDAPI virtual bool IsValid() const GD_PURE_VIRTUAL;

	};	// class BaseStream

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Base class for all classes representing an input stream of bytes.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class InputStream : public BaseStream
	{
	public:

		/*!
		 * Returns current position in stream or -1 on error.
		 */
		GDAPI virtual SizeTp GetPosition() const GD_PURE_VIRTUAL;

		/*!
		 * Returns size of data that stream handles or -1 on error.
		 */
		GDAPI virtual SizeTp GetLength() const GD_PURE_VIRTUAL;

		/*!
		 * Closes this stream and releases all resources associated with this stream.
		 */
		GDAPI virtual void Close() GD_PURE_VIRTUAL;

		/*!
		 * Reposition this stream to new specified position.
		 *
		 * @param offset The offset in bytes from specified origin.
		 * @param origin Defines origin from which point make offset.
		 *
		 * @returns New position in file, -1 on error.
		 */
		GDAPI virtual SizeTp Seek(PtrDiffTp const offset, SeekOrigin const origin = SeekOrigin::Current) GD_PURE_VIRTUAL;

		/*!
		 * Reads next byte from input stream.
		 * @returns Read byte from stream, or -1 on the error.
		 */
		GDAPI virtual Int16 Read() GD_PURE_VIRTUAL;

		/*!
		 * Reads several elements from input stream.
		 *
		 * @param array Output memory to which data would be written.
		 * @param size Length of the element in bytes,
		 * @param count Amount of the elements.
		 *
		 * @returns Total amount of elements read.
		 */
		GDAPI virtual SizeTp Read(Handle const array, SizeTp const size, SizeTp const count)
		{
			SizeTp cnt = 0;
			for (; cnt < size * count; ++cnt)
			{
				auto const byte = Read();
				if (byte == -1)
				{
					break;
				}
				static_cast<Byte*>(array)[cnt] = static_cast<Byte>(byte);
			}
			return cnt / size;
		}

		template<typename TValue>
		GDINL bool Read(TValue& value)
		{
			return this->Read(&value, sizeof value, 1) == 1;
		}

		/*!
		 * Reads elements from input stream into vector.
		 * Amount of elements would be read is defined by vector size.
		 *
		 * @param vector The Vector to read into.
		 * @returns True if all elements where successfully read.
		 */
		template<typename TElement>
		GDINL bool Read(Vector<TElement>& vector)
		{
			return this->Read(vector.GetData(), sizeof(TElement), vector.GetLength()) == vector.GetLength();
		}

	};	// class InputStream

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Base class for all classes representing an output stream of bytes.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class OutputStream : public BaseStream
	{
	public:

		/*!
		 * Closes this stream and releases all resources associated with this stream.
		 */
		GDAPI virtual void Close() GD_PURE_VIRTUAL;

		/*!
		 * Writes all unsaved to the resource.
		 */
		GDAPI virtual void Flush() GD_PURE_VIRTUAL;

		/*!
		 * Writes a byte into output.
		 *
		 * @param byte Value that would be written to output.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool Write(Byte const byte) GD_PURE_VIRTUAL;

		/*!
		 * Writes several elements to output.
		 *
		 * @param array Input elements that would be written.
		 * @param size Length of the element in bytes,
		 * @param count Amount of the elements.
		 *
		 * @returns Total amount of elements written.
		 */
		GDAPI virtual SizeTp Write(CHandle const array, SizeTp const size, SizeTp const count)
		{
			SizeTp cnt = 0;
			for (; cnt < size * count; ++cnt)
			{
				if (!Write(static_cast<Byte const*>(array)[cnt]))
				{
					break;
				}
			}
			return cnt / size;
		}

		GDINL bool Write(String const& value)
		{
			return this->Write(value.CStr(), sizeof value[0], value.GetLength()) == value.GetLength();
		}

		template<UInt32 TLength>
		GDINL bool Write(Char const (&value)[TLength])
		{
			return this->Write(value, sizeof value[0], TLength - 1) == TLength - 1;
		}

		template<typename TValue>
		GDINL bool Write(TValue const& value)
		{
			return this->Write(&value, sizeof value, 1) == 1;
		}

		/*!
		 * Writes elements from input stream into vector.
		 * Amount of elements would be written is defined by vector size.
		 *
		 * @param vector The Vector to write into.
		 * @returns True if all elements where successfully written.
		 */
		template<typename TElement>
		GDINL bool Write(Vector<TElement> const& vector)
		{
			return this->Write(vector.GetData(), sizeof(TElement), vector.GetLength()) == vector.GetLength();
		}

	};	// class OutputStream

GD_NAMESPACE_END
