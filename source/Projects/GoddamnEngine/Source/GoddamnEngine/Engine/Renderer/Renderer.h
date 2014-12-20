/// ==========================================================================================
/// Renderer.h - <p>Hardcore</p> Hardware Rendering Interface (HRI).
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2014 - Created by James Jhuighuy
///     * 16.05.2014 - Some staff got fixed.
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_HARDWARE_RENDERING_INTERFACE
#define GD_ENGINE_HARDWARE_RENDERING_INTERFACE

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Format/Format.h>
#include <GoddamnEngine/Core/Containers/Pointer/RefPtr.h>
#include <GoddamnEngine/Core/Diagnostics/Exception/Exception.h>
#include <GoddamnEngine/Engine/Plugin/Plugin.h>
#include <GoddamnEngine/Engine/Application/Application.h>

#define GD_HRI_DYNAMIC 1
#define GD_HRI_MAX_RENDER_TARGETS 10

GD_NAMESPACE_BEGIN

	struct HRIShaderCtorInfo;
	class HRIShaderParamLocationDesc;
	class HRIShaderParamLocation;
	class HRIShaderParamDesc;
	class HRIShaderParam;
	class HRIShaderInstanceDesc;
	class HRIShaderInstance;
	class HRIShaderProgram;
	class HRIShader;
	class HRIComputeShader;
	class HRIGeometryShader;
	class HRIVertexShader;
	class HRIHullShader;
	class HRIDomainShader;
	class HRIPixelShader;

	enum HRIDriverType : UInt8
	{
		GD_HRI_DRIVER_D3D11,
		GD_HRI_DRIVER_OGL430,
		GD_HRI_DRIVER_OGLES2,
		GD_HRI_DRIVER_OGLES3,
	};	// enum HRIDriverType

	/// Describes support of Vertical Synchronization (VSync) by hardware
	enum HRIVSyncDescription : UInt8
	{
		GD_HRI_VSYNC_MODE_UNKNOWN            = 0,
		GD_HRI_VSYNC_MODE_HARDWARE_SUPPORTED = GD_BIT(0),	///< Is VSync supported by hardware
		GD_HRI_VSYNC_MODE_SOFTWARE_SUPPORTED = GD_BIT(1),	///< Is custom VSync implemented in renderer 
		GD_HRI_VSYNC_MODE_IS_ENABLED		 = GD_BIT(2),	///< Is VSync enabled
	};	// HRIVSyncDescription

	/// Describes of fullscreen/windowed mode by hardware / OS
	enum HRIScreenModeDescription : UInt8
	{
		GD_HRI_SCREEN_MODE_UNKNOWN              = 0,
		GD_HRI_SCREEN_MODE_FULLSCREEN_SUPPORTED = GD_BIT(0),
		GD_HRI_SCREEN_MODE_WINDOWED_SUPPORTED	= GD_BIT(1),
		GD_HRI_SCREEN_MODE_IS_FULLSCREEN		= GD_BIT(2),
	};	// enum HRIScreenModeDescription

	/// List of semantics used by engine.
	enum HRISemantic : UInt8
	{
		GD_HRI_SEMANTIC_POSITION,      ///< Semantic for Vertex position
		GD_HRI_SEMANTIC_NORMAL,		   ///< Semantic for Normals
		GD_HRI_SEMANTIC_TANGENT,       ///< Semantic for tangents of vertices
		GD_HRI_SEMANTIC_BINORMAL,      ///< Semantic for Bitangents/Binormals
		GD_HRI_SEMANTIC_TEXCOORD,      ///< Semantic for texture coordinates (UV coordinates)
		GD_HRI_SEMANTIC_BLENDINDICES,  ///< Semantic for Blending indices
		GD_HRI_SEMANTIC_BLENDWEIGHT,   ///< Semantic for Blending masses of bones
		GD_HRI_SEMANTIC_COLOR,         ///< Semantic for Vertex colors
		GD_HRI_SEMANTIC_UNKNOWN,	   ///< Semantic for texture coordinates (UV coordinates)
		GD_HRI_SEMANTIC_FIRST          = GD_HRI_SEMANTIC_POSITION,
		GD_HRI_SEMANTICS_COUNT         = GD_HRI_SEMANTIC_UNKNOWN
	};	// enum HRISemantic

	/// Describes types of buffers
	enum HRIBufferType : UInt8
	{
		GD_HRI_BUFFER_TYPE_VERTEX,	 ///< Buffer contains vertex description data
		GD_HRI_BUFFER_TYPE_INDEX,	 ///< Buffer contains vertex indices data
		GD_HRI_BUFFER_TYPE_CONSTANT, ///< Buffer contains data for constant buffer
		GD_HRI_BUFFER_TYPE_UNKNOWN,	 ///< Unknown buffer
		GD_HRI_BUFFER_TYPES_COUNT    = GD_HRI_BUFFER_TYPE_UNKNOWN,
	};	// enum HRIBufferType

	/// Indicates about runtime error caused by renderer interface.
	GD_DEFINE_EXCPETION(HRIException);

	/// ==========================================================================================
	/// Semantics
	/// ==========================================================================================
	
	struct HRISemanticDesc final
	{
		Str    Name;
		Format SlotFormat;
	};	// struct HRISemanticDesc

	GDAPI extern HRISemanticDesc const& HRISemanticGetDesc(HRISemantic const Semantic);

	/// ==========================================================================================
	/// Buffers
	/// ==========================================================================================

	/// Provides data interaction with GPU.
	class HRIBuffer : public HRIObject
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIBuffer, HRIObject, GDAPI);
		HRIBufferType BufferType = GD_HRI_BUFFER_TYPE_UNKNOWN;
	protected:
		GDINL explicit HRIBuffer(HRIBufferType const BufferType) : BufferType(BufferType) { }
		GDINL virtual ~HRIBuffer(                              )                          { }
	public:
		GDINL HRIBufferType  GetType()          const { return this->BufferType; }
		GDAPI virtual handle GetNativePointer() const abstract;
	};	// class HRIBuffer

	/// Provides interaction of Vertex data with GPU.
	class HRIVertexBuffer : public HRIBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIVertexBuffer, HRIBuffer, GDAPI);
		Float32 const* Data = nullptr;
		size_t         Size = 0;
	protected:
		GDINL explicit HRIVertexBuffer(Float32 const* const Data, size_t const Size) : HRIBuffer(GD_HRI_BUFFER_TYPE_VERTEX), Data(Data), Size(Size) { }
		GDINL virtual ~HRIVertexBuffer(                                            ) { }
	public:
		GDINL Float32 const* GetData() const { return this->Data; }
		GDINL size_t         GetLength() const { return this->Size; }
	};	// class HRIVertexBuffer

	/// Provides interaction of Index data with GPU.
	class HRIIndexBuffer : public HRIBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIIndexBuffer, HRIBuffer, GDAPI);
		chandle Data = nullptr;
		size_t  Size   = 0;
		size_t  Stride = 2;
	protected:
		GDINL explicit HRIIndexBuffer(chandle const Data, size_t const Size, size_t const Stride) : HRIBuffer(GD_HRI_BUFFER_TYPE_VERTEX), Data(Data), Size(Size), Stride(Stride) { }
		GDINL virtual ~HRIIndexBuffer(                                                          ) { }
	public:
		GDINL chandle GetData  () const { return this->Data; }
		GDINL size_t  GetSize  () const { return this->Size; }
		GDINL size_t  GetStride() const { return this->Stride; }
	};	// class HRIIndexBuffer

	/// Provides interaction of constant buffers data with GPU.
	class HRIConstantBuffer : public HRIBuffer
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIConstantBuffer, HRIBuffer, GDAPI);
		size_t Size = 0;
	protected:
		GDINL explicit HRIConstantBuffer(size_t const Size) : HRIBuffer(GD_HRI_BUFFER_TYPE_CONSTANT), Size(Size) { }
		GDINL virtual ~HRIConstantBuffer(                 ) { }
	public:
		GDINL size_t GetLength() const { return this->Size; }
		GDAPI virtual void CopyDataTo  ( handle const Data) const abstract;
		GDAPI virtual void CopyDataFrom(chandle const Data)       abstract;
	};	// class HRIConstantBuffer

	/// ==========================================================================================
	/// Shapes.
	/// ==========================================================================================

	/// Describes type of topology shape uses
	enum HRITopologyType : UInt8
	{
		GD_HRI_TOPOLOGY_TYPE_TRIANGLE_LIST, ///< Topology is built using trianles.
		GD_HRI_TOPOLOGY_TYPE_UNKNOWN,
		GD_HRI_TOPOLOGY_TYPES_COUNT = GD_HRI_TOPOLOGY_TYPE_UNKNOWN,
	};	// enum HRITopologyType

	/// Provides uploading to GPU custom shapes.
	class HRIIndexedShape : public HRIObject
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIIndexedShape, HRIObject, GDAPI);
		StackVector<RefPtr<HRIVertexBuffer>, GD_HRI_SEMANTICS_COUNT> VertexBuffers;
		RefPtr<HRIIndexBuffer> IndexBuffer;
		HRITopologyType TopologyType;

	protected:
		GDINL explicit HRIIndexedShape() : VertexBuffers(GD_HRI_SEMANTICS_COUNT) { }
		GDINL virtual ~HRIIndexedShape() { }
	public:
		GDAPI virtual HRITopologyType GetTopologyType(                                  ) const { return this->TopologyType;                }
		GDAPI virtual void            SetTopologyType(HRITopologyType const TopologyType)       {        this->TopologyType = TopologyType; }
		GDAPI virtual void SetVertexData(HRISemantic const Semantic, Float32 const* const Data, size_t const Size                     );
		GDAPI virtual void SetIndexData (                            chandle        const Data, size_t const Size, size_t const Stride);
		GDAPI virtual HRIIndexBuffer  const* GetIndexBuffer() const { return this->IndexBuffer.GetPointer(); }
		GDAPI virtual HRIIndexBuffer       * GetIndexBuffer()       { return this->IndexBuffer.GetPointer(); }
		GDAPI virtual HRIVertexBuffer const* GetVertexBuffer(HRISemantic const Semantic) const 
		{ 
			return this->VertexBuffers[static_cast<size_t>(Semantic)].GetPointer(); 
		}
		
		GDAPI virtual HRIVertexBuffer* GetVertexBuffer(HRISemantic const Semantic)       
		{ 
			return const_cast<HRIVertexBuffer*>(const_cast<HRIIndexedShape const*>(this)->GetVertexBuffer(Semantic)); 
		}
	};	// class HRIIndexedShape

	/// ==========================================================================================
	/// Dynamic Shape-Shader linkage.
	/// ==========================================================================================

	/// Provides dynamic linkage of shader program and shape.
	class HRILinkagePoint : public HRIObject
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRILinkagePoint, HRIObject, GDAPI);
	protected:
		GDINL explicit HRILinkagePoint() { }
		GDINL virtual ~HRILinkagePoint() { }
	public:
		GDAPI virtual HRIShaderProgram const* GetShaderProgram() const abstract;
		GDAPI virtual HRIIndexedShape  const* GetIndexedShape () const abstract;
		GDINL         HRIShaderProgram      * GetShaderProgram() { return const_cast<HRIShaderProgram*>(const_cast<HRILinkagePoint const*>(this)->GetShaderProgram()); }
		GDINL         HRIIndexedShape       * GetIndexedShape () { return const_cast<HRIIndexedShape *>(const_cast<HRILinkagePoint const*>(this)->GetIndexedShape ()); }
		GDAPI virtual void SetShaderProgram(HRIShaderProgram const* const Effect) abstract;
		GDAPI virtual void SetIndexedShape (HRIIndexedShape  const* const IndexedShape ) abstract;
		GDAPI virtual void RenderSelf() const abstract;
	};	// class HRILinkagePoint

	/// ==========================================================================================
	/// Textures.
	/// ==========================================================================================

	/// Describes modes of creating texture.
	enum HRITexture2DMode : UInt8
	{
		GD_HRI_TEXTURE_2D_MODE_STATIC,		        ///< This texture is statically created from data.
		GD_HRI_TEXTURE_2D_MODE_DYNAMIC,	            ///< Read-write texture. 
		GD_HRI_TEXTURE_2D_MODE_RenderTarget,         ///< Texture that can be binded to frame buffer.
		GD_HRI_TEXTURE_2D_MODE_UNKNOWN,		        ///< Unknown texture mode (internal usage only).
		GD_HRI_TEXTURE_2D_MODES_COUNT		        = GD_HRI_TEXTURE_2D_MODE_UNKNOWN,
	};	// enum HRITexture2DMode

	/// Describes texture filtering type.
	enum HRITexture2DFilteringMode : UInt8
	{
		GD_HRI_TEXTURE_2D_FILTERING_MODE_POINT,		///< Simplest filtering mode.
		GD_HRI_TEXTURE_2D_FILTERING_MODE_BILINEAR,	///< Filtering is done within bilinear interpolation.
		GD_HRI_TEXTURE_2D_FILTERING_MODE_TRILINEAR,	///< Filtering is done within trilinear interpolation.
		GD_HRI_TEXTURE_2D_FILTERING_MODE_UNKNOWN,	///< Unknown texture filtering mode (internal usage only).
		GD_HRI_TEXTURE_2D_FILTERING_MODES_COUNT		= GD_HRI_TEXTURE_2D_FILTERING_MODE_UNKNOWN,
	};	// enum HRITexture2DFilteringMode

	/// Describes texture wrap mode.
	enum HRITexture2DWrapMode : UInt8
	{
		GD_HRI_TEXTURE_2D_WRAP_MODE_REPEAT,         ///< Texture would be tiled if UV goes out of bounds.
		GD_HRI_TEXTURE_2D_WRAP_MODE_CLAMP,          ///< UV would be clamped in [0, 1].
		GD_HRI_TEXTURE_2D_WRAP_MODE_UNKNOWN,        ///< Unknown texture warp mode (internal usage only).
		GD_HRI_TEXTURE_2D_WRAP_MODES_COUNT          = GD_HRI_TEXTURE_2D_WRAP_MODE_UNKNOWN,
	};	// enum HRITexture2DWrapMode

	/// Describes single texture.
	class HRITexture2DCtorInfo
	{
	private:
		GD_CLASS_UNASSIGNABLE(HRITexture2DCtorInfo);
	public:
		Resolution                const TheResolution; ///< Resolution of this texture
		Format                    const PixelFormat;   ///< Pixel format of this texture
		HRITexture2DMode          const Mode;		   ///< Type of texture
		HRITexture2DWrapMode      const WrapMode;	   ///< Texture wrapping mode, specifies how UVs would behave when come out ouf bounds
		HRITexture2DFilteringMode const FilteringMode; ///< Texture filtering mode, specifies support of per-pixel interpolation and mip-mapping
		GDINL HRITexture2DCtorInfo(HRITexture2DCtorInfo const& CtorInfo);
		GDINL HRITexture2DCtorInfo(
			Resolution                const& TheResolution,
			Format                    const  PixelFormat   = GD_FORMAT_R8G8B8A8_UINT,
			HRITexture2DMode          const  Mode          = GD_HRI_TEXTURE_2D_MODE_DYNAMIC,
			HRITexture2DWrapMode      const  WrapMode	   = GD_HRI_TEXTURE_2D_WRAP_MODE_CLAMP,
			HRITexture2DFilteringMode const  FilteringMode = GD_HRI_TEXTURE_2D_FILTERING_MODE_POINT
		);
	};	// class HRITexture2DCtorInfo

	/// Basic interface for 2D textures.
	class HRITexture2D : public HRIObject, public HRITexture2DCtorInfo
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRITexture2D, HRIObject, GDAPI);
	protected:
		GDINL virtual ~HRITexture2D() { }
		GDINL explicit HRITexture2D(HRITexture2DCtorInfo const& CtorInfo, chandle const InitialData, size_t const InitialDataLength) 
			: HRITexture2DCtorInfo(CtorInfo) 
		{
			GD_UNUSED(InitialData);
			GD_UNUSED(InitialDataLength);
		}
	public:
		GDAPI virtual handle GetNativePointer() const abstract;
	};	// class HRITexture2D

	class HRITextureCube;
	class HRITextureCubeCtorInfo;

	/// ==========================================================================================
	/// Render target.
	/// ==========================================================================================

	class HRIRenderTargetCtorInfo
	{
	private:
		GD_CLASS_UNASSIGNABLE(HRIRenderTargetCtorInfo);

	public:
		Resolution const RenderTargetResolution;
		StackVector<HRITexture2D const*, 10> RenderTargetTextures;

		GDAPI ~HRIRenderTargetCtorInfo();
		GDAPI  HRIRenderTargetCtorInfo(HRIRenderTargetCtorInfo const& gpuRenderTargetCtorInfo);
		GDAPI  HRIRenderTargetCtorInfo(Resolution const& RenderTargetResolution, HRITexture2D const* const firstTexture = nullptr, ...);
	};	// class HRIRenderTargetCtorInfo

	/// Interface to native frame buffer.
	class HRIRenderTarget : public HRIObject, public HRIRenderTargetCtorInfo
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIRenderTarget, HRIObject, GDAPI);
	protected:
		GDINL explicit HRIRenderTarget(HRIRenderTargetCtorInfo const& CtorInfo) : HRIRenderTargetCtorInfo(CtorInfo) { }
		GDINL virtual ~HRIRenderTarget(                                      ) { }
	public:
		GDAPI virtual handle GetNativePointer() const abstract;
		GDAPI virtual void   BindRenderTarget() const abstract;
		GDAPI virtual void UnbindRenderTarget() const abstract;
	};	// class HRIRenderTarget

	/// ==========================================================================================
	/// Hardware Rendering Interface (HRI).
	/// ==========================================================================================

