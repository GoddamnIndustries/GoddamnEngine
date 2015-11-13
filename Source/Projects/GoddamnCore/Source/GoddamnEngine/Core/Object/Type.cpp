// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Type.cpp
//! Common interface for runtime type information and reflection.
#include <GoddamnEngine/Core/Object/Type.h>
#include <GoddamnEngine/Core/Containers/Map.h>

GD_NAMESPACE_BEGIN

	Map<HashCode, ClassInformation const*>& GetClassInformations()
	{
		Map<HashCode, ClassInformation const*> static ClassInformations;
		return ClassInformations;
	}

	GDAPI ClassInformation::ClassInformation(CStr const ClassName, ClassInformation const* const ClassSuper, IClassConstrutorProc const ClassConstrutor)
		: ClassName(ClassName), ClassSuper(ClassSuper), ClassConstrutor(ClassConstrutor)
	{
		GetClassInformations().Insert(this->ClassName.GetHashCode(), this);
	}

GD_NAMESPACE_END

#include "Object.h"
GD_NAMESPACE_BEGIN
		GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(Object);
GD_NAMESPACE_END
