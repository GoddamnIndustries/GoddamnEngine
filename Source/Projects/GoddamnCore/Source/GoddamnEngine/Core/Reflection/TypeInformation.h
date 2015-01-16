/// ==========================================================================================
/// TypeInformation.h: Common interface for type information.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_REFLECTION_TYPEINFORMATION
#define GD_CORE_REFLECTION_TYPEINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Reflection/MemberInformation.h>

GD_NAMESPACE_BEGIN

	class IReflectable;
	class IFieldInformation;
	class IMethodInformation;
	class Assembly;

	/// @brief Represents type declarations for type parameters.
	class ITypeInformation : public IMemberInformation
	{
	private:
		Assembly const* HostAssembly = nullptr;

	protected:

		/// @brief Initializes a type information.
		GDAPI ITypeInformation();

		/// @brief Deinitializes a type information.
		GDAPI virtual ~ITypeInformation();

	public:

		GDAPI ITypeInformation const static* FindType(String const& Name);

		/// @brief Returns assembly in which this type is defined.
		/// @returns Assembly in which this type is defined.
		GDINL Assembly const* GetAssembly() const
		{
			return this->HostAssembly;
		}

		/// @brief Returns size of instance of this type
		/// @returns Size of instance of this type
		GDAPI virtual size_t GetSize() const abstract;

		/// @brief Returns type information for base type of this class
		/// @returns Type information for base type of this class
		GDAPI virtual ITypeInformation const* GetBaseType() const abstract;

		/// @brief Returns collection of public fields in type.
		/// @returns Collection of public fields in type.
		GDAPI virtual Vector<IFieldInformation const*> const& GetFields() const abstract;

		/// @brief Returns collection of public methods in type.
		/// @returns Collection of public methods in type.
		GDAPI virtual Vector<IMethodInformation const*> const& GetMethods() const abstract;

	//	/// @brief Creates instance of this type.
	//	/// @returns Newly created instance of the object.
	//	GDAPI virtual IReflectable* CreateInstance(...) const abstract;
	};	// enum MemberAttributes

	template<typename ThisType, typename BaseType/*, typename = typename EnableIf<TypeTraits::IsBase<IReflectable, ThisType>>::Type*/>
	class TypeInformation : public ITypeInformation
	{
		/// @brief Returns size of instance of this type
		/// @returns Size of instance of this type
		GDAPI virtual size_t GetSize() const override final
		{
			return sizeof(ThisType);
		}

		/// @brief Returns type information for base type of this class
		/// @returns Type information for base type of this class
		GDAPI virtual ITypeInformation const* GetBaseType() const override final
		{
			return TypeOf<BaseType>();
		}
	};	// class TypeInformation
		
GD_NAMESPACE_END

#endif	// ifndef GD_CORE_REFLECTION_TYPEINFORMATION

#if 0

#ifndef GD_CORE_OBJECT_TYPEINFORMATION
#define GD_CORE_OBJECT_TYPEINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>

#define ITypeInformation ITypeInformation

