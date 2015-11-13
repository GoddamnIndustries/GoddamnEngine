// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsOpenGL/GraphicsOpenGLFormat.h
 * File contains export interface for OpenGL 4.3 Implementation of the graphics interface: format tables.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsFormat.h>

#if GD_IGRAPHICS_OPENGL_IMPL
#	include <GL/glew.h>
#endif

GD_NAMESPACE_BEGIN

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLFormatTypesTable[IGRAPHICS_FORMAT_TYPES_COUNT] = {
		/* IGRAPHICS_FORMAT_TYPE_INT8          */ GL_BYTE,
		/* IGRAPHICS_FORMAT_TYPE_UINT8         */ GL_UNSIGNED_BYTE,
		/* IGRAPHICS_FORMAT_TYPE_INT16         */ GL_SHORT,
		/* IGRAPHICS_FORMAT_TYPE_UINT16        */ GL_UNSIGNED_SHORT,
		/* IGRAPHICS_FORMAT_TYPE_INT32         */ GL_INT,
		/* IGRAPHICS_FORMAT_TYPE_UINT32        */ GL_UNSIGNED_INT,
		/* IGRAPHICS_FORMAT_TYPE_NORM8         */ GL_BYTE,
		/* IGRAPHICS_FORMAT_TYPE_UNORM8        */ GL_UNSIGNED_BYTE,
		/* IGRAPHICS_FORMAT_TYPE_NORM16        */ GL_SHORT,
		/* IGRAPHICS_FORMAT_TYPE_UNORM16       */ GL_UNSIGNED_SHORT,
		/* IGRAPHICS_FORMAT_TYPE_NORM32        */ GL_INT,
		/* IGRAPHICS_FORMAT_TYPE_UNORM32       */ GL_UNSIGNED_INT,
		/* IGRAPHICS_FORMAT_TYPE_FLOAT32       */ GL_FLOAT,
		/* IGRAPHICS_FORMAT_TYPE_FLOAT64       */ GL_DOUBLE,
		/* IGRAPHICS_FORMAT_TYPE_UNKNOWN       */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLFormatLayoutsTable[IGRAPHICS_FORMAT_LAYOUTS_COUNT] = {
		/* IGRAPHICS_FORMAT_LAYOUT_DEPTH       */ GL_DEPTH,
		/* IGRAPHICS_FORMAT_LAYOUT_R           */ GL_RED,
		/* IGRAPHICS_FORMAT_LAYOUT_RG          */ GL_RG,
		/* IGRAPHICS_FORMAT_LAYOUT_RGB         */ GL_RGB,
		/* IGRAPHICS_FORMAT_LAYOUT_RGBA        */ GL_RGBA,
		/* IGRAPHICS_FORMAT_LAYOUT_UNKNOWN     */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLIntegerFormatLayoutsTable[IGRAPHICS_FORMAT_LAYOUTS_COUNT] = {
		/* IGRAPHICS_FORMAT_LAYOUT_DEPTH       */ GL_DEPTH,	// Same for integer textures..
		/* IGRAPHICS_FORMAT_LAYOUT_R           */ GL_RED_INTEGER,
		/* IGRAPHICS_FORMAT_LAYOUT_RG          */ GL_RG_INTEGER,
		/* IGRAPHICS_FORMAT_LAYOUT_RGB         */ GL_RGB_INTEGER,
		/* IGRAPHICS_FORMAT_LAYOUT_RGBA        */ GL_RGBA_INTEGER,
		/* IGRAPHICS_FORMAT_LAYOUT_UNKNOWN     */ 0,
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	GLenum static const IGraphicsOpenGLFormatsTable[IGRAPHICS_FORMATS_COUNT] = {
		// ..Generic formats..
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* IGRAPHICS_FORMAT_R32G32B32A32_FLOAT */ GL_RGBA32F,
		/* IGRAPHICS_FORMAT_R32G32B32A32_UINT  */ GL_RGBA32UI,
		/* IGRAPHICS_FORMAT_R32G32B32A32_SINT  */ GL_RGBA32I,
		/* IGRAPHICS_FORMAT_R32G32B32_FLOAT    */ GL_RGB32F,
		/* IGRAPHICS_FORMAT_R32G32B32_UINT     */ GL_RGB32UI,
		/* IGRAPHICS_FORMAT_R32G32B32_SINT     */ GL_RGB32I,
		/* IGRAPHICS_FORMAT_R32G32_FLOAT       */ GL_RG32F,
		/* IGRAPHICS_FORMAT_R32G32_UINT        */ GL_RG32UI,
		/* IGRAPHICS_FORMAT_R32G32_SINT        */ GL_RG32I,
		/* IGRAPHICS_FORMAT_R32_FLOAT          */ GL_R32F,
		/* IGRAPHICS_FORMAT_R32_UINT           */ GL_R32UI,
		/* IGRAPHICS_FORMAT_R32_SINT           */ GL_R32I,
	//	/* IGRAPHICS_FORMAT_R16G16B16A16_FLOAT */ GL_RGBA16F, 
		/* IGRAPHICS_FORMAT_R16G16B16A16_UNORM */ GL_RGBA16,
		/* IGRAPHICS_FORMAT_R16G16B16A16_NORM  */ GL_RGBA16_SNORM,
		/* IGRAPHICS_FORMAT_R16G16B16A16_UINT  */ GL_RGBA16UI, 
		/* IGRAPHICS_FORMAT_R16G16B16A16_SINT  */ GL_RGBA16I,
	//	/* IGRAPHICS_FORMAT_R16G16B16_FLOAT    */ GL_RGB16F, 
		/* IGRAPHICS_FORMAT_R16G16B16_UNORM    */ GL_RGB16,
		/* IGRAPHICS_FORMAT_R16G16B16_NORM     */ GL_RGB16_SNORM,
		/* IGRAPHICS_FORMAT_R16G16B16_UINT     */ GL_RGB16UI, 
		/* IGRAPHICS_FORMAT_R16G16B16_SINT     */ GL_RGB16I,
	//	/* IGRAPHICS_FORMAT_R16G16_FLOAT       */ GL_RG16F, 
		/* IGRAPHICS_FORMAT_R16G16_UNORM       */ GL_RG16,
		/* IGRAPHICS_FORMAT_R16G16_NORM        */ GL_RG16_SNORM,
		/* IGRAPHICS_FORMAT_R16G16_UINT        */ GL_RG16UI, 
		/* IGRAPHICS_FORMAT_R16G16_SINT        */ GL_RG16I,
	//	/* IGRAPHICS_FORMAT_R16_FLOAT          */ GL_R16F, 
		/* IGRAPHICS_FORMAT_R16_UNORM          */ GL_R16,
		/* IGRAPHICS_FORMAT_R16_NORM           */ GL_R16_SNORM,
		/* IGRAPHICS_FORMAT_R16_UINT           */ GL_R16UI, 
		/* IGRAPHICS_FORMAT_R16_SINT           */ GL_R16I,
	//	/* IGRAPHICS_FORMAT_R8G8B8A8_FLOAT     */ GL_RGBA8F, 
		/* IGRAPHICS_FORMAT_R8G8B8A8_UNORM     */ GL_RGBA8,
		/* IGRAPHICS_FORMAT_R8G8B8A8_NORM      */ GL_RGBA8_SNORM,
		/* IGRAPHICS_FORMAT_R8G8B8A8_UINT      */ GL_RGBA8UI, 
		/* IGRAPHICS_FORMAT_R8G8B8A8_SINT      */ GL_RGBA8I,
	//	/* IGRAPHICS_FORMAT_R8G8B8_FLOAT       */ GL_RGB8F, 
		/* IGRAPHICS_FORMAT_R8G8B8_UNORM       */ GL_RGB8,
		/* IGRAPHICS_FORMAT_R8G8B8_NORM        */ GL_RGB8_SNORM,
		/* IGRAPHICS_FORMAT_R8G8B8_UINT        */ GL_RGB8UI, 
		/* IGRAPHICS_FORMAT_R8G8B8_SINT        */ GL_RGB8I, 
	//	/* IGRAPHICS_FORMAT_R8G8_FLOAT         */ GL_RG8F, 
		/* IGRAPHICS_FORMAT_R8G8_UNORM         */ GL_RG8,
		/* IGRAPHICS_FORMAT_R8G8_NORM          */ GL_RG8_SNORM,
		/* IGRAPHICS_FORMAT_R8G8_UINT          */ GL_RG8UI, 
		/* IGRAPHICS_FORMAT_R8G8_SINT          */ GL_RG8I,
	//	/* IGRAPHICS_FORMAT_R8_FLOAT           */ GL_R8F,
		/* IGRAPHICS_FORMAT_R8_UNORM           */ GL_R8,
		/* IGRAPHICS_FORMAT_R8_NORM            */ GL_R8_SNORM,
		/* IGRAPHICS_FORMAT_R8_UINT            */ GL_R8UI, 
		/* IGRAPHICS_FORMAT_R8_SINT            */ GL_R8I,

		// ..Depth formats..
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* IGRAPHICS_FORMAT_DEPTH32_FLOAT      */ GL_DEPTH_COMPONENT32F, 
		/* IGRAPHICS_FORMAT_DEPTH32_UINT       */ GL_DEPTH_COMPONENT32,
		/* IGRAPHICS_FORMAT_DEPTH32_INT        */ GL_DEPTH_COMPONENT32,
	//	/* IGRAPHICS_FORMAT_DEPTH16_FLOAT      */ GL_DEPTH_COMPONENT16F, 
		/* IGRAPHICS_FORMAT_DEPTH16_UINT       */ GL_DEPTH_COMPONENT16, 
		/* IGRAPHICS_FORMAT_DEPTH16_INT        */ GL_DEPTH_COMPONENT16,
	//	/* IGRAPHICS_FORMAT_DEPTH8_FLOAT       */ GL_DEPTH_COMPONENT8F, 
		/* IGRAPHICS_FORMAT_DEPTH8_UINT        */ GL_DEPTH_COMPONENT, 
		/* IGRAPHICS_FORMAT_DEPTH8_INT         */ GL_DEPTH_COMPONENT,

		// ..Additional invalid format..
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		/* IGRAPHICS_FORMAT_UNKNOWN            */ 0, 
	};
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	// ------------------------------------------------------------------------------------------
	//! Returns the OpenGL format that this format value represents.
	//! @param gfxFormat the format value to process.
	//! @returns The OpenGL format that this format value represents.
	GDINL static GLenum IGraphicsOpenGLFormatConvert(IGraphicsFormat const gfxFormat)
	{
		return IGraphicsOpenGLFormatsTable[IGraphicsFormatGetIndex(gfxFormat)];
	}
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	// ------------------------------------------------------------------------------------------
	//! Returns the OpenGL type that this format value represents.
	//! @param gfxFormat the format value to process.
	//! @returns The OpenGL type that this format value represents.
	GDINL static GLenum IGraphicsOpenGLFormatGetType(IGraphicsFormat const gfxFormat)
	{
		return IGraphicsOpenGLFormatTypesTable[IGraphicsFormatGetType(gfxFormat)];
	}
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

#if GD_IGRAPHICS_OPENGL_IMPL
	// ------------------------------------------------------------------------------------------
	//! Returns the OpenGL layout that this format value represents.
	//! @param gfxFormat the format value to process.
	//! @returns The OpenGL layout that this format value represents.
	GDINL static GLenum IGraphicsOpenGLFormatGetLayout(IGraphicsFormat const gfxFormat)
	{
		// According to the link above we cannot just simply translate our layout via a table -
		// integer textures use ~different layouts.
		// https://www.opengl.org/wiki/Pixel_Transfer#Pixel_format
		auto const gfxFormatLayout = IGraphicsFormatGetLayout(gfxFormat);
		return IGraphicsFormatGetType(gfxFormat) <= IGRAPHICS_FORMAT_TYPE_UINT32
			? IGraphicsOpenGLIntegerFormatLayoutsTable[gfxFormatLayout]
			: IGraphicsOpenGLFormatLayoutsTable[gfxFormatLayout];
	}
#endif	// if GD_IGRAPHICS_OPENGL_IMPL

GD_NAMESPACE_END
