/// ==========================================================================================
/// RefectionBuilder.cpp: reflection builder main source file.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/Reflector.h>
#include <GoddamnReflector/CodeGenerator/CodeGenerator.h>

#include <GoddamnEngine/Core/IO/Path/Path.h>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>
#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.h>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.h>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>

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
		if (this->Lexer->GetNextLexem(&this->CurrentLexem))	{	// Reading succeded, we need to skip comments comments until read token.
			if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_COMMENT)) {	// Comment coming. Maybe we need to store it somewhere or ... 
				return this->TryReadNextLexem();
			} else if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_EOS))	{	// End-Of-Stream coming.
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
	CPPBaseParser::CPPBaseParser(IToolchain* const Toolchain, UniquePtr<InputStream>&& Input)
		: IToolchainTool(Toolchain)
		, Lexer(new StreamedLexer(Toolchain, Forward<UniquePtr<InputStream>>(Input), StreamedLexerDefaultOptions::GetDefaultOptionsForCpp(), StreamedLexerMode::Basic))
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
		return (this->CurrentLexem.GetContentType() == ContentType);
	}

	/// Expects a match of current lexem content type with specified one.
	/// If lexem does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// @param ContentType The expected lexem content type.
	void CPPBaseParser::ExpectLexem(LexemContentType const ContentType)
	{
		if (!this->TryExpectLexem(ContentType))	{	// Unexpected lexem type.
			CPPBaseParserErrorDesc static const UnexpectedLexemError("unexpected %s. Expected %s.");
			throw CPPParsingException(UnexpectedLexemError.ToString(&this->CurrentLexem, LexemContentTypeToString(this->CurrentLexem.GetContentType()), LexemContentTypeToString(ContentType)));
		}
	}

	/// Expects a match of current lexem content type and parsed data ID (PDID) with specified ones.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	/// @returns True if current lexem content type and parsed data ID mathes with specified one.
	bool CPPBaseParser::TryExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (this->TryExpectLexem(ContentType)) {
			if (this->CurrentLexem.GetProcessedDataID() == ID) {
				return true;
			}
		}

		return false;
	}

	/// Expects a match of current lexem content type and parsed data ID (PDID) with specified ones.
	/// If lexem does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// If lexem matches with content type, but does not matches with parsed data ID (PDID), then raises 'unexpected Existing-PDID. Expected-PDID expected' error.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	void CPPBaseParser::ExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		this->ExpectLexem(ContentType);

		// Next lexem exists and has expected content type.
		if (this->CurrentLexem.GetProcessedDataID() != ID) {	// Unexpected lexem value.
			CPPBaseParserErrorDesc static const UnexpectedLexemValueError("unexpected '%s'.");
			throw CPPParsingException(UnexpectedLexemValueError.ToString(&this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
		}
	}

	/// Expects next lexem from input stream.
	/// @returns True if lexem was succesfully read.
	bool CPPBaseParser::TryExpectNextLexem()
	{
		return this->TryReadNextLexem();
	}

	/// Expects next lexem from input stream. 
	/// If lexem does not exists then raises 'unexpected End-Of-Stream' error.
	void CPPBaseParser::ExpectNextLexem()
	{
		if (!this->TryExpectNextLexem()) {	// Unexpected end of stream while reading lexem.
			CPPBaseParserErrorDesc static const EndOfStreamInVariableDeclError("unexpected End-Of-Stream.");
			throw CPPParsingException(EndOfStreamInVariableDeclError.ToString(&this->CurrentLexem));
		}
	}

	/// Expects next lexem from input stream and a match of lexem content type with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// @param ContentType The expected lexem content type.
	/// @returns True if lexem was succesfully read and mathes with specified content type.
	bool CPPBaseParser::TryExpectNextLexem(LexemContentType const ContentType)
	{
		this->ExpectNextLexem();
		return this->TryExpectLexem(ContentType);
	}

	/// Expects next lexem from input stream and a match of lexem content type with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// If lexem exists, but does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// @param ContentType The expected lexem content type.
	void CPPBaseParser::ExpectNextLexem(LexemContentType const ContentType)
	{
		this->ExpectNextLexem();
		this->ExpectLexem(ContentType);
	}

	/// Expects next lexem from input stream, and a match of lexem content type with specified one, and a match of lexem parsed data ID with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	/// @returns True if lexem was succesfully read and mathes with specified content type and specified parsed data ID.
	bool CPPBaseParser::TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (this->TryExpectNextLexem(ContentType)) {
			if (this->CurrentLexem.GetProcessedDataID() == ID) {
				return true;
			}
		}

		return false;
	}

	/// Expects next lexem from input stream, and a match of lexem content type with specified one, and a match of lexem parsed data ID (PDID) with specified one.
	/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
	/// If lexem exists, but does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
	/// If lexem exists, matches with content type, but does not matches with parsed data ID, then raises 'unexpected Existing-PDID. Expected-PDID expected' error.
	/// @param ContentType The expected lexem content type.
	/// @param ID          The expected lexem parsed data ID (PDID).
	void CPPBaseParser::ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		this->ExpectNextLexem();
		this->ExpectLexem(ContentType, ID);
	}

	/// ------------------------------------------------------------------------------------------
	/// Public class API (C++-specific template/macro identifiers parsing).
	/// ------------------------------------------------------------------------------------------

	/// Parses complex expression. Complex expression can be referenced with something like normal C++ expression. Limitations:
	/// @li Template parameters are not really parsed, analyzer just finds where they begin/end. So something like right shift operator could break everything.
	/// @param Output Output string to which complex expression would be written. May be nullptr.
	void CPPBaseParser::ParseComplexExpression(UniquePtr<CPPCodeGenerator> const& _CodeGenerator /* = nullptr */)
	{
		UniquePtr<CPPCodeGenerator> const& CodeGenerator = ((_CodeGenerator != nullptr) ? _CodeGenerator : CPPDummyCodeGenerator);
		Vector<StreamedLexerID> BracesStack;
		do
		{	// Expression determination based on braces stack.
			using namespace StreamedLexerDefaultOptions;
			if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) {	// Here is our operator. 
				StreamedLexerID const PDID = this->GetCurrentLexem().GetProcessedDataID();
				if (   (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_BEGIN) || (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN  )
					|| (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN) || (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_BEGIN))	{	// Opening brace. 
					BracesStack.PushLast(PDID);
				} else if (!BracesStack.IsEmpty()) {	// In-stack closing braces.
					if (   (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_END) || (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END  )
						|| (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END) || (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_END))	{	// Closing brace. Stack not empty -> matching braces balance.
						this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, *(BracesStack.End() - 1) + 1);
						BracesStack.PopLast();
					} else if (PDID == GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT) {	// >> Operator. Treating is as dual template closing brace.
						if (   (*(BracesStack.End() - 2) != GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_BEGIN)
							|| (*(BracesStack.End() - 1) != GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_BEGIN))	{	// <...< ... >> Mistmatch.
							this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_END);
						}

						BracesStack.Resize(BracesStack.GetSize() - 2);
					}
				} else {	// Out of stack. This should be our end of expression.
					return;
				}
			}
			CodeGenerator->WriteLexem(&this->GetCurrentLexem());
		} while (this->TryExpectNextLexem());
	}

	/// Parses complex type name. Complex type names are ones with namespace definitions and template parameters. Limitations: 
	/// @li Template parameters are not really parsed, analyzer just finds where they begin/end. So something like right shift operator could break everything.
	/// @param Output Output string to which complex type name would be written. May be nullptr.
	/// @returns True if complex type name was successfully parsed.
	bool CPPBaseParser::ParseComplexTypename(UniquePtr<CPPCodeGenerator> const& _CodeGenerator /* = nullptr */)
	{
		UniquePtr<CPPCodeGenerator> const& CodeGenerator = ((_CodeGenerator != nullptr) ? _CodeGenerator : CPPDummyCodeGenerator);

		// Idenitifier name of type.
		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
		CodeGenerator->WriteLexem(&this->GetCurrentLexem());

		// Trying to analyze template parameters.
		using namespace StreamedLexerDefaultOptions;
		if (this->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_BEGIN)) {	// Here comes template parameters. Inside template parameters are only types.
			for (;;) {	// Trying to parse existing template parameter.
				if (!this->ParseComplexTypenameWithCVs(_CodeGenerator)) {
					return false;
				}

				if (!this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA)) {
					this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_END);
				}
			
				CodeGenerator->WriteLexem(&this->GetCurrentLexem());
			}
		}

		return true;
	}

	/// Parses complex type name. Complex type name can be referenced with as type names with const/volatile cvs, references, pointers, template arguments.
	/// Here is only limitation: type names should be written in GoddamnCoding standart. Here comes limitations: 
	/// @li Type modifiers should follow type names. As sequence, type names could not be defined with struct/class keyword.
	/// @li Type modifiers cannot be wrapped with macroes. That means that analyzer could not determine that MY_CONST, defined as '#define MY_CONST const' is real 'const' and deside end of type name.
	/// @li Template parameters are not really parsed, analyzer just finds where they begin/end. So something like right shift operator could break everything.
	/// @param Output Output string to which complex type name would be written. May be nullptr.
	/// @returns True if complex type name was successfully parsed.
	bool CPPBaseParser::ParseComplexTypenameWithCVs(UniquePtr<CPPCodeGenerator> const& _CodeGenerator /* = nullptr */)
	{
		this->ParseComplexTypename(_CodeGenerator);
		UniquePtr<CPPCodeGenerator> const& CodeGenerator = ((_CodeGenerator != nullptr) ? _CodeGenerator : CPPDummyCodeGenerator);

		do
		{	// Parsing CVs.
			using namespace StreamedLexerDefaultOptions;
			StreamedLexerID const PDID = this->GetCurrentLexem().GetProcessedDataID();
			if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD)) {	// Parsing all const-s, volatile-s, signess and etc.
				if (   (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST    )
					|| (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONSTEXPR)
					|| (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOLATILE )
					|| (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIGNED   )
					|| (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNSIGNED )) {
					break;
				}
			} else if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) {	// Parsing references and pointers.
				if (   (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_REFERENCING  )
					|| (PDID != GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DEREFERENCING)) {
					break;
				}
			} else {	// Something unexpected. Lets think that here complex type name ends.	
				break;
			}

			CodeGenerator->WriteLexem(&this->GetCurrentLexem());
		} while (this->TryExpectNextLexem());
		return true;
	}

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Annotations parsing).
	/// ------------------------------------------------------------------------------------------

	/// Reads data from input stream until comes up with annotation - idenitifier starting with specified prefix.
	/// @param ExpectedAnnotationPrefix Prefix of expected annotation idenitifier.
	/// @return True if skipped to next annotation otherwise false if next annotation was not found.
	CPPResult CPPBaseParser::TrySkipToNextAnnotation(String const& ExpectedAnnotationPrefix /* = "$GD_" */)
	{
		do {
			if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) { // This is idenitifier lexem.
				if (strncmp(this->CurrentLexem.GetRawData().CStr(), ExpectedAnnotationPrefix.CStr(), ExpectedAnnotationPrefix.GetSize()) == 0) { // This lexem matches with prefix.
					return GD_CPP_RESULT_SUCCEEDED;
				}
			}
		} while (this->TryReadNextLexem());
		return GD_CPP_RESULT_EMPTY;
	}

	/// Processes next found annotation in input stream.
	/// @param ExpectedAnnotationPrefix Prefix of expected annotation idenitifier.
	/// @return True if annotation was succesfully processed.
	CPPResult CPPBaseParser::ParseAnnotation(handle const Args /* = nullptr */, String const& ExpectedAnnotationPrefix /* = "$GD_" */)
	{
		CPPResult const SkippingResult = this->TrySkipToNextAnnotation(ExpectedAnnotationPrefix);
		if (SkippingResult != GD_CPP_RESULT_SUCCEEDED) {
			return SkippingResult;
		}

		SharedPtr<CPPAnnotationParser> AnnotationParser = CPPAnnotationParserSpawner::SpawnAnnotationParser(this->CurrentLexem.GetRawData(), Args);
		if (AnnotationParser.GetPointer() != nullptr) {	// Enabling full-featured parser.
			this->Lexer->SwitchMode(StreamedLexerMode::Advanced);
			AnnotationParser->ParseAnnotation(this);
			// Switching back to simple one.
			this->Lexer->SwitchMode(StreamedLexerMode::Basic);
			return GD_CPP_RESULT_SUCCEEDED;
		}

		// Trying to read next lexem not to make parser stuck on current one.
		this->TryExpectNextLexem();
		return GD_CPP_RESULT_EMPTY;
	}

	/// ==========================================================================================
	/// CPPAnnotationParser class.
	/// Abstract interface for GoddamnC++ annotation parser.
	/// ==========================================================================================

	/// Parses annotation params.
	/// @param BaseParser Parser that provides low lever source parsing.
	/// @returns True if annotation argumnts were succesfully parsed.
	void CPPAnnotationParser::ParseAnnotationParams(CPPBaseParser* const BaseParser)
	{
		using namespace StreamedLexerDefaultOptions;

		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN);
		if (!BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END))	{	// Annotation contains parameters
			for (;;) {	// Parsing annotation parameters.
				UniquePtr<CPPAnnotationParamParser> const AnnotationParamParser = this->SpawnParamParser(BaseParser->GetCurrentLexem().GetRawData());
				if (AnnotationParamParser == nullptr) {	// Failed to create argument parser.
					CPPBaseParserErrorDesc static const InvalidAnnotationParameterSpecified("invalid annotation parameter '%s' specified.");
					throw CPPParsingException(InvalidAnnotationParameterSpecified.ToString(&BaseParser->GetCurrentLexem(), BaseParser->GetCurrentLexem().GetRawData().CStr()));
				}

				if ((BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN))) {	// Found assigment expression.
					BaseParser->ExpectNextLexem(); 
					for (;;) {	// Loop made to parse multiple paramaters with '|' operator.
						BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
						AnnotationParamParser->ParseArgument(BaseParser, this, BaseParser->GetCurrentLexem().GetRawData());
						if (!BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR)) {
							break;
						}

						BaseParser->ExpectNextLexem();
					}
				}

				if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END)) {	// Annotation parametrs end here.
					BaseParser->ExpectNextLexem();
				} else {	// Comma parameters separator (for values without assigment expression).
					BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA);
					BaseParser->ExpectNextLexem();
				}
			}
		} else {	// Annotation does not contains parameters.
			BaseParser->ExpectNextLexem();
		}
	}

	/// Parses upcoming annotation.
	/// @param BaseParser Parser that provides low lever source parsing.
	/// @returns True if annotation was succesfully parsed.
	void CPPAnnotationParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{
		this->ParseAnnotationParams(BaseParser);
	}

	/// ==========================================================================================
	/// CPPAnnotationParserSpawner class.
	/// Provides registering all annotation-parser-derived classes and spawning them while parsing.
	/// ==========================================================================================

	/// Contains specialized annotation parser registry.
	Map<HashCode, CPPAnnotationParserSpawner::CtorProc> CPPAnnotationParserSpawner::AnnotationParsersRegistry;

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	/// Registers new specialized annotation parser in registry.
	/// @param Name Name of annotaion idenitifier this parser processes.
	/// @param Ctor Procedure that constructs instance of this annotation parser.
	/// @note It is not recommended to use this method directly: use RAI CPPAnnotationParserSpawner::Node<T> class instead.
	void CPPAnnotationParserSpawner::RegisterAnnotationParser(String const& Name, CtorProc const Ctor)
	{
		HashCode const NameHash = Name.GetHashCode();
		if (CPPAnnotationParserSpawner::AnnotationParsersRegistry.FindFirstElement(NameHash) != SIZE_MAX) {
			GD_DEBUG_ASSERT_FALSE("Parser for annotation '%s' was already defined.", Name.CStr());
		}

		CPPAnnotationParserSpawner::AnnotationParsersRegistry.PushLast(NameHash, Ctor);
	}

	/// Spawns new specialized annotation parser.
	/// @param Name Name of annotaion idenitifier of required parser.
	/// @param Args Packed constructor params May be null pointer.
	/// @returns Pointer to parser if it was succesfullt created.
	SharedPtr<CPPAnnotationParser> CPPAnnotationParserSpawner::SpawnAnnotationParser(String const& Name, handle const Args)
	{
		HashCode const NameHash = Name.GetHashCode();
		size_t const FoundIndex = CPPAnnotationParserSpawner::AnnotationParsersRegistry.FindFirstElement(NameHash);
		if (FoundIndex != SIZE_MAX) {
			return CPPAnnotationParserSpawner::AnnotationParsersRegistry.GetElementAt(FoundIndex).Value(Args);
		} else {
			return SharedPtr<CPPAnnotationParser>(nullptr);
		}
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
		HashCode const NameHash = Name.GetHashCode();
		if (this->AnnotationParamParsersRegistry.FindFirstElement(NameHash) != SIZE_MAX) {
			GD_DEBUG_ASSERT_FALSE("Parser for paremeter of an annotation '%s' was already defined.", Name.CStr());
		}

		this->AnnotationParamParsersRegistry.PushLast(NameHash, Ctor);
	}

	/// Spawns new specialized annotation param parser.
	/// @param Name Name of annotaion idenitifier of required parser.
	/// @param Args Packed constructor params May be null pointer.
	/// @returns Pointer to parser if it was succesfullt created.
	UniquePtr<CPPAnnotationParamParser> CPPAnnotationParamParserSpawner::SpawnAnnotationParamParser(String const& Name, handle const Args) const
	{
		HashCode const NameHash = Name.GetHashCode();
		size_t const FoundIndex = this->AnnotationParamParsersRegistry.FindFirstElement(NameHash);
		if (FoundIndex != SIZE_MAX) {
			return this->AnnotationParamParsersRegistry.GetElementAt(FoundIndex).Value(Args);
		} else {
			return UniquePtr<CPPAnnotationParamParser>(nullptr);
		}
	}

