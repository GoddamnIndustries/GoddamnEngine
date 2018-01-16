// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_DIRECT3D1X_IMPL 1

/*!
 * @file GoddamnEngine/Core/System/Graphics/GraphicsDirect3D11/GraphicsDirect3D11Textures.cpp
 * File contains Implementation for Direct3D11 4.3 Implementation of the graphics interface: sampler states and textures.
 */
#include <GoddamnEngine/GraphicsDirect3D11/GraphicsDirect3D11.h>
#include <GoddamnEngine/GraphicsDirect3D11/GraphicsDirect3D11FormatDXGI.h>
#if GD_PLATFORM_API_MICROSOFT

GD_NAMESPACE_BEGIN

#if 0

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D11ShaderResourceView);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Sampler state objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D11SamplerState);

#define GD_IGRAPHICS_DIRECT3D1X_COMBINE_MINMAG_FILTERS(min, mag) ((min) | ((mag) << (8 * sizeof(mag))))

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU sampler state with specified parameters.
	//! @param gfxSamplerStatePtr Pointer for output.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	GDAPI IGraphicsDirect3D11SamplerState::IGraphicsDirect3D11SamplerState(IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo)
		: IGraphicsSamplerState(gfxSamplerStateCreationInfo), D3DSamplerState(nullptr)
	{
		D3D11_(SAMPLER_DESC) d3dSamplerState = {};
		CMemory::Memcpy(d3dSamplerState.BorderColor, sizeof(d3dSamplerState.BorderColor), &gfxSamplerStateCreationInfo->SamplerBorderColor);
		d3dSamplerState.AddressU = IGraphicsDirect3D11TextureAddressModesTable[gfxSamplerStateCreationInfo->SamplerAddressModeU];
		d3dSamplerState.AddressV = IGraphicsDirect3D11TextureAddressModesTable[gfxSamplerStateCreationInfo->SamplerAddressModeV];
		d3dSamplerState.AddressW = IGraphicsDirect3D11TextureAddressModesTable[gfxSamplerStateCreationInfo->SamplerAddressModeW];
		d3dSamplerState.MinLOD   = gfxSamplerStateCreationInfo->SamplerLODMinClamp;
		d3dSamplerState.MaxLOD   = gfxSamplerStateCreationInfo->SamplerLODMaxClamp;
		if (gfxSamplerStateCreationInfo->SamplerMaxAnisotropy == 0)
		{
			// Using the specified minification and magnification filtering..
			switch (GD_IGRAPHICS_DIRECT3D1X_COMBINE_MINMAG_FILTERS(gfxSamplerStateCreationInfo->SamplerMinFilter, gfxSamplerStateCreationInfo->SamplerMagFilter))
			{
			case GD_IGRAPHICS_DIRECT3D1X_COMBINE_MINMAG_FILTERS(IGRAPHICS_TEXTURE_MINMAG_FILTER_NEAREST, IGRAPHICS_TEXTURE_MINMAG_FILTER_NEAREST):
				d3dSamplerState.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
				break;
			case GD_IGRAPHICS_DIRECT3D1X_COMBINE_MINMAG_FILTERS(IGRAPHICS_TEXTURE_MINMAG_FILTER_NEAREST, IGRAPHICS_TEXTURE_MINMAG_FILTER_LINEAR):
				d3dSamplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
				break;
			case GD_IGRAPHICS_DIRECT3D1X_COMBINE_MINMAG_FILTERS(IGRAPHICS_TEXTURE_MINMAG_FILTER_LINEAR, IGRAPHICS_TEXTURE_MINMAG_FILTER_NEAREST):
				d3dSamplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				break;
			case GD_IGRAPHICS_DIRECT3D1X_COMBINE_MINMAG_FILTERS(IGRAPHICS_TEXTURE_MINMAG_FILTER_LINEAR, IGRAPHICS_TEXTURE_MINMAG_FILTER_LINEAR):
				d3dSamplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				break;
			default:
				GD_DEBUG_ASSERT_FALSE("Unknown sampler state min mag filtering mode.");
			}
		}
		else
		{
			// Anisotropic filtering requires special filtering being enabled..
			d3dSamplerState.Filter = D3D11_(FILTER_ANISOTROPIC);
			d3dSamplerState.MaxAnisotropy = gfxSamplerStateCreationInfo->SamplerMaxAnisotropy;
		}

		ThrowIfFailed(GD_IGRAPHICS_DIRECT3D1X->D3DDevice->CreateSamplerState(&d3dSamplerState, &D3DSamplerState.p));
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes the specified GPU sampler state.
	GDAPI IGraphicsDirect3D11SamplerState::~IGraphicsDirect3D11SamplerState()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU sampler state with specified parameters.
	//! @param gfxSamplerStatePtr Pointer for output.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D11WithSamplerStates::GfxImm_SamplerStateCreate(IGraphicsSamplerState** const gfxSamplerStatePtr
		, IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_SamplerStateCreate(gfxSamplerStatePtr, gfxSamplerStateCreationInf)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxSamplerStatePtr = GD_NEW(IGraphicsDirect3D11SamplerState, gfxSamplerStateCreationInfo);
		return IResult::Ok;
	}


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes an empty shader resource view.
	GDAPI IGraphicsDirect3D11ShaderResourceView::IGraphicsDirect3D11ShaderResourceView(IGraphicsResourceType const gfxResourceType)
		: D3DShaderResourceView(nullptr)
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes the shader resource view.
	GDAPI IGraphicsDirect3D11ShaderResourceView::~IGraphicsDirect3D11ShaderResourceView()
	{
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D11Texture2D);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU 2D texture with specified parameters.
	//! @param gfxTexture2DCreationInfo Pointer to the 2D texture creation information.
	//! @param gfxTexture2DInitialData Initial data for our texture.
	GDAPI IGraphicsDirect3D11Texture2D::IGraphicsDirect3D11Texture2D(IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
		: IGraphicsTexture2D(gfxTexture2DCreationInfo)
	{
			
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU 2D texture.
	GDAPI IGraphicsDirect3D11Texture2D::~IGraphicsDirect3D11Texture2D()
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Queries the shader resource for current resource.
	//! @returns Pointer to the shader resource view. Should be released manually.
	GDAPI IGraphicsShaderResourceView const* IGraphicsDirect3D11Texture2D::CPU_GetShaderResourceView() const
	{
		return D3DShaderResourceView;
	}

	// ------------------------------------------------------------------------------------------
	//! Updates data stored inside texture.
	//! @param gfxTexture2DData New data that would be uploaded to GPU.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D11Texture2D::Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GLTexture2DID);
			
		GLuint glMipLevelOffset = 0;
		GLuint const glMipLevelsCount = Max(1u, gfxTexture2DCreationInfo.TextureMipMapsCount);
		for (GLuint glMipLevel = 0; glMipLevel < glMipLevelsCount; ++glMipLevel)
		{
			// Computing the dimensions of our mipmap level..
			GLuint const glMipLevelWidth = Max(1u, gfxTexture2DCreationInfo.TextureWidth >> glMipLevel);
			GLuint const glMipLevelHeight = Max(1u, gfxTexture2DCreationInfo.TextureHeight >> glMipLevel);
			if (gfxTexture2DData == nullptr)
			{
				// Even if no initial data was specified, we need to provide pixel formats into the Direct3D11.
				// Specifying null image information for all mipmap levels.
				glTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsDirect3D11FormatConvert(gfxTexture2DCreationInfo.TexturePixelFormat)
					, glMipLevelWidth, glMipLevelHeight, 0
					, IGraphicsDirect3D11FormatGetLayout(gfxTexture2DCreationInfo.TexturePixelFormat)
					, IGraphicsDirect3D11FormatGetType(gfxTexture2DCreationInfo.TexturePixelFormat)
					, nullptr
					);
			}
			else if (gfxTexture2DCreationInfo.TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_RAW)
			{
				// Specifying raw image information for all mipmap levels.
				// See link below how to correctly do it.
				// https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
				GLuint const glMipLevelBlockSize = static_cast<GLuint>(IGraphicsFormatSizeof(gfxTexture2DCreationInfo.TexturePixelFormat));
				glTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsDirect3D11FormatConvert(gfxTexture2DCreationInfo.TexturePixelFormat)
					, glMipLevelWidth, glMipLevelHeight, 0
					, IGraphicsDirect3D11FormatGetLayout(gfxTexture2DCreationInfo.TexturePixelFormat)
					, IGraphicsDirect3D11FormatGetType(gfxTexture2DCreationInfo.TexturePixelFormat)
					, static_cast<Byte const*>(gfxTexture2DData->TextureData) + glMipLevelOffset
					);
				glMipLevelOffset += glMipLevelWidth * glMipLevelHeight * glMipLevelBlockSize;
			}
			else
			{
				GD_DEBUG_ASSERT(false, "Test this correctly!");

				GLuint const glMipLevelBlockSize = gfxTexture2DCreationInfo.TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1 ? 8 : 16;
				GLuint const glMipLevelSize = (glMipLevelWidth + 3) / 4 * (glMipLevelHeight + 3) / 4 * glMipLevelBlockSize;

				// Specifying DXT-compressed image information for all mipmap levels.
				// See link below how to correctly do it.
				// https://www.opengl.org/sdk/docs/man/html/glCompressedTexImage2D.xhtml
				glCompressedTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsDirect3D11TextureCompressionModesTable[gfxTexture2DCreationInfo.TextureCompressionMode]
					, glMipLevelWidth, glMipLevelHeight, 0, glMipLevelSize
					, static_cast<Byte const*>(gfxTexture2DData->TextureData) + glMipLevelOffset
					);
				glMipLevelOffset += glMipLevelSize;
			}

		}
		if (gfxTexture2DCreationInfo.TextureMipMapsCount == 0)
		{
			// We have to also generate mipmaps for our texture.
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		// Cleaning up..
		glBindTexture(GL_TEXTURE_2D, 0);

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU 2D texture with specified parameters.
	//! @param gfxTexture2DPtr Pointer for output.
	//! @param gfxTexture2DInitialData Initial data for our texture.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsBaseDirect3D11_WithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
		, IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_SamplerStateCreate(gfxSamplerStatePtr, gfxSamplerStateCreationInf)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxTexture2DPtr = GD_NEW(IGraphicsDirect3D11Texture2D, gfxTexture2DCreationInfo, gfxTexture2DInitialData);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D11TextureCube);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Cube texture with specified parameters.
	//! @param gfxTextureCubeCreationInfo Pointer to the Cube texture creation information.
	//! @param gfxTextureCubeInitialData Initial data for our texture.
	GDAPI IGraphicsDirect3D11TextureCube::IGraphicsDirect3D11TextureCube(IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
		: IGraphicsTextureCube(gfxTextureCubeCreationInfo), GLTextureCubeID(0), GLShaderResourceView(nullptr)
	{
		GD_DEBUG_ASSERT(false, "Test this correctly!");
		
		glGenTextures(1, &GLTextureCubeID);
		Imm_UpdateData(gfxTextureCubeInitialData);
		
		GLShaderResourceView = GD_NEW(IGraphicsDirect3D11ShaderResourceView, IGRAPHICS_RESOURCE_TYPE_TEXTURE_CUBE);
		GLShaderResourceView->GLShaderResourceID = GLTextureCubeID;
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Cube texture.
	GDAPI IGraphicsDirect3D11TextureCube::~IGraphicsDirect3D11TextureCube()
	{
		glDeleteTextures(1, &GLTextureCubeID);
		GLShaderResourceView->Release();
	}

	// ------------------------------------------------------------------------------------------
	//! Queries the shader resource for current resource.
	//! @returns Pointer to the shader resource view. Should be released manually.
	GDAPI IGraphicsShaderResourceView const* IGraphicsDirect3D11TextureCube::CPU_GetShaderResourceView() const
	{
		return GLShaderResourceView;
	}

	// ------------------------------------------------------------------------------------------
	//! Updates data stored inside texture.
	//! @param gfxTextureCubeData New data that would be uploaded to GPU.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsDirect3D11TextureCube::Imm_UpdateData(IGraphicsTextureCubeData const* /*const*/ gfxTextureCubeData)
	{
		if (gfxTextureCubeData == nullptr)
		{
			IGraphicsTextureCubeData static const gfxEmptyTextureCubeData = {};
			gfxTextureCubeData = &gfxEmptyTextureCubeData;
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, GLTextureCubeID);
		for (GLenum glCubeMapSide = 0; glCubeMapSide < 6; ++glCubeMapSide)
		{
			GLuint glMipLevelOffset = 0;
			GLuint const glMipLevelsCount = Max(1u, gfxTextureCubeCreationInfo.TextureMipMapsCount);
			for (GLuint glMipLevel = 0; glMipLevel < glMipLevelsCount; ++glMipLevel)
			{
				// Computing the dimensions of our mipmap level..
				GLuint const glMipLevelWidth = Max(1u, gfxTextureCubeCreationInfo.TextureWidth >> glMipLevel);
				GLuint const glMipLevelHeight = Max(1u, gfxTextureCubeCreationInfo.TextureHeight >> glMipLevel);
				if (gfxTextureCubeData->TexureCubeSides[glCubeMapSide].TextureData == nullptr)
				{
					// Even if no initial data was specified, we need to provide pixel formats into the Direct3D11.
					// Specifying null image information for all mipmap levels.
					// https://www.opengl.org/sdk/docs/man/html/glTexImageCube.xhtml
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + glCubeMapSide, glMipLevel
						, IGraphicsDirect3D11FormatConvert(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, glMipLevelWidth, glMipLevelHeight, 0
						, IGraphicsDirect3D11FormatGetLayout(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, IGraphicsDirect3D11FormatGetType(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, nullptr
						);
				}
				else if (gfxTextureCubeCreationInfo.TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_RAW)
				{
					//! @todo Compute the block size correctly!
					GLuint const glMipLevelBlockSize = IGraphicsFormatGetLayout(gfxTextureCubeCreationInfo.TexturePixelFormat);

					// Specifying raw image information for all mipmap levels.
					// See link below how to correctly do it.
					// https://www.opengl.org/sdk/docs/man/html/glTexImageCube.xhtml
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + glCubeMapSide, glMipLevel
						, IGraphicsDirect3D11FormatConvert(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, glMipLevelWidth, glMipLevelHeight, 0
						, IGraphicsDirect3D11FormatGetLayout(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, IGraphicsDirect3D11FormatGetType(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, static_cast<Byte const*>(gfxTextureCubeData->TexureCubeSides[glCubeMapSide].TextureData) + glMipLevelOffset
						);
					glMipLevelOffset += glMipLevelWidth * glMipLevelHeight * glMipLevelBlockSize;
				}
				else
				{
					GLuint const glMipLevelBlockSize = gfxTextureCubeCreationInfo.TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1 ? 8 : 16;
					GLuint const glMipLevelSize = (glMipLevelWidth + 3) / 4 * (glMipLevelHeight + 3) / 4 * glMipLevelBlockSize;

					// Specifying DXT-compressed image information for all mipmap levels.
					// See link below how to correctly do it.
					// https://www.opengl.org/sdk/docs/man/html/glCompressedTexImageCube.xhtml
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + glCubeMapSide, glMipLevel
						, IGraphicsDirect3D11TextureCompressionModesTable[gfxTextureCubeCreationInfo.TextureCompressionMode]
						, glMipLevelWidth, glMipLevelHeight, 0, glMipLevelSize
						, static_cast<Byte const*>(gfxTextureCubeData->TexureCubeSides[glCubeMapSide].TextureData) + glMipLevelOffset
						);
					glMipLevelOffset += glMipLevelSize;
				}
			}
		}
		if (gfxTextureCubeCreationInfo.TextureMipMapsCount == 0)
		{
			// We have to also generate mipmaps for our texture.
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		// Cleaning up..
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Cube texture with specified parameters.
	//! @param gfxTextureCubePtr Pointer for output.
	//! @param gfxTextureCubeInitialData Initial data for our texture.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsBaseDirect3D11_WithTexturesCube::GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
		, IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_SamplerStateCreate(gfxSamplerStatePtr, gfxSamplerStateCreationInf)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxTextureCubePtr = GD_NEW(IGraphicsDirect3D11TextureCube, gfxTextureCubeCreationInfo, gfxTextureCubeInitialData);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 3D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsDirect3D11Texture3D);

#endif

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
