/// ==========================================================================================
/// ReflectionBase.h: Basic definitions for RTTI/Introspection/Reflection interface.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_OBJECT_REFLECTION_BASE
#define GD_CORE_OBJECT_REFLECTION_BASE

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	class ITypeInfo;
	class IBoolInfo;
	class INumericInfo;
	class IEnumInfo;
	class IPointerInfo;
	class IClassInfo;
	
	class IMemberInfo;
	class IFieldInfo;
	class IMethodInfo;

	/// @brief Base class for all reflectable enumerations.
	class IEnum
	{
	public:

		/// @brief Returns pointer to RTTI data of this enum.
		/// @returns Pointer to RTTI data of this enum.
		GDINT static IEnumInfo const* GetEnumInfo();

	};	// class IEnum

	/// @brief Interface to reflectable classes.
	class IClass
	{
	public:

		/// @brief Returns pointer to RTTI data of this type. 
		/// @returns Pointer to RTTI data of this type.
		GDAPI virtual IClassInfo const* GetClassInfo() const abstract;

		/// @brief Returns pointer to RTTI data of this type. 
		/// @returns Pointer to RTTI data of this type.
		GDINT IClassInfo const static* GetTypeClassInfo();

	};	// class IClass

	/// @brief Interface to reflectable structures.
	class IStruct : public IClass
	{
	};	// class IStruct

	/// @brief Base class for all reflectable containers.
	class IContainer
	{
	};	// class IContainer

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_OBJECT_REFLECTION_BASE
