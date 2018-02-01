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
#include <GoddamnEngine/Core/Object/Serialization/SerializationReaderWriterJson.h>

#include <GoddamnEngine/Core/IO/Stream.h>
#include <GoddamnEngine/Core/Misc/StringConv.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT_INTRINSIC(SerializableObject);

    /*!
     * Object reference as it is represented in the serialized data.
     */
    GD_OBJECT_KERNEL struct ObjectReference final : public Struct
    {
        GD_DECLARE_STRUCT(ObjectReference, Struct)
        GD_PROPERTY(public, String, ObjectGuid);
        GD_PROPERTY(public, String, ObjectClass);
        
    public:
        GDINL ObjectReference() {}
        GDINL ObjectReference(RefPtr<Object> const& object)
            : ObjectGuid(object->GetGUID().ToString()), ObjectClass(object->GetClass()->ClassName)
        {
        }
    };  // struct ObjectReference

    // **------------------------------------------------------------------------------------------**
    //! Serialization implementation based on reflection system.
    // **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL struct ObjectSerializationVisitor final : public TObjectVisitor<ObjectSerializationVisitor>
	{
	private:
		ISerializationWriter& m_SerializationWriter;

	public:
		GDINL explicit ObjectSerializationVisitor(ISerializationWriter& serializationWriter)
			: m_SerializationWriter(serializationWriter)
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
                m_SerializationWriter.WritePropertyName(propertyMetaInfo->PropertyName);
                m_SerializationWriter.WritePropertyValue(const_cast<TValue const&>(value));
            }
		}
        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, WideString& value)
        {
            // Serializing wide string properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                String valueUTF8(GD_ENCODE_UTF8(value));
                VisitProperty(propertyMetaInfo, valueUTF8);
            }
        }
        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, RefPtr<Object>& value)
        {
            // Serializing object properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                ObjectReference valueReference(value);
                VisitProperty(propertyMetaInfo, valueReference);
            }
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

	};	// struct ObjectSerializationVisitor

    // **------------------------------------------------------------------------------------------**
    //! Deserialization implementation based on reflection system.
    // **------------------------------------------------------------------------------------------**
    GD_OBJECT_KERNEL struct ObjectDeserializationVisitor final : public TObjectVisitor<ObjectDeserializationVisitor>
    {
    private:
        ISerializationReader& m_SerializationReader;
        
    public:
        GDINT explicit ObjectDeserializationVisitor(ISerializationReader& serializationReader)
        : m_SerializationReader(serializationReader)
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
        
        template<typename TValue>
        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, TValue& value)
        {
            // Deserializing primitive properties..
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
        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, WideString& value)
        {
            // Deserializing wide string properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                String valueUTF8;
                VisitProperty(propertyMetaInfo, valueUTF8);
                value = GD_DECODE_UTF8(valueUTF8);
            }
        }
        GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, RefPtr<Object>& value)
        {
            // Deserializing object properties..
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                ObjectReference valueReference;
                VisitProperty(propertyMetaInfo, valueReference);
                GD_NOT_IMPLEMENTED();
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
            }
            return true;
        }
        
        GDINT virtual void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) override
        {
            if ((propertyMetaInfo->PropertyFlags & PFNotSerializable) == 0)
            {
                m_SerializationReader.EndReadStructPropertyValue();
            }
        }
        
    };    // struct ObjectDeserializationVisitor

    // ------------------------------------------------------------------------------------------
    // Serialization subsystem.
    // ------------------------------------------------------------------------------------------

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
	GDAPI GD_OBJECT_KERNEL bool SerializableObject::SerializeSync(OutputStream& outputStream)
	{
		SerializationWriterJson serializationWriter(outputStream);
		ObjectSerializationVisitor serializationWriterVisitor(serializationWriter);
        
        OnPreSerialize(true);
		Reflect(serializationWriterVisitor);
        OnPostSerialize();
		return true;
	}

	// ------------------------------------------------------------------------------------------
	// Deserialization subsystem.
	// ------------------------------------------------------------------------------------------

	GDAPI GD_OBJECT_KERNEL bool SerializableObject::DeserializeSync(InputStream& inputStream)
	{
		SerializationReaderJson serializationReader(inputStream);
		ObjectDeserializationVisitor deserializer(serializationReader);
        
        OnPreDeserialize();
		Reflect(deserializer);
        OnPostDeserialize();
		return true;
	}

GD_NAMESPACE_END
