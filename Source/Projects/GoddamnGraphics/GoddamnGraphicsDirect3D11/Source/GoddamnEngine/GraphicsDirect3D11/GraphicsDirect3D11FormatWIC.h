// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
* @file GoddamnEngine/Core/System/Graphics/GraphicsDirect3D11/GraphicsDirect3D11FormatWIC.h
* File contains export interface for Direct3D 11 Implementation of the graphics interface: WIC format tables.
*/
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Graphics/GraphicsFormat.h>

#if GD_IGRAPHICS_DIRECT3D1X_IMPL || GD_IGRAPHICS_DIRECT3D12_IMPL || GD_IGRAPHICS2D_DIRECT2D_IMPL
#	include <wincodec.h>
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL || GD_IGRAPHICS_DIRECT3D12_IMPL || GD_IGRAPHICS2D_DIRECT2D_IMPL

GD_NAMESPACE_BEGIN

	#if GD_IGRAPHICS_DIRECT3D1X_IMPL || GD_IGRAPHICS_DIRECT3D12_IMPL || GD_IGRAPHICS2D_DIRECT2D_IMPL
	WICPixelFormatGUID static const* IGraphicsDirect3D11FormatsTableWIC[IGRAPHICS_FORMATS_COUNT] = {
		// ..Generic formats..
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* IGRAPHICS_FORMAT_R32G32B32A32_FLOAT */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32B32A32_UINT  */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32B32A32_SINT  */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32B32_FLOAT    */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32B32_UINT     */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32B32_SINT     */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32_FLOAT       */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32_UINT        */ nullptr,
		/* IGRAPHICS_FORMAT_R32G32_SINT        */ nullptr,
		/* IGRAPHICS_FORMAT_R32_FLOAT          */ nullptr,
		/* IGRAPHICS_FORMAT_R32_UINT           */ nullptr,
		/* IGRAPHICS_FORMAT_R32_SINT           */ nullptr,
	//	/* IGRAPHICS_FORMAT_R16G16B16A16_FLOAT */ nullptr, 
		/* IGRAPHICS_FORMAT_R16G16B16A16_UNORM */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16B16A16_NORM  */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16B16A16_UINT  */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16B16A16_SINT  */ nullptr,
	//	/* IGRAPHICS_FORMAT_R16G16B16_FLOAT    */ nullptr, 
		/* IGRAPHICS_FORMAT_R16G16B16_UNORM    */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16B16_NORM     */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16B16_UINT     */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16B16_SINT     */ nullptr,
	//	/* IGRAPHICS_FORMAT_R16G16_FLOAT       */ nullptr, 
		/* IGRAPHICS_FORMAT_R16G16_UNORM       */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16_NORM        */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16_UINT        */ nullptr,
		/* IGRAPHICS_FORMAT_R16G16_SINT        */ nullptr,
	//	/* IGRAPHICS_FORMAT_R16_FLOAT          */ DXGI_FORMAT_R16_FLOAT, 
		/* IGRAPHICS_FORMAT_R16_UNORM          */ nullptr,
		/* IGRAPHICS_FORMAT_R16_NORM           */ nullptr,
		/* IGRAPHICS_FORMAT_R16_UINT           */ nullptr,
		/* IGRAPHICS_FORMAT_R16_SINT           */ nullptr,
	//	/* IGRAPHICS_FORMAT_R8G8B8A8_FLOAT     */ nullptr, 
		/* IGRAPHICS_FORMAT_R8G8B8A8_UNORM     */ &GUID_WICPixelFormat32bppRGBA,
		/* IGRAPHICS_FORMAT_R8G8B8A8_NORM      */ &GUID_WICPixelFormat32bppRGBA,
		/* IGRAPHICS_FORMAT_R8G8B8A8_UINT      */ &GUID_WICPixelFormat32bppRGBA,
		/* IGRAPHICS_FORMAT_R8G8B8A8_SINT      */ &GUID_WICPixelFormat32bppRGBA,
	//	/* IGRAPHICS_FORMAT_R8G8B8_FLOAT       */ nullptr, 
		/* IGRAPHICS_FORMAT_R8G8B8_UNORM       */ &GUID_WICPixelFormat24bppRGB,
		/* IGRAPHICS_FORMAT_R8G8B8_NORM        */ &GUID_WICPixelFormat24bppRGB,
		/* IGRAPHICS_FORMAT_R8G8B8_UINT        */ &GUID_WICPixelFormat24bppRGB,
		/* IGRAPHICS_FORMAT_R8G8B8_SINT        */ &GUID_WICPixelFormat8bppGray,
	//	/* IGRAPHICS_FORMAT_R8G8_FLOAT         */ nullptr, 
		/* IGRAPHICS_FORMAT_R8G8_UNORM         */ nullptr,
		/* IGRAPHICS_FORMAT_R8G8_NORM          */ nullptr,
		/* IGRAPHICS_FORMAT_R8G8_UINT          */ nullptr,
		/* IGRAPHICS_FORMAT_R8G8_SINT          */ nullptr,
	//	/* IGRAPHICS_FORMAT_R8_FLOAT           */ nullptr,
		/* IGRAPHICS_FORMAT_R8_UNORM           */ &GUID_WICPixelFormat8bppGray,
		/* IGRAPHICS_FORMAT_R8_NORM            */ &GUID_WICPixelFormat8bppGray,
		/* IGRAPHICS_FORMAT_R8_UINT            */ &GUID_WICPixelFormat8bppGray,
		/* IGRAPHICS_FORMAT_R8_SINT            */ &GUID_WICPixelFormat8bppGray,

		// ..Depth formats..
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* IGRAPHICS_FORMAT_DEPTH32_FLOAT      */ nullptr, 
		/* IGRAPHICS_FORMAT_DEPTH32_UINT       */ nullptr,
		/* IGRAPHICS_FORMAT_DEPTH32_INT        */ nullptr,
	//	/* IGRAPHICS_FORMAT_DEPTH16_FLOAT      */ nullptr, 
		/* IGRAPHICS_FORMAT_DEPTH16_UINT       */ nullptr,
		/* IGRAPHICS_FORMAT_DEPTH16_INT        */ nullptr,
	//	/* IGRAPHICS_FORMAT_DEPTH8_FLOAT       */ DXGI_FORMAT_D8_FLOAT, 
		/* IGRAPHICS_FORMAT_DEPTH8_UINT        */ nullptr,
		/* IGRAPHICS_FORMAT_DEPTH8_INT         */ nullptr,

		// ..Additional invalid format..
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* IGRAPHICS_FORMAT_UNKNOWN            */ nullptr,
	};
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL || GD_IGRAPHICS_DIRECT3D12_IMPL || GD_IGRAPHICS2D_DIRECT2D_IMPL

#if GD_IGRAPHICS_DIRECT3D1X_IMPL || GD_IGRAPHICS_DIRECT3D12_IMPL || GD_IGRAPHICS2D_DIRECT2D_IMPL
	// ------------------------------------------------------------------------------------------
	//! Returns the DXGI format that this format value represents.
	//! @param gfxFormat the format value to process.
	//! @returns The DXGI format that this format value represents.
	GDINL static REFWICPixelFormatGUID IGraphicsDirect3D11FormatConvertWIC(IGraphicsFormat const gfxFormat)
	{
		return *IGraphicsDirect3D11FormatsTableWIC[IGraphicsFormatGetIndex(gfxFormat)];
	}
#endif	// if GD_IGRAPHICS_DIRECT3D1X_IMPL || GD_IGRAPHICS_DIRECT3D12_IMPL || GD_IGRAPHICS2D_DIRECT2D_IMPL

GD_NAMESPACE_END
