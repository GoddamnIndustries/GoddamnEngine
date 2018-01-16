// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Misc/Endian.h
 * File contains utilities for endian conversion.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	// Base swapping functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Swaps endian for the unsigned 8-bit integer.
	 * Actually, does nothing.
	 */
	GDINL static UInt8 SwapEndian(UInt8 const value)
	{
		return value;
	}

	/*!
	 * Swaps endian for the signed 8-bit integer.
	 * Actually, does nothing.
	 */
	GDINL static Int8 SwapEndian(Int8 const value)
	{
		return value;
	}

	/*!
	 * Swaps endian for the unsigned 16-bit integer.
	 */
	GDINL static UInt16 SwapEndian(UInt16 const value)
	{
		return (value & 0x00FF) << 8 
			 | (value & 0xFF00) >> 8;
	}

	/*!
	 * Swaps endian for the signed 16-bit integer.
	 */
	GDINL static Int16 SwapEndian(Int16 const value)
	{
		return union_cast<Int16>(SwapEndian(union_cast<UInt16>(value)));
	}

	/*!
	 * Swaps endian for the unsigned 32-bit integer.
	 */
	GDINL static UInt32 SwapEndian(UInt32 const value)
	{
		return (value & 0x000000FF) << 24
			 | (value & 0x0000FF00) << 8
			 | (value & 0x00FF0000) >> 8
			 | (value & 0xFF000000) >> 24;
	}

	/*!
	 * Swaps endian for the signed 32-bit integer.
	 */
	GDINL static Int32 SwapEndian(Int32 const value)
	{
		return union_cast<Int32>(SwapEndian(union_cast<UInt32>(value)));
	}

	/*!
	 * Swaps endian for the unsigned 64-bit integer.
	 */
	GDINL static UInt64 SwapEndian(UInt64 const value)
	{
		return (value & 0x00000000000000FF) << 56
			 | (value & 0x000000000000FF00) << 40
			 | (value & 0x0000000000FF0000) << 24
			 | (value & 0x00000000FF000000) << 8
			 | (value & 0x000000FF00000000) >> 8
			 | (value & 0x0000FF0000000000) >> 24
			 | (value & 0x00FF000000000000) >> 40
			 | (value & 0xFF00000000000000) >> 56;
	}

	/*!
	 * Swaps endian for the signed 64-bit integer.
	 */
	GDINL static Int64 SwapEndian(Int64 const value)
	{
		return union_cast<Int64>(SwapEndian(union_cast<UInt64>(value)));
	}

	/*!
	 * Swaps endian for the signed 32-bit floating point value.
	 */
	GDINL static Float32 SwapEndian(Float32 const value)
	{
		return union_cast<Float32>(SwapEndian(union_cast<UInt32>(value)));
	}

	/*!
	 * Swaps endian for the signed 64-bit floating point value.
	 */
	GDINL static Float64 SwapEndian(Float64 const value)
	{
		return union_cast<Float64>(SwapEndian(union_cast<UInt64>(value)));
	}

	// ------------------------------------------------------------------------------------------
	// Template swapping functions.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Swaps to little endian.
	 * @param value The value to swap.
	 */
	template<typename TValue>
	GDINL static void SwapToLittleEndian(TValue& value)
	{
#if GD_ENDIANITY_BIG
		value = GD::SwapEndian(value);
#endif	// if GD_ENDIANITY_BIG
		GD_NOT_USED(value);
	}

	/*!
	 * Swaps to little endian.
	 * @param valuePtr Pointer to the array.
	 * @param count Array length.
	 */
	template<typename TValue>
	GDINL static void SwapToLittleEndianArray(TValue* valuePtr, SizeTp const count)
	{
#if GD_ENDIANITY_BIG
		for (SizeTp cnt = 0; cnt < count; ++cnt)
		{
			GD::SwapToLittleEndian(valuePtr[cnt]);
		}
#endif	// if GD_ENDIANITY_BIG
		GD_NOT_USED(valuePtr);
		GD_NOT_USED(count);
	}

	/*!
	 * Swaps to little endian.
	 * @param valuePtr Pointer to the array.
	 * @param size The size of array in bytes.
	 */
	template<typename TValue>
	GDINL static void SwapToLittleEndianArraySized(TValue* valuePtr, SizeTp const size)
	{
		GD_DEBUG_VERIFY(size % sizeof *valuePtr == 0, "Incorrect size of data specified.");
		GD::SwapToLittleEndianArray(valuePtr, size / sizeof *valuePtr);
	}

	/*!
	 * Swaps to big endian.
	 * @param value The value to swap.
	 */
	template<typename TValue>
	GDINL static void SwapToBigEndian(TValue& value)
	{
#if GD_ENDIANITY_LITTLE
		value = GD::SwapEndian(value);
#endif	// if GD_ENDIANITY_LITTLE
		GD_NOT_USED(value);
	}

	/*!
	 * Swaps to big endian.
	 * @param valuePtr Pointer to the array.
	 * @param count Array length.
	 */
	template<typename TValue>
	GDINL static void SwapToBigEndianArray(TValue* valuePtr, SizeTp const count)
	{
#if GD_ENDIANITY_LITTLE
		for (SizeTp cnt = 0; cnt < count; ++cnt)
		{
			GD::SwapToBigEndian(valuePtr[cnt]);
		}
#endif	// if GD_ENDIANITY_LITTLE
		GD_NOT_USED(valuePtr);
		GD_NOT_USED(count);
	}

	/*!
	 * Swaps to big endian.
	 * @param valuePtr Pointer to the array.
	 * @param size The size of array in bytes.
	 */
	template<typename TValue>
	GDINL static void SwapToLittleBigArraySized(TValue* valuePtr, SizeTp const size)
	{
		GD_DEBUG_VERIFY(size % sizeof *valuePtr == 0, "Incorrect size of data specified.");
		GD::SwapToBigEndianArray(valuePtr, size / sizeof *valuePtr);
	}

	/*!
	 * Swaps to endian, used by engine to store data.
	 * @param value The value to swap.
	 */
	template<typename TValue>
	GDINL static void SwapToEngineEndian(TValue& value)
	{
		// Due to all primary target platforms are x86/ARM, we use little endian as default. 
		GD::SwapToLittleEndian(value);
	}

	/*!
	 * Swaps to endian, used by engine to store data.
	 * @param valuePtr Pointer to the array.
	 * @param count Array length.
	 */
	template<typename TValue>
	GDINL static void SwapToEngineEndianArray(TValue* valuePtr, SizeTp const count)
	{
		GD::SwapToLittleEndianArray(valuePtr, count);
	}

	/*!
	 * Swaps to endian, used by engine to store data.
	 * @param valuePtr Pointer to the array.
	 * @param size The size of array in bytes.
	 */
	template<typename TValue>
	GDINL static void SwapToEngineEndianArraySized(TValue* valuePtr, SizeTp const size)
	{
		GD::SwapToLittleEndianArraySized(valuePtr, size);
	}

	/*!
	 * Swaps to endian, used by CPU to store data.
	 * @param value The value to swap.
	 */
	template<typename TValue>
	GDINL static void SwapToSystemEndian(TValue& value)
	{
#if GD_ENDIANITY_LITTLE
		GD::SwapToLittleEndian(value);
#endif	// if GD_ENDIANITY_LITTLE
#if GD_ENDIANITY_BIG
		GD::SwapToBigEndian(value);
#endif	// if GD_ENDIANITY_BIG
	}

	/*!
	 * Swaps to endian, used by CPU to store data.
	 * @param valuePtr Pointer to the array.
	 * @param count Array length.
	 */
	template<typename TValue>
	GDINL static void SwapToSystemEndianArray(TValue* valuePtr, SizeTp const count)
	{
#if GD_ENDIANITY_LITTLE
		GD::SwapToLittleEndianArray(valuePtr, count);
#endif	// if GD_ENDIANITY_LITTLE
#if GD_ENDIANITY_BIG
		GD::SwapToBigEndianArray(valuePtr, count);
#endif	// if GD_ENDIANITY_BIG
	}

	/*!
	 * Swaps to endian, used by CPU to store data.
	 * @param valuePtr Pointer to the array.
	 * @param size The size of array in bytes.
	 */
	template<typename TValue>
	GDINL static void SwapToSystemBigArraySized(TValue* valuePtr, SizeTp const size)
	{
#if GD_ENDIANITY_LITTLE
		GD::SwapToLittleEndianArraySized(valuePtr, size);
#endif	// if GD_ENDIANITY_LITTLE
#if GD_ENDIANITY_BIG
		GD::SwapToBigEndianArraySized(valuePtr, size);
#endif	// if GD_ENDIANITY_BIG
	}

GD_NAMESPACE_END
