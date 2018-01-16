// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/Graphics.h
 * File contains export interface for the engine graphics subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/UniqueInterface.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsFormat.h>

#include <GoddamnEngine/Core/Math/Geometry.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#define GD_ARG_VERIFY(cond) if(!(cond)) return IResult::InvalidArguments;

#define GD_IGRAPHICS_VIRTUAL virtual
#define GD_IGRAPHICS_PURE_VIRTUAL GD_PURE_VIRTUAL

/*
DEFINE_GUID(IID_ID3D11DepthStencilState,0x03823efb,0x8d8f,0x4e1c,0x9a,0xa2,0xf6,0x4b,0xb2,0xcb,0xfd,0xf1);
DEFINE_GUID(IID_ID3D11BlendState,0x75b68faa,0x347d,0x4159,0x8f,0x45,0xa0,0x64,0x0f,0x01,0xcd,0x9a);
DEFINE_GUID(IID_ID3D11RasterizerState,0x9bb4ab81,0xab1a,0x4d8f,0xb5,0x06,0xfc,0x04,0x20,0x0b,0x6e,0xe7);

1.	DEFINE_GUID(IID_ID3D11Resource,0xdc8e63f3,0xd12b,0x4952,0xb4,0x7b,0x5e,0x45,0x02,0x6a,0x86,0x2d);

2.	DEFINE_GUID(IID_ID3D11Buffer,0x48570b85,0xd1ee,0x4fcd,0xa2,0x50,0xeb,0x35,0x07,0x22,0xb0,0x37);

4.	DEFINE_GUID(IID_ID3D11Texture1D,0xf8fb5c27,0xc6b3,0x4f75,0xa4,0xc8,0x43,0x9a,0xf2,0xef,0x56,0x4c);
	DEFINE_GUID(IID_ID3D11Texture2D,0x6f15aaf2,0xd208,0x4e89,0x9a,0xb4,0x48,0x95,0x35,0xd3,0x4f,0x9c);
	DEFINE_GUID(IID_ID3D11Texture3D,0x037e866e,0xf56d,0x4357,0xa8,0xaf,0x9d,0xab,0xbe,0x6e,0x25,0x0e);

5.	DEFINE_GUID(IID_ID3D11View,0x839d1216,0xbb2e,0x412b,0xb7,0xf4,0xa9,0xdb,0xeb,0xe0,0x8e,0xd1);

    DEFINE_GUID(IID_ID3D11ShaderResourceView,0xb0e06fe0,0x8192,0x4e1a,0xb1,0xca,0x36,0xd7,0x41,0x47,0x10,0xb2);
6.	DEFINE_GUID(IID_ID3D11RenderTargetView,0xdfdba067,0x0b8d,0x4865,0x87,0x5b,0xd7,0xb4,0x51,0x6c,0xc1,0x64);
	DEFINE_GUID(IID_ID3D11DepthStencilView,0x9fdac92a,0x1876,0x48c3,0xaf,0xad,0x25,0xb9,0x4f,0x84,0xa9,0xb6);
	DEFINE_GUID(IID_ID3D11UnorderedAccessView,0x28acf509,0x7f5c,0x48f6,0x86,0x11,0xf3,0x16,0x01,0x0a,0x63,0x80);

3.	DEFINE_GUID(IID_ID3D11Sampler,0xda6fea51,0x564c,0x4487,0x98,0x10,0xf0,0xd0,0xf9,0xb4,0xe3,0xa5);

6.	DEFINE_GUID(IID_ID3D11VertexShader,0x3b301d64,0xd678,0x4289,0x88,0x97,0x22,0xf8,0x92,0x8b,0x72,0xf3);
	DEFINE_GUID(IID_ID3D11HullShader,0x8e5c6061,0x628a,0x4c8e,0x82,0x64,0xbb,0xe4,0x5c,0xb3,0xd5,0xdd);
	DEFINE_GUID(IID_ID3D11DomainShader,0xf582c508,0x0f36,0x490c,0x99,0x77,0x31,0xee,0xce,0x26,0x8c,0xfa);
	DEFINE_GUID(IID_ID3D11GeometryShader,0x38325b96,0xeffb,0x4022,0xba,0x02,0x2e,0x79,0x5b,0x70,0x27,0x5c);
	DEFINE_GUID(IID_ID3D11PixelShader,0xea82e40d,0x51dc,0x4f33,0x93,0xd4,0xdb,0x7c,0x91,0x25,0xae,0x8c);
	DEFINE_GUID(IID_ID3D11ComputeShader,0x4f5b196e,0xc2bd,0x495e,0xbd,0x01,0x1f,0xde,0xd3,0x8e,0x49,0x69);

DEFINE_GUID(IID_ID3D11InputLayout,0xe4819ddc,0x4cf0,0x4025,0xbd,0x26,0x5d,0xe8,0x2a,0x3e,0x07,0xb7);

DEFINE_GUID(IID_ID3D11Asynchronous,0x4b35d0cd,0x1e15,0x4258,0x9c,0x98,0x1b,0x13,0x33,0xf6,0xdd,0x3b);
DEFINE_GUID(IID_ID3D11Query,0xd6c00747,0x87b7,0x425e,0xb8,0x4d,0x44,0xd1,0x08,0x56,0x0a,0xfd);
DEFINE_GUID(IID_ID3D11Predicate,0x9eb576dd,0x9f77,0x4d86,0x81,0xaa,0x8b,0xab,0x5f,0xe4,0x90,0xe2);
DEFINE_GUID(IID_ID3D11Counter,0x6e8c49fb,0xa371,0x4770,0xb4,0x40,0x29,0x08,0x60,0x22,0xb7,0x41);

DEFINE_GUID(IID_ID3D11ClassInstance,0xa6cd7faa,0xb0b7,0x4a2f,0x94,0x36,0x86,0x62,0xa6,0x57,0x97,0xcb);
DEFINE_GUID(IID_ID3D11ClassLinkage,0xddf57cba,0x9543,0x46e4,0xa1,0x2b,0xf2,0x07,0xa0,0xfe,0x7f,0xed);

DEFINE_GUID(IID_ID3D11CommandList,0xa24bc4d1,0x769e,0x43f7,0x80,0x13,0x98,0xff,0x56,0x6c,0x18,0xe2);

DEFINE_GUID(IID_ID3D11DeviceContext,0xc0bfa96c,0xe089,0x44fb,0x8e,0xaf,0x26,0xf8,0x79,0x61,0x90,0xda);
DEFINE_GUID(IID_ID3D11Device,0xdb6f6ddb,0xac77,0x4e88,0x82,0x53,0x81,0x9d,0xf9,0xbb,0xf1,0x40);
*/

