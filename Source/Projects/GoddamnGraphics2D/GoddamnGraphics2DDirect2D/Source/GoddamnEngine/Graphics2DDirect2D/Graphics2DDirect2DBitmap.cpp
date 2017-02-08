// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS2D_DIRECT2D_IMPL 1

/*!
 * @file GoddamnEngine/Core/System/Graphics/GraphicsDirect2D/GraphicsDirect2DBitmap.cpp
 * File contains Implementation for Direct2D Implementation of the graphics interface: bitmaps.
 */
#include <GoddamnEngine/Core/System/Graphics2D/Graphics2DDirect2D/Graphics2DDirect2D.h>
#include <GoddamnEngine/Core/System/Graphics/GraphicsDirect3D1x/GraphicsDirect3D1xFormatWIC.h>
#include <GoddamnEngine/Core/System/Graphics/GraphicsDirect3D1x/GraphicsDirect3D1xFormatDXGI.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D bitmaps.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphics2DDirect2DBitmap);

	// ------------------------------------------------------------------------------------------
	//! Creates a 2D font with specified parameters.
	//! @param gfxBitmapCreationInfo Pointer to the bitmap creation information.
	GDAPI IGraphics2DDirect2DBitmap::IGraphics2DDirect2DBitmap(IGraphics2DBitmapCreationInfo const* const gfxBitmapCreationInfo)
		: IGraphics2DBitmap(gfxBitmapCreationInfo)
	{
		// First of all, we will try to upload or bitmap into Direct2D directly.
		// This may possibly fail, that we will try again with different pixel format.
		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd756686(v=vs.85).aspx
		HRESULT const Hr = GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->CreateBitmap(D2D1::SizeU(gfxBitmapCreationInfo->BitmapWidth, gfxBitmapCreationInfo->BitmapHeight)
			, gfxBitmapCreationInfo->BitmapData
			, gfxBitmapCreationInfo->BitmapWidth * static_cast<UINT>(IGraphicsFormatSizeof(gfxBitmapCreationInfo->BitmapPixelFormat))
			, D2D1::BitmapProperties(D2D1::PixelFormat(IGraphicsDirect3D1xFormatConvertDXGI(gfxBitmapCreationInfo->BitmapPixelFormat))), &D2DBitmap.p);
		if (Hr == WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT)
		{
			// It seems that this pixel format is not supported on current platform (or by Direct2D itself).
			// We will try to convert the bitmap data into appropriate format.

			// Loading our image into the WIC..
			CComPtr<IWICBitmap> D2DWICBitmap;
			ThrowIfFailed(GD_IGRAPHICS2D_DIRECT2D->D2DWICImagingFactory->CreateBitmapFromMemory(gfxBitmapCreationInfo->BitmapWidth, gfxBitmapCreationInfo->BitmapHeight 
				, IGraphicsDirect3D1xFormatConvertWIC(gfxBitmapCreationInfo->BitmapPixelFormat)
				, static_cast<UINT>(IGraphicsFormatSizeof(gfxBitmapCreationInfo->BitmapPixelFormat) * gfxBitmapCreationInfo->BitmapWidth)
				, static_cast<UINT>(gfxBitmapCreationInfo->BitmapDataLength), const_cast<BYTE*>(static_cast<BYTE const*>(gfxBitmapCreationInfo->BitmapData))
				, &D2DWICBitmap.p));

			// Converting our bitmap..
			CComPtr<IWICFormatConverter> D2DWICBitmapConverter;
			ThrowIfFailed(GD_IGRAPHICS2D_DIRECT2D->D2DWICImagingFactory->CreateFormatConverter(&D2DWICBitmapConverter.p));
			ThrowIfFailed(D2DWICBitmapConverter->Initialize(D2DWICBitmap, GUID_WICPixelFormat32bppPBGRA
				, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut));

			// And finally, creating our bitmap from the converted pixel data..
			ThrowIfFailed(GD_IGRAPHICS2D_DIRECT2D->D2DRenderTarget->CreateBitmapFromWicBitmap(D2DWICBitmapConverter, nullptr, &D2DBitmap.p));
		}
		else
		{
			ThrowIfFailed(Hr);
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a 2D font with specified parameters.
	//! @param gfxBitmapPtr Pointer for output.
	//! @param gfxBitmapCreationInfo Pointer to the bitmap creation information.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphics2DDirect2DWithBitmaps::GfxImm_BitmapCreate(IGraphics2DBitmap** gfxBitmapPtr
		, IGraphics2DBitmapCreationInfo const* const gfxBitmapCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_BitmapCreate(gfxBitmapPtr, gfxBitmapCreationInfo)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxBitmapPtr = GD_NEW(IGraphics2DDirect2DBitmap, gfxBitmapCreationInfo);
		return IResult::Ok;
	}

GD_NAMESPACE_END
