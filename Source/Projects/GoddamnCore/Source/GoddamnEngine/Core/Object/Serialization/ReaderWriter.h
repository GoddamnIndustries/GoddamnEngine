// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/ReaderWriter.h
 * File contains base interface to the serialization readers and writers.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/System/IO/Stream.h>

#define gd_s_virtual /*virtual*/
#define gd_s_override /*override*/

#define gd_s_pure_virtual_ \
	{ \
		GD_NOT_SUPPORTED(); \
	} \

#define gd_s_pure_virtual_v \
	{ \
		GD_NOT_USED(value); \
		GD_NOT_SUPPORTED(); \
	} \

#define gd_s_pure_virtual_n \
	{ \
		GD_NOT_USED(name); \
		GD_NOT_SUPPORTED(); \
	} \

#define gd_s_pure_virtual_vn \
	{ \
		GD_NOT_USED(name); \
		GD_NOT_USED(value); \
		GD_NOT_SUPPORTED(); \
	} \

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Interface for the serialization reader - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class IGenericReader : public TNonCopyable
	{
		// ReSharper disable CppHiddenFunction

	protected:
		enum CurrentlyReading
		{
			CurrentlyReadingArray,
			CurrentlyReadingStruct,
		};	// enum CurrentlyReading
		Stack<CurrentlyReading> m_ReadingScope;
		SharedPtr<InputStream > m_ReadingStream;

	protected:

		/*!
		 * Initializes the reader interface.
		 * @param readingStream Pointer to the stream, from the one we are reading.
		 */
		GDINL explicit IGenericReader(SharedPtr<InputStream> const readingStream)
			: m_ReadingStream(readingStream)
		{
			GD_DEBUG_ASSERT(m_ReadingStream != nullptr, "Null pointer reading stream was specified.");
		}

		GDINL ~IGenericReader()
		{
			GD_DEBUG_ASSERT(m_ReadingScope.IsEmpty(), "Scoping error.");
		}

	public:

		/*!
		 * Reads name of the property.
		 *
		 * @param name String property name.
		 * @returns True if property with such name was found.
		 */
		GDINL gd_s_virtual bool TryReadPropertyName(String const& name)
		{
			GD_NOT_USED(name);
			return m_ReadingScope.IsEmpty() ? false : m_ReadingScope.GetLast() == CurrentlyReadingArray;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writers.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Reads template property.
		 *
		 * @param value Template property value.
		 * @returns True if property value was successfully read. 
		 */
		template<typename TValue>
		GDINL gd_s_virtual bool TryReadPropertyValue(TValue& value) gd_s_pure_virtual_v;

		// ------------------------------------------------------------------------------------------
		// Array properties writers.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Reads array property.
		 * This function should be called to initialize array reading.
		 * Array elements should be read by 'TryReadPropertyValue*' calls.
		 * After reading the whole array, 'EndReadArrayPropertyValue' should be called.
		 *
		 * While reading elements of array, any 'TryReadPropertyName' call are ignored and return true.
		 *
		 * @param arraySize Output size of array we are going to read. Would be set to 0 on failure.
		 * @returns True if reading array property was successfully initialized.
		 */
		GDINL gd_s_virtual bool TryBeginReadArrayPropertyValue(UInt32& arraySize)
		{
			GD_NOT_USED(arraySize);
			m_ReadingScope.InsertLast(CurrentlyReadingArray);
			return false;
		}

		/*!
		 * Notifies end of the array property.
		 */
		GDINL gd_s_virtual void EndReadArrayPropertyValue()
		{
			GD_DEBUG_ASSERT(!m_ReadingScope.IsEmpty() && m_ReadingScope.GetLast() == CurrentlyReadingArray, "Array scoping error.");
			m_ReadingScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties writers.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Reads structure property.
		 * This function should be called to initialize structure reading. 
		 * Structure properties should be written by 'TryReadPropertyValue*' calls.
		 * After reading the whole structure, 'EndReadStructPropertyValue' is called.
		 *
		 * @returns True if reading struct property was successfully initialized.
		 */
		GDINL gd_s_virtual bool TryBeginReadStructPropertyValue()
		{
			m_ReadingScope.InsertLast(CurrentlyReadingStruct);
			return false;
		}

		/*!
		 * Notifies end of the structure property.
		 */
		GDINL gd_s_virtual void EndReadStructPropertyValue()
		{
			GD_DEBUG_ASSERT(!m_ReadingScope.IsEmpty() && m_ReadingScope.GetLast() == CurrentlyReadingStruct, "Struct scoping error.");
			m_ReadingScope.EraseLast();
		}

		// ReSharper restore CppHiddenFunction
	};	// class IGenericReader

	// **------------------------------------------------------------------------------------------**
	//! Interface for the serialization writer - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class IGenericWriter : public IVirtuallyDestructible
	{
	protected:
		enum CurrentlyWriting
		{
			CurrentlyWritingArray,
			CurrentlyWritingStruct,
		};	// enum CurrentlyWriting
		Stack<CurrentlyWriting> m_WritingScope;
		SharedPtr<OutputStream> m_WritingStream;

	protected:

		/*!
		 * Initializes the reader interface.
		 * @param readingStream Pointer to the stream, from the one we are reading.
		 */
		GDINL explicit IGenericWriter(SharedPtr<OutputStream> const writingStream)
			: m_WritingStream(writingStream)
		{
			GD_DEBUG_ASSERT(m_WritingStream != nullptr, "Null pointer reading stream was specified.");
		}

		GDINL ~IGenericWriter()
		{
			GD_DEBUG_ASSERT(m_WritingScope.IsEmpty(), "Scoping error.");
		}

	public:
		// ReSharper disable CppHiddenFunction

		/*!
		 * Writes name of the property.
		 *
		 * @param name String property name.
		 * @returns True false if property name should not be written (serializing arrays).
		 */
		GDINL gd_s_virtual bool WritePropertyName(String const& name)
		{
			GD_NOT_USED(name);
			return m_WritingScope.IsEmpty() ? false : m_WritingScope.GetLast() != CurrentlyWritingArray;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writers.
		// ------------------------------------------------------------------------------------------
		
		/*!
		 * Writes template property.
		 * @param value Template property value.
		 */
		template<typename TValue>
		GDINL gd_s_virtual void WritePropertyValue(TValue const value) gd_s_pure_virtual_v;

		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Writes array property.
		 * This function should be called to initialize array writing.
		 * Array elements should be written by 'WritePropertyValue*' calls.
		 * After writing the whole array, 'EndWriteArrayPropertyValue' should be called.
		 *
		 * While writing elements of array, any 'WritePropertyName' call are ignored.
		 */
		GDINL gd_s_virtual void BeginWriteArrayPropertyValue()
		{
			m_WritingScope.InsertLast(CurrentlyWritingArray);
		}

		/*!
		 * Notifies end of the array property.
		 */
		GDINL gd_s_virtual void EndWriteArrayPropertyValue()
		{
			GD_DEBUG_ASSERT(!m_WritingScope.IsEmpty() && m_WritingScope.GetLast() == CurrentlyWritingArray, "Array scoping error.");
			m_WritingScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Writes structure property.
		 * This function should be called to initialize structure writing. 
		 * Structure properties should be written by 'WritePropertyValue*' calls.
		 * After writing the whole structure, 'EndWriteStructPropertyValue' is called.
		 */
		GDINL gd_s_virtual void BeginWriteStructPropertyValue()
		{
			m_WritingScope.InsertLast(CurrentlyWritingStruct);
		}

		/*!
		 * Notifies end of the structure property.
		 */
		GDINL gd_s_virtual void EndWriteStructPropertyValue()
		{
			GD_DEBUG_ASSERT(!m_WritingScope.IsEmpty() && m_WritingScope.GetLast() == CurrentlyWritingStruct, "Struct scoping error.");
			m_WritingScope.EraseLast();
		}

		// ReSharper restore CppHiddenFunction
	};	// class IGenericWriter

GD_NAMESPACE_END
