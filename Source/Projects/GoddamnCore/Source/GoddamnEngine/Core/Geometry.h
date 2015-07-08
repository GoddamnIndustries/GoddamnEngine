// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Geometry.h
//! File contains basic geometry structures.
#pragma once
#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	template<typename Tp>
	struct GeomPointTp final
	{
		Tp X, Y;
	};	// struct GeomPointTp final
	using GeomPoint = GeomPointTp<Int32>;

	template<typename Tp>
	struct GeomSizeTp final
	{
		Tp Width, Height;
	};	// struct GeomSizeTp final
	using GeomSize = GeomSizeTp<Int32>;

	template<typename Tp>
	struct GeomResolutionTp final
	{
		Tp Width, Height;
		Tp VsyncNumerator, VsyncDenominator;
	};	// struct GeomResolutionTp final
	using GeomResolution = GeomResolutionTp<Int32>;

	template<typename Tp>
	struct GeomRectTp final
	{
		Tp X, Y;
		Tp Width, Height;
	};	// struct GeomRectTp final
	using GeomRect = GeomRectTp<Int32>;

GD_NAMESPACE_END
