/// ==========================================================================================
/// RendererMethods.h - HRI method declarations.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///  * 21.05.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#if (!defined(GD_HRI_API))
#	  define  GD_HRI_API GDINT
#endif	// if (!defined(GD_HRI_API))
#if (!defined(GD_HRI_SPEC))
#	  define  GD_HRI_SPEC
#endif	// if (!defined(GD_HRI_SPEC))
#if (!defined(GD_HRI_IMPL))
#	  define  GD_HRI_IMPL ;
#endif	// if (!defined(GD_HRI_IMPL))
#if (!defined(GD_HRI_NAME_MODIF))
# 	  define  GD_HRI_NAME_MODIF(Name) Name
#endif	// if (!defined(GD_HRI_NAME_MODIF))
#if (!defined(GD_DEFINE_HRI_METHOD))
#	  define  GD_DEFINE_HRI_METHOD(ReturnType, MethodName, ...) GD_HRI_API ReturnType GD_HRI_NAME_MODIF(MethodName)(__VA_ARGS__) GD_HRI_SPEC GD_HRI_IMPL
#endif	// if (!defined(GD_DEFINE_HRI_METHOD))

/// Initializes renderer context.
/// @returns True if initialized successfully.
GD_DEFINE_HRI_METHOD(
	bool, CreateContex,
	void
	)

/// Destroys rendrer context.
/// @returns True if deinitialzed successfully.
GD_DEFINE_HRI_METHOD(
	bool, DestroyContext,
	void
	)

/// Clears all context.
GD_DEFINE_HRI_METHOD(
	void, ClearContext,
	Rectangle const& ClearingViewport,
	Color  const& ClearColor,
	bool   const  DoClearDepth = true
	)

/// Swaps dual screen buffers.
GD_DEFINE_HRI_METHOD(
	void, SwapBuffers,
	void
	)

/// Creates new vertex buffer.
/// @param   Data Pointer to initial vertex data.
/// @param   Size Number of floats in initial data.
/// @returns   New vertex buffer.
GD_DEFINE_HRI_METHOD(
	HRIVertexBuffer*, CreateVertexBuffer,
	Float32 const* const Data, 
	size_t  const  Size
	)

/// Creates new index buffer.
/// @param Data   Pointer to initial vertex data.
/// @param Size	  Number of elements in initial data.
/// @param Stride Size of one element in initial data.
/// @returns   New index buffer.
GD_DEFINE_HRI_METHOD(
	HRIIndexBuffer*, CreateIndexBuffer,
	chandle const Data, 
	size_t  const Size, 
	size_t  const Stride
	)

/// Creates new constant buffer.
/// @param   Size Number of elements in initial data.
/// @returns   New constant buffer.
GD_DEFINE_HRI_METHOD(
	HRIConstantBuffer*, CreateConstantBuffer,
	size_t const Size
	)
		
/// Creates new frame buffer.
/// @param CtorInfo Frame buffer construction information.
/// @returns New frame buffer.
GD_DEFINE_HRI_METHOD(
	HRIRenderTarget*, CreateRenderTarget,
	HRIRenderTargetCtorInfo const& CtorInfo
	)

/// Creates new 2D texture.
/// @param CtorInfo    2D texture construction information.
/// @param InitialData    Pointer to texture`s initial data.
/// @param InitialDataLength Size of textures`s initial data.
/// @returns     New 2D texture.
GD_DEFINE_HRI_METHOD(
	HRITexture2D*, CreateTexture2D,
	HRITexture2DCtorInfo const& CtorInfo, 
	chandle const InitialData, 
	size_t  const InitialDataLength
	)

#if 0
/// Creates new Cube map texture.
/// @param CtorInfo    Cube map construction information.
/// @param InitialData    Pointer to texture`s initial data.
/// @param InitialDataLength Size of textures`s initial data.
/// @returns     New Cube map.
GD_DEFINE_HRI_METHOD(
	HRITextureCube*, CreateTextureCube,
	HRITextureCubeCtorInfo const& CtorInfo, 
	chandle const InitialData, 
	size_t  const InitialDataLength
	)
#endif

/// Creates new HRIShaderProgram-HRIIndexedShape linkage point.
/// @returns New linkage point.
GD_DEFINE_HRI_METHOD(
	HRILinkagePoint*, CreateLinkagePoint,
	void
	)

/// Creates a new renderable shape.
/// @returns New shape.
GD_DEFINE_HRI_METHOD(
	HRIIndexedShape*, CreateIndexedShape,
	void
	)

/// Creates new shader program
/// @returns New shader program
GD_DEFINE_HRI_METHOD(
	HRIShaderProgram*, CreateShaderProgram,
	void
	)

/// Creates instance of shader.
/// @param   InstanceDesc Description of instance.
/// @returns New instance of specified shader.
GD_DEFINE_HRI_METHOD(
	HRIShaderInstance*, CreateShaderInstance,
	HRIShaderInstanceDesc const* const InstanceDesc
	)

/// Creates a new object that can store shader parameters inside constant buffer.
/// @param ShaderInstance Shader instance that would contain this location.
/// @param LocationDesc   Description of this location.
GD_DEFINE_HRI_METHOD(
	HRIShaderParamLocation*, CreateShaderParamLocationConstantBuffer,
	HRIShaderInstance      * const ShaderInstance,
	HRIShaderParamLocationDesc const* const LocationDesc
	)

/// Creates a new object that can store shader resource-parameters.
/// @param ShaderInstance Shader instance that would contain this location.
/// @param LocationDesc   Description of this location.
GD_DEFINE_HRI_METHOD(
	HRIShaderParamLocation*, CreateShaderParamLocationResources,
	HRIShaderInstance      * const ShaderInstance,
	HRIShaderParamLocationDesc const* const LocationDesc
	)

/// Creates new vertex shader.
/// @param CtorInfo Vertex shader construction information.
/// @returns  New vertex shader.
GD_DEFINE_HRI_METHOD(
	HRIVertexShader*, CreateVertexShader,
	HRIShaderCtorInfo const& CtorInfo
	)

/// Creates new hull shader.
/// @param CtorInfo Hull shader construction information.
/// @returns  New hull shader.
GD_DEFINE_HRI_METHOD(
	HRIHullShader*, CreateHullShader,
	HRIShaderCtorInfo const& CtorInfo
	)

/// Creates new domain shader.
/// @param CtorInfo Domain shader construction information.
/// @returns  New domain shader.
GD_DEFINE_HRI_METHOD(
	HRIDomainShader*, CreateDomainShader,
	HRIShaderCtorInfo const& CtorInfo
	)

/// Creates new pixel shader.
/// @param CtorInfo Pixel shader construction information.
/// @returns  New pixel shader.
GD_DEFINE_HRI_METHOD(
	HRIPixelShader*, CreatePixelShader,
	HRIShaderCtorInfo const& CtorInfo
	)

#undef GD_HRI_API
#undef GD_HRI_SPEC
#undef GD_HRI_IMPL
#undef GD_HRI_NAME_MODIF
#undef GD_DEFINE_HRI_METHOD
