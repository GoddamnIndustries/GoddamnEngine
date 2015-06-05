// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Variant.h
//! Class, that can store boxed values inside.
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	class TypeInfo;
	
	class Variant final 
	{
	public:
		GDINL TypeInfo const* GetType() const { return nullptr; }
	};

GD_NAMESPACE_END
