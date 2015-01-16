/// ==========================================================================================
/// FieldInformation.h: Common interface for field information
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_REFLECTION_REFLECTABLE
#define GD_CORE_REFLECTION_REFLECTABLE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/List.h>
#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.h>
#include <GoddamnEngine/Core/Reflection/TypeInformation.h>
#include <GoddamnEngine/Core/Reflection/FieldInformation.h>
#include <GoddamnEngine/Core/Reflection/MethodInformation.h>

GD_NAMESPACE_BEGIN

	/// @brief Type used as collection of member information.
	typedef LinkedList<IMemberInformation const*> MemberInformationCollection;

	/// @brief Represents a type of c collectible member information.
	enum class MemberInformationCollectionType : UInt8
	{	
		Field,
		Method
	};	// enum class MemberInformationCollectionType

	/// @brief Interface to reflectable types.
	class IReflectable
	{
	public:

		/// @brief Returns pointer to RTTI data of this type. 
		/// @returns Pointer to RTTI data of this type.
		GDAPI virtual ITypeInformation const* GetTypeInformation() const abstract;

		/// @brief Returns pointer to RTTI data of this type. 
		/// @returns Pointer to RTTI data of this type.
		GDINT ITypeInformation const static* GetClassTypeInformation();
	};	// class IReflectable

GD_NAMESPACE_END

#if (defined(GD_DOCUMENTATION) || defined(__INTELLISENSE__))
/// @brief Define this to use dummy macros for the RTTI system. 
///        Dummy macros just hide all internal stuff from the developer and documentation tool.
#	define GD_TYPEINFORMATION_DUMMY 1
#endif	// if (defined(GD_DOCUMENTATION) || defined(__INTELLISENCE__))

/// ==========================================================================================
/// Macros for getters.
/// ==========================================================================================

#if (!defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines a final overridden name getter function.
/// @param Name The name.
#define GD_DEFINE_NAME_GETTER(Name) \
	/** @brief Name getter implementation. */ \
	/** @returns A reference to the name of the member. */ \
	GDINT virtual GD::String const& GetName() const override final \
	{ \
		String static const TheName(Name); \
		return TheName; \
	} \

/// @brief Defines a final overridden name getter function.
/// @param Type Some reflectable type to collect information.
/// @param Member Type of the member (field, method, etc..).
#define GD_DEFINE_MEMBERS_GETTER(ThisType, Member) \
	/** @brief Returns collection of public methods in type. */ \
	/** @returns Collection of public methods in type. */ \
	GDINL virtual GD::Vector<GD::I##Member##Information const*> const& Get##Member##s() const override final \
	{ \
		GD_USING_NAMESPACE; \
		Vector<I##Member##Information const*> const static Members = []() { \
			/* Collecting information on the specified members. */\
			MemberInformationCollection MembersCollection; \
			ThisType::__CollectReflectionInformation<0>(MembersCollection, MemberInformationCollectionType::Member); \
			\
			/* Converting information into the vector. */ \
			Vector<I##Member##Information const*> MembersVector(MembersCollection.GetLength()); \
			for (IMemberInformation const* MemberInformation : MembersCollection) { \
				MembersVector.PushLast(static_cast<I##Member##Information const*>(MemberInformation)); \
			} \
			\
			return MembersVector; \
		}(); \
		\
		return Members;\
	} \

#endif	// if (!defined(GD_TYPEINFORMATION_DUMMY))

/// ==========================================================================================
/// The 'GD_TYPEINFORMATION_*' macros.
/// ==========================================================================================

#if (!defined(__COUNTER__))
#	error '__COUNTER__' macro is not defined. Check your compiler to support of it.
#endif	// if (!defined(__COUNTER__))

