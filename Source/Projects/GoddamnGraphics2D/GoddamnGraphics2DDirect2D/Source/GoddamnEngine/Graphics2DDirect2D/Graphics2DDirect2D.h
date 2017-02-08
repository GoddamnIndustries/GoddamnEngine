// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Graphics2D/Graphics2DDirect2D/GraphicsDirect2D.h
 * File contains export interface for Direct2D Implementation of the 2D graphics interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Graphics2D/Graphics2D.h>
#include <GoddamnEngine/Core/System/Graphics/GraphicsWindows.h>

#if !defined(GD_IGRAPHICS2D_DIRECT2D_IMPL)
#	define GD_IGRAPHICS2D_DIRECT2D_IMPL GD_FALSE
#endif	// if !defined(GD_IGRAPHICS2D_DIRECT2D_IMPL)

#include <atlbase.h>
#if GD_IGRAPHICS2D_DIRECT2D_IMPL
#	undef DrawText
#	include <d2d1_1.h>
#	include <dwrite.h>
#	include <WinCodec.h>
#endif	// if GD_IGRAPHICS2D_DIRECT2D_IMPL

GD_NAMESPACE_BEGIN

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL_BASE(IGraphics2DDirect2D, IGraphics2D);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename IGraphics2DBaseRendererType>
	uinterface IGraphics2DDirect2DRenderer : public IGraphics2DBaseRendererType
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>, IGraphics2DBaseRendererType);
	public:
		D2D1_RECT_F								D2DBaseRendererRect = {};
		D2D1_MATRIX_3X2_F						D2DBaseRendererTransform = {};
		CComPtr<interface ID2D1Layer>           D2DBaseRendererLayer;
		CComPtr<interface ID2D1SolidColorBrush> D2DBaseRendererBrush;
	protected:
		friend class Allocator;
		template<typename IGraphics2DBaseRendererTypeCreationInfo>
		GDINL explicit IGraphics2DDirect2DRenderer(IGraphics2DBaseRendererTypeCreationInfo const* const gfxBaseRendererTypeCreationInfo);
		GDINL virtual ~IGraphics2DDirect2DRenderer() {}
	public:
		GDINL virtual void Imm_SetTransform(GeomRect const gfxRendererRect, Float32 const gfxRendererRotation = 0.0f) override;
#if GD_FALSE
		GDINL virtual void Imm_SetDepth(UInt8 const gfxRendererDepth) override final;
