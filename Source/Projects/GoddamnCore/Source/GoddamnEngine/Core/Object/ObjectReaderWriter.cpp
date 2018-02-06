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
    GD_OBJECT_KERNEL struct ObjectReaderVisitor final : public TObjectVisitor<ObjectReaderVisitor>
    {
    private:
        ObjectReader& m_ObjectReader;
        SizeTp m_ChildObjectsCount;
        
    public:
		GDINL explicit ObjectReaderVisitor(ObjectReader& objectReader)
			: m_ObjectReader(objectReader), m_ChildObjectsCount(0)
        {
        }
        
        GDINL SizeTp GetChildObjectsCount() const
        {
            return m_ChildObjectsCount;
        }
        
        // ------------------------------------------------------------------------------------------
        // Unknown properties visitors.
        // ------------------------------------------------------------------------------------------
        
        GDINL virtual void VisitUnknownProperty(PropertyMetaInfo const* const propertyMetaInfo, Handle const valueHandle) override final
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
                if ((propertyMetaInfo->PropertyFlags & PFChildren) != 0)
                {
                    m_ChildObjectsCount += 1;
                }
                
                // Reading object class and GUID..
                if (!m_ObjectReader.TryReadPropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName))
                {
                    Debug::LogWarningFormat("Unable to find serialized data for property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return;
                }
                if (!m_ObjectReader.TryBeginReadStructPropertyValue())
                {
                    Debug::LogWarningFormat("Unable to read value of the serialized data for object property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return;
                }
                GUID valueGuid;
                String valueClassString;
                if (!m_ObjectReader.TryReadPropertyName(g_ObjectClassPropertyName) || !m_ObjectReader.TryReadPropertyValue(valueClassString) ||
                    !m_ObjectReader.TryReadPropertyName(g_ObjectGuidPropertyName) || !m_ObjectReader.TryReadPropertyValue(valueGuid))
                {
                    Debug::LogWarningFormat("Unable to read builtin values of the serialized data for object property '%s'", propertyMetaInfo->PropertyName.CStr());
                    m_ObjectReader.EndReadStructPropertyValue();
                    return;
                }
                m_ObjectReader.EndReadStructPropertyValue();
                
                // Creating the object..
                auto const valueClass = ObjectClass::FindClass(valueClassString);
                if (valueClass == nullptr)
                {
                    Debug::LogWarningFormat("Unable to parse find class of the serialized data for object property '%s'", propertyMetaInfo->PropertyName.CStr());
                    return;
                }
                value = Object::CreateOrFindClassRelatedObjectByGUID(valueGuid, valueClass);
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
        
        GDINT virtual void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override
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
	GD_OBJECT_KERNEL struct ObjectWriterVisitor final : public TObjectVisitor<ObjectWriterVisitor>
	{
	private:
		ObjectWriter& m_ObjectWriter;
		Vector<RefPtr<Object>> m_ChildObjects;

	public:
		GDINL explicit ObjectWriterVisitor(ObjectWriter& objectWriter)
			: m_ObjectWriter(objectWriter)
		{
		}

		GDINL virtual ~ObjectWriterVisitor()
		{
		}

		GDINL Vector<RefPtr<Object>> const& GetChildObjects() const
		{
			return m_ChildObjects;
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
				if ((propertyMetaInfo->PropertyFlags & PFChildren) != 0)
				{
					m_ChildObjects.InsertLast(value);
				}
                m_ObjectWriter.WritePropertyNameOrSelectNextArrayElement(&propertyMetaInfo->PropertyName);
                m_ObjectWriter.BeginWriteStructPropertyValue();
                m_ObjectWriter.WritePropertyName(g_ObjectClassPropertyName);
                m_ObjectWriter.WritePropertyValue(value->GetClass()->ClassName);
                m_ObjectWriter.WritePropertyName(g_ObjectGuidPropertyName);
                m_ObjectWriter.WritePropertyValue(value->GetGUID());
                m_ObjectWriter.EndWriteStructPropertyValue();
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

	};	// struct ObjectSerializationVisitor

	// ------------------------------------------------------------------------------------------
	// ObjectReader class.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Reads object.
	 * @returns Pointer to the deserialized object or nullptr on failure.
	 */
	GDAPI RefPtr<Object> ObjectReader::ReadObject()
	{
		bool r;
		r = TrySelectNextArrayElement();
		r = TryBeginReadStructPropertyValue();

		String objectClassString;
		TryReadPropertyName("GODDAMN_OBJECT_CLASS");
		TryReadPropertyValue(objectClassString);
		auto const objectClass = ObjectClass::FindClass(objectClassString);

        GUID objectGuid;
		TryReadPropertyName("GODDAMN_OBJECT_GUID");
		TryReadPropertyValue(objectGuid);

		auto const object = Object::CreateOrFindClassRelatedObjectByGUID(objectGuid, objectClass);

		ObjectReaderVisitor objectReaderVisitor(*this);
		object->Reflect(objectReaderVisitor);

		EndReadStructPropertyValue();

        for (SizeTp cnt = 0; cnt < objectReaderVisitor.GetChildObjectsCount(); ++cnt)
        {
            static_cast<void>(ReadObject());
        }
		return object;
	}

	/*!
	 * Writes object.
	 * @param object Object to write.
	 */
	GDAPI void ObjectWriter::WriteObject(RefPtr<Object> const& object)
	{
		TrySelectNextArrayElement();
		BeginWriteStructPropertyValue();
		WritePropertyName(g_ObjectClassPropertyName);
		WritePropertyValue(object->GetClass()->ClassName);
		WritePropertyName(g_ObjectGuidPropertyName);
		WritePropertyValue(object->GetGUID());

		ObjectWriterVisitor objectWriterVisitor(*this);
		object->Reflect(objectWriterVisitor);

		EndWriteStructPropertyValue();

		for (auto const& childObject : objectWriterVisitor.GetChildObjects())
		{
			WriteObject(childObject);
		}
	}

GD_NAMESPACE_END
