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
 * File contains Implementation for OpenGL 4.3 Implementation of the graphics interface: pipeline states.
 */ 
#include <GoddamnEngine/Engine/Interface/GraphicsOpenGL/GraphicsOpenGL.h>
//#include <GoddamnEngine/Core/OutputDevice/OutputDevice.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Pipeline state object.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsOpenGLPipelineState);

	// ------------------------------------------------------------------------------------------
	//! Creates a new GPU pipeline with specified parameters.
	//! @param gfxPipelineCreationInfo Creation information for the pipeline.
	GDAPI IGraphicsOpenGLPipelineState::IGraphicsOpenGLPipelineState(IGraphicsPipelineStateCreationInfo const* const gfxPipelineStateCreationInfo)
		: IGraphicsPipelineState(gfxPipelineStateCreationInfo), m_GLProgramPipelineID(0)
	{
		glUseProgram(0);
		glGenProgramPipelines(1, &m_GLProgramPipelineID);
		glBindProgramPipeline(m_GLProgramPipelineID);
		if (gfxPipelineStateCreationInfo->PipelineVertexShader != nullptr)
		{
			// Binding our vertex shader (if it exist).
			auto const gfxVertexShader = static_cast<IGraphicsOpenGLVertexShader const*>(gfxPipelineStateCreationInfo->PipelineVertexShader);
			glUseProgramStages(m_GLProgramPipelineID, GL_VERTEX_SHADER_BIT, gfxVertexShader->m_GLShaderProgramID);
		}
		if (gfxPipelineStateCreationInfo->PipelinePixelShader != nullptr)
		{
			// Binding our pixel ('Fragment' in OpenGL terms) shader (if it exist).
			auto const gfxFragmentShader = static_cast<IGraphicsOpenGLPixelShader const*>(gfxPipelineStateCreationInfo->PipelinePixelShader);
			glUseProgramStages(m_GLProgramPipelineID, GL_FRAGMENT_SHADER_BIT, gfxFragmentShader->m_GLShaderProgramID);
		}
		if (gfxPipelineStateCreationInfo->PipelineGeometryShader != nullptr)
		{
			// Binding our geometry shader (if it exist).
			auto const gfxGeometryShader = static_cast<IGraphicsOpenGLGeometryShader const*>(gfxPipelineStateCreationInfo->PipelineGeometryShader);
			glUseProgramStages(m_GLProgramPipelineID, GL_GEOMETRY_SHADER_BIT, gfxGeometryShader->m_GLShaderProgramID);
		}
#if GD_FALSE
		if (gfxPipelineStateCreationInfo->PipelineHullShader != nullptr)
		{
			// Binding our hull ('Tessellation Control' in OpenGL terms) shader (if it exist).
			auto const gfxTesselationControlShader = static_cast<IGraphicsOpenGLHullShader const*>(gfxPipelineStateCreationInfo->PipelineHullShader);
			glUseProgramStages(GLProgramPipelineID, GL_TESS_CONTROL_SHADER_BIT, gfxTesselationControlShader->GLShaderProgramID);
		}
		if (gfxPipelineStateCreationInfo->PipelineDomainShader != nullptr)
		{
			// Binding our domain ('Tessellation Evaluation' in OpenGL terms) shader (if it exist).
			auto const gfxTesselationEvaluationShader = static_cast<IGraphicsOpenGLDomainShader const*>(gfxPipelineStateCreationInfo->PipelineDomainShader);
			glUseProgramStages(GLProgramPipelineID, GL_TESS_EVALUATION_SHADER_BIT, gfxTesselationEvaluationShader->GLShaderProgramID);
		}
#endif	// if GD_FALSE

#if !GD_RELEASE
		// Validating program pipeline..
		// No validation is performed in the release builds: see Apple's best practices in OpenGL development:
		// https://developer.apple.com/library/ios/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/BestPracticesforShaders/BestPracticesforShaders.html
		GLint glPipelineLogLength = 0;
		glValidateProgramPipeline(m_GLProgramPipelineID);
		glGetProgramPipelineiv(m_GLProgramPipelineID, GL_INFO_LOG_LENGTH, &glPipelineLogLength);
		if (glPipelineLogLength > 1)
		{
			BaseString<GLchar> glPipelineValidationLog(static_cast<SizeTp>(glPipelineLogLength));
			glGetProgramInfoLog(m_GLProgramPipelineID, static_cast<GLsizei>(glPipelineLogLength), nullptr, glPipelineValidationLog.CStr());
		//	ConsoleDevice->LogFormat("Validation of the program pipeline object proceeded with the following log: \n%s", glPipelineValidationLog.CStr());

			glBindProgramPipeline(0);
			glDeleteProgramPipelines(1, &m_GLProgramPipelineID);
			return;
		}
#endif	// if !GD_RELEASE

		// Cleaning up..
		glBindProgramPipeline(0);
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes the current GPU pipeline.
	GDAPI IGraphicsOpenGLPipelineState::~IGraphicsOpenGLPipelineState()
	{
		glBindProgramPipeline(0);
		glDeleteProgramPipelines(1, &m_GLProgramPipelineID);
	}

	// ------------------------------------------------------------------------------------------
	//! Creates a new GPU pipeline with specified parameters.
	//! @param gfxPipelinePtr Pointer for output.
	//! @param gfxPipelineCreationInfo Creation information for the pipeline.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWithPipelineStates::GfxImm_PipelineCreate(IGraphicsPipelineState** const gfxPipelinePtr
		, IGraphicsPipelineStateCreationInfo const* const gfxPipelineCreationInfo)
	{
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxImm_PipelineCreate(gfxPipelinePtr, gfxPipelineCreationInfo)))
			return IResult::InvalidArguments;
#endif	// if GD_DEBUG

		*gfxPipelinePtr = GD_NEW(IGraphicsOpenGLPipelineState, gfxPipelineCreationInfo);
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
#if GD_DEBUG
		if (!GD_IGRAPHICS_CHECK_ARGS(GfxCmd_PipelineBind(gfxCommandList, gfxPipeline)))
			return;
#endif	// if GD_DEBUG

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
