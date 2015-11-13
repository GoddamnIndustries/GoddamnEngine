// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS2D_DIRECT2D_IMPL 1

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsDirect2D/Graphics2DDirect2DRectRenderer.cpp
 * File contains Implementation for Direct2D Implementation of the graphics interface: rectangle renderers.
 */
#include <GoddamnEngine/Engine/Interface/Graphics2D/Graphics2DDirect2D/Graphics2DDirect2D.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Rectangle rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DDirect2DRectRenderer);

	// ------------------------------------------------------------------------------------------
	//! Creates a 2D Rectangle renderer with specified parameters.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	GDAPI IGraphics2DDirect2DRectRenderer::IGraphics2DDirect2DRectRenderer(IGraphics2DRectRendererCreationInfo const* const gfxRectRendererCreationInfo)
		: IGraphics2DDirect2DRenderer<IGraphics2DRectRenderer>(gfxRectRendererCreationInfo)
		, D2DBitmap(object_cast<IGraphics2DDirect2DBitmap const*>(gfxRectRendererCreationInfo->RectBitmap))
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Updates the color parameter of the 2D renderable object.
	//! @param gfxColor Color multiplier of the 2D object.
	GDINL void IGraphics2DDirect2DRectRenderer::Imm_SetColor(GeomColor const gfxColor)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(Imm_SetColor(gfxColor)))
			return;
#endif	// if GD_DEBUG

		D2DBitmapOpacity = FLOAT(gfxColor.A);
		D2D1::ColorF D2DBrushColor(gfxColor.R, gfxColor.G, gfxColor.B, gfxColor.A);
		if (D2DBitmapColorSolidBrush != nullptr)
		{
			auto const D2DExistingBrushColor = D2DBaseRendererBrush->GetColor();
			if (CMemory::Memcmp(&D2DBrushColor, &D2DExistingBrushColor, sizeof(D2DBrushColor)) != 0)
			{
				// We have the different color of our bitmap. We need to recreate the brushes.
				D2DBitmapColorSolidBrush = nullptr;
				D2DBitmapColorOverlayBrush = nullptr;
			}
		}

		// Is the brush does not exist, create the new one.
		if (D2DBitmapColorSolidBrush == nullptr)
		{
			GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->CreateSolidColorBrush(D2DBrushColor, &D2DBaseRendererBrush.p);
		}
		if (D2DBitmapColorOverlayBrush == nullptr)
		{
			D2DBrushColor.a = 1.0f - (D2DBrushColor.r + D2DBrushColor.g + D2DBrushColor.b) / 3.0f;
			if (D2DBrushColor.a > 0.0f)	// No need to render the overlay for white color.
				GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->CreateSolidColorBrush(D2DBrushColor, &D2DBitmapColorOverlayBrush.p);
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Renders the 2D object.
	GDAPI void IGraphics2DDirect2DRectRenderer::Imm_Render()
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(Imm_Render(gfxRendererRect, gfxRendererRotation, gfxRendererDepth)))
			return;
#endif	// if GD_DEBUG

		IGraphics2DDirect2DRenderer<IGraphics2DRectRenderer>::Imm_Render();
		if (D2DBitmap != nullptr)
		{
			// The bitmap was specified, we will render it.
			GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->DrawBitmap(D2DBitmap->D2DBitmap, D2DBaseRendererRect, D2DBitmapOpacity);
			if (D2DBitmapColorOverlayBrush != nullptr)
			{
				// Direct2D has weak support for the color modifications. So we are going to use the dirty hack:
				// drawing the semi-transparent rectangle on top of the our bitmap. 
				// http://stackoverflow.com/a/8624342 - read for more info.
				GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->FillRectangle(D2DBaseRendererRect, D2DBitmapColorOverlayBrush);
			}
		}
		else
		{
			// Now bitmap was specified. We can only draw the solid rectangle.
			GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->FillRectangle(D2DBaseRendererRect
				, D2DBitmapColorSolidBrush != nullptr ? D2DBitmapColorSolidBrush : GD_IGRAPHICS2D_DIRECT2D->D2DDefaultBrush);
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a 2D Rectangle renderer with specified parameters.
	//! @param gfxSamplerStatePtr Pointer for output.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphics2DDirect2DWithRectRenderers::GfxImm_RectRendererCreate(IGraphics2DRectRenderer** gfx2DRectRendererPtr
		, IGraphics2DRectRendererCreationInfo const* const gfxRectRendererCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_RectRendererCreate(gfx2DRectRendererPtr, gfxRectRendererCreationInfo)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfx2DRectRendererPtr = GD_NEW(IGraphics2DDirect2DRectRenderer, gfxRectRendererCreationInfo);
		return IResult::Ok;
	}

GD_NAMESPACE_END
