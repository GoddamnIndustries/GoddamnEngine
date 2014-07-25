/// ==========================================================================================
/// OGLShader.h: OpenGL shaders implementation interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.02.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_OGL_SHADER
#define GD_ENGINE_RENDERER_IMPL_OGL_SHADER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLRenderer.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLTexture2D.h>

GD_NAMESPACE_BEGIN

	/// Shader program OpenGL implementation.
	class HRIOGLShaderProgram final : public HRIShaderProgram
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLShaderProgram, HRIShaderProgram, GDINT);
		GLuint mutable ProgramID = 0;

	public:
		GDINL explicit HRIOGLShaderProgram() { }
		GDINT virtual ~HRIOGLShaderProgram();
		GDINT void BindShaderProgram() const;
		GDINT void UnbindShaderProgram() const;
	};	// class HRIOGLShaderProgram

	/// Standard cbuffer location is quite enough.
	using HRIOGLShaderParamLocationConstantBuffer = HRIShaderParamLocationConstantBufferNative;

	/// Shader instance implementation for OpenGL.
	class HRIOGLShaderInstance : public HRIShaderInstance
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLShaderInstance, HRIShaderInstance, GDINT);
		StackVector<GLuint, GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT> ShaderConstantBuffers;

	public:
		GDINT explicit HRIOGLShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc);
		GDINL virtual ~HRIOGLShaderInstance() { }
		GDINT void BindInstance() const;
	};	// class HRID3D11ShaderInstance

	/// Vertex shader implementation for OGL.
	class HRIOGLVertexShader final : public HRIVertexShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLVertexShader, HRIVertexShader, GDINT);
		GLuint VertexShaderID = 0;

	private:
		GDINT void CreateShader(HRIShaderCtorInfo const& CtorInfo);
		GDINT void CreateInputLayout(HRIShaderCtorInfo const& CtorInfo);

	public:
		GDINT explicit HRIOGLVertexShader(HRIShaderCtorInfo const& CtorInfo);
		GDINT virtual ~HRIOGLVertexShader();
		GDINT virtual void BindShader(HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader() const final;
		GDINL GLuint GetVertexShaderID() const { return self->VertexShaderID; }
	};	// class HRIOGLVertexShader

#if (!defined(GD_HRI_OGL_ES))
	/// Hull shader implementation for OGL.
	class HRIOGLTessCtrlShader final : public HRIHullShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLTessCtrlShader, HRIHullShader, GDINT);
		GLuint TessCtrlShaderID = 0;

	public:
		GDINT explicit HRIOGLTessCtrlShader(HRIShaderCtorInfo const& CtorInfo);
		GDINT virtual ~HRIOGLTessCtrlShader();
		GDINT virtual void BindShader(HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader() const final;
		GDINL GLuint GetTessCtrlShaderID() const { return self->TessCtrlShaderID; }
	};	// class HRIOGLTessCtrlShader

	/// Domain shader implementation for OGL.
	class HRIOGLTessEvalShader final : public HRIDomainShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLTessEvalShader, HRIDomainShader, GDINT);
		GLuint TessEvalShaderID = 0;

	public:
		GDINT explicit HRIOGLTessEvalShader(HRIShaderCtorInfo const& CtorInfo);
		GDINT virtual ~HRIOGLTessEvalShader();
		GDINT virtual void BindShader(HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader() const final;
		GDINL GLuint GetTessEvalShaderID() const { return self->TessEvalShaderID; }
	};	// class HRIOGLTessEvalShader
#endif	// if (!defined(GD_HRI_OGL_ES))

	/// Pixel shader implementation for OGL.
	class HRIOGLFragmentShader final : public HRIPixelShader
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIOGLFragmentShader, HRIPixelShader, GDINT);
		GLuint FragmentShaderID = 0;

	public:
		GDINT explicit HRIOGLFragmentShader(HRIShaderCtorInfo const& CtorInfo);
		GDINT virtual ~HRIOGLFragmentShader();
		GDINT virtual void BindShader(HRIShaderInstance const* const ShaderInstance) const final;
		GDINT virtual void UnbindShader() const final;
		GDINL GLuint GetFragmentShaderID() const { return self->FragmentShaderID; }
	};	// class HRIOGLFragmentShader

GD_NAMESPACE_END

#endif