GD_NAMESPACE_BEGIN

	enum IGraphicsDriver : UInt32
	{
		IGRAPHICS_DRIVER_DIRECTX12,
		IGRAPHICS_DRIVER_METAL,
		IGRAPHICS_DRIVER_VULKAN,
		IGRAPHICS_DRIVER_DIRECTX11,
		IGRAPHICS_DRIVER_OPENGL,
		IGRAPHICS_DRIVER_OPENGLES,
		IGRAPHICS_DRIVER_ZERO,
		IGRAPHICS_DRIVER_UNKNOWN,
	};	// enum IGraphicsDriver

	/*!
	 * Comparison mode.
	 */
	enum IGraphicsComparisonMode : UInt32
	{
		IGRAPHICS_COMPARISON_MODE_NEVER,
		IGRAPHICS_COMPARISON_MODE_ALWAYS,
		IGRAPHICS_COMPARISON_MODE_EQUAL,
		IGRAPHICS_COMPARISON_MODE_NOT_EQUAL,
		IGRAPHICS_COMPARISON_MODE_LESS,
		IGRAPHICS_COMPARISON_MODE_LESS_OR_EQUAL,
		IGRAPHICS_COMPARISON_MODE_GREATER,
		IGRAPHICS_COMPARISON_MODE_GREATER_OR_EQUAL,
		IGRAPHICS_COMPARISON_MODE_UNKNOWN,
		IGRAPHICS_COMPARISON_MODES_COUNT,
	};	// enum IGraphicsComparisonMode 

	GD_DEFINE_PARTIAL_CALSS_FIRST(IGraphics, IRuntimeUniqueInterface);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsObject interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Description of a GPU object.
	 */
	struct IGraphicsObjectCreateInfo {};

	/*!
	 * A GPU object base interface.
	 */
	class IGraphicsObject : public IInterface
	{
	protected:
		GDINL explicit IGraphicsObject(IGraphicsObjectCreateInfo const&) {}
	};	// class IGraphicsObject