#endif	// if GD_FALSE
		GDINL virtual void Imm_SetColor(GeomColor const gfxColor) override;
		GDINL virtual void Imm_Render() override;
	};	// uinterface IGraphics2DDirect2DRenderer<T>

	/*template<typename IGraphics2DBaseRendererType>
	struct IClassConstrutor<IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>> { GDINL static IClassConstrutorProc Get() { return nullptr; } };
	template<typename IGraphics2DBaseRendererType>
	GD_IMPLEMENT_CLASS_INFORMATION_FORCE_NO_TAGS(IGraphics2DDirect2DRenderer<IGraphics2DBaseRendererType>);*/

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D bitmaps.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphics2DDirect2DBitmap final : public IGraphics2DBitmap
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DDirect2DBitmap, IGraphics2DBitmap);
	public:
		CComPtr<uinterface ID2D1Bitmap> D2DBitmap;
	private:
		friend class Allocator;
		friend uinterface IGraphics2DDirect2DWithBitmaps;
		GDAPI explicit IGraphics2DDirect2DBitmap(IGraphics2DBitmapCreationInfo const* const gfxBitmapCreationInfo);
		GDAPI virtual ~IGraphics2DDirect2DBitmap() {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphics2DDirect2DBitmap
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DDirect2DWithBitmaps, IGraphics2DDirect2D)
	{
	public:
		GDAPI virtual IResult GfxImm_BitmapCreate(IGraphics2DBitmap** gfxBitmapPtr
			, IGraphics2DBitmapCreationInfo const* const gfxBitmapCreationInfo) override final;
	};	// uinterface IGraphics2DDirect2DWithBitmaps


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Rect rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphics2DDirect2DRectRenderer final : public IGraphics2DDirect2DRenderer<IGraphics2DRectRenderer>
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DDirect2DRectRenderer, IGraphics2DRectRenderer);
	public:
		IPtr<IGraphics2DDirect2DBitmap const>   D2DBitmap;
		FLOAT									D2DBitmapOpacity = 1.0f;
		CComPtr<interface ID2D1SolidColorBrush> D2DBitmapColorSolidBrush;
		CComPtr<interface ID2D1SolidColorBrush> D2DBitmapColorOverlayBrush;
	private:
		friend class Allocator;
		friend uinterface IGraphics2DDirect2DWithRectRenderers;
		GDAPI explicit IGraphics2DDirect2DRectRenderer(IGraphics2DRectRendererCreationInfo const* const gfxRectRendererCreationInfo);
		GDAPI virtual ~IGraphics2DDirect2DRectRenderer() {}
	public:
		GDINL virtual void Imm_SetColor(GeomColor const gfxColor) override final;
		GDAPI virtual void Imm_Render() override final;
	};	// uinterface IGraphics2DDirect2DRectRenderer
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DDirect2DWithRectRenderers, IGraphics2DDirect2D)
	{
	public:
		GDAPI virtual IResult GfxImm_RectRendererCreate(IGraphics2DRectRenderer** gfx2DRectRendererPtr
			, IGraphics2DRectRendererCreationInfo const* const gfxRectRendererCreationInfo) override final;
	};	// uinterface IGraphics2DDirect2DWithRectRenderers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Fonts.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uinterface IGraphics2DDirect2DFont final : public IGraphics2DFont
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DDirect2DFont, IGraphics2DFont);
	public:
		BaseString<WCHAR>                         D2DFontName;
		CComPtr<uinterface IDWriteFontCollection> D2DFontCollection;
		CComPtr<uinterface IDWriteFont>			  D2DFont;
	private:
		friend class Allocator;
		friend uinterface IGraphics2DDirect2DWithFonts;
		GDAPI explicit IGraphics2DDirect2DFont(IGraphics2DFontCreationInfo const* const gfxFontCreationInfo);
		GDAPI virtual ~IGraphics2DDirect2DFont() {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphics2DDirect2DFont
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DDirect2DWithFonts, IGraphics2DDirect2D)
	{
	public:
		GDAPI virtual IResult GfxImm_FontCreate(IGraphics2DFont** gfxFontPtr
			, IGraphics2DFontCreationInfo const* const gfxFontCreationInfo) override final;
	};	// uinterface IGraphics2DDirect2DWithTextRenderers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Text rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_IGRAPHICS2D_DIRECT2D_IMPL
	DWRITE_FONT_WEIGHT static const IGraphics2DDirect2DTextRendererWeightsTable[IGRAPHICS2D_TEXT_RENDERER_WEIGHTS_COUNT] = {
		/* IGRAPHICS2D_TEXT_RENDERER_WEIGHT_REGULAR */ DWRITE_FONT_WEIGHT_REGULAR,
		/* IGRAPHICS2D_TEXT_RENDERER_WEIGHT_THIN    */ DWRITE_FONT_WEIGHT_LIGHT,
		/* IGRAPHICS2D_TEXT_RENDERER_WEIGHT_BOLD    */ DWRITE_FONT_WEIGHT_BOLD,
		/* IGRAPHICS2D_TEXT_RENDERER_WEIGHT_UNKNOWN */ DWRITE_FONT_WEIGHT(0),
	};
#endif	// if GD_IGRAPHICS2D_DIRECT2D_IMPL

#if GD_IGRAPHICS2D_DIRECT2D_IMPL
	DWRITE_FONT_STYLE static const IGraphics2DDirect2DTextRendererStylesTable[IGRAPHICS2D_TEXT_RENDERER_STYLES_COUNT] = {
		/* IGRAPHICS2D_TEXT_RENDERER_STYLE_NORMAL  */ DWRITE_FONT_STYLE_NORMAL,
		/* IGRAPHICS2D_TEXT_RENDERER_STYLE_OBLIQUE */ DWRITE_FONT_STYLE_OBLIQUE,
		/* IGRAPHICS2D_TEXT_RENDERER_STYLE_ITALIC  */ DWRITE_FONT_STYLE_ITALIC,
		/* IGRAPHICS2D_TEXT_RENDERER_STYLE_UNKNOWN */ DWRITE_FONT_STYLE(0),
	};
