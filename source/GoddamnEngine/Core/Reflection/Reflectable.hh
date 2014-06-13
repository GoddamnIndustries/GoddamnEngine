//////////////////////////////////////////////////////////////////////////
/// FieldInformation.hh: Common interface for field information
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#ifndef GD_CORE_REFLECTION_REFLECTABLE
#define GD_CORE_REFLECTION_REFLECTABLE

#include <GoddamnEngine/Include.hh>

GD_NAMESPACE_BEGIN

	class ITypeInformation;

	template<class ReflectableType>
	GDAPI extern ITypeInformation const* TypeOf();

	class IReflectable
	{
		GDAPI virtual ITypeInformation const* GetTypeInformation() const abstract;
	};

GD_NAMESPACE_END

#endif