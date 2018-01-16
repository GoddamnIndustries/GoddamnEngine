// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
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
	using GeomSize = GeomSizeTp<UInt32>;

	template<typename Tp>
	struct GeomResolutionTp final
	{
		Tp Width, Height;
		Tp VsyncNumerator, VsyncDenominator;
	};	// struct GeomResolutionTp final
	using GeomResolution = GeomResolutionTp<UInt32>;

	template<typename Tp>
	struct GeomRectTp final
	{
		Tp X, Y;
		Tp Width, Height;
	};	// struct GeomRectTp final
	using GeomRect = GeomRectTp<Int32>;

	template<typename Tp>
	struct GeomColorTp final
	{
		Tp R, G, B, A;
		GDINL Tp const* GetData() const { return &R; }
		GDINL static GeomColorTp Black() { return{ Tp(0.0f), Tp(0.0f), Tp(0.0f), Tp(0.0f), Tp(0.0f) }; }
	};	// struct GeomColorTp final
	using GeomColor = GeomColorTp<Float32>;
	using GeomColor32 = GeomColorTp<Byte>;

GD_NAMESPACE_END
