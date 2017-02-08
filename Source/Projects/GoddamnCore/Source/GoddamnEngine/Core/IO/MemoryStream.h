// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/IO/Stream.h
 * File contains in-memory stream interfaces.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream.h>

#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Input stream implementation for in-memory implementations.
	//! @warning All operations are no thread-safe.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class MemoryInputStream : public InputStream
	{
	private:
		Byte const* m_Memory;
		SizeTp      m_MemoryLength;
		SizeTp		m_MemoryPosition;

	public:

		/*!
		 * Initializes the in-memory input stream.
		 * 
		 * @param memory The memory to read from.
		 * @param memoryLength Length of the memory block.
		 */
		GDINL MemoryInputStream(CHandle const memory, SizeTp const memoryLength)
			: m_Memory(static_cast<Byte const*>(memory)), m_MemoryLength(memoryLength), m_MemoryPosition(0)
		{
			GD_DEBUG_VERIFY(m_Memory != nullptr, "Invalid memory specified.");
		}

	public:

		// ------------------------------------------------------------------------------------------
		// BaseStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns true if this stream is valid and ready for I/O operations.
		 */
		GDINT virtual bool IsValid() const override final
		{
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// InputStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns current position in stream or -1 on error.
		 */
		GDINT virtual SizeTp GetPosition() const override final
		{
			return m_MemoryPosition;
		}

		/*!
		 * Returns size of data that stream handles or -1 on error.
		 */
		GDINT virtual SizeTp GetLength() const override final
		{
			return m_MemoryLength;
		}

		/*!
		 * Closes this stream and releases all resources associated with this stream.
		 */
		GDINT virtual void Close() override final {}

		/*!
		 * Reposition this stream to new specified position.
		 *
		 * @param offset The offset in bytes from specified origin.
		 * @param origin Defines origin from which point make offset.
		 *
		 * @returns New position in file, -1 on error.
		 */
		GDINT virtual SizeTp Seek(PtrDiffTp const offset, SeekOrigin const origin = SeekOrigin::Current) override final
		{
			SizeTp newMemoryPosition;
			switch (origin)
			{
				case SeekOrigin::SeekSet: 
					newMemoryPosition = 0; 
					break;
				case SeekOrigin::SeekCur: 
					newMemoryPosition = m_MemoryPosition; 
					break;
				case SeekOrigin::SeekEnd: 
					newMemoryPosition = m_MemoryLength; 
					break;
				default: 
					GD_DEBUG_VERIFY_FALSE("Invalid origin specified.");
			}
			newMemoryPosition += offset;
			if (newMemoryPosition <= m_MemoryLength)
			{
				return m_MemoryPosition = newMemoryPosition;
			}
			return SizeTpMax;
		}

		/*!
		 * Reads next byte from input stream.
		 * @returns Read byte from stream, or -1 on the error.
		 */
		GDINT virtual Int16 Read() override final
		{
			if (m_MemoryPosition < m_MemoryLength)
			{
				++m_MemoryPosition;
				return static_cast<Int16>(m_Memory[m_MemoryPosition - 1]);
			}
			return -1;
		}

		/*!
		 * Reads several elements from input stream.
		 *
		 * @param array Output memory to which data would be written.
		 * @param size Length of the element in bytes,
		 * @param count Amount of the elements.
		 *
		 * @returns Total amount of elements read.
		 */
		GDINT virtual SizeTp Read(Handle const array, SizeTp const size, SizeTp const count) override final
		{
			auto dataLength = size * count;
			auto newMemoryPosition = m_MemoryPosition + dataLength;
			if (newMemoryPosition > m_MemoryLength)
			{
				auto const deltaDataLength = newMemoryPosition - m_MemoryLength;
				newMemoryPosition = m_MemoryLength;
				dataLength -= deltaDataLength;
			}
			CMemory::Memcpy(array, m_Memory + m_MemoryPosition, dataLength);
			m_MemoryPosition = newMemoryPosition;
			return dataLength / size;
		}

	};	// class MemoryInputStream

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Output stream implementation for in-memory implementations.
	//! @warning All operations are no thread=safe.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class MemoryOutputStream : public OutputStream
	{
	private:
		Vector<Byte> m_Memory;

	public:

		/*!
		 * Returns data, that was written to this stream.
		 */
		GDINL Vector<Byte> const& GetData() const
		{
			return m_Memory;
		}

		// ------------------------------------------------------------------------------------------
		// BaseStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns true if this stream is valid and ready for I/O operations.
		 */
		GDINT virtual bool IsValid() const override final
		{
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// InputStream interface.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Closes this stream and releases all resources associated with this stream.
		 */
		GDINT virtual void Close() override final
		{
			m_Memory.Clear();
		}

		/*!
		 * Writes all unsaved to the resource.
		 */
		GDINT virtual void Flush() override final {}

		/*!
		 * Writes a byte into output.
		 *
		 * @param byte Value that would be written to output.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool Write(Byte const byte) override final
		{
			m_Memory.InsertLast(byte);
			return true;
		}

		/*!
		 * Writes several elements to output.
		 *
		 * @param array Input elements that would be written.
		 * @param size Length of the element in bytes,
		 * @param count Amount of the elements.
		 *
		 * @returns Total amount of elements written.
		 */
		GDINT virtual SizeTp Write(CHandle const array, SizeTp const size, SizeTp const count) override final
		{
			auto const memoryLocation = m_Memory.GetLength();
			m_Memory.Resize(static_cast<UInt32>(memoryLocation + size * count));
			CMemory::Memcpy(m_Memory.GetData() + memoryLocation, array, size * count);
			return count;
		}

	};	// class MemoryOutputStream

GD_NAMESPACE_END
