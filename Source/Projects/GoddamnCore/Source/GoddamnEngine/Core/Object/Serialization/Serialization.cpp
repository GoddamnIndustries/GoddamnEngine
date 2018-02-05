// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Serialization implementation.
 */
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>
#include <GoddamnEngine/Core/Object/ObjectReaderWriterJson.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT_INTRINSIC(SerializableObject);

    // ------------------------------------------------------------------------------------------
    // Serialization subsystem.
    // ------------------------------------------------------------------------------------------

	GDAPI GD_OBJECT_KERNEL bool SerializableObject::SerializeSync(ObjectWriter& serializationWriter)
	{
		return true;
	}

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
		return true;
	}

	// ------------------------------------------------------------------------------------------
	// Deserialization subsystem.
	// ------------------------------------------------------------------------------------------

	GDAPI GD_OBJECT_KERNEL bool SerializableObject::DeserializeSync(InputStream& inputStream)
	{
		return true;
	}

GD_NAMESPACE_END
