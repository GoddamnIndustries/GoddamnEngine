// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

// Required to be included first.

/*! 
 * @file GoddamnEngine/Core/Object/Object.cpp
 * File contains base class for all engine entities.
 */
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Class.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	// Constructor and destructor.
	// ------------------------------------------------------------------------------------------

	/*!
	 * @brief Initializes object.
	 * Initializes the object and registers it inside the system to make garbage collecting
	 * and recycling work.
	 *
	 * @param objectCtorInfo Construction information for this object.
	 */
	GDAPI GD_OBJECT_KERNEL Object::Object(ObjectCtorInfo const& objectCtorInfo)
		: m_GUID(objectCtorInfo.Guid), m_ReferenceCount(1)
	{
		GD_DEBUG_VERIFY(objectCtorInfo.Guid != EmptyGUID, "Empty GUID was specified.");
		GD_DEBUG_VERIFY(objectCtorInfo.Class != nullptr, "Null pointer class was specified.");
		{	
			// Registering the object in the class table.
			ScopedLiteMutex classInstancesRegistryLock(objectCtorInfo.Class->m_ClassInstancesRegistryLock);
			objectCtorInfo.Class->m_ClassInstancesRegistry.Insert(m_GUID, this);
		}
	}

	/*!
	 * Deinitializes object.
	 */
	GDAPI GD_OBJECT_KERNEL Object::~Object()
	{
		GD_DEBUG_VERIFY(m_ReferenceCount == 0, "Attempting to delete a referenced object.");
	}

	// ------------------------------------------------------------------------------------------
	// Global Registry & Garbage Collecting.
	// ------------------------------------------------------------------------------------------

	/*!
	 * @brief Increments reference counter for this object.
	 * This method should be called for each copy of the pointer to the object.
	 *
	 * @returns New value of the reference counter.
	 * @see RefPtr<T> class for automated reference counting.
	 */
	GDAPI GD_OBJECT_KERNEL UInt32 Object::AddRef()
	{
		return ++m_ReferenceCount;
	}

	/*!
	 * @brief Decrements reference counter for this object.
	 * When reference counter reaches zero, object is recycled.
	 * This method should be called for each copy of the pointer to the object.
	 * 
	 * @returns New value of the reference counter.
	 * @see RefPtr<T> class for automated reference counting.
	 */
	GDAPI GD_OBJECT_KERNEL UInt32 Object::Release()
	{
		if (--m_ReferenceCount == 0)
		{
			// Zero reference counter reached, it is time to recycle object.
			{	
				ScopedLiteMutex classInstancesRegistryLock(GetClass()->m_ClassInstancesRegistryLock);
				GetClass()->m_ClassInstancesRegistry.Erase(m_GUID);
			}
			gd_delete this;
			return 0;
		}
		return m_ReferenceCount;
	}

	/*!
	 * @brief Finds the object by it's GUID.
	 * Searches all class-related registry for object with specified GUID.
	 *
	 * @param guid GUID of the required object. 
	 * @param klass Class of the required object.
	 *
	 * @returns Found object or null pointer.
	 */
	GDAPI GD_OBJECT_KERNEL RefPtr<Object> Object::FindObject(GUID const& guid, ClassPtr const klass)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		GD_DEBUG_VERIFY(guid != EmptyGUID, "Invalid GUID was specified.");

		{	// First, trying to find this object among instances of this class.
			ScopedLiteMutex classInstancesRegistryLock(klass->m_ClassInstancesRegistryLock);
			auto const objectPtr = klass->m_ClassInstancesRegistry.Find(guid);
			if (objectPtr != nullptr)
			{
				return *objectPtr;
			}
		}

		// Next, trying to find this object among instances of the derived classes.
		for (auto const derivedKlass : klass->m_ClassDerived)
		{
			auto const object = FindObject(guid, derivedKlass);
			if (object != nullptr)
			{
				return object;
			}
		}
		return nullptr;
	}

	/*!
	 * @brief Finds the object by GUID or creates new instance of the specified class with GUID.
	 * Searches all class-related registry for object with specified GUID.
	 * If nothing was found, creates the new object.
	 *
	 * @param guid GUID of the required object.
	 * @param klass Class of the required object.
	 *
	 * @returns Found or created object.
	 */
	GDAPI GD_OBJECT_KERNEL RefPtr<Object> Object::FindOrCreateObject(GUID const& guid, ClassPtr const klass /*= nullptr*/)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");

		// First, trying to find the object among registered.
		if (guid != EmptyGUID)
		{
			auto const object = FindObject(guid, klass);
			if (object != nullptr)
			{
				return object;
			}
		}

		// No object was found, and we are sure it does not exist. We need to create it.
		GD_VERIFY(klass->m_ClassConstructor != nullptr, "Specified class is abstract.");

		/*
		 * Some life principles for the new-born object:
		 * 1. Happiness lies in its absence. Suffering is a measure of creation.
		 * 2. Creation justifies any expenses for creation.
		 * 3. Creation should lead to creation.
		 * (I was drunk when I've writen this. Sorry)
		 */
		ObjectCtorInfo objectCtorInfo = {};
		objectCtorInfo.Guid = guid == EmptyGUID ? GUID::New() : guid;
		objectCtorInfo.Class = const_cast<Class*>(klass);
		auto const object = RefPtr<Object>(klass->m_ClassConstructor(objectCtorInfo));
		object->Release();
		return object;
	}

	/*!
	 * @brief Finds all instance of the specified class.
	 * This function is rather slow. It is not recommended to call it every frame.
	 *
	 * @param klass Class of the required objects.
	 * @returns Vector with all instances of the specified class.
	 */
	GDAPI GD_OBJECT_KERNEL Vector<RefPtr<Object>> Object::FindObjects(ClassPtr const klass)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		{	
			// Safely copying all instances of the specified class into array.
			ScopedLiteMutex classInstancesRegistryLock(klass->m_ClassInstancesRegistryLock);
			Vector<RefPtr<Object>> foundObjects(0, klass->m_ClassInstancesRegistry.GetLength());
			for (auto const& foundObjectPair : klass->m_ClassInstancesRegistry)
			{
				foundObjects.InsertLast(foundObjectPair.Value);
			}
			return foundObjects;
		}
	}

	// ------------------------------------------------------------------------------------------
	// Reflection & Runtime type information.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns true, if this object is instance of the specified class (or derived classes).
	 *
	 * @param klass Possible object class.
	 * @returns True, if this object is instance of the specified class (or derived classes).
	 */
	GDAPI GD_OBJECT_HELPER bool Object::IsInstanceOf(ClassPtr const klass) const
	{
		for (auto thisKlass = GetClass(); thisKlass != nullptr; thisKlass = thisKlass->ClassSuper)
		{
			if (klass == thisKlass)
				return true;
		}
		return false;
	}

GD_NAMESPACE_END
