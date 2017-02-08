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
#include <GoddamnEngine/Core/Object/Base.h>
#include <GoddamnEngine/Core/Object/GUID.h>
#include <GoddamnEngine/Core/Object/RefPtr.h>
#include <GoddamnEngine/Core/Object/Struct.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Concurrency/Atomics.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                Object class.                                     ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Represents constructor information for objects. 
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL struct ObjectCtorInfo final
	{
	public:
		GUID     Guid;
		Class*   Class;

	private:
		friend class Object;
		GDINL ObjectCtorInfo() : Class(nullptr) {}
		GDINL ~ObjectCtorInfo() {}
	};	// struct ObjectCtorInfo

	GD_OBJECT_KERNEL enum ObjectCastFlags : UInt64
	{
		CFNoCastFlags = 0,
		CFObject = 0,
		CFSerializableObject = GD_BIT(0),
		CFNotInheritableFlags = UInt64Max,

		CFBehaviour
	};	// enum ObjectCastFlags
	GD_ENUM_DEFINE_FLAG_OPERATORS(ObjectCastFlags);

	// **------------------------------------------------------------------------------------------**
	//! @brief Base interface for all engine-handled entities.
	//! Goddamn Engine's object system provides the following some automated subsystems.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL class Object : public Struct
	{
		friend class Package;

	private:
		GUID m_GUID;
		/*Atomic*/UInt32 m_ReferenceCount;

	protected:
		static ObjectCastFlags const s_CastFlags = CFObject;

	protected:

		/*!
		 * @brief Initializes object.
		 * Initializes the object and registers it inside the system to make garbage collecting
		 * and recycling work.
		 *
		 * @param objectCtorInfo Construction information for this object.
		 */
		GDAPI GD_OBJECT_KERNEL explicit Object(ObjectCtorInfo const& objectCtorInfo);

		/*!
		 * Deinitializes object.
		 */
		GDAPI GD_OBJECT_KERNEL virtual ~Object();

	public:

		// ------------------------------------------------------------------------------------------
		// Global Registry & Garbage Collecting.
		// ------------------------------------------------------------------------------------------
		
		GDINL GD_OBJECT_KERNEL GUID const& GetGUID() const { return m_GUID; }

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
		 * @brief Finds the object by it's GUID.
		 * Searches all class-related registry for object with specified GUID.
		 *
		 * @param klass Class of the required object.
		 * @param guid GUID of the required object.
		 *
		 * @returns Found object or null pointer.
		 */
		//! @{
		GDAPI static GD_OBJECT_KERNEL RefPtr<Object> FindObject(GUID const& guid, ClassPtr const klass);
		template<typename TObject>
		GDINL static GD_OBJECT_HELPER RefPtr<TObject> FindObject(GUID const& guid)
		{
			return GD::object_cast<RefPtr<TObject>>(Object::FindObject(guid, TObject::GetStaticClass()));
		}
		//! @}

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
		//! @{
		GDAPI static GD_OBJECT_KERNEL RefPtr<Object> FindOrCreateObject(GUID const& guid, ClassPtr const klass = nullptr);
		template<typename TObject>
		GDINL static GD_OBJECT_HELPER RefPtr<TObject> FindOrCreateObject(GUID const& guid)
		{
			return GD::object_cast<TObject>(Object::FindOrCreateObject(guid, TObject::GetStaticClass()));
		}
		//! @}

		/*!
		 * @brief Finds all instance of the specified class.
		 * This function is rather slow. It is not recommended to call it every frame.
		 *
		 * @param klass Class of the required objects.
		 * @returns Vector with all instances of the specified class.
		 */
		GDAPI static GD_OBJECT_KERNEL Vector<RefPtr<Object>> FindObjects(ClassPtr const klass);

		// ------------------------------------------------------------------------------------------
		// Reflection & Runtime type information.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns pointer to the type information object for this type.
		 *
		 * @returns Pointer to the type information object for this type.
		 * @see Class interface.
		 */
		GDAPI GD_OBJECT_GENERATED virtual ClassPtr GetClass() const GD_PURE_VIRTUAL;

		/*!
		 * Returns pointer to the type information object for this class.
		 *
		 * @returns Pointer to the type information object for this class.
		 * @see Class interface.
		 */
		GDAPI GD_OBJECT_GENERATED static ClassPtr GetStaticClass() { return nullptr; }

		/*!
		 * Returns true, if this object is instance of the specified class (or derived classes).
		 *
		 * @param klass Possible object class.
		 * @returns True, if this object is instance of the specified class (or derived classes).
		 */
		//! @{
		GDAPI GD_OBJECT_HELPER bool IsInstanceOf(ClassPtr const klass) const;
		template<typename TObject>
		GDINL GD_OBJECT_HELPER bool IsInstanceOf() const
		{
			return IsInstanceOf(TObject::GetStaticClass());
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
	// ******                Very scary macros for generating boilerplate code.                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*! Declares a base class information. Implements RTTI & reflection algorithms. */
	#define GD_DECLARE_OBJECT_BASE(TEAPI, TThisClass, TSuperClass, TCastFlags, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_STRUCT(TThisClass, TSuperClass, ##__VA_ARGS__); \
			\
		public: \
			template<typename TObject> \
			friend struct ObjectCtorGetter; \
			static ObjectCastFlags const s_CastFlags = TCastFlags | (CFNotInheritableFlags & TSuperClass::s_CastFlags); \
			\
		protected: \
			GDINL explicit TThisClass(ObjectCtorInfo const& objectCtorInfo) : TSuperClass(objectCtorInfo) {} \
			TEAPI virtual ~TThisClass(); \
			\
		public: \
			GDINL virtual ClassPtr GetClass() const override { return TThisClass::GetStaticClass(); }\
			GDINL static RefPtr<TThisClass> Create() { return Object::FindOrCreateObject<TThisClass>(EmptyGUID); } \
			TEAPI static ClassPtr GetStaticClass(); \

	/*! Implements the class information. Implements RTTI & reflection algorithms. */
	#define GD_IMPLEMENT_OBJECT(TEAPI, TThisClass) GD_OBJECT_KERNEL \
			static Class g_##TThisClass##Class(#TThisClass, TThisClass::Super::GetStaticClass(), TThisClass::s_CastFlags, ObjectCtorGetter<TThisClass>::Get());\
			TEAPI ClassPtr TThisClass::GetStaticClass() \
			{ \
				return &g_##TThisClass##Class; \
			} \

	/*! Declares an intrinsic class information. */
	#define GD_DECLARE_OBJECT_INTRINSIC(TThisClass, TSuperClass, TCastFlags, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_BASE(GDAPI, TThisClass, TSuperClass, TCastFlags, ##__VA_ARGS__) \

	/*! Declares a GoddamnCore intrinsic class information. */
	#define GD_DECLARE_OBJECT_INTRINSIC_CORE(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_INTRINSIC(TThisClass, TSuperClass, CF##TThisClass, "CoreIntrinsic/"#TThisClass, ##__VA_ARGS__) \
			friend class Package; friend class HotReload; \

	/*! Declares a GoddamnEngine intrinsic class information. */
	#define GD_DECLARE_OBJECT_INTRINSIC_ENGINE(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_INTRINSIC(TThisClass, TSuperClass, /*CF##TThisClass*/CFNoCastFlags, "EngineIntrinsic/"#TThisClass, ##__VA_ARGS__) \
			friend class Component; friend class Resource; \

	/*! Declares a GoddamnEngine intrinsic class information without tagged casts support. */
	#define GD_DECLARE_OBJECT_INTRINSIC_ENGINE_EXT(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_INTRINSIC(TThisClass, TSuperClass, CFNoCastFlags, "EngineIntrinsic/"#TThisClass, ##__VA_ARGS__) \

	/*! Declares the user class information. */
	#define GD_DECLARE_OBJECT(TThisClass, TSuperClass, TModule, ...) GD_OBJECT_KERNEL \
			GD_DECLARE_OBJECT_BASE(GDINT, TThisClass, TSuperClass, CFNoCastFlags, #TModule, ##__VA_ARGS__) \

	/*!
	 * @brief Performs a dynamic cast for two objects.
	 */
	template<typename TObjectDest, typename TObjectSource>
	GDINL static TObjectDest* object_cast(TObjectSource* const sourceObject)
	{
		static_assert(TypeTraits::IsBase<Object, TObjectSource>::Value, "Class 'TObjectSource' should be derived from 'Object'.");
		static_assert(TypeTraits::IsBase<Object, TObjectDest>::Value, "Class 'TObjectDest' should be derived from 'Object'.");
		static_assert(TypeTraits::IsBase<TObjectSource, TObjectDest>::Value, "Class 'TObjectDest' should be derived from 'TObjectDest'.");

		GD_STUBBED(object_cast);
		if (sourceObject != nullptr)
		{
			return static_cast<TObjectDest*>(sourceObject);
		}
		return nullptr;
	}
	template<typename TObjectDest, typename TObjectSource>
	GDINL static RefPtr<TObjectDest> object_cast(RefPtr<TObjectSource> const sourceObject)
	{
		auto const destObject = object_cast<TObjectDest>(sourceObject.Get());
		if (destObject != nullptr)
		{
			destObject->AddRef();
		}
		return destObject;
	}

GD_NAMESPACE_END
