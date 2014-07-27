/// ==========================================================================================
/// OGLShader.cpp: OpenGL shaders implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.02.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLShader.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLBuffer.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// HRIShaderProgram class.
	/// Shader program OpenGL implementation.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION(HRIOGLShaderProgram, HRIShaderProgram, GDINT);
	HRIOGLShaderProgram::~HRIOGLShaderProgram()
	{
		if (self->ProgramID != 0) {
			auto const& GL = HROGLInterface::GetInstance().Driver;
			GL.DeleteProgram(self->ProgramID);
		}
	}

	void HRIOGLShaderProgram::BindShaderProgram() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		if (self->ProgramID == 0) {	// Program was not linked.
			/*We need to mutate shader programe variable. */
			if ((self->ProgramID = GL.CreateProgram()) == 0) {
				throw HRIOGLException("Failed to create the shader program.");
			}

			HRIOGLVertexShader const* const VertexShader = object_cast<HRIOGLVertexShader const*>(self->GetProgramVertexShader());
			if (VertexShader != nullptr) {	// All other shaders are only useful is vertex shader exists.
				GL.AttachShader(ProgramID, VertexShader->GetVertexShaderID());
#if (!defined(GD_HRI_OGL_ES))
				HRIOGLTessCtrlShader const* const TessCtrlShader = object_cast<HRIOGLTessCtrlShader const*>(self->GetProgramTessellationControlShader());
				if (TessCtrlShader != nullptr) {	// Tesselation evaluation is useless without tesselation control.
					GL.AttachShader(ProgramID, TessCtrlShader->GetTessCtrlShaderID());
					HRIOGLTessEvalShader const* const TessEvalShader = object_cast<HRIOGLTessEvalShader const*>(self->GetProgramTessellationEvaluationShader());
					if (TessEvalShader != nullptr) {
						GL.AttachShader(ProgramID, TessEvalShader->GetTessEvalShaderID());
					}
				}
#endif	// if (!defined(GD_HRI_OGL_ES))
				HRIOGLFragmentShader const* const TessEvalShader = object_cast<HRIOGLFragmentShader const*>(self->GetProgramFragmentShader());
				if (TessEvalShader != nullptr) {
					GL.AttachShader(ProgramID, TessEvalShader->GetFragmentShaderID());
				}
			}

			GLint LinkingStatus = GL_FALSE;
			GL.LinkProgram(self->ProgramID);
			GL.GetProgramiv(self->ProgramID, GL_LINK_STATUS, &LinkingStatus);
			if (LinkingStatus != GL_TRUE) {
				GLint LinkingResultLogLength = 0;
				GL.GetProgramiv(self->ProgramID, GL_INFO_LOG_LENGTH, &LinkingResultLogLength);
				if (LinkingResultLogLength > 0) {
					String LinkingResultLog(nullptr, static_cast<size_t>(LinkingResultLogLength));
					GL.GetProgramInfoLog(self->ProgramID, static_cast<GLsizei>(LinkingResultLogLength), nullptr, LinkingResultLog.CStr());
					throw HRIOGLException(String("Shader linking failed with following log: ") + LinkingResultLog);
				} else {
					throw HRIOGLException("Unknown error accured while linking shader program.");
				}
			}
		}

		GL.UseProgram(self->ProgramID);
		GD_HRI_OGL_CHECK_ERRORS("Failed to bind shader program");
	}

	void HRIOGLShaderProgram::UnbindShaderProgram() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.UseProgram(0);
	}

	/// ==========================================================================================
	/// HRIOGLShaderInstance class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLShaderInstance, HRIShaderInstance, GDINT, nullptr);

	HRIOGLShaderInstance::HRIOGLShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc) : HRIShaderInstance(InstanceDesc)
	{	// Caching all inner constant buffer.
		for (auto const Location : IterateChildObjects<HRIShaderParamLocation const>(self)) {
			if (Location->LocationDesc->LocationType == GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER) {
				HRIOGLShaderParamLocationConstantBuffer const* const ConstantBufferLocation = object_cast<HRIOGLShaderParamLocationConstantBuffer const*>(Location);
				HRIOGLConstantBuffer const* const ConstantBuffer = object_cast<HRIOGLConstantBuffer const*>(ConstantBufferLocation->GetBuffer());
				self->ShaderConstantBuffers.PushLast(ConstantBuffer->GetBufferID());
			}
		}
	}

	void HRIOGLShaderInstance::BindInstance() const
	{
		GLuint ConstantBufferBindingPoint = 0;
		auto const& GL = HROGLInterface::GetInstance().Driver;
		for (auto const ConstantBufferID : self->ShaderConstantBuffers) {
			GL.BindBufferBase(GL_UNIFORM_BUFFER, ConstantBufferBindingPoint, ConstantBufferID);
			GD_HRI_OGL_CHECK_ERRORS("Failed to bind uniform buffer ('BindBufferBase' failed)");
			++ConstantBufferBindingPoint;
		}
	}

	/// ==========================================================================================
	/// HRIOGLVertexShader class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLVertexShader, HRIVertexShader, GDINT, nullptr);
	HRIOGLVertexShader::HRIOGLVertexShader(HRIShaderCtorInfo const& CtorInfo) : HRIVertexShader(CtorInfo)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		if ((self->VertexShaderID = GL.CreateShader(GL_VERTEX_SHADER)) == 0) {
			throw HRIOGLException("Failed to create a Vertex shader.");
		}

		GLint CompilationStatus = GL_FALSE;
		GLint const CreationDataSize = static_cast<GLint>(CtorInfo.CreationDataSize);
		GL.ShaderSource(self->VertexShaderID, 1, reinterpret_cast<GLchar const* const*>(&CtorInfo.CreationData), &CreationDataSize);
		GD_HRI_OGL_CHECK_ERRORS("Failed to pass Vertex Shader source ('ShaderSource' failed)");
		GL.CompileShader(self->VertexShaderID);
		GL.GetShaderiv(self->VertexShaderID, GL_COMPILE_STATUS, &CompilationStatus);
		if (CompilationStatus != GL_TRUE) {
			GLint CompilationResultLogLength = 0;
			GL.GetShaderiv(self->VertexShaderID, GL_INFO_LOG_LENGTH, &CompilationResultLogLength);
			if (CompilationResultLogLength > 0) {
				String CompilationResultLog(nullptr, static_cast<size_t>(CompilationResultLogLength));
				GL.GetShaderInfoLog(self->VertexShaderID, static_cast<GLsizei>(CompilationResultLogLength), nullptr, CompilationResultLog.CStr());
				throw HRIOGLException(String("Vertex shader compilation failed with following log: ") + CompilationResultLog);
			} else {
				throw HRIOGLException("Unknown error accured while compiling Vertex shader.");
			}
		}
	}

	HRIOGLVertexShader::~HRIOGLVertexShader()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteProgram(self->VertexShaderID);
	}

	void HRIOGLVertexShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->BindShaderProgram();
		if (TheShaderInstance != nullptr) {
			HRIOGLShaderInstance const* const ShaderInstance = object_cast<HRIOGLShaderInstance const*>(TheShaderInstance);
			ShaderInstance->BindInstance();
		}
	}

	void HRIOGLVertexShader::UnbindShader() const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->UnbindShaderProgram();
	}

