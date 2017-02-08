// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/RefPtr.h
 * Smart pointer for reference-countable classes. 
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Object/Base.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Implements casting operations for ref pointers.
	// **------------------------------------------------------------------------------------------**
	struct RefPtrCastOperation final : public TNonCreatable
	{
		/*!
		 * Casts to types using object_cast.
		 */
		template<typename TCastTo, typename TCastFrom>
		GDINL static TCastTo Cast(TCastFrom castFrom)
		{
			return object_cast<TCastTo>(castFrom);
		}
	};	// struct RefPtrCastOperation

	// **------------------------------------------------------------------------------------------**
	//! Helper class that implements a reference-counting pattern for objects.
	// **------------------------------------------------------------------------------------------**
	template<typename TObjectPointee>
	using RefPtr = SharedPtr<TObjectPointee, RefPtrCastOperation>;

GD_NAMESPACE_END
