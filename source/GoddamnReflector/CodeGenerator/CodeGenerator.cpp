/// ==========================================================================================
/// CodeGenerator.cpp: GoddamnC++ formatted code generator implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/CodeGenerator/CodeGenerator.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// CPPCodeGenerator class.
	/// Provides basic formatted code generation and writing it to specified stream.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API ().
	/// ------------------------------------------------------------------------------------------

	CPPCodeGenerator::CPPCodeGenerator(UniquePtr<OutputStream>&& Stream) : Stream(Forward<UniquePtr<OutputStream>>(Stream)) 
	{
	}

	CPPCodeGenerator::~CPPCodeGenerator()
	{
	}

	void CPPCodeGenerator::WriteLexem(Lexem const* const CurrentLexem)
	{
		switch (CurrentLexem->GetContentType())
		{
		case GD_LEXEM_CONTENT_TYPE_IDENTIFIER:
			this->Stream->Write(reinterpret_cast<UInt8 const*>(CurrentLexem->GetRawData().CStr()), 0, CurrentLexem->GetRawData().GetSize());
			break;
		}
	}

	class CPPDummyCodeGeneratorType final : public CPPCodeGenerator
	{
	public:
		GDINL CPPDummyCodeGeneratorType() : CPPCodeGenerator(nullptr) { }
		GDINL virtual ~CPPDummyCodeGeneratorType() { }
		GDINL virtual void WriteLexem(Lexem const* const CurrentLexem) override final { }
		GDINL virtual void WriteBrace(StreamedLexerID const PDID) override final { }
		GDINL virtual void WriteRefOrPtr(StreamedLexerID const PDID) override final { }
	};	// class CPPDummyCodeGeneratorType

	UniquePtr<CPPCodeGenerator> const CPPDummyCodeGenerator = new CPPDummyCodeGeneratorType();

GD_NAMESPACE_END
