// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/JSON/Json.h
//! JavaScript Object Notation data parsing and writing.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	class JsonValue;
	class JsonObject;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// JSON Value.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	// Represents a type of the supported values of JSON objects.
	enum class JsonValueType : UInt8
	{
		Unknown,
		Null,
		Bool,
		Number,
		String,
		Object,
		Array,
	};	// enum class JsonValueType
	
	// ------------------------------------------------------------------------------------------
	// Represents a value of that can hold any of the supported JSON objects.
	class JsonValue : public IVirtuallyDestructible
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Returns the real type of this JSON value object.
		//! @returns Real type of this JSON value object.
		GDINL virtual JsonValueType GetType() const { return JsonValueType::Unknown; }

		// ------------------------------------------------------------------------------------------
		//! Tries to query the data, stored inside current JSON value object.
		//! @param value Reference to the output data, where the output would be written.
		//! @returns True if operation succeeded, false if conversion is impossible or the specified value is inconvertible.
		//! @{
		GDINL virtual bool TryGetNull  ()                                    const { return false; }
		GDINL virtual bool TryGetBool  (bool                        & value) const { GD_NOT_USED(value); return false; }
		GDINL virtual bool TryGetNumber(Float64                     & value) const { GD_NOT_USED(value); return false; }
		GDINL virtual bool TryGetString(String                      & value) const { GD_NOT_USED(value); return false; }
		GDINL virtual bool TryGetObject(SharedPtr<JsonObject>&        value) const { GD_NOT_USED(value); return false; }
		GDINL virtual bool TryGetArray (Vector<SharedPtr<JsonValue>>& value) const { GD_NOT_USED(value); return false; }
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Tries to write new data to the JSON value object.
		//! @param value Data, that would be set to the object.
		//! @returns True if operation succeeded, false if conversion is impossible or the specified value is inconvertible.
		//! @{
		GDINL virtual bool TrySetNull  ()                                          { return false; }
		GDINL virtual bool TrySetBool  (bool                         const  value) { GD_NOT_USED(value); return false; }
		GDINL virtual bool TrySetNumber(Float64                      const  value) { GD_NOT_USED(value); return false; }
		GDINL virtual bool TrySetString(String                       const& value) { GD_NOT_USED(value); return false; }
		GDINL virtual bool TrySetObject(SharedPtr<JsonObject>        const  value) { GD_NOT_USED(value); return false; }
		GDINL virtual bool TrySetArray (Vector<SharedPtr<JsonValue>> const& value) { GD_NOT_USED(value); return false; }
		//! @}

	};	// class JsonValue

	// ------------------------------------------------------------------------------------------
	//! Specialized version of the JSON value for NULL types.
	class JsonValueNull final : public JsonValue
	{
	public:
		GDINL explicit JsonValueNull() {}

	public:
#if !GD_DOCUMENTATION
		GDINL virtual JsonValueType GetType() const override final { return JsonValueType::Null; };
		GDINL virtual bool TryGetNull  ()                             const override final { return true; }
		GDINL virtual bool TryGetBool  (bool                 & value) const override final { value = false; return true; }
		GDINL virtual bool TryGetString(String               & value) const override final { value = "null"; return true; }
		GDINL virtual bool TryGetObject(SharedPtr<JsonObject>& value) const override final { value = nullptr; return true; }
		GDINL virtual bool TrySetNull  ()                                   override final { return true; }
#endif	// if !GD_DOCUMENTATION
	};	// class JsonValueNull

	// ------------------------------------------------------------------------------------------
	//! Specialized version of the JSON value for boolean types.
	class JsonValueBool final : public JsonValue
	{
	public:
		bool ValueBool = false;
		GDINL explicit JsonValueBool(bool const value) : ValueBool(value) {}

	public:
#if !GD_DOCUMENTATION
		GDINL virtual JsonValueType GetType() const override final { return JsonValueType::Bool; };
		GDINL virtual bool TryGetBool  (bool&         value) const override final { value = ValueBool; return true; }
		GDINL virtual bool TryGetNumber(Float64     & value) const override final { value = ValueBool ? 1.0 : 0.0; return true; }
		GDINL virtual bool TryGetString(String      & value) const override final { value = ValueBool ? "true" : "false"; return true; }
		GDINL virtual bool TrySetBool  (bool          value)       override final { ValueBool = value; return true; }
		GDINL virtual bool TrySetNumber(Float64       value)       override final { ValueBool = value != 0.0f; return true; }
		GDINL virtual bool TrySetString(String const& value)       override final { if (value == "true") { ValueBool = true; return true; } if (value == "false") { ValueBool = false; return true; } return false; }
		GDINL virtual bool TrySetObject(SharedPtr<JsonObject> const value) override final { ValueBool = value != nullptr; return true; }
#endif	// if !GD_DOCUMENTATION
	};	// class JsonValueBool

	// ------------------------------------------------------------------------------------------
	//! Specialized version of the JSON value for numeric types.
	class JsonValueNumber final : public JsonValue
	{
	public:
		Float64 ValueNumber = 0.0;
		GDINL explicit JsonValueNumber(Float64 const value) : ValueNumber(value) {}

	public:
#if !GD_DOCUMENTATION
		GDINL virtual JsonValueType GetType() const override final { return JsonValueType::Number; };
		GDINL virtual bool TryGetBool  (bool         & value) const override final { value = ValueNumber != 0.0; return true; }
		GDINL virtual bool TryGetNumber(Float64      & value) const override final { value = ValueNumber; return true; }
		GDINL virtual bool TryGetString(String       & value) const override final { value = String::FromFloat64(ValueNumber); return true; }
		GDINL virtual bool TrySetBool  (bool    const  value)       override final { ValueNumber = value ? 1.0 : 0.0; return true; }
		GDINL virtual bool TrySetNumber(Float64 const  value)       override final { ValueNumber = value; return true; }
		GDINL virtual bool TrySetString(String  const& value)       override final { GD_NOT_IMPLEMENTED(); }
#endif	// if !GD_DOCUMENTATION
	};	// class JsonValueNumber

	// ------------------------------------------------------------------------------------------
	//! Specialized version of the JSON value for string types.
	class JsonValueString final : public JsonValue
	{
	public:
		String ValueString;
		GDINL explicit JsonValueString(String const& value) : ValueString(value) {}

	public:
#if !GD_DOCUMENTATION
		GDINL virtual JsonValueType GetType() const override final { return JsonValueType::String; };
		GDINL virtual bool TryGetString(String       & value) const override final { value = ValueString; return true; }
		GDINL virtual bool TryGetNumber(Float64      & value) const override final { GD_NOT_IMPLEMENTED(); }
		GDINL virtual bool TrySetBool  (bool    const  value)       override final { ValueString = value ? "true" : "false"; return true; }
		GDINL virtual bool TrySetNumber(Float64 const  value)       override final { ValueString = String::FromFloat64(value); return true; }
		GDINL virtual bool TrySetString(String  const& value)       override final { ValueString = value; return true; }
#endif	// if !GD_DOCUMENTATION
	};	// class JsonValueString

	// ------------------------------------------------------------------------------------------
	//! Specialized version of the JSON value for object types.
	class JsonValueObject final : public JsonValue
	{
	public:
		SharedPtr<JsonObject> ValueObject;
		GDINL explicit JsonValueObject() : ValueObject() {}
		GDINL explicit JsonValueObject(SharedPtr<JsonObject> value) : ValueObject(value) {}
	
	public:
#if !GD_DOCUMENTATION
		GDINL virtual JsonValueType GetType() const override final { return JsonValueType::Object; };
		GDINL virtual bool TryGetObject(SharedPtr<JsonObject>&      value) const override final { value = ValueObject/*.GetPointer()*/; return true; }
		GDINL virtual bool TrySetObject(SharedPtr<JsonObject> const value)       override final { ValueObject = value; return true; }
		GDINL virtual bool TrySetNull  ()                                        override final { ValueObject/*.Reset(nullptr)*/ = nullptr; return true; }
#endif	// if !GD_DOCUMENTATION
	};	// class JsonValueObject

	// ------------------------------------------------------------------------------------------
	//! Specialized version of the JSON value for array types.
	class JsonValueArray final : public JsonValue
	{
	public:
		Vector<SharedPtr<JsonValue>> ValueArray;
		GDINL explicit JsonValueArray() {}
		GDINL explicit JsonValueArray(Vector<SharedPtr<JsonValue>> const& value) : ValueArray(value) {}

	public:
#if !GD_DOCUMENTATION
		GDINL virtual JsonValueType GetType() const override final { return JsonValueType::Array; };
		GDINL virtual bool TryGetArray(Vector<SharedPtr<JsonValue>>      & value) const override final { value = ValueArray; return true; }
		GDINL virtual bool TrySetArray(Vector<SharedPtr<JsonValue>> const& value)       override final { ValueArray = value; return true; }
#endif	// if !GD_DOCUMENTATION
	};	// class JsonValueArray

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// JSON Object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents the JSON object with fields.
	class JsonObject final : public IUncopiable
	{
	private:
		Map<String, SharedPtr<JsonValue>> m_Fields;
		Vector<String>                    m_FieldsOrder;

	public:

		GDINL void AddField(String const& fieldName, SharedPtr<JsonValue> const fieldValue)
		{
			m_Fields.Insert(fieldName, fieldValue);
			m_FieldsOrder.InsertLast(fieldName);
		}

		GDINL Map<String, SharedPtr<JsonValue>> const& GetFields() const
		{
			return m_Fields;
		}

		GDINL Vector<String> const& GetFieldsOrder() const
		{
			return m_FieldsOrder;
		}

		GDAPI SharedPtr<JsonValue> GetField(String const& name)
		{
			auto const iter = m_Fields.QueryIterator(name);
			return iter != m_Fields.End() ? iter->Value : nullptr;
		}

		/*
		GDINL void RemoveField(String const& name)
		{
			auto const iter = m_Fields.QueryIterator(name);
			if (iter != m_Fields.End())
			{
				m_FieldsOrder.EraseAt(m_FieldsOrder.FindFirst(name));
				m_Fields.Erase(name);
			}
		}
		*/
	};	// class JsonObject

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// JSON Parser.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents a high-level parsing result.
	template<typename Tp>
	struct JsonResult
	{
		SharedPtr<Tp> Result;
		String ErrorDesc;
	};	// struct JsonResult

	typedef JsonResult<JsonObject> JsonDocument;

	// ------------------------------------------------------------------------------------------
	//! Parser for the JSON data.
	class JsonSerializer final
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Parses JSON data from the input stream.
		//! @param stream Input stream with possible JSON data.
		//! @returns Pointer to the JSON m_RootNode object or error description.
		GDAPI static JsonDocument Parse(class InputStream* const stream);

		GDAPI static void Write(SharedPtr<JsonObject> const json, class OutputStream* const stream);

	};	// class JsonParser

GD_NAMESPACE_END
