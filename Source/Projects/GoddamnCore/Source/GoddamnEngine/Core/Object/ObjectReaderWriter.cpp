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

	// **------------------------------------------------------------------------------------------**
	//! Object reference as it is represented in the serialized data.
	// **------------------------------------------------------------------------------------------**
    GD_OBJECT_KERNEL struct ObjectStringReference final : public Struct
    {
        GD_DECLARE_STRUCT(ObjectStringReference, Struct)
		GD_PROPERTY(public, String, __ObjectClass__);
		GD_PROPERTY(public, String, __ObjectGuid__);
        
    public:
		GDINL ObjectStringReference() = default;
        GDINL explicit ObjectStringReference(RefPtr<Object> const& object)
            : __ObjectClass__(object->GetClass()->ClassName), __ObjectGuid__(object->GetGUID().ToString())
        {
        }

		/*!
		 * Resolves the reference.
		 * 
		 * @param object Resolving output.
		 * @return True if reference was successfully resolved.
		 */
		GDINL bool TryResolve(RefPtr<Object>& object) const
        {
			GUID objectGuid;
			if (GUID::TryParse(__ObjectGuid__, objectGuid))
			{
				auto const objectClass = ObjectClass::FindClass(__ObjectClass__);
				if (objectClass != nullptr)
				{
					object = Object::CreateOrFindClassRelatedObjectByGUID(objectGuid, objectClass);
					return true;
				}
			}
			return false;
        }

    };  // struct ObjectReference

	// **------------------------------------------------------------------------------------------**
    //! Deserialization implementation based on reflection system.
    // **------------------------------------------------------------------------------------------**
    GD_OBJECT_KERNEL struct ObjectReaderVisitor final : public TObjectVisitor<ObjectReaderVisitor>
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
                ObjectStringReference valueReference;
                VisitProperty(propertyMetaInfo, valueReference);
                if (!valueReference.TryResolve(value))
                {
					Debug::LogWarningFormat("Unable to parse serialized data for property property '%s'", propertyMetaInfo->PropertyName.CStr());
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

                ObjectStringReference valueReference(value);
                VisitProperty(propertyMetaInfo, valueReference);
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
		r = TryReadPropertyNameOrSelectNextArrayElement();
		r = TryBeginReadStructPropertyValue();

		String objectClassString;
		TryReadPropertyName("GODDAMN_OBJECT_CLASS");
		TryReadPropertyValue(objectClassString);
		auto const objectClass = ObjectClass::FindClass(objectClassString);

		String objectGuidString;
		TryReadPropertyName("GODDAMN_OBJECT_GUID");
		TryReadPropertyValue(objectGuidString);
		GUID objectGuid;
		GUID::TryParse(objectGuidString, objectGuid);

		auto const object = Object::CreateOrFindClassRelatedObjectByGUID(objectGuid, objectClass);

		ObjectReaderVisitor objectReaderVisitor(*this);
		object->Reflect(objectReaderVisitor);

		EndReadStructPropertyValue();

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

		WritePropertyName("GODDAMN_OBJECT_CLASS");
		WritePropertyValue(object->GetClass()->ClassName);

		WritePropertyName("GODDAMN_OBJECT_GUID");
		WritePropertyValue(object->GetGUID().ToString());

		ObjectWriterVisitor objectWriterVisitor(*this);
		object->Reflect(objectWriterVisitor);

		EndWriteStructPropertyValue();

		for (auto const& childObject : objectWriterVisitor.GetChildObjects())
		{
			WriteObject(childObject);
		}
	}

GD_NAMESPACE_END
