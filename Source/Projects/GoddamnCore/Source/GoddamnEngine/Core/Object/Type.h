// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! @file GoddamnEngine/Core/Object/Type.h
 * Common interface for runtime type information and reflection.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraitsEx.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IClass interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// ------------------------------------------------------------------------------------------
	//! Base interface for all reflectable classes (structures/interfaces).
	struct IClass : public IVirtuallyDestructible
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Returns pointer to the class information object of this object.
		//! @{
		GDAPI virtual struct ClassInformation const* GetClassInformation() const GD_PURE_VIRTUAL;
		GDINL static struct ClassInformation const* GetTypeClassInformation() { return nullptr; }
		//! @}

	};	// struct IClass

	typedef IClass* (*IClassConstrutorProc) ();
	template<typename Tp>
	struct IClassConstrutor final
	{
	private:
		GDINL static IClass* Construtor() { return GD_NEW(Tp); }
	public:
		GDINL static IClassConstrutorProc Get() { return &Construtor; }
	};	// struct IClassConstrutor

#define GD_CLASS_NOT_CONSTRUCTIBLE_TAG \
	template<> \
	struct IClassConstrutor<GD_GLUE(__ClassAtLine, __LINE__)> { GDINL static IClassConstrutorProc Get() { return nullptr; } }; \
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ClassInformation structure.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Contains brief type information for any arbitrary class.
	struct ClassInformation final : public IUncopiable
	{
	private:
		String                  const ClassName;
		ClassInformation const* const ClassSuper;
		IClassConstrutorProc    const ClassConstrutor;

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors & Destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !GD_DOCUMENTATION
	private:
		GDAPI ClassInformation(CStr const ClassName, ClassInformation const* const ClassSuper, IClassConstrutorProc const ClassConstrutor);
	public:
		template<typename ClassTp, typename SuperClassTp>
		GDINL ClassInformation(Dummy<ClassTp> const, Dummy<SuperClassTp> const)
			: ClassInformation(typeid(ClassTp).name(), SuperClassTp::GetTypeClassInformation(), IClassConstrutor<ClassTp>::Get())
		{}
#endif	// if !GD_DOCUMENTATION

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns name of the type.
		GDINL String const& GetName() const { return ClassName; }

		// ------------------------------------------------------------------------------------------
		//! Returns class information for the super class of this type.
		GDINL ClassInformation const* GetSuper() const { return ClassSuper; }


		/// @brief			Returns information about type or nullptr if type is not registered.
		/// @param typeName	Name of type searching for. Is should be equal to full typename.
		/// @returns		'TypeInformation' for type.
		GDAPI static const ClassInformation* SearchForType(const String& typeName)
		{
			return nullptr;
		}


		/// @brief			Contructs instance of this type,
		///					passes 'arg' to virtual constructor specified in 'TypeInformationGenerator'
		/// @param arg		Argument would be passed to virtual constructor if this type is abstract
		/// @see TypeInformationGenerator
		GDAPI IClass* CreateInstanceAlloc(...) const
		{
			return ClassConstrutor();
		}

		/// @brief			Calls destuctor of this type for 'pointer' and deallocates it
		/// @param pointer	Object to delete
		GDAPI void DestroyInstanceDealloc(IClass* const pointer) const
		{
			GD_DELETE(pointer);
		}

		/// @brief			Determines if 'typeinfo' is derived from this type information
		/// @param typeinfo 'Typeinformation' to check for
		/// @returns		Boolean for 'typeinfo' is derived from this type information
		GDAPI bool IsBaseOf(const ClassInformation* typeinfo) const
		{
			if (typeinfo == nullptr)
				return true;

			for (const ClassInformation* t = typeinfo; t != nullptr; t = t->ClassSuper)
			{
				if (t == this)
				{
					return true;
				}
			}

			return false;
		}

		/// @brief			Determines if this type information is derived from 'typeinfo'
		/// @param typeinfo	Type to check for. It should be registered in RTTI system.
		/// @returns		Boolean for this type information is derived from 'typeinfo'
		GDAPI bool IsChildOf(const ClassInformation* typeinfo) const
		{
			return typeinfo->IsBaseOf(this);
		}

	};	// struct ClassInformation

#define GD_DEFINE_CLASS_INFORMATION(Class, SuperClass) \
private: \
	static_assert(GD::TypeTraits::IsBase<SuperClass, Class>::Value, "'GD_DEFINE_CLASS_INFORMATION' error: " #Class " should be derived from " #SuperClass "."); \
	static_assert(GD::TypeTraits::IsBase<GD::IClass, Class>::Value, "'GD_DEFINE_CLASS_INFORMATION' error: " #Class " should be derived from IClass."); \
	typedef Class ClassTp; \
	typedef SuperClass SuperClassTp; \
public: \
	GDAPI static  GD::ClassInformation const* GetTypeClassInformation(); \
	GDAPI virtual GD::ClassInformation const* GetClassInformation() const override { return GetTypeClassInformation(); }; \

#define GD_IMPLEMENT_CLASS_INFORMATION_FORCE_NO_TAGS(Class) \
	GDEXP GD::ClassInformation const* Class::GetTypeClassInformation() \
	{ \
		GD::ClassInformation static const TypeClassInformation(GD::Dummy<typename Class::ClassTp>{}, GD::Dummy<typename Class::SuperClassTp>{}); \
		return &TypeClassInformation; \
	} \

#define GD_IMPLEMENT_CLASS_INFORMATION(Class, ...) \
	typedef Class GD_GLUE(__ClassAtLine, __LINE__); \
	__VA_ARGS__; \
	GD_IMPLEMENT_CLASS_INFORMATION_FORCE_NO_TAGS(Class)

#define GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(Class, ...) \
	GD_IMPLEMENT_CLASS_INFORMATION(Class, GD_CLASS_NOT_CONSTRUCTIBLE_TAG, __VA_ARGS__)

GD_NAMESPACE_END

