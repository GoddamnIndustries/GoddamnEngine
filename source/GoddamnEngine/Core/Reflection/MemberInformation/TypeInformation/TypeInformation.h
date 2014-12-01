//#ifndef GD_CORE_REFLECTION_TYPEINFORMATION
//#define GD_CORE_REFLECTION_TYPEINFORMATION
//
//#include <GoddamnEngine/Include.h>
//#include <GoddamnEngine/Core/Text/String/String.h>
//
//#include <GoddamnEngine/Core/Reflection/MemberInformation.h>
//
//#pragma region GD_TYPEINFORMATION_DEFINITION
//#define GD_TYPEINFORMATION_DEFINITION(ThisType, ...) public: typedef ThisType __ThisType;									\
//	static_assert(TypeTraits::IsBaseType<IReflectable, ThisType>::Value, "Specified type should inherit from IReflectable");\
//	GDAPI static  ITypeInformation const* GetClassTypeInformation();														\
//	GDINL virtual ITypeInformation const* GetTypeInformation() const { return ThisType::GetClassTypeInformation(); }
//#pragma endregion // GD_TYPEINFORMATION_DEFINITION
//
//#pragma region GD_TYPEINFORMATION_IMPLEMENTATION
//#	define GD_TYPEINFORMATION_IMPLEMENT_SIZE(ThisType)						virtual size_t GetSize() const { return sizeof(ThisType); }
//#	define GD_TYPEINFORMATION_IMPLEMENT_BASE_TYPE(ThisType, BaseType)		virtual ITypeInformation const* GetBaseType() const { return BaseType::GetClassTypeInformation(); }
//#	define GD_TYPEINFORMATION_IMPLEMENT_BASE_TYPE_DEFAULT(ThisType)			virtual ITypeInformation const* GetBaseType() const { return nullptr; }
//#	define GD_TYPEINFORMATION_IMPLEMENT_ASSEMBLY(ThisType, Asm)				virtual Assembly const* GetAssembly() const { return (Asm); }
//#	define GD_TYPEINFORMATION_IMPLEMENT_ASSEMBLY_EXECUTING(ThisType)		virtual Assembly const* GetAssembly() const { return Assembly::GetExecutingAssembly(); }
//#	define GD_TYPEINFORMATION_IMPLEMENT_ASSEMBLY_CORE(ThisType)				virtual Assembly const* GetAssembly() const { return Assembly::GetCoreAssembly(); }
//#	define GD_TYPEINFORMATION_IMPLEMENT_ASSEMBLY_DEFAULT(ThisType)			virtual Assembly const* GetAssembly() const { return nullptr; }
//
//#	define GD_TYPEINFORMATION_IMPLEMENT_INSTANCE_CREATOR(ThisType, Creator)	virtual IReflectable* CreateInstance(chandle const Argument) const { return /*reinterpret_cast<IReflectable*>(Creator(Argument))*/nullptr; }
//#	define GD_TYPEINFORMATION_IMPLEMENT_INSTANCE_CREATOR_DEFAULT(ThisType)	virtual IReflectable* CreateInstance(chandle const Argument) const { \
//	static_assert(!TypeTraits::IsAbstractType<ThisType>::Value, "Default constructor is not allowed for abstract types");						 \
//	return reinterpret_cast<IReflectable*>(new ThisType());																						 \
//}
//
//#	define GD_TYPEINFORMATION_IMPLEMENTATION_BEGIN(ThisType) ITypeInformation const* ThisType::GetClassTypeInformation() {			\
//																static class ThisType##TypeInfo final : public ITypeInformation { /*\*/
//#	define GD_TYPEINFORMATION_IMPLEMENTATION_END(ThisType)		} ThisType##TypeInfoInstance;										\
//																return &ThisType##TypeInfoInstance;									\
//															 }
//// Basic type information implementation
//#define GD_TYPEINFORMATION_IMPLEMENTATION_C(ThisType, BaseType, Exp, Ctor, ...)	\
//	GD_TYPEINFORMATION_IMPLEMENTATION_BEGIN(ThisType)							\
//		GD_MEMBERINFORMATION_IMPLEMENT_NAME(ThisType)							\
//		GD_MEMBERINFORMATION_IMPLEMENT_ATTRIBUTES_DEFAULT(ThisType)				\
//		GD_TYPEINFORMATION_IMPLEMENT_SIZE(ThisType)								\
//		GD_TYPEINFORMATION_IMPLEMENT_BASE_TYPE(ThisType, BaseType)				\
//		GD_TYPEINFORMATION_IMPLEMENT_ASSEMBLY_DEFAULT(ThisType)					\
//		GD_TYPEINFORMATION_IMPLEMENT_INSTANCE_CREATOR(ThisType, Ctor)			\
//	GD_TYPEINFORMATION_IMPLEMENTATION_END(ThisType)
//#define GD_TYPEINFORMATION_IMPLEMENTATION(ThisType, BaseType, ...)				\
//	GD_TYPEINFORMATION_IMPLEMENTATION_BEGIN(ThisType)							\
//		GD_MEMBERINFORMATION_IMPLEMENT_NAME(ThisType)							\
//		GD_MEMBERINFORMATION_IMPLEMENT_ATTRIBUTES_DEFAULT(ThisType)				\
//		GD_TYPEINFORMATION_IMPLEMENT_SIZE(ThisType)								\
//		GD_TYPEINFORMATION_IMPLEMENT_BASE_TYPE(ThisType, BaseType)				\
//		GD_TYPEINFORMATION_IMPLEMENT_ASSEMBLY_DEFAULT(ThisType)					\
//		GD_TYPEINFORMATION_IMPLEMENT_INSTANCE_CREATOR_DEFAULT(ThisType)			\
//	GD_TYPEINFORMATION_IMPLEMENTATION_END(ThisType)
//
//#pragma endregion // GD_TYPEINFORMATION_IMPLEMENTATION
//
//GD_NAMESPACE_BEGIN
//
//	class IReflectable;
//	class IFieldInformation;
//	class IPropertyInformation;
//	class IMethodInformation;
//	class Assembly;
//
//	/// Represents type specific attributes
//	enum TypeAttributes : UInt64
//	{
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_POD		 = GD_BIT(8),	///< This type is Plain-Old-Data structure
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_CLASS    = GD_BIT(7),	///< This type is class
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_ATOMIC	 = GD_BIT(5),	///< This type is core atomic
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_STRUCT	 = GD_BIT(6),	///< This type is structure
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_SEALED	 = GD_BIT(3),	///< This type does not allows to inherit from this type
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_NESTED	 = GD_BIT(4),	///< This type is declared in another type
//		GD_MEMBER_ATTRIBUTE_TYPE_IS_ABSTRACT = GD_BIT(9),	///< This type contains abstract methods
//	};
//
//	/// Represents type declarations for type parameters.
//	class ITypeInformation : public IMemberInformation
//	{
//		/// Returns assembly in which this type is defined.
//		GDAPI virtual Assembly const* GetAssembly() const abstract;
//
//		/// Returns size of instance of this type
//		GDAPI virtual size_t GetSize() const abstract;
//
//		/// Returns type information for base type of this class
//		GDAPI virtual ITypeInformation const* GetBaseType() const abstract;
//
//		// Useless for now
//		// /// Returns collection of template parameters located in type.
//		// GDAPI virtual Vector<ITypeInformation const*> const& GetTemplateParamters() const abstract;
//
//		/// Returns collection of public fields in type.
//		//GDAPI virtual Vector<IFieldInformation const*> const& GetFields() const abstract;
//
//		/// Returns collection of public properties in type.
//		//GDAPI virtual Vector<IPropertyInformation const*> const& GetProperties() const abstract;
//
//		/// Returns collection of public methods in type.
//		//GDAPI virtual Vector<IMethodInformation const*> const& GetMethods() const abstract;
//
//		/// Creates instance of this type.
//		GDAPI virtual IReflectable* CreateInstance(chandle const Argument) const abstract;
//	};	// enum MemberAttributes
//
//	/// Special class that represents all types that does not have base classes
//	struct NoBaseClass final
//	{
//		GDINL static  ITypeInformation const* GetClassTypeInformation() { return nullptr; }
//	};	// class NoBaseClass final
//
//GD_NAMESPACE_END
//
//#endif