#if (defined(GD_TYPEINFORMATION_DUMMY))
/// @brief Defines some internal data for reflection algorithms.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param ExportSpecification Export specification of the static methods.
#	define GD_TYPEINFORMATION_DEFINITION_BEGIN(ThisType, BaseType, ExportSpecification)
#else	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines some internal data for reflection algorithms.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param ExportSpecification Export specification of the static methods.
#define GD_TYPEINFORMATION_DEFINITION_BEGIN(ThisType, BaseType, ExportSpecification) \
private: \
	static_assert(GD::TypeTraits::IsBase<GD::IReflectable, ThisType>::Value, "'GD_TYPEINFORMATION_DEFINITION_BEGIN' error: 'ThisType' should be derived from 'IReflectable'"); \
	static_assert(GD::TypeTraits::IsBase<BaseType, ThisType>::Value, "'GD_TYPEINFORMATION_DEFINITION_BEGIN' error: 'ThisType' should be derived from 'BaseType'"); \
	\
	/* Making this class accessible to the reflection manager. */ \
	friend class __##ThisType##TypeInformation; \
    friend GD::IReflectable* __VirtuallyContruct##ThisType(GD::IReflectable* const Parent, handle const Argument); \
	\
	/** @brief Stores a name of this type. */ \
	typedef ThisType __ThisType; \
	/** @brief Stores a name of this base type. */ \
	typedef BaseType __ThisBaseType; \
	\
	/** @brief Defines a safe workaround for start of the counter macro. */ \
	GD::size_t constexpr static __CounterStart = __COUNTER__; \
	\
private: \
	\
	/** @brief Method that collects method information. */ \
	/** @param Collection Container that would store collected member information. */ \
	/** @tparam MemberIndex Index of the member. Function recursively invokes self with incremented index. */ \
	template<size_t const MemberIndex> \
	GDINL static void __CollectReflectionInformation(GD::MemberInformationCollection& Collection, GD::MemberInformationCollectionType const CollectionType); \
	\
public: \
	\
	/** @brief Returns pointer to RTTI data of this type. */ \
	/** @returns Pointer to RTTI data of this type. */ \
	ExportSpecification GD::ITypeInformation const static* GetClassTypeInformation(); \
	\
	/** @brief Returns pointer to RTTI data of this type. */ \
	/** @returns Pointer to RTTI data of this type. */ \
	GDINL virtual GD::ITypeInformation const* GetTypeInformation() const override \
	{ \
		return __ThisType::GetClassTypeInformation(); \
	} \
public: \

#endif	// if (defined(GD_TYPEINFORMATION_DUMMY))

#if (defined(GD_TYPEINFORMATION_DUMMY))
/// @brief Defines some internal data for reflection algorithms.
#	define GD_TYPEINFORMATION_DEFINITION_END()
#else	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines some internal data for reflection algorithms.
#define GD_TYPEINFORMATION_DEFINITION_END() \
private: \
	/** @brief Defines a safe workaround for end of the counter macro. */ \
	GD::size_t constexpr static __CounerEnd = __COUNTER__ - __CounterStart - 1; \
	\
	/** @brief Method that collects method information. */ \
	/** @param Collection Container that would store collected member information. */ \
	template<> \
	GDINL static void __CollectReflectionInformation<__CounerEnd>(GD::MemberInformationCollection& Collection, GD::MemberInformationCollectionType const CollectionType) \
	{ \
		GD_NOT_USED(Collection); \
		GD_NOT_USED(CollectionType); \
		/* Nothing to do here, just a dummy for end of recursive branch... */ \
	} \
public: \

#endif	// if (defined(GD_TYPEINFORMATION_DUMMY))

// No dummy-check here due proximity of this macro.
/// @brief Defines some internal data for reflection algorithms for types without accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param ExportSpecification Export specification of the static methods.
#define GD_TYPEINFORMATION_DEFINITION(ThisType, BaseType, ExportSpecification) \
	GD_TYPEINFORMATION_DEFINITION_BEGIN(ThisType, BaseType, ExportSpecification) \
	GD_TYPEINFORMATION_DEFINITION_END() \

