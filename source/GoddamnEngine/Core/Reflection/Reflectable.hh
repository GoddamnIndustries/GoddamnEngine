//////////////////////////////////////////////////////////////////////////
/// FieldInformation.hh: Common interface for field information
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_REFLECTION_REFLECTABLE
#define GD_CORE_REFLECTION_REFLECTABLE

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.hh>
#include <GoddamnEngine/Core/Reflection/TypeInformation/TypeInformation.hh>
#include <GoddamnEngine/Core/Reflection/FieldInformation/FieldInformation.hh>
#include <GoddamnEngine/Core/Reflection/MethodInformation/MethodInformation.hh>
#include <GoddamnEngine/Core/Reflection/PropertyInformation/PropertyInformation.hh>

#define $GD_ENUMERATION(...)
#define $GD_ENUMERATION_BODY_GENERATED_CRAP(ClassName)

#define $GD_REFLECTABLE(...)
#define $GD_REFLECTABLE_BODY_GENERATED_CRAP(ClassName)

#define $GD_PROPERTY(...)
#define $GD_FUNCTION(...) 

GD_NAMESPACE_BEGIN

	class ITypeInformation;

//	template<class ReflectableType>
//	GDAPI extern ITypeInformation const* TypeOf();

	class IReflectable
	{
		GDAPI virtual ITypeInformation const* GetTypeInformation() const abstract;
	};

GD_NAMESPACE_END

#endif