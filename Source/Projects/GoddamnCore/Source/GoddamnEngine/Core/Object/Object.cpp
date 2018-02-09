// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Base class for all engine entities.
 */
#include <GoddamnEngine/Core/Object/Object.h>

GD_NAMESPACE_BEGIN
	
	static Map<String, ObjectClassPtr>& g_ObjectClasses()
	{
		static Map<String, ObjectClassPtr> o;
		return o;
	}

	/*!
	 * Initializes a new class. 
	 * @warning Never call this constructor manually.
	 */
	GDAPI GD_OBJECT_KERNEL ObjectClass::ObjectClass(CStr const className, ObjectClassPtr const classSuper, ObjectCtorProc const classConstructor)
		: ClassName(className), ClassSuper(classSuper), m_InstanceCtor(classConstructor)
	{
		g_ObjectClasses().Insert(className, this);
		if (classSuper != nullptr)
		{
			const_cast<ObjectClass*>(classSuper)->m_ClassDerived.InsertLast(this);
		}
	}

	/*!
	 * Searches for class by it's name.
	 *
	 * @param className Name of the class we are looking for.
 	 * @returns Pointer to found class or nullptr if no suck class was found.
	 */
	GDAPI GD_OBJECT_KERNEL ObjectClassPtr ObjectClass::FindClass(String const& className)
	{
		auto const classPtrPtr = g_ObjectClasses().Find(className);
		return classPtrPtr != nullptr ? *classPtrPtr : nullptr;
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               'Object' class.                                    ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// ------------------------------------------------------------------------------------------
	// Constructor and destructor.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Initializes object.
	 */
	GDAPI GD_OBJECT_KERNEL Object::Object()
		: m_ReferenceCount(1)
	{
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
	GDAPI void GD_OBJECT_KERNEL Object::AddRef()
	{
		++m_ReferenceCount;
	}

	/*!
	 * @brief Decrements reference counter for this object.
	 * When reference counter reaches zero, object is recycled.
	 * This method should be called for each copy of the pointer to the object.
	 * 
	 * @returns New value of the reference counter.
	 * @see RefPtr<T> class for automated reference counting.
	 */
	GDAPI void GD_OBJECT_KERNEL Object::Release()
	{
		if (--m_ReferenceCount == 0)
		{
			// Zero reference counter reached, it is time to recycle object.
			GetClass()->m_Instances.Erase(m_GUID);
			gd_delete this;
		}
	}
	
	/*!
	 * @brief Finds the object by GUID.
	 * Searches through all class-related (it's own and derived classes') registry for object with specified GUID.
	 *
	 * @param guid GUID of the required object.
	 * @param klass Class of the required object.
	 *
	 * @returns Found object or null pointer.
	 */
	GDINT RefPtr<Object> GD_OBJECT_KERNEL Object::FindClassRelatedObjectByGUID(GUID const& guid, ObjectClassPtr const klass)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		GD_DEBUG_VERIFY(guid != EmptyGUID, "Invalid GUID was specified.");
			
		// First, trying to find this object among instances of this class.
		auto const classObjectPtr = klass->m_Instances.Find(guid);
		if (classObjectPtr != nullptr)
		{
            auto const classObject = *classObjectPtr;
            classObject->AddRef();
			return classObject;
		}

		// Next, trying to find this object among instances of the derived classes.
		for (auto const derivedKlass : klass->m_ClassDerived)
		{
			auto const object = FindClassRelatedObjectByGUID(guid, derivedKlass);
			if (object != nullptr)
			{
				return object;
			}
		}
		return nullptr;
	}

	/*!
	 * @brief Finds the object by GUID.
	 *
	 * @param guid GUID of the required object.
	 * @returns Found object or null pointer.
	 */
	GDINT RefPtr<Object> GD_OBJECT_HELPER Object::FindGlobalObjectByGUID(GUID const& guid)
	{
		return FindClassRelatedObjectByGUID(guid, GetStaticClass());
	}

	/*!
	 * @brief Finds all instance of the specified class.
	 * This function is rather slow. It is not recommended to call it every frame.
	 *
	 * @param klass Class of the required objects.
	 * @returns Vector with all instances of the specified class.
	 */
	GDAPI Vector<RefPtr<Object>> GD_OBJECT_KERNEL Object::FindObjectsByClass(ObjectClassPtr const klass)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		
		// Safely copying all instances of the specified class into array.
		Vector<RefPtr<Object>> foundObjects(0, klass->m_Instances.GetLength());
		GD_STUBBED(FindObjectsByClass);
		/*for (auto const& foundObjectPair : klass->m_ClassInstances)
		{
			foundObjects.InsertLast(foundObjectPair.Value);
		}*/

		return foundObjects;
	}
	
	/*!
	 * @brief Finds all instance of the specified and derived classes.
	 * This function is rather slow. It is not recommended to call it every frame.
	 *
	 * @param klass Class of the required objects.
	 * @returns Vector with all instances of the specified class.
	 */
	GDAPI ChunkedVector<RefPtr<Object>> GD_OBJECT_KERNEL Object::FindClassRelatedObjects(ObjectClassPtr const klass)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");
		
		// Safely copying all instances of the specified class into array.
		auto foundObjects(FindObjectsByClass(klass));

		// Next, safely copying all instances of the derived classes into array.
		for (auto const derivedKlass : klass->m_ClassDerived)
		{
			foundObjects += FindClassRelatedObjects(derivedKlass);
		}

		return foundObjects;
	}

	/*!
	 * @brief Finds the object by GUID or creates new instance of the specified class with GUID.
	 * Searches all class-related (it's own and derived classes') registry for object with specified GUID.
	 * If nothing was found, creates the new object.
	 *
	 * @param guid GUID of the required object.
	 * @param klass Class of the required object.
	 *
	 * @returns Found or created object.
	 */
	GDAPI RefPtr<Object> GD_OBJECT_KERNEL Object::CreateOrFindClassRelatedObjectByGUID(GUID const& guid, ObjectClassPtr const klass /*= nullptr*/)
	{
		GD_DEBUG_VERIFY(klass != nullptr, "Null pointer class was specified.");

		// First, trying to find the object among registered.
		if (guid != EmptyGUID)
		{
			auto const object = FindClassRelatedObjectByGUID(guid, klass);
			if (object != nullptr)
			{
				return object;
			}
			auto const globalObject = FindGlobalObjectByGUID(guid);
			GD_VERIFY(globalObject == nullptr, "Object with specified GUID exists, but is of the unrelated type.");
		}

		// No object was found, and we are sure it does not exist. We need to create it.
		GD_VERIFY(klass->m_InstanceCtor != nullptr, "Specified class is abstract.");
		auto const object = klass->m_InstanceCtor();
		if (guid == EmptyGUID)
		{
			// We need to be sure that the generated GUID is really unique.
			object->m_GUID = GUID::New();
			while (FindGlobalObjectByGUID(object->m_GUID) != nullptr)
			{
				object->m_GUID = GUID::New();
			}
		}
		else
		{
			object->m_GUID = guid;
		}

		// Registering the object in the class table.
		klass->m_Instances.Insert(object->m_GUID, object);
		return object;
	}

	// ------------------------------------------------------------------------------------------
	// Reflection & Runtime type information.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Returns pointer to the type information object for this class.
	 *
	 * @returns Pointer to the type information object for this class.
	 * @see Class interface.
	 */
	GDAPI ObjectClassPtr GD_OBJECT_GENERATED Object::GetStaticClass()
	{
		static ObjectClass objectClass("Object", nullptr, ObjectCtorGetter<Object>::Get());
		return &objectClass;
	}

	/*!
	 * Returns true, if this object is instance of the specified class (or derived classes).
	 *
	 * @param klass Possible object class.
	 * @returns True, if this object is instance of the specified class (or derived classes).
	 */
	GDAPI bool GD_OBJECT_HELPER Object::IsRelatedToClass(ObjectClassPtr const klass) const
	{
		for (auto thisKlass = GetClass(); thisKlass != nullptr; thisKlass = thisKlass->ClassSuper)
		{
			if (klass == thisKlass)
			{
				return true;
			}
		}
		return false;
	}

GD_NAMESPACE_END
