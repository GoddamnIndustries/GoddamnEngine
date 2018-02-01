// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/ReaderWriterJSON.h
 * File contains interface to the JSON serialization readers and writers.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Serialization/Doc/DocJson.h>
#include <GoddamnEngine/Core/Object/Serialization/SerializationReaderWriterDoc.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! JSON serialization reader.
	// **------------------------------------------------------------------------------------------**
	class SerializationReaderJson final : public SerializationReaderDoc
	{
	public:
        
        /*!
         * Initializes the JSON-based reader interface.
         * @param jsonReadingStream Pointer to the stream, from the one we are reading.
         */
		GDINL explicit SerializationReaderJson(InputStream& jsonReadingStream)
			: SerializationReaderDoc(DocPtr(gd_new Json()), jsonReadingStream)
		{
		}
        
	};	// class SerializationReaderJson

	// **------------------------------------------------------------------------------------------**
	//! JSON serialization writer.
	// **------------------------------------------------------------------------------------------**
	class SerializationWriterJson final : public TSerializationWriter<SerializationWriterJson, SerializationWriterDoc>
	{
	private:
        using SerializationWriterJsonBase = TSerializationWriter<SerializationWriterJson, SerializationWriterDoc>;
		bool m_FirstPropertyWritten;

	public:
        
        /*!
         * Initializes the JSON-based writer interface.
         * @param jsonWritingStream Pointer to the stream to the one we are writing.
         */
		GDINL explicit SerializationWriterJson(OutputStream& jsonWritingStream)
			: SerializationWriterJsonBase(jsonWritingStream), m_FirstPropertyWritten(false)
		{
			BeginWriteStructPropertyValue();
		}

		GDAPI virtual ~SerializationWriterJson()
		{
			EndWriteStructPropertyValue();
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

	public:
		GDINL virtual bool WritePropertyName(String const& name) override final
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
			if (SerializationWriterJsonBase::WritePropertyName(name))
			{
				m_WritingStream.Write("\"");
				m_WritingStream.Write(name);
				m_WritingStream.Write("\" : ");
				return true;
			}
			return false;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writers.
		// ------------------------------------------------------------------------------------------
		
		GDAPI void WritePropertyValueImpl(bool const value)
		{
            m_WritingStream.Write(String::FromBool(value));
		}

		template<typename TValue>
		GDINL void WritePropertyValueImpl(TValue const value)
		{
			m_WritingStream.Write(String::FromFloat64(static_cast<Float64>(value)));
		}
        
		GDINL void WritePropertyValueImpl(String const& value)
		{
			m_WritingStream.Write("\"");
			m_WritingStream.Write(value);
			m_WritingStream.Write("\"");
		}
        
        GDINL void WritePropertyValueImpl(RefPtr<Object> const& value)
        {
            // Serializing as array containing GUID and class name.
            m_WritingStream.Write("[ \"");
            m_WritingStream.Write(value->GetGUID().ToString());
            m_WritingStream.Write("\", \"");
            m_WritingStream.Write(value->GetClass()->ClassName);
            m_WritingStream.Write("\" ]");
        }

		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL virtual void BeginWriteArrayPropertyValue() override final
		{
			SerializationWriterJsonBase::BeginWriteArrayPropertyValue();
			m_FirstPropertyWritten = false;
			m_WritingStream.Write("[");
			WriteNewline();
		}

		GDINL virtual void EndWriteArrayPropertyValue() override final
		{
			SerializationWriterJsonBase::EndWriteArrayPropertyValue();
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
			SerializationWriterJsonBase::BeginWriteStructPropertyValue();
			m_FirstPropertyWritten = false;
			m_WritingStream.Write("{");
			WriteNewline();
		}

		GDINL virtual void EndWriteStructPropertyValue() override final
		{
			SerializationWriterJsonBase::EndWriteStructPropertyValue();
			WriteNewline();
			WriteTabs();
			m_WritingStream.Write("}");
			m_FirstPropertyWritten = true;
		}

	};	// class JsonWriter

GD_NAMESPACE_END
