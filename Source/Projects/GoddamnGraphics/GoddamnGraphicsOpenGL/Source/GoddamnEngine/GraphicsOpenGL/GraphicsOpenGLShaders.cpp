// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLShaders.cpp
 * File contains implementation for OpenGL(ES) graphics interface: shaders.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GLenum static const g_OpenGLShaderTypesTable[IGRAPHICS_SHADER_TYPES_COUNT] = {
		/* IGRAPHICS_SHADER_TYPE_VERTEX   */ GL_VERTEX_SHADER,
		/* IGRAPHICS_SHADER_TYPE_PIXEL    */ GL_FRAGMENT_SHADER,
		/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ GL_GEOMETRY_SHADER,
		/* IGRAPHICS_SHADER_TYPE_HULL     */ GL_TESS_CONTROL_SHADER,
		/* IGRAPHICS_SHADER_TYPE_DOMAIN   */ GL_TESS_EVALUATION_SHADER,
		/* IGRAPHICS_SHADER_TYPE_COMPUTE  */ GL_COMPUTE_SHADER,
		/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ 0,
	};

	static GLchar const* g_OpenGLShaderSourceHeader = R"(
		#version 430 core
		#extension GL_ARB_separate_shader_objects : enable
		#extension GL_ARB_enhanced_layouts : enable
	)";

	// The 'ARB_separate_shader_objects' extension requires to redefine all shader output manually.
	// https://community.amd.com/message/1279274
	// https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL)
	static GLchar const* const g_OpenGLShaderSourceBuiltIns[] = {
		/* IGRAPHICS_SHADER_TYPE_VERTEX */ R"(	
			in int gl_VertexID;
			in int gl_InstanceID;
			out gl_PerVertex {
				vec4  gl_Position;
				float gl_PointSize;
				float gl_ClipDistance[];
			};
		)",

		/* IGRAPHICS_SHADER_TYPE_PIXEL */ R"(	
			in vec4 gl_FragCoord;
		//	in bool gl_FrontFacing;
			in vec2 gl_PointCoord;
			in int  gl_Layer;
			in int  gl_ViewportIndex;
			out float gl_FragDepth;
			out int gl_SampleMask[];
		)",

		/* IGRAPHICS_SHADER_TYPE_GEOMETRY */ R"(
			in int gl_PrimitiveIDIn;
			in int gl_InvocationID;
			in gl_PerVertex {
			  vec4  gl_Position;
			  float gl_PointSize;
			  float gl_ClipDistance[];
			} gl_in[];
			out gl_PerVertex {
				vec4  gl_Position;
				float gl_PointSize;
				float gl_ClipDistance[];
			};
			out int gl_PrimitiveID;
			out int gl_Layer;
			out int gl_ViewportIndex;
		)",

		/* IGRAPHICS_SHADER_TYPE_HULL */ R"(	
			in int gl_PatchVerticesIn;
			in int gl_PrimitiveID;
			in int gl_InvocationID;
			in gl_PerVertex {
				vec4  gl_Position;
				float gl_PointSize;
				float gl_ClipDistance[];
			} gl_in[gl_MaxPatchVertices];
			patch out float gl_TessLevelOuter[4];
			patch out float gl_TessLevelInner[2];
			out gl_PerVertex {
				vec4  gl_Position;
				float gl_PointSize;
				float gl_ClipDistance[];
			} gl_out[];
		)",

		/* IGRAPHICS_SHADER_TYPE_DOMAIN */ R"(	
			in vec3 gl_TessCoord;
			in int  gl_PatchVerticesIn;
			in int  gl_PrimitiveID;
			in gl_PerVertex {
				vec4  gl_Position;
				float gl_PointSize;
				float gl_ClipDistance[];
			} gl_in[gl_MaxPatchVertices];
			out gl_PerVertex {
				vec4  gl_Position;
				float gl_PointSize;
				float gl_ClipDistance[];
			} gl_out[];
		)",

		/* IGRAPHICS_SHADER_TYPE_COMPUTE */ R"(	
			in uvec3 gl_NumWorkGroups;
			in uvec3 gl_WorkGroupID;
			in uvec3 gl_LocalInvocationID;
			in uvec3 gl_GlobalInvocationID;
			in uint  gl_LocalInvocationIndex;
		)",

		/* IGRAPHICS_SHADER_TYPE_UNKNOWN  */ "\0",
	};

	static GLchar const* g_OpenGLShaderSourcePostHeader = R"(
		#line 0
	)";

	/*!
	 * Creates an OpenGL GPU shader with specified parameters.
	 * @param gfxShaderCreateInfo Creation information for the shader.
	 */
	template<typename TIGraphicsShaderBase>
	GDINT IGraphicsOpenGLBaseShader<TIGraphicsShaderBase>::IGraphicsOpenGLBaseShader(IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo)
		: TIGraphicsShaderBase(gfxShaderCreateInfo)
		, m_GLShaderProgramID(0), m_GLShaderProgramType(g_OpenGLShaderTypesTable[ShaderType])
	{
		if (ShaderByteCode != nullptr || (ShaderFlags & IGRAPHICS_SHADER_FLAGS_FORCE_COMPILE_FROM_SOURCE) == 0)
		{
			// This should reduce loading time dramatically.
			GD_NOT_IMPLEMENTED();
		}
		
		// No cache for our shader was found. We need to compile this shader now and cache it's Byte code.
		GLchar const* glShaderSources[] = {
			g_OpenGLShaderSourceHeader,
			g_OpenGLShaderSourceBuiltIns[ShaderType], 
			g_OpenGLShaderSourcePostHeader,
			static_cast<GLchar const*>(ShaderSource),
			"\n"
		};

		m_GLShaderProgramID = glCreateShaderProgramv(m_GLShaderProgramType, GD_ARRAY_LENGTH(glShaderSources), glShaderSources);
#if !GD_RELEASE
		// Validating program and checking shader compilation status..
		// No validation is performed in the release builds: translated from the HLSL shaders
		// are validated in the offline mode in the editor on the translation step. This is just an another useless paranoid check.
		GLint glShaderCompilationLogLength = 0;
		glGetProgramiv(m_GLShaderProgramID, GL_INFO_LOG_LENGTH, &glShaderCompilationLogLength);
		if (glShaderCompilationLogLength > 0)
		{
			BaseString<GLchar> glShaderCompilationLog(static_cast<SizeTp>(glShaderCompilationLogLength));
			glGetProgramInfoLog(m_GLShaderProgramID, static_cast<GLsizei>(glShaderCompilationLogLength)
				, nullptr, glShaderCompilationLog.CStr());
			
			Debug::LogErrorFormat("Validation of the separate shader object of type %d proceeded with the following log: \n%s"
				, ShaderType, glShaderCompilationLog.CStr());

			glUseProgram(0);
			glDeleteProgram(m_GLShaderProgramID);
			return;
		}
#endif	// if !GD_RELEASE
	}

	/*!
	 * Deletes the specified GPU shader.
	 */
	template<typename TIGraphicsShaderBase>
	GDINT IGraphicsOpenGLBaseShader<TIGraphicsShaderBase>::~IGraphicsOpenGLBaseShader()
	{
		glUseProgram(0);
		glBindProgramPipeline(0);
		glDeleteProgram(m_GLShaderProgramID);
	}

	/*!
	 * Binds specified uniform buffers into some shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	template<IGraphicsShaderType TShaderType>
	GDINT void IGraphicsBaseOpenGLWithBaseShaders::GfxCmd_BaseShaderBindUniformBuffers(IGraphicsBuffer const* const* const gfxUniformBuffers
		, SizeTp const gfxUniformBuffersCount)
	{
		auto const glUniformBuffers = reinterpret_cast<IGraphicsOpenGLBuffer const* const*>(gfxUniformBuffers);

		//! @todo Check whether the count of the uniform buffers binding points is large enough for uniform buffers.
		GLint p;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &p);
		GL_MAX_VERTEX_UNIFORM_BLOCKS;
		GL_MAX_FRAGMENT_UNIFORM_BLOCKS;
		GL_MAX_GEOMETRY_UNIFORM_BLOCKS;
		GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS;
		GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS;

		// Binding our buffers array into the uniform buffer slots.
		// We offset each buffer binding point on the constant value - to make all the binding points different for
		// single binded separate shader programs.
		//! @todo Somehow export the offset value out of this block.
		GLuint static const gfxMaxUniformBuffersPerType = 14;
		GLuint static const gfxUniformBufferBindingPointOffset = gfxMaxUniformBuffersPerType * TShaderType;
		for (GLuint i = 0; i < gfxUniformBuffersCount; ++i)
		{
			if (glUniformBuffers != nullptr && glUniformBuffers[i] != nullptr)
			{
				// Binding current buffer...
				auto const glUniformBuffer = glUniformBuffers[i];
				glBindBufferBase(GL_UNIFORM_BUFFER, gfxUniformBufferBindingPointOffset + i, glUniformBuffer->m_GLBufferID);
			}
			else
			{
				// Unbinding current uniform buffer..
				glBindBufferBase(GL_UNIFORM_BUFFER, gfxUniformBufferBindingPointOffset + i, 0);
			}
		}
	}

	/*!
	 * Binds specified resources into some shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	template<IGraphicsShaderType TShaderType>
	GDINT void IGraphicsBaseOpenGLWithBaseShaders::GfxCmd_BaseShaderBindResources(IGraphicsShaderResourceView const* const* const gfxResources
		, SizeTp const gfxResourcesCount)
	{
		auto const glResources = reinterpret_cast<IGraphicsOpenGLShaderResourceView const* const*>(gfxResources);

		// Binding our resources array into the texture/sampler slots.
		// We offset each resource binding point on the constant value - to make all the binding points different for
		// single binded separate shader programs.
		//! @todo Check whether the count of the texture/sampler binding points is large enough for textures.
		//! @todo Somehow export the offset value out of this block.
		GLuint static const gfxMaxResourcesPerType = 8;
		GLuint static const gfxResourceBindingPointOffset = gfxMaxResourcesPerType * TShaderType;
		for (GLuint i = 0; i < gfxResourcesCount; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + gfxResourceBindingPointOffset + i);
			if (glResources != nullptr && glResources[i] != nullptr)
			{
				// Binding current texture...
				auto const glResource = glResources[i];
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

	/*!
	 * Binds specified samplers into some shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	template<IGraphicsShaderType TShaderType>
	GDINT void IGraphicsBaseOpenGLWithBaseShaders::GfxCmd_BaseShaderBindSamplers(IGraphicsSampler const* const* const gfxSamplers
		, SizeTp const gfxSamplersCount)
	{
		auto const glSamplers = reinterpret_cast<IGraphicsOpenGLSampler const* const*>(gfxSamplers);

		// Binding our samplers array into the texture/sampler slots.
		// We offset each texture/sampler binding point on the constant value - to make all the binding points different for
		// single binded separate shader programs.
		//! @todo Check whether the count of the texture/sampler binding points is large enough for samplers.
		//! @todo Somehow export the offset value out of this block.
		GLuint static const gfxMaxSamplersPerType = 8;
		GLuint static const gfxSamplerBindingPointOffset = gfxMaxSamplersPerType * TShaderType;
		for (GLuint i = 0; i < gfxSamplersCount; ++i)
		{
			if (glSamplers != nullptr && glSamplers[i] != nullptr)
			{
				// Binding current sampler state..
				auto const gfxSampler = glSamplers[i];
				glBindSampler(gfxSamplerBindingPointOffset + i, gfxSampler->m_GLSamplerID);
			}
			else
			{
				// Unbinding current sampler state..
				glBindSampler(gfxSamplerBindingPointOffset + i, 0);
			}
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vertex shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
	GDAPI IResult IGraphicsOpenGLWithVertexShaders::GfxImm_VertexShaderCreate(IGraphicsVertexShader** const gfxVertexShaderPtr
		, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo, IGraphicsVertexArrayLayout const* const gfxVertexArrayLayout)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxVertexShaderPtr != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCode != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCodeLength > 0);
#endif	// if GD_DEBUG

		// Input layouts are not used in OpenGL.
		GD_NOT_USED(gfxVertexArrayLayout);

		// Copying parameters and forcedly specifying Vertex shader type.
		auto gfxShaderCreateInfoCopy(*gfxShaderCreateInfo);
		gfxShaderCreateInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_VERTEX;
		
		*gfxVertexShaderPtr = gd_new IGraphicsOpenGLVertexShader(&gfxShaderCreateInfoCopy);
		return IResult::Ok;
	}

	/*!
	 * Binds specified uniform buffers into Vertex shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	GDAPI void IGraphicsOpenGLWithVertexShaders::GfxCmd_VertexShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	/*!
	 * Binds specified resources into Vertex shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsOpenGLWithVertexShaders::GfxCmd_VertexShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxResources, gfxResourcesCount);
	}

	/*!
	 * Binds specified samplers into Vertex shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsOpenGLWithVertexShaders::GfxCmd_VertexShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_VERTEX>(gfxSamplers, gfxSamplersCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pixel shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new Pixel GPU shader with specified parameters and specifies the input layout
	 * of the Pixel GPU shader.
	 *
	 * @param gfxPixelShaderPtr Pointer for output.
	 * @param gfxShaderCreateInfo Creation information for the Pixel shader.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithPixelShaders::GfxImm_PixelShaderCreate(IGraphicsPixelShader** const gfxPixelShaderPtr
		, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxPixelShaderPtr != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCode != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCodeLength > 0);
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Pixel shader type.
		auto gfxShaderCreateInfoCopy(*gfxShaderCreateInfo);
		gfxShaderCreateInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_PIXEL;
		
		*gfxPixelShaderPtr = gd_new IGraphicsOpenGLPixelShader(&gfxShaderCreateInfoCopy);
		return IResult::Ok;
	}

	/*!
	 * Binds specified uniform buffers into Pixel shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	GDAPI void IGraphicsOpenGLWithPixelShaders::GfxCmd_PixelShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	/*!
	 * Binds specified resources into Pixel shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsOpenGLWithPixelShaders::GfxCmd_PixelShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxResources, gfxResourcesCount);
	}

	/*!
	 * Binds specified samplers into Pixel shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsOpenGLWithPixelShaders::GfxCmd_PixelShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_PIXEL>(gfxSamplers, gfxSamplersCount);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Geometry shaders.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new Geometry GPU shader with specified parameters and specifies the input layout
	 * of the Geometry GPU shader.
	 *
	 * @param gfxGeometryShaderPtr Pointer for output.
	 * @param gfxShaderCreateInfo Creation information for the Geometry shader.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithGeometryShaders::GfxImm_GeometryShaderCreate(IGraphicsGeometryShader** const gfxGeometryShaderPtr
		, IGraphicsShaderCreateInfo const* const gfxShaderCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxGeometryShaderPtr != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCode != nullptr);
		GD_ARG_VERIFY(gfxShaderCreateInfo->ShaderSource || gfxShaderCreateInfo->ShaderByteCodeLength > 0);
#endif	// if GD_DEBUG

		// Copying parameters and forcedly specifying Geometry shader type.
		auto gfxShaderCreateInfoCopy(*gfxShaderCreateInfo);
		gfxShaderCreateInfoCopy.ShaderType = IGRAPHICS_SHADER_TYPE_GEOMETRY;

		*gfxGeometryShaderPtr = gd_new IGraphicsOpenGLGeometryShader(&gfxShaderCreateInfoCopy);
		return IResult::Ok;
	}

	/*!
	 * Binds specified uniform buffers into Geometry shader input. Layout of the buffers in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxUniformBuffers Pointer to the uniform buffers list.
	 * @param gfxUniformBuffersCount Number of the uniform buffers to bind.
	 */
	GDAPI void IGraphicsOpenGLWithGeometryShaders::GfxCmd_GeometryShaderBindUniformBuffers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsBuffer const* const* const gfxUniformBuffers, SizeTp const gfxUniformBuffersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindUniformBuffers<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxUniformBuffers, gfxUniformBuffersCount);
	}

	/*!
	 * Binds specified resources into Geometry shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 *
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxResources Pointer to the resources list.
	 * @param gfxResourcesCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsOpenGLWithGeometryShaders::GfxCmd_GeometryShaderBindResources(IGraphicsCommandList* const gfxCommandList
		, IGraphicsShaderResourceView const* const* const gfxResources, SizeTp const gfxResourcesCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindResources<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxResources, gfxResourcesCount);
	}

	/*!
	 * Binds specified samplers into Geometry shader input. Layout of the resources in
	 * shader and in parameters should be preserved.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxSamplers Pointer to the samplers list.
	 * @param gfxSamplersCount Number of the resources to bind.
	 */
	GDAPI void IGraphicsOpenGLWithGeometryShaders::GfxCmd_GeometryShaderBindSamplers(IGraphicsCommandList* const gfxCommandList
		, IGraphicsSampler const* const* const gfxSamplers, SizeTp const gfxSamplersCount)
	{
		GD_NOT_USED(gfxCommandList);
		GfxCmd_BaseShaderBindSamplers<IGRAPHICS_SHADER_TYPE_GEOMETRY>(gfxSamplers, gfxSamplersCount);
	}

GD_NAMESPACE_END
