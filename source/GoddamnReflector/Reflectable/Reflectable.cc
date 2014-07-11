/// ==========================================================================================
/// Reflectable.cc: GoddamnC++ reflectable type implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 02.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/Reflectable/Reflectable.hh>
#include <GoddamnReflector/CodeGenerator/CodeGenerator.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

GD_NAMESPACE_BEGIN

	LockFreeList<SharedPtr<CPPReflectable>> static CPPReflectablesListImpl;
	LockFreeList<SharedPtr<CPPReflectable>> const& CPPReflectablesList = CPPReflectablesListImpl;

	/// Parses "$GD_REFLECTABLE(...)" annotation.
	class CPPReflectableParser final : public CPPAnnotationParser
	{
	public /*Class members & constructor / destructor*/:
		SharedPtr<CPPReflectable> CurrentReflectable;

		GDINL explicit CPPReflectableParser(chandle const Args) : CPPAnnotationParser(Args) { }
		GDINL virtual ~CPPReflectableParser() { }

		/// @see CPPAnnotationParser::SpawnParamParser
		GDINT virtual UniquePtr<CPPAnnotationParamParser> SpawnParamParser(String const& ParamName) override final;
		/// @see CPPAnnotationParser::ParseAnnotation
		GDINT virtual void ParseAnnotation(CPPBaseParser* const BaseParser) override final;
	};	// class CPPEnumerationParser  

	/// ==========================================================================================
	/// CPPReflectableParser class.
	/// Parses "$GD_REFLECTABLE(...)" annotation.
	/// ==========================================================================================

	/// Spanwer node for this type.
	CPPAnnotationParserSpawner::Node<CPPReflectableParser> static const CPPReflectableParserSpawnerNode("$GD_REFLECTABLE");

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Implemetations):
	/// ------------------------------------------------------------------------------------------

	/// @see CPPAnnotationParser::SpawnParamParser
	UniquePtr<CPPAnnotationParamParser> CPPReflectableParser::SpawnParamParser(String const& ParamName)
	{
		return UniquePtr<CPPAnnotationParamParser>(nullptr);
	}

	/// @see CPPAnnotationParser::ParseAnnotation
	void CPPReflectableParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{
		self->CurrentReflectable = new CPPReflectable();
		self->CPPAnnotationParser::ParseAnnotation(BaseParser);
		self->CurrentReflectable->SetDefaultsForUnknowns();

		// Expecting 'class'/'struct' keyword.
		using namespace StreamedLexerDefaultOptions;
		if (!BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STRUCT))
			BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CLASS);

		// Parsing reflectable identifier name.
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
		self->CurrentReflectable->ReflectableName = BaseParser->GetCurrentLexem().GetRawData();

		// Reflectable can be specified with 'final' keyword.
		if (BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FINAL))
		{	// Reflectable was specified with 'final' keyword.
			self->CurrentReflectable->ReflectableIsFinal = true;
			BaseParser->ExpectNextLexem();
		}

		// Reflectable should be specified with base type. Now expecting it being inherited publically. 
		BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON);
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PUBLIC);
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
		self->CurrentReflectable->ReflectableBaseTypeName = BaseParser->GetCurrentLexem().GetRawData();

		// Expecting reflectable body here.
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN);

		// Expecting '$GD_REFLECTABLE_BODY_GENERATED_CRAP()' macro.
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
		if (BaseParser->GetCurrentLexem().GetRawData() != GD_STRINGIFY($GD_REFLECTABLE_BODY_GENERATED_CRAP))
		{	// Serializable's body should start with '$GD_REFLECTABLE_BODY_GENERATED_CRAP(TypeName)' macro. 
			CPPBaseParserErrorDesc static const NoGeneratedBodyCrapMacroLocatedError("reflectable body should start with '"GD_STRINGIFY($GD_REFLECTABLE_BODY_GENERATED_CRAP)"(%s)'");
			throw CPPParsingException(NoGeneratedBodyCrapMacroLocatedError.ToString(&BaseParser->GetCurrentLexem(), self->CurrentReflectable->ReflectableName.CStr()));
		}
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN);
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END);
		BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SEMICOLON);

		// Expecting body filling here.
		for (;;)
		{	// Considering all preprocessor directives we met while parsing.
			while (self->CurrentReflectable->ReflectableElements.ÑonsiderPreprocessorDirective(BaseParser));
			/**/ if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN)) BaseParser->ParseComplexExpression();
			/**/ if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END)) break;
			else if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER))
				if (strncmp(BaseParser->GetCurrentLexem().GetRawData().CStr(), "$GD_", (sizeof("$GD_") - 1)) == 0)
				{	// This lexem matches is identifier and does matches with prefix. 
					BaseParser->ParseAnnotation(self);
					continue;
				}

			BaseParser->ExpectNextLexem();
		}

		CPPReflectablesListImpl.PushLast(self->CurrentReflectable);
		self->CurrentReflectable = nullptr;
	}

GD_NAMESPACE_END
