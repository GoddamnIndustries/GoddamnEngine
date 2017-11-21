// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
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
#include <GoddamnEngine/Core/Object/Serialization/ReaderWriterDOM.h>
#include <GoddamnEngine/Core/Object/Serialization/DOMJson.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Interface for the JSON serialization reader - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class JsonReader final : public DOMReader
	{
	public:
		GDINL explicit JsonReader(SharedPtr<InputStream> const readingStream)
			: DOMReader(DomPtr(gd_new Json()), readingStream)
		{
		}
	};	// class JsonReader

	// **------------------------------------------------------------------------------------------**
	//! Interface for the JSON serialization writer - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class JsonWriter final : public IGenericWriter
	{
	private:
		bool m_FirstPropertyWritten;

	public:
		GDINL explicit JsonWriter(SharedPtr<OutputStream> const writingStream)
			: IGenericWriter(writingStream), m_FirstPropertyWritten(false)
		{
			BeginWriteStructPropertyValue();
		}

		GDINL virtual ~JsonWriter()
		{
			EndWriteStructPropertyValue();
		}

	private:
		GDINL void WriteTabs() const
		{
			for (UInt32 cnt = 0; cnt < m_WritingScope.GetLength(); ++cnt)
			{
				m_WritingStream->Write('\t');
			}
		}

		GDINL void WriteNewline() const
		{
			m_WritingStream->Write('\n');
		}

	public:
		GDINL gd_s_virtual bool WritePropertyName(String const& name) gd_s_override
		{
			if (m_FirstPropertyWritten)
			{
				m_WritingStream->Write(",");
				WriteNewline();
			}
			else
			{
				m_FirstPropertyWritten = true;
			}
			WriteTabs();
			if (IGenericWriter::WritePropertyName(name))
			{
				m_WritingStream->Write("\"");
				m_WritingStream->Write(name);
				m_WritingStream->Write("\" : ");
				return true;
			}
			return false;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties writers.
		// ------------------------------------------------------------------------------------------
		
		GDINL gd_s_virtual void WritePropertyValue(bool const value) const gd_s_override
		{
			m_WritingStream->Write(value ? String("true") : String("false"));
		}

		template<typename TValue>
		GDINL gd_s_virtual void WritePropertyValue(TValue const value) const gd_s_override
		{
			m_WritingStream->Write(String::FromFloat64(static_cast<Float64>(value)));
		}

		GDINL gd_s_virtual void WritePropertyValue(String const value) const gd_s_override
		{
			m_WritingStream->Write("\"");
			m_WritingStream->Write(value);
			m_WritingStream->Write("\"");
		}

		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL gd_s_virtual void BeginWriteArrayPropertyValue() gd_s_override
		{
			IGenericWriter::BeginWriteArrayPropertyValue();
			m_FirstPropertyWritten = false;
			m_WritingStream->Write("[");
			WriteNewline();
		}

		GDINL gd_s_virtual void EndWriteArrayPropertyValue() gd_s_override
		{
			IGenericWriter::EndWriteArrayPropertyValue();
			WriteNewline();
			WriteTabs();
			m_WritingStream->Write("]");
			m_FirstPropertyWritten = true;
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL gd_s_virtual void BeginWriteStructPropertyValue() gd_s_override
		{
			IGenericWriter::BeginWriteStructPropertyValue();
			m_FirstPropertyWritten = false;
			m_WritingStream->Write("{");
			WriteNewline();
		}

		GDINL gd_s_virtual void EndWriteStructPropertyValue() gd_s_override
		{
			IGenericWriter::EndWriteStructPropertyValue();
			WriteNewline();
			WriteTabs();
			m_WritingStream->Write("}");
			m_FirstPropertyWritten = true;
		}

	};	// class JsonWriter

GD_NAMESPACE_END