#if 0
	bool CPPHeaderReflectorImpl::OpenOutputStreamsAndWriteInitializers()
	{
		this->HeaderInputStream = new FileInputStream(this->HeaderPath);
		if (!this->HeaderInputStream->CanRead())
		{
			CPPBaseParserErrorDesc static const HeaderNotFoundError("Unable to locate target header at (%s).");
			this->RaiseError(HeaderNotFoundError, this->HeaderPath.CStr());
			this->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
			return false;
		}

		this->HeaderLexer = new StreamedLexer(this->Toolchain, StreamedLexerDefaultOptions::GetDefaultOptionsForCpp());

		Str static const Copyrights =
			  "/// =========================================================================================="
			"\n/// %s - some generated reflection code."
			"\n/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved."
			"\n/// "
			"\n/// Note: this file was generated, please do not edit it manually."
			"\n/// ==========================================================================================\n\n";

		String const HeaderPathWithoutExtension = Path::GetDirectoryAndFileNameWithoutExtension(this->HeaderPath);
		String const HeadeRHeaderOutputPath = HeaderPathWithoutExtension + ".Generated.h";
		this->HeadeRHeaderOutput = new FileOutputStream(HeadeRHeaderOutputPath);
		if (!this->HeadeRHeaderOutput->CanWrite())
		{	// Failed to open output.
			CPPBaseParserErrorDesc static const CannonWriteHeadeRHeaderError("Failed to create header`s generated header output file (%s).");
			this->RaiseError(CannonWriteHeadeRHeaderError, HeadeRHeaderOutputPath.CStr());
			this->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
			return false;
		}
		else
		{
			String const HeadeRHeaderOutputRelativePath = Path::GetFileName(HeadeRHeaderOutputPath);
			String const HeadeRHeaderOutputCopyright = String::Format(Copyrights, HeadeRHeaderOutputRelativePath.CStr());
			this->HeadeRHeaderOutput->Write(HeadeRHeaderOutputCopyright.CStr(), 0, sizeof(Char) * HeadeRHeaderOutputCopyright.GetSize());

			Str static const HeaderInitializer =
				  "#pragma once"
				"\n#ifndef __%sGeneratedHeader__"
				"\n#define __%sGeneratedHeader__"
				"\n"
				"\nGD_NAMESPACE_BEGIN"
				"\n";

			String const HeaderPathWithoutDirAndExtension = Path::GetFileName(HeaderPathWithoutExtension);
			String const HeadeRHeaderOutputInitializer = String::Format(HeaderInitializer, HeaderPathWithoutDirAndExtension.CStr(), HeaderPathWithoutDirAndExtension.CStr());
			this->HeadeRHeaderOutput->Write(HeadeRHeaderOutputInitializer.CStr(), 0, sizeof(Char) * HeadeRHeaderOutputInitializer.GetSize());
		}

		String const HeaderSourceOutputPath = HeaderPathWithoutExtension + ".Generated.cpp";
		this->HeaderSourceOutput = new FileOutputStream(HeaderSourceOutputPath);
		if (!this->HeaderSourceOutput->CanWrite())
		{
			CPPBaseParserErrorDesc static const CannonWriteHeaderSourceError("Failed to create header`s generated source output file (%s).");
			this->RaiseError(CannonWriteHeaderSourceError, HeaderSourceOutputPath.CStr());
			this->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);
			return false;
		}
		else
		{
			String const HeaderSourceOutputRelativePath = Path::GetFileName(HeaderSourceOutputPath);
			String const HeaderSourceOutputCopyright = String::Format(Copyrights, HeaderSourceOutputRelativePath.CStr());
			this->HeaderSourceOutput->Write(HeaderSourceOutputCopyright.CStr(), 0, sizeof(Char) * HeaderSourceOutputCopyright.GetSize());

			Str static const SourceInitializer =
				  "#include \"%s\""
				"\n"
				"\nGD_NAMESPACE_BEGIN"
				"\n";

			String const HeaderPathWithoutDir = Path::GetFileName(this->HeaderPath);
			String const HeaderSourceOutputInitializer = String::Format(SourceInitializer, HeaderPathWithoutDir.CStr());
			this->HeadeRHeaderOutput->Write(HeaderSourceOutputInitializer.CStr(), 0, sizeof(Char) * HeaderSourceOutputInitializer.GetSize());
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

		this->HeadeRHeaderOutput->Write(HeaderFinalizers, 0, (sizeof(HeaderFinalizers) - 1) * sizeof(Char));
		this->HeaderSourceOutput->Write(SourceFinalizers, 0, (sizeof(SourceFinalizers) - 1) * sizeof(Char));
		return true;
	}