#endif	// if GD_IGRAPHICS2D_DIRECT2D_IMPL

#if GD_IGRAPHICS2D_DIRECT2D_IMPL
	DWRITE_TEXT_ALIGNMENT static const IGraphics2DDirect2DTextRendererHorizontalAlignment[IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENTS_COUNT] = {
		/* IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_LEFT      */ DWRITE_TEXT_ALIGNMENT_LEADING,
		/* IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_CENTER    */ DWRITE_TEXT_ALIGNMENT_CENTER,
		/* IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_RIGHT     */ DWRITE_TEXT_ALIGNMENT_TRAILING,
		/* IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_JUSTIFGDD */ DWRITE_TEXT_ALIGNMENT_JUSTIFIED,
		/* IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_UNKNOWN   */ DWRITE_TEXT_ALIGNMENT(0),
	};
#endif	// if GD_IGRAPHICS2D_DIRECT2D_IMPL

	uinterface IGraphics2DDirect2DTextRenderer final : public IGraphics2DDirect2DRenderer<IGraphics2DTextRenderer>
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DDirect2DTextRenderer, IGraphics2DTextRenderer);
	public:
		CComPtr<interface IDWriteTextFormat> D2DTextRendererTextFormat;
		BaseString<WCHAR>                    D2DTextRendererText;
	private:
		friend class Allocator;
		friend uinterface IGraphics2DDirect2DWithTextRenderers;
		GDAPI explicit IGraphics2DDirect2DTextRenderer(IGraphics2DTextRendererCreationInfo const* const gfxTextRendererCreationInfo);
		GDAPI virtual ~IGraphics2DDirect2DTextRenderer() {}
	public:
		GDAPI virtual void Imm_Render() override final;
	};	// uinterface IGraphics2DDirect2DTextRenderer
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DDirect2DWithTextRenderers, IGraphics2DDirect2D)
	{
	public:
		GDAPI virtual IResult GfxImm_TextRendererCreate(IGraphics2DTextRenderer** gfx2DTextRendererPtr
			, IGraphics2DTextRendererCreationInfo const* const gfxTextRendererCreationInfo) override final;
	};	// uinterface IGraphics2DDirect2DWithTextRenderers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DDirect2D, IGraphics2DDirect2D)
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DDirect2D, IGraphics2D);
		interface WriteFontFileEnumerator;

	public:
		// DriectWrite interfaces.
		CComPtr<interface IDWriteFactory>        D2DDWFactory;
		CComPtr<interface IDWriteFontCollection> D2DDWFontCollectionDefault;

		// WIC interfaces.
		CComPtr<interface IWICImagingFactory>    D2DWICImagingFactory;

		// Direct2D interfaces.
		CComPtr<interface ID2D1Factory>          D2DFactory;
		CComPtr<interface ID2D1HwndRenderTarget> D2DRenderTarget;
		CComPtr<interface ID2D1SolidColorBrush>  D2DDefaultBrush;

	public:
		GDAPI virtual IResult OnRuntimePostInitialize() override final;
		GDAPI virtual void OnRuntimePreUpdate() override;
		GDAPI virtual void OnRuntimePostUpdate() override;
};	// uinterface IGraphics2DDirect2D

	//! Reference to the global Direct2D interface.
#if GD_IGRAPHICS2D_DIRECT2D_IMPL
#	define GD_IGRAPHICS2D_DIRECT2D static_cast<GD::IGraphics2DDirect2D*>(GD::Graphics2D.GetPointer())
#endif	// if GD_IGRAPHICS2D_DIRECT2D_IMPL

GD_NAMESPACE_END

#if GD_IGRAPHICS2D_DIRECT2D_IMPL
#	include <GoddamnEngine/Core/System/Graphics2D/Graphics2DDirect2D/Grpahics2DDirect2DRenderer.inl>
#endif	// if GD_IGRAPHICS2D_DIRECT2D_IMPL
