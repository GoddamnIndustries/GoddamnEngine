/// ==========================================================================================
/// Reflector.h: GoddamnC++ reflector interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_REFLECTOR_REFLECTOR
#define GD_REFLECTOR_REFLECTOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.h>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.h>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.h>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>

#define GD_STRINGIFY(What) #What

GD_NAMESPACE_BEGIN

	class InputStream;
	class CPPCodeGenerator;

	enum CPPResult : UInt8
	{
		GD_CPP_RESULT_FAILED = 0,
		GD_CPP_RESULT_SUCCEEDED = 1,
		GD_CPP_RESULT_EMPTY,
	};	// enum CPPResult

	/// Represents lexer error description.
	struct CPPBaseParserErrorDesc final : public ToolchainMessageDesc
	{
	public:
		GDINL explicit CPPBaseParserErrorDesc(String const& Message) : ToolchainMessageDesc(Message) { }
	};	// struct StreamedLexerErrorDesc

	/// Fatal parsing error exception
	class CPPParsingException : public ToolchainException
	{
	public /*Public API*/:
		GDINL explicit CPPParsingException(String const& Message) : ToolchainException(Message.CStr()) { }
		GDINL virtual ~CPPParsingException() { }
	};	// class ToolchainException

	struct CPPDefinition
	{
	};	// struct CPPDefinition

	/// Contains useful methods for basic GoddamnC++ headers parsing.
	class CPPBaseParser : public IToolchainTool
	{
	private:
		UniquePtr<StreamedLexer> const Lexer;
		Lexem CurrentLexem;

	private /*Class API helpers*/:
		/// Attempts to read next lexem from input stream.
		/// @returns True if lexem was succesfully read.
		GDINT bool TryReadNextLexem();

	public /*Class API*/:
		/// Initializes new basic GoddamnC++ parser.
		/// @param Toolchain Corrseponding toolchain.
		/// @param Input     Shared pointer on input stream that contains GoddamnC++ data.
		GDINT CPPBaseParser(IToolchain* const Toolchain, UniquePtr<InputStream>&& Input);
		GDINL virtual ~CPPBaseParser() { }

		/// @name Getters / Setters
		/// @{

		/// Returns reference on last parsed lexem.
		/// @returns Reference on last parsed lexem.
		GDINL Lexem const& GetCurrentLexem() const { return self->CurrentLexem; }

	//	/// Returns reference on input stream this parser reads from.
	//	/// @returns Reference on input stream this parser reads from.
	//	GDINL InputStream* GetInputStream() { return self->Input.GetPointer(); }

		/// Returns reference on lexer this parser ususes.
		/// @returns Reference on lexer this parser ususes.
		GDINL StreamedLexer* GetLexer() { return self->Lexer.GetPointer(); }

		/// @}

		/// @name Basic lexem pipeline and expectings.
		/// @{

		/// Expects a match of current lexem content type with specified one.
		/// @param ContentType The expected lexem content type.
		/// @returns True if current lexem content type mathes with specified one.
		GDINT bool TryExpectLexem(LexemContentType const ContentType);

		/// Expects a match of current lexem content type with specified one.
		/// If lexem does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
		/// @param ContentType The expected lexem content type.
		GDINT void ExpectLexem(LexemContentType const ContentType);

		/// Expects a match of current lexem content type and parsed data ID (PDID) with specified ones.
		/// @param ContentType The expected lexem content type.
		/// @param ID          The expected lexem parsed data ID (PDID).
		/// @returns True if current lexem content type and parsed data ID mathes with specified one.
		GDINT bool TryExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID);

		/// Expects a match of current lexem content type and parsed data ID (PDID) with specified ones.
		/// If lexem does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
		/// If lexem matches with content type, but does not matches with parsed data ID (PDID), then raises 'unexpected Existing-PDID. Expected-PDID expected' error.
		/// @param ContentType The expected lexem content type.
		/// @param ID          The expected lexem parsed data ID (PDID).
		GDINT void ExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID);

		/// Expects next lexem from input stream.
		/// @returns True if lexem was succesfully read.
		GDINT bool TryExpectNextLexem();

		/// Expects next lexem from input stream. 
		/// If lexem does not exists then raises 'unexpected End-Of-Stream' error.
		GDINT void ExpectNextLexem();

		/// Expects next lexem from input stream and a match of lexem content type with specified one.
		/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
		/// @param ContentType The expected lexem content type.
		/// @returns True if lexem was succesfully read and mathes with specified content type.
		GDINT bool TryExpectNextLexem(LexemContentType const ContentType);

		/// Expects next lexem from input stream and a match of lexem content type with specified one.
		/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
		/// If lexem exists, but does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
		/// @param ContentType The expected lexem content type.
		GDINT void ExpectNextLexem(LexemContentType const ContentType);

		/// Expects next lexem from input stream, and a match of lexem content type with specified one, and a match of lexem parsed data ID with specified one.
		/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
		/// @param ContentType The expected lexem content type.
		/// @param ID          The expected lexem parsed data ID (PDID).
		/// @returns True if lexem was succesfully read and mathes with specified content type and specified parsed data ID.
		GDINT bool TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID);

		/// Expects next lexem from input stream, and a match of lexem content type with specified one, and a match of lexem parsed data ID (PDID) with specified one.
		/// If lexem does not exists, then raises 'unexpected End-Of-Stream' error.
		/// If lexem exists, but does not matches with content type, then raises 'unexpected Existing-Content-Type. Expected-Content-Type expected' error.
		/// If lexem exists, matches with content type, but does not matches with parsed data ID, then raises 'unexpected Existing-PDID. Expected-PDID expected' error.
		/// @param ContentType The expected lexem content type.
		/// @param ID          The expected lexem parsed data ID (PDID).
		GDINT void ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID);

		/// @}

		/// @name C++-specific template/macro/identifiers expresssions parsing.
		/// @{

		/// Parses complex expression. Complex expression can be referenced with something like normal C++ expression.
		/// @param Output Output string to which complex expression would be written. May be nullptr.
		/// @returns True if complex type name was successfully parsed.
		GDINT void ParseComplexExpression(UniquePtr<CPPCodeGenerator> const& CodeGenerator = nullptr);

		/// Parses complex type name. Complex type names are ones with namespace definitions and template parameters. Limitations: 
		/// @li Template parameters are not really parsed, analyzer just finds where they begin/end. So something like right shift operator could break everything.
		/// @param Output Output string to which complex type name would be written. May be nullptr.
		/// @returns True if complex type name was successfully parsed.
		GDINT bool ParseComplexTypename(UniquePtr<CPPCodeGenerator> const& CodeGenerator = nullptr);

		/// Parses complex type name. Complex type name can be referenced with as type names with const/volatile cvs, references, pointers, template arguments. Limitations: 
		/// @li Type modifiers should follow type names. As sequence, type names could not be defined with struct/class keyword.
		/// @li Type modifiers cannot be wrapped with macroes. That means that analyzer could not determine that MY_CONST, defined as '#define MY_CONST const' is real 'const' and deside end of type name.
		/// @li Template parameters are not really parsed, analyzer just finds where they begin/end. So something like right shift operator could break everything.
		/// @param Output Output string to which complex type name would be written. May be nullptr.
		/// @returns True if complex type name was successfully parsed.
		GDINT bool ParseComplexTypenameWithCVs(UniquePtr<CPPCodeGenerator> const& CodeGenerator = nullptr);

		/// @}

		/// @name Annotations parsing.
		/// @{

		/// Reads data from input stream until comes up with annotation - idenitifier starting with specified prefix.
		/// @param ExpectedAnnotationPrefix Prefix of expected annotation idenitifier.
		/// @return Success if skipped to next annotation, otherwise Empty if next annotation was not found.
		GDINT CPPResult TrySkipToNextAnnotation(String const& ExpectedAnnotationPrefix = "$GD_");

		/// Processes next found annotation in input stream.
		/// @param ExpectedAnnotationPrefix Prefix of expected annotation idenitifier.
		/// @return Success if annotation was succesfully processed, Failed if falied, Empty if no processor/annotation was found.
		GDINT CPPResult ParseAnnotation(handle const Args = nullptr, String const& ExpectedAnnotationPrefix = "$GD_");

		/// @}
	};	// class CPPBaseParser

	class CPPAnnotationParamParser;

	/// Abstract interface for GoddamnC++ annotation parser.
	class CPPAnnotationParser 
	{
	private:
		GD_CLASS_UNASSIGNABLE(CPPAnnotationParser);
		GD_CLASS_UNCOPIABLE  (CPPAnnotationParser);
	public /*Constructor / Destrutor*/:
		/// Initializes specialized annotation parser.
		/// @param Args Packed constructor params May be null pointer.
		GDINL explicit CPPAnnotationParser(handle const Args) { }
		GDINL virtual ~CPPAnnotationParser() { }

	private /*Class API*/:
		/// Parses annotation params.
		/// @param BaseParser Parser that provides low lever source parsing.
		/// @returns True if annotation argumnts were succesfully parsed.
		GDINT void ParseAnnotationParams(CPPBaseParser* const BaseParser);

	public /*Class API*/:
		/// Spawns new parser of annotaion param value.
		/// @param ParamName Name of param that would be parsed.
		/// @returns Pointer to parser on success, or nullptr on fail.
		GDINT virtual UniquePtr<CPPAnnotationParamParser> SpawnParamParser(String const& ParamName) abstract;

		/// Parses upcoming annotation.
		/// @param BaseParser Parser that provides low lever source parsing.
		/// @returns True if annotation was succesfully parsed.
		GDINT virtual void ParseAnnotation(CPPBaseParser* const BaseParser) impl_abstract;
	};	// class CPPAnnotationParser

	/// Provides registering all annotation-parser-derived classes and spawning them while parsing.
	/// To create some specific annotation parser follow this example:
	/// @code
	///		// Header file:
	///		class CPPMyAnnotationParser final : public CPPAnnotationParser {
	///			...	// Implement all abstract methods here.
	///     };	// class CPPMyAnnotationParser
	///
	///		// Source file:
	///		CPPAnnotationParserSpawner::Node<CPPMyAnnotationParser> static const CPPMyAnnotationParserSpawnerNode("$GD_MY_ANNOTATION");
	/// @endcode
	class CPPAnnotationParserSpawner final
	{
	private /*Class Types*/:
		/// Pointer to function that creates new specialized annotation parser.
		/// @param Args Packed constructor params. May be null pointer.
		/// @returns Shared pointer on newly created specialized annotation parser.
		typedef SharedPtr<CPPAnnotationParser>(*CtorProc)(handle const Args);

	public /*Class Types*/:
		GDINL  CPPAnnotationParserSpawner() = delete;
		GDINL ~CPPAnnotationParserSpawner() = delete;

		/// Provides registering all annotation-parser-derived classes.
		/// @param CPPAnnotationParserType CPPAnnotationParser-derived non-abstract type that would be registered.
		template<typename CPPAnnotationParserType>
		struct Node final
		{
			static_assert( TypeTraits::IsBaseType<CPPAnnotationParser, CPPAnnotationParserType>::Value, "'CPPAnnotationParserSpawner::Node<T>' error: T should inherit from CPPAnnotationParser.");
			static_assert(!TypeTraits::IsAbstractType<CPPAnnotationParserType>::Value, "'CPPAnnotationParserSpawner::Node<T>' error: T should not be abstract.");

			/// Registers specialized annotation parser in global registry.
			/// @param Name Name of annotaion idenitifier this parser processes.
			GDINL explicit Node(String const& Name)
			{
				CPPAnnotationParserSpawner::RegisterAnnotationParser(Name, [](handle const Args) {
					return SharedPtr<CPPAnnotationParser>(new CPPAnnotationParserType(Args));
				});
			}
		};	// struct CPPAnnotationParserSpawner::Node

	private /*Class Members*/:
		/// Contains specialized annotation parser registry.
		Map<HashSumm, CtorProc> static AnnotationParsersRegistry;

	public /*Class API*/:
		/// Registers new specialized annotation parser in registry.
		/// @param Name Name of annotaion idenitifier this parser processes.
		/// @param Ctor Procedure that constructs instance of this annotation parser.
		/// @note It is not recommended to use this method directly: use RAI CPPAnnotationParserSpawner::Node<T> class instead.
		GDINT static void RegisterAnnotationParser(String const& Name, CtorProc const Ctor);

		/// Spawns new specialized annotation parser.
		/// @param Name Name of annotaion idenitifier of required parser.
		/// @param Args Packed constructor params May be null pointer.
		/// @returns Pointer to parser if it was succesfullt created.
		GDINT static SharedPtr<CPPAnnotationParser> SpawnAnnotationParser(String const& Name, handle const Args);
	};	// class CPPAnnotationParserSpawner

	/// Abstract interface for GoddamnC++ annotation param parser.
	class CPPAnnotationParamParser
	{
	public /*Constructor / Destrutor*/:
		/// Initializes specialized annotation param parser.
		GDINL explicit CPPAnnotationParamParser(handle const Args) { }
		GDINL virtual ~CPPAnnotationParamParser() { }

	public /*Class API*/:
		/// Parses upcoming annotation param.
		/// @param BaseParser       Parser that provides low lever source parsing.
		/// @param AnnotationParser Parser, that currently works on upcoming annotation.
		/// @param ParamValue       String value of annotation paramater or it`s part.
		/// @returns True if annotation param was succesfully parsed.
		GDINT virtual void ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const AnnotationParser, String const& ParamValue) abstract;
	};	// class CPPAnnotationParamParser

	/// Provides registering all annotation-params-parser-derived classes and spawning them while parsing.
	/// To create some specific annotation params parser follow this example:
	/// @code
	///		class CPPMyAnnotationAaParamParser final : public CPPAnnotationParamParser {
	///			...	// Implement all abstract methods here.
	///     };	// class CPPMyAnnotationParamParser
	///		...
	///		UniquePtr<CPPAnnotationParamParser> CPPMyAnnotationParser::SpawnParamParser(String const& ParamName) {
	///				CPPAnnotationParamParserSpawner static const Spawner; {
	///				CPPAnnotationParamParserSpawner::Node<CPPMyAnnotationAaParamParser> static const ParamAaSpawnerNode(Spawner, "AaaaAa");
	///				CPPAnnotationParamParserSpawner::Node<CPPMyAnnotationBbParamParser> static const ParamBbSpawnerNode(Spawner, "BbBBbb");
	///			}
	///			return Spawner.SpawnParamParser(ParamName, SomeCtorParams);
	///		}
	/// @endcode
	/// You can create your own implementation of spawning if you like it.
	class CPPAnnotationParamParserSpawner final
	{
	private /*Class types and members.*/:
		/// Pointer to function that creates new specialized annotation param parser.
		/// @param Args Packed constructor params. May be null pointer.
		/// @returns Unique pointer on newly created specialized annotation param parser.
		typedef UniquePtr<CPPAnnotationParamParser>(*CtorProc)(handle const Args);

		/// Contains specialized annotation params parser registry.
		Map<HashSumm, CtorProc> AnnotationParamParsersRegistry;

	public /*Class Types and API*/:
		/// Provides registering all annotation-parser-derived classes.
		/// @param CPPAnnotationParserType CPPAnnotationParser-derived non-abstract type that would be registered.
		template<typename CPPAnnotationParamParserType>
		struct Node final
		{
			static_assert(TypeTraits::IsBaseType<CPPAnnotationParamParser, CPPAnnotationParamParserType>::Value, "'CPPAnnotationParamParserSpawner::Node<T>' error: T should inherit from CPPAnnotationParamParser.");
			static_assert(!TypeTraits::IsAbstractType<CPPAnnotationParamParserType>::Value, "'CPPAnnotationParamParserSpawner::Node<T>' error: T should not be abstract.");

			/// Registers specialized annotation parser in global registry.
			/// @param Name Name of annotaion idenitifier this parser processes.
			GDINL explicit Node(CPPAnnotationParamParserSpawner& Spawner, String const& Name)
			{
				Spawner.RegisterAnnotationParamParser(Name, [](handle const Args) {
					return UniquePtr<CPPAnnotationParamParser>(new CPPAnnotationParamParserType(Args));
				});
			}
		};	// struct CPPAnnotationParserSpawner::Node

		/// Initializes new annotation param parser spawner.
		GDINL  CPPAnnotationParamParserSpawner() { }
		GDINL ~CPPAnnotationParamParserSpawner() { }

		/// Registers new specialized annotation param parser in registry.
		/// @param Name Name of annotaion param this parser processes.
		/// @param Ctor Procedure that constructs instance of this annotation param parser.
		/// @note It is not recommended to use this method directly: use RAI CPPAnnotationParserParamSpawner::Node<T> class instead.
		GDINT void RegisterAnnotationParamParser(String const& Name, CtorProc const Ctor);

		/// Spawns new specialized annotation param parser.
		/// @param Name Name of annotaion idenitifier of required parser.
		/// @param Args Packed constructor params May be null pointer.
		/// @returns Pointer to parser if it was succesfullt created.
		GDINT UniquePtr<CPPAnnotationParamParser> SpawnAnnotationParamParser(String const& Name, handle const Args) const;
	};	// class CPPAnnotationParamParserSpawner

GD_NAMESPACE_END

#endif