// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * File contains interface to the JSON serialization readers and writers.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/ObjectReaderWriterDoc.h>
#include <GoddamnEngine/Core/Object/Serialization/Doc/DocJson.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! JSON serialization reader.
	// **------------------------------------------------------------------------------------------**
	class ObjectReaderJson final : public ObjectReaderDoc
	{
	public:
        
        /*!
         * Initializes the JSON-based reader interface.
         * @param jsonReadingStream Pointer to the stream, from the one we are reading.
         */
		GDINL explicit ObjectReaderJson(InputStream& jsonReadingStream)
			: ObjectReaderDoc(DocPtr(gd_new Json()), jsonReadingStream)
		{
		}
        
	};	// class ObjectReaderJson

	// **------------------------------------------------------------------------------------------**
	//! JSON serialization writer.
	// **------------------------------------------------------------------------------------------**
	class ObjectWriterJson final : public TObjectWriter<ObjectWriterJson, ObjectWriterDoc>
	{
        using ObjectWriterJsonBase = TObjectWriter<ObjectWriterJson, ObjectWriterDoc>;
        
	private:
		bool m_FirstPropertyWritten;

	public:
        
        /*!
         * Initializes the JSON-based writer interface.
         * @param jsonWritingStream Pointer to the stream to the one we are writing.
         */
		GDINL explicit ObjectWriterJson(OutputStream& jsonWritingStream)
			: ObjectWriterJsonBase(jsonWritingStream), m_FirstPropertyWritten(false)
		{
			BeginWriteArrayPropertyValue();
		}

		GDAPI virtual ~ObjectWriterJson()
		{
			EndWriteArrayPropertyValue();
		}

	private:
		GDINL void WriteTabs() const
		{
			for (UInt32 cnt = 0; cnt < m_WritingScope.GetLength(); ++cnt)
			{
				m_WritingStream.Write('\t');
			}
		}

		GDINL void WriteNewline() const
		{
			m_WritingStream.Write('\n');
		}

		// ------------------------------------------------------------------------------------------
		// Properties writing.
		// ------------------------------------------------------------------------------------------

	public:
		GDAPI virtual bool TrySelectNextArrayElement() override final
		{
			if (m_FirstPropertyWritten)
			{
				m_WritingStream.Write(",");
				WriteNewline();
			}
			else
			{
				m_FirstPropertyWritten = true;
			}
			WriteTabs();
			return ObjectWriterJsonBase::TrySelectNextArrayElement();
		}

		GDAPI virtual void WritePropertyName(String const& name) override final
		{
			m_WritingStream.Write("\"");
			m_WritingStream.Write(name);
			m_WritingStream.Write("\" : ");
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writers.
		// ------------------------------------------------------------------------------------------
		
		GDAPI void WritePropertyValueImpl(bool const value)
		{
            m_WritingStream.Write(String::FromBool(value));
		}

		template<typename TNumericValue>
		GDINL void WritePropertyValueImpl(TNumericValue const value)
		{
			m_WritingStream.Write(String::FromFloat64(static_cast<Float64>(value)));
		}
        
		GDINL void WritePropertyValueImpl(String const& value)
		{
			m_WritingStream.Write("\"");
			m_WritingStream.Write(value);
			m_WritingStream.Write("\"");
		}
        
		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL virtual void BeginWriteArrayPropertyValue() override final
		{
			ObjectWriterJsonBase::BeginWriteArrayPropertyValue();
			m_FirstPropertyWritten = false;
			m_WritingStream.Write("[");
			WriteNewline();
		}

		GDINL virtual void EndWriteArrayPropertyValue() override final
		{
			ObjectWriterJsonBase::EndWriteArrayPropertyValue();
			WriteNewline();
			WriteTabs();
			m_WritingStream.Write("]");
			m_FirstPropertyWritten = true;
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL virtual void BeginWriteStructPropertyValue() override final
		{
			ObjectWriterJsonBase::BeginWriteStructPropertyValue();
			m_FirstPropertyWritten = false;
			m_WritingStream.Write("{");
			WriteNewline();
		}

		GDINL virtual void EndWriteStructPropertyValue() override final
		{
			ObjectWriterJsonBase::EndWriteStructPropertyValue();
			WriteNewline();
			WriteTabs();
			m_WritingStream.Write("}");
			m_FirstPropertyWritten = true;
		}

	};	// class ObjectReaderJson

GD_NAMESPACE_END
