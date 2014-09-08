/// ==========================================================================================
/// Enumeration.h: GoddamnC++ enumeration implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/Enumeration/Enumeration.h>
#include <GoddamnReflector/CodeGenerator/CodeGenerator.h>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>

GD_NAMESPACE_BEGIN

	LockFreeList<SharedPtr<CPPEnumeration>> static CPPEnumerationsListImpl;
	LockFreeList<SharedPtr<CPPEnumeration>> const& CPPEnumerationsList = CPPEnumerationsListImpl;

	/// Parses "$GD_ENUMERATION(...)" annotation.
	class CPPEnumerationParser final : public CPPAnnotationParser
	{
	public /*Class members & constructor / destructor*/:
		SharedPtr<CPPEnumeration> CurrentEnumeration;

		GDINL explicit CPPEnumerationParser(handle const Args) : CPPAnnotationParser(Args) { }
		GDINL virtual ~CPPEnumerationParser() { }

	protected /*Class API*/:
		/// @see CPPAnnotationParser::SpawnParamParser
		GDINT virtual UniquePtr<CPPAnnotationParamParser> SpawnParamParser(String const& ParamName) override final;
		/// @see CPPAnnotationParser::ParseAnnotation
		GDINT virtual void ParseAnnotation(CPPBaseParser* const BaseParser) override final;
	};	// class CPPEnumerationParser  

	/// Parses "$GD_ENUMERATION(...)" annotation`s "Type" argument.
	class CPPEnumerationTypeParamParser final : public CPPAnnotationParamParser
	{
	public /*Class API*/:
		GDINL explicit CPPEnumerationTypeParamParser(handle const Args) : CPPAnnotationParamParser(Args) { }
		GDINL virtual ~CPPEnumerationTypeParamParser() { }
		/// @see CPPAnnotationParamParser::ParseArgument
		GDINT virtual void ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const AnnotationParser, String const& ParamValue) override final;
	};	// class CPPEnumerationParamTypeParser

	/// Parses "$GD_ENUMERATION(...)" annotation`s "Stringification" argument.
	class CPPEnumerationStrinigificationParamParser final : public CPPAnnotationParamParser
	{
	public /*Class API*/:
		GDINL explicit CPPEnumerationStrinigificationParamParser(handle const Args) : CPPAnnotationParamParser(Args) { }
		GDINL virtual ~CPPEnumerationStrinigificationParamParser() { }
		/// @see CPPAnnotationParamParser::ParseArgument
		GDINT virtual void ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const AnnotationParser, String const& ParamValue) override final;
	};	// class CPPEnumerationParamStrinigificationParser

	/// ==========================================================================================
	/// CPPEnumerationParser class.
	/// Parses "$GD_ENUMERATION(...)" annotation.
	/// ==========================================================================================

	/// Spanwer node for this type.
	CPPAnnotationParserSpawner::Node<CPPEnumerationParser> static const CPPEnumerationParserSpawnerNode("$GD_ENUMERATION");

	/// ------------------------------------------------------------------------------------------
	/// Protected class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	/// @see CPPAnnotationParser::SpawnParamParser
	UniquePtr<CPPAnnotationParamParser> CPPEnumerationParser::SpawnParamParser(String const& ParamName)
	{
		CPPAnnotationParamParserSpawner static Spawner; {
			CPPAnnotationParamParserSpawner::Node<CPPEnumerationTypeParamParser> static const TypeParamSpawner(Spawner, "Type");
			CPPAnnotationParamParserSpawner::Node<CPPEnumerationStrinigificationParamParser> static const StringificationParamSpawner(Spawner, "Stringification");
		}

		return Spawner.SpawnAnnotationParamParser(ParamName, nullptr);
	}

	/// @see CPPAnnotationParser::ParseAnnotation
	void CPPEnumerationParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{	/// Initialization.
		this->CurrentEnumeration = new CPPEnumeration();
		this->CPPAnnotationParser::ParseAnnotation(BaseParser);
		this->CurrentEnumeration->SetDefaultsForUnknowns();

		/// Expecting 'enum'/'enum class' declaration.
		using namespace StreamedLexerDefaultOptions;
		BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ENUM);
		if (BaseParser->TryExpectNextLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD, GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CLASS))
			BaseParser->ExpectNextLexem();

		// Parsing enumeration identifier name.
		BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
		this->CurrentEnumeration->EnumerationName = BaseParser->GetCurrentLexem().GetRawData();
		BaseParser->ExpectNextLexem();

		// Parsing enumeration base type (if it was specified).
		if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON)) 
		{	// Enumeration has it`s own base type.
			BaseParser->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
			this->CurrentEnumeration->EnumerationBaseTypeName = BaseParser->GetCurrentLexem().GetRawData();
			BaseParser->ExpectNextLexem();
		}

		// Parsing enumeration body.
		BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN);
		BaseParser->ExpectNextLexem();
		if (!BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END))
		{	// We are having non-empty enumeration here.
			for (;;)
			{	// Considering all preprocessor directives we met while parsing.
				while (this->CurrentEnumeration->EnumerationElements.ÑonsiderPreprocessorDirective(BaseParser));
				if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END)) break;

				// Parsing enumeration element name.
				BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
				CPPEnumerationElement* const EnumerationElement = new CPPEnumerationElement();
				EnumerationElement->Name = BaseParser->GetCurrentLexem().GetRawData();
				this->CurrentEnumeration->EnumerationElements.AppendElement(SharedPtr<CPPDefinition>(EnumerationElement));
				if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END)) break;

				BaseParser->ExpectNextLexem();
				if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN))
				{	// Here we have explicit enum value specified.
					BaseParser->ExpectNextLexem();
					BaseParser->ParseComplexExpression();
				}

				if (BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END)) break;
				BaseParser->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA);
				BaseParser->ExpectNextLexem();
			}
		}

		CPPEnumerationsListImpl.PushLast(this->CurrentEnumeration);
		this->CurrentEnumeration = nullptr;
	}

	/// ==========================================================================================
	/// CPPEnumerationParamTypeParser class.
	/// Parses "$GD_ENUMERATION(...)" annotation`s "Type" argument.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	void CPPEnumerationTypeParamParser::ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const SomeAnnotationParser, String const& ParamValue)
	{
		CPPEnumerationParser* const AnnotationParser = static_cast<CPPEnumerationParser*>(SomeAnnotationParser);
		if (AnnotationParser->CurrentEnumeration->EnumerationType == GD_CPP_ENUMERATION_TYPE_UNKNOWN)
		{
			/**/ if (ParamValue == "Enumeration") AnnotationParser->CurrentEnumeration->EnumerationType = GD_CPP_ENUMERATION_TYPE_ENUMERATION;
			else if (ParamValue == "Bitset"     ) AnnotationParser->CurrentEnumeration->EnumerationType = GD_CPP_ENUMERATION_TYPE_BITSET;
			else
			{
				CPPBaseParserErrorDesc static const MultipleEnumTypesSpecifiedError("invalid parameter Type value '%s' specified.");
				throw CPPParsingException(MultipleEnumTypesSpecifiedError.ToString(&BaseParser->GetCurrentLexem(), ParamValue.CStr()));
			}
		}
		else
		{	
			CPPBaseParserErrorDesc static const MultipleEnumTypesSpecifiedError("multiple enumeration Type parameters specified.");
			throw CPPParsingException(MultipleEnumTypesSpecifiedError.ToString(&BaseParser->GetCurrentLexem()));
		}
	}

	/// ==========================================================================================
	/// CPPEnumerationParamStrinigificationParser class.
	/// Parses "$GD_ENUMERATION(...)" annotation`s "Stringification" argument.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	void CPPEnumerationStrinigificationParamParser::ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const SomeAnnotationParser, String const& ParamValue)
	{
		CPPEnumerationParser* const AnnotationParser = static_cast<CPPEnumerationParser*>(SomeAnnotationParser);
		auto& Stringification = AnnotationParser->CurrentEnumeration->EnumerationStringification;

		if (Stringification.ChoppingPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_UNKNOWN)
			do
			{	// This params could be here.
				/**/ if (ParamValue == "Chopped") Stringification.ChoppingPolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING;
				else if (ParamValue == "Full"   ) Stringification.ChoppingPolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_FULL;
				else break;
				return;
			} while (false);
		else if ((ParamValue == "Chopped") || (ParamValue == "Full"))
		{	// Validating multilple redifinition error.
			CPPBaseParserErrorDesc static const MultipleEnumChoppingSpecifiedError("multiple enumeration Stringification chopping policy parameters specified.");
			throw CPPParsingException(MultipleEnumChoppingSpecifiedError.ToString(&BaseParser->GetCurrentLexem()));
		}

		if (Stringification.CasePolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UNKNOWN)
			do
			{	// This params could be here.
				/**/ if (ParamValue == "GoddamnCase") Stringification.CasePolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN;
				else if (ParamValue == "AsIs"       ) Stringification.CasePolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_AS_IS;
				else if (ParamValue == "UpperCase"  ) Stringification.CasePolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UPPER_CASE;
				else if (ParamValue == "LowerCase"  ) Stringification.CasePolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_LOWER_CASE;
				else break;
				return;
			} while (false);
		else if ((ParamValue == "GoddamnCase") || (ParamValue == "AsIs") || (ParamValue == "UpperCase") || (ParamValue == "LowerCase"))
		{	// Validating multilple redifinition error.
			CPPBaseParserErrorDesc static const MultipleEnumCaseSpecifiedError("multiple enumeration Stringification case policy parameters specified.");
			throw CPPParsingException(MultipleEnumCaseSpecifiedError.ToString(&BaseParser->GetCurrentLexem()));
		}

		if (Stringification.ExportPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_UNKNOWN)
			do
			{	// This params could be here.
				/**/ if (ParamValue == "Internal") Stringification.ExportPolicy = GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_INTERNAL;
				else if (ParamValue == "Public"  ) Stringification.ExportPolicy = GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC;
				else break;
				return;
			} while (false);
		else if ((ParamValue == "Internal") || (ParamValue == "Public"))
		{	// Validating multilple redifinition error.
			CPPBaseParserErrorDesc static const MultipleEnumExportSpecifiedError("multiple enumeration Stringification export policy parameters specified.");
			throw CPPParsingException(MultipleEnumExportSpecifiedError.ToString(&BaseParser->GetCurrentLexem()));
		}

		CPPBaseParserErrorDesc static const InvalidStringificationParamSpecified("invalid Stringification parameter '%s' specified.");
		throw CPPParsingException(InvalidStringificationParamSpecified.ToString(&BaseParser->GetCurrentLexem(), ParamValue.CStr()));
	}

