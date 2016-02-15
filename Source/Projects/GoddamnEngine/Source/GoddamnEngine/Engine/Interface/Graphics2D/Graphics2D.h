// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics2D/Graphics2D.h
 * File contains export interface for the engine 2D graphics subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

#include <GoddamnEngine/Core/Math/Geometry.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN
	struct IGraphics2DFontCreationInfo;

	enum IGraphics2DDriver : UInt8
	{
		IGRAPHICS2D_DRIVER_DIRECT2D,
		IGRAPHICS2D_DRIVER_3D_OVERLAY,
		IGRAPHICS2D_DRIVER_UNKNOWN,
	};	// enum IGraphicsDriver

	// ==========================================================================================
	// IGraphics2D interface.
	// ==========================================================================================

	typedef IInterface IGraphics2DInterface;
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL_BASE(IGraphics2D, IRuntimeUniqueInterface);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Structure that contains information that is required to create a 2D GPU base renderer.
	GD_DSTRUCT(CopyFields)
	struct IGraphics2DRendererCreationInfo final
	{
	public:
	};	// struct IGraphics2DRendererCreationInfo

	// ------------------------------------------------------------------------------------------
	//! Base 2D GPU renderable object interface.
	uinterface IGraphics2DRenderer : public IGraphics2DInterface
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DRenderer, IGraphics2DInterface);

	public:
		IGraphics2DRendererCreationInfo const gfxRendererCreationInfo;
	protected:
		GD_DCTOR()
		GDINL explicit IGraphics2DRenderer(/*IGraphics2DRendererCreationInfo const* const gfxRendererCreationInfo*/)
			: gfxRendererCreationInfo(/**gfxRendererCreationInfo*/) {}
	public:

		// ------------------------------------------------------------------------------------------
		//! Updates the transformation parameters of the 2D renderable object.
		//! @param gfxRendererRect Rectangle into which the 2D object would be rendered.
		//! @param gfxRendererRotation Z rotation of the object in radians.
		GD_DFUNCTION()
		GDAPI virtual void Imm_SetTransform(GeomRect const gfxRendererRect
			, Float32 const gfxRendererRotation = 0.0f) GD_PURE_VIRTUAL;

#if GD_FALSE
		// ------------------------------------------------------------------------------------------
		//! Updates the depth parameters of the 2D renderable object.
		//! @param gfxRendererDepth Depth value of the 2D object.
		GD_DFUNCTION()
		GDAPI virtual void Imm_SetDepth(UInt8 const gfxRendererDepth) GD_PURE_VIRTUAL;