#if (defined(GD_HRI_DYNAMIC))
	/// Hardware Rendering Interface (HRI).
	class HRInterface : public Plugin<GD_PLUGIN_TYPE_RENDERER>, public Singleton<HRInterface>
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRInterface, Object, GDAPI);

	protected:
		GDINL explicit HRInterface() : ContextRectagle(Application::GetInstance().GetApplicationGameWindow()->GetWindowRectangle()) { }
		GDINL virtual ~HRInterface() { }

	public:
		Rectangle ContextRectagle; ///< Reference to low level screen resolution

	public /*Class API*/:
		GDINL bool DoesSupportsTesselation() const
		{
			return false;
		}
		GDINL bool DoesSupportsConstantBuffers() const
		{
			return true;
		}

		// Please, add new virtual methods derectrly to this header!
#		define GD_HRI_API  GDAPI virtual
#		define GD_HRI_SPEC abstract
#		include <GoddamnEngine/Engine/Renderer/RendererMethods.h>
	};	// class HRInterface
#else	// if (defined(GD_HRI_DYNAMIC))
#		define GD_HRI_API              GDAPI extern
#		define GD_HRI_NAME_MODIF(Name) HRI##Name
#		include <GoddamnEngine/Engine/Renderer/RendererMethods.h>
#endif	// if (defined(GD_HRI_DYNAMIC))

	GDINL HRITexture2DCtorInfo::HRITexture2DCtorInfo(
		Resolution                const& TheResolution,
		Format                    const  PixelFormat   /* = GD_FORMAT_R8G8B8A8_UINT */,
		HRITexture2DMode          const  Mode          /* = GD_HRI_TEXTURE_2D_MODE_DYNAMIC */,
		HRITexture2DWrapMode      const  WrapMode      /* = GD_HRI_TEXTURE_2D_WRAP_MODE_CLAMP */,
		HRITexture2DFilteringMode const  FilteringMode /* = GD_HRI_TEXTURE_2D_FILTERING_MODE_POINT */)
	 : TheResolution(TheResolution)
	 , PixelFormat  (PixelFormat  )
	 , Mode         (Mode         )
	 , WrapMode     (WrapMode     )
	 , FilteringMode(FilteringMode)
	{
	}

	GDINL HRITexture2DCtorInfo::HRITexture2DCtorInfo(HRITexture2DCtorInfo const& CtorInfo)
	 : TheResolution(CtorInfo.TheResolution)
	 , PixelFormat  (CtorInfo.PixelFormat  )
	 , Mode         (CtorInfo.Mode         )
	 , WrapMode     (CtorInfo.WrapMode     )
	 , FilteringMode(CtorInfo.FilteringMode)
	{
	}

GD_NAMESPACE_END

#endif