#if 0
	/// Fills the array with default enumeration values names.
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationFillNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		OutputNames.Emptify();
		for (auto const& EnumerationValue : Enumeration->EnumerationValues)
			OutputNames.PushLast(EnumerationValue.Key);
	}

	/// Chops enumeration values common parts E.g. (MY_ENUM_E1RT_A, MY_ENUM_E2RT_A) would be stringified as ("E1RT_A", "E2RT_A").
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationChopNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		if (Enumeration->EnumerationValues.GetSize() != OutputNames.GetSize())
			CPPEnumerationFillNames(Enumeration, OutputNames);
		if (Enumeration->EnumerationValues.GetSize() == 0)
			return;

		if (Enumeration->EnumerationStringification.ChoppingPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING)
		{	// We have to chop enum values names here.
			size_t EnumerationValueMaxIndex = SIZE_MAX;
			for (;;)
			{
				String const& InitialStubableString = OutputNames[0];
				if (InitialStubableString.GetSize() == EnumerationValueMaxIndex)
					break;

				Char const CurrentMatchingCharacter = InitialStubableString[EnumerationValueMaxIndex];
				for (auto EnumerationValue = OutputNames.Begin() + 1; EnumerationValue != OutputNames.End(); ++EnumerationValue)
				{
					if ((*EnumerationValue).GetSize() != EnumerationValueMaxIndex)
						if ((*EnumerationValue)[EnumerationValueMaxIndex] == CurrentMatchingCharacter)
						{
							++EnumerationValueMaxIndex;
							continue;
						}

					goto LeaveOuterLoop;
				}
			}
		LeaveOuterLoop:;

			++EnumerationValueMaxIndex;
			for (auto& OutputName : OutputNames)
				OutputName = OutputName.GetSubstring(EnumerationValueMaxIndex);
		}
	}

	/// Translates enumeration values capicalization styles.
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationTranslateCasePolicy(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		if (Enumeration->EnumerationValues.GetSize() != OutputNames.GetSize())
			CPPEnumerationFillNames(Enumeration, OutputNames);
		if (Enumeration->EnumerationValues.GetSize() == 0)
			return;

		if (Enumeration->EnumerationStringification.CasePolicy != GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_AS_IS)
		{	// We have to change policy.
			for (auto& EnumerationValue : OutputNames)
			{
				Vector<Char const*> WordsStubs;
				for (Char const* WordPosition = EnumerationValue.CStr() + 1; (*WordPosition) != Char('\0'); ++WordPosition)
					if (((*WordPosition) == Char('_')) || (islower(*WordPosition) && (isupper(*(WordPosition - 1))))) // We have '_' separator. OR Lowercase character and previous uppercase.
						WordsStubs.PushLast(WordPosition);

				WordsStubs.PushLast(&*EnumerationValue.End());

				StringBuilder EnumerationValueBuilder;
				for (auto const WordStub : WordsStubs)
				{
					if (Enumeration->EnumerationStringification.CasePolicy != GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN)
					{
						if (EnumerationValueBuilder.GetSize() != 0)
							EnumerationValueBuilder.Append('_');

						if (Enumeration->EnumerationStringification.CasePolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UPPER_CASE)
							EnumerationValueBuilder.Append(String(EnumerationValue.CStr() + (WordStub - EnumerationValue.CStr())).ToUpper());
						else if (Enumeration->EnumerationStringification.CasePolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_LOWER_CASE)
							EnumerationValueBuilder.Append(String(EnumerationValue.CStr() + (WordStub - EnumerationValue.CStr())).ToLower());
					}
					else
					{
						String EnumerationValueBuilderStub(EnumerationValue.CStr() + (WordStub - EnumerationValue.CStr()));
						EnumerationValueBuilderStub = Move(EnumerationValueBuilderStub.ToLower());
						EnumerationValueBuilderStub[0] = static_cast<Char>(toupper(EnumerationValueBuilderStub[0]));
						EnumerationValueBuilder.Append(EnumerationValueBuilderStub);
					}
				}

				EnumerationValue = EnumerationValueBuilder.ToString();
			}
		}
	}

	/// Generates stringiciators methods for specified enumeration.
	/// @param Enumeration                 Pointer on enumeration description.
	/// @param OutputGeneratedHeaderStream Stream to which stringiciators interfaces would be written.
	/// @param OutputGeneratedSourceStream Stream to which stringiciators implementations would be written.
	extern void CPPEnumerationWriteStringificators(SharedPtr<CPPEnumeration const> const& Enumeration, OutputStream* const OutputGeneratedHeaderStream, OutputStream* const OutputGeneratedSourceStream)
	{
		GD_DEBUG_ASSERT((OutputGeneratedHeaderStream != nullptr) || (Enumeration->EnumerationStringification.ExportPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_INTERNAL), "No header output specified.");
		GD_DEBUG_ASSERT((OutputGeneratedHeaderStream != nullptr), "No source output specified.");

		String const EnumerationToStrInterface   = String::Format("extern Str %sToStr(%s const Value)",   Enumeration->EnumerationName.CStr(), Enumeration->EnumerationName.CStr());
		String const EnumerationFromStrInterface = String::Format("extern %s %sFromStr(Str const Value)", Enumeration->EnumerationName.CStr(), Enumeration->EnumerationName.CStr());

		if (Enumeration->EnumerationStringification.ExportPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC)
		{	// We need to write prototypes into header file.
			OutputGeneratedHeaderStream->WriteString(String::Format("\n\n\tenum %s : %s;", Enumeration->EnumerationName.CStr(), Enumeration->EnumerationBaseTypeName.CStr()));
			OutputGeneratedHeaderStream->WriteString(String::Format("\n\n\t/// Converts %s to string.\n\tGDAPI %s;", Enumeration->EnumerationName.CStr(), EnumerationToStrInterface.CStr()));
			OutputGeneratedHeaderStream->WriteString(String::Format("\n\n\t/// Converts string to %s.\n\tGDAPI %s;", Enumeration->EnumerationName.CStr(), EnumerationFromStrInterface.CStr()));
		}

		/// Writing stringificators bodies.
		Vector<String> EnumerationNames;
		CPPEnumerationChopNames(Enumeration, EnumerationNames);
		CPPEnumerationTranslateCasePolicy(Enumeration, EnumerationNames);

		// Writing ToString function.
		OutputGeneratedSourceStream->WriteString(String("\n\n\t") + EnumerationToStrInterface);
		OutputGeneratedSourceStream->WriteString("\n\t{\n\t\tswitch(Value)\n\t\t{");
		for (size_t Cnt = 0; Cnt < EnumerationNames.GetSize(); ++Cnt)
		{	// Writing stringified values.
			OutputGeneratedSourceStream->WriteString(String::Format("\n\t\t\tcase %s: return \"%s\";"
				, Enumeration->EnumerationValues.GetElementAt(Cnt).Key.CStr()
				, EnumerationNames.GetElementAt(Cnt).CStr()));
		}
		OutputGeneratedSourceStream->WriteString("\n\t\t}\n\t\treturn nullptr;\n\t}");

		// Writing FromString function.
		OutputGeneratedSourceStream->WriteString(String("\n\n\t") + EnumerationFromStrInterface);
		OutputGeneratedSourceStream->WriteString("\n\t{\n\t\tUInt32 const HashCode = String(Value).GetHashCode();\n\t\tswitch(HashCode) {");
		for (size_t Cnt = 0; Cnt < EnumerationNames.GetSize(); ++Cnt)
		{	// Writing prehashed values.
			OutputGeneratedSourceStream->WriteString(String::Format("\n\t\t\tcase %d: return %s;"
				, static_cast<Int32>(EnumerationNames.GetElementAt(Cnt).GetHashCode().GetValue())
				, Enumeration->EnumerationValues.GetElementAt(Cnt).Key.CStr()));
		}
		OutputGeneratedSourceStream->WriteString("\n\t\t}\n\t\treturn ???Something???;\n\t}");
	}
#endif	// if 0

GD_NAMESPACE_END