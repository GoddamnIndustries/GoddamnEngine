// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Object.h
 * File contains base class for all engine entities.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Misc/GUID.h>
#include <GoddamnEngine/Core/Object/RefPtr.h>
#include <GoddamnEngine/Core/Object/Struct.h>
#include <GoddamnEngine/Core/Object/ObjectUtility.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Concurrency/Atomics.h>

GD_NAMESPACE_BEGIN

	class Object;
	class ObjectClass;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           'ObjectRegistry' class.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Multithreaded object registry.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_HELPER struct ObjectRegistry final : public TNonCopyable
	{
	private:
		Map<GUID, Object*> m_Registry;
		CriticalSection m_RegistryLock;

	public:
		GDINL SizeTp GetLength() const
		{
			ScopedCriticalSection registryLock(m_RegistryLock);
			return m_Registry.GetLength();
		}

		GDINL Object* const* Find(GUID const& guid) const
		{
			ScopedCriticalSection registryLock(m_RegistryLock);
			return m_Registry.Find(guid);
		}

		GDINL void Insert(GUID const& guid, Object* const object)
		{
			ScopedCriticalSection registryLock(m_RegistryLock);
			m_Registry.Insert(guid, object);
		}

		GDINL void Erase(GUID const& guid)
		{
			ScopedCriticalSection registryLock(m_RegistryLock);
			m_Registry.Erase(guid);
		}

	};	// class ObjectRegistry

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            'ObjectClass' class.                                  ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Implements runtime class information.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL class ObjectClass : public TNonCopyable
	{
		friend class Object;

	public:
		String		   ClassName;		/*!< Name of the class. */
		ObjectClassPtr ClassSuper;		/*!< Pointer to the super class information. */

	private:
		Vector<ObjectClassPtr> m_ClassDerived;
		
		ObjectCtorProc m_InstanceCtor;
		ObjectRegistry m_Instances;

	public:

		GDAPI GD_OBJECT_GENERATED explicit ObjectClass(CStr const className, ObjectClassPtr const classSuper, ObjectCtorProc const classConstructor);

	public:

		/*!
		 * Searches for class by it's name.
		 *
		 * @param className Name of the class we are looking for.
 		 * @returns Pointer to found class or nullptr if no suck class was found.
		 */
		GDAPI GD_OBJECT_KERNEL static ObjectClassPtr FindClass(String const& className) { return nullptr; }

	};	// struct ObjectClass

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               'Object' class.                                    ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! @brief Base interface for all engine-handled entities.
	//! Goddamn Engine's object system provides the following some automated subsystems.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL class Object : public Struct
	{
	private:
		GUID m_GUID;
		AtomicUInt32 m_ReferenceCount;

	protected:
		GDAPI GD_OBJECT_KERNEL explicit Object();
		GDAPI GD_OBJECT_KERNEL virtual ~Object();

	public:

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
		GDAPI GD_OBJECT_KERNEL UInt32 AddRef();

		/*!
		 * @brief Decrements reference counter for this object.
		 * When reference counter reaches zero, object is recycled.
		 * This method should be called for each copy of the pointer to the object.
		 * 
		 * @returns New value of the reference counter.
		 * @see RefPtr<T> class for automated reference counting. 
		 */
		GDAPI GD_OBJECT_KERNEL UInt32 Release();
		
		/*!
		 * Return global unique ID of the object.
		 */
		GDINL GD_OBJECT_KERNEL GUID const& GetGUID() const { return m_GUID; }

	private:
		GDINT static GD_OBJECT_KERNEL RefPtr<Object> FindClassRelatedObjectByGUID(GUID const& guid, ObjectClassPtr const klass);
		GDINT static GD_OBJECT_KERNEL RefPtr<Object> FindGlobalObjectByGUID(GUID const& guid);

	public:

		/*!
		 * @brief Finds all instance of the specified class.
		 * This function is rather slow. It is not recommended to call it every frame.
		 *
		 * @param klass Class of the required objects.
		 * @returns Vector with all instances of the specified class.
		 */
		GDAPI static GD_OBJECT_KERNEL Vector<RefPtr<Object>> FindObjectsByClass(ObjectClassPtr const klass);

		/*!
		 * @brief Finds all instance of the specified and derived classes.
		 * This function is rather slow. It is not recommended to call it every frame.
		 *
		 * @param klass Class of the required objects.
		 * @returns Vector with all instances of the specified class.
		 */
		GDAPI static GD_OBJECT_KERNEL ChunkedVector<RefPtr<Object>> FindClassRelatedObjects(ObjectClassPtr const klass);

		/*!
		 * @brief Finds the object by GUID or creates new instance of the specified class with GUID.
		 * 
		 * Searches all class-related (it's own and derived classes') registry for object with specified GUID.
		 * If nothing was found, creates the new object.
		 *
		 * @param guid GUID of the required object.
		 * @param klass Class of the required object.
		 *
		 * @returns Found or created object.
		 */
		//! @{
		GDAPI static GD_OBJECT_KERNEL RefPtr<Object> CreateOrFindClassRelatedObjectByGUID(GUID const& guid, ObjectClassPtr const klass = nullptr);
		template<typename TObject>
		GDINL static GD_OBJECT_HELPER RefPtr<TObject> CreateOrFindClassRelatedObjectByGUID(GUID const& guid)
		{
			return static_cast<RefPtr<TObject>>(Object::CreateOrFindClassRelatedObjectByGUID(guid, TObject::GetStaticClass()));
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		// Reflection & Runtime type information.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns pointer to the type information object for this type.
		 *
		 * @returns Pointer to the type information object for this type.
		 * @see Class interface.
		 */
		GDAPI GD_OBJECT_GENERATED virtual ObjectClassPtr GetClass() const GD_PURE_VIRTUAL;

		/*!
		 * Returns pointer to the type information object for this class.
		 *
		 * @returns Pointer to the type information object for this class.
		 * @see Class interface.
		 */
		GDAPI GD_OBJECT_GENERATED static ObjectClassPtr GetStaticClass();

		/*!
		 * Returns true, if this object is instance of the specified class (or derived classes).
		 *
		 * @param klass Possible object class.
		 * @returns True, if this object is instance of the specified class (or derived classes).
		 */
		//! @{
		GDAPI GD_OBJECT_HELPER bool IsRelatedToClass(ObjectClassPtr const klass) const;
		template<typename TObject>
		GDINL GD_OBJECT_HELPER bool IsRelatedToClass() const
		{
			return Object::IsRelatedToClass(TObject::GetStaticClass());
		}
		//! @}

		// ReSharper disable CppHidingFunction

		/*!
		 * Visits all properties of this class.
		 * 
		 * @param objectVisitor Visitor to process this object.
		 * @see IObjectVisitor interface.
		 */
		GDAPI GD_OBJECT_GENERATED virtual void Reflect(ObjectVisitor& objectVisitor)
		{
			GD_NOT_USED(objectVisitor);
		}

		// ReSharper restore CppHidingFunction

	};	// class Object

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                          Object dynamic cast operations.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	template<typename TObjectDest, typename TObjectSource>
	GD_OBJECT_HELPER struct ObjectCastImpl;
	template<typename TObjectDest, typename TObjectSource>
	GD_OBJECT_HELPER struct ObjectCastImpl<TObjectDest*, TObjectSource*> final : public TNonCreatable
	{
		static_assert(TypeTraits::IsBase<Object, TObjectDest>::Value, "Class 'TObjectDest' should be derived from 'Object'.");
		static_assert(TypeTraits::IsBase<Object, TObjectSource>::Value, "Class 'TObjectSource' should be derived from 'Object'.");
		static_assert(TypeTraits::IsRelated<TObjectSource, TObjectDest>::Value, "Class 'TObjectDest' should be related to 'TObjectDest'.");
		GDINL static TObjectDest* Cast(TObjectSource* const sourceObject)
		{
			if (TypeTraits::IsBase<TObjectDest, TObjectSource>::Value || sourceObject->template IsRelatedToClass<TObjectDest>())
			{
				auto const destObject = static_cast<TObjectDest*>(sourceObject); 
				destObject->AddRef();
				return destObject;
			}
			return nullptr;
		}
	};	// struct ObjectCastImpl<D*, S*>
	template<typename TObjectDest, typename TObjectSource>
	GD_OBJECT_HELPER struct ObjectCastImpl<RefPtr<TObjectDest>, RefPtr<TObjectSource>> final : public TNonCreatable
	{
		GDINL static RefPtr<TObjectDest> Cast(RefPtr<TObjectSource> const sourceObject)
		{
			return ObjectCastImpl<TObjectDest*, TObjectSource*>::Cast(sourceObject.Get());
		}
	};	// struct ObjectCastImpl<RefPtr<D>, RefPtr<S>>

	/*!
	 * Performs a dynamic cast for two objects.
	 * 
	 * @param sourceObject Object to cast.
	 * @returns Null pointer if classes are not related and a valid pointer otherwise.
	 */
	template<typename TObjectDest, typename TObjectSource>
	GDINL GD_OBJECT_KERNEL static TObjectDest object_cast(TObjectSource const sourceObject)
	{
		return ObjectCastImpl<TObjectDest, TObjectSource>::Cast(sourceObject);
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                Very scary macros for generating boilerplate code.                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*! 
	 * Declares a base class information. Implements RTTI & reflection algorithms. 
	 * 
	 * @param TEAPI Information exporting attributes.
	 * @param TThisClass Name of the class.
	 * @param TSuperClass Name of the parent class.
	 */
	#define GD_DECLARE_OBJECT_BASE(TEAPI, TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_STRUCT(TThisClass, TSuperClass, ##__VA_ARGS__); \
			\
		private: \
			template<typename, bool> \
			friend struct ObjectCtorGetterBase; \
			\
		public: \
			GDINL virtual ObjectClassPtr GetClass() const override { return TThisClass::GetStaticClass(); }\
			GDINL static RefPtr<TThisClass> CreateObject() { return Object::CreateOrFindClassRelatedObjectByGUID<TThisClass>(EmptyGUID); } \
			TEAPI static ObjectClassPtr GetStaticClass(); \

	/*! 
	 * Declares the user class information. 
	 * 
	 * @param TThisClass Name of the class.
	 * @param TSuperClass Name of the parent class.
	 */
	#define GD_DECLARE_OBJECT(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_BASE(GDINT, TThisClass, TSuperClass, ##__VA_ARGS__) \

	/*! 
	 * Declares an intrinsic class information. 
	 *
	 * @param TThisClass Name of the class.
	 * @param TSuperClass Name of the parent class.
	 */
	#define GD_DECLARE_OBJECT_INTRINSIC(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_BASE(GDAPI, TThisClass, TSuperClass, ##__VA_ARGS__) \

	template<typename TObject, bool TIsAbstract>
	GD_OBJECT_HELPER struct ObjectCtorGetterBase : public TNonCreatable
	{
		GDINL static constexpr ObjectCtorProc Get()
		{
			return nullptr;
		}
	};	// struct ObjectCtorGetterBase<T, B>
	template<typename TObject>
	GD_OBJECT_HELPER struct ObjectCtorGetterBase<TObject, false> : public TNonCreatable
	{
		GDINL static constexpr ObjectCtorProc Get()
		{
			return []() -> Object* { return gd_new TObject(); };
		}
	};	// struct ObjectCtorGetterBase<T, false>
	template<typename TObject>
	GD_OBJECT_HELPER struct ObjectCtorGetter final : public ObjectCtorGetterBase<TObject, TypeTraits::IsAbstract<TObject>::Value>
	{
	};	// struct ObjectCtorGetter

	/*! 
	 * Implements the class information. Implements RTTI & reflection algorithms. 
	 * 
	 * @param TEAPI Information exporting attributes. Should match the one used in @c GD_DECLARE_OBJECT_BASE
	 * @param TThisClass Name of the class.
	 */
	#define GD_IMPLEMENT_OBJECT_BASE(TEAPI, TThisClass) GD_OBJECT_KERNEL \
			static ObjectClass g_##TThisClass##Class(#TThisClass, TThisClass::Super::GetStaticClass(), ObjectCtorGetter<TThisClass>::Get());\
			TEAPI  ObjectClassPtr TThisClass::GetStaticClass() \
			{ \
				return &g_##TThisClass##Class; \
			} \

	/*! 
	 * Implements the user class information. 
	 * @param TThisClass Name of the class.
	 */
	#define GD_IMPLEMENT_OBJECT(TThisClass) GD_OBJECT_KERNEL \
			GD_IMPLEMENT_OBJECT_BASE(GDINT, TThisClass)

	/*! 
	 * Implements the intrinsic class information. 
	 * @param TThisClass Name of the class.
	 */
	#define GD_IMPLEMENT_OBJECT_INTRINSIC(TThisClass) GD_OBJECT_KERNEL \
			GD_IMPLEMENT_OBJECT_BASE(GDAPI, TThisClass)

GD_NAMESPACE_END
