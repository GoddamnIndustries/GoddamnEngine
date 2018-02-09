// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Interface to the serialization readers and writers.
 */
#include <GoddamnEngine/Core/Object/ObjectReaderWriter.h>

#include <GoddamnEngine/Core/Misc/StringConv.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

    static String const g_ObjectClassPropertyName("GODDAMN_OBJECT_CLASS");
    static String const g_ObjectGuidPropertyName("GODDAMN_OBJECT_GUID");

	// **------------------------------------------------------------------------------------------**
    //! Deserialization implementation based on reflection system.
    // **------------------------------------------------------------------------------------------**
    struct GD_OBJECT_KERNEL ObjectReaderVisitor final : public TObjectVisitor<ObjectReaderVisitor>
    {
    private:
        ObjectReader& m_ObjectReader;
        
    public:
		GDINL explicit ObjectReaderVisitor(ObjectReader& objectReader)
			: m_ObjectReader(objectReader)
        {
        }
        
        // ------------------------------------------------------------------------------------------
        // Unknown properties visitors.
        // ------------------------------------------------------------------------------------------
        
        GDINT virtual void VisitUnknownProperty(PropertyMetaInfo const* const propertyMetaInfo, Handle const valueHandle) override final
        {
            // Deserializing unknown properties..
            GD_NOT_USED_L(this, valueHandle);
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                Debug::LogErrorFormat("Failed to serialize property '%s' with unknown type.", propertyMetaInfo->PropertyName.CStr());
            }
        }
        
        // ------------------------------------------------------------------------------------------
        // Primitives properties visitors.
        // ------------------------------------------------------------------------------------------
        
	public:
        template<typename TValue>
        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, TValue& value)
        {
            // Deserializing primitive properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                if (!m_ObjectReader.TryReadPropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName))
                {
                    Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					return;
                }
                if (!m_ObjectReader.TryReadPropertyValue(value))
                {
                    Debug::LogWarningFormat("Unable to read value of the serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
                }
            }
        }

        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, WideString& value)
        {
            // Deserializing wide string properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
				if (!m_ObjectReader.TryReadPropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName))
				{
					Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					return;
				}
				String valueUTF8;
				if (!m_ObjectReader.TryReadPropertyValue(valueUTF8))
				{
					Debug::LogWarningFormat("Unable to read value of the serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					return;
				}
                value = StringConv::DecodeUTF8(valueUTF8);
            }
        }

        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, RefPtr<Object>& value)
        {
            // Deserializing object properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
				if (!m_ObjectReader.TryReadPropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName))
				{
					Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
					return;
				}
				if (!m_ObjectReader.TryReadPropertyValueNull())
				{
					if (!m_ObjectReader.TryBeginReadObjectPropertyValue(value))
					{
						Debug::LogWarningFormat("Unable to read value of the serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
						return;
					}
					if ((propertyMetaInfo->PropertyFlags & PFChildren) != 0)
					{
						value->Reflect(*this);
					}
					m_ObjectReader.EndReadObjectPropertyValue();
				}
            }
        }
        
        // ------------------------------------------------------------------------------------------
        // Array properties visitors.
        // ------------------------------------------------------------------------------------------
        
        GDINT virtual bool BeginVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo, SizeTp& arraySize) override final
        {
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                if (!m_ObjectReader.TryReadPropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName))
                {
                    Debug::LogWarningFormat("Unable to find serialized data for array property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return false;
                }
                if (!m_ObjectReader.TryBeginReadArrayPropertyValue(arraySize))
                {
                    Debug::LogWarningFormat("Unable to read value of the serialized data for array property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return false;
                }
				return true;
			}
			return false;
		}
        
        GDINT virtual void EndVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
        {
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                m_ObjectReader.EndReadArrayPropertyValue();
            }
        }
        
        // ------------------------------------------------------------------------------------------
        // Structure properties visitors.
        // ------------------------------------------------------------------------------------------
        
        GDINT virtual bool BeginVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
        {
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                if (!m_ObjectReader.TryReadPropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName))
                {
                    Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return false;
                }
                if (!m_ObjectReader.TryBeginReadStructPropertyValue())
                {
                    Debug::LogWarningFormat("Unable to read value of the serialized data for struct property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return false;
                }
				return true;
			}
            return false;
        }
        
        GDINT virtual void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
        {
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                m_ObjectReader.EndReadStructPropertyValue();
            }
        }
    };    // struct ObjectReaderVisitor

    // **------------------------------------------------------------------------------------------**
    //! Serialization implementation based on reflection system.
    // **------------------------------------------------------------------------------------------**
	struct GD_OBJECT_KERNEL ObjectWriterVisitor final : public TObjectVisitor<ObjectWriterVisitor>
	{
	private:
		ObjectWriter& m_ObjectWriter;

	public:
		GDINL explicit ObjectWriterVisitor(ObjectWriter& objectWriter)
			: m_ObjectWriter(objectWriter)
		{
		}

        // ------------------------------------------------------------------------------------------
        // Unknown properties visitors.
        // ------------------------------------------------------------------------------------------
        
        GDINL virtual void VisitUnknownProperty(PropertyMetaInfo const* const propertyMetaInfo, Handle const valueHandle) override final
        {
            // Serializing unknown properties..
            GD_NOT_USED_L(this, valueHandle);
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                Debug::LogErrorFormat("Failed to serialize property '%s' with unknown type.", propertyMetaInfo->PropertyName.CStr());
            }
        }
        
        // ------------------------------------------------------------------------------------------
        // Primitives properties visitors.
        // ------------------------------------------------------------------------------------------
        
		template<typename TValue>
		GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, TValue& value) 
		{
            // Serializing primitive generic properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                m_ObjectWriter.WritePropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName);
                m_ObjectWriter.WritePropertyValue(const_cast<TValue const&>(value));
            }
		}

        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, WideString& value)
        {
            // Serializing wide string properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
				m_ObjectWriter.WritePropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName);
				m_ObjectWriter.WritePropertyValue(StringConv::EncodeUTF8(value));
            }
        }

        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, RefPtr<Object>& value)
        {
            // Serializing object properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
				m_ObjectWriter.WritePropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName);
				if (value != nullptr)
				{
					m_ObjectWriter.BeginWriteObjectPropertyValue(const_cast<RefPtr<Object> const&>(value));
					if ((propertyMetaInfo->PropertyFlags & PFChildren) != 0)
					{
						value->Reflect(*this);
					}
					m_ObjectWriter.EndWriteObjectPropertyValue();
				}
				else
				{
					m_ObjectWriter.WritePropertyValueNull();
				}
            }
        }
        
		// ------------------------------------------------------------------------------------------
		// Array properties visitors.
		// ------------------------------------------------------------------------------------------

		GDINT bool BeginVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo, SizeTp& arraySize) override final
		{
			GD_NOT_USED_L(arraySize);
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_ObjectWriter.WritePropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName);
				m_ObjectWriter.BeginWriteArrayPropertyValue();
			}
			return true;
		}

		GDINT void EndVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_ObjectWriter.EndWriteArrayPropertyValue();
			}
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties visitors.
		// ------------------------------------------------------------------------------------------

		GDINT bool BeginVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_ObjectWriter.WritePropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName);
				m_ObjectWriter.BeginWriteStructPropertyValue();
			}
			return true;
		}

		GDINT void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override final
		{
			if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
			{
				m_ObjectWriter.EndWriteStructPropertyValue();
			}
		}
	};	// struct ObjectWriterVisitor

	// ------------------------------------------------------------------------------------------
	// Object properties reading.
	// ------------------------------------------------------------------------------------------

	/*!
	 * @brief Reads structure property.
	 * This function should be called to initialize object reading. 
	 * After reading the whole structure, 'EndReadObjectPropertyValue' is called.
	 *
	 * @param value Object property value.
	 * @returns True if reading struct property was successfully initialized.
	 */
	GDINT bool ObjectReader::TryBeginReadObjectPropertyValue(RefPtr<Object>& value)
	{
		GUID valueGuid;
		String valueClassName;
		if (!TryBeginReadStructPropertyValue())
		{
			return false;
		}
		if (!(TryReadPropertyName(g_ObjectClassPropertyName) && TryReadPropertyValue(valueClassName)
			&& TryReadPropertyName(g_ObjectGuidPropertyName) && TryReadPropertyValue(valueGuid)))
		{
			return false;
		}

		// Creating the object..
		auto const valueClass = ObjectClass::FindClass(valueClassName);
		if (valueClass == nullptr)
		{
			return false;
		}
		value = Object::CreateOrFindClassRelatedObjectByGUID(valueGuid, valueClass);
		return true;
	}

	/*!
	 * Notifies end of the object property.
	 */
	GDINT void ObjectReader::EndReadObjectPropertyValue()
	{
		EndReadStructPropertyValue();
	}

	/*!
	 * Reads object.
	 * @returns Pointer to the deserialized object or nullptr on failure.
	 */
	GDAPI RefPtr<Object> ObjectReader::ReadObject()
	{
		if (!TrySelectNextArrayElement())
		{
			Debug::LogWarningFormat("Unable to find serialized data for object");
			return nullptr;
		}
		RefPtr<Object> object;
		if (!TryBeginReadObjectPropertyValue(object))
		{
			Debug::LogWarningFormat("Unable to read value of the serialized data for object");
			return nullptr;
		}
		{
			ObjectReaderVisitor objectReaderVisitor(*this);
			object->Reflect(objectReaderVisitor);
		}
		EndReadObjectPropertyValue();
		return object;
	}

	// ------------------------------------------------------------------------------------------
	// Object properties writing.
	// ------------------------------------------------------------------------------------------

	/*!
	 * @brief Writes object property.
	 * This function should be called to initialize object writing. 
	 * After writing the object structure, 'EndWriteObjectPropertyValue' should be called.
	 */
	GDINT void ObjectWriter::BeginWriteObjectPropertyValue(RefPtr<Object> const& value)
	{
		BeginWriteStructPropertyValue();
		WritePropertyName(g_ObjectClassPropertyName);
		WritePropertyValue(value->GetClass()->ClassName);
		WritePropertyName(g_ObjectGuidPropertyName);
		WritePropertyValue(value->GetGUID());
	}

	/*!
	 * Notifies end of the object property.
	 */
	GDINT void ObjectWriter::EndWriteObjectPropertyValue()
	{
		EndWriteStructPropertyValue();
	}

	/*!
	 * Writes object.
	 * @param object Object to write.
	 */
	GDAPI void ObjectWriter::WriteObject(RefPtr<Object> const& object)
	{
		if (!TrySelectNextArrayElement())
		{
			Debug::LogWarningFormat("Invalid writer format");
			return;
		}
		BeginWriteObjectPropertyValue(object);
		{
			ObjectWriterVisitor objectWriterVisitor(*this);
			object->Reflect(objectWriterVisitor);
		}
		EndWriteObjectPropertyValue();
	}

GD_NAMESPACE_END
