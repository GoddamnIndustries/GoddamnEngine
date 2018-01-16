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
 * File contains implementation for OpenGL(ES) graphics interface: pipeline states.
 */ 
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGL.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a new GPU pipeline with specified parameters.
	 * @param gfxPipelineCreateInfo Creation information for the pipeline.
	 */
	GDAPI IGraphicsOpenGLPipelineState::IGraphicsOpenGLPipelineState(IGraphicsPipelineStateCreateInfo const* const gfxPipelineStateCreateInfo)
		: IGraphicsPipelineState(gfxPipelineStateCreateInfo)
		, m_GLProgramPipelineID(0)
	{
		glUseProgram(0);
		glGenProgramPipelines(1, &m_GLProgramPipelineID);
		glBindProgramPipeline(m_GLProgramPipelineID);
		if (PipelineVertexShader != nullptr)
		{
			// Binding our vertex shader (if it exist).
			auto const gfxVertexShader = static_cast<IGraphicsOpenGLVertexShader const*>(PipelineVertexShader);
			glUseProgramStages(m_GLProgramPipelineID, GL_VERTEX_SHADER_BIT, gfxVertexShader->m_GLShaderProgramID);
		}
		if (PipelinePixelShader != nullptr)
		{
			// Binding our pixel ('Fragment' in OpenGL terms) shader (if it exist).
			auto const gfxFragmentShader = static_cast<IGraphicsOpenGLPixelShader const*>(PipelinePixelShader);
			glUseProgramStages(m_GLProgramPipelineID, GL_FRAGMENT_SHADER_BIT, gfxFragmentShader->m_GLShaderProgramID);
		}
		if (PipelineGeometryShader != nullptr)
		{
			// Binding our geometry shader (if it exist).
			auto const gfxGeometryShader = static_cast<IGraphicsOpenGLGeometryShader const*>(PipelineGeometryShader);
			glUseProgramStages(m_GLProgramPipelineID, GL_GEOMETRY_SHADER_BIT, gfxGeometryShader->m_GLShaderProgramID);
		}

#if !GD_RELEASE
		// Validating program pipeline..
		// No validation is performed in the release builds: see Apple's best practices in OpenGL development:
		// https://developer.apple.com/library/ios/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/BestPracticesforShaders/BestPracticesforShaders.html
		GLint glPipelineLogLength = 0;
		glValidateProgramPipeline(m_GLProgramPipelineID);
		glGetProgramPipelineiv(m_GLProgramPipelineID, GL_INFO_LOG_LENGTH, &glPipelineLogLength);
		if (glPipelineLogLength > 0)
		{
			BaseString<GLchar> glPipelineValidationLog(static_cast<SizeTp>(glPipelineLogLength));
			glGetProgramInfoLog(m_GLProgramPipelineID, static_cast<GLsizei>(glPipelineLogLength), nullptr, glPipelineValidationLog.CStr());
			Debug::LogErrorFormat("Validation of the program pipeline object proceeded with the following log: \n%s"
				, glPipelineValidationLog.CStr());

			glBindProgramPipeline(0);
			glDeleteProgramPipelines(1, &m_GLProgramPipelineID);
			return;
		}
#endif	// if !GD_RELEASE

		// Cleaning up..
		glBindProgramPipeline(0);
	}

	/*!
	 * Deletes the specified GPU pipeline.
	 */
	GDAPI IGraphicsOpenGLPipelineState::~IGraphicsOpenGLPipelineState()
	{
		glBindProgramPipeline(0);
		glDeleteProgramPipelines(1, &m_GLProgramPipelineID);
	}

	/*!
	 * Creates a new GPU pipeline with specified parameters.
	 *
	 * @param gfxPipelinePtr Pointer for output.
	 * @param gfxPipelineCreateInfo Creation information for the pipeline.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsOpenGLWithPipelineStates::GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
		, IGraphicsPipelineStateCreateInfo const* const gfxPipelineCreateInfo)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxPipelinePtr != nullptr);
		GD_ARG_VERIFY(gfxPipelineCreateInfo != nullptr);
#endif	// if GD_DEBUG

		*gfxPipelinePtr = gd_new IGraphicsOpenGLPipelineState(gfxPipelineCreateInfo);
		return IResult::Ok;
	}

	/*!
	 * Binds specified GPU pipeline into active GPU pipeline.
	 * 
	 * @param gfxCommandList Command list into which this command would be written.
	 * @param gfxPipeline Pointer to the pipeline.
	 */
	GDAPI void IGraphicsOpenGLWithPipelineStates::GfxCmd_PipelineBind(IGraphicsCommandList* const gfxCommandList
		, IGraphicsPipelineState* const gfxPipeline)
	{
		auto const glPipelineState = static_cast<IGraphicsOpenGLPipelineState*>(gfxPipeline);
		if (glPipelineState != nullptr)
		{
			// Binding the pipeline..
			glBindProgramPipeline(glPipelineState->m_GLProgramPipelineID);
		}
		else
		{
			// Unbinding everything..
			glBindProgramPipeline(0);
		}
	}

GD_NAMESPACE_END
