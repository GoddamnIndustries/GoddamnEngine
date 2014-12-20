/// ==========================================================================================
/// CodeGenerator.h: GoddamnC++ formatted code generator interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_REFLECTOR_CODE_GENERATOR
#define GD_REFLECTOR_CODE_GENERATOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.h>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>

GD_NAMESPACE_BEGIN

	class OutputStream;

	/// Provides basic formatted code generation and writing it to specified stream.
	class CPPCodeGenerator
	{
	private:
		UniquePtr<OutputStream> Stream;
		size_t Identations = 0;

	public /*Class API*/:
		GDINT explicit CPPCodeGenerator(UniquePtr<OutputStream>&& Stream);
		GDINT virtual ~CPPCodeGenerator();

		GDINT virtual void WriteLexem(Lexem const* const CurrentLexem);
		
		GDINT virtual void WriteBrace(StreamedLexerID const PDID) { GD_NOT_IMPLEMENTED(); }
		GDINT virtual void WriteRefOrPtr(StreamedLexerID const PDID) { GD_NOT_IMPLEMENTED(); }
	};	// class CPPCodeGenerator

	extern UniquePtr<CPPCodeGenerator> const CPPDummyCodeGenerator;

GD_NAMESPACE_END

#endif