#ifndef GD_CORE_OBJECT_TYPEINFORMATION
#define GD_CORE_OBJECT_TYPEINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Text/String/String.h>

#define TypeInformation ITypeInformation

/// ==========================================================================================
#pragma region GD_TYPEINFORMATION_DEFINITION
#define GD_TYPEINFORMATION_DEFINITION(Type, BaseType, ExpSpec)												\
	static_assert((TypeTraits::IsBaseType<Object, Type>::Value),											\
		"'GD_TYPEINFORMATION_DEFINITION' error: 'Type' should be derived from 'Object'");					\
	static_assert(((TypeTraits::IsBaseType<BaseType, Type>::Value)											\
		|| (TypeTraits::IsSameType<BaseType, NoBaseClass>::Value)),											\
		"'GD_TYPEINFORMATION_DEFINITION' error: 'Type' should be derived from 'BaseType'");					\
public:																										\
	typedef Type	 __ThisType;																			\
	typedef BaseType __ThisBaseType;																		\
private:																									\
    friend Object* __VirtualConstructor_##Type(Object* const parent, handle const argument);            	\
public:																										\
	ExpSpec static TypeInformation const* GetClassTypeInformation();										\
	GDINL virtual TypeInformation const* GetTypeInformation() const	{ return Type::GetClassTypeInformation(); }
