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
 * File contains implementation for OpenGL(ES) graphics interface: sampler states.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLFormat.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsSampler interface.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Creates a GPU sampler state with specified parameters.
	 *
	 * @param gfxSamplerPtr Pointer for output.
	 * @param gfxSamplerCreateInfo Pointer to the sampler state creation information.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithSamplers::GfxImm_SamplerCreate(IGraphicsSampler*& gfxSamplerPtr
		, IGraphicsSamplerCreateInfo const& gfxSamplerCreateInfo)
	{
		GLuint glSamplerID = 0;
		glThrowIfFailed(glCreateSamplers(1, &glSamplerID));

		// Setting up the specified minification and magnification filtering..
		switch (gfxSamplerCreateInfo.SamplerMinFilter)
		{
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_NEAREST:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
				break;
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_LINEAR:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerMinFilter >= IGRAPHICS_SAMPLER_MINMAG_FILTER_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}
		switch (gfxSamplerCreateInfo.SamplerMagFilter)
		{
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_NEAREST:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
				break;
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_LINEAR:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerMagFilter >= IGRAPHICS_SAMPLER_MINMAG_FILTER_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the wrap modes..
		switch (gfxSamplerCreateInfo.SamplerAddressModeU)
		{
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_S, GL_REPEAT));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerAddressModeU >= IGRAPHICS_SAMPLER_ADDRESS_MODE_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}
		switch (gfxSamplerCreateInfo.SamplerAddressModeV)
		{
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_T, GL_REPEAT));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerAddressModeV >= IGRAPHICS_SAMPLER_ADDRESS_MODE_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}
		switch (gfxSamplerCreateInfo.SamplerAddressModeW)
		{
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_R, GL_REPEAT));
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerAddressModeW >= IGRAPHICS_SAMPLER_ADDRESS_MODE_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the border color..
		GLfloat glSamplerBorderColor[4] = {};
		switch (gfxSamplerCreateInfo.SamplerBorderColor)
		{
			case IGRAPHICS_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK:
				glSamplerBorderColor[3] = 0.0f;
				glSamplerBorderColor[0] = glSamplerBorderColor[1] = glSamplerBorderColor[2] = 0.0f;
				glThrowIfFailed(glSamplerParameterfv(glSamplerID, GL_TEXTURE_BORDER_COLOR, glSamplerBorderColor));
				break;
			case IGRAPHICS_SAMPLER_BORDER_COLOR_OPAQUE_BLACK:
				glSamplerBorderColor[3] = 1.0f;
				glSamplerBorderColor[0] = glSamplerBorderColor[1] = glSamplerBorderColor[2] = 0.0f;
				glThrowIfFailed(glSamplerParameterfv(glSamplerID, GL_TEXTURE_BORDER_COLOR, glSamplerBorderColor));
				break;
			case IGRAPHICS_SAMPLER_BORDER_COLOR_OPAQUE_WHITE:
				glSamplerBorderColor[3] = 1.0f;
				glSamplerBorderColor[0] = glSamplerBorderColor[1] = glSamplerBorderColor[2] = 1.0f;
				glThrowIfFailed(glSamplerParameterfv(glSamplerID, GL_TEXTURE_BORDER_COLOR, glSamplerBorderColor));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerBorderColor >= IGRAPHICS_SAMPLER_BORDER_COLOR_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the comparison function..
		switch (gfxSamplerCreateInfo.SamplerCompFunc)
		{
			case IGRAPHICS_COMPARISON_MODE_NEVER:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_NEVER));
				break;
			case IGRAPHICS_COMPARISON_MODE_ALWAYS:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_ALWAYS));
				break;
			case IGRAPHICS_COMPARISON_MODE_EQUAL:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_EQUAL));
				break;
			case IGRAPHICS_COMPARISON_MODE_NOT_EQUAL:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_NOTEQUAL));
				break;
			case IGRAPHICS_COMPARISON_MODE_LESS:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_LESS));
				break;
			case IGRAPHICS_COMPARISON_MODE_LESS_OR_EQUAL:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
				break;
			case IGRAPHICS_COMPARISON_MODE_GREATER:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_GREATER));
				break;
			case IGRAPHICS_COMPARISON_MODE_GREATER_OR_EQUAL:
				glThrowIfFailed(glSamplerParameteri(glSamplerID, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL));
				break;
#if GD_DEBUG
			default:
				GD_DEBUG_VERIFY(gfxSamplerCreateInfo.SamplerCompFunc >= IGRAPHICS_COMPARISON_MODE_UNKNOWN);
				glDeleteSamplers(1, &glSamplerID);
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the MIP level-of-details bias..
		GLfloat const glSamplerMipLodBias = gfxSamplerCreateInfo.SamplerMipLodBias;
		glThrowIfFailed(glSamplerParameterf(glSamplerID, GL_TEXTURE_LOD_BIAS_EXT, glSamplerMipLodBias));

		// Setting up the level-of-details parameters..
		GLfloat const glSamplerMinLod = gfxSamplerCreateInfo.SamplerLodMin;
		GLfloat const glSamplerMaxLod = gfxSamplerCreateInfo.SamplerLodMax;
		glThrowIfFailed(glSamplerParameterf(glSamplerID, GL_TEXTURE_MIN_LOD, glSamplerMinLod));
		if (gfxSamplerCreateInfo.SamplerLodMax != 0.0f)
		{
			glThrowIfFailed(glSamplerParameterf(glSamplerID, GL_TEXTURE_MAX_LOD, glSamplerMaxLod));
		}
		else
		{
			glThrowIfFailed(glSamplerParameterf(glSamplerID, GL_TEXTURE_MAX_LOD, FLT_MAX));
		}

		// Setting up the anisotropic filtering mode..
		if (gfxSamplerCreateInfo.SamplerAnisotropyEnabled)
		{
			// Clamping max supported filtering level and the specified one.
			GLfloat glSystemAnisotropyMax = 0.0f;
			GLfloat const glSamplerAnisotropyMax = gfxSamplerCreateInfo.SamplerAnisotropyMax;
			glThrowIfFailed(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glSystemAnisotropyMax));
			glThrowIfFailed(glSamplerParameterf(glSamplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT
				, Min(glSamplerAnisotropyMax, glSystemAnisotropyMax)));
		}

		gfxSamplerPtr = gd_new IGraphicsOpenGLSampler(gfxSamplerCreateInfo, glSamplerID);
		return IResult::Ok;
	}

	/*!
	 * Destroys the GPU sampler state.
	 *
	 * @param gfxSampler Sampler state to destroy.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithSamplers::GfxImm_SamplerDestroy(IGraphicsSampler* const gfxSampler)
	{
	    GD_DEBUG_VERIFY(gfxSampler != nullptr);

		auto const glSampler = static_cast<IGraphicsOpenGLSampler*>(gfxSampler);
		auto const glSamplerID = glSampler->m_GLSamplerID;
		gd_delete glSampler;

		glThrowIfFailed(glDeleteSamplers(1, &glSamplerID));

		return IResult::Ok;
	}

GD_NAMESPACE_END