GD_NAMESPACE_BEGIN

	class Object;
	class ITypeInformation;

	typedef Object* (*TypeVirtualConstructor)(Object* const parent, handle const argument);
	typedef UInt16 TypeInformationFlags;

	enum TypeInformationFlagsEnumeration : TypeInformationFlags
	{
		GD_TYPEINFORMATION_FLAGS_IS_FINAL		= GD_BIT(0),	///< This type is final
		GD_TYPEINFORMATION_FLAGS_IS_EXTERNAL	= GD_BIT(0),	///< This type was imported from DLL
		GD_TYPEINFORMATION_FLAGS_UNKNOWN		= (0xFFFF),	///< Unknown flags of type information
		GD_TYPEINFORMATION_FLAGS_NONE			= (0)	
	};

	struct IReflectable final
	{
		GD_CLASS_UNASSIGNABLE(IReflectable);
		GD_CLASS_UNCOPIABLE(IReflectable);

	public:
		GDINL ITypeInformation static const* GetClassTypeInformation() { return nullptr; }
		GDINL ITypeInformation const* GetTypeInformation() { return nullptr; }
	};

	class TypeInformationClassInfo
	{
		GD_CLASS_UNASSIGNABLE(TypeInformationClassInfo);

	public:
		String const Name;							///< Name of this type. It is independent for C++`s RTTI system.
		size_t const Size;							///< Size of this type as it is in 'sizeof(MyClass)'
		TypeVirtualConstructor VirtualConstructor;	///< Pointer to function that creates element of this type.
		ITypeInformation const* const BaseType;		///< Pointer to type information of base class of this object
		TypeInformationFlags Flags;					///< Flags describing this type

		GDINL TypeInformationClassInfo(String const& name, size_t const size,
			ITypeInformation const* const baseType,
			TypeVirtualConstructor const virtualConstructor,
			TypeInformationFlags const flags = GD_TYPEINFORMATION_FLAGS_NONE
		) :
			Name(name), Size(size), 
			VirtualConstructor(virtualConstructor), 
			BaseType(baseType), Flags(flags)
		{ }

		GDINL TypeInformationClassInfo(TypeInformationClassInfo const& typeInformationClassInfo) :
			Name(typeInformationClassInfo.Name), Size(typeInformationClassInfo.Size), 
			VirtualConstructor(typeInformationClassInfo.VirtualConstructor), 
			BaseType(typeInformationClassInfo.BaseType),
			Flags(typeInformationClassInfo.Flags)
		{ }
	};

	/// @brief RTTI custom Implementation
	/// Runtime Type Information (or RTTI) provides information about types in runtime.
	/// For example, we can receive pointer like 'Object*' and obtain what type of this   
	/// pointer really is. Also we can search for types be their names in runtime and
	/// creates dynamic instances of classes. 
	/// To use engine`s internal RTTI system for your classes:
	/// @code
	///		// Header file:
	///		class MyObject : MyObjectDerivedType
	///		{
	///		private:
	///			GD_TYPEINFORMATION_DEFINITION(MyObject, MyObjectDerivedType, MY_EXPORT_SPECIFICATION);
	///			...
	///		};
	///
	///		// Source file
	///		//	* IF this class is not abstract and no virtual constructor overloading is required 
	///		GD_TYPEINFORMATION_IMPLEMENTATION(MyObject, MyObjectDerivedType, MY_EXPORT_SPECIFICATION);
	///		//	* OR if this class is abstract or virtual constructor overloading is required 
	///		GD_TYPEINFORMATION_IMPLEMENTATION_C(MyObject, MyObjectDerivedType, MY_EXPORT_SPECIFICATION, 
	///			&MyObjectVirtualCtor);
	/// @endcode
	class ITypeInformation : public TypeInformationClassInfo
	{
		GD_CLASS_UNASSIGNABLE(ITypeInformation);
		GD_CLASS_UNCOPIABLE(ITypeInformation);

	private:
		GDAPI static ITypeInformation const* lastRegisteredType;

	public:
		/// Pointer to type information registered previously in RTTI system.
		ITypeInformation const* const PreviousType;	

		/// Returns pointer to last type information, registered in RTTI system
		GDAPI static const ITypeInformation* GetLastRegistered();
		
		/// Returns pointer to type that was found by name or nullptr if nothing found
		GDAPI static ITypeInformation const* FindType(String const& typeName);

		/// COnstructs type information in base on type`s name, size, base type and virtual constructor
		GDAPI ITypeInformation(TypeInformationClassInfo const& typeInformationClassInfo);

		/// Checks if this type information is derived from base type
		GDAPI bool IsDerivedFrom(ITypeInformation const* const typeInformation) const;
	};

GD_NAMESPACE_END

#endif

#endif	// if 0
