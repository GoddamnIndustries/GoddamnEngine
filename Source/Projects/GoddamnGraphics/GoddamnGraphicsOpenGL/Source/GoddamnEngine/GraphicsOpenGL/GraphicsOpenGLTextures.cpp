// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLTextures.cpp
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface: sampler states and textures.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLFormat.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Sampler state objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU sampler state with specified parameters.
	//! @param gfxSamplerStatePtr Pointer for output.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	GDAPI IGraphicsOpenGLSamplerState::IGraphicsOpenGLSamplerState(IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo)
		: IGraphicsSamplerState(gfxSamplerStateCreationInfo), m_GLSamplerStateID(0)
	{
		glGenSamplers(1, &m_GLSamplerStateID);

		// Setting up the wrap modes..
		glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_WRAP_S, IGraphicsOpenGLTextureAddressModesTable[gfxSamplerStateCreationInfo->SamplerAddressModeU]);
		glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_WRAP_T, IGraphicsOpenGLTextureAddressModesTable[gfxSamplerStateCreationInfo->SamplerAddressModeV]);
		glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_WRAP_R, IGraphicsOpenGLTextureAddressModesTable[gfxSamplerStateCreationInfo->SamplerAddressModeW]);

		// Setting up the minification, magnification and the anisotropic filtering mode..
		if (GLEW_EXT_texture_filter_anisotropic && gfxSamplerStateCreationInfo->SamplerMaxAnisotropy != 0)
		{
			// Anisotropic filtering requires linear filtering being enabled..
			// https://www.opengl.org/wiki/Sampler_Object#Anisotropic_filtering
			glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Clamping max supported filtering level and the specified one.
			GLfloat glAnisotropicFilteringMax = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glAnisotropicFilteringMax);
			glSamplerParameterf(m_GLSamplerStateID, GL_TEXTURE_MAX_ANISOTROPY_EXT
				, Min(glAnisotropicFilteringMax, static_cast<GLfloat>(gfxSamplerStateCreationInfo->SamplerMaxAnisotropy)));
		}
		else
		{
			// Using the specified minification and magnification filtering..
			glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_MIN_FILTER, IGraphicsOpenGLTextureMinFiltersTable[gfxSamplerStateCreationInfo->SamplerMinFilter]);
			glSamplerParameteri(m_GLSamplerStateID, GL_TEXTURE_MAG_FILTER, IGraphicsOpenGLTextureMagFiltersTable[gfxSamplerStateCreationInfo->SamplerMagFilter]);
		}

		// Setting up the level-of-details-parameters and border color..
		glSamplerParameterfv(m_GLSamplerStateID, GL_TEXTURE_BORDER_COLOR, gfxSamplerStateCreationInfo->SamplerBorderColor.GetData());
		glSamplerParameterf(m_GLSamplerStateID, GL_TEXTURE_MIN_LOD, gfxSamplerStateCreationInfo->SamplerLODMinClamp);
		glSamplerParameterf(m_GLSamplerStateID, GL_TEXTURE_MAX_LOD, gfxSamplerStateCreationInfo->SamplerLODMaxClamp != 0.0f
			? gfxSamplerStateCreationInfo->SamplerLODMaxClamp : FLT_MAX);
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes the specified GPU sampler state.
	GDAPI IGraphicsOpenGLSamplerState::~IGraphicsOpenGLSamplerState()
	{
		glDeleteSamplers(1, &m_GLSamplerStateID);
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU sampler state with specified parameters.
	//! @param gfxSamplerStatePtr Pointer for output.
	//! @param gfxSamplerStateCreationInfo Pointer to the sampler state creation information.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithSamplerStates::GfxImm_SamplerStateCreate(IGraphicsSamplerState** const gfxSamplerStatePtr
		, IGraphicsSamplerStateCreationInfo const* const gfxSamplerStateCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_SamplerStateCreate(gfxSamplerStatePtr, gfxSamplerStateCreationInf)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxSamplerStatePtr = GD_NEW(IGraphicsOpenGLSamplerState, gfxSamplerStateCreationInfo);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes an empty shader resource view.
	GDAPI IGraphicsOpenGLShaderResourceView::IGraphicsOpenGLShaderResourceView(IGraphicsResourceType const gfxResourceType)
		: m_GLShaderResourceID(0), m_GLShaderResourceType(IGraphicsOpenGLResourceTypesTable[gfxResourceType])
	{
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes the shader resource view.
	GDAPI IGraphicsOpenGLShaderResourceView::~IGraphicsOpenGLShaderResourceView()
	{
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU 2D texture with specified parameters.
	//! @param gfxTexture2DCreationInfo Pointer to the 2D texture creation information.
	//! @param gfxTexture2DInitialData Initial data for our texture.
	GDAPI IGraphicsOpenGLTexture2D::IGraphicsOpenGLTexture2D(IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
		: IGraphicsTexture2D(gfxTexture2DCreationInfo), m_GLTexture2DID(0), m_GLShaderResourceView(nullptr)
	{
		glGenTextures(1, &m_GLTexture2DID);
		Imm_UpdateData(gfxTexture2DInitialData);

		m_GLShaderResourceView = gd_new IGraphicsOpenGLShaderResourceView(IGRAPHICS_RESOURCE_TYPE_TEXTURE_2D);
		m_GLShaderResourceView->m_GLShaderResourceID = m_GLTexture2DID;
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU 2D texture.
	GDAPI IGraphicsOpenGLTexture2D::~IGraphicsOpenGLTexture2D()
	{
		glDeleteTextures(1, &m_GLTexture2DID);
		m_GLShaderResourceView->Release();
	}

	/*!
	 * Queries the shader resource for current resource.
	 * @returns Pointer to the shader resource view. Should be released manually.
	 */
	GDAPI IGraphicsShaderResourceView const* IGraphicsOpenGLTexture2D::CPU_GetShaderResourceView() const
	{
		return m_GLShaderResourceView;
	}

	/*!
	 * Updates data stored inside texture.
	 *
	 * @param gfxTexture2DData New data that would be uploaded to GPU.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLTexture2D::Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_GLTexture2DID);
			
		GLuint glMipLevelOffset = 0;
		GLuint const glMipLevelsCount = Max(1u, gfxTexture2DCreationInfo.TextureMipMapsCount);
		for (GLuint glMipLevel = 0; glMipLevel < glMipLevelsCount; ++glMipLevel)
		{
			// Computing the dimensions of our mipmap level..
			GLuint const glMipLevelWidth = Max(1u, gfxTexture2DCreationInfo.TextureWidth >> glMipLevel);
			GLuint const glMipLevelHeight = Max(1u, gfxTexture2DCreationInfo.TextureHeight >> glMipLevel);
			if (gfxTexture2DData == nullptr)
			{
				// Even if no initial data was specified, we need to provide pixel formats into the OpenGL.
				// Specifying null image information for all mipmap levels.
				glTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsOpenGLFormatConvert(gfxTexture2DCreationInfo.TexturePixelFormat)
					, glMipLevelWidth, glMipLevelHeight, 0
					, IGraphicsOpenGLFormatGetLayout(gfxTexture2DCreationInfo.TexturePixelFormat)
					, IGraphicsOpenGLFormatGetType(gfxTexture2DCreationInfo.TexturePixelFormat)
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
					, IGraphicsOpenGLFormatConvert(gfxTexture2DCreationInfo.TexturePixelFormat)
					, glMipLevelWidth, glMipLevelHeight, 0
					, IGraphicsOpenGLFormatGetLayout(gfxTexture2DCreationInfo.TexturePixelFormat)
					, IGraphicsOpenGLFormatGetType(gfxTexture2DCreationInfo.TexturePixelFormat)
					, static_cast<Byte const*>(gfxTexture2DData->TextureData) + glMipLevelOffset
					);
				glMipLevelOffset += glMipLevelWidth * glMipLevelHeight * glMipLevelBlockSize;
			}
			else
			{
				GD_DEBUG_VERIFY(false, "Test this correctly!");

				GLuint const glMipLevelBlockSize = gfxTexture2DCreationInfo.TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1 ? 8 : 16;
				GLuint const glMipLevelSize = (glMipLevelWidth + 3) / 4 * (glMipLevelHeight + 3) / 4 * glMipLevelBlockSize;

				// Specifying DXT-compressed image information for all mipmap levels.
				// See link below how to correctly do it.
				// https://www.opengl.org/sdk/docs/man/html/glCompressedTexImage2D.xhtml
				glCompressedTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsOpenGLTextureCompressionModesTable[gfxTexture2DCreationInfo.TextureCompressionMode]
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

	/*!
	 * Creates a GPU 2D texture with specified parameters.
	 *
	 * @param gfxTexture2DPtr Pointer for output.
	 * @param gfxTexture2DCreationInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture2DInitialData Initial data for our texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseOpenGL_WithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
		, IGraphicsTexture2DCreationInfo const* const gfxTexture2DCreationInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_SamplerStateCreate(gfxSamplerStatePtr, gfxSamplerStateCreationInf)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxTexture2DPtr = gd_new IGraphicsOpenGLTexture2D(gfxTexture2DCreationInfo, gfxTexture2DInitialData);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsOpenGLTextureCube);

	// ------------------------------------------------------------------------------------------
	//! Creates a GPU Cube texture with specified parameters.
	//! @param gfxTextureCubeCreationInfo Pointer to the Cube texture creation information.
	//! @param gfxTextureCubeInitialData Initial data for our texture.
	GDAPI IGraphicsOpenGLTextureCube::IGraphicsOpenGLTextureCube(IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
		: IGraphicsTextureCube(gfxTextureCubeCreationInfo), m_GLTextureCubeID(0), m_GLShaderResourceView(nullptr)
	{
		GD_DEBUG_VERIFY(false, "Test this correctly!");
		
		glGenTextures(1, &m_GLTextureCubeID);
		Imm_UpdateData(gfxTextureCubeInitialData);
		
		m_GLShaderResourceView = GD_NEW(IGraphicsOpenGLShaderResourceView, IGRAPHICS_RESOURCE_TYPE_TEXTURE_CUBE);
		m_GLShaderResourceView->m_GLShaderResourceID = m_GLTextureCubeID;
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Cube texture.
	GDAPI IGraphicsOpenGLTextureCube::~IGraphicsOpenGLTextureCube()
	{
		glDeleteTextures(1, &m_GLTextureCubeID);
		m_GLShaderResourceView->Release();
	}

	// ------------------------------------------------------------------------------------------
	//! Queries the shader resource for current resource.
	//! @returns Pointer to the shader resource view. Should be released manually.
	GDAPI IGraphicsShaderResourceView const* IGraphicsOpenGLTextureCube::CPU_GetShaderResourceView() const
	{
		return m_GLShaderResourceView;
	}

	// ------------------------------------------------------------------------------------------
	//! Updates data stored inside texture.
	//! @param gfxTextureCubeData New data that would be uploaded to GPU.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLTextureCube::Imm_UpdateData(IGraphicsTextureCubeData const* /*const*/ gfxTextureCubeData)
	{
		if (gfxTextureCubeData == nullptr)
		{
			IGraphicsTextureCubeData static const gfxEmptyTextureCubeData = {};
			gfxTextureCubeData = &gfxEmptyTextureCubeData;
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLTextureCubeID);
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
					// Even if no initial data was specified, we need to provide pixel formats into the OpenGL.
					// Specifying null image information for all mipmap levels.
					// https://www.opengl.org/sdk/docs/man/html/glTexImageCube.xhtml
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + glCubeMapSide, glMipLevel
						, IGraphicsOpenGLFormatConvert(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, glMipLevelWidth, glMipLevelHeight, 0
						, IGraphicsOpenGLFormatGetLayout(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, IGraphicsOpenGLFormatGetType(gfxTextureCubeCreationInfo.TexturePixelFormat)
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
						, IGraphicsOpenGLFormatConvert(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, glMipLevelWidth, glMipLevelHeight, 0
						, IGraphicsOpenGLFormatGetLayout(gfxTextureCubeCreationInfo.TexturePixelFormat)
						, IGraphicsOpenGLFormatGetType(gfxTextureCubeCreationInfo.TexturePixelFormat)
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
						, IGraphicsOpenGLTextureCompressionModesTable[gfxTextureCubeCreationInfo.TextureCompressionMode]
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
	GDAPI IResult IGraphicsBaseOpenGL_WithTexturesCube::GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
		, IGraphicsTextureCubeCreationInfo const* const gfxTextureCubeCreationInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_SamplerStateCreate(gfxSamplerStatePtr, gfxSamplerStateCreationInf)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxTextureCubePtr = gd_new IGraphicsOpenGLTextureCube(gfxTextureCubeCreationInfo, gfxTextureCubeInitialData);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 3D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsOpenGLTexture3D);

#endif	// if GD_FALSE

GD_NAMESPACE_END
