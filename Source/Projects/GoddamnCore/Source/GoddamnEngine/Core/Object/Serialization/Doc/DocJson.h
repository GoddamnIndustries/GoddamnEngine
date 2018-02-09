// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/DocJson.h
 * JavaScript Object Notation data parsing and writing.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Serialization/Doc/Doc.h>

#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>

GD_NAMESPACE_BEGIN

	class Json;
	class JsonValue;
	class JsonObject;

	using JsonTypeInfo = DocValueTypeInfo;
	using JsonPtr = SharedPtr<Json>;
	using JsonValuePtr = SharedPtr<JsonValue>;
	using JsonObjectPtr = SharedPtr<JsonObject>;
	using JsonValueVector = Vector<JsonValuePtr>;

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON abstract value.
	// **------------------------------------------------------------------------------------------**
	class JsonValue : public IDocValue
	{
	public:

		/*!
		 * Returns type of current property.
		 */
		GDINT virtual JsonTypeInfo GetTypeInfo() const
		{
			return JsonTypeInfo::Unknown;
		}
		GDINT virtual DocValueTypeInfo _GetTypeInfo() const override final
		{ 
			return GetTypeInfo(); 
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Tries to convert current JSON value to Boolean type.
		 *
		 * @param value Boolean output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(bool& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT virtual bool _TryGetValue(bool& value) override final
		{
			return TryGetValue(value);
		}

	private:
		template<typename TValue>
		GDINL bool TryGetNumericValueImpl(TValue& value)
		{
			// JSON supports only 'real' number types - 64-bit floats.
			auto number = static_cast<Float64>(value);
			if (TryGetValue(number))
			{
				value = static_cast<TValue>(number);
				return true;
			}
			return false;
		}

	public:

		/*!
		 * Tries to convert current JSON value to 8-bit integer type.
		 *
		 * @param value 8-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(Int8& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(Int8& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 8-bit integer type.
		 *
		 * @param value 8-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(UInt8& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(UInt8& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 16-bit integer type.
		 *
		 * @param value 16-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(Int16& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(Int16& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 16-bit integer type.
		 *
		 * @param value 16-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(UInt16& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(UInt16& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 32-bit integer type.
		 *
		 * @param value 32-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(Int32& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(Int32& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 32-bit integer type.
		 *
		 * @param value 32-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(UInt32& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(UInt32& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 64-bit integer type.
		 *
		 * @param value 64-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(Int64& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(Int64& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 64-bit integer type.
		 *
		 * @param value 64-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(UInt64& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(UInt64& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 32-bit floating-point type.
		 *
		 * @param value 32-bit floating-point output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(Float32& value)
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT virtual bool _TryGetValue(Float32& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 64-bit floating-point type.
		 *
		 * @param value 64-bit floating-point output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(Float64& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT virtual bool _TryGetValue(Float64& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to string type.
		 *
		 * @param value String output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(String& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT virtual bool _TryGetValue(String& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to array type.
		 *
		 * @param value Array output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(JsonValueVector& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT virtual bool _TryGetValue(DocValueVector& value) override final
		{ 
			JsonValueVector valueJson;
			if (TryGetValue(valueJson))
			{
				value.Emptify();
				for (auto const& valueElement : valueJson)
				{
					value.InsertLast(static_cast<DocValuePtr>(valueElement));
				}
				return true;
			}
			return false;
		}

		/*!
		 * Tries to convert current JSON value to object type.
		 *
		 * @param value Object output value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TryGetValue(JsonObjectPtr& value);
		GDINT virtual bool _TryGetValue(DocObjectPtr& value) override final;

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Tries to set to the current JSON value the value of the Boolean type.
		 *
		 * @param value Boolean input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(bool const value)
		{
			GD_NOT_USED(value);
			return false;
		}

	private:
		template<typename TValue>
		GDINL bool TrySetNumericValueImpl(TValue const value)
		{
			auto const number = static_cast<Float64>(value);
			return TrySetValue(number);
		}

	public:

		/*!
		 * Tries to set to the current JSON value the value of the 8-bit integer type.
		 *
		 * @param value 8-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(Int8 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 8-bit integer type.
		 *
		 * @param value 8-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(UInt8 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 16-bit integer type.
		 *
		 * @param value 16-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(Int16 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 16-bit integer type.
		 *
		 * @param value 16-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(UInt16 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 32-bit integer type.
		 *
		 * @param value 32-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(Int32 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 32-bit integer type.
		 *
		 * @param value 32-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(UInt32 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 64-bit integer type.
		 *
		 * @param value 64-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(Int64 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 64-bit integer type.
		 *
		 * @param value 64-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(UInt64 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 32-bit floating-point type.
		 *
		 * @param value 32-bit floating-point input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(Float32 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 64-bit floating-point type.
		 *
		 * @param value 64-bit floating-point input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(Float64 const value)
		{
			GD_NOT_USED(value);
			return false;
		}

		/*!
		 * Tries to set to the current JSON value the value of the string type.
		 *
		 * @param value String input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(String const& value)
		{
			GD_NOT_USED(value);
			return false;
		}

		/*!
		 * Tries to set to the current JSON value the value of the array type.
		 *
		 * @param value Array input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(JsonValueVector const& value)
		{
			GD_NOT_USED(value);
			return false;
		}

		/*!
		 * Tries to set to the current JSON value the value of the object type.
		 *
		 * @param value Object input value.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool TrySetValue(JsonObjectPtr const value);

	};	// class JsonValue

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON object.
	// **------------------------------------------------------------------------------------------**
	class JsonObject final : public IDocObject
	{
	private:
		Map<String, JsonValuePtr> m_Properties;

	public:

		/*!
		 * Finds Doc value property by name.
		 *
		 * @param name The name of the property.
		 * @returns Pointer to the found value or null pointer.
		 */
		GDINL JsonValuePtr GetProperty(String const& name)
		{
			auto const propertyPtr = m_Properties.Find(name);
			return propertyPtr != nullptr ? *propertyPtr : nullptr;
		}
		GDINT virtual DocValuePtr _GetProperty(String const& name) override final
        {
            return static_cast<DocValuePtr>(GetProperty(name));
        }

		/*!
		 * Removes Doc property with specified name.
		 * If no such property was found, does nothing.
		 *
		 * @param name The name of the property.
		 */
		GDINL void RemoveProperty(String const& name)
		{
			if (m_Properties.Find(name) != nullptr)
			{
				m_Properties.Erase(name);
			}
		}

		/*!
		 * Adds a new Doc property with specified name.
		 *
		 * @param name The name of the property.
		 * @param value The value of the property.
		 */
		GDINL void AddProperty(String const& name, JsonValuePtr const value)
		{
			m_Properties.Insert(name, value);
		}

	};	// class JsonObject

	// JsonValue->TryGet/SetValue - needs to be here because of checks inside SharedPtr.
	GDINL bool JsonValue::TryGetValue(JsonObjectPtr& value)
	{
		GD_NOT_USED(value);
		return false;
	}
	GDINL bool JsonValue::_TryGetValue(DocObjectPtr& value)
	{ 
		JsonObjectPtr valueJson;
		if (TryGetValue(valueJson))
		{
			value = static_cast<DocObjectPtr>(valueJson);
			return true;
		}
		return false;
	}
	GDINL bool JsonValue::TrySetValue(JsonObjectPtr const value)
	{
		GD_NOT_USED(value);
		return false;
	}

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON null value.
	// **------------------------------------------------------------------------------------------**
	class JsonValueNull final : public JsonValue
	{
	public:
		GDINL JsonValueNull() = default;

	public:
		GDINT virtual JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Null;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryGetValue(bool& value) override final
		{
			value = false;
			return true;
		}

		GDINT virtual bool TryGetValue(Float64& value) override final
		{
			value = 0.0;
			return true;
		}

		GDINT virtual bool TryGetValue(String& value) override final
		{
			value = "null";
			return true;
		}

		GDINT virtual bool TryGetValue(JsonObjectPtr& value) override final
		{
			value = nullptr;
			return true;
		}

	};	// class JsonValueNull

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON Boolean value.
	// **------------------------------------------------------------------------------------------**
	class JsonValueBool final : public JsonValue
	{
	public:
		bool ValueBool = false;
        
    public:
		GDINL explicit JsonValueBool(bool const valueBool)
            : ValueBool(valueBool)
        {
        }

	public:
		GDINT virtual JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Bool;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryGetValue(bool& value) override final
		{
			value = ValueBool;
			return true;
		}

		GDINT virtual bool TryGetValue(Float64& value) override final
		{
			value = ValueBool ? 1.0 : 0.0;
			return true;
		}

		GDINT virtual bool TryGetValue(String& value) override final
		{
            value = String::FromBool(ValueBool);
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TrySetValue(bool const value) override final
		{
			ValueBool = value;
			return true;
		}

		GDINT virtual bool TrySetValue(Float64 const value) override final
		{
			ValueBool = value != 0.0;
			return true;
		}

		GDINT virtual bool TrySetValue(String const& value) override final
		{
			auto result = false;
			ValueBool = value.ToBool(&result);
			return result;
		}

	};	// class JsonValueBool

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON numeric value.
	// **------------------------------------------------------------------------------------------**
	class JsonValueNumber final : public JsonValue
	{
	public:
		Float64 ValueNumber;
        
    public:
		GDINL explicit JsonValueNumber(Float64 const valueNumber)
            : ValueNumber(valueNumber)
        {
        }

	public:
		GDINT virtual JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Float64;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryGetValue(bool& value) override final
		{
			value = ValueNumber != 0.0;
			return true;
		}

		GDINT virtual bool TryGetValue(Float64& value) override final
		{
			value = ValueNumber;
			return true;
		}

		GDINT virtual bool TryGetValue(String& value) override final
		{
			value = String::FromFloat64(ValueNumber);
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TrySetValue(bool const value) override final
		{
			ValueNumber = value ? 1.0 : 0.0;
			return true;
		}

		GDINT virtual bool TrySetValue(Float64 const value) override final
		{
			ValueNumber = value;
			return true;
		}

		GDINT virtual bool TrySetValue(String const& value) override final
		{
			auto result = false;
			ValueNumber = value.ToFloat64(&result);
			return result;
		}

	};	// class JsonValueNumber

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON numeric value.
	// **------------------------------------------------------------------------------------------**
	class JsonValueString final : public JsonValue
	{
	public:
		String ValueString;

	public:
		GDINL explicit JsonValueString(String const& valueString)   // NOLINT
			: ValueString(valueString)
		{
		}

	public:
		GDINT virtual JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::String;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryGetValue(bool& value) override final
		{
			auto result = false;
			value = ValueString.ToBool(&result);
			return result;
		}

		GDINT virtual bool TryGetValue(Float64& value) override final
		{
			auto result = false;
			value = ValueString.ToFloat64(&result);
			return result;
		}

		GDINT virtual bool TryGetValue(String& value) override final
		{
			value = ValueString;
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TrySetValue(bool const value) override final
		{
            ValueString = String::FromBool(value);
			return true;
		}

		GDINT virtual bool TrySetValue(Float64 const value) override final
		{
			ValueString = String::FromFloat64(value);
			return true;
		}

		GDINT virtual bool TrySetValue(String const& value) override final
		{
			ValueString = value;
			return true;
		}

	};	// class JsonValueString

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON array value.
	// **------------------------------------------------------------------------------------------**
	class JsonValueArray final : public JsonValue
	{
	public:
		JsonValueVector ValueArray;
        
    public:
		GDINL explicit JsonValueArray(UInt32 const valueArrayCapacity = 0)
            : ValueArray(0, valueArrayCapacity)
        {
        }
		GDINL explicit JsonValueArray(JsonValueVector const& valueArray)  // NOLINT
            : ValueArray(valueArray)
        {
        }

	public:
		GDINT virtual JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Array;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryGetValue(JsonValueVector& value) override final
		{
			value = ValueArray;
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TrySetValue(JsonValueVector const& value) override final
		{
			ValueArray = value;
			return true;
		}

	};	// class JsonValueNumber

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON object value.
	// **------------------------------------------------------------------------------------------**
	class JsonValueObject final : public JsonValue
	{
	public:
		JsonObjectPtr ValueObject;
        
    public:
		GDINL explicit JsonValueObject(JsonObjectPtr const& valueObject)	// NOLINT
            : ValueObject(valueObject)
        {
        }

	public:
		GDINT virtual JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Object;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TryGetValue(JsonObjectPtr& value) override final
		{
			value = ValueObject;
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool TrySetValue(JsonObjectPtr const value) override final
		{
			ValueObject = value;
			return true;
		}

	};	// class JsonValueObject

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON document.
	// **------------------------------------------------------------------------------------------**
	class Json final : public IDoc
	{
    public:
		GDINT virtual Result _TryParseDocument(InputStream& docInputStream) override final;

	};	// class Json

GD_NAMESPACE_END
