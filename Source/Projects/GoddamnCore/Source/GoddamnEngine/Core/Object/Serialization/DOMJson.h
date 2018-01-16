// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/DOMJson.h
 * JavaScript Object Notation data parsing and writing.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Object/Serialization/DOM.h>

GD_NAMESPACE_BEGIN

	class Json;
	class JsonValue;
	class JsonObject;

	using JsonTypeInfo = DomTypeInfo;

	using JsonPtr = SharedPtr<Json>;
	using JsonValuePtr = SharedPtr<JsonValue>;
	using JsonObjectPtr = SharedPtr<JsonObject>;
	using JsonValueVector = Vector<JsonValuePtr>;

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON abstract value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValue : public IDomValue
	{
	public:

		/*!
		 * Returns type of current property.
		 */
		GDINT virtual JsonTypeInfo GetTypeInfo() const
		{
			return JsonTypeInfo::Unknown;
		}
		GDINT SerializationTypeInfo _GetTypeInfo() const override final 
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
		GDAPI virtual bool TryGetValue(bool& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT bool _TryGetValue(bool& value) override final 
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
		GDAPI virtual bool TryGetValue(Int8& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(Int8& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 8-bit integer type.
		 *
		 * @param value 8-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(UInt8& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(UInt8& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 16-bit integer type.
		 *
		 * @param value 16-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(Int16& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(Int16& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 16-bit integer type.
		 *
		 * @param value 16-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(UInt16& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(UInt16& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 32-bit integer type.
		 *
		 * @param value 32-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(Int32& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(Int32& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 32-bit integer type.
		 *
		 * @param value 32-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(UInt32& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(UInt32& value) override final
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 64-bit integer type.
		 *
		 * @param value 64-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(Int64& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(Int64& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 64-bit integer type.
		 *
		 * @param value 64-bit integer output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(UInt64& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(UInt64& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 32-bit floating-point type.
		 *
		 * @param value 32-bit floating-point output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(Float32& value) final
		{
			return TryGetNumericValueImpl(value);
		}
		GDINT bool _TryGetValue(Float32& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to 64-bit floating-point type.
		 *
		 * @param value 64-bit floating-point output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(Float64& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT bool _TryGetValue(Float64& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to string type.
		 *
		 * @param value String output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(String& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT bool _TryGetValue(String& value) override final 
		{
			return TryGetValue(value);
		}

		/*!
		 * Tries to convert current JSON value to array type.
		 *
		 * @param value Array output value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TryGetValue(JsonValueVector& value)
		{
			GD_NOT_USED(value);
			return false;
		}
		GDINT bool _TryGetValue(DomValueVector& value) override final 
		{ 
			JsonValueVector valueJson;
			if (TryGetValue(valueJson))
			{
				value.Emptify();
				for (auto const valueElement : valueJson)
				{
					value.InsertLast(static_cast<DomValuePtr>(valueElement));
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
		GDAPI virtual bool TryGetValue(JsonObjectPtr& value);
		GDINT bool _TryGetValue(DomObjectPtr& value) override final;

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Tries to set to the current JSON value the value of the Boolean type.
		 *
		 * @param value Boolean input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TrySetValue(bool const value)
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
		GDAPI bool TrySetValue(Int8 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 8-bit integer type.
		 *
		 * @param value 8-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(UInt8 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 16-bit integer type.
		 *
		 * @param value 16-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(Int16 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 16-bit integer type.
		 *
		 * @param value 16-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(UInt16 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 32-bit integer type.
		 *
		 * @param value 32-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(Int32 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 32-bit integer type.
		 *
		 * @param value 32-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(UInt32 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 64-bit integer type.
		 *
		 * @param value 64-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(Int64 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 64-bit integer type.
		 *
		 * @param value 64-bit integer input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(UInt64 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 32-bit floating-point type.
		 *
		 * @param value 32-bit floating-point input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI bool TrySetValue(Float32 const value)
		{
			return TrySetNumericValueImpl(value);
		}

		/*!
		 * Tries to set to the current JSON value the value of the 64-bit floating-point type.
		 *
		 * @param value 64-bit floating-point input value.
		 * @returns True if operation succeeded.
		 */
		GDAPI virtual bool TrySetValue(Float64 const value)
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
		GDAPI virtual bool TrySetValue(String const& value)
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
		GDAPI virtual bool TrySetValue(JsonValueVector const& value)
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
		GDAPI virtual bool TrySetValue(JsonObjectPtr const value);

	};	// class JsonValue

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON object - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonObject final : public IDomObject
	{
	private:
		Map<String, JsonValuePtr> m_Properties;

	public:

		/*!
		 * Finds DOM value property by name.
		 *
		 * @param name The name of the property.
		 * @returns Pointer to the found value or null pointer.
		 */
		GDINT JsonValuePtr GetProperty(String const& name)
		{
			auto const propertyPtr = m_Properties.Find(name);
			return propertyPtr != nullptr ? *propertyPtr : nullptr;
		}
		GDINT DomValuePtr _GetProperty(String const& name) override final { return static_cast<DomValuePtr>(GetProperty(name)); }

		/*!
		 * Removes DOM property with specified name.
		 * If no such property was found, does nothing.
		 *
		 * @param name The name of the property.
		 */
		GDINT void RemoveProperty(String const& name)
		{
			if (m_Properties.Find(name) != nullptr)
			{
				m_Properties.Erase(name);
			}
		}

		/*!
		 * Adds a new DOM property with specified name.
		 *
		 * @param name The name of the property.
		 * @param value The value of the property.
		 */
		GDINT void AddProperty(String const& name, JsonValuePtr const value)
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
	GDINL bool JsonValue::_TryGetValue(DomObjectPtr& value)
	{ 
		JsonObjectPtr valueJson;
		if (TryGetValue(valueJson))
		{
			value = valueJson;
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
	//! Interface for JSON null value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValueNull final : public JsonValue
	{
	public:
		GDINL JsonValueNull() {}

	public:
		GDINT JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Null;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TryGetValue(bool& value) override final
		{
			value = false;
			return true;
		}

		GDAPI bool TryGetValue(Float64& value) override final
		{
			value = 0.0;
			return true;
		}

		GDAPI bool TryGetValue(String& value) override final
		{
			value = "null";
			return true;
		}

		GDAPI bool TryGetValue(JsonObjectPtr& value) override final
		{
			value = nullptr;
			return true;
		}

	};	// class JsonValueNull

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON Boolean value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValueBool final : public JsonValue
	{
	public:
		bool ValueBool = false;
		GDINL explicit JsonValueBool(bool const valueBool) : ValueBool(valueBool) {}

	public:
		GDINT JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Bool;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TryGetValue(bool& value) override final
		{
			value = ValueBool;
			return true;
		}

		GDAPI bool TryGetValue(Float64& value) override final
		{
			value = ValueBool ? 1.0 : 0.0;
			return true;
		}

		GDAPI bool TryGetValue(String& value) override final
		{
			value = ValueBool ? "true" : "false";
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TrySetValue(bool const value) override final
		{
			ValueBool = value;
			return true;
		}

		GDAPI bool TrySetValue(Float64 const value) override final
		{
			ValueBool = value != 0.0;
			return true;
		}

		GDAPI bool TrySetValue(String const& value) override final
		{
			auto result = false;
			ValueBool = value.ToBool(&result);
			return result;
		}

	};	// class JsonValueBool

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON numeric value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValueNumber final : public JsonValue
	{
	public:
		Float64 ValueNumber;
		GDINL explicit JsonValueNumber(Float64 const valueNumber) : ValueNumber(valueNumber) {}

	public:
		GDINT JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Float64;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TryGetValue(bool& value) override final
		{
			value = ValueNumber != 0.0;
			return true;
		}

		GDAPI bool TryGetValue(Float64& value) override final
		{
			value = ValueNumber;
			return true;
		}

		GDAPI bool TryGetValue(String& value) override final
		{
			value = String::FromFloat64(ValueNumber);
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TrySetValue(bool const value) override final
		{
			ValueNumber = value ? 1.0 : 0.0;
			return true;
		}

		GDAPI bool TrySetValue(Float64 const value) override final
		{
			ValueNumber = value;
			return true;
		}

		GDAPI bool TrySetValue(String const& value) override final
		{
			auto result = false;
			ValueNumber = value.ToFloat64(&result);
			return result;
		}

	};	// class JsonValueNumber

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON numeric value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValueString final : public JsonValue
	{
	public:
		String ValueString;
		GDINL explicit JsonValueString(String const& valueString) : ValueString(valueString) {}

	public:
		GDINT JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::String;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TryGetValue(bool& value) override final
		{
			auto result = false;
			value = ValueString.ToBool(&result);
			return result;
		}

		GDAPI bool TryGetValue(Float64& value) override final
		{
			auto result = false;
			value = ValueString.ToFloat64(&result);
			return result;
		}

		GDAPI bool TryGetValue(String& value) override final
		{
			value = ValueString;
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TrySetValue(bool const value) override final
		{
			ValueString = value ? "true" : "false";
			return true;
		}

		GDAPI bool TrySetValue(Float64 const value) override final
		{
			ValueString = String::FromFloat64(value);
			return true;
		}

		GDAPI bool TrySetValue(String const& value) override final
		{
			ValueString = value;
			return true;
		}

	};	// class JsonValueString

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON array value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValueArray final : public JsonValue
	{
	public:
		JsonValueVector ValueArray;
		GDINL explicit JsonValueArray(UInt32 const valueArrayCapacity = 0) : ValueArray(0, valueArrayCapacity) {}
		GDINL explicit JsonValueArray(JsonValueVector const& valueArray) : ValueArray(valueArray) {}

	public:
		GDINT JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Array;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TryGetValue(JsonValueVector& value) override final
		{
			value = ValueArray;
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TrySetValue(JsonValueVector const& value) override final
		{
			ValueArray = value;
			return true;
		}

	};	// class JsonValueNumber

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON object value - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class JsonValueObject final : public JsonValue
	{
	public:
		JsonObjectPtr ValueObject;
		GDINL explicit JsonValueObject(JsonObjectPtr const valueObject) : ValueObject(valueObject) {}

	public:
		GDINT JsonTypeInfo GetTypeInfo() const override final
		{
			return JsonTypeInfo::Object;
		}

		// ------------------------------------------------------------------------------------------
		// Getter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TryGetValue(JsonObjectPtr& value) override final
		{
			value = ValueObject;
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Setter operations.
		// ------------------------------------------------------------------------------------------

		GDAPI bool TrySetValue(JsonObjectPtr const value) override final
		{
			ValueObject = value;
			return true;
		}

	};	// class JsonValueObject

	// **------------------------------------------------------------------------------------------**
	//! Interface for JSON document - ORIGINALLY INTENDED FOR INTERNAL USAGE ONLY.
	// **------------------------------------------------------------------------------------------**
	class Json final : public IDom
	{
		/*!
		 * Tries to parse a DOM document.
		 *
		 * @param domInputStream Input stream that represents DOM data.
		 * @returns Struct that represents DOM parsing result.
		 */
		GDINT Result _TryParseDOM(SharedPtr<InputStream> const domInputStream) override final;

	};	// class Json

GD_NAMESPACE_END
