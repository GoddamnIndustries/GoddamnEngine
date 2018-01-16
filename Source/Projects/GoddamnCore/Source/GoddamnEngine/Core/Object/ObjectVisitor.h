// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Object.h
 * File contains base interface for object reflection.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Base.h>

#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Declares flags for object properties.
	// **------------------------------------------------------------------------------------------**
	//! @{
	GD_OBJECT_KERNEL enum PropertyFlags : UInt64
	{
		PFNone = 0,			//!< Zero property flags.
		PFNotSerializable,	//!< Property is not serializable.
		PFDefault,
	};	// enum PropertyFlags
	GD_ENUM_DEFINE_FLAG_OPERATORS(PropertyFlags);
	//! @}

	// **------------------------------------------------------------------------------------------**
	//! Defines reflection information for the property.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL struct PropertyMetaInfo final : public TNonCopyable
	{
		String const PropertyName;
		PropertyFlags const PropertyFlags;

	public:
		GDINL explicit PropertyMetaInfo(CStr propertyName, GD::PropertyFlags const propertyFlags = PFDefault, ...)
			: PropertyName(propertyName), PropertyFlags(propertyFlags)
		{
		}
	};	// struct PropertyMetaInfo

	// **------------------------------------------------------------------------------------------**
	//! Defines interface for reflecting objects.
	//!
	//! @warning Never implement this interface directly. Use ObjectVisitor class instead.
	//! @see ObjectVisitor class.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL struct IObjectVisitor : public IVirtuallyDestructible
	{
	protected:

		// ------------------------------------------------------------------------------------------
		// Primitive properties visitors.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Handles unknown property.
		 * Value of the property can be retrieved through va_start. 
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, ...) GD_PURE_VIRTUAL;

		/*!
		 * Handles boolean property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Boolean property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, bool& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles signed 8-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int8& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles unsigned 8-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt8& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles signed 16-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int16& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles unsigned 16-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt16& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles signed 32-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int32& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles unsigned 32-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt32& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles signed 64-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int64& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles unsigned 64-bit integer property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Integer property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt64& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles 32-bit floating-point property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Float property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Float32& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles 64-bit floating-point property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Float property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Float64& value) GD_PURE_VIRTUAL;

		/*!
		 * Handles string property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value String property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, String& value) GD_PURE_VIRTUAL;
		
		/*!
		 * Handles object property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value String property value.
		 */
		GDAPI virtual void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Object const*& value) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Array properties visitors.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Handles array property.
		 * This function would be called to initialize array handling. Visitor may modify size of the array.
		 * Array elements would be handled by 'VisitPrimitiveProperty' calls with array meta information.
		 * After visiting the whole array, 'EndVisitArrayProperty' is called.
		 *
		 * Support of the multi-dimensional arrays should be implemented.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param arraySize Size of the array. May be modified.
		 * @returns False is reflecting this array property should be skipped.
		 */
		GDAPI virtual bool BeginVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo, SizeTp& arraySize) GD_PURE_VIRTUAL;

		/*!
		 * @brief Handles array property.
		 * Notifies end of the array property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 */
		GDAPI virtual void EndVisitArrayProperty(PropertyMetaInfo const* const propertyMetaInfo) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Structure properties visitors.
		// ------------------------------------------------------------------------------------------

		/*!
		 * @brief Handles structure property.
		 * This function would be called to initialize structure handling. 
		 * Structure properties would be handled by 'VisitPrimitiveProperty' calls with array meta information.
		 * After visiting the whole structure, 'EndVisitStructProperty' is called.
		 *
		 * Support of the scoped structures should be implemented.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @returns False is reflecting this struct property should be skipped.
		 */
		GDAPI virtual bool BeginVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) GD_PURE_VIRTUAL;

		/*!
		 * @brief Handles structure property.
		 * Notifies end of the structure property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 */
		GDAPI virtual void EndVisitStructProperty(PropertyMetaInfo const* const propertyMetaInfo) GD_PURE_VIRTUAL;

	};	// struct IObjectVisitor

	// **------------------------------------------------------------------------------------------**
	//! Implements template wrappers from the 'IObjectVisitor' interface.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_HELPER struct ObjectVisitor : public IObjectVisitor
	{
	public:

		/*!
		 * Handles primitive property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Primitive property value.
		 */
		template<typename TProperty>
		GDINL GD_OBJECT_HELPER void VisitProperty(PropertyMetaInfo const* const propertyMetaInfo, TProperty& value)
		{
			this->VisitPrimitiveProperty(propertyMetaInfo, value);
		}

		/*!
		 * Handles array property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param array Array property value.
		 */
		template<typename TElement>
		GDINL GD_OBJECT_HELPER void VisitProperty(PropertyMetaInfo const* const propertyMetaInfo, Vector<TElement>& array)
		{
			auto arraySize = array.GetLength();
			if (this->BeginVisitArrayProperty(propertyMetaInfo, arraySize))
			{
				array.Resize(arraySize);
				for (auto& arrayElement : array)
				{
					this->VisitProperty(propertyMetaInfo, arrayElement);
				}
				this->EndVisitArrayProperty(propertyMetaInfo);
			}
		}

	};	// struct ObjectVisitor

	// **------------------------------------------------------------------------------------------**
	//! Implements template wrappers from the 'ObjectVisitor' primitive visiting interface.
	// **------------------------------------------------------------------------------------------**
	template<typename TImplementation>
	GD_OBJECT_HELPER struct TObjectVisitor : public ObjectVisitor
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Primitive properties visitors.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Handles template primitive property.
		 *
		 * @param propertyMetaInfo Meta information, declared with property.
		 * @param value Property value.
		 */
		template<typename TValue>
		GDINL void VisitPrimitivePropertyImpl(PropertyMetaInfo const* const propertyMetaInfo, TValue& value)
		{
			GD_NOT_USED(this);
			GD_NOT_USED(propertyMetaInfo);
			GD_NOT_USED(value);
			GD_NOT_IMPLEMENTED();
		}

	private:
		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, ...) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, bool& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int8& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt8& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int16& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt16& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int32& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt32& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Int64& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, UInt64& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Float32& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Float64& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, String& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}
		
		GDAPI void VisitPrimitiveProperty(PropertyMetaInfo const* const propertyMetaInfo, Object const*& value) override
		{
			static_cast<TImplementation*>(this)->VisitPrimitivePropertyImpl(propertyMetaInfo, value);
		}

	};	// struct TObjectVisitor

GD_NAMESPACE_END
