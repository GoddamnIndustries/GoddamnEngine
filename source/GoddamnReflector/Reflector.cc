//////////////////////////////////////////////////////////////////////////
/// RefectionBuilder.cc: reflection builder main source file.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Include.hh>

#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.hh>
#include <GoddamnEngine/Core/Containers/Map/Map.hh>

#include <GoddamnEngine/Core/IO/Path/Path.hh>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.hh>

#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.hh>

GD_NAMESPACE_BEGIN
	
	using namespace StreamedLexerDefaultOptions;
	typedef Str CPPHeaderReflectorErrorDesc;

	struct CPPReflectable;
	struct CPPEnumeration;
	struct CPPProperty;
	struct CPPFunction;

	enum CPPHeaderReflectorExceptionCodes : ToolchainException
	{
		GD_CPP_REFLECTOR_EXCEPTION_MODULE = 1000L,
		GD_CPP_REFLECTOR_EXCEPTION_SYNTAX = +GD_CPP_REFLECTOR_EXCEPTION_MODULE + 1L, 
	};	// enum StreamedLexerExceptionCodes

	enum CPPEnumerationFlags : UInt64
	{
		GD_CPP_ENUMERATION_UNKNOWN                    = 0,
		GD_CPP_ENUMERATION_TYPE                       = GD_BIT( 0),
		GD_CPP_ENUMERATION_TYPE_ENUMERATION           = GD_BIT( 1),
		GD_CPP_ENUMERATION_TYPE_BITSET                = GD_BIT( 2),
		GD_CPP_ENUMERATION_STRINGIFICATION            = GD_BIT( 3),
		GD_CPP_ENUMERATION_STRINGIFICATION_FULL       = GD_BIT( 4),
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING    = GD_BIT( 5),
		GD_CPP_ENUMERATION_STRINGIFICATION_LOWER_CASE = GD_BIT( 6),
		GD_CPP_ENUMERATION_STRINGIFICATION_UPPER_CASE = GD_BIT( 7),
		GD_CPP_ENUMERATION_STRINGIFICATION_AS_IS_CASE = GD_BIT( 8),
		GD_CPP_ENUMERATION_STRINGIFICATION_PUBLIC     = GD_BIT( 9),
		GD_CPP_ENUMERATION_STRINGIFICATION_INTERNAL   = GD_BIT(10),
	};	// enum CPPEnumerationType

	struct CPPEnumeration final
	{
		UInt64 Flags = GD_CPP_ENUMERATION_UNKNOWN;
		String Name;
		String BaseType;
		UnorderedMap<String, String> Values;
	};	// struct CPPEnumeration

	struct CPPReflectable final
	{
		String Name;
		CPPReflectable const* BaseType = nullptr;
		Vector<String> OtherBaseTypeNames;
		Vector<CPPEnumeration const*> InnerEnumerations;
		Vector<CPPReflectable const*> InnerReflectables;
		Vector<CPPProperty const*> InnerProperties;
		Vector<CPPFunction const*> InnerFunctions;
	};	// struct CPPReflectable

	class CPPHeaderReflectorImpl final : public IToolchainTool
	{
	private /*Class members*/:
		String const HeaderPath;
		UniquePtr<FileInputStream> HeaderInputStream;
		UniquePtr<FileOutputStream> HeaderHeaderOutput;
		UniquePtr<FileOutputStream> HeaderSourceOutput;
		
		String LastCommentLexem;
		UniquePtr<StreamedLexer> HeaderLexer;
		Lexem CurrentLexem;

	public /*Public API*/:
		GDINT ~CPPHeaderReflectorImpl()	{ }
		GDINT  CPPHeaderReflectorImpl(IToolchain* const Toolchain, String const& HeaderPath)
			: IToolchainTool(Toolchain), HeaderPath(HeaderPath)
		{	
		}

		GDINT bool ReflectHeader();

	private /*Private API*/:
		GDINT bool TryReadNextLexem(                                                        );
		GDINT bool TryExpectNextLexem(                                                      );
		GDINT bool    ExpectNextLexem(                                                      );
		GDINT bool TryExpectNextLexem(LexemContentType ContentType                          );
		GDINT bool    ExpectNextLexem(LexemContentType ContentType                          );
		GDINT bool TryExpectNextLexem(LexemContentType ContentType, StreamedLexerId const Id);
		GDINT bool    ExpectNextLexem(LexemContentType ContentType, StreamedLexerId const Id);
		GDINT bool TryExpectLexem(LexemContentType ContentType                              );
		GDINT bool    ExpectLexem(LexemContentType ContentType                              );
		GDINT bool TryExpectLexem(LexemContentType ContentType, StreamedLexerId const Id    );
		GDINT bool    ExpectLexem(LexemContentType ContentType, StreamedLexerId const Id    );

		GDINT bool OpenOutputStreamsAndWriteInitializers();
		GDINT bool ProcessHeaderBody();
		GDINT bool WriteFinalizers();

		GDINT bool ProcessAnnotation();
		GDINT bool ProcessAnnotationParams(UInt64& OutputList, UnorderedMap<String, UInt64> const& Params);
		GDINT bool ProcessReflectableAnnotation();
		GDINT bool ProcessEnumerationAnnotation();
		GDINT bool ProcessPropertyAnnotation();
		GDINT bool ProcessFunctionAnnotation();
	};	// class CPPHeaderReflectorImpl

	bool CPPHeaderReflectorImpl::ReflectHeader()
	{
		if (!self->OpenOutputStreamsAndWriteInitializers())
			return false;

		if (!self->ProcessHeaderBody())
			return false;
		
		if (!self->WriteFinalizers())
			return false;

		return true;
	}

	bool CPPHeaderReflectorImpl::OpenOutputStreamsAndWriteInitializers()
	{
		self->HeaderInputStream = new FileInputStream(self->HeaderPath);
		if (!self->HeaderInputStream->CanRead())
		{
			CPPHeaderReflectorErrorDesc static const HeaderNotFoundError("Unable to locate target header at (%s).");
			self->RaiseError(HeaderNotFoundError, self->HeaderPath.CStr());
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
			return false;
		}

		self->HeaderLexer = new StreamedLexer(self->Toolchain, StreamedLexerDefaultOptions::GetDefaultOptionsForCpp());

		Str static const Copyrights =
			  "//////////////////////////////////////////////////////////////////////////"
			"\n/// %s - some generated reflection code."
			"\n/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved."
			"\n/// "
			"\n/// Note: this file was generated, please do not edit it manually."
			"\n//////////////////////////////////////////////////////////////////////////\n\n";

		String const HeaderPathWithoutExtension = Path::GetDirectoryAndFileNameWithoutExtension(self->HeaderPath);
		String const HeaderHeaderOutputPath = HeaderPathWithoutExtension + ".Generated.hh";
		self->HeaderHeaderOutput = new FileOutputStream(HeaderHeaderOutputPath);
		if (!self->HeaderHeaderOutput->CanWrite())
		{	// Failed to open output.
			CPPHeaderReflectorErrorDesc static const CannonWriteHeaderHeaderError("Failed to create header`s generated header output file (%s).");
			self->RaiseError(CannonWriteHeaderHeaderError, HeaderHeaderOutputPath.CStr());
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
			return false;
		}
		else
		{
			String const HeaderHeaderOutputRelativePath = Path::GetFileName(HeaderHeaderOutputPath);
			String const HeaderHeaderOutputCopyright = String::Format(Copyrights, HeaderHeaderOutputRelativePath.CStr());
			self->HeaderHeaderOutput->WriteCheck(HeaderHeaderOutputCopyright.CStr(), 0, sizeof(Char) * HeaderHeaderOutputCopyright.GetSize());

			Str static const HeaderInitializer =
				  "#pragma once"
				"\n#ifndef __%sGeneratedHeader__"
				"\n#define __%sGeneratedHeader__"
				"\n"
				"\nGD_NAMESPACE_BEGIN"
				"\n";

			String const HeaderPathWithoutDirAndExtension = Path::GetFileName(HeaderPathWithoutExtension);
			String const HeaderHeaderOutputInitializer = String::Format(HeaderInitializer, HeaderPathWithoutDirAndExtension.CStr(), HeaderPathWithoutDirAndExtension.CStr());
			self->HeaderHeaderOutput->WriteCheck(HeaderHeaderOutputInitializer.CStr(), 0, sizeof(Char) * HeaderHeaderOutputInitializer.GetSize());
		}

		String const HeaderSourceOutputPath = HeaderPathWithoutExtension + ".Generated.cc";
		self->HeaderSourceOutput = new FileOutputStream(HeaderSourceOutputPath);
		if (!self->HeaderSourceOutput->CanWrite())
		{
			CPPHeaderReflectorErrorDesc static const CannonWriteHeaderSourceError("Failed to create header`s generated source output file (%s).");
			self->RaiseError(CannonWriteHeaderSourceError, HeaderSourceOutputPath.CStr());
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
			return false;
		}
		else
		{
			String const HeaderSourceOutputRelativePath = Path::GetFileName(HeaderSourceOutputPath);
			String const HeaderSourceOutputCopyright = String::Format(Copyrights, HeaderSourceOutputRelativePath.CStr());
			self->HeaderSourceOutput->WriteCheck(HeaderSourceOutputCopyright.CStr(), 0, sizeof(Char) * HeaderSourceOutputCopyright.GetSize());

			Str static const SourceInitializer =
				  "#include \"%s\""
				"\n"
				"\nGD_NAMESPACE_BEGIN"
				"\n";

			String const HeaderPathWithoutDir = Path::GetFileName(self->HeaderPath);
			String const HeaderSourceOutputInitializer = String::Format(SourceInitializer, HeaderPathWithoutDir.CStr());
			self->HeaderHeaderOutput->WriteCheck(HeaderSourceOutputInitializer.CStr(), 0, sizeof(Char) * HeaderSourceOutputInitializer.GetSize());
		}

		return true;
	}

	bool CPPHeaderReflectorImpl::ProcessHeaderBody()
	{
		while (!self->Toolchain->WasExceptionRaised())
		{
			if (!self->TryReadNextLexem()) return false;
			if  (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_EOS) break;
			if ((self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_IDENTIFIER) && (self->CurrentLexem.GetRawData()[0] == Char('$')))
			{	// This is Goddamn annotation mark.
				if (!self->ProcessAnnotation())
					return false;
			}
		}

		return true;
	}

	bool CPPHeaderReflectorImpl::ProcessAnnotation()
	{
		/**/ if (self->CurrentLexem.GetRawData() == "$GD_REFLECTABLE") return self->ProcessReflectableAnnotation();
		else if (self->CurrentLexem.GetRawData() == "$GD_ENUMERATION") return self->ProcessEnumerationAnnotation();
		else if (self->CurrentLexem.GetRawData() == "$GD_PROPERTY") return self->ProcessPropertyAnnotation();
		else if (self->CurrentLexem.GetRawData() == "$GD_FUNCTION")	return self->ProcessFunctionAnnotation();

		// Some identifier starting with Dollar sign. Nothing unusual.
		return true;
	}

	bool CPPHeaderReflectorImpl::ProcessAnnotationParams(UInt64& OutputList, UnorderedMap<String, UInt64> const& Params)
	{
		if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN)) return false;
		if (!self->ExpectNextLexem(                                                                                   )) return false;
		if (!(self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END )))
		{	// Annotation contains parameters
			for (;;)
			{	// Parsing annotation parameters.
				size_t const AnnotationParamNameIndex = Params.FindFirstElement(self->CurrentLexem.GetRawData());
				if (AnnotationParamNameIndex == SIZE_MAX)
				{	// Invalid annotation parameter name.
					CPPHeaderReflectorErrorDesc static const UnknownAnnotationParameterSpecified("unknown annotation parameter '%s' specified.");
					self->RaiseError(UnknownAnnotationParameterSpecified, self->CurrentLexem.GetRawData().CStr());
					self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
					return false;
				}

				OutputList |= Params.GetElementAt(AnnotationParamNameIndex).Value;

				if (!self->ExpectNextLexem()) return false;
				if ((self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN)))
				{	// Found assigment expression.
					if (!self->ExpectNextLexem()) return false;
					for (;;)
					{	// Loop made to parse multiple paramaters with '|' operator.
						if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) return false;

						size_t const AnnotationParamValueIndex = Params.FindFirstElement(self->CurrentLexem.GetRawData());
						if (AnnotationParamValueIndex == SIZE_MAX)
						{	// Invalid annotation paramter value.
							CPPHeaderReflectorErrorDesc static const UnknownAnnotationParameterValueSpecifiedError("unknown annotation parameter`s '%s' value '%s' specified.");
							self->RaiseError(UnknownAnnotationParameterValueSpecifiedError, Params.GetElementAt(AnnotationParamNameIndex).Key.CStr(), self->CurrentLexem.GetRawData().CStr());
							self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
							return false;
						}

						OutputList |= Params.GetElementAt(AnnotationParamValueIndex).Value;
						if (!self->ExpectNextLexem()) return false;
						if ((self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_OR)))
						{	// Multiple value packed using '|' operator.
							if (!self->ExpectNextLexem()) return false;
							continue;
						}
						if (self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END))
						{	// Annotation parametrs end here (found ')' operator).
							if (!self->ExpectNextLexem()) return false;
							return true;
						}
						if ((self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA)))
						{	// Comma parameters separator (for values that have assigment expression).
							if (!self->ExpectNextLexem()) return false;
							break;
						}
					}
				}
				else if (self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END))
				{	// Annotation parametrs end here.
					if (!self->ExpectNextLexem()) return false;
					return true;
				}
				else
				{	// Comma parameters separator (for values without assigment expression).
					if (!(self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA))) return false;
					if (!self->ExpectNextLexem()) return false;
				}
			}
		}
		else
		{	// Annotation does not contains parameters.
			if (!self->ExpectNextLexem()) return false;
			return true;
		}
	}

	bool CPPHeaderReflectorImpl::ProcessReflectableAnnotation()
	{
		GD_NOT_IMPLEMENTED();
		return false;
	}

	bool CPPHeaderReflectorImpl::ProcessEnumerationAnnotation()
	{
		UniquePtr<CPPEnumeration> Enumeration(new CPPEnumeration());
		UnorderedMap<String, UInt64> static const CPPEnumerationAnnotationParams = {
			{ "Type",            GD_CPP_ENUMERATION_TYPE },
				{ "Enumeration", GD_CPP_ENUMERATION_TYPE_ENUMERATION },
				{ "Bitset",      GD_CPP_ENUMERATION_TYPE_BITSET },
			{ "Stringification", GD_CPP_ENUMERATION_STRINGIFICATION },
				{ "Full",        GD_CPP_ENUMERATION_STRINGIFICATION_FULL },
				{ "Chopped",    GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING },
				{ "LowerCase",   GD_CPP_ENUMERATION_STRINGIFICATION_LOWER_CASE },
				{ "UpperCase",   GD_CPP_ENUMERATION_STRINGIFICATION_UPPER_CASE },
				{ "AsIs",        GD_CPP_ENUMERATION_STRINGIFICATION_AS_IS_CASE },
				{ "Public",      GD_CPP_ENUMERATION_STRINGIFICATION_PUBLIC },
				{ "Internal",    GD_CPP_ENUMERATION_STRINGIFICATION_INTERNAL },
		};
		
		// Pasring annotation parameters and expecting 'enum' keyword.
		if (!self->ProcessAnnotationParams(Enumeration->Flags, CPPEnumerationAnnotationParams)) return false;
		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ENUM)) return false;
		
		// Parsing enumeration identifier name.
		if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) return false;
		Enumeration->Name = self->CurrentLexem.GetRawData();
		
		// Parsing enumeration base type (if it was specified).
		if (!self->ExpectNextLexem()) return false;
		if (self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON))
		{	// Enumeration has it`s own base type.
			if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) return false;
			Enumeration->BaseType = self->CurrentLexem.GetRawData();
			if (!self->ExpectNextLexem()) return false;
		}
		
		// Parsing enumeration body.
		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN)) return false;
		for (;;)
		{	// Expecting '};' as enumeration body end.
			if (!self->   ExpectNextLexem()) return false;
			if ((self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END)))
			{	// Found '}' mark. Now expecting ';'
				if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SEMICOLON))
				{	// Really I do not know conditions where '}' in enumeration definition means something except end of enum.
					// If they are - please, edit this code.
					return false;
				}

				break;
			}

			printf("\n%s", self->CurrentLexem.GetRawData().CStr());
		}

		return true;
	}

	bool CPPHeaderReflectorImpl::ProcessPropertyAnnotation()
	{
		GD_NOT_IMPLEMENTED();
		return false;
	}

	bool CPPHeaderReflectorImpl::ProcessFunctionAnnotation()
	{
		GD_NOT_IMPLEMENTED();
		return false;
	}

	bool CPPHeaderReflectorImpl::WriteFinalizers()
	{
		Char static const HeaderFinalizers[] =
			"\n\t"
			"\nGD_NAMESPACE_END"
			"\n"
			"\n#endif \t// ifndef __xxxGeneratedHeader__"
			"\n";
		
		Char static const SourceFinalizers[] =
			"\n\t"
			"\nGD_NAMESPACE_END"
			"\n";

		self->HeaderHeaderOutput->WriteCheck(HeaderFinalizers, 0, (sizeof(HeaderFinalizers) - 1) * sizeof(Char));
		self->HeaderSourceOutput->WriteCheck(SourceFinalizers, 0, (sizeof(SourceFinalizers) - 1) * sizeof(Char));
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// bool CPPHeaderReflectorImpl::[Try]Expect[Next]Lexem([LexemType], [ID])
	//////////////////////////////////////////////////////////////////////////

	bool CPPHeaderReflectorImpl::TryReadNextLexem()
	{
		if (self->HeaderLexer->GetNextLexem(self->HeaderInputStream.GetPointer(), &self->CurrentLexem))
		{	// Reading succeded, we need to skip comments comments until read token.
			if (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_COMMENT)
			{
				self->LastCommentLexem = self->CurrentLexem.GetRawData();
				return self->TryReadNextLexem();
			}

			return true;
		}

		return false;
	}

	bool CPPHeaderReflectorImpl::TryExpectNextLexem()
	{
		return self->TryReadNextLexem();
	}

	bool CPPHeaderReflectorImpl::ExpectNextLexem()
	{
		if (!self->TryExpectNextLexem())
		{	// Unexpected end of stream while reading lexem.
			CPPHeaderReflectorErrorDesc static const EndOfStreamInVariableDeclError("unexpected End-Of-Stream.");
			self->RaiseError(EndOfStreamInVariableDeclError);
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	bool CPPHeaderReflectorImpl::TryExpectNextLexem(LexemContentType ExpectedContentType)
	{
		if (self->TryExpectNextLexem())
			return self->TryExpectLexem(ExpectedContentType);

		return false;
	}

	bool CPPHeaderReflectorImpl::ExpectNextLexem(LexemContentType ExpectedContentType)
	{
		if (self->ExpectNextLexem())
			return self->ExpectLexem(ExpectedContentType);

		return false;
	}

	bool CPPHeaderReflectorImpl::TryExpectLexem(LexemContentType ExpectedContentType)
	{
		return (self->CurrentLexem.GetContentType() == ExpectedContentType);
	}

	bool CPPHeaderReflectorImpl::ExpectLexem(LexemContentType ExpectedContentType)
	{
		if (!self->TryExpectLexem(ExpectedContentType))
		{	// Unexpected lexem type.
			CPPHeaderReflectorErrorDesc static const UnexpectedLexemError("unexpected %s. Expected %s.");
			self->RaiseError(UnexpectedLexemError, LexemContentTypeToString(self->CurrentLexem.GetContentType()), LexemContentTypeToString(ExpectedContentType));
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	bool CPPHeaderReflectorImpl::TryExpectLexem(LexemContentType ExpectedContentType, StreamedLexerId const ID)
	{
		if (self->TryExpectLexem(ExpectedContentType))
			if (self->CurrentLexem.GetProcessedDataId() == ID)
				return true;
		return false;
	}

	bool CPPHeaderReflectorImpl::ExpectLexem(LexemContentType ExpectedContentType, StreamedLexerId const ID)
	{
		if (self->ExpectLexem(ExpectedContentType))
		{	// Next lexem exists and has expected content type.
			if (self->CurrentLexem.GetProcessedDataId() != ID)
			{	// Unexpected lexem value.
				CPPHeaderReflectorErrorDesc static const UnexpectedLexemValueError("unexpected '%s'.");
				self->RaiseError(UnexpectedLexemValueError, &self->CurrentLexem.GetRawData()[0]);
				self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

				return false;
			}

			return true;
		}

		return false;
	}

	bool CPPHeaderReflectorImpl::ExpectNextLexem(LexemContentType ExpectedContentType, StreamedLexerId const ID)
	{
		if (self->ExpectNextLexem())
		{	// Next lexem exists and has expected content type.
			return self->ExpectLexem(ExpectedContentType, ID);
		}

		return false;
	}

GD_NAMESPACE_END

int main(int const ArgumensCount, char const* const* const ArgumentsList)
{
#if (defined(GD_NAMESPACE))
	using namespace GD_NAMESPACE;
#endif	// if (defined(GD_NAMESPACE))

#if (defined(GD_DEBUG))
	static char const* const HeaderPath = R"(D:\GoddamnEngine\source\GoddamnEngine\Engine\Renderer\Shader\Shader.hh)";
#else
	GD_ASSERT(ArgumensCount >= 2);
	char const* const HeaderPath = ArgumentsList[1];
#endif	// if (defined(GD_DEBUG))

	IToolchain Toolchain;
	if (!CPPHeaderReflectorImpl(&Toolchain, HeaderPath).ReflectHeader())
		return 1;

	return 0;
}
