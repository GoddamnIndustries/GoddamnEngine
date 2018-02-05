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
	GD_OBJECT_KERNEL class ObjectReader : public IVirtuallyDestructible
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
		GDAPI explicit ObjectReader(InputStream& readingStream)
			: m_ReadingStream(readingStream)
		{
		}

		GDAPI virtual ~ObjectReader()
		{
			GD_DEBUG_VERIFY(m_ReadingScope.IsEmpty(), "Scoping error.");
		}

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

	protected:

		// ------------------------------------------------------------------------------------------
		// Properties reading.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Selects next array element.
		 * @returns True if next array element was selected.
		 */
		GDAPI virtual bool TrySelectNextArrayElement()
		{
			return m_ReadingScope.GetLast() == CurrentlyReading::Array;
		}

		/*!
		 * Reads name of the property or selects next array element.
		 *
		 * @param name String property name.
		 * @returns True if property with such name was found.
		 */
		GDAPI virtual bool TryReadPropertyName(String const& name)
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
		GDAPI bool TryReadPropertyNameOrSelectNextArrayElement(String const* const namePtr = nullptr)
		{
			if (!TrySelectNextArrayElement())
			{
				GD_DEBUG_VERIFY(namePtr != nullptr);
				return TryReadPropertyName(*namePtr);
			}
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties reading.
		// ------------------------------------------------------------------------------------------
        
        /*!
         * Reads boolean property.
         *
         * @param value Boolean property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(bool& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 8-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(Int8& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 8-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(UInt8& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 16-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(Int16& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 16-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(UInt16& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 32-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(Int32& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 32-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(UInt32& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads signed 64-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(Int64& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads unsigned 64-bit integer property.
         *
         * @param value Integer property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(UInt64& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads 32-bit floating-point property.
         *
         * @param value Float property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(Float32& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads 64-bit floating-point property.
         *
         * @param value Float property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(Float64& value) GD_PURE_VIRTUAL;
        
        /*!
         * Reads string property.
         *
         * @param value String property value.
         * @returns True if property value was successfully read.
         */
        GDAPI virtual bool TryReadPropertyValue(String& value) GD_PURE_VIRTUAL;
        
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
		GDAPI virtual bool TryBeginReadArrayPropertyValue(SizeTp& arraySize)
		{
			GD_NOT_USED(arraySize);
            m_ReadingScope.InsertLast(CurrentlyReading::Array);
			return false;
		}

		/*!
		 * Notifies end of the array property.
		 */
		GDAPI virtual void EndReadArrayPropertyValue()
		{
            GD_DEBUG_VERIFY(m_ReadingScope.GetLast() == CurrentlyReading::Array, "Array scoping error.");
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
		GDAPI virtual bool TryBeginReadStructPropertyValue()
		{
            m_ReadingScope.InsertLast(CurrentlyReading::Struct);
			return false;
		}

		/*!
		 * Notifies end of the structure property.
		 */
		GDAPI virtual void EndReadStructPropertyValue()
		{
            GD_DEBUG_VERIFY(m_ReadingScope.GetLast() == CurrentlyReading::Struct, "Struct scoping error.");
			m_ReadingScope.EraseLast();
		}

	};	// class ObjectReader

	// **------------------------------------------------------------------------------------------**
	//! Generic serialization writer.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL class ObjectWriter : public IVirtuallyDestructible
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

		GDINL virtual ~ObjectWriter()
		{
			GD_DEBUG_VERIFY(m_WritingScope.IsEmpty(), "Scoping error.");
		}

	public:

		/*!
		 * Writes object.
		 * @param object Object to write.
		 */
		GDAPI void WriteObject(RefPtr<Object> const& object);

		// ------------------------------------------------------------------------------------------
		// Properties writing.
		// ------------------------------------------------------------------------------------------

	protected:

		/*!
		 * Selects next array element.
		 * @returns True if next array element was selected.
		 */
		GDAPI virtual bool TrySelectNextArrayElement()
		{
			return m_WritingScope.GetLast() == CurrentlyWriting::Array;
		}

		/*!
		 * Writes name of the property.
		 * @param name String property name.
		 */
		GDAPI virtual void WritePropertyName(String const& name)
		{
			GD_NOT_USED(name);
		}

		/*!
		 * Writes name of the property or selects next array element.
		 *
		 * @param namePtr Pointer to the property name.
		 * @returns False if property name should not be written (e.g. while serializing arrays).
		 */
		GDAPI void WritePropertyNameOrSelectNextArrayElement(String const* const namePtr = nullptr)
		{
			if (!TrySelectNextArrayElement())
			{
				GD_DEBUG_VERIFY(namePtr != nullptr);
				WritePropertyName(*namePtr);
			}
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writing.
		// ------------------------------------------------------------------------------------------
        
        /*!
         * Writes boolean property.
         * @param value Boolean property value.
         */
        GDAPI virtual void WritePropertyValue(bool const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 8-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(Int8 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 8-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(UInt8 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 16-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(Int16 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 16-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(UInt16 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 32-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(Int32 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 32-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(UInt32 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes signed 64-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(Int64 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes unsigned 64-bit integer property.
         * @param value Integer property value.
         */
        GDAPI virtual void WritePropertyValue(UInt64 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes 32-bit floating-point property.
         * @param value Float property value.
         */
        GDAPI virtual void WritePropertyValue(Float32 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes 64-bit floating-point property.
         * @param value Float property value.
         */
        GDAPI virtual void WritePropertyValue(Float64 const value) GD_PURE_VIRTUAL;
        
        /*!
         * Writes string property.
         * @param value String property value.
         */
        GDAPI virtual void WritePropertyValue(String const& value) GD_PURE_VIRTUAL;
        
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
		GDINL virtual void BeginWriteArrayPropertyValue()
		{
            m_WritingScope.InsertLast(CurrentlyWriting::Array);
		}

		/*!
		 * Notifies end of the array property.
		 */
		GDAPI virtual void EndWriteArrayPropertyValue()
		{
            GD_DEBUG_VERIFY(!m_WritingScope.IsEmpty() && m_WritingScope.GetLast() == CurrentlyWriting::Array, "Array scoping error.");
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
		GDAPI virtual void BeginWriteStructPropertyValue()
		{
            m_WritingScope.InsertLast(CurrentlyWriting::Struct);
		}

		/*!
		 * Notifies end of the structure property.
		 */
		GDAPI virtual void EndWriteStructPropertyValue()
		{
            GD_DEBUG_VERIFY(!m_WritingScope.IsEmpty() && m_WritingScope.GetLast() == CurrentlyWriting::Struct, "Struct scoping error.");
			m_WritingScope.EraseLast();
		}

	};	// class ISerializationWriter

	// **------------------------------------------------------------------------------------------**
    //! Wrappers for the generic serialization reader.
    // **------------------------------------------------------------------------------------------**
    template<typename TImplementation, typename TSerializationReaderBase = ObjectReader>
    GD_OBJECT_HELPER class TObjectReader : public TSerializationReaderBase
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
        
    public:
        GDAPI virtual bool TryReadPropertyValue(bool& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(Int8& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(UInt8& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(Int16& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(UInt16& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(Int32& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(UInt32& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(Int64& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(UInt64& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(Float32& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(Float64& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
        
        GDAPI virtual bool TryReadPropertyValue(String& value) override
        {
            return static_cast<TImplementation*>(this)->TryReadPropertyValueImpl(value);
        }
                
    };  // class TSerializationReader

    // **------------------------------------------------------------------------------------------**
    //! Wrappers for the generic serialization writer.
    // **------------------------------------------------------------------------------------------**
    template<typename TImplementation, typename TSerializationWriterBase = ObjectWriter>
	GD_OBJECT_HELPER class TObjectWriter : public TSerializationWriterBase
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
        
    public:
        GDAPI virtual void WritePropertyValue(bool const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(Int8 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(UInt8 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(Int16 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(UInt16 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(Int32 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(UInt32 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(Int64 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(UInt64 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(Float32 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(Float64 const value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
        
        GDAPI virtual void WritePropertyValue(String const& value) override
        {
            static_cast<TImplementation*>(this)->WritePropertyValueImpl(value);
        }
                
    };  // class TSerializationWriter

GD_NAMESPACE_END
