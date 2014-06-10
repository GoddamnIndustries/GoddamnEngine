//////////////////////////////////////////////////////////////////////////
/// Vector3Fast.h: Fast floating-point three-dimensional vector class.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.06.2014  - Created by James Jhuighuy,
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_MATH_VECTOR3_FAST
#define GD_CORE_MATH_VECTOR3_FAST

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Float32x4Intrinsics/Float32x4Intrinsics.h>

#if (defined(GD_HAS_FLOAT32X4INTRINSICS))
#	include<GoddamnEngine/Core/Math/Vector4.h>
#	define GD_VECTOR3_FAST_IMPL Float32x4 // Float32 intrinsics instructions set.
#else	// if (defined(GD_SUPPORT_SSE))
#	include<GoddamnEngine/Core/Math/Vector3.h>
#	define GD_VECTOR3_FAST_IMPL Float32x3 GD_ALIGN(16) // No SSE instructions set.
#endif	// if (defined(GD_SUPPORT_SSE))

GD_NAMESPACE_BEGIN

	/// Fastest possible Vector3t<Float32> implementation on current platform.
	/// If possible, uses SSE4 instructions to optimize computing.
	/// @note This is 16-bytes aligned type. (@c sizeof(Vector3FloatFast) == 16)
	typedef GD_VECTOR3_FAST_IMPL Vector3Fast;

GD_NAMESPACE_END

#undef GD_VECTOR3_FLOAT32FAST_IMPL
#endif