#endif	// if GD_FALSE

		// ------------------------------------------------------------------------------------------
		//! Updates the color parameter of the 2D renderable object.
		//! @param gfxColor Color multiplier of the 2D object.
		GD_DFUNCTION()
		GDAPI virtual void Imm_SetColor(GeomColor const gfxColor) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		//! Renders the 2D object.
		GD_DFUNCTION()
		GDAPI virtual void Imm_Render() GD_PURE_VIRTUAL;

	};	// uinterface IGraphics2DRenderer

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Line rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D bitmaps.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Structure that contains information that is required to create a 2D GPU bitmap.
	GD_DSTRUCT(CopyFields)
	struct IGraphics2DBitmapCreationInfo final
	{
	public:
		UInt32			BitmapWidth;			//!< Width of the bitmap data.
		UInt32			BitmapHeight;			//!< Height of the bitmap data.
		IGraphicsFormat	BitmapPixelFormat;		//!< Pixel format of the bitmap data.
		CHandle			BitmapData;
		SizeTp			BitmapDataLength;
	};	// struct IGraphics2DBitmapCreationInfo

	// ------------------------------------------------------------------------------------------
	//! GPU 2D font interface.
	GD_DINTERFACE()
	uinterface IGraphics2DBitmap : public IGraphics2DInterface
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DBitmap, IGraphics2DInterface);

	public:
		IGraphics2DBitmapCreationInfo const gfxBitmapCreationInfo;
	protected:
		GD_DCTOR()
		GDINL explicit IGraphics2DBitmap(IGraphics2DBitmapCreationInfo const* const gfxBitmapCreationInfo)
			: gfxBitmapCreationInfo(*gfxBitmapCreationInfo) {}
	public:
		// No public interface functions here..
	};	//uinterface IGraphics2DBitmap

	// ------------------------------------------------------------------------------------------
	//! Adds 2D bitmaps rendering support to the zero IGraphics2D interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DBaseWithBitmaps, IGraphics2D)
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Creates a 2D font with specified parameters.
		//! @param gfxBitmapPtr Pointer for output.
		//! @param gfxBitmapCreationInfo Pointer to the bitmap creation information.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult GfxImm_BitmapCreate(IGraphics2DBitmap** gfxBitmapPtr
			, IGraphics2DBitmapCreationInfo const* const gfxBitmapCreationInfo) GD_PURE_VIRTUAL;

	};	// uinterface IGraphics2DBaseWithBitmaps

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Rectangle rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Structure that contains information that is required to create a 2D GPU string renderer.
	GD_DSTRUCT(CopyFields)
	struct IGraphics2DRectRendererCreationInfo final
	{
	public:
		IGraphics2DRendererCreationInfo RectBaseRendererCreationInfo;
		IGraphics2DBitmap const*        RectBitmap;
	};	// struct IGraphics2DRectRenderer

	// ------------------------------------------------------------------------------------------
	//! GPU 2D rectangle renderer interface.
	GD_DINTERFACE()
	uinterface IGraphics2DRectRenderer : public IGraphics2DRenderer
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DRectRenderer, IGraphics2DRenderer);

	public:
		IGraphics2DRectRendererCreationInfo const gfxRectRendererCreationInfo;
	protected:
		GD_DCTOR()
		GDINL explicit IGraphics2DRectRenderer(IGraphics2DRectRendererCreationInfo const* const gfxRectRendererCreationInfo)
			: gfxRectRendererCreationInfo(*gfxRectRendererCreationInfo) {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphics2DRectRenderer

	// ------------------------------------------------------------------------------------------
	//! Adds 2D text rendering support to the zero IGraphics2D interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DBaseWithRectRenderers, IGraphics2D)
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Creates a 2D text renderer with specified parameters.
		//! @param gfxRectRendererPtr Pointer for output.
		//! @param gfxRectRendererCreationInfo Pointer to the text renderer creation information.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult GfxImm_RectRendererCreate(IGraphics2DRectRenderer** gfxRectRendererPtr
			, IGraphics2DRectRendererCreationInfo const* const gfxRectRendererCreationInfo) GD_PURE_VIRTUAL;

	};	// uinterface IGraphics2DBaseWithRectRenderers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Fonts.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines supported font location for the 2D graphics interface font loader.
	GD_DENUM()
	enum IGraphics2DFontLocation : UInt8
	{
		IGRPAHICS2D_FONT_LOCATION_FILE,
		IGRPAHICS2D_FONT_LOCATION_UNKNOWN,
		IGRPAHICS2D_FONT_LOCATIONS_COUNT,
	};	// enum IGraphics2DFontLocation

	// ------------------------------------------------------------------------------------------
	//! Defines supported font loading 2D GPU flags.
	GD_DENUM(FlagsEnum)
	enum IGraphics2DFontFlags : UInt8
	{
		IGRAPHICS2D_FONT_FLAGS_NONE                 = 0,
		IGRAPHICS2D_FONT_FLAGS_USE_SYSTEM_IF_EXISTS = GD_BIT(0),
	};	// enum IGraphics2DFontFlags
	GD_ENUM_DEFINE_FLAG_OPERATORS(IGraphics2DFontFlags);

	// ------------------------------------------------------------------------------------------
	//! Structure that contains information that is required to create a 2D GPU string renderer.
	GD_DSTRUCT(CopyFields)
	struct IGraphics2DFontCreationInfo final
	{
	public: 
		union {
			WideCStr            FontFilePath;	//!< Path to the font file on the disk.
			CHandle             FontData;		//!< Pointer to the loaded font data.
		};	// anonymous union
		IGraphics2DFontLocation FontLocation;	//!< Type of the location of the font data.
		IGraphics2DFontFlags	FontFlags;		//!< Flags of the font.
	};	// struct IGraphics2DFontCreationInfo

	// ------------------------------------------------------------------------------------------
	//! GPU 2D font interface.
	GD_DINTERFACE()
	uinterface IGraphics2DFont : public IGraphics2DInterface
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DFont, IGraphics2DInterface);

	public:
		IGraphics2DFontCreationInfo const gfxFontCreationInfo;
	protected:
		GD_DCTOR()
		GDINL explicit IGraphics2DFont(IGraphics2DFontCreationInfo const* const gfxFontCreationInfo)
			: gfxFontCreationInfo(*gfxFontCreationInfo) {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphics2DFont

	// ------------------------------------------------------------------------------------------
	//! Adds 2D text rendering support to the zero IGraphics2D interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DBaseWithFonts, IGraphics2D)
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Creates a 2D font with specified parameters.
		//! @param gfxFontPtr Pointer for output.
		//! @param gfxFontCreationInfo Pointer to the font creation information.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult GfxImm_FontCreate(IGraphics2DFont** gfxFontPtr
			, IGraphics2DFontCreationInfo const* const gfxFontCreationInfo) GD_PURE_VIRTUAL;

	};	// uinterface IGraphics2DBaseWithFonts

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Text rendering.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines the text rendering weights.
	GD_DENUM()
	enum IGraphics2DTextRendererWeight : UInt8
	{
		IGRAPHICS2D_TEXT_RENDERER_WEIGHT_REGULAR,
		IGRAPHICS2D_TEXT_RENDERER_WEIGHT_THIN,
		IGRAPHICS2D_TEXT_RENDERER_WEIGHT_BOLD,
		IGRAPHICS2D_TEXT_RENDERER_WEIGHT_UNKNOWN,
		IGRAPHICS2D_TEXT_RENDERER_WEIGHTS_COUNT,
	};	// enum IGraphics2DTextRendererTextWeight

	// ------------------------------------------------------------------------------------------
	//! Defines the text rendering styles.
	GD_DENUM()
	enum IGraphics2DTextRendererStyle : UInt8
	{
		IGRAPHICS2D_TEXT_RENDERER_STYLE_NORMAL,
		IGRAPHICS2D_TEXT_RENDERER_STYLE_OBLIQUE,
		IGRAPHICS2D_TEXT_RENDERER_STYLE_ITALIC,
		IGRAPHICS2D_TEXT_RENDERER_STYLE_UNKNOWN,
		IGRAPHICS2D_TEXT_RENDERER_STYLES_COUNT,
	};	// enum IGraphics2DTextRendererTextStyle

	// ------------------------------------------------------------------------------------------
	//! Defines the text rendering data horizontal alignment.
	GD_DENUM()
	enum IGraphics2DTextRendererHorizontalAlignment : UInt8
	{
		IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_LEFT,
		IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_CENTER,
		IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_RIGHT,
		IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_JUSTIFGDD,
		IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENT_UNKNOWN,
		IGRAPHICS2D_TEXT_RENDERER_HORIZONTAL_ALIGNMENTS_COUNT,
	};	// enum IGraphics2DTextRendererHorizontalAlignment

	// ------------------------------------------------------------------------------------------
	//! Defines the text rendering data vertical alignment.
	GD_DENUM()
	enum IGraphics2DTextRendererVerticalAlignment : UInt8
	{
		IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_TOP,
		IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_MIDDLE,
		IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_BOTTOM,
		IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_JUSTIFGDD,
		IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENT_UNKNOWN,
		IGRAPHICS2D_TEXT_RENDERER_VERTICAL_ALIGNMENTS_COUNT,
	};	// enum IGraphics2DTextRendererVerticalAlignment

	// ------------------------------------------------------------------------------------------
	//! Structure that contains information that is required to create a 2D GPU string renderer.
	GD_DSTRUCT(CopyFields)
	struct IGraphics2DTextRendererCreationInfo final
	{
	public:
		union {
			WideCStr								TextRendererDataWide;				//!< Text data as wide characters array.
				CStr							    TextRendererDataANSI;				//!< Text data as multi-Byte characters array.
		};	// anonymous union
		bool									    TextRendererIsWideString;			//!< Tests, whether we are rendering a wide string or the multi-Byte one.
		IGraphics2DFont const*						TextRendererFont;					//!< Pointer to the font.
		Float32										TextRendererSize;					//!< Size of the text font.
		IGraphics2DTextRendererWeight				TextRendererWeight;					//!< Weight of the text font.
		IGraphics2DTextRendererStyle				TextRendererStyle;					//!< Style of the text font.
		IGraphics2DTextRendererHorizontalAlignment	TextRendererHorizontalAlignment;	//!< Horizontal text alignment.
		IGraphics2DTextRendererVerticalAlignment	TextRendererVerticalAlignment;		//!< Vertical text alignment.
	};	// struct IGraphics2DTextRendererCreationInfo

	// ------------------------------------------------------------------------------------------
	//! Safely sets the text data to the @ref IGraphics2DTextRendererCreationInfo struct.
