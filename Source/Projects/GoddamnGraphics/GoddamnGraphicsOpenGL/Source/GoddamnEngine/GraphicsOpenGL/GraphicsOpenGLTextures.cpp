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
 * File contains implementation for OpenGL(ES) graphics interface: sampler states and textures.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLFormat.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsTexture2D interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GLenum static const TextureCompressionModesTable[IGRAPHICS_TEXTURE_2D_COMPRESSIONS_COUNT] = {
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_NONE    */ 0,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1    */ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT3    */ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT5    */ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
		/* IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN */ 0,
	};

	/*!
	 * Creates a GPU 2D texture with specified parameters.
	 *
	 * @param gfxTexture2DPtr Pointer for output.
	 * @param gfxTexture2DCreateInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture2DInitialData Initial data for the texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseOpenGLWithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
		, IGraphicsTexture2DCreateInfo const* const gfxTexture2DCreateInfo, CHandle const gfxTexture2DInitialData) 
	{
		GD_DEBUG_VERIFY(gfxTexture2DPtr != nullptr);
		GD_DEBUG_VERIFY(gfxTexture2DCreateInfo != nullptr);

		glGenTextures(1, &m_GLTexture2DID);
		Imm_UpdateData(gfxTexture2DInitialData);

		m_GLShaderResourceView = gd_new IGraphicsOpenGLShaderResourceView(IGRAPHICS_RESOURCE_TYPE_TEXTURE_2D);
		m_GLShaderResourceView->m_GLShaderResourceID = m_GLTexture2DID;
	}

	/*! 
	 * Deletes current GPU 2D texture.
	 */
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
		return m_GLShaderResourceView.Get();
	}

	/*!
	 * Updates data stored inside texture.
	 *
	 * @param gfxTexture2DData New data that would be uploaded to GPU.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLTexture2D::Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTexture2DData != nullptr);
#endif	// if GD_DEBUG

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_GLTexture2DID);
			
		GLuint glMipLevelOffset = 0;
		GLuint const glMipLevelsCount = Max(1u, TextureMipMapsCount);
		for (GLuint glMipLevel = 0; glMipLevel < glMipLevelsCount; ++glMipLevel)
		{
			// Computing the dimensions of our mipmap level..
			GLuint const glMipLevelWidth = Max(1u, TextureWidth >> glMipLevel);
			GLuint const glMipLevelHeight = Max(1u, TextureHeight >> glMipLevel);
			if (gfxTexture2DData == nullptr)
			{
				// Even if no initial data was specified, we need to provide pixel formats into the OpenGL.
				// Specifying null image information for all mipmap levels.
				glTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsOpenGLFormatConvert(TexturePixelFormat)
					, glMipLevelWidth, glMipLevelHeight, 0
					, IGraphicsOpenGLFormatGetLayout(TexturePixelFormat)
					, IGraphicsOpenGLFormatGetType(TexturePixelFormat)
					, nullptr
					);
			}
			else if (TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_NONE)
			{
				// Specifying raw image information for all mipmap levels.
				// See link below how to correctly do it.
				// https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
				GLuint const glMipLevelBlockSize = static_cast<GLuint>(IGraphicsFormatSizeof(TexturePixelFormat));
				glTexImage2D(GL_TEXTURE_2D, glMipLevel
					, IGraphicsOpenGLFormatConvert(TexturePixelFormat)
					, glMipLevelWidth, glMipLevelHeight, 0
					, IGraphicsOpenGLFormatGetLayout(TexturePixelFormat)
					, IGraphicsOpenGLFormatGetType(TexturePixelFormat)
					, static_cast<Byte const*>(gfxTexture2DData->TextureData) + glMipLevelOffset
					);
				glMipLevelOffset += glMipLevelWidth * glMipLevelHeight * glMipLevelBlockSize;
			}
			else
			{
				GD_DEBUG_VERIFY(false, "Test this correctly!");

				GLuint const glMipLevelBlockSize = TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1 ? 8 : 16;
				GLuint const glMipLevelSize = (glMipLevelWidth + 3) / 4 * (glMipLevelHeight + 3) / 4 * glMipLevelBlockSize;

				// Specifying DXT-compressed image information for all mipmap levels.
				// See link below how to correctly do it.
				// https://www.opengl.org/sdk/docs/man/html/glCompressedTexImage2D.xhtml
				glCompressedTexImage2D(GL_TEXTURE_2D, glMipLevel
					, TextureCompressionModesTable[TextureCompressionMode]
					, glMipLevelWidth, glMipLevelHeight, 0, glMipLevelSize
					, static_cast<Byte const*>(gfxTexture2DData->TextureData) + glMipLevelOffset
					);
				glMipLevelOffset += glMipLevelSize;
			}
		}
		if (TextureMipMapsCount == 0)
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
	 * @param gfxTexture2DCreateInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture2DInitialData Initial data for our texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseOpenGLWithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
		, IGraphicsTexture2DCreateInfo const* const gfxTexture2DCreateInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTexture2DPtr != nullptr);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo->TextureWidth > 0);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo->TextureHeight > 0);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo->TextureCompressionMode < IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN);
		GD_ARG_VERIFY(IGraphicsFormatGetIndex(gfxTexture2DCreateInfo->TexturePixelFormat)
			< IGRAPHICS_FORMATS_COUNT);
#endif	// if GD_DEBUG

		*gfxTexture2DPtr = gd_new IGraphicsOpenGLTexture2D(gfxTexture2DCreateInfo, gfxTexture2DInitialData);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if 0
	/*!
	 * Creates a GPU Cube texture with specified parameters.
	 *
	 * @param gfxTextureCubePtr Pointer for output.
	 * @param gfxTextureCubeCreateInfo Pointer to the Cube texture creation information.
	 * @param gfxTextureInitialCubeData Initial data for our texture.
	 */
	GDAPI IGraphicsOpenGLTextureCube::IGraphicsOpenGLTextureCube(IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
		: IGraphicsTextureCube(gfxTextureCubeCreateInfo)
		, m_GLTextureCubeID(0), m_GLShaderResourceView(nullptr)
	{
		GD_DEBUG_VERIFY(false, "Test this correctly!");
		
		glGenTextures(1, &m_GLTextureCubeID);
		Imm_UpdateData(gfxTextureCubeInitialData);
		
		m_GLShaderResourceView = gd_new IGraphicsOpenGLShaderResourceView(IGRAPHICS_RESOURCE_TYPE_TEXTURE_CUBE);
		m_GLShaderResourceView->m_GLShaderResourceID = m_GLTextureCubeID;
	}

	/*!
	 * Deletes current GPU Cube texture.
	 */
	GDAPI IGraphicsOpenGLTextureCube::~IGraphicsOpenGLTextureCube()
	{
		glDeleteTextures(1, &m_GLTextureCubeID);
		m_GLShaderResourceView->Release();
	}

	/*!
	 * Queries the shader resource for current resource.
	 * @returns Pointer to the shader resource view. Should be released manually.
	 */
	GDAPI IGraphicsShaderResourceView const* IGraphicsOpenGLTextureCube::CPU_GetShaderResourceView() const
	{
		return m_GLShaderResourceView.Get();
	}

	/*!
	 * Updates data stored inside texture.
	 *
	 * @param gfxTextureCubeData New data that would be uploaded to GPU.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLTextureCube::Imm_UpdateData(IGraphicsTextureCubeData const* /*const*/ gfxTextureCubeData)
	{
		if (gfxTextureCubeData == nullptr)
		{
			IGraphicsTextureCubeData static const gfxEmptyTextureCubeData = {};
			gfxTextureCubeData = &gfxEmptyTextureCubeData;
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_GLTextureCubeID);
		for (GLenum i = 0; i < 6; ++i)
		{
			GLuint glMipLevelOffset = 0;
			GLuint const glMipLevelsCount = Max(1u, CubeSides[i].TextureMipMapsCount);
			for (GLuint glMipLevel = 0; glMipLevel < glMipLevelsCount; ++glMipLevel)
			{
				// Computing the dimensions of our mipmap level..
				GLuint const glMipLevelWidth = Max(1u, CubeSides[i].TextureWidth >> glMipLevel);
				GLuint const glMipLevelHeight = Max(1u, CubeSides[i].TextureHeight >> glMipLevel);
				if (gfxTextureCubeData->TexureCubeSides[i].TextureData == nullptr)
				{
					// Even if no initial data was specified, we need to provide pixel formats into the OpenGL.
					// Specifying null image information for all mipmap levels.
					// https://www.opengl.org/sdk/docs/man/html/glTexImageCube.xhtml
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, glMipLevel
						, IGraphicsOpenGLFormatConvert(CubeSides[i].TexturePixelFormat)
						, glMipLevelWidth, glMipLevelHeight, 0
						, IGraphicsOpenGLFormatGetLayout(CubeSides[i].TexturePixelFormat)
						, IGraphicsOpenGLFormatGetType(CubeSides[i].TexturePixelFormat)
						, nullptr
						);
				}
				else if (CubeSides[i].TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_NONE)
				{
					//! @todo Compute the block size correctly!
					GLuint const glMipLevelBlockSize = IGraphicsFormatGetLayout(CubeSides[i].TexturePixelFormat);

					// Specifying raw image information for all mipmap levels.
					// See link below how to correctly do it.
					// https://www.opengl.org/sdk/docs/man/html/glTexImageCube.xhtml
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, glMipLevel
						, IGraphicsOpenGLFormatConvert(CubeSides[i].TexturePixelFormat)
						, glMipLevelWidth, glMipLevelHeight, 0
						, IGraphicsOpenGLFormatGetLayout(CubeSides[i].TexturePixelFormat)
						, IGraphicsOpenGLFormatGetType(CubeSides[i].TexturePixelFormat)
						, static_cast<Byte const*>(gfxTextureCubeData->TexureCubeSides[i].TextureData) + glMipLevelOffset
						);
					glMipLevelOffset += glMipLevelWidth * glMipLevelHeight * glMipLevelBlockSize;
				}
				else
				{
					GLuint const glMipLevelBlockSize = CubeSides[i].TextureCompressionMode == IGRAPHICS_TEXTURE_2D_COMPRESSION_DXT1 ? 8 : 16;
					GLuint const glMipLevelSize = (glMipLevelWidth + 3) / 4 * (glMipLevelHeight + 3) / 4 * glMipLevelBlockSize;

					// Specifying DXT-compressed image information for all mipmap levels.
					// See link below how to correctly do it.
					// https://www.opengl.org/sdk/docs/man/html/glCompressedTexImageCube.xhtml
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, glMipLevel
						, TextureCompressionModesTable[CubeSides[i].TextureCompressionMode]
						, glMipLevelWidth, glMipLevelHeight, 0, glMipLevelSize
						, static_cast<Byte const*>(gfxTextureCubeData->TexureCubeSides[i].TextureData) + glMipLevelOffset
						);
					glMipLevelOffset += glMipLevelSize;
				}
			}
		}
		if (CubeSides[0].TextureMipMapsCount == 0)
		{
			// We have to also generate mipmaps for our texture.
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		// Cleaning up..
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return IResult::Ok;
	}

	/*!
	 * Creates a GPU Cube texture with specified parameters.
	 *
	 * @param gfxTextureCubePtr Pointer for output.
	 * @param gfxTextureCubeCreateInfo Pointer to the Cube texture creation information.
	 * @param gfxTextureInitialCubeData Initial data for our texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseOpenGLWithTexturesCube::GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
		, IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTextureCubePtr != nullptr);
		GD_ARG_VERIFY(gfxTextureCubeCreateInfo != nullptr);
		for (GLenum i = 0; i < 6; ++i)
		{
			GD_ARG_VERIFY(gfxTextureCubeCreateInfo->CubeSides[i].TextureWidth > 0);
			GD_ARG_VERIFY(gfxTextureCubeCreateInfo->CubeSides[i].TextureHeight > 0);
			GD_ARG_VERIFY(gfxTextureCubeCreateInfo->CubeSides[i].TextureCompressionMode < IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN);
			GD_ARG_VERIFY(IGraphicsFormatGetIndex(gfxTextureCubeCreateInfo->CubeSides[i].TexturePixelFormat) 
				< IGRAPHICS_FORMATS_COUNT);
		}
#endif	// if GD_DEBUG

		*gfxTextureCubePtr = gd_new IGraphicsOpenGLTextureCube(gfxTextureCubeCreateInfo, gfxTextureCubeInitialData);
		return IResult::Ok;
	}
#endif

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GLenum static const ResourceTypesTable[IGRAPHICS_RESOURCE_TYPES_COUNT] = {
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_2D   */ GL_TEXTURE_2D,
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_3D   */ GL_TEXTURE_3D,
		/* IGRAPHICS_RESOURCE_TYPE_TEXTURE_CUBE */ GL_TEXTURE_CUBE_MAP,
		/* IGRAPHICS_RESOURCE_TYPE_UNKNOWN      */ 0,
	};

	/*!
	 * Initializes an empty shader resource view.
	 */
	GDAPI IGraphicsOpenGLShaderResourceView::IGraphicsOpenGLShaderResourceView(IGraphicsShaderResourceType const gfxResourceType)
		: m_GLShaderResourceID(0), m_GLShaderResourceType(ResourceTypesTable[gfxResourceType])
	{
	}

	/*!
	 * Deletes the shader resource view.
	 */
	GDAPI IGraphicsOpenGLShaderResourceView::~IGraphicsOpenGLShaderResourceView()
	{
	}

GD_NAMESPACE_END
