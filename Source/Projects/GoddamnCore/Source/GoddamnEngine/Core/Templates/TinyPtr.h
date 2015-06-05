// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Tiny.h
//! Smart pointer class that can store tiny types inside itself without heap.

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//template<typename PointeeTp>
	//class TinyPtr;

	//template<typename PointeeTp, typename = typename EnableIf<sizeof(PointeeTp) < sizeof(Handle)>::Type>
	//class TinyPtr<PointeeTp>
	//{
	//private:
	//	union
	//	{
	//		PointeeTp TinyValue;
	//		UInt8 TinyValuePadding[sizeof(Handle)];
	//	};



	//};	// class TinyPtr 

GD_NAMESPACE_END
