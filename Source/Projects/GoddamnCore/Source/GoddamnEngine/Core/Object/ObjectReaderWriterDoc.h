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
			UInt32 mutable CurrentIndex;
		public:
			GDINL explicit DocValueArrayScope(UInt32 const currentIndex = 0) : CurrentIndex(currentIndex) {}
			GDINL explicit DocValueArrayScope(DocValueVector&& array, UInt32 const currentIndex = 0)
				: Array(Utils::Forward<DocValueVector>(array)), CurrentIndex(currentIndex)
			{
			}
		};	// struct DocValueArrayScope
        struct DocObjectPtrScope
        {
            DocObjectPtr Object;
		public:
			GDINL explicit DocObjectPtrScope() {}
			GDINL explicit DocObjectPtrScope(DocObjectPtr const object)
				: Object(object)
			{
			}
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
		GDINT explicit ObjectReaderDoc(DocPtr const& doc, InputStream& docReadingStream)
			: ObjectReaderDocBase(docReadingStream)
		{
			auto const result = doc->_TryParseDocument(docReadingStream);
			switch (result.RootValue->_GetTypeInfo())
			{
				case DocValueTypeInfo::Object:
					{
						DocObjectPtr docObject;
						result.RootValue->_TryGetValue(docObject);
						m_ObjectsScope.InsertLast(DocObjectPtrScope(docObject));
						m_ReadingScope.InsertLast(CurrentlyReading::Struct);
                    }
					break;
				case DocValueTypeInfo::Array:
					{
						DocValueVector docArray;
						result.RootValue->_TryGetValue(docArray);
						m_ArraysScope.InsertLast(DocValueArrayScope(Utils::Move(docArray)));
						m_ReadingScope.InsertLast(CurrentlyReading::Array);
                    }
					break;
				default:
					GD_NOT_SUPPORTED();
			}
		}
        
		GDINT virtual ~ObjectReaderDoc()
        {
            m_ReadingScope.EraseLast();
        }

		// ------------------------------------------------------------------------------------------
		// Properties reading.
		// ------------------------------------------------------------------------------------------

	public:
		GDINT virtual bool TryReadPropertyName(String const& name) override
		{
			if (!ObjectReaderDocBase::TryReadPropertyName(name))
			{
				// Selecting top object's property by name.
				GD_ASSERT(!m_ObjectsScope.IsEmpty(), "Object scoping error.");

				auto const topDocObject = m_ObjectsScope.GetLast().Object;
				m_SelectedValue = topDocObject->_GetProperty(name);
				return m_SelectedValue != nullptr;
			}
			return true;
		}

		GDINT virtual bool TrySelectNextArrayElement() override
		{
			if (ObjectReaderDocBase::TrySelectNextArrayElement())
			{
				// Selecting next element of the array.
				GD_ASSERT(!m_ArraysScope.IsEmpty(), "Array scoping error.");

				auto const& topDocArray = m_ArraysScope.GetLast();
				GD_ASSERT(topDocArray.CurrentIndex < topDocArray.Array.GetLength(), "Reading array element runs out of bounds.");
				m_SelectedValue = topDocArray.Array[topDocArray.CurrentIndex++];
				return true;
			}
			return false;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties readers.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryReadPropertyValueNull() override
		{
			return m_SelectedValue->_GetTypeInfo() == DocValueTypeInfo::Null;
		}

		template<typename TValue>
		GDINL bool TryReadPropertyValueImpl(TValue& value)
		{
			GD_ASSERT(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
			return m_SelectedValue->_TryGetValue(value);
		}

		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryBeginReadArrayPropertyValue(SizeTp& arraySize) override
		{
			if (!ObjectReaderDocBase::TryBeginReadArrayPropertyValue(arraySize))
			{
				DocValueVector value;
				GD_ASSERT(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
				if (!m_SelectedValue->_TryGetValue(value))
				{
					ObjectReaderDocBase::EndReadArrayPropertyValue();
					return false;
				}
				arraySize = value.GetLength();
				m_ArraysScope.InsertLast(DocValueArrayScope(Utils::Move(value)));
			}
			return true;
		}
			
		GDINT virtual void EndReadArrayPropertyValue() override
		{
			ObjectReaderDocBase::EndReadArrayPropertyValue();
			m_ArraysScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryBeginReadStructPropertyValue() override
		{
			if (!ObjectReaderDocBase::TryBeginReadStructPropertyValue())
			{
				DocObjectPtr value;
				GD_ASSERT(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
				if (!m_SelectedValue->_TryGetValue(value))
				{
					ObjectReaderDocBase::EndReadArrayPropertyValue();
					return false;
				}
                m_ObjectsScope.InsertLast(DocObjectPtrScope(value));
			}
			return true;
		}

		GDINT virtual void EndReadStructPropertyValue() override
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
