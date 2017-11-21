// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization.h
 * File contains serialization mechanisms.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/IO/Stream.h>

GD_NAMESPACE_BEGIN

	class ArchiveWriter;

	// **------------------------------------------------------------------------------------------**
	//! Declares a serializable object.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL class SerializableObject : public Object
	{
	private:
		GD_DECLARE_OBJECT_INTRINSIC(SerializableObject, Object)

		/*! 
		 * @brief Specifies, whether this object is dynamic.
		 * Dynamic objects are the ones that are saved during user-save operations.
		 */
		GD_OBJECT_KERNEL GD_PROPERTY(private, bool, m_IsDynamic) = false;

	public:

		// ------------------------------------------------------------------------------------------
		// Serialization subsystem.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Callback being triggered before this object is saved.
		 *
		 * @param isUserSave True if this save is not system.
		 * @returns True if this object need to be saved. This value is ignored for non-user saves.
		 */
		GDAPI GD_OBJECT_KERNEL virtual bool OnPreSerialize(bool const isUserSave) const
		{
			GD_NOT_USED(isUserSave);
			return true;
		}

		/*!
		 * Callback being triggered after this object is saved.
		 */
		GDAPI GD_OBJECT_KERNEL virtual void OnPostSerialize() const { }

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
		GDAPI GD_OBJECT_KERNEL virtual bool SerializeSync(SharedPtr<OutputStream> const outputStream) final;

		/*!
		 * @brief Asynchronously writes serializable properties of this object.
		 * @see SerializeSync enum.
		 */
	//	GDAPI GD_OBJECT_KERNEL virtual bool SerializeAsync(ArchiveWriter const* archive) final;

		// ------------------------------------------------------------------------------------------
		// Deserialization subsystem.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Callback being triggered before this object is loaded.
		 */
		GDAPI virtual void OnPreDeserialize() {}

		/*!
		 * Callback being triggered after this object is loaded.
		 */
		GDAPI virtual void OnPostDeserialize() {}

		GDAPI GD_OBJECT_KERNEL virtual bool DeserializeSync(SharedPtr<InputStream> const inputStream) final;

	};	// class SerializableObject

GD_NAMESPACE_END
