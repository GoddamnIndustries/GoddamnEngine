//////////////////////////////////////////////////////////////////////////
/// RefectionBuilder.cc: reflection builder main source file.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnReflector/Reflector.hh>

#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.hh>
#include <GoddamnEngine/Core/Containers/Map/Map.hh>

#include <GoddamnEngine/Core/IO/Path/Path.hh>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.hh>

#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.hh>

GD_NAMESPACE_BEGIN
	
	/// ==========================================================================================
	/// CPPBaseParser class.
	/// Contains useful methods for basic GoddamnC++ headers parsing.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Private class API:
	/// ------------------------------------------------------------------------------------------

	/// Attempts to read next lexem from input stream.
	/// @returns True if lexem was succesfully read.
	bool CPPBaseParser::TryReadNextLexem()
	{
		if (self->Lexer->GetNextLexem(&self->CurrentLexem, self->ProvideSimpleParsing))
		{	// Reading succeded, we need to skip comments comments until read token.
			if (self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_COMMENT))
			{	// Comment coming. Maybe we need to store it somewhere or ... 
				return self->TryReadNextLexem();
			}
			else if (self->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_EOS))
			{	// End-Of-Stream coming.
				return false;
			}

			return true;
		}

		return false;
	}

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	/// Initializes new basic GoddamnC++ parser.
	/// @param Toolchain Corrseponding toolchain.
	/// @param Input     Shared pointer on input stream that contains GoddamnC++ data.
	CPPBaseParser::CPPBaseParser(IToolchain* const Toolchain, SharedPtr<InputStream> const& Input)
		: IToolchainTool(Toolchain)
		, Lexer(new StreamedLexer(Toolchain, Input.GetPointer(), StreamedLexerDefaultOptions::GetDefaultOptionsForCpp()))
		, Input(Input)
	{
	}

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Basic lexem pipeline and expectings):
	/// ------------------------------------------------------------------------------------------

	/// Expects a match of current lexem content type with specified one.
	/// @param ContentType The expected lexem content type.
	/// @returns True if current lexem content type mathes with specified one.
	bool CPPBaseParser::TryExpectLexem(LexemContentType const ContentType)
	{
		return (self->CurrentLexem.GetContentType() == ContentType);
	}

	/// Expects a match of current lexem content type with specified one.
	/// If lexem does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// @param ContentType The expected lexem content type.
	/// @returns True if current lexem content type mathes with specified one.
	bool CPPBaseParser::ExpectLexem(LexemContentType const ContentType)
	{
		if (!self->TryExpectLexem(ContentType))
		{	// Unexpected lexem type.
			CPPBaseParserErrorDesc static const UnexpectedLexemError("unexpected %s. Expected %s.");
			self->RaiseError(UnexpectedLexemError, LexemContentTypeToString(self->CurrentLexem.GetContentType()), LexemContentTypeToString(ContentType));
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	/// Expects a match of current lexem content type and parsed data ID (PDID) with specified ones.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	/// @returns True if current lexem content type and parsed data ID mathes with specified one.
	bool CPPBaseParser::TryExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (self->TryExpectLexem(ContentType))
			if (self->CurrentLexem.GetProcessedDataID() == ID)
				return true;
		return false;
	}

	/// Expects a match of current lexem content type and parsed data ID (PDID) with specified ones.
	/// If lexem does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// If lexem matches with content type, but does not matches with parsed data ID (PDID), then raises 'unexpected Existing-PDID. Expected-PDID expected' error.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	/// @returns True if current lexem content type and parsed data ID mathes with specified one.
	bool CPPBaseParser::ExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (self->ExpectLexem(ContentType))
		{	// Next lexem exists and has expected content type.
			if (self->CurrentLexem.GetProcessedDataID() != ID)
			{	// Unexpected lexem value.
				CPPBaseParserErrorDesc static const UnexpectedLexemValueError("unexpected '%s'.");
				self->RaiseError(UnexpectedLexemValueError, self->CurrentLexem.GetRawData().CStr());
				self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

				return false;
			}

			return true;
		}

		return false;
	}

	/// Expects next lexem from input stream.
	/// @returns True if lexem was succesfully read.
	bool CPPBaseParser::TryExpectNextLexem()
	{
		return self->TryReadNextLexem();
	}

	/// Expects next lexem from input stream. 
	/// If lexem does not exists then raises 'unexpected End-Of-Stream' error.
	/// @returns True if lexem was succesfully read.
	bool CPPBaseParser::ExpectNextLexem()
	{
		if (!self->TryExpectNextLexem())
		{	// Unexpected end of stream while reading lexem.
			CPPBaseParserErrorDesc static const EndOfStreamInVariableDeclError("unexpected End-Of-Stream.");
			self->RaiseError(EndOfStreamInVariableDeclError);
			self->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	/// Expects next lexem from input stream and a match of lexem content type with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// @param ContentType The expected lexem content type.
	/// @returns True if lexem was succesfully read and mathes with specified content type.
	bool CPPBaseParser::TryExpectNextLexem(LexemContentType const ContentType)
	{
		if (self->ExpectNextLexem())
			return self->TryExpectLexem(ContentType);
		return false;
	}

	/// Expects next lexem from input stream and a match of lexem content type with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// If lexem exists, but does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// @param ContentType The expected lexem content type.
	/// @returns True if lexem was succesfully read and mathes with specified content type.
	bool CPPBaseParser::ExpectNextLexem(LexemContentType const ContentType)
	{
		if (self->ExpectNextLexem())
			return self->ExpectLexem(ContentType);
		return false;
	}

	/// Expects next lexem from input stream, and a match of lexem content type with specified one, and a match of lexem parsed data ID with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	/// @returns True if lexem was succesfully read and mathes with specified content type and specified parsed data ID.
	bool CPPBaseParser::TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (self->TryExpectNextLexem(ContentType))
			if (self->CurrentLexem.GetProcessedDataID() == ID)
				return true;
		return false;
	}

	/// Expects next lexem from input stream, and a match of lexem content type with specified one, and a match of lexem parsed data ID (PDID) with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// If lexem exists, but does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// If lexem exists, matches with content type, but does not matches with parsed data ID, then raises 'unexpected Existing-PDID. Expected-PDID expected' error.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	/// @returns True if lexem was succesfully read and mathes with specified content type and specified parsed data ID.
	bool CPPBaseParser::ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (self->ExpectNextLexem())
			return self->ExpectLexem(ContentType, ID);
		return false;
	}

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Annotations parsing).
	/// ------------------------------------------------------------------------------------------

	/// Reads data from input stream until comes up with annotation - idenitifier starting with specified prefix.
	/// @param ExpectedAnnotationPrefix Prefix of expected annotation idenitifier.
	/// @return True if skipped to next annotation otherwise false if next annotation was not found.
	bool CPPBaseParser::TrySkipToNextAnnotation(String const& ExpectedAnnotationPrefix /* = "$GD_" */)
	{
		do
		{
			if (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_IDENTIFIER) // This is idenitifier lexem.
				if (strncmp(self->CurrentLexem.GetRawData().CStr(), ExpectedAnnotationPrefix.CStr(), ExpectedAnnotationPrefix.GetSize()) == 0) // This lexem matches with prefix.
					return true;
		} while (self->TryReadNextLexem());
		return false;
	}

	/// Processes next found annotation in input stream.
	/// @param ExpectedAnnotationPrefix Prefix of expected annotation idenitifier.
	/// @return True if annotation was succesfully processed.
	bool CPPBaseParser::ProcessNextAnnotation(String const& ExpectedAnnotationPrefix /* = "$GD_" */)
	{
		if (!self->TrySkipToNextAnnotation(ExpectedAnnotationPrefix))
			return false;

		SharedPtr<CPPAnnotationParser> AnnotationParser = CPPAnnotationParserSpawner::SpawnAnnotationParser(self->CurrentLexem.GetRawData(), nullptr);
		if (AnnotationParser.GetPointer() != nullptr)
		{	// Enabling full-featured parser.
			self->ProvideSimpleParsing = false;
			bool const AnnotationParsingResult = AnnotationParser->ParseAnnotation(self);
			self->ProvideSimpleParsing = true;
			return AnnotationParsingResult;
		}
	
		return false;
	}

	/// ==========================================================================================
	/// CPPAnnotationParser class.
	/// Abstract interface for GoddamnC++ annotation parser.
	/// ==========================================================================================

	/// Parses annotation params.
	/// @param BaseParser Parser that provides low lever source parsing.
	/// @returns True if annotation argumnts were succesfully parsed.
	bool CPPAnnotationParser::ParseAnnotationParams(CPPBaseParser* const BaseParser)
	{
		using namespace StreamedLexerDefaultOptions;

		if (!BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN)) return false;
		if (!BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END))
		{	// Annotation contains parameters
			for (;;)
			{	// Parsing annotation parameters.
				UniquePtr<CPPAnnotationParamParser> const AnnotationParamParser = self->SpawnParamParser(BaseParser->GetCurrentLexem().GetRawData());
				if (AnnotationParamParser == nullptr)
				{	// Failed to create argument parser.
					CPPBaseParserErrorDesc static const InvalidAnnotationParameterSpecified("invalid annotation parameter '%s' specified.");
					BaseParser->RaiseError(InvalidAnnotationParameterSpecified, BaseParser->GetCurrentLexem().GetRawData().CStr());
					BaseParser->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
					return false;
				}

				if ((BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN)))
				{	// Found assigment expression.
					if (!BaseParser->ExpectNextLexem()) return false;
					for (;;)
					{	// Loop made to parse multiple paramaters with '|' operator.
						if (!BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) return false;
						if (!AnnotationParamParser->ParseArgument(BaseParser, self, BaseParser->GetCurrentLexem().GetRawData()))
						{	// Failed to create argument parser.
							CPPBaseParserErrorDesc static const InvalidAnnotationParameterValueSpecifiedError("invalid annotation parameter`s value '%s' specified.");
							BaseParser->RaiseError(InvalidAnnotationParameterValueSpecifiedError, BaseParser->GetCurrentLexem().GetRawData().CStr());
							BaseParser->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
							return false;
						}

						if (!BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR)) break;
						if (!BaseParser->ExpectNextLexem()) return false;
					}
				}
				
				if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END))
				{	// Annotation parametrs end here.
					if (!BaseParser->ExpectNextLexem()) return false;
					return true;
				}
				else
				{	// Comma parameters separator (for values without assigment expression).
					if (!(BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA))) return false;
					if (!BaseParser->ExpectNextLexem()) return false;
				}
			}
		}
		else
		{	// Annotation does not contains parameters.
			if (!BaseParser->ExpectNextLexem()) return false;
			return true;
		}
	}

	/// Parses upcoming annotation.
	/// @param BaseParser Parser that provides low lever source parsing.
	/// @returns True if annotation was succesfully parsed.
	bool CPPAnnotationParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{
		if (!self->ParseAnnotationParams(BaseParser))
			return false;

		return true;
	}

	/// ==========================================================================================
	/// CPPAnnotationParserSpawner class.
	/// Provides registering all annotation-parser-derived classes and spawning them while parsing.
	/// ==========================================================================================

	/// Contains specialized annotation parser registry.
	Map<HashSumm, CPPAnnotationParserSpawner::CtorProc> CPPAnnotationParserSpawner::AnnotationParsersRegistry;

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	/// Registers new specialized annotation parser in registry.
	/// @param Name Name of annotaion idenitifier this parser processes.
	/// @param Ctor Procedure that constructs instance of this annotation parser.
	/// @note It is not recommended to use this method directly: use RAI CPPAnnotationParserSpawner::Node<T> class instead.
	void CPPAnnotationParserSpawner::RegisterAnnotationParser(String const& Name, CtorProc const Ctor)
	{
		HashSumm const NameHash = Name.GetHashSumm();
		if (CPPAnnotationParserSpawner::AnnotationParsersRegistry.FindFirstElement(NameHash) != SIZE_MAX)
			GD_ASSERT_FALSE("Parser for annotation '%s' was already defined.", Name.CStr());

		CPPAnnotationParserSpawner::AnnotationParsersRegistry.PushLast(NameHash, Ctor);
	}

	/// Spawns new specialized annotation parser.
	/// @param Name Name of annotaion idenitifier of required parser.
	/// @param Args Packed constructor params May be null pointer.
	/// @returns Pointer to parser if it was succesfullt created.
	SharedPtr<CPPAnnotationParser> CPPAnnotationParserSpawner::SpawnAnnotationParser(String const& Name, CPPAnnotationCtorArgs const* const Args)
	{
		HashSumm const NameHash = Name.GetHashSumm();
		size_t const FoundIndex = CPPAnnotationParserSpawner::AnnotationParsersRegistry.FindFirstElement(NameHash);
		if (FoundIndex != SIZE_MAX)
			return CPPAnnotationParserSpawner::AnnotationParsersRegistry.GetElementAt(FoundIndex).Value(Args);
		return SharedPtr<CPPAnnotationParser>(nullptr);
	}

	/// ==========================================================================================
	/// CPPAnnotationParamParserSpawner class.
	/// Provides registering all annotation-params-parser-derived classes and spawning them while 
	/// parsing.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	/// Registers new specialized annotation param parser in registry.
	/// @param Name Name of annotaion param this parser processes.
	/// @param Ctor Procedure that constructs instance of this annotation param parser.
	/// @note It is not recommended to use this method directly: use RAI CPPAnnotationParserParamSpawner::Node<T> class instead.
	void CPPAnnotationParamParserSpawner::RegisterAnnotationParamParser(String const& Name, CtorProc const Ctor)
	{
		HashSumm const NameHash = Name.GetHashSumm();
		if (self->AnnotationParamParsersRegistry.FindFirstElement(NameHash) != SIZE_MAX)
			GD_ASSERT_FALSE("Parser for paremeter of an annotation '%s' was already defined.", Name.CStr());

		self->AnnotationParamParsersRegistry.PushLast(NameHash, Ctor);
	}

	/// Spawns new specialized annotation param parser.
	/// @param Name Name of annotaion idenitifier of required parser.
	/// @param Args Packed constructor params May be null pointer.
	/// @returns Pointer to parser if it was succesfullt created.
	UniquePtr<CPPAnnotationParamParser> CPPAnnotationParamParserSpawner::SpawnAnnotationParamParser(String const& Name, CPPAnnotationCtorArgs const* const Args) const
	{
		HashSumm const NameHash = Name.GetHashSumm();
		size_t const FoundIndex = self->AnnotationParamParsersRegistry.FindFirstElement(NameHash);
		if (FoundIndex != SIZE_MAX)
			return self->AnnotationParamParsersRegistry.GetElementAt(FoundIndex).Value(Args);
		return UniquePtr<CPPAnnotationParamParser>(nullptr);
	}

