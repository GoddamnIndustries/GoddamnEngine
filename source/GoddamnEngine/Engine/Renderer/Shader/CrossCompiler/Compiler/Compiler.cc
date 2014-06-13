//////////////////////////////////////////////////////////////////////////
/// Compiler.cc: shader cross compiler generator and compiler implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.06.2014 - Created by James Jhuighuy.
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/Compiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/HLSL/HLSLCompiler.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/GLSL/GLSLCompiler.hh>
#if (defined(GD_PLATFORM_OSX))
#	include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Compiler/Metal/MetalCompiler.hh>
#endif	// if (defined(GD_PLATFORM_OSX))

GD_NAMESPACE_BEGIN	

	typedef Str HRIShaderCrossCompilerCompilerErrorDesc;

	bool HRIShaderCrossCompilerCompiler::GenerateAndCompileShader(OutputStream* const Output, HLSLScope const* const Input, HRIShaderCrossCompilerTarget const Target)
	{
		switch (Target)
		{
		case GD_HRI_SHADERCC_COMPILER_TARGET_HLSL:
			return HLSLCompiler(self->Toolchain).GenerateAndCompileShader(Output, Input);

		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSL410:	// No ES, with constant buffers.
			return GLSLCompiler(self->Toolchain).GenerateAndCompileShader(Output, Input, false, true);

		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES2:	// ES, without constant buffers.
			return GLSLCompiler(self->Toolchain).GenerateAndCompileShader(Output, Input, true, false);

		case GD_HRI_SHADERCC_COMPILER_TARGET_GLSLES3:	// ES, with constant buffers.
			return GLSLCompiler(self->Toolchain).GenerateAndCompileShader(Output, Input, true, true);

#if (defined(GD_PLATFORM_OSX))
		case GD_HRI_SHADERCC_COMPILER_TARGET_METAL:
			return MetalCompiler(self->Toolchain).GenerateAndCompileShader(Output, Input);
#endif	// if (defined(GD_PLATFORM_OSX))
		}

		HRIShaderCrossCompilerCompilerErrorDesc static const UknownTargetError("unknown target specified.");
		self->RaiseError(UknownTargetError);
		self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);
		return false;
	}

GD_NAMESPACE_END