#define GD_DEFINE_IGRAPHICS_BASE_OBJECT(TGraphicsObject, TGraphicsObjectBase, ...) \
	class TGraphicsObject : public TGraphicsObjectBase \
	{ \
	protected:\
		GDINL explicit TGraphicsObject(TGraphicsObject##CreateInfo const& gfxCreateInfo) \
			: TGraphicsObjectBase(gfxCreateInfo) {} \
		__VA_ARGS__ \
	} /*class TGraphicsObject*/

#define GD_DEFINE_IGRAPHICS_OBJECT(TGraphicsObject, TGraphicsObjectBase, ...) \
	class TGraphicsObject : public TGraphicsObjectBase, public TGraphicsObject##CreateInfo \
	{ \
	protected:\
		GDINL explicit TGraphicsObject(TGraphicsObject##CreateInfo const& gfxCreateInfo) \
			: TGraphicsObjectBase(gfxCreateInfo), TGraphicsObject##CreateInfo(gfxCreateInfo) {} \
		__VA_ARGS__ \
	} /*class TGraphicsObject*/

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Canvas output.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * List of the supported canvas output modes.
	 */
	enum IGraphicsCanvasMode : UInt32
	{
		IGRAPHICS_OUTPUT_MODE_WINDOWED,
		IGRAPHICS_OUTPUT_MODE_PSEUDO_FULLSCREEN,
		IGRAPHICS_OUTPUT_MODE_FULLSCREEN,
		IGRAPHICS_OUTPUT_MODE_UNKNOWN,
	};	// enum IGraphicsCanvasMode

	/*!
	 * List of the supported canvas vsync modes.
	 */
	enum IGraphicsCanvasVsyncMode : UInt32
	{
		IGRAPHICS_OUTPUT_VSYNC_30FPS,
		IGRAPHICS_OUTPUT_VSYNC_60FPS,
		IGRAPHICS_OUTPUT_VSYNC_DISABLED,
		IGRAPHICS_OUTPUT_VSYNC_UNKNOWN,
	};	// enum IGraphicsCanvasVsyncMode 

	/*!
	 * Adds canvas output support to the zero IGraphics interface.
	 */
	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseWithCanvas, IGraphics)
	{
	public:
		
		/*!
		 * Retrieves the mode in which graphics canvas device is running. 
		 */
		GDAPI virtual IGraphicsCanvasMode Canvas_GetMode() const GD_PURE_VIRTUAL;

		/*!
		 * Changes the mode in which the graphics canvas device is running. 
		 *
		 * @param gfxCanvasMode The new canvas mode that would be set.
		 * @param gfxForcelyReapply Do forcedly reapply mode, even if device is running is the same mode.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult Canvas_SetMode(IGraphicsCanvasMode const gfxCanvasMode
			, bool const gfxForcelyReapply = false) GD_PURE_VIRTUAL;

#if 0
		/*!
		 * Returns mode, in which graphics device is performing vertical sync.
		 */
		GDAPI virtual IGraphicsCanvasVsyncMode Canvas_GetVsyncMode() const GD_PURE_VIRTUAL;

		/*!
		 * Changes mode, in which graphics device is performing vertical sync.
		 *
		 * @param gfxVsyncMode The mode in which graphics device is performing vertical sync.
		 * @param gfxForcelyReapply Do forcedly reapply mode, even if vsync is running is the same mode.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult Canvas_SetVsyncEnabled(IGraphicsCanvasVsyncMode const gfxVsyncMode, bool const gfxForcelyReapply = false) GD_PURE_VIRTUAL;
#endif	// if 0

		//! @todo Do something with pointer here. It is not very safe.

		/*!
		 * Returns a vector reference with all supported resolution descriptors.
		 */
		GDAPI virtual GeomResolution const* Canvas_GetSupportedResolutions() const GD_PURE_VIRTUAL;
	
		/*!
		 * Returns pointer to the resolution descriptor that is currently set.
		 */
		GDAPI virtual GeomResolution const* Canvas_GetResolution() const GD_PURE_VIRTUAL;

		/*!
		 * Changes resolution in which graphics canvas device is rendering and resize several frame
		 * buffers (those, that have a corresponding flag specified).
		 *
		 * @param gfxResolution Pointer to the resolution descriptor that would be set.
		 * @param gfxForcelyReapply Do forcedly resolution mode, even if device is running is the same resolution.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult Canvas_SetResolution(GeomResolution const* const gfxResolution
			, bool const gfxForcelyReapply = false) GD_PURE_VIRTUAL;

	};	// class IGraphicsBaseWithCanvas

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsCommandList interface.                          ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Description of a GPU command list.
	 */
	struct IGraphicsCommandListCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
	};	// struct IGraphicsCommandListCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsCommandList, IGraphicsObject);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           Graphics Resource interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Resource usage patterns.
	 */
	enum IGraphicsResourceUsage : UInt32
	{
		IGRAPHICS_RESOURCE_USAGE_DEFAULT,
		IGRAPHICS_RESOURCE_USAGE_IMMUTABLE,
		IGRAPHICS_RESOURCE_USAGE_DYNAMIC,
		IGRAPHICS_RESOURCE_USAGE_STAGING,
		IGRAPHICS_RESOURCE_USAGE_UNKNOWN,
		IGRAPHICS_RESOURCE_USAGES_COUNT,
	};	// enum IGraphicsResourceUsage

	/*!
	 * Resource CPU usage flags.
	 */
	//! @{
	enum IGraphicsResourceCpuAccessFalgs : UInt32
	{
		IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_NONE  = 0,
		IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_READ  = GD_BIT(0),
		IGRAPHICS_RESOURCE_CPU_ACCESS_FLAGS_WRITE = GD_BIT(1),
	};	// enum IGraphicsResourceCpuAccessFalgs
	GD_ENUM_DEFINE_FLAG_OPERATORS(IGraphicsResourceCpuAccessFalgs);
	//! @}

	/*!
	 * Resource map mode.
	 */
	enum IGraphicsResourceMapMode : UInt32
	{
		IGRAPHICS_RESOURCE_MAP_MODE_READ,
		IGRAPHICS_RESOURCE_MAP_MODE_WRITE,
		IGRAPHICS_RESOURCE_MAP_MODE_READ_WRITE,
		IGRAPHICS_RESOURCE_MAP_MODE_UNKNOWN,
		IGRAPHICS_RESOURCE_MAP_MODES_COUNT,
	};	// enum IGraphicsResourceMapMode

	/*!
	 * Information required to create a GPU resource.
	 */
	struct IGraphicsResourceCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
		IGraphicsResourceUsage			ResourceUsage;			//!< Resource usage.
		IGraphicsResourceCpuAccessFalgs ResourceCpuAccessFlags;	//!< CPU access flags,
	};	// struct IGraphicsResourceCreateInfo

	GD_DEFINE_IGRAPHICS_BASE_OBJECT(IGraphicsResource, IGraphicsObject);

	// ------------------------------------------------------------------------------------------
	// Buffers.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Supported buffer object types.
	 */
	enum IGraphicsBufferType : UInt32
	{
		IGRAPHICS_BUFFER_TYPE_VERTEX,
		IGRAPHICS_BUFFER_TYPE_INDEX,
		IGRAPHICS_BUFFER_TYPE_UNIFORM,
		IGRAPHICS_BUFFER_TYPE_UNKNOWN,
		IGRAPHICS_BUFFER_TYPES_COUNT,
	};	// enum IGraphicsBufferType

	/*!
	 * Description of a GPU buffer.
	 */
	struct IGraphicsBufferCreateInfo : public IGraphicsResourceCreateInfo
	{
	public:
		SizeTp               BufferSize;	//!< Size of the buffer in bytes.
		IGraphicsBufferType  BufferType;	//!< Type of the buffer: vertex, index, uniform or etc buffer.
	};	// struct IGraphicsBufferCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsBuffer, IGraphicsResource);
	
	// ------------------------------------------------------------------------------------------
	// 1D, 2D, 3D Texture Arrays and Cube textures.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Defines flags for the texture.
	 */
	//! @{
	enum IGraphicsTextureFlags : UInt32
	{
		IGRAPHICS_TEXTURE_FLAGS_GENERATE_MIPMAPS = GD_BIT(0),
		IGRAPHICS_TEXTURE_FLAGS_TEXTURECUBE      = GD_BIT(1),
	};	// enum IGraphicsTexture2DFlags
	GD_ENUM_DEFINE_FLAG_OPERATORS(IGraphicsTextureFlags);
	//! @}

	/*!
	 * Compression mode for the textures data.
	 */
	enum IGraphicsTextureCompressionMode : UInt32
	{
		IGRAPHICS_TEXTURE_COMPRESSION_NONE,
		IGRAPHICS_TEXTURE_COMPRESSION_DXT1,
		IGRAPHICS_TEXTURE_COMPRESSION_DXT3,
		IGRAPHICS_TEXTURE_COMPRESSION_DXT5,
		IGRAPHICS_TEXTURE_COMPRESSION_UNKNOWN,
		IGRAPHICS_TEXTURE_COMPRESSIONS_COUNT,
	};	// enum IGraphicsTextureCompressionMode

	/*!
	 * Description of a GPU texture.
	 */
	struct IGraphicsTextureCreateInfo : public IGraphicsResourceCreateInfo
	{
	public:
		IGraphicsFormat                   TexturePixelFormat;	    //!< Pixel format of the texture data.
		IGraphicsTextureCompressionMode   TextureCompressionMode;	//!< Compression mode of the texture. 
		IGraphicsTextureFlags			  TextureFlags;             //!< Additional flags.
		UInt32                            TextureArraySize;         //!< Number of textures in the array.
		UInt32			                  TextureMipMapsCount;      //!< Amount of the mipmap levels of the texture.
	};	// struct IGraphicsTexture1DCreateInfo

	/*!
	 * Description of a GPU 1D texture.
	 */
	struct IGraphicsTexture1DCreateInfo : public IGraphicsTextureCreateInfo
	{
	public:
		UInt32 Texture1DWidth;	//!< Width of the texture.
	};	// struct IGraphicsTexture1DCreateInfo

	/*!
	 * Description of a GPU 2D texture.
	 */
	struct IGraphicsTexture2DCreateInfo : public IGraphicsTextureCreateInfo
	{
	public:
		UInt32 Texture2DWidth;	//!< Width of the texture.
		UInt32 Texture2DHeight;	//!< Height of the texture.
	};	// struct IGraphicsTexture2DCreateInfo

	/*!
	 * Description of a GPU 3D texture.
	 */
	struct IGraphicsTexture3DCreateInfo : public IGraphicsTextureCreateInfo
	{
	public:
		UInt32 Texture3DWidth;	//!< Width of the texture.
		UInt32 Texture3DHeight;	//!< Height of the texture.
		UInt32 Texture3DDepth;	//!< Depth of the texture.
	};	// struct IGraphicsTexture3DCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsTexture1D, IGraphicsResource);
	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsTexture2D, IGraphicsResource);
	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsTexture3D, IGraphicsResource);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                              Graphics View interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Information required to create a GPU view.
	 */
	struct IGraphicsViewCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
	};	// struct IGraphicsViewCreateInfo

	GD_DEFINE_IGRAPHICS_BASE_OBJECT(IGraphicsView, IGraphicsObject);

	// ------------------------------------------------------------------------------------------
	// Shader resource view.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Defines types of the supported shader resource views.
	 */
	enum IGraphicsShaderResourceViewType : UInt32
	{
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_BUFFER,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_1D,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_1D_ARRAY,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_2D,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_2D_ARRAY,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_2D_MS,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_2D_MS_ARRAY,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_3D,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_3D_ARRAY,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_CUBE,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_TEXTURE_CUBE_ARRAY,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPE_UNKNOWN,
		IGRAPHICS_SHADER_RESOURCE_VIEW_TYPES_COUNT,
	};	// enum IGraphicsShaderResourceViewType

	/*!
	 * Information required to create a GPU shader resource view.
	 */
	struct IGraphicsShaderResourceViewCreateInfo : public IGraphicsViewCreateInfo
	{
	public:
		IGraphicsFormat SrvFormat;
		IGraphicsShaderResourceViewType SrvType;
		union {
			struct {
			} SrvBuffer;
			struct {
			} SrvTexture1D;
			struct {
			} SrvTexture1DArray;
			struct {
			} SrvTexture2D;
			struct {
			} SrvTexture2DArray;
			struct {
			} SrvTexture2DMS;
			struct {
			} SrvTexture2DMSArray;
			struct {
			} SrvTexture3D;
			struct {
			} SrvTexture3DArray;
		};
	};	// struct IGraphicsShaderResourceViewCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsShaderResourceView, IGraphicsView);

	// ------------------------------------------------------------------------------------------
	// Unordered Access resource view.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Defines types of the supported unordered access views.
	 */
	enum IGraphicsUnorderedAccessViewType : UInt32
	{
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_BUFFER,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_TEXTURE_1D,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_TEXTURE_1D_ARRAY,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_TEXTURE_2D,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_TEXTURE_2D_ARRAY,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_TEXTURE_3D,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_TEXTURE_3D_ARRAY,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPE_UNKNOWN,
		IGRAPHICS_UNORDERED_ACCESS_VIEW_TYPES_COUNT,
	};	// enum IGraphicsUnorderedAccessViewType

	/*!
	 * Information required to create a GPU unordered access view.
	 */
	struct IGraphicsUnorderedAcessViewCreateInfo : public IGraphicsViewCreateInfo
	{
	public:
		IGraphicsFormat UavFormat;
		IGraphicsUnorderedAccessViewType UavType;
		union {
			struct {
			} UavBuffer;
			struct {
			} UavTexture1D;
			struct {
			} UavTexture1DArray;
			struct {
			} UavTexture2D;
			struct {
			} UavTexture2DArray;
			struct {
			} UavTexture3D;
			struct {
			} UavTexture3DArray;
		};
	};	// struct IGraphicsShaderResourceViewCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsUnorderedAcessView, IGraphicsView);

	// ------------------------------------------------------------------------------------------
	// Render Target resource view.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Defines types of the supported render target views.
	 */
	enum IGraphicsRenderTargetViewType : UInt32
	{
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_BUFFER,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_TEXTURE_1D,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_TEXTURE_1D_ARRAY,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_TEXTURE_2D,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_TEXTURE_2D_ARRAY,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_TEXTURE_3D,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_TEXTURE_3D_ARRAY,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPE_UNKNOWN,
		IGRAPHICS_RENDER_TARGET_VIEW_TYPES_COUNT,
	};	// enum IGraphicsRenderTargetViewType

	/*!
	 * Information required to create a GPU render target view.
	 */
	struct IGraphicsRenderTargetViewCreateInfo : public IGraphicsViewCreateInfo
	{
	public:
		IGraphicsFormat RtvFormat;
		IGraphicsUnorderedAccessViewType RtvType;
		union {
			struct {
			} RtvBuffer;
			struct {
			} RtvTexture1D;
			struct {
			} RtvTexture1DArray;
			struct {
			} RtvTexture2D;
			struct {
			} RtvTexture2DArray;
			struct {
			} RtvTexture3D;
			struct {
			} RtvTexture3DArray;
		};
	};	// struct IGraphicsRenderTargetViewCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsRenderTargetView, IGraphicsView);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            Graphics Sampler interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	
	/*!
	 * Wrap mode for some dimension of the sampler.
	 */
	enum IGraphicsSamplerAddressMode : UInt32
	{
		IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO,
		IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT,
		IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR,
		IGRAPHICS_SAMPLER_ADDRESS_MODE_UNKNOWN,
		IGRAPHICS_SAMPLER_ADDRESS_MODES_COUNT,
	};	// enum IGraphicsSamplerAddressMode 

	/*!
	 * Minification and magnification filtering modes.
	 */
	enum IGraphicsSamplerMinMagFilter : UInt32
	{
		IGRAPHICS_SAMPLER_MINMAG_FILTER_NEAREST,
		IGRAPHICS_SAMPLER_MINMAG_FILTER_LINEAR,
		IGRAPHICS_SAMPLER_MINMAG_FILTER_UNKNOWN,
		IGRAPHICS_SAMPLER_MINMAG_FILTERS_COUNT,
	};	// enum IGraphicsSamplerMinMagFilter

	/*!
	 * Sampling border color.
	 */
	enum IGraphicsSamplerBorderColor : UInt32
	{
		IGRAPHICS_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK,
		IGRAPHICS_SAMPLER_BORDER_COLOR_OPAQUE_BLACK,
		IGRAPHICS_SAMPLER_BORDER_COLOR_OPAQUE_WHITE,
		IGRAPHICS_SAMPLER_BORDER_COLOR_UNKNOWN,
		IGRAPHICS_SAMPLER_BORDER_COLORS_COUNT,
	};	// enum IGraphicsSamplerBorderColor

	/*!
	 * Description of a GPU sampler state.
	 */
	struct IGraphicsSamplerCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
		IGraphicsSamplerMinMagFilter SamplerMinFilter;			//!< Minification filtering.
		IGraphicsSamplerMinMagFilter SamplerMagFilter;			//!< Magnification filtering.
		IGraphicsSamplerAddressMode  SamplerAddressModeU;	    //!< Wrap mode for the U dimension of the texture.
		IGraphicsSamplerAddressMode  SamplerAddressModeV;	    //!< Wrap mode for the V dimension of the texture.
		IGraphicsSamplerAddressMode  SamplerAddressModeW;	    //!< Wrap mode for the W dimension of the texture (for 3D textures).
		IGraphicsSamplerBorderColor  SamplerBorderColor;		//!< Border color.
		IGraphicsComparisonMode      SamplerCompFunc;           //!< Compares sampled data against existing sampled data.
		Float32                      SamplerMipLodBias;			//!< Offset from the calculated mipmap level.
		Float32                      SamplerLodMin;				//!< Minimum level of detail parameter.
		Float32                      SamplerLodMax;				//!< Maximum level of detail parameter.
		bool                         SamplerAnisotropyEnabled;	//!< Is anisotropy enabled;
		Float32	                     SamplerAnisotropyMax;		//!< Max anisotropy filtering level.
	};	// struct IGraphicsSamplerCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsSampler, IGraphicsObject);

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                             Graphics Shader interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * List of the supported shader types.
	 */
	enum IGraphicsShaderType : UInt32
	{
		IGRAPHICS_SHADER_TYPE_VERTEX,
		IGRAPHICS_SHADER_TYPE_TESS_CONTROL,
		IGRAPHICS_SHADER_TYPE_TESS_EVALUATION,
		IGRAPHICS_SHADER_TYPE_GEOMETRY,
		IGRAPHICS_SHADER_TYPE_FRAGMENT,
		IGRAPHICS_SHADER_TYPE_COMPUTE,
		IGRAPHICS_SHADER_TYPE_UNKNOWN,
		IGRAPHICS_SHADER_TYPES_COUNT,
	};	// enum IGraphicsShaderType
	
	/*!
	 * Flags for the GPU shader.
	 */
	//! @{
	enum IGraphicsShaderFlags : Int32
	{
		IGRAPHICS_SHADER_FLAGS_GENERATE_BYTECODE         = GD_BIT(0),
		IGRAPHICS_SHADER_FLAGS_FORCE_COMPILE_FROM_SOURCE = GD_BIT(1),
	};	// enum IGraphicsShaderFlags
	GD_ENUM_DEFINE_FLAG_OPERATORS(IGraphicsShaderFlags)
	//! @}

	/*!
	 * Description of a GPU shader.
	 */
	struct IGraphicsShaderCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
		IGraphicsShaderType  ShaderType;			//!< Shader type. Could be unspecified, internal usage.
		IGraphicsShaderFlags ShaderFlags;			//!< Flags of the shader.
		CHandle				 ShaderByteCode;		//!< Pointer to the data of the shader Byte code.
		SizeTp				 ShaderByteCodeLength;	//!< Length of the shader Byte code data in bytes.
		CStr				 ShaderSource;			//!< Pointer to the data of the shader source.
	};	// struct IGraphicsShaderCreateInfo

	GD_DEFINE_IGRAPHICS_BASE_OBJECT(IGraphicsShader, IGraphicsObject);

	// ------------------------------------------------------------------------------------------
	// Vertex, Tessellation, Geometry and Fragment shaders.
	// ------------------------------------------------------------------------------------------

	/*! @copydoc IGraphicsShaderCreateInfo */
	typedef IGraphicsShaderCreateInfo IGraphicsVertexShaderCreateInfo;
	/*! @copydoc IGraphicsShaderCreateInfo */
	typedef IGraphicsShaderCreateInfo IGraphicsTessControlShaderCreateInfo;
	/*! @copydoc IGraphicsShaderCreateInfo */
	typedef IGraphicsShaderCreateInfo IGraphicsTessEvaluationShaderCreateInfo;
	/*! @copydoc IGraphicsShaderCreateInfo */
	typedef IGraphicsShaderCreateInfo IGraphicsGeometryShaderCreateInfo;
	/*! @copydoc IGraphicsShaderCreateInfo */
	typedef IGraphicsShaderCreateInfo IGraphicsFragmentShaderCreateInfo;

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsVertexShader, IGraphicsShader);
	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsTessControlShader, IGraphicsShader);
	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsTessEvaluationShader, IGraphicsShader);
	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsGeometryShader, IGraphicsShader);
	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsFragmentShader, IGraphicsShader);

	/*!
	 * Adds Vertex shaders support to the zero IGraphics interface.
	 */
	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseWithVertexShaders, IGraphics)
	{
	public:
		
		/*!
		 * Creates a GPU object with specified parameters.
		 *
		 * @param gfxObjPtr Reference to the output.
		 * @param gfxObjCreateInfo Reference to object create information.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_BufferCreate(IGraphicsBuffer*& gfxObjPtr
			, IGraphicsBufferCreateInfo const& gfxObjCreateInfo) GD_PURE_VIRTUAL;
#define GD_IGRAPHICS_CREATE_FUNC(Object) \
		/*! @copydoc GfxImm_BufferCreate */ \
		GDAPI virtual IResult GfxImm_##Object##Create(IGraphics##Object*& gfxObjPtr \
		    , IGraphics##Object##CreateInfo const& gfxObjCreateInfo) GD_PURE_VIRTUAL
        GD_IGRAPHICS_CREATE_FUNC(Texture1D);
		GD_IGRAPHICS_CREATE_FUNC(Texture2D);
		GD_IGRAPHICS_CREATE_FUNC(Texture3D);

		/*!
		 * Destroys the GPU object.
		 *
		 * @param gfxObj Object to destroy.
		 * @returns Non-negative value if the operation succeeded.
		 */
		//! @{
		GDAPI virtual IResult GfxImm_Texture1DDestroy(IGraphicsTexture1D* const gfxObj) GD_PURE_VIRTUAL;
		GDAPI virtual IResult GfxImm_Texture2DDestroy(IGraphicsTexture2D* const gfxObj) GD_PURE_VIRTUAL;
		GDAPI virtual IResult GfxImm_Texture3DDestroy(IGraphicsTexture3D* const gfxObj) GD_PURE_VIRTUAL;
		//! @}

		/*!
		 * Destroys the GPU 2D texture.
		 *
		 * @param gfxTexture2D 2D texture to destroy.
		 * @returns Non-negative value if the operation succeeded.
		 */

		/*!
		 * Maps all GPU's 2D texture data on the CPU memory.
		 *
		 * @param gfxTexture2D 2D texture to map.
		 * @param gfxMapMode 2D texture map mode.
		 * @param gfxMappedMemory Pointer to the memory handle.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_Texture2DMap(IGraphicsTexture2D* const gfxTexture2D
			, IGraphicsResourceMapMode const gfxMapMode, Handle& gfxMappedMemory) GD_PURE_VIRTUAL;

		/*!
		 * Unmaps the buffer from the CPU memory.
		 *
		 * @param gfxBuffer Buffer to unmap.
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_TextureUnmap(IGraphicsTexture2D* const gfxTexture2D) GD_PURE_VIRTUAL;

		/*!
		 * Creates a GPU sampler state with specified parameters.
		 *
		 * @param gfxSamplerPtr Pointer for output.
		 * @param gfxSamplerCreateInfo Pointer to the sampler state creation information.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_SamplerCreate(IGraphicsSampler*& gfxSamplerPtr
			, IGraphicsSamplerCreateInfo const& gfxSamplerCreateInfo) GD_PURE_VIRTUAL;

		/*!
		 * Destroys the GPU sampler state.
		 *
		 * @param gfxSampler Sampler state to destroy.
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_SamplerDestroy(IGraphicsSampler* const gfxSampler) GD_PURE_VIRTUAL;

		/*!
		 * Creates a new Vertex GPU shader with specified parameters and specifies the input layout
		 * of the Vertex GPU shader.
		 *
		 * @param gfxVertexShaderPtr Pointer for output.
		 * @param gfxShaderCreateInfo Creation information for the Vertex shader.
		 * @param gfxVertexArrayLayout input layout of the vertex shader input data.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
			, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified uniform buffers into Vertex shader input. Layout of the buffers in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxUniformBuffers Pointer to the uniform buffers list.
		 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
		 */
		GDAPI virtual void GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified resources into Vertex shader input. Layout of the resources in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxResources Pointer to the resources list.
		 * @param gfxResourcesCount Number of the resources to bind.
		 */
		GDAPI virtual void GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified samplers into Vertex shader input. Layout of the resources in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxSamplers Pointer to the samplers list.
		 * @param gfxSamplersCount Number of the resources to bind.
		 */
		GDAPI virtual void GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) GD_PURE_VIRTUAL;

		/*!
		 * Creates a new Pixel GPU shader with specified parameters and specifies the input layout
		 * of the Pixel GPU shader.
		 *
		 * @param gfxPixelShaderPtr Pointer for output.
		 * @param gfxShaderCreateInfo Creation information for the Pixel shader.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_PixelShaderCreate(IGraphicsFragmentShader** const gfxPixelShaderPtr
			, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified uniform buffers into Pixel shader input. Layout of the buffers in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxUniformBuffers Pointer to the uniform buffers list.
		 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
		 */
		GDAPI virtual void GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified resources into Pixel shader input. Layout of the resources in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxResources Pointer to the resources list.
		 * @param gfxResourcesCount Number of the resources to bind.
		 */
		GDAPI virtual void GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified samplers into Pixel shader input. Layout of the resources in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxSamplers Pointer to the samplers list.
		 * @param gfxSamplersCount Number of the resources to bind.
		 */
		GDAPI virtual void GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) GD_PURE_VIRTUAL;

		/*!
		 * Creates a new Geometry GPU shader with specified parameters and specifies the input layout
		 * of the Geometry GPU shader.
		 *
		 * @param gfxGeometryShaderPtr Pointer for output.
		 * @param gfxShaderCreateInfo Creation information for the Geometry shader.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
			, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified uniform buffers into Geometry shader input. Layout of the buffers in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxUniformBuffers Pointer to the uniform buffers list.
		 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
		 */
		GDAPI virtual void GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified resources into Geometry shader input. Layout of the resources in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxResources Pointer to the resources list.
		 * @param gfxResourcesCount Number of the resources to bind.
		 */
		GDAPI virtual void GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
			, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified samplers into Geometry shader input. Layout of the resources in
		 * shader and in parameters should be preserved.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxSamplers Pointer to the samplers list.
		 * @param gfxSamplersCount Number of the resources to bind.
		 */
		GDAPI virtual void GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
			, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount) GD_PURE_VIRTUAL;

	};	// class IGraphicsBaseWithGeometryShaders

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                          IGraphicsVertexArray interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * List of supported vertex/shader flow semantics.
	 */
	enum IGraphicsSemanticType : UInt32
	{
		IGRAPHICS_SEMANTIC_POSITION,
		IGRAPHICS_SEMANTIC_NORMAL,
		IGRAPHICS_SEMANTIC_TANGENT,
		IGRAPHICS_SEMANTIC_BINORMAL,
		IGRAPHICS_SEMANTIC_TEXCOORD,
		IGRAPHICS_SEMANTIC_BLENDINDICES,
		IGRAPHICS_SEMANTIC_BLENDWEIGHT,
		IGRAPHICS_SEMANTIC_COLOR,
		IGRAPHICS_SEMANTIC_UNKNOWN,
		IGRAPHICS_SEMANTICS_COUNT
	};	// enum IGraphicsSemanticType

	/*!
	 * List of the supported vertex slots classes.
	 */
	enum IGraphicsInputSlotClass : UInt32
	{
		IGRAPHICS_INPUT_SLOT_CLASS_PER_VERTEX,
		IGRAPHICS_INPUT_SLOT_CLASS_PER_INSTANCE,
		IGRAPHICS_INPUT_SLOT_CLASS_PER_UNKNOWN,
		IGRAPHICS_INPUT_SLOT_CLASSES_COUNT,
	};	// enum IGraphicsInputSlotClass

	/*!
	 * Helper struct that describes the single slot of the vertex array input data.
	 */
	struct IGraphicsVertexArrayLayoutSlot final
	{
	public:
		IGraphicsFormat         SlotFormat;			//!< Format of the slot data.
		IGraphicsSemanticType   SlotSemantic;		//!< The semantic of the slot.
		Int32					SlotSemanticIndex;	//!< Index of the semantic.	
		IGraphicsInputSlotClass SlotClass;			//!< Class of this slot: per vertex or instance, etc.
		SizeTp                  SlotAlignedOffset;	//!< Offset (in bytes) between each element.
		UInt32					SlotDataStepRate;	//!< Number of the instances using same per-instance data.
	};	// struct IGraphicsVertexArrayLayoutSlot

	/*!
	 * Layout for the shader input/vertex array data.
	 */
	struct IGraphicsVertexArrayLayout final
	{
	public:
		IGraphicsVertexArrayLayoutSlot   LayoutSlots[16];	//!< All slots of our input layout.
		SizeTp                           LayoutSlotsCount;	//!< Number of the slots, used by this layout.
	};	// struct IGraphicsVertexArrayLayout 

	/*!
	 * Structure that contains information that is required to create a GPU vertex array.
	 */
	struct IGraphicsVertexArrayCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
		IGraphicsVertexArrayLayout ArrayLayout;	    //!< The layout of the current input data.
		IGraphicsBuffer const*     ArrayVertexBuffers[16];	//!< Vertex buffers list of the current input data.
		IGraphicsBuffer const*	   ArrayIndexBuffer;		//!< Index buffer of the current input data.
	};	// struct IGraphicsVertexArrayCreateInfo

	// **------------------------------------------------------------------------------------------**
	//! GPU input data interface.
	// **------------------------------------------------------------------------------------------**
	class IGraphicsVertexArray : public IGraphicsObject, public IGraphicsVertexArrayCreateInfo
	{
	protected:
		GDINL explicit IGraphicsVertexArray(IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo) 
			: IGraphicsObject(*gfxVertexArrayCreateInfo), IGraphicsVertexArrayCreateInfo(*gfxVertexArrayCreateInfo) {}
	
	public:
		// No public interface functions here..
	};	// class IGraphicsVertexArray
	
	// **------------------------------------------------------------------------------------------**
	//! Adds vertex arrays support to the zero IGraphics interface.
	// **------------------------------------------------------------------------------------------**
	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseWithVertexArrays, IGraphics)
	{
	public:

		/*!
		 * Creates a GPU vertex array with specified parameters.
		 *
		 * @param gfxVertexArrayPtr Pointer for output.
		 * @param gfxVertexArrayCreateInfo Pointer to the vertex array creation information.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_VertexArrayCreate(IGraphicsVertexArray** const gfxVertexArrayPtr
			, IGraphicsVertexArrayCreateInfo const* const gfxVertexArrayCreateInfo) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified GPU vertex array into active GPU vertex array.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxVertexArray Pointer to the vertex array.
		 */
		GDAPI virtual void GfxCmd_VertexArrayBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsVertexArray* const gfxVertexArray) GD_PURE_VIRTUAL;

	};	// class IGraphicsBaseWithVertexArrays

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Structure that contains information that is required to create a GPU pipeline.
	 */
	struct IGraphicsPipelineStateCreateInfo : public IGraphicsObjectCreateInfo
	{
	public:
		IGraphicsVertexShader   const* PipelineVertexShader;	//!< Vertex shader of this pipeline.
		IGraphicsFragmentShader const* PipelinePixelShader;		//!< Pixel shader of this pipeline.
		IGraphicsGeometryShader const* PipelineGeometryShader;	//!< Geometry shader of this pipeline.
	};	// struct IGraphicsPipelineStateCreateInfo

	GD_DEFINE_IGRAPHICS_OBJECT(IGraphicsPipelineState, IGraphicsObject);
	
	/*!
	 * Adds command lists support to the zero IGraphics interface.
	 */
	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseWithPipelineStates, IGraphics)
	{
	public:

		/*!
		 * Creates a new GPU pipeline with specified parameters.
		 *
		 * @param gfxPipelinePtr Pointer for output.
		 * @param gfxPipelineCreateInfo Creation information for the pipeline.
		 *
		 * @returns Non-negative value if the operation succeeded.
		 */
		GDAPI virtual IResult GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
			, IGraphicsPipelineStateCreateInfo const* const gfxPipelineCreateInfo) GD_PURE_VIRTUAL;

		/*!
		 * Binds specified GPU pipeline into active GPU pipeline.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxPipeline Pointer to the pipeline.
		 */
		GDAPI virtual void GfxCmd_PipelineBind(IGraphicsCommandList* const gfxCommandList
			, IGraphicsPipelineState* const gfxPipeline) GD_PURE_VIRTUAL;

	};	// class IGraphicsBaseWithPipelineStates

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Render targets.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * List of the supported topologies.
	 */
	enum IGraphicsTopologyType : UInt32
	{
		IGRAPHICS_SHADER_TOPOLOGY_POINTS,
		IGRAPHICS_SHADER_TOPOLOGY_LINE_STRIP,
		IGRAPHICS_SHADER_TOPOLOGY_LINES,
		IGRAPHICS_SHADER_TOPOLOGY_TRIANGLE_STRIP,
		IGRAPHICS_SHADER_TOPOLOGY_TRIANGLES,
		IGRAPHICS_SHADER_TOPOLOGY_UNKNOWN,
		IGRAPHICS_SHADER_TOPOLOGIES_COUNT,
	};	// enum IGraphicsShaderType

	/*!
	 * Adds command lists support to the zero IGraphics interface.
	 */
	GD_DEFINE_PARTIAL_CLASS(IGraphicsBaseWithRenderTargets, IGraphics)
	{

		/*!
		 * Clears current render target depth buffer.
		 * @param gfxCommandList Command list into which this command would be written.
		 */
		GDAPI virtual void GfxCmd_RenderTargetClearDepth(IGraphicsCommandList* const gfxCommandList) GD_PURE_VIRTUAL;

		/*!
		 * Clear current render target with a specified color.
		 *
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxClearColor Color that would be used to fill the render target.
		 */
		GDAPI virtual void GfxCmd_RenderTargetClearColor(IGraphicsCommandList* const gfxCommandList, GeomColor const gfxClearColor) GD_PURE_VIRTUAL;

		/*!
		 * Renders currently binded vertex array with the binded pipeline state.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxTopology Topology of the binded vertex array.
		 * @param gfxVerticesCount Amount of the vertices in the vertex array.
		 */
		GDAPI virtual void GfxCmd_RenderTargetRender(IGraphicsCommandList* const gfxCommandList, IGraphicsTopologyType const gfxTopology
			, SizeTp const gfxVerticesCount) GD_PURE_VIRTUAL;

		/*!
		 * Renders currently binded indexed vertex array with the binded pipeline state.
		 * 
		 * @param gfxCommandList Command list into which this command would be written.
		 * @param gfxTopology Topology of the binded vertex array.
		 * @param gfxIndicesCount Amount of the indeces in the index array.
		 * @param gfxIndexType Type of the single index.
		 */
		GDAPI virtual void GfxCmd_RenderTargetRenderIndexed(IGraphicsCommandList* const gfxCommandList, IGraphicsTopologyType const gfxTopology
			, SizeTp const gfxIndicesCount, IGraphicsFormatType gfxIndexType) GD_PURE_VIRTUAL;

	};	// class IGraphicsBaseWithRenderTargets

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global interface.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * GPU-accelerated graphics interface.
	 */
	GD_DEFINE_PARTIAL_CLASS(IGraphics, IGraphics)
	{
	protected:
		IGraphicsCanvasMode         m_GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_UNKNOWN;
		IGraphicsCanvasVsyncMode	m_GfxVsyncMode  = IGRAPHICS_OUTPUT_VSYNC_UNKNOWN;
		GeomResolution const*		m_GfxResolutionSelected = nullptr;
		Vector<GeomResolution>		m_GfxResolutionsList;
	};	// class IGraphics

	// ------------------------------------------------------------------------------------------
	//! Global graphics interface pointer.
	GDAPI extern IGraphics* Graphics;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/PlatformSpecificInclude.h>
#if GD_PLATFORM_API_LIBSDL1 || GD_PLATFORM_API_LIBSDL2
#	include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Engine/Interface/Graphics, Graphics)
#else // if GD_PLATFORM_API_LIBSDL1 || GD_PLATFORM_API_LIBSDL2
#	include GD_PLATFORM_INCLUDE(GoddamnEngine/Engine/Interface/Graphics, Graphics)
#endif	// if GD_PLATFORM_API_LIBSDL1 || GD_PLATFORM_API_LIBSDL2
