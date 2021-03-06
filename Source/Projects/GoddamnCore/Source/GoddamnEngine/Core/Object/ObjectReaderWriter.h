// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Interface to the serialization readers and writers.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Object.h>

#include <GoddamnEngine/Core/IO/Stream.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Generic serialization reader.
	// **------------------------------------------------------------------------------------------**
	class GD_OBJECT_KERNEL ObjectReader : public IVirtuallyDestructible
	{
		friend struct ObjectReaderVisitor;

	protected:
		enum class CurrentlyReading
		{
			Array,
			Struct,
        };	// enum class CurrentlyReading
		Stack<CurrentlyReading> m_ReadingScope;
		InputStream& m_ReadingStream;

	protected:

		/*!
		 * Initializes the reader interface.
		 * @param readingStream Pointer to the stream, from the one we are reading.
		 */
		GDINL explicit ObjectReader(InputStream& readingStream)
			: m_ReadingStream(readingStream)
		{
		}

		GDINT virtual ~ObjectReader()
		{
			GD_ASSERT(m_ReadingScope.IsEmpty(), "Scoping error.");
		}

	protected:

		// ------------------------------------------------------------------------------------------
		// Properties reading.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Selects next array element.
		 * @returns True if next array element was selected.
		 */
		GDINT virtual bool TrySelectNextArrayElement()
		{
			return m_ReadingScope.GetLast() == CurrentlyReading::Array;
		}

		/*!
		 * Reads name of the property or selects next array element.
		 *
		 * @param name String property name.
		 * @returns True if property with such name was found.
		 */
		GDINT virtual bool TryReadPropertyName(String const& name)
		{
			GD_NOT_USED(name);
			return false;
		}

		/*!
		 * Reads name of the property or selects next array element.
		 *
		 * @param namePtr Pointer to the property name.
		 * @returns True if property with such name was found.
		 */
		GDINL bool TryReadPropertyNameOrSelectNextArrayElement(String const* const namePtr = nullptr)
		{
			if (!TrySelectNextArrayElement())
			{
				GD_ASSERT(namePtr != nullptr);
				return TryReadPropertyName(*namePtr);
			}
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties reading.
		// ------------------------------------------------------------------------------------------
        
		/*!
         * Reads null property.
         * @returns True if property value was successfully read.
         */
        GDINT virtual bool TryReadPropertyValueNull() GD_PURE_VIRTUAL;

        /*!
         * Reads boolean property.
         *
         * @param value Boolean property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(bool& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 8-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(Int8& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 8-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(UInt8& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 16-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(Int16& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 16-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(UInt16& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 32-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(Int32& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 32-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(UInt32& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 64-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(Int64& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 64-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(UInt64& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads 32-bit floating-point property.
         *
         * @param value Float property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(Float32& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads 64-bit floating-point property.
         *
         * @param value Float property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(Float64& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads string property.
         *
         * @param value String property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(String& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads GUID property.
         * Default implementation uses string GUID representation.
         *
         * @param value GUID property value.
         * @returns True if property value was successfully read.
         */
		GDINT virtual bool TryReadPropertyValue(GUID& value)
        {
            String valueString;
            if (TryReadPropertyValue(valueString))
            {
                return GUID::TryParse(valueString, value);
            }
            return false;
        }
        
		// ------------------------------------------------------------------------------------------
		// Array properties reading.
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
		GDINT virtual bool TryBeginReadArrayPropertyValue(SizeTp& arraySize)
		{
			GD_NOT_USED(arraySize);
            m_ReadingScope.InsertLast(CurrentlyReading::Array);
			return false;
		}

		/*!
		 * Notifies end of the array property.
		 */
		GDINT virtual void EndReadArrayPropertyValue()
		{
            GD_ASSERT(m_ReadingScope.GetLast() == CurrentlyReading::Array, "Array scoping error.");
			m_ReadingScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties reading.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Reads structure property.
		 * This function should be called to initialize structure reading. 
		 * Structure properties should be written by 'TryReadPropertyValue*' calls.
		 * After reading the whole structure, 'EndReadStructPropertyValue' is called.
		 *
		 * @returns True if reading struct property was successfully initialized.
		 */
		GDINT virtual bool TryBeginReadStructPropertyValue()
		{
            m_ReadingScope.InsertLast(CurrentlyReading::Struct);
			return false;
		}

		/*!
		 * Notifies end of the structure property.
		 */
		GDINT virtual void EndReadStructPropertyValue()
		{
            GD_ASSERT(m_ReadingScope.GetLast() == CurrentlyReading::Struct, "Struct scoping error.");
			m_ReadingScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Object properties reading.
		// ------------------------------------------------------------------------------------------

	private:
		GDINT bool TryBeginReadObjectPropertyValue(RefPtr<Object>& value);
		GDINT void EndReadObjectPropertyValue();

	public:

		/*!
		 * Reads object.
		 * @returns Pointer to the deserialized object or nullptr on failure.
		 */
		//! @{
		GDAPI RefPtr<Object> ReadObject();
		template<typename TObject>
		GDINL RefPtr<TObject> ReadObject()
		{
			return object_cast<RefPtr<TObject>>(ReadObject());
		}
		//! @}

	};	// class ObjectReader

	// **------------------------------------------------------------------------------------------**
	//! Generic serialization writer.
	// **------------------------------------------------------------------------------------------**
	class GD_OBJECT_KERNEL ObjectWriter : public IVirtuallyDestructible
	{
		friend struct ObjectWriterVisitor;

	protected:
		enum class CurrentlyWriting
		{
			Array,
			Struct,
		};	// enum CurrentlyWriting
		Stack<CurrentlyWriting> m_WritingScope;
		OutputStream& m_WritingStream;

	protected:

		/*!
		 * Initializes the writer interface.
		 * @param writingStream Pointer to the stream to the one we are writing.
		 */
		GDINL explicit ObjectWriter(OutputStream& writingStream)
			: m_WritingStream(writingStream)
		{
		}

		GDINT virtual ~ObjectWriter()
		{
			GD_ASSERT(m_WritingScope.IsEmpty(), "Scoping error.");
		}

		// ------------------------------------------------------------------------------------------
		// Properties writing.
		// ------------------------------------------------------------------------------------------

	protected:

		/*!
		 * Selects next array element.
		 * @returns True if next array element was selected.
		 */
		GDINT virtual bool TrySelectNextArrayElement()
		{
			return m_WritingScope.GetLast() == CurrentlyWriting::Array;
		}

		/*!
		 * Writes name of the property.
		 * @param name String property name.
		 */
		GDINT virtual void WritePropertyName(String const& name)
		{
			GD_NOT_USED(name);
		}

		/*!
		 * Writes name of the property or selects next array element.
		 * @param namePtr Pointer to the property name.
		 */
		GDINT void WritePropertyNameOrSelectNextArrayElement(String const* const namePtr = nullptr)
		{
			if (!TrySelectNextArrayElement())
			{
				GD_ASSERT(namePtr != nullptr);
				WritePropertyName(*namePtr);
			}
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writing.
		// ------------------------------------------------------------------------------------------
        
		/*!
         * Writes null property.
         */
		GDINT virtual void WritePropertyValueNull() GD_PURE_VIRTUAL;

        /*!
         * Writes boolean property.
         * @param value Boolean property value.
         */
		GDINT virtual void WritePropertyValue(bool const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 8-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(Int8 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 8-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(UInt8 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 16-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(Int16 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 16-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(UInt16 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 32-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(Int32 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 32-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(UInt32 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 64-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(Int64 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 64-bit integer property.
         * @param value Integer property value.
         */
		GDINT virtual void WritePropertyValue(UInt64 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes 32-bit floating-point property.
         * @param value Float property value.
         */
		GDINT virtual void WritePropertyValue(Float32 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes 64-bit floating-point property.
         * @param value Float property value.
         */
		GDINT virtual void WritePropertyValue(Float64 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes string property.
         * @param value String property value.
         */
		GDINT virtual void WritePropertyValue(String const& value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes GUID property.
		 * Default implementation uses string GUID representation.
		 *
         * @param value GUID property value.
         */
		GDINT virtual void WritePropertyValue(GUID const& value)
        {
            WritePropertyValue(value.ToString());
        }
        
		// ------------------------------------------------------------------------------------------
		// Array properties writing.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Writes array property.
		 * This function should be called to initialize array writing.
		 * Array elements should be written by 'WritePropertyValue*' calls.
		 * After writing the whole array, 'EndWriteArrayPropertyValue' should be called.
		 *
		 * While writing elements of array, any 'WritePropertyName' call are ignored.
		 */
		GDINT virtual void BeginWriteArrayPropertyValue()
		{
            m_WritingScope.InsertLast(CurrentlyWriting::Array);
		}

		/*!
		 * Notifies end of the array property.
		 */
		GDINT virtual void EndWriteArrayPropertyValue()
		{
            GD_ASSERT(!m_WritingScope.IsEmpty() && m_WritingScope.GetLast() == CurrentlyWriting::Array, "Array scoping error.");
			m_WritingScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties writing.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Writes structure property.
		 * This function should be called to initialize structure writing. 
		 * Structure properties should be written by 'WritePropertyValue*' calls.
		 * After writing the whole structure, 'EndWriteStructPropertyValue' is called.
		 */
		GDINT virtual void BeginWriteStructPropertyValue()
		{
            m_WritingScope.InsertLast(CurrentlyWriting::Struct);
		}

		/*!
		 * Notifies end of the structure property.
		 */
		GDINT virtual void EndWriteStructPropertyValue()
		{
            GD_ASSERT(!m_WritingScope.IsEmpty() && m_WritingScope.GetLast() == CurrentlyWriting::Struct, "Struct scoping error.");
			m_WritingScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Object properties writing.
		// ------------------------------------------------------------------------------------------

	private:
		GDINT void BeginWriteObjectPropertyValue(RefPtr<Object> const& value);
		GDINT void EndWriteObjectPropertyValue();

	public:

		/*!
		 * Writes object.
		 * @param object Object to write.
		 */
		GDAPI void WriteObject(RefPtr<Object> const& object);

	};	// class ISerializationWriter

	// **------------------------------------------------------------------------------------------**
    //! Wrappers for the generic serialization reader.
    // **------------------------------------------------------------------------------------------**
    template<typename TImplementation, typename TSerializationReaderBase = ObjectReader>
    class GD_OBJECT_HELPER TObjectReader : public TSerializationReaderBase
    {
    protected:
        
        /*!
         * Initializes the reader interface.
         * @param readingStream Pointer to the stream, from the one we are reading.
         */
        GDINL explicit TObjectReader(InputStream& readingStream)
            : TSerializationReaderBase(readingStream)
        {
        }
        
    protected:
        
        /*!
         * Reads template property.
         *
         * @param value Template property value.
         * @returns True if property value was successfully read.
         */
        template<typename TValue>
        GDINL bool TryReadPropertyValueImpl(TValue& value)
        {
            GD_NOT_USED_L(this, value);
            GD_NOT_IMPLEMENTED();
        }
        
	private:
		GDINT virtual bool TryReadPropertyValue(bool& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(Int8& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(UInt8& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(Int16& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(UInt16& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(Int32& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(UInt32& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(Int64& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(UInt64& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(Float32& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(Float64& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
		GDINT virtual bool TryReadPropertyValue(String& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
    };  // class TSerializationReader

    // **------------------------------------------------------------------------------------------**
    //! Wrappers for the generic serialization writer.
    // **------------------------------------------------------------------------------------------**
    template<typename TImplementation, typename TSerializationWriterBase = ObjectWriter>
	class GD_OBJECT_HELPER TObjectWriter : public TSerializationWriterBase
    {
    protected:
        
        /*!
         * Initializes the writer interface.
         * @param writingStream Pointer to the stream to the one we are writing.
         */
        GDINL explicit TObjectWriter(OutputStream& writingStream)
            : TSerializationWriterBase(writingStream)
        {
        }
        
    protected:
        
        /*!
         * Writes template property.
         * @param value Template property value.
         */
        template<typename TValue>
        GDINL void WritePropertyValueImpl(TValue const& value)
        {
            GD_NOT_USED_L(this, value);
            GD_NOT_IMPLEMENTED();
        }
        
    private:
		GDINT virtual void WritePropertyValue(bool const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(Int8 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(UInt8 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(Int16 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(UInt16 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(Int32 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(UInt32 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(Int64 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(UInt64 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(Float32 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(Float64 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
		GDINT virtual void WritePropertyValue(String const& value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
    };  // class TSerializationWriter

GD_NAMESPACE_END
