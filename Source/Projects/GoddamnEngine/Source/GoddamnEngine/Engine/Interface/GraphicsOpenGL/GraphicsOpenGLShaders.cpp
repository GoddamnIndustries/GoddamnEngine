// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGLShaders.cpp
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface: shaders.
 */
#include <GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGL.h>
//#include <GoddamnEngine/Core/OutputDevice/OutputDevice.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates an OpenGL GPU shader with specified parameters.
	//! @param gfxShaderCreationInfo Creation information for the shader.
	template<typename IGraphicsShaderBase>
	GDINT IGraphicsOpenGLBaseShader<IGraphicsShaderBase>::IGraphicsOpenGLBaseShader(IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
		: IGraphicsShaderBase(gfxShaderCreationInfo), m_GLShaderProgramID(0), m_GLShaderProgramType(IGraphicsOpenGLShaderTypesTable[gfxShaderCreationInfo->ShaderType])
	{
		//! @todo Implement the caching code.
		bool const _HasCachedBinary = false;
		if (!_HasCachedBinary)
		{
			// No cache for our shader was found. We need to compile this shader now and cache it's Byte code.
			// This should reduce loading time dramatically.
			GLchar const* glShaderSources[] = {
				"#version 430 core\n",
				"#extension GL_ARB_separate_shader_objects : enable\n",
				"\n", // Here should be stored additional code that is required to be emitted on different drivers.
				static_cast<GLchar const*>(gfxShaderCreationInfo->ShaderByteCode),
				"\n"
			};

			//auto static const glVendor = reinterpret_cast<CStr>(glGetString(GL_VENDOR));
			//if (CString::Strstr(glVendor, "ATI") != nullptr)
			{
				// The 'ARB_separate_shader_objects' extension requires to redefine all shader output manually.
				// We need to emit additional code to compile this shaders successfully.
				// Read following to understand what the fuck is going on here and now.
				// https://community.amd.com/message/1279274
				static GLchar const* const glShaderSourceATIInsetions[] = {
					/* IGRAPHICS_SHADER_TYPE_VERTEX */ R"(	
					/*	in int gl_VertexID;			*/
					/*	in int gl_InstanceID;		*/
						out gl_PerVertex 
						{
							vec4 gl_Position;
							float gl_PointSize;
							float gl_ClipDistance[];
						};
					)",

					/* IGRAPHICS_SHADER_TYPE_PIXEL */ R"(	
					/*	in vec4 gl_FragCoord;		*/
					/*	in bool gl_FrontFacing;		*/
					/*	in vec2 gl_PointCoord;		*/
					/*	in int  gl_SampleID;		*/
					/*	in vec2 gl_SamplePosition;	*/
					/*	in int  gl_SampleMaskIn[];	*/
					)",

					/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ "\0",
					/* IGRAPHICS_SHADER_TYPE_HULL     */ "\0",
					/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ "\0",

					/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ "\0",
				};

				// Inserting our additional GLSL code to successfully compile our shader on the ATI drivers..
				glShaderSources[1] = glShaderSourceATIInsetions[gfxShaderCreationInfo->ShaderType];
			}

			m_GLShaderProgramID = glCreateShaderProgramv(m_GLShaderProgramType, GD_ARRAY_LENGTH(glShaderSources), glShaderSources);
#if !GD_RELEASE
			// Validating program and checking shader compilation status..
			// No validation is performed in the release builds: translated from the HLSL shaders
			// are validated in the offline mode in the editor on the translation step. This is just an another useless paranoid check.
			GLint glShaderCompilationLogLength = 0;
			glGetProgramiv(m_GLShaderProgramID, GL_INFO_LOG_LENGTH, &glShaderCompilationLogLength);
			if (glShaderCompilationLogLength > 1)
			{
				BaseString<GLchar> glShaderCompilationLog(static_cast<SizeTp>(glShaderCompilationLogLength));
				glGetProgramInfoLog(m_GLShaderProgramID, static_cast<GLsizei>(glShaderCompilationLogLength), nullptr, glShaderCompilationLog.CStr());
				/*ConsoleDevice->LogFormat("Validation of the separate shader object of type %d proceeded with the following log: \n%s"
					, gfxShaderCreationInfo->ShaderType, glShaderCompilationLog.CStr());*/

				glUseProgram(0);
				glDeleteProgram(m_GLShaderProgramID);
				return;
			}
#endif	// if !GD_RELEASE
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes current GPU Pixel shader.
	template<typename IGraphicsShaderBase>
	GDINT IGraphicsOpenGLBaseShader<IGraphicsShaderBase>::~IGraphicsOpenGLBaseShader()
	{
		glUseProgram(0);
		glBindProgramPipeline(0);
		glDeleteProgram(m_GLShaderProgramID);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @tparam gfxShaderType Type of the current shader to bind.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	template<IGraphicsShaderType gfxShaderType>
	// ReSharper disable once CppMemberFunctionMayBeConst
	GDINT void IGraphicsBaseOpenGL_WithBaseShaders::GfxCmd_BaseShaderBindUniformBuffers(IGraphicsBuffer const* const* const gfxUniformBuffers
		, SizeTp const gfxUniformBuffersCount)
	{
		auto const glUniformBuffers = reinterpret_cast<IGraphicsOpenGLBuffer const* const*>(gfxUniformBuffers);

		// Binding our buffers array into the uniform buffer slots.
		// We offset each buffer binding point on the constant value - to make all the binding points different for
		// single binded separate shader programs.
		//! @todo Check whether the count of the uniform buffers binding points is large enough for uniform buffers.
		//! @todo Somehow export the offset value out of this block.
		GLuint static const gfxMaxUniformBuffersPerType = 14;
		GLuint static const gfxUniformBufferBindingPointOffset = gfxMaxUniformBuffersPerType * gfxShaderType;
		if (glUniformBuffers != nullptr)
		{
			for (GLuint glUniformBlockIndex = 0; glUniformBlockIndex < gfxUniformBuffersCount; ++glUniformBlockIndex)
			{
				// Binding current buffer...
				auto const glUniformBuffer = glUniformBuffers[glUniformBlockIndex];
				glBindBufferBase(GL_UNIFORM_BUFFER, gfxUniformBufferBindingPointOffset + glUniformBlockIndex, glUniformBuffer->m_GLBufferID);
			}
		}
		else
		{
			for (GLuint glUniformBlockIndex = 0; glUniformBlockIndex < gfxUniformBuffersCount; ++glUniformBlockIndex)
			{
				// Unbinding current uniform buffer..
				glBindBufferBase(GL_UNIFORM_BUFFER, gfxUniformBufferBindingPointOffset + glUniformBlockIndex, 0);
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @tparam gfxShaderType Type of the shader to process.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	template<IGraphicsShaderType gfxShaderType>
	GDINT void IGraphicsBaseOpenGL_WithBaseShaders::GfxCmd_BaseShaderBindResources(IGraphicsShaderResourceView const* const* const gfxResources
		, SizeTp const gfxResourcesCount)
	{
		auto const glResources = reinterpret_cast<IGraphicsOpenGLShaderResourceView const* const*>(gfxResources);

		// Binding our resources array into the texture/sampler slots.
		// We offset each resource binding point on the constant value - to make all the binding points different for
		// single binded separate shader programs.
		//! @todo Check whether the count of the texture/sampler binding points is large enough for textures.
		//! @todo Somehow export the offset value out of this block.
		GLuint static const gfxMaxResourcesPerType = 8;
		GLuint static const gfxResourceBindingPointOffset = gfxMaxResourcesPerType * gfxShaderType;
		for (GLuint glResourceIndex = 0; glResourceIndex < gfxResourcesCount; ++glResourceIndex)
		{
			glActiveTexture(GL_TEXTURE0 + gfxResourceBindingPointOffset + glResourceIndex);
			if (glResources != nullptr)
			{
				// Binding current texture...
				auto const glResource = glResources[glResourceIndex];
				glBindTexture(glResource->m_GLShaderResourceType, glResource->m_GLShaderResourceID);
			}
			else
			{
				// Unbinding all the types of resources from current binding point..
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glBindTexture(GL_TEXTURE_3D, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Vertex shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @tparam gfxShaderType Type of the shader to process.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	template<IGraphicsShaderType gfxShaderType>
	GDINT void IGraphicsBaseOpenGL_WithBaseShaders::GfxCmd_BaseShaderBindSamplers(IGraphicsSamplerState const* const* const gfxSamplerStates
		, SizeTp const gfxSamplerStatesCount)
	{
		auto const glSamplerStates = reinterpret_cast<IGraphicsOpenGLSamplerState const* const*>(gfxSamplerStates);

		// Binding our samplers array into the texture/sampler slots.
		// We offset each texture/sampler binding point on the constant value - to make all the binding points different for
		// single binded separate shader programs.
		//! @todo Check whether the count of the texture/sampler binding points is large enough for samplers.
		//! @todo Somehow export the offset value out of this block.
		GLuint static const gfxMaxSamplersPerType = 8;
		GLuint static const gfxSamplerBindingPointOffset = gfxMaxSamplersPerType * gfxShaderType;
		for (GLuint glSamplerIndex = 0; glSamplerIndex < gfxSamplerStatesCount; ++glSamplerIndex)
		{
			if (glSamplerStates != nullptr)
			{
				// Binding current sampler state..
				auto const gfxSamplerState = glSamplerStates[glSamplerIndex];
				glBindSampler(gfxSamplerBindingPointOffset + glSamplerIndex, gfxSamplerState->m_GLSamplerStateID);
			}
			else
			{
				// Unbinding current sampler state..
				glBindSampler(gfxSamplerBindingPointOffset + glSamplerIndex, 0);
			}
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a new Vertex GPU shader with specified parameters.
	//! @param gfxVertexShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Vertex shader.
	//! @param gfxVertexArrayLayout input layout of the vertex shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithVertexShaders::GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo
		, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_VertexShaderCreate(gfxVertexShaderPtr, gfxShaderCreationInfo, gfxVertexArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// input layouts are not used in OpenGL.
		GD_NOT_USED(gfxVertexArrayLayout);

		// Copying parameters and forcedly specifying Vertex shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_VERTEX;
		*gfxVertexShaderPtr = GD_NEW(IGraphicsOpenGLVertexShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Vertex shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsOpenGLWithVertexShaders::GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Vertex shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithVertexShaders::GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxResources, gfxResourcesCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Vertex shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithVertexShaders::GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_VertexShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxSamplerStates, gfxSamplerStatesCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a new Pixel GPU shader with specified parameters.
	//! @param gfxPixelShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Pixel shader.
	//! @param gfxPixelArrayLayout input layout of the vertex shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithPixelShaders::GfxImm_PixelShaderCreate(IGraphicsPixelShader** const gfxPixelShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_PixelShaderCreate(gfxPixelShaderPtr, gfxShaderCreationInfo, gfxPixelArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Pixel shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_PIXEL;
		*gfxPixelShaderPtr = GD_NEW(IGraphicsOpenGLPixelShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Pixel shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsOpenGLWithPixelShaders::GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PixelShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Pixel shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithPixelShaders::GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PixelShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxResources, gfxResourcesCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Pixel shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithPixelShaders::GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PixelShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxSamplerStates, gfxSamplerStatesCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a new Geometry GPU shader with specified parameters.
	//! @param gfxGeometryShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Geometry shader.
	//! @param gfxGeometryArrayLayout input layout of the vertex shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithGeometryShaders::GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_GeometryShaderCreate(gfxGeometryShaderPtr, gfxShaderCreationInfo, gfxGeometryArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Geometry shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_GEOMETRY;
		*gfxGeometryShaderPtr = GD_NEW(IGraphicsOpenGLGeometryShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Geometry shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsOpenGLWithGeometryShaders::GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_GeometryShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Geometry shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithGeometryShaders::GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_GeometryShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxResources, gfxResourcesCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Geometry shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithGeometryShaders::GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_GeometryShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxSamplerStates, gfxSamplerStatesCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Hull shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if GD_FALSE

	// ------------------------------------------------------------------------------------------
	//! Creates a new Hull GPU shader with specified parameters.
	//! @param gfxHullShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Hull shader.
	//! @param gfxHullArrayLayout input layout of the vertex shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithHullShaders::GfxImm_HullShaderCreate(IGraphicsHullShader** const gfxHullShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_HullShaderCreate(gfxHullShaderPtr, gfxShaderCreationInfo, gfxHullArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Hull shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_HULL;
		*gfxHullShaderPtr = GD_NEW(IGraphicsOpenGLHullShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Hull shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsOpenGLWithHullShaders::GfxCmd_HullShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_HullShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_HULL>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Hull shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithHullShaders::GfxCmd_HullShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_HullShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_HULL>(gfxResources, gfxResourcesCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Hull shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithHullShaders::GfxCmd_HullShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_HullShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_HULL>(gfxSamplerStates, gfxSamplerStatesCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Domain shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Creates a new Domain GPU shader with specified parameters.
	//! @param gfxDomainShaderPtr Pointer for output.
	//! @param gfxShaderCreationInfo Creation information for the Domain shader.
	//! @param gfxDomainArrayLayout input layout of the vertex shader input data.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithDomainShaders::GfxImm_DomainShaderCreate(IGraphicsDomainShader** const gfxDomainShaderPtr
		, IGraphicsShaderCreationInfo const* const gfxShaderCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_DomainShaderCreate(gfxDomainShaderPtr, gfxShaderCreationInfo, gfxDomainArrayLayout)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Domain shader type.
		auto gfxShaderCreationInfoCopy(*gfxShaderCreationInfo);
		gfxShaderCreationInfoCopy.gfxShaderType = IGRAPHICS_SHADER_TYPE_DOMAIN;
		*gfxDomainShaderPtr = GD_NEW(IGraphicsOpenGLDomainShader, &gfxShaderCreationInfoCopy);
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified uniform buffers into Domain shader input. Layout of the buffers in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxUniformBuffers Pointer to the uniform buffers list.
	//! @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	GDAPI void IGraphicsOpenGLWithDomainShaders::GfxCmd_DomainShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_DomainShaderBindUniformBuffers(gfxCommandList, gfxUniformBuffers, gfxUniformBuffersCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_DOMAIN>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified resources into Domain shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxResources Pointer to the resources list.
	//! @param gfxResourcesCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithDomainShaders::GfxCmd_DomainShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_DomainShaderBindResources(gfxCommandList, gfxResources, gfxResourcesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_DOMAIN>(gfxResources, gfxResourcesCount);
	}

	// ------------------------------------------------------------------------------------------
	//! Binds specified samplers into Domain shader input. Layout of the resources in
	//! shader and in parameters should be preserved.
	//! @param gfxCommandList Command list into which this command would be written.
	//! @param gfxSamplers Pointer to the samplers list.
	//! @param gfxSamplersCount Number of the resources to bind.
	GDAPI void IGraphicsOpenGLWithDomainShaders::GfxCmd_DomainShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSamplerState const* const* const gfxSamplerStates, SizeTp const gfxSamplerStatesCount)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_DomainShaderBindSamplers(gfxCommandList, gfxSamplerStates, gfxSamplerStatesCount)))
			return;
#endif	// if GD_DEBUG

		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_DOMAIN>(gfxSamplerStates, gfxSamplerStatesCount);
	}

#endif	// if GD_FALSE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Compute shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
#if GD_FALSE

	//! @todo Provide functionality for compute shaders.

#endif	// if GD_FALSE

GD_NAMESPACE_END
