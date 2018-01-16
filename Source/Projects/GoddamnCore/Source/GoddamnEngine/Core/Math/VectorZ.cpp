// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Math/Vector.cpp
 * 2D, 3D and 4D vectors classes.
 */
#include <GoddamnEngine/Core/Math/Vector.h>

GD_NAMESPACE_BEGIN

	Vector2     const Vector2::Zero         = Vector2(+0.0f, +0.0f);	//!< The { 0, 0} constant.
	Vector2     const Vector2::One          = Vector2(+1.0f, +1.0f);	//!< The { 1, 1} constant.
	Vector2     const Vector2::Right        = Vector2(+1.0f, +0.0f);	//!< The { 1, 0} constant.
	Vector2     const Vector2::Left         = Vector2(-1.0f, +0.0f);	//!< The {-1, 0} constant.
	Vector2     const Vector2::Up           = Vector2(+0.0f, +1.0f);	//!< The { 0, 1} constant.
	Vector2     const Vector2::Down         = Vector2(+0.0f, -1.0f);	//!< The { 0,-1} constant.

	Vector3     const Vector3::Zero         = Vector3(+0.0f, +0.0f, +0.0f);	//!< The { 0, 0, 0} constant.
	Vector3     const Vector3::One          = Vector3(+1.0f, +1.0f, +1.0f);	//!< The { 1, 1, 1} constant.
	Vector3     const Vector3::Right        = Vector3(+1.0f, +0.0f, +0.0f);	//!< The { 1, 0, 0} constant.
	Vector3     const Vector3::Left         = Vector3(-1.0f, +0.0f, +0.0f);	//!< The {-1, 0, 0} constant.
	Vector3     const Vector3::Up           = Vector3(+0.0f, +1.0f, +0.0f);	//!< The { 0, 1, 0} constant.
	Vector3     const Vector3::Down         = Vector3(+0.0f, -1.0f, +0.0f);	//!< The { 0,-1, 0} constant.
	Vector3     const Vector3::Forward      = Vector3(+0.0f, +0.0f, +1.0f);	//!< The { 0, 0, 1} constant.
	Vector3     const Vector3::Backward     = Vector3(+0.0f, +0.0f, -1.0f);	//!< The { 0, 0,-1} constant.	

	Vector4     const Vector4::Zero         = Vector4(+0.0f, +0.0f, +0.0f, +0.0f);	//!< The { 0, 0, 0, 0} constant.
	Vector4     const Vector4::Identity     = Vector4(+0.0f, +0.0f, +0.0f, +1.0f);	//!< The { 0, 0, 0, 1} constant.
	Vector4     const Vector4::One          = Vector4(+1.0f, +1.0f, +1.0f, +1.0f);	//!< The { 1, 1, 1, 1} constant.
	Vector4     const Vector4::Right        = Vector4(+1.0f, +0.0f, +0.0f, +1.0f);	//!< The { 1, 0, 0, 1} constant.
	Vector4     const Vector4::Left         = Vector4(-1.0f, +0.0f, +0.0f, +1.0f);	//!< The {-1, 0, 0, 1} constant.
	Vector4     const Vector4::Up           = Vector4(+0.0f, +1.0f, +0.0f, +1.0f);	//!< The { 0, 1, 0, 1} constant.
	Vector4     const Vector4::Down         = Vector4(+0.0f, -1.0f, +0.0f, +1.0f);	//!< The { 0,-1, 0, 1} constant.
	Vector4     const Vector4::Forward      = Vector4(+0.0f, +0.0f, +1.0f, +1.0f);	//!< The { 0, 0, 1, 1} constant.
	Vector4     const Vector4::Backward     = Vector4(+0.0f, +0.0f, -1.0f, +1.0f);	//!< The { 0, 0,-1, 1} constant.

#if GD_SIMD_ENABLED || GD_DOCUMENTATION // We do not need to redefine a class for the missing SIMD instructions.

	Vector3Fast const Vector3Fast::Zero     = Vector3Fast(+0.0f, +0.0f, +0.0f);	//!< The { 0, 0, 0} constant.
	Vector3Fast const Vector3Fast::One      = Vector3Fast(+1.0f, +1.0f, +1.0f);	//!< The { 1, 1, 1} constant.
	Vector3Fast const Vector3Fast::Right    = Vector3Fast(+1.0f, +0.0f, +0.0f);	//!< The { 1, 0, 0} constant.
	Vector3Fast const Vector3Fast::Left     = Vector3Fast(-1.0f, +0.0f, +0.0f);	//!< The {-1, 0, 0} constant.
	Vector3Fast const Vector3Fast::Up       = Vector3Fast(+0.0f, +1.0f, +0.0f);	//!< The { 0, 1, 0} constant.
	Vector3Fast const Vector3Fast::Down     = Vector3Fast(+0.0f, -1.0f, +0.0f);	//!< The { 0,-1, 0} constant.
	Vector3Fast const Vector3Fast::Forward  = Vector3Fast(+0.0f, +0.0f, +1.0f);	//!< The { 0, 0, 1} constant.
	Vector3Fast const Vector3Fast::Backward = Vector3Fast(+0.0f, +0.0f, -1.0f);	//!< The { 0, 0,-1} constant.	

#endif	// if GD_SIMD_ENABLED || GD_DOCUMENTATION 

GD_NAMESPACE_END