#pragma endregion
/// ==========================================================================================

/// ==========================================================================================
#pragma region GD_TYPEINFORMATION_IMPLEMENTATION_GC
#define GD_TYPEINFORMATION_IMPLEMENTATION_GC(Type, BaseType, ExpSpec, TypeInformationClass, Ctor)			\
	static_assert(((TypeTraits::IsBaseType<BaseType, Type>::Value)											\
				|| (TypeTraits::IsSameType<BaseType, NoBaseClass>::Value)),									\
				  "'GD_TYPEINFORMATION_DEFINITION_GC' error: 'Type' should be derived from 'BaseType'");	\
	static TypeInformationClass const __typeInformation_##Type(TypeInformationClassInfo(#Type, sizeof(Type),\
		BaseType::GetClassTypeInformation(), Ctor));														\
	TypeInformation const* Type::GetClassTypeInformation() { return &__typeInformation_##Type; }
#pragma endregion
/// ==========================================================================================

/// ==========================================================================================
#pragma region GD_TYPEINFORMATION_IMPLEMENTATION_C
#define GD_TYPEINFORMATION_IMPLEMENTATION_C(Type, BaseType, ExpSpec, Ctor)									\
	GD_TYPEINFORMATION_IMPLEMENTATION_GC(Type, BaseType, ExpSpec, TypeInformation, Ctor)
#pragma endregion
/// ==========================================================================================

/// ==========================================================================================
#pragma region GD_TYPEINFORMATION_IMPLEMENTATION_G
#define GD_TYPEINFORMATION_IMPLEMENTATION_G(Type, BaseType, ExpSpec, TypeInformationClass, ...)				\
	static_assert((!TypeTraits::IsAbstractType<Type>::Value),												\
		"'GD_SERIALIZABLE_IMPLEMENTATION' error: sepcified type can not be abstract. "						\
		"Use 'GD_SERIALIZABLE_IMPLEMENTATION_C' to specify virtual constructor.");							\
	GDINL Object* __VirtualConstructor_##Type(Object* const Parent, handle const Argument) {			    \
		GD_UNUSED(Parent); GD_UNUSED(Argument); return new Type(__VA_ARGS__); }								\
	GD_TYPEINFORMATION_IMPLEMENTATION_GC(Type, BaseType, ExpSpec, TypeInformationClass,						\
		(TypeVirtualConstructor)(&__VirtualConstructor_##Type))
#pragma endregion
/// ==========================================================================================

/// ==========================================================================================
#pragma region GD_TYPEINFORMATION_IMPLEMENTATION
#define GD_TYPEINFORMATION_IMPLEMENTATION(Type, BaseType, ExpSpec, ...)										\
	static_assert((!TypeTraits::IsAbstractType<Type>::Value),												\
		"'GD_SERIALIZABLE_IMPLEMENTATION' error: sepcified type can not be abstract. "						\
		"Use 'GD_SERIALIZABLE_IMPLEMENTATION_C' to specify virtual constructor.");							\
	GDINL Object* __VirtualConstructor_##Type(Object* const Parent, handle const Argument) {		    	\
		GD_UNUSED(Parent); GD_UNUSED(Argument); return new Type(__VA_ARGS__); }								\
	GD_TYPEINFORMATION_IMPLEMENTATION_C(Type, BaseType, ExpSpec,											\
		(TypeVirtualConstructor)(&__VirtualConstructor_##Type))
#pragma endregion
/// ==========================================================================================

GD_NAMESPACE_BEGIN

	class Object;
	class TypeInformation;

	typedef Object* (*TypeVirtualConstructor)(Object* const parent, handle const argument);
	typedef UInt16 TypeInformationFlags;

	enum TypeInformationFlagsEnumeration : TypeInformationFlags
	{
		GD_TYPEINFORMATION_FLAGS_IS_FINAL		= GD_BIT(0),	///< This type is final
		GD_TYPEINFORMATION_FLAGS_IS_EXTERNAL	= GD_BIT(0),	///< This type was imported from DLL
		GD_TYPEINFORMATION_FLAGS_UNKNOWN		= (0xFFFF),	///< Unknown flags of type information
		GD_TYPEINFORMATION_FLAGS_NONE			= (0)	
	};

	struct NoBaseClass final
	{
		GD_CLASS_UNASSIGNABLE(NoBaseClass);
		GD_CLASS_UNCOPIABLE(NoBaseClass);

	public:
		GDINL TypeInformation static const* GetClassTypeInformation() { return nullptr; }
		GDINL TypeInformation const* GetTypeInformation() { return nullptr; }
	};

	class TypeInformationClassInfo
	{
		GD_CLASS_UNASSIGNABLE(TypeInformationClassInfo);

	public:
		String const Name;							///< Name of this type. It is independent for C++`s RTTI system.
		size_t const Size;							///< Size of this type as it is in 'sizeof(MyClass)'
		TypeVirtualConstructor VirtualConstructor;	///< Pointer to function that creates element of this type.
		TypeInformation const* const BaseType;		///< Pointer to type information of base class of this object
		TypeInformationFlags Flags;					///< Flags describing this type

		GDINL TypeInformationClassInfo(String const& name, size_t const size,
			TypeInformation const* const baseType,
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
	class TypeInformation : public TypeInformationClassInfo
	{
		GD_CLASS_UNASSIGNABLE(TypeInformation);
		GD_CLASS_UNCOPIABLE(TypeInformation);

	private:
		GDAPI static TypeInformation const* lastRegisteredType;

	public:
		/// Pointer to type information registered previously in RTTI system.
		TypeInformation const* const PreviousType;	

		/// Returns pointer to last type information, registered in RTTI system
		GDAPI static const TypeInformation* GetLastRegistered();
		
		/// Returns pointer to type that was found by name or nullptr if nothing found
		GDAPI static TypeInformation const* FindType(String const& typeName);

		/// COnstructs type information in base on type`s name, size, base type and virtual constructor
		GDAPI TypeInformation(TypeInformationClassInfo const& typeInformationClassInfo);

		/// Checks if this type information is derived from base type
		GDAPI bool IsDerivedFrom(TypeInformation const* const typeInformation) const;
	};

GD_NAMESPACE_END

#endif