#if 0
	bool CPPHeaderReflectorImpl::OpenOutputStreamsAndWriteInitializers()
	{
		self->HeaderInputStream = new FileInputStream(self->HeaderPath);
		if (!self->HeaderInputStream->CanRead())
		{
			CPPBaseParserErrorDesc static const HeaderNotFoundError("Unable to locate target header at (%s).");
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
			CPPBaseParserErrorDesc static const CannonWriteHeaderHeaderError("Failed to create header`s generated header output file (%s).");
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
			CPPBaseParserErrorDesc static const CannonWriteHeaderSourceError("Failed to create header`s generated source output file (%s).");
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
#endif

GD_NAMESPACE_END

#include <ctime>
#include <GoddamnReflector/Enumeration/Enumeration.hh>

int main(int const ArgumensCount, char const* const* const ParamsList)
{
#if (defined(GD_NAMESPACE))
	using namespace GD_NAMESPACE;
#endif	// if (defined(GD_NAMESPACE))

#if (defined(GD_DEBUG))
	static char const* const HeaderPath = R"(D:\GoddamnEngine\source\GoddamnEngine\Engine\Renderer\Shader\Shader.hh)";
#else
	GD_ASSERT(ArgumensCount >= 2);
	char const* const HeaderPath = ParamsList[1];
#endif	// if (defined(GD_DEBUG))

	clock_t const StartTime = clock();

	IToolchain static Toolchain;
	SharedPtr<InputStream> static HeaderInputStream(new FileInputStream(HeaderPath));

	CPPBaseParser static BaseParser(&Toolchain, HeaderInputStream);
	while(BaseParser.ProcessNextAnnotation());

	clock_t const TotalTime = clock() - StartTime;
	printf("\nGeneration took %f seconds.", static_cast<float>(TotalTime) / 1000.0f);

	return 0;
}
