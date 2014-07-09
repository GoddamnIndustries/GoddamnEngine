/// ==========================================================================================
/// Reflectable.cc: GoddamnC++ reflectable type implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 02.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/Reflectable/Reflectable.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

GD_NAMESPACE_BEGIN

	LockFreeList<SharedPtr<CPPReflectable>> static CPPReflectablesListImpl;
	LockFreeList<SharedPtr<CPPReflectable>> const& CPPReflectablesList = CPPReflectablesListImpl;

	/// Parses "$GD_REFLECTABLE(...)" annotation.
	class CPPReflectableParser final : public CPPAnnotationParser
	{
	public /*Class members & constructor / destructor*/:
		SharedPtr<CPPReflectable> CurrentReflectable;

		GDINL explicit CPPReflectableParser(CPPAnnotationCtorArgs const* const Args) : CPPAnnotationParser(Args) { }
		GDINL virtual ~CPPReflectableParser() { }

		/// @see CPPAnnotationParser::SpawnParamParser
		GDINT virtual UniquePtr<CPPAnnotationParamParser> SpawnParamParser(String const& ParamName) override final;
		/// @see CPPAnnotationParser::ParseAnnotation
		GDINT virtual bool ParseAnnotation(CPPBaseParser* const BaseParser) override final;
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
	bool CPPReflectableParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{
		self->CurrentReflectable = new CPPReflectable();
		if (!self->CPPAnnotationParser::ParseAnnotation(BaseParser))
			return false;
		self->CurrentReflectable->SetDefaultsForUnknowns();

		/// Expecting 'class'/'struct' keyword.
		using namespace StreamedLexerDefaultOptions;
		if ( !BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STRUCT))
			if (!BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CLASS)) return false;

		// Parsing reflectable identifier name.
		if (!BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) return false;
		self->CurrentReflectable->ReflectableName = BaseParser->GetCurrentLexem().GetRawData();

		if (!BaseParser->ExpectNextLexem()) return false;
		if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FINAL))
		{	// Reflectable was specified with 'final' keyword.
			self->CurrentReflectable->ReflectableIsFinal = true;
			if (!BaseParser->ExpectNextLexem()) return false;
		}
		if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON))
		{	// Reflectable was specified with base type. Now expecting it being inheritec publically. 
			if (!BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PUBLIC)) return false;
			if (!BaseParser->ExpectNextLexem()) return false;

			BaseParser->ParseComplexTypename(&self->CurrentReflectable->ReflectableBaseTypeName);
		}

		// Expecting reflectable body here.
		if (!BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN)) return false;
		if (!BaseParser->ExpectNextLexem()) return false;
		if (!BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END))
		{	// We are having non-empty reflectable body here.
			for (;;)
			{	// Considering all preprocessor directives we met while parsing.
				while (self->CurrentReflectable->ReflectableElements.ÑonsiderPreprocessorDirective(BaseParser));
				if ((BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END))) break;
			}
		}

		CPPReflectablesListImpl.PushLast(self->CurrentReflectable);
		self->CurrentReflectable = nullptr;
		return true;
	}

GD_NAMESPACE_END
