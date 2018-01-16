// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization.cpp
 * File contains serialization mechanisms.
 */
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>
#include <GoddamnEngine/Core/Object/Serialization/ReaderWriterJson.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>
#include <GoddamnEngine/Core/IO/Stream.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT_INTRINSIC(SerializableObject);

	// ------------------------------------------------------------------------------------------
	// Serialization subsystem.
	// ------------------------------------------------------------------------------------------

	// A bridge between worlds of ObjectVisitor and ISerializationWriter.
	template<typename TSerializationWriter>
	GD_OBJECT_HELPER struct ObjectSerializer final : public TObjectVisitor<ObjectSerializer<TSerializationWriter>>
	{
	private:
		TSerializationWriter& m_SerializationWriter;

	public:
		GDINL explicit ObjectSerializer(TSerializationWriter& serializationWriter)
			: m_SerializationWriter(serializationWriter)
		{
		}

	private:
		// Serializing generic properties..
		template<typename TValue>
		GDINL void VisitPrimitivePropertyImplBase(PropertyMetaInfo const* const propertyMetaInfo, TValue& value) 
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationWriter.WritePropertyName(propertyMetaInfo->PropertyName);
				m_SerializationWriter.WritePropertyValue(value);
			}
		}

	public:
		// Deserializing unknown properties..
		GDINL static void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, ...)
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				Debug::LogErrorFormat("Failed to serialize property '%s' with unknown type.", propertyMetaInfo->PropertyName);
			}
		}

		// Serializing primitive properties..
		template<typename TValue>
		GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, TValue& value) 
		{
			this->VisitPrimitivePropertyImplBase(propertyMetaInfo, value);
		}
		
		// Serializing primitive object properties..
		GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, Object const*& value) 
		{
			//this->VisitPrimitivePropertyImplBase(propertyMetaInfo, value->GetGUID());
			GD_NOT_IMPLEMENTED();
		}

		// ------------------------------------------------------------------------------------------
		// Array properties visitors.
		// ------------------------------------------------------------------------------------------

		GDINT bool BeginVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo, SizeTp& arraySize) override final
		{
			GD_NOT_USED(arraySize);
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationWriter.WritePropertyName(propertyMetaInfo->PropertyName);
				m_SerializationWriter.BeginWriteArrayPropertyValue();
			}
			return true;
		}

		GDINT void EndVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationWriter.EndWriteArrayPropertyValue();
			}
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties visitors.
		// ------------------------------------------------------------------------------------------

		GDINT bool BeginVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationWriter.WritePropertyName(propertyMetaInfo->PropertyName);
				m_SerializationWriter.BeginWriteStructPropertyValue();
			}
			return true;
		}

		GDINT void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationWriter.EndWriteStructPropertyValue();
			}
		}

	};	// struct ObjectSerializer

	/*!
	 * @brief Synchronously writes serializable properties of this object.
	 *
	 * Calls the 'OnPreSerialize' function, then serializes most properties of this object. 
	 * After all, if serialization succeeded, calls the 'OnPostSerialize' function.
	 *
	 * Properties that are declared with 'PFNotSerializable' flag are not affected by call of this
	 * function.
	 *
	 * @param outputStream The archive into which object would be serialized.
	 * @returns True if serialization succeeded.
	 * @see PropertyFlags enum.
	 */
	GDAPI GD_OBJECT_KERNEL bool SerializableObject::SerializeSync(SharedPtr<OutputStream> const outputStream)
	{
		JsonWriter serializationWriter(outputStream);
		ObjectSerializer<JsonWriter> serializer(serializationWriter);
		Reflect(serializer);
		return true;
	}

	// ------------------------------------------------------------------------------------------
	// Deserialization subsystem.
	// ------------------------------------------------------------------------------------------

	// A bridge between worlds of ObjectVisitor and IGenericReader.
	template<typename TSerializationReader>
	GD_OBJECT_HELPER struct ObjectDeserializer final : public TObjectVisitor<ObjectDeserializer<TSerializationReader>>
	{
	private:
		TSerializationReader& m_SerializationReader;

	public:
		GDINT explicit ObjectDeserializer(TSerializationReader& serializationReader)
			: m_SerializationReader(serializationReader)
		{
		}

	private:
		// Deserializing generic properties..
		template<typename TValue>
		GDINL void VisitPrimitivePropertyImplBase(PropertyMetaInfo const* const propertyMetaInfo, TValue& value)
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				if (!m_SerializationReader.TryReadPropertyName(propertyMetaInfo->PropertyName))
				{
					Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
				}
				else
				{
					if (!m_SerializationReader.TryReadPropertyValue(value))
					{
						Debug::LogWarningFormat("Unable to read value of the serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					}
				}
			}
		}

	public:
		// Deserializing unknown properties..
		GDINL static void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, ...)
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				Debug::LogErrorFormat("Failed to deserialize property '%s' with unknown type.", propertyMetaInfo->PropertyName);
			}
		}

		// Deserializing primitive properties..
		template<typename TValue>
		GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, TValue& value)
		{
			this->VisitPrimitivePropertyImplBase(propertyMetaInfo, value);
		}

		// Deserializing primitive object properties..
		GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, Object const*& value)
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				GD_NOT_IMPLEMENTED();
				/*GUID guid;
				VisitPrimitivePropertyImpl(propertyMetaInfo, guid);
				value = Object::FindObject(guid, nullptr);*/
			}
		}

		// ------------------------------------------------------------------------------------------
		// Array properties visitors.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool BeginVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo, SizeTp& arraySize) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				if (!m_SerializationReader.TryReadPropertyName(propertyMetaInfo->PropertyName))
				{
					Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					return false;
				}
				if (!m_SerializationReader.TryBeginReadArrayPropertyValue(arraySize))
				{
					Debug::LogWarningFormat("Unable to read value of the serialized data for array property '%s'", propertyMetaInfo->PropertyName.CStr());
					return false;
				}
			}
			return true;
		}

		GDINT virtual void EndVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationReader.EndReadArrayPropertyValue();
			}
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties visitors.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool BeginVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				if (!m_SerializationReader.TryReadPropertyName(propertyMetaInfo->PropertyName))
				{
					Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					return false;
				}
				if (!m_SerializationReader.TryBeginReadStructPropertyValue())
				{
					Debug::LogWarningFormat("Unable to read value of the serialized data for array property '%s'", propertyMetaInfo->PropertyName.CStr());
					return false;
				}
				return true;
			}
			return false;
		}

		GDINT virtual void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_SerializationReader.EndReadStructPropertyValue();
			}
		}

	};	// struct ObjectDeserializer

	GDAPI GD_OBJECT_KERNEL bool SerializableObject::DeserializeSync(SharedPtr<InputStream> const inputStream)
	{
		JsonReader serializationReader(inputStream);
		ObjectDeserializer<JsonReader> deserializer(serializationReader);
		Reflect(deserializer);
		return true;
	}

GD_NAMESPACE_END
