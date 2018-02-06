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
#include <GoddamnEngine/Core/Object/ObjectReaderWriter.h>
#include <GoddamnEngine/Core/Object/Serialization/Doc/Doc.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Interface for the document-based serialization reader.
	// **------------------------------------------------------------------------------------------**
	class ObjectReaderDoc : public TObjectReader<ObjectReaderDoc>
	{
        using ObjectReaderDocBase = TObjectReader<ObjectReaderDoc>;
        
	private:
		struct DocValueArrayScope
		{
			DocValueVector Array;
			UInt32 mutable CurrentIndex = 0;
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
		GDAPI explicit ObjectReaderDoc(DocPtr const& doc, InputStream& docReadingStream)
			: ObjectReaderDocBase(docReadingStream)
		{
			auto const result = doc->_TryParseDocument(docReadingStream);
			switch (result.RootValue->_GetTypeInfo())
			{
				case DocValueTypeInfo::Object:
					{
						DocObjectPtr docObject;
						result.RootValue->_TryGetValue(docObject);
						m_ObjectsScope.InsertLast({ docObject });
						m_ReadingScope.InsertLast(CurrentlyReading::Struct);
                    }
					break;
				case DocValueTypeInfo::Array:
					{
						DocValueVector docArray;
						result.RootValue->_TryGetValue(docArray);
						m_ArraysScope.InsertLast({ docArray, 0 });
						m_ReadingScope.InsertLast(CurrentlyReading::Array);
                    }
					break;
				default:
					GD_NOT_SUPPORTED();
			}
		}
        
        GDAPI virtual ~ObjectReaderDoc()
        {
            m_ReadingScope.EraseLast();
        }

		// ------------------------------------------------------------------------------------------
		// Properties reading.
		// ------------------------------------------------------------------------------------------

	public:
		GDAPI virtual bool TryReadPropertyName(String const& name) override
		{
			// Selecting top object's property by name.
			GD_DEBUG_VERIFY(!m_ObjectsScope.IsEmpty(), "Object scoping error.");

			auto const topDocObject = m_ObjectsScope.GetLast().Object;
			m_SelectedValue = topDocObject->_GetProperty(name);
			return m_SelectedValue != nullptr;
		}

		GDAPI virtual bool TrySelectNextArrayElement() override
		{
			if (ObjectReaderDocBase::TrySelectNextArrayElement())
			{
				// Selecting next element of the array.
				GD_DEBUG_VERIFY(!m_ArraysScope.IsEmpty(), "Array scoping error.");

				auto const& topDocArray = m_ArraysScope.GetLast();
				GD_DEBUG_VERIFY(topDocArray.CurrentIndex < topDocArray.Array.GetLength(), "Reading array element runs out of bounds.");
				m_SelectedValue = topDocArray.Array[topDocArray.CurrentIndex++];
				return true;
			}
			return false;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties readers.
		// ------------------------------------------------------------------------------------------

		GDAPI virtual bool TryReadPropertyValueNull() override
		{
			return m_SelectedValue->_GetTypeInfo() == DocValueTypeInfo::Null;
		}

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
			if (!ObjectReaderDocBase::TryBeginReadArrayPropertyValue(arraySize))
			{
				DocValueVector value;
				GD_DEBUG_VERIFY(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
				if (!m_SelectedValue->_TryGetValue(value))
				{
					ObjectReaderDocBase::EndReadArrayPropertyValue();
					return false;
				}
				arraySize = value.GetLength();
				m_ArraysScope.InsertLast({ value, 0 });
			}
			return true;
		}
			
		GDAPI virtual void EndReadArrayPropertyValue() override
		{
			ObjectReaderDocBase::EndReadArrayPropertyValue();
			m_ArraysScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL virtual bool TryBeginReadStructPropertyValue() override
		{
			if (!ObjectReaderDocBase::TryBeginReadStructPropertyValue())
			{
				DocObjectPtr value;
				GD_DEBUG_VERIFY(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
				if (!m_SelectedValue->_TryGetValue(value))
				{
					ObjectReaderDocBase::EndReadArrayPropertyValue();
					return false;
				}
                m_ObjectsScope.InsertLast({ value });
			}
			return true;
		}

		GDINL virtual void EndReadStructPropertyValue() override
		{
			ObjectReaderDocBase::EndReadStructPropertyValue();
			m_ObjectsScope.EraseLast();
		}

	};	// class ObjectReaderDoc

    // **------------------------------------------------------------------------------------------**
    //! Interface for the document-based serialization reader.
    // **------------------------------------------------------------------------------------------**
    class ObjectWriterDoc : public TObjectWriter<ObjectWriterDoc>
    {
    protected:
        
        /*!
         * Initializes the document-based writer interface.
         * @param docWritingStream Pointer to the stream to the one we are writing.
         */
        GDINL explicit ObjectWriterDoc(OutputStream& docWritingStream)
            : TObjectWriter<ObjectWriterDoc>(docWritingStream)
        {
        }
        
    };  // class ObjectWriterDoc

GD_NAMESPACE_END