#endif

GD_NAMESPACE_END

/// ------------------------------------------------------------------------------------------
/// The reflector's entry point.
/// ------------------------------------------------------------------------------------------

#define GD_REFLECTOR_SUCCEDED 0
#define GD_REFLECTOR_FAILED 1

#include <ctime>
#include <GoddamnReflector/Enumeration/Enumeration.h>

GD_NORETURN static void PrintUsageAndExit()
{

}

int main(int const ArgumensCount, char const* const* const ParamsList)
{
#if (defined(GD_NAMESPACE))
	using namespace GD_NAMESPACE;
#endif	// if (defined(GD_NAMESPACE))

	// Parsing command-line arguments.
	bool WasSuccesfullyParsed = false;
	Str InputHeaderPath = nullptr;
	Str OutputSourcePath = nullptr;
	for (size_t CommandLineArgumentIndex = 1; (CommandLineArgumentIndex < ArgumensCount) && WasSuccesfullyParsed; ++CommandLineArgumentIndex) {
		// ..
		auto const CommandLineArgument = ParamsList[CommandLineArgumentIndex];
		if (::strncmp(CommandLineArgument, "/in:", sizeof("/in:") - 1) == 0) {
			InputHeaderPath  = CommandLineArgument + sizeof("/in:");
		} else if (::strncmp(CommandLineArgument, "/out:", sizeof("/out:") - 1) == 0) {
			OutputSourcePath = CommandLineArgument + sizeof("/out:");
		} else if (::strcmp(CommandLineArgument, "/s") == 0) {
			// Disable the console output.
		} else {
			GD_REFLECTOR_ERROR("\n\nUnknown parameter \"%s\" specified.", CommandLineArgument);
			GD_REFLECTOR_ERROR("\n\nUsage: \"%s\" /in:FileName [/out:FileName] [..OPTIONS..]\n"
				"    /in:FileName  Path to the C++ header that is required to be processed.    \n"
				"    /out:FileName Path to the C++ source file to which generated reflection   \n"
				"                  data would be written. If the file exists than it would be  \n"
				"                  extended.                                                   \n"
				"    /s            Specify it to disable console output.                       \n"
				"", ParamsList[0]);
			return GD_REFLECTOR_FAILED;
		}
	}
	if (InputHeaderPath == nullptr) {

	}

	GD_REFLECTOR_LOG("\nGoddamnEngine C++ Reflection utility.\nCopyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.");

	clock_t const StartTime = ::clock();
	try {
		IToolchain Toolchain;
		CPPBaseParser BaseParser(&Toolchain, new FileInputStream(HeaderPath));
		for (;;) {
			if (BaseParser.TrySkipToNextAnnotation() == GD_CPP_RESULT_EMPTY) {
				break;
			}

			BaseParser.ParseAnnotation();
		}
	} catch (Exception const& TheException) {
		GD_REFLECTOR_ERROR("\nGeneration failed with following excpetion:\n%s", TheException.GetErrorMessage());
		return GD_REFLECTOR_FAILED;
	}

	clock_t const TotalTime = ::clock() - StartTime;
	GD_REFLECTOR_LOG("\nGeneration took %f seconds.", static_cast<float>(TotalTime) / 1000.0f);
	return GD_REFLECTOR_SUCCEDED;
}