#define GD_IGRAPHICS2D_TEXT_RENDERER_DATA(TextData) &TextData[0], TypeTraits::IsSame<decltype(&TextData[0]), WideCStr>::Value

	// ------------------------------------------------------------------------------------------
	//! GPU 2D text renderer interface.
	GD_DINTERFACE()
	uinterface IGraphics2DTextRenderer : public IGraphics2DRenderer
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2DTextRenderer, IGraphics2DRenderer);

	public:
		IGraphics2DTextRendererCreationInfo const gfxTextRendererCreationInfo;
	protected:
		GD_DCTOR()
		GDINL explicit IGraphics2DTextRenderer(IGraphics2DTextRendererCreationInfo const* const gfxTextRendererCreationInfo)
			: gfxTextRendererCreationInfo(*gfxTextRendererCreationInfo) {}
	public:
		// No public interface functions here..
	};	// uinterface IGraphics2DTextRenderer

	// ------------------------------------------------------------------------------------------
	//! Adds 2D text rendering support to the zero IGraphics2D interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2DBaseWithTextRenderers, IGraphics2D)
	{
	public:

		// ------------------------------------------------------------------------------------------
		//! Creates a 2D text renderer with specified parameters.
		//! @param gfxSamplerStatePtr Pointer for output.
		//! @param gfxSamplerStateCreationInfo Pointer to the text renderer creation information.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult GfxImm_TextRendererCreate(IGraphics2DTextRenderer** gfx2DTextRendererPtr
			, IGraphics2DTextRendererCreationInfo const* const gfxTextRendererCreationInfo) GD_PURE_VIRTUAL;

	};	// uinterface IGraphics2DBase_WithTextRenderers

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! GPU-accelerated 2D graphics interface.
	GD_DINTERFACE()
	GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IGraphics2D, IGraphics2D)
#if !GD_D_REFLECTOR	// DReflector should not parse singletons declarations.
		, public Singleton<IGraphics2D>
#endif	// if !GD_D_REFLECTOR
	{
		GD_DEFINE_CLASS_INFORMATION(IGraphics2D, IRuntimeUniqueInterface);
	};	// uinterface IGraphics2D

	// ------------------------------------------------------------------------------------------
	//! Global 2D graphics interface pointer.
	GD_DSTATIC_VARIABLE()
	GDAPI extern IUniqueInterfacePointer<IGraphics2D> Graphics2D;

GD_NAMESPACE_END

GDINL bool RRR() { return true; }
#define GD_IGRAPHICS2D_CHECK_ARGS(Operation) RRR() //GD_GLUE(Check_, Operation)