#if (!defined(GD_HRI_OGL_ES))

	/// ==========================================================================================
	/// HRIOGLTessCtrlShader class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLTessCtrlShader, HRITessellationControlShader, GDINT, nullptr);
	HRIOGLTessCtrlShader::HRIOGLTessCtrlShader(HRIShaderCtorInfo const& CtorInfo) : HRITessellationControlShader(CtorInfo)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		if ((self->TessCtrlShaderID = GL.CreateShader(GL_TESS_CONTROL_SHADER)) == 0) {
			throw HRIOGLException("Failed to create a Tesselation Control shader.");
		}

		GLint CompilationStatus = GL_FALSE;
		GLint const CreationDataSize = static_cast<GLint>(CtorInfo.CreationDataSize);
		GL.ShaderSource(self->TessCtrlShaderID, 1, reinterpret_cast<GLchar const* const*>(&CtorInfo.CreationData), &CreationDataSize);
		GD_HRI_OGL_CHECK_ERRORS("Failed to pass Tesselation Control Shader source ('ShaderSource' failed)");
		GL.CompileShader(self->TessCtrlShaderID);
		GL.GetShaderiv(self->TessCtrlShaderID, GL_COMPILE_STATUS, &CompilationStatus);
		if (CompilationStatus != GL_TRUE) {
			GLint CompilationResultLogLength = 0;
			GL.GetShaderiv(self->TessCtrlShaderID, GL_INFO_LOG_LENGTH, &CompilationResultLogLength);
			if (CompilationResultLogLength > 0) {
				String CompilationResultLog(nullptr, static_cast<size_t>(CompilationResultLogLength));
				GL.GetShaderInfoLog(self->TessCtrlShaderID, static_cast<GLsizei>(CompilationResultLogLength), nullptr, CompilationResultLog.CStr());
				throw HRIOGLException(String("Tesselation Control shader compilation failed with following log: ") + CompilationResultLog);
			} else {
				throw HRIOGLException("Unknown error accured while compiling Tesselation Control shader.");
			}
		}
	}

	HRIOGLTessCtrlShader::~HRIOGLTessCtrlShader()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteProgram(self->TessCtrlShaderID);
	}

	void HRIOGLTessCtrlShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->BindShaderProgram();
		if (TheShaderInstance != nullptr) {
			HRIOGLShaderInstance const* const ShaderInstance = object_cast<HRIOGLShaderInstance const*>(TheShaderInstance);
			ShaderInstance->BindInstance();
		}
	}

	void HRIOGLTessCtrlShader::UnbindShader() const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->UnbindShaderProgram();
	}

	/// ==========================================================================================
	/// HRIOGLTessEvalShader class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLTessEvalShader, HRITessellationEvaluationShader, GDINT, nullptr);
	HRIOGLTessEvalShader::HRIOGLTessEvalShader(HRIShaderCtorInfo const& CtorInfo) : HRITessellationEvaluationShader(CtorInfo)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		if ((self->TessEvalShaderID = GL.CreateShader(GL_TESS_EVALUATION_SHADER)) == 0) {
			throw HRIOGLException("Failed to create a Tesselation Evaluation shader.");
		}

		GLint CompilationStatus = GL_FALSE;
		GLint const CreationDataSize = static_cast<GLint>(CtorInfo.CreationDataSize);
		GL.ShaderSource(self->TessEvalShaderID, 1, reinterpret_cast<GLchar const* const*>(&CtorInfo.CreationData), &CreationDataSize);
		GD_HRI_OGL_CHECK_ERRORS("Failed to pass Tesselation Evaluation Shader source ('ShaderSource' failed)");
		GL.CompileShader(self->TessEvalShaderID);
		GL.GetShaderiv(self->TessEvalShaderID, GL_COMPILE_STATUS, &CompilationStatus);
		if (CompilationStatus != GL_TRUE) {
			GLint CompilationResultLogLength = 0;
			GL.GetShaderiv(self->TessEvalShaderID, GL_INFO_LOG_LENGTH, &CompilationResultLogLength);
			if (CompilationResultLogLength > 0) {
				String CompilationResultLog(nullptr, static_cast<size_t>(CompilationResultLogLength));
				GL.GetShaderInfoLog(self->TessEvalShaderID, static_cast<GLsizei>(CompilationResultLogLength), nullptr, CompilationResultLog.CStr());
				throw HRIOGLException(String("Tesselation Evaluation shader compilation failed with following log: ") + CompilationResultLog);
			} else {
				throw HRIOGLException("Unknown error accured while compiling Tesselation Evaluation shader.");
			}
		}
	}

	HRIOGLTessEvalShader::~HRIOGLTessEvalShader()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteProgram(self->TessEvalShaderID);
	}

	void HRIOGLTessEvalShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->BindShaderProgram();
		if (TheShaderInstance != nullptr) {
			HRIOGLShaderInstance const* const ShaderInstance = object_cast<HRIOGLShaderInstance const*>(TheShaderInstance);
			ShaderInstance->BindInstance();
		}
	}

	void HRIOGLTessEvalShader::UnbindShader() const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->UnbindShaderProgram();
	}

