// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * OpenGL(ES) graphics interface: sampler states and textures.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLFormat.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsTexture1D interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Creates a GPU 1D texture with specified parameters.
	 *
	 * @param gfxTexture1DPtr Pointer for output.
	 * @param gfxTexture1DCreateInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture1DInitialData Initial data for the texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult GfxImm_Texture1DCreate(IGraphicsTexture1D*& gfxTexture1DPtr, IGraphicsTexture1DCreateInfo const& gfxTexture1DCreateInfo, IGraphicsResourceData const& gfxTexture1DInitialData)
	{
		if (gfxTexture1DCreateInfo.TextureArraySize <= 1)
		{
			// Creating the 1D texture..
			GLuint glTexture1DID = 0;
			glThrowIfFailed(glCreateTextures(GL_TEXTURE_1D, 1, &glTexture1DID));
		}
	}

	GDAPI IResult GfxImm_Texture1DDestroy(IGraphicsTexture1D* const gfxObj)
	{
	}

	GDAPI IResult GfxImm_Texture1DMap(IGraphicsTexture1D* const gfxObj, IGraphicsResourceMapMode const gfxMapMode, IGraphicsResourceData& gfxMappedMemory)
	{
	}

	GDAPI IResult GfxImm_Texture1DUnmap(IGraphicsTexture1D* const gfxObj)
	{
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsTexture2D interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Creates a GPU 2D texture with specified parameters.
	 *
	 * @param gfxTexture2DPtr Pointer for output.
	 * @param gfxTexture2DCreateInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture2DInitialData Initial data for the texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseOpenGLWithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D**& gfxTexture2DPtr
		, IGraphicsTexture2DCreateInfo const& gfxTexture2DCreateInfo, CHandle const gfxTexture2DInitialData) 
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
		glDeleteTextures(1, &Texture2DID);
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
		glBindTexture(GL_TEXTURE_2D, Texture2DID);
			
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
	GDAPI IResult IGraphicsBaseOpenGLWithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D**& gfxTexture2DPtr
		, IGraphicsTexture2DCreateInfo const& gfxTexture2DCreateInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTexture2DPtr != nullptr);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo.TextureWidth > 0);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo.TextureHeight > 0);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo.TextureCompressionMode < IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN);
		GD_ARG_VERIFY(IGraphicsFormatGetIndex(gfxTexture2DCreateInfo.TexturePixelFormat)
			< IGRAPHICS_FORMATS_COUNT);
#endif	// if GD_DEBUG

		*gfxTexture2DPtr = gd_new IGraphicsOpenGLTexture2D(gfxTexture2DCreateInfo, gfxTexture2DInitialData);
		return IResult::Ok;
	}

GD_NAMESPACE_END
