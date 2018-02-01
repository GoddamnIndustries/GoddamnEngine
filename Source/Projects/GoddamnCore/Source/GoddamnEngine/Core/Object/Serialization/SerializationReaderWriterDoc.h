// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Interface to the Document-based serialization readers and writers.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Serialization/Doc/Doc.h>
#include <GoddamnEngine/Core/Object/Serialization/SerializationReaderWriter.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Interface for the document-based serialization reader.
	// **------------------------------------------------------------------------------------------**
	class SerializationReaderDoc : public TSerializationReader<SerializationReaderDoc>
	{
	private:
        using SerializationReaderDocBase = TSerializationReader<SerializationReaderDoc>;
		struct DocValueArrayScope
		{
			DocValueVector Array;
			UInt32 mutable CurrentIndex;
		};	// struct DocValueArrayScope
        struct DocObjectPtrScope
        {
            DocObjectPtr Object;
        };  // struct DocObjectPtrScope

		DocValuePtr m_SelectedValue;
		Stack<DocObjectPtrScope> m_ObjectsScope;
		Stack<DocValueArrayScope> m_ArraysScope;

	protected:
        
        /*!
         * Initializes the Document-based reader interface.
         *
         * @param doc Serialized data document.
         * @param docReadingStream Pointer to the stream, from the one we are reading.
         */
		GDINL explicit SerializationReaderDoc(DocPtr const doc, InputStream& docReadingStream)
			: SerializationReaderDocBase(docReadingStream)
		{
			auto result = doc->_TryParseDocument(docReadingStream);
            m_ObjectsScope.InsertLast({ result.RootObject });
		}

	public:
		GDAPI virtual bool TryReadPropertyName(String const& name) override
		{
			if (!SerializationReaderDocBase::TryReadPropertyName(name))
			{
				// Selecting top object's property by name.
				GD_DEBUG_VERIFY(!m_ObjectsScope.IsEmpty(), "Object scoping error.");
				
				auto const topDocObject = m_ObjectsScope.GetLast().Object;
				m_SelectedValue = topDocObject->_GetProperty(name);
				return m_SelectedValue != nullptr;
			}
            else
            {
                // Selecting next element of the array.
                GD_DEBUG_VERIFY(!m_ArraysScope.IsEmpty(), "Array scoping error.");
                
                auto const& topDocArray = m_ArraysScope.GetLast();
                GD_DEBUG_VERIFY(topDocArray.CurrentIndex < topDocArray.Array.GetLength(), "Reading array element runs out of bounds.");
                m_SelectedValue = topDocArray.Array[topDocArray.CurrentIndex++];
                return true;
            }
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties readers.
		// ------------------------------------------------------------------------------------------

		template<typename TValue>
		GDINL bool TryReadPropertyValueImpl(TValue& value)
		{
			GD_DEBUG_VERIFY(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
			return m_SelectedValue->_TryGetValue(value);
		}

		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		GDAPI virtual bool TryBeginReadArrayPropertyValue(SizeTp& arraySize) override
		{
			if (!SerializationReaderDocBase::TryBeginReadArrayPropertyValue(arraySize))
			{
				DocValueVector value;
				if (!TryReadPropertyValueImpl(value))
				{
					SerializationReaderDocBase::EndReadArrayPropertyValue();
					return false;
				}
				arraySize = value.GetLength();
				m_ArraysScope.InsertLast({ value, 0 });
			}
			return true;
		}
			
		GDAPI virtual void EndReadArrayPropertyValue() override
		{
			SerializationReaderDocBase::EndReadArrayPropertyValue();
			m_ArraysScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL virtual bool TryBeginReadStructPropertyValue() override
		{
			if (!SerializationReaderDocBase::TryBeginReadStructPropertyValue())
			{
				// If we are trying to parse root object.
				if (!m_ObjectsScope.IsEmpty())
				{
					DocObjectPtr value;
					if (!TryReadPropertyValueImpl(value))
					{
						SerializationReaderDocBase::EndReadArrayPropertyValue();
						return false;
					}
                    m_ObjectsScope.InsertLast({ value });
				}
			}
			return true;
		}

		GDINL virtual void EndReadStructPropertyValue() override
		{
			SerializationReaderDocBase::EndReadStructPropertyValue();
			m_ObjectsScope.EraseLast();
		}

	};	// class SerializationReaderDoc

    // **------------------------------------------------------------------------------------------**
    //! Interface for the document-based serialization reader.
    // **------------------------------------------------------------------------------------------**
    class SerializationWriterDoc : public TSerializationWriter<SerializationWriterDoc>
    {
    protected:
        
        /*!
         * Initializes the document-based writer interface.
         * @param docWritingStream Pointer to the stream to the one we are writing.
         */
        GDINL explicit SerializationWriterDoc(OutputStream& docWritingStream)
            : TSerializationWriter<SerializationWriterDoc>(docWritingStream)
        {
        }
        
    };  // class SerializationWriterDoc

GD_NAMESPACE_END
