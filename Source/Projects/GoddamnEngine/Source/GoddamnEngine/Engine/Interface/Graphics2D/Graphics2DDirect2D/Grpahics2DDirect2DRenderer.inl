// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS2D_DIRECT2D_IMPL 1

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsDirect2D/Graphics2DDirect2DRenderer.inl
 * File contains Implementation for Direct2D Implementation of the graphics interface: base renderers.
 */
#pragma once
#include <GoddamnEngine/Engine/Interface/Graphics2D/Graphics2DDirect2D/Graphics2DDirect2D.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Initializes the base Direct2D renderer.
	//! @param gfxBaseRendererTypeCreationInfo Pointer to the base renderer creation information.
	template<typename IGraphics2DBaseRendererType>
	template<typename IGraphics2DBaseRendererTypeCreationInfo>
	GDINL IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>::IGraphics2DDirect2DRenderer(IGraphics2DBaseRendererTypeCreationInfo const* const gfxBaseRendererTypeCreationInfo)
		: IGraphics2DBaseRendererType(gfxBaseRendererTypeCreationInfo)
		, D2DBaseRendererRect(D2D1::RectF(0.0f, 0.0f, 100.0f, 100.0f))
		, D2DBaseRendererTransform(D2D1::Matrix3x2F::Identity())
		, D2DBaseRendererLayer(nullptr), D2DBaseRendererBrush(nullptr)
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Updates the transformation parameters of the 2D renderable object.
	//! @param gfxRendererRect Rectangle into which the 2D object would be rendered.
	//! @param gfxRendererRotation Z rotation of the object in radians.
	template<typename IGraphics2DBaseRendererType>
	GDINL void IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>::Imm_SetTransform(GeomRect const gfxRendererRect
		, Float32 const gfxRendererRotation /*= 0.0f*/)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(Imm_SetTransform(gfxRendererRect, gfxRendererRotation)))
			return;
#endif	// if GD_DEBUG

		D2DBaseRendererTransform = D2D1::Matrix3x2F::Rotation(gfxRendererRotation, D2D1::Point2F(
			static_cast<Float32>(gfxRendererRect.X + 0.5f * gfxRendererRect.Width), static_cast<Float32>(gfxRendererRect.Y + 0.5f * gfxRendererRect.Height)));
		D2DBaseRendererRect = D2D1::RectF(
			static_cast<Float32>(gfxRendererRect.X), static_cast<Float32>(gfxRendererRect.Y), 
			static_cast<Float32>(gfxRendererRect.X + gfxRendererRect.Width), static_cast<Float32>(gfxRendererRect.Y + gfxRendererRect.Height));
	}

#if GD_FALSE
	// ------------------------------------------------------------------------------------------
	//! Updates the depth parameters of the 2D renderable object.
	//! @param gfxRendererDepth Depth value of the 2D object.
	template<typename IGraphics2DBaseRendererType>
	GDINL void IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>::Imm_SetDepth(UInt8 const gfxRendererDepth)
	{
	}
#endif	// if GD_FALSE

	// ------------------------------------------------------------------------------------------
	//! Updates the color parameter of the 2D renderable object.
	//! @param gfxColor Color multiplier of the 2D object.
	template<typename IGraphics2DBaseRendererType>
	GDINL void IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>::Imm_SetColor(GeomColor const gfxColor)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(Imm_SetColor(gfxColor)))
			return;
#endif	// if GD_DEBUG

		D2D1_COLOR_F const& D2DBrushColor = D2D1::ColorF(gfxColor.R, gfxColor.G, gfxColor.B, gfxColor.A);
		if (D2DBaseRendererBrush != nullptr)
		{
			// If the existing brash has the same color, there is no need to load the new one.
			D2D1_COLOR_F const& D2DExistingBrushColor = D2DBaseRendererBrush->GetColor();
			if (CMemory::Memcmp(&D2DBrushColor, &D2DExistingBrushColor, sizeof(D2DBrushColor)) != 0)
				D2DBaseRendererBrush = nullptr;
		}
		if (D2DBaseRendererBrush == nullptr)
		{
			// Is the brush does not exist, create the new one.
			ThrowIfFailed(GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->CreateSolidColorBrush(D2DBrushColor, &D2DBaseRendererBrush.p));
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Renders the 2D object.
	template<typename IGraphics2DBaseRendererType>
	GDINL void IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>::Imm_Render()
	{
		GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->SetTransform(D2DBaseRendererTransform);
	}

GD_NAMESPACE_END
