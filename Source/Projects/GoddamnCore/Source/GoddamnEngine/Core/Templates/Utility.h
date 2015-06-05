// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Utility.h
//! Base utilities definition and implementations.

#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//! Returns RValue reference on type without creating any instance.
	//! @returns RValue reference on type without creating any instance.
	template<typename SignatureType>
	GDINT SignatureType&& DeclValue();

	//! Returns RValue reference to specified object.
	//! @param Instance Some object.
	//! @returns RValue reference to specified object.
	template<typename SignatureType> 
	GDINL typename TypeTraits::RemoveReference<SignatureType>::Type&& Move(SignatureType&& Instance)
	{
		return static_cast<typename TypeTraits::RemoveReference<SignatureType>::Type&&>(Instance);
	}

	//! Returns an RValue reference to specified object if it is not an LValue reference. If specified object is an LValue reference, the function returns it without modifying it`s type.
	//! @param Instance Some object.
	//! @returns RValue reference to specified object if it is not an LValue reference.
	//! @{
	template<typename SignatureType>
	GDINL SignatureType&& Forward(typename TypeTraits::RemoveReference<SignatureType>::Type& Instance)
	{	
		return (static_cast<SignatureType&&>(Instance));
	}
	template<typename SignatureType>
	GDINL SignatureType&& Forward(typename TypeTraits::RemoveReference<SignatureType>::Type&& Instance)
	{	
		static_assert(!TypeTraits::IsLValueReference<SignatureType>::Value, "Invocation of forward function with non-LValue reference type.");
		return static_cast<SignatureType&&>(Instance);
	}
	//! @}

	//! Swaps value between two objects.
	template<typename SignatureType>
	void Swap(SignatureType& First, SignatureType& Second)
	{	
		SignatureType Temporary = Move(First);
		First = Move(Second);
		Second = Move(Temporary);
	}

GD_NAMESPACE_END