#if (defined(GD_TYPEINFORMATION_DUMMY))
/// @brief Defines some internal data for reflection algorithms for types without accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param ExportSpecification Export specification of the static methods.
#	define GD_TYPEINFORMATION_IMPLEMENTATION(ThisType, BaseType, ExportSpecification)
#else	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines some internal data for reflection algorithms for types without accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param ExportSpecification Export specification of the static methods.
#define GD_TYPEINFORMATION_IMPLEMENTATION(ThisType, BaseType, ExportSpecification, ...) \
	\
	static_assert(GD::TypeTraits::IsBase<GD::IReflectable, ThisType>::Value, "'GD_TYPEINFORMATION_IMPLEMENTATION' error: 'ThisType' should be derived from 'IReflectable'"); \
	static_assert(GD::TypeTraits::IsBase<BaseType, ThisType>::Value, "'GD_TYPEINFORMATION_IMPLEMENTATION' error: 'ThisType' should be derived from 'BaseType'");	\
	\
	/** @brief Type description information class. */ \
	class __##ThisType##TypeInformation final : public GD::TypeInformation<ThisType, BaseType> \
	{ \
		GD_DEFINE_NAME_GETTER(#ThisType); \
		GD_DEFINE_MEMBERS_GETTER(ThisType, Field); \
		GD_DEFINE_MEMBERS_GETTER(ThisType, Method); \
	} static const __##ThisType##TypeInformationInstance; \
	\
	/** @brief Returns pointer to RTTI data of this type. */ \
	/** @returns Pointer to RTTI data of this type. */ \
	ExportSpecification GD::ITypeInformation const* ThisType::GetClassTypeInformation() \
	{ \
		return &__##ThisType##TypeInformationInstance; \
	} \

#define GD_TYPEINFORMATION_IMPLEMENTATION_C(ThisType, BaseType, ExportSpecification, ...) GD_TYPEINFORMATION_IMPLEMENTATION(ThisType, BaseType, ExportSpecification)
#define GD_TYPEINFORMATION_IMPLEMENTATION_GC(ThisType, BaseType, ExportSpecification, ...) GD_TYPEINFORMATION_IMPLEMENTATION(ThisType, BaseType, ExportSpecification)

#endif	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// ==========================================================================================
/// The 'GD_DEFINE_*' macros.
/// ==========================================================================================

#if (!defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines a generic body for member information collector function. Internal usage only.
/// @param Name Name of the member.
/// @param Member Type of the member (field, method, etc..).
/// @param VaArgs Body of the collector function.
#define GD_DEFINE_MEMBER(Name, Member, ...) \
private: \
	/** @brief Defines a safe workaround for this member index. */ \
	GD::size_t constexpr static __##Name##MemberIndex = __COUNTER__ - __CounterStart - 1; \
	\
	/** @brief Method that collects method information. */ \
	/** @param Collection Container that would store collected member information. */ \
	template<> \
	GDINL static void __CollectReflectionInformation<__##Name##MemberIndex>(GD::MemberInformationCollection& Collection, GD::MemberInformationCollectionType const CollectionType) \
	{ \
		GD_USING_NAMESPACE; \
		__CollectReflectionInformation<__##Name##MemberIndex + 1>(Collection, CollectionType); /* Invoking the next collector. */ \
		if(CollectionType == MemberInformationCollectionType::Member) { /* Invoking the real code only on some specific collection pass. */ \
			__VA_ARGS__; /* Placing a real body of the function. */ \
		} \
	} \
public: \

#endif	// if (!defined(GD_TYPEINFORMATION_DUMMY))

#if (defined(GD_TYPEINFORMATION_DUMMY))
/// @brief Defines a information about field. Example:
/// @code
///		...
///		/// @brief Player's moving speed.
///		GD_DEFINE_FIELD(Float32, MovingSpeed) = 2.6f;
///		...
/// @endcode
/// @param Type Type of the field.
/// @param Name Name of the field.
#	define GD_DEFINE_FIELD(Type, Name) Type Name
#else	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines a information about field. Example:
/// @code
///		...
///		/// @brief Player's moving speed.
///		GD_DEFINE_FIELD(Float32, MovingSpeed) = 2.6f;
///		...
/// @endcode
/// @param Type Type of the field.
/// @param Name Name of the field.
#define GD_DEFINE_FIELD(Type, Name) \
	GD_DEFINE_MEMBER(Name, Field, \
	{ \
		/** @brief Field description implantation class. */ \
		class Name##FieldInformation final : public FieldInformation<decltype(&__ThisType::Name), &__ThisType::Name> \
		{ \
			GD_DEFINE_NAME_GETTER(#Name); \
		} static const Name##FieldInformation; \
		Collection.InsertFirst(&Name##FieldInformation); \
	}) \
	\
	/* Defining a field... */ \
	Type Name \

#endif	// if (defined(GD_TYPEINFORMATION_DUMMY))

#if (defined(GD_TYPEINFORMATION_DUMMY))
/// @brief Defines a information about method. Example:
/// @code
///		...
///		/// @brief Computes number of items in the inventory.
///		/// @param IncludeExtraItems If true, also counts extra items.
///		/// @returns Number of items in the inventory.
///		GD_DEFINE_METHOD(GDINL, UInt64, ComputeNumberOfItems)(bool const IncludeExtraItems)
///		{
///			...	// Implementation can be here...
///		}
///		... /** OR **/ ...
///		/// @brief Computes number of items in the inventory.
///		/// @param IncludeExtraItems If true, also counts extra items.
///		/// @returns Number of items in the inventory.
///		GD_DEFINE_METHOD(GDINT, UInt64, ComputeNumberOfItems)(bool const IncludeExtraItems);	// Implemented in some other place...
///		...
/// @endcode
/// @param ExportSpecification Export specification of the method definition.
/// @param Type Type of the method.
/// @param Name Name of the method.
#	define GD_DEFINE_METHOD(ExportSpecification, ReturnType, Name) ExportSpecification ReturnType Name
#else	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// @brief Defines a information about method. Example:
/// @code
///		...
///		/// @brief Computes number of items in the inventory.
///		/// @param IncludeExtraItems If true, also counts extra items.
///		/// @returns Number of items in the inventory.
///		GD_DEFINE_METHOD(GDINL, UInt64, ComputeNumberOfItems)(bool const IncludeExtraItems)
///		{
///			...	// Implementation can be here...
///		}
///		... /** OR **/ ...
///		/// @brief Computes number of items in the inventory.
///		/// @param IncludeExtraItems If true, also counts extra items.
///		/// @returns Number of items in the inventory.
///		GD_DEFINE_METHOD(GDINT, UInt64, ComputeNumberOfItems)(bool const IncludeExtraItems);	// Implemented in some other place...
///		...
/// @endcode
/// @param ExportSpecification Export specification of the method definition.
/// @param Type Type of the method.
/// @param Name Name of the method.
#define GD_DEFINE_METHOD(ExportSpecification, ReturnType, Name) \
	GD_DEFINE_MEMBER(Name, Method, \
	{ \
		/** @brief Field description implantation class. */ \
		class Name##MethodInformation final : public MethodInformation<decltype(&__ThisType::Name), &__ThisType::Name> \
		{ \
			GD_DEFINE_NAME_GETTER(#Name); \
		} static const Name##MethodInformation; \
		Collection.InsertFirst(&Name##MethodInformation); \
	}) \
	\
	/* Defining a method... */ \
	ExportSpecification ReturnType Name \

#endif	// if (defined(GD_TYPEINFORMATION_DUMMY))

/// @todo Remove this ones..
#define $GD_ENUMERATION(...)
#define $GD_ENUMERATION_BODY_GENERATED_CRAP()

#define $GD_REFLECTABLE(...)
#define $GD_REFLECTABLE_BODY_GENERATED_CRAP()

#define $GD_PROPERTY(...)
#define $GD_FUNCTION(...) 

GD_NAMESPACE_BEGIN

GD_NAMESPACE_END

#endif