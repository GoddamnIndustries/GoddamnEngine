//////////////////////////////////////////////////////////////////////////
/// Utility.hh - Base utilities definition and implementations.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 29.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_UTILITY
#define GD_CORE_UTILITY

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>

GD_NAMESPACE_BEGIN

	/// Returns RValue reference on type without creating any instance.
	template<typename SignatureType>
	GDINT SignatureType&& DeclValue();

	/// Returns RValue reference to specified object.
	template<typename SignatureType> 
	GDINL typename TypeTraits::RemoveReference<SignatureType>::Type&& Move(SignatureType&& Instance)
	{
		return (static_cast<typename TypeTraits::RemoveReference<SignatureType>::Type&&>(Instance));
	}

	/// Returns an RValue reference to specified object if it is not an LValue reference.
	/// If specified object is an LValue reference, the function returns it without modifying it`s type.
	template<typename SignatureType>
	GDINL SignatureType&& Forward(typename TypeTraits::RemoveReference<SignatureType>::Type& Instance)
	{	
		return (static_cast<SignatureType&&>(Instance));
	}

	template<typename SignatureType>
	GDINL SignatureType&& Forward(typename TypeTraits::RemoveReference<SignatureType>::Type&& Instance)
	{	
		static_assert(!TypeTraits::IsLValueReference<SignatureType>::Value, "Invocation of forward function with non-LValue reference type.");
		return (static_cast<SignatureType&&>(Instance));
	}

	/// Swaps value between two objects.
	template<typename SignatureType>
    void Swap(SignatureType& First, SignatureType& Second)
	{	
		SignatureType Temporary = Move(First);
		First = Move(Second);
		Second = Move(Temporary);
	}

GD_NAMESPACE_END

#endif
