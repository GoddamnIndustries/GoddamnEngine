// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Reflection.h
//! Common interface for RTTI/Introspection/Reflection.
#pragma once

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Misc/CMemory.h>

#include <GoddamnEngine/Core/Math/Vector2.h>
#include <GoddamnEngine/Core/Math/Vector3.h>
#include <GoddamnEngine/Core/Math/Vector4.h>
#include <GoddamnEngine/Core/Math/Quaternion.h>
//#include <GoddamnEngine/Core/Math/Matrix3x3.h>
//#include <GoddamnEngine/Core/Math/Matrix3x4.h>
#include <GoddamnEngine/Core/Math/Matrix4x4.h>

// ------------------------------------------------------------------------------------------
// Objective-C++ style syntax enhancements.
#define $comma__			,
#define $expand__(tokens)	tokens
#define $class				class
#define $as					$comma__
#define $returns			$comma__
#define $extends			$comma__
#define $end				};
#define $self				Self
#define $value				Value
#define $typeof(...)		(GD::ReflectionTypeResolver<__VA_ARGS__>::Value)
#define $dimensionof(...)	(GD::ReflectionVectorDimensionResolver<__VA_ARGS__>::Value)
#define $empty_property		(reinterpret_cast<GD::ReflectionProperty const*>(SizeTpMax))
#define $empty_method		(reinterpret_cast<GD::ReflectionMethod const*>(SizeTpMax))
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! @brief Defines a reflectable class.
//! @param ClassTags Tags of the class. Is optional.
//! @param ClassThisType Type name of a class.
//! @param ClassSuperType Type name of a super class of this one.
#define $interface_tagged(...) $expand__($interface__(__VA_ARGS__))
#define $interface(...) $expand__($interface__(, __VA_ARGS__))
#define $interface__(ClassFlags, ClassThisType, ClassSuperType, ...) \
$class ClassThisType : public ClassSuperType \
{ \
private: \
	GD::SizeTp static const __ReflIndexBase = __COUNTER__; \
	typedef ClassThisType This; \
	typedef ClassSuperType Super; \
	template<GD::SizeTp PropertyIndex> struct __ReflDataCollector final \
	{ \
		GDINL static void CollectProperty(GD::ReflectionProperty const*& NextProperty) { GD_NOT_USED(NextProperty); }\
		GDINL static void CollectMethod(GD::ReflectionMethod const*& NextMethod) { GD_NOT_USED(NextMethod); } \
	};	/* struct __ReflDataCollector */ \
public: \
	GDINL virtual GD::ReflectionClass const* GetClass() const override { return This::GetClassType(); } \
	GDINL static  GD::ReflectionClass const* GetClassType() \
	{ \
		GD::ReflectionClass static  ClassThisType##Class((#ClassFlags), (#ClassThisType), ClassSuperType::GetClassType()); \
		GD::ReflectionClass static* ClassThisType##ClassRegistered = GD::ReflectionClass::RegisterOrFind(&ClassThisType##Class); \
		if ((ClassThisType##ClassRegistered->ClassFirstProperty == $empty_property) || (ClassThisType##ClassRegistered->ClassFirstMethod == $empty_method)) \
		{ \
			if (ClassThisType##ClassRegistered->ClassFirstProperty == $empty_property) ClassThisType##ClassRegistered->ClassFirstProperty = nullptr; \
			if (ClassThisType##ClassRegistered->ClassFirstMethod   == $empty_method)   ClassThisType##ClassRegistered->ClassFirstMethod = nullptr; \
			__ReflDataCollector<0>::CollectProperty(ClassThisType##Class.ClassFirstProperty); \
			__ReflDataCollector<0>::CollectMethod(ClassThisType##Class.ClassFirstMethod);\
		}; \
		return ClassThisType##ClassRegistered; \
	} 
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! @brief Defines information about a property with generated getters/setters.
//! @param PropertyTags Tags of the property. Is optional.
//! @param PropertyName Name of the property.
//! @param PropertyType Type of the property.
#define $synthesize_property_raw_tagged(...) $expand__($synthesize_property_raw__(__VA_ARGS__))
#define $synthesize_property_raw(...) $expand__($synthesize_property_raw__(, __VA_ARGS__))
#define $synthesize_property_raw__(PropertyTags, PropertyName, PropertyType) \
public: \
	/** @todo Add some checks for existence of the accessors/mutators. */ \
	GDINL PropertyType Get##PropertyName() const { return __Refl##PropertyName##GetterSetter::Get(this); } \
	GDINL void Set##PropertyName(PropertyType const Value) { __Refl##PropertyName##GetterSetter::Set(this, Value); } \
private: \
	struct __Refl##PropertyName##GetterSetter final : GD::ReflectionGenericGetterSetter<__Refl##PropertyName##GetterSetter, This, PropertyType> \
	{ \
		typedef PropertyType PropertyTp; 
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! @brief Defines information about a reflectable property with generated getters/setters.
//! @param PropertyTags Tags of the property. Is optional.
//! @param PropertyName Name of the property.
//! @param PropertyType Type of the property.
#define $synthesize_property_tagged(...) $expand__($synthesize_property__(__VA_ARGS__))
#define $synthesize_property(...) $expand__($synthesize_property__(, __VA_ARGS__))
#define $synthesize_property__(PropertyTags, PropertyName, PropertyType) \
private: \
	GD::SizeTp static const __Refl##PropertyName##Index = __COUNTER__ - __ReflIndexBase - 1; \
	template<> struct __ReflDataCollector<__Refl##PropertyName##Index> final \
	{ \
		typedef __ReflDataCollector<__Refl##PropertyName##Index + 1> NextCollectorTp; \
		GDINL static void CollectMethod(GD::ReflectionMethod const*& NextMethod) { NextCollectorTp::CollectMethod(NextMethod); } \
		GDINL static void CollectProperty(GD::ReflectionProperty const*& NextProperty) \
		{ \
			GD::ReflectionProperty static PropertyName##ReflectionProperty((#PropertyTags), $typeof(PropertyType), GetClassType(), (#PropertyName) \
				, $dimensionof(PropertyType), &__Refl##PropertyName##GetterSetter::GetRaw, &__Refl##PropertyName##GetterSetter::SetRaw); \
			NextProperty = &PropertyName##ReflectionProperty; \
			NextCollectorTp::CollectProperty(PropertyName##ReflectionProperty.PropertyNext); \
		} \
	};	/* struct __ReflDataCollector */\
	$synthesize_property_raw(PropertyName, PropertyType) 
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! @brief Defines a Getter/Setter for a property.
//! @{
#define $getter GDINL static PropertyTp Get(This const* const Self) 
#define $setter GDINL static void Set(This* const Self, PropertyTp const& Value) 
//! @}
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! @brief Synthesize a generic Getter/Setter function for a property.
//! @param PropertyAssociatedField Field to associate with.
//! @{
#define $synthesize_generic_getter(PropertyAssociatedField) { return Self->PropertyAssociatedField; }
#define $synthesize_generic_setter(PropertyAssociatedField) { Self->PropertyAssociatedField = value; }
//! @}
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
//! @brief Defines information about a reflectable property with trivial generated getters/setters.
//! @param PropertyTags Tags of the property. Is optional.
//! @param PropertyName Name of the property.
//! @param PropertyAssociatedField Field to associate with.
//! @{
#define $synthesize_property_trivial_raw_tagged(PropertyTags, PropertyName, PropertyAssociatedField) \
	$synthesize_property_raw_tagged(PropertyTags, PropertyName $as decltype(PropertyAssociatedField)) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
		$setter $synthesize_generic_setter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial_raw(PropertyName, PropertyAssociatedField) \
	$synthesize_property_raw(PropertyName $as decltype(PropertyAssociatedField) const) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
		$setter $synthesize_generic_setter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial_tagged(PropertyTags, PropertyName, PropertyAssociatedField) \
	$synthesize_property_tagged(PropertyTags, PropertyName $as decltype(PropertyAssociatedField)) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
		$setter $synthesize_generic_setter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial(PropertyName, PropertyAssociatedField) \
	$synthesize_property(PropertyName $as decltype(PropertyAssociatedField)&) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
		$setter $synthesize_generic_setter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial_readonly_raw_tagged(PropertyTags, PropertyName, PropertyAssociatedField) \
	$synthesize_property_raw_tagged(PropertyTags, PropertyName $as decltype(PropertyAssociatedField)) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial_readonly_raw(PropertyName, PropertyAssociatedField) \
	$synthesize_property_raw(PropertyName $as decltype(PropertyAssociatedField)) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial_readonly_tagged(PropertyTags, PropertyName, PropertyAssociatedField) \
	$synthesize_property_tagged(PropertyTags, PropertyName $as decltype(PropertyAssociatedField)) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
	$end
#define $synthesize_property_trivial_readonly(PropertyName, PropertyAssociatedField) \
	$synthesize_property(PropertyName $as decltype(PropertyAssociatedField)) \
		$getter $synthesize_generic_getter(PropertyAssociatedField); \
	$end
//! @}
// ------------------------------------------------------------------------------------------

#if 0

// ------------------------------------------------------------------------------------------
//! @todo Refactor this macro. Probably enhance syntax.
//! @brief Defines information about a reflectable method.
//! @param METHOD_EXPORT_SPECIFICATION Export specifications of a method.
//! @param MethodName Name of the method.
//! @param MethodType Type of the method.
#define $method(...) $expand__($method__(, __VA_ARGS__))
#define $method__(MethodFlags, METHOD_EXPORT_SPECIFICATION, MethodName, MethodType) \
private: \
	GD::SizeTp static const __Refl##MethodName##Index = __COUNTER__ - __ReflIndexBase - 1; \
	template<> struct __ReflDataCollector<__Refl##MethodName##Index> final \
	{ \
		typedef __ReflDataCollector<__Refl##MethodName##Index + 1> NextCollectorTp; \
		GDINL static void CollectProperty(GD::ReflectionMethod*& NextProperty) { NextCollectorTp::CollectProperty(NextProperty); } \
		GDINL static void CollectMethod(GD::ReflectionMethod*& NextMethod) \
		{ \
			GD::ReflectionMethod static MethodName##ReflectionMethod((#MethodFlags), $typeof(MethodType), $typeof(This), (#MethodName)); \
			NextMethod = &MethodName##ReflectionMethod; \
			NextCollectorTp::CollectMethod(const_cast<GD::ReflectionMethod*&>(MethodName##ReflectionMethod.Next)); \
		} \
	};	/* struct __ReflDataCollector */\
public: \
	METHOD_EXPORT_SPECIFICATION MethodType MethodName 
// ------------------------------------------------------------------------------------------

#endif	// if 0

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;

	template<typename ObjectTp>
	struct RefPtr;

	struct ReflectionField;
	struct ReflectionProperty;
	struct ReflectionMethod;
	struct ReflectionClass;
	struct ReflectionEnum;

	//! @brief Base type for all reflectable types.
	struct IReflectable	: IUncopiable
	{
		//! @brief Returns pointer to type information of this object.
		//! @returns Pointer to type information of this object.
		//! @{
		GDAPI static  ReflectionClass const* GetClassType()
		{
			return nullptr;
		}
		GDAPI virtual ReflectionClass const* GetClass() const
		{
			return nullptr;
		}
		//! @}
	};	// class IReflectable
	
	//! @brief Helper structure for properties mechanics. Do not use manually.
	template<typename GetterSetterTp, typename ReflectableTp, typename PropertyTp>
	struct ReflectionGenericGetterSetter
	{
		GDINL static PropertyTp Get(ReflectableTp const* const Self) { GD_NOT_USED(Self); GD_NOT_SUPPORTED(); }
		GDINL static bool GetRaw(CHandle const Instance, Handle const OutputValuePtr)
		{
			*reinterpret_cast<PropertyTp*>(OutputValuePtr) = GetterSetterTp::Get(reinterpret_cast<ReflectableTp const*>(Instance));
			return true;
		}
		GDINL static void Set(ReflectableTp* const Self, PropertyTp const Value) { GD_NOT_USED(Self); GD_NOT_USED(Value); GD_NOT_SUPPORTED(); }
		GDINL static bool SetRaw(Handle const Instance, CHandle const InputValuePtr)
		{
			GetterSetterTp::Set(reinterpret_cast<ReflectableTp*>(Instance), *reinterpret_cast<PropertyTp const*>(InputValuePtr));
			return true;
		}
	};	// struct ReflectionGenericGetterSetter

	//! @brief Parses a C++ identifier name into human-readable representation.
	//! @param Identifier C++ identifier.
	//! @returns Human-readable representation of a C++ identifier.
	GDAPI extern String ReflectionStringToReadableName(String const& Identifier);

	// ------------------------------------------------------------------------------------------
	// Reflection Types.
	// ------------------------------------------------------------------------------------------

	//! @brief Represents identifier of supported reflectable types.
	struct ReflectionTypeTp
	{
		typedef UInt32 Type;
		static Type const TypeUndefined  = 0x00000000;
		static Type const TypeBool       = 0x00000001;
		static Type const TypeANSIChar   = 0x00000002;
		static Type const TypeWideChar   = 0x00000003;
		static Type const TypeInt8       = 0x00000004;
		static Type const TypeUInt8      = 0x00000005;
		static Type const TypeInt16      = 0x00000006;
		static Type const TypeUInt16     = 0x00000007;
		static Type const TypeInt32      = 0x00000008;
		static Type const TypeUInt32     = 0x00000009;
		static Type const TypeInt64      = 0x0000000A;
		static Type const TypeUInt64     = 0x0000000B;
		static Type const TypeFloat32    = 0x0000000C;
		static Type const TypeFloat64    = 0x0000000D;
		static Type const TypeVector2    = 0x0000000E;
		static Type const TypeRange      = TypeVector2;
		static Type const TypeVector3    = 0x0000000F;
		static Type const TypeVector4    = 0x00000010;
		static Type const TypeColor      = TypeVector4;
		static Type const TypeQuaternion = TypeVector4;
		static Type const TypeMatrix3x3  = 0x00000011;
		static Type const TypeMatrix3x4  = 0x00000012;
		static Type const TypeMatrix4x4  = 0x00000013;
		static Type const TypeString     = 0x00000014;
		static Type const TypeWideString = 0x00000015;
		static Type const TypeReference  = 0x00000016;
		static Type const TypeEnum       = 0x00000017;
		static Type const TypeBlob       = 0x00000018;
	};	// struct ReflectionTypeTp

	typedef ReflectionTypeTp::Type ReflectionType;

	//! @brief Helper structure for counting vector dimensions.
	//! @tparam SomeType Type for which dimension would be resolved.
	//! @note Use the '$dimensionof' macro instead.
	//! @{
	template<typename SomeType> 
	struct ReflectionVectorDimensionResolver { SizeTp static const Value = 0; };
	template<typename ContaineeType> 
	struct ReflectionVectorDimensionResolver<Vector<ContaineeType>> { SizeTp static const Value = 1 + ReflectionVectorDimensionResolver<ContaineeType>::Value; };
	//! @}

	//! @brief Helper structure for resolving types.
	//! @tparam SomeType Type for which Type would be resolved.
	//! @note Use the '$typeof' macro instead.
	//! @{
	template<typename SomeType>
	struct ReflectionTypeResolver { ReflectionType static const Value = ReflectionTypeTp::TypeUndefined; };
	template<typename ContaineeType>	// Array unwinding...
	struct ReflectionTypeResolver<Vector<ContaineeType>> { ReflectionType static const Value = ReflectionTypeResolver<ContaineeType>::Value; };
	template<typename PointeeType>		// References unwinding...
	struct ReflectionTypeResolver<RefPtr<PointeeType>> { ReflectionType static const Value = ReflectionTypeTp::TypeReference; };
	// Trivial types:
#define GD_REFLECTION_DEFINE_TYPEOF(Tp) \
	template<> struct ReflectionTypeResolver<Tp> { ReflectionType static const Value = ReflectionTypeTp::Type##Tp; }
	template<> struct ReflectionTypeResolver<bool> { ReflectionType static const Value = ReflectionTypeTp::TypeBool; };
	GD_REFLECTION_DEFINE_TYPEOF( Int8  );
	GD_REFLECTION_DEFINE_TYPEOF(UInt8  );
	GD_REFLECTION_DEFINE_TYPEOF( Int16 );
	GD_REFLECTION_DEFINE_TYPEOF(UInt16 );
	GD_REFLECTION_DEFINE_TYPEOF( Int32 );
	GD_REFLECTION_DEFINE_TYPEOF(UInt32 );
	GD_REFLECTION_DEFINE_TYPEOF( Int64 );
	GD_REFLECTION_DEFINE_TYPEOF(UInt64 );
	GD_REFLECTION_DEFINE_TYPEOF(Float32);
	GD_REFLECTION_DEFINE_TYPEOF(Float64);
	GD_REFLECTION_DEFINE_TYPEOF(Vector2);
	GD_REFLECTION_DEFINE_TYPEOF(Vector3);
	GD_REFLECTION_DEFINE_TYPEOF(Vector4);
//	GD_REFLECTION_DEFINE_TYPEOF(Matrix3x3);
//	GD_REFLECTION_DEFINE_TYPEOF(Matrix3x4);
	GD_REFLECTION_DEFINE_TYPEOF(Matrix4x4);
	GD_REFLECTION_DEFINE_TYPEOF(String);
	GD_REFLECTION_DEFINE_TYPEOF(WideString);
#undef GD_REFLECTION_DEFINE_TYPEOF
	//! @}

	// ------------------------------------------------------------------------------------------
	// Reflection Tags.
	// ------------------------------------------------------------------------------------------

	struct ReflectionTagsTp
	{
		typedef UInt32 Type;
		static Type const TagsNone                  = 0;
		static Type const TagsHideInEditor          = GD_BIT(0);
		static Type const TagsNotSerializable       = GD_BIT(1);
		static Type const TagsHasOverriddenName     = GD_BIT(2);
		static Type const TagsHasOverriddenCategory = GD_BIT(3);
	};	// struct ReflectionTagsTp

	typedef ReflectionTagsTp::Type ReflectionTags;

	GDAPI extern ReflectionTags ReflectionTagsFromString(CStr const CString);

	// ------------------------------------------------------------------------------------------
	// ReflectionClass class.
	// ------------------------------------------------------------------------------------------

	struct ReflectionClass final
	{
	public:
		ReflectionTags				ClassTags;
		String						ClassName;
		String						ClassHumanReadableName;
		ReflectionClass    const*	ClassSuper;
		ReflectionProperty const*	ClassFirstProperty;
		ReflectionMethod   const*	ClassFirstMethod;

	public:

		//! @brief Initializes a reflectable class.
		//! @param ClassTagsString Tags of a class.
		//! @param ClassName Name of a class.
		//! @param ClassSuper Super class of a class.
		GDAPI ReflectionClass(CStr const ClassTagsString, CStr const ClassName, ReflectionClass const* ClassSuper);

		//! @brief Deinitializes a reflectable class.
		GDAPI ~ReflectionClass();

		//! @brief Checks of
		GDAPI static ReflectionClass* RegisterOrFind(ReflectionClass* const RegistrationCondidate) { return RegistrationCondidate; }
	};	// struct ReflectionClass

	// ------------------------------------------------------------------------------------------
	// ReflectionProperty class.
	// ------------------------------------------------------------------------------------------

	typedef bool(*ReflectionPropertyGetter)(CHandle const Instance, Handle const OutputValuePtr);
	typedef bool(*ReflectionPropertySetter)(Handle const Instance, CHandle const InputValuePtr);

	struct ReflectionProperty final
	{
	public:
		ReflectionTags            PropertyTags;						//!< @brief Tags of this property.
		ReflectionType            PropertyType;						//!< @brief Type of this property.
		ReflectionClass    const* PropertyOwnerClass;				//!< @brief Owner class of this property.
		ReflectionProperty const* PropertyNext;						//!< @brief Pointer to next property in the owner class.
		String                    PropertyName;						//!< @brief Name of this property.
		String                    PropertyHumanReadableName;		//!< @brief Human readable name of this property.
		String                    PropertyCategory;					//!< @brief Category of this property.
		UInt32                    PropertyVectorDimensions;			//!< @brief Vector dimension of this property.
		ReflectionPropertyGetter  PropertyGetter;					//!< @brief Pointer to generated getter of the property.
		ReflectionPropertySetter  PropertySetter;					//!< @brief Pointer to generated setter of the property.
	
	public:

		// ------------------------------------------------------------------------------------------
		// Constructor/Destructor.
		// ------------------------------------------------------------------------------------------

		//! @brief Initializes a reflectable property.
		//! @param PropertyTagsString Tags of this property.
		//! @param PropertyType Type of this property.
		//! @param PropertyOwnerClass Owner class of this property.
		//! @param PropertyName Name of this property.
		//! @param PropertyVectorDimensions Vector dimension of this property.
		//! @param PropertyGetter Pointer to generated getter of the property.
		//! @param PropertySetter Pointer to generated setter of the property.
		GDAPI ReflectionProperty(CStr const PropertyTagsString, ReflectionType const PropertyType, ReflectionClass const* const PropertyOwnerClass, CStr const PropertyName
			, UInt32 const PropertyVectorDimensions, ReflectionPropertyGetter const PropertyGetter, ReflectionPropertySetter const PropertySetter);

		// ------------------------------------------------------------------------------------------
		// Generator/Deleter for property containers.
		// ------------------------------------------------------------------------------------------

		GDAPI Handle AllocateInstance() const;
		GDAPI void DeallocateInstance(Handle const) const;

		// ------------------------------------------------------------------------------------------
		// Generic Accessor/Mutator.
		// ------------------------------------------------------------------------------------------

		//! @brief Copies value of the property to output.
		//! @param Instance Reflectable object we are operating on.
		//! @param OutputValuePointer Here would be copied value of the member.
		//! @returns True on success.
		GDINL bool GetValueViaCopying(CHandle const Instance, Handle const OutputValuePointer) const
		{
			if (this->PropertyGetter != nullptr) {
				return this->PropertyGetter(Instance, OutputValuePointer);
			}
			return false;
		}

		//! @brief Copies value of input into member.
		//! @param Instance Reflectable object we are operating on.
		//! @param InputValuePointer New value of the member would be copied out of here.
		//! @returns True on success.
		GDINL bool SetValueViaCopying(Handle const Instance, Handle const InputValuePointer) const
		{
			if (this->PropertySetter != nullptr) {
				return this->PropertySetter(Instance, InputValuePointer);
			}
			return false;
		}

		// ------------------------------------------------------------------------------------------
		// Array Accessor/Mutator.
		// ------------------------------------------------------------------------------------------

		

	};	// struct ReflectionProperty

GD_NAMESPACE_END