#endif	// if (!defined(GD_HRI_OGL_ES))

	/// ==========================================================================================
	/// HRIOGLFragmentShader class.
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION_C(HRIOGLFragmentShader, HRIFragmentShader, GDINT, nullptr);
	HRIOGLFragmentShader::HRIOGLFragmentShader(HRIShaderCtorInfo const& CtorInfo) : HRIFragmentShader(CtorInfo)
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		if ((self->FragmentShaderID = GL.CreateShader(GL_FRAGMENT_SHADER)) == 0) {
			throw HRIOGLException("Failed to create a Fragment shader.");
		}

		GLint CompilationStatus = GL_FALSE;
		GLint const CreationDataSize = static_cast<GLint>(CtorInfo.CreationDataSize);
		GL.ShaderSource(self->FragmentShaderID, 1, reinterpret_cast<GLchar const* const*>(&CtorInfo.CreationData), &CreationDataSize);
		GD_HRI_OGL_CHECK_ERRORS("Failed to pass Fragment Shader source ('ShaderSource' failed)");
		GL.CompileShader(self->FragmentShaderID);
		GL.GetShaderiv(self->FragmentShaderID, GL_COMPILE_STATUS, &CompilationStatus);
		if (CompilationStatus != GL_TRUE) {
			GLint CompilationResultLogLength = 0;
			GL.GetShaderiv(self->FragmentShaderID, GL_INFO_LOG_LENGTH, &CompilationResultLogLength);
			if (CompilationResultLogLength > 0) {
				String CompilationResultLog(nullptr, static_cast<size_t>(CompilationResultLogLength));
				GL.GetShaderInfoLog(self->FragmentShaderID, static_cast<GLsizei>(CompilationResultLogLength), nullptr, CompilationResultLog.CStr());
				throw HRIOGLException(String("Fragment shader compilation failed with following log: ") + CompilationResultLog);
			} else {
				throw HRIOGLException("Unknown error accured while compiling Fragment shader.");
			}
		}
	}

	HRIOGLFragmentShader::~HRIOGLFragmentShader()
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		GL.DeleteProgram(self->FragmentShaderID);
	}

	void HRIOGLFragmentShader::BindShader(HRIShaderInstance const* const TheShaderInstance) const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->BindShaderProgram();
		if (TheShaderInstance != nullptr) {
			HRIOGLShaderInstance const* const ShaderInstance = object_cast<HRIOGLShaderInstance const*>(TheShaderInstance);
			ShaderInstance->BindInstance();
		}
	}

	void HRIOGLFragmentShader::UnbindShader() const
	{
		HRIOGLShaderProgram const* ShaderProgram = object_cast<HRIOGLShaderProgram const*>(self->GetShaderProgram());
		ShaderProgram->UnbindShaderProgram();
	}

	/// ==========================================================================================
	/// HROGLInterface class.
	/// ==========================================================================================

	HRIShaderParamLocation* HROGLInterface::CreateShaderParamLocationConstantBuffer(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
	{
		return new HRIOGLShaderParamLocationConstantBuffer(ShaderInstance, LocationDesc);
	}

	HRIShaderParamLocation* HROGLInterface::CreateShaderParamLocationResources(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc)
	{
		return nullptr;
	//	return new HRIOGLShaderParamLocationResources(ShaderInstance, LocationDesc);
	}

	HRIShaderInstance* HROGLInterface::CreateShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc)
	{
		return new HRIOGLShaderInstance(InstanceDesc);
	}

	HRIVertexShader* HROGLInterface::CreateVertexShader(HRIShaderCtorInfo const& CtorInfo) { return new HRIOGLVertexShader  (CtorInfo); }
	HRIHullShader  * HROGLInterface::CreateHullShader  (HRIShaderCtorInfo const& CtorInfo) { return new HRIOGLTessCtrlShader(CtorInfo); }
	HRIDomainShader* HROGLInterface::CreateDomainShader(HRIShaderCtorInfo const& CtorInfo) { return new HRIOGLTessEvalShader(CtorInfo); }
	HRIPixelShader * HROGLInterface::CreatePixelShader (HRIShaderCtorInfo const& CtorInfo) { return new HRIOGLFragmentShader(CtorInfo); }

	HRIShaderProgram* HROGLInterface::CreateShaderProgram()
	{
		return new HRIOGLShaderProgram();
	}

GD_NAMESPACE_END