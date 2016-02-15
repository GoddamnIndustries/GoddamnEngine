// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS2D_DIRECT2D_IMPL 1

/*!
 * @file GoddamnEngine/Core/System/Graphics/GraphicsDirect2D/Graphics2DDirect2DTextRenderer.cpp
 * File contains Implementation for Direct2D Implementation of the graphics interface: string renderers.
 */
#include <GoddamnEngine/Core/System/Graphics2D/Graphics2DDirect2D/Graphics2DDirect2D.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Text rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DDirect2DTextRenderer);

	// ------------------------------------------------------------------------------------------
	//! Creates a 2D text renderer with specified parameters.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	GDAPI IGraphics2DDirect2DTextRenderer::IGraphics2DDirect2DTextRenderer(IGraphics2DTextRendererCreationInfo const* const gfxTextRendererCreationInfo)
		: IGraphics2DDirect2DRenderer<IGraphics2DTextRenderer>(gfxTextRendererCreationInfo)
	{
		// We need to create a format for our text...
		auto const gfx2DFont = static_cast<IGraphics2DDirect2DFont const*>(gfxTextRendererCreationInfo->TextRendererFont);
		ThrowIfFailed(GD_IGRAPHICS2D_DIRECT2D->D2DDWFactory->CreateTextFormat(gfx2DFont->D2DFontName.CStr(), gfx2DFont->D2DFontCollection
			, IGraphics2DDirect2DTextRendererWeightsTable[gfxTextRendererCreationInfo->TextRendererWeight]
			, IGraphics2DDirect2DTextRendererStylesTable [gfxTextRendererCreationInfo->TextRendererStyle]
			, DWRITE_FONT_STRETCH_NORMAL, gfxTextRendererCreationInfo->TextRendererSize, L"" // empty locale
			, &D2DTextRendererTextFormat.p));

		// Setting up the text alignments...
		ThrowIfFailed(D2DTextRendererTextFormat->SetTextAlignment(IGraphics2DDirect2DTextRendererHorizontalAlignment[gfxTextRendererCreationInfo->TextRendererHorizontalAlignment]));
		if (gfxTextRendererCreationInfo->TextRendererVerticalAlignment != IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_TOP)
		{
			//! @todo Implement vertical text alignment.
			GD_NOT_IMPLEMENTED();
		}

		// And finally we need to prepare our string data..
		if (gfxTextRendererCreationInfo->TextRendererIsWideString)
		{
			D2DTextRendererText = gfxTextRendererCreationInfo->TextRendererDataWide;
		}
		else
		{
			//! @todo Implement transfer here..
			GD_NOT_IMPLEMENTED();
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Renders the 2D object.
	GDAPI void IGraphics2DDirect2DTextRenderer::Imm_Render()
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(Imm_Render(gfxRendererRect, gfxRendererRotation, gfxRendererDepth)))
			return;
#endif	// if GD_DEBUG

		IGraphics2DDirect2DRenderer<IGraphics2DTextRenderer>::Imm_Render();

		if (gfxTextRendererCreationInfo.TextRendererVerticalAlignment != IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_TOP)
		{
			// Direct2D has not support for vertical data alignment. So we have to do this manually..
			//! @todo Implement vertical text alignment..
			GD_NOT_IMPLEMENTED();
		}
		GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->DrawText(D2DTextRendererText.CStr()
			, static_cast<UINT32>(D2DTextRendererText.GetLength()), D2DTextRendererTextFormat
			, D2DBaseRendererRect, D2DBaseRendererBrush != nullptr ? D2DBaseRendererBrush : GD_IGRAPHICS2D_DIRECT2D->D2DDefaultBrush);
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a 2D text renderer with specified parameters.
	//! @param gfxSamplerStatePtr Pointer for output.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphics2DDirect2DWithTextRenderers::GfxImm_TextRendererCreate(IGraphics2DTextRenderer** gfx2DTextRendererPtr
		, IGraphics2DTextRendererCreationInfo const* const gfxTextRendererCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_TextRendererCreate(gfx2DTextRendererPtr, gfxTextRendererCreationInfo)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfx2DTextRendererPtr = GD_NEW(IGraphics2DDirect2DTextRenderer, gfxTextRendererCreationInfo);
		return IResult::Ok;
	}

GD_NAMESPACE_END
