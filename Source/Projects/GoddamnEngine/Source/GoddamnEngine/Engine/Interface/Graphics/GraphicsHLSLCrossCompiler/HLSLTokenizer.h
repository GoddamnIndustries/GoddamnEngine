#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Diagnostics/Exception/Exception.h>
#include <GoddamnEngine/Core/UniquePtr.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>

GD_NAMESPACE_BEGIN

	//! Describes all supported lexem types.
	enum class LexemContentType : UInt8
	{
		Identifier,			///< Lexem contains identifier data (string data type).
		Keyword,			///< Lexem contains keyword data (string data type, has ID).
		Operator,			///< Lexem contains operator data (string data type, has ID).
		Comment,			///< Lexem contains comment data (string data type).
		ConstantString,		///< Lexem contains string constant (string data type).
		ConstantCharacter,	///< Lexem contains character constant (character data type).
		ConstantInteger,	///< Lexem contains integer constant (integer data type).
		ConstantFloat,		///< Lexem contains floating-point constant (float data type).
		Unknown,			///< Lexem contains unknown data. (Internal usage only).
		EndOfStream
	};	// enum LexemContentType
	SizeTp static const LexemContentTypesCount = static_cast<SizeTp>(LexemContentType::Unknown);

	//! Returns human readable string for lexem content type
	GDAPI extern CStr LexemContentTypeToString(LexemContentType const ContentType);
	
	typedef UInt16 StreamedLexerID;
	typedef StreamedLexerID StreamedLexerKeyword;
	typedef StreamedLexerID StreamedLexerOperator;
	typedef Pair<StreamedLexerKeyword, String> StreamedLexerKeywordDecl;
	typedef Pair<StreamedLexerOperator, String> StreamedLexerOperatorDecl;
	typedef Vector<StreamedLexerKeywordDecl> StreamedLexerKeywordsList;
	typedef Vector<StreamedLexerOperatorDecl> StreamedLexerOperatorsList;

	//! @brief Describes information about one lexem.
	struct Lexem final : public IUncopiable
	{
		friend class StreamedLexerImpl;
		friend class BasicStreamedLexerImpl;

	private:
		SizeTp Line = 0;
		SizeTp Symbol = 0;
		LexemContentType ContentType = LexemContentType::Unknown;
		String RawData;
		union {
			Char  ProcessedDataCharacter;
			UInt64    ProcessedDataInteger;
			Float64   ProcessedDataFloat;
			StreamedLexerID ProcessedDataID;
			UInt8     Padding[8];
		};	

		GDINL void ResetLexem()
		{
			Line = 0;
			Symbol = 0;
			ContentType = LexemContentType::Unknown;
			RawData = "";
			memset(&Padding[0], 0, sizeof(Padding));
		}

	public:
		GDINL  Lexem() { }
		GDINL ~Lexem() { ResetLexem(); }
		
		//! Returns position of this lexem in input stream.
		GDINL SizeTp GetLine()      const { return Line; }
		GDINL SizeTp GetSymbol()    const { return Symbol; }

		//! Returns type of content that is located inside lexem.
		GDINL LexemContentType GetContentType()  const { return ContentType; }

		//! Returns raw lexem data that is 2015ed as string.
		GDINL String const& GetRawData()   const { return RawData; }

		//! Returns character data, located inside this lexem if this lexem is character contant.
		GDINL Char GetProcessedDataChar()  const 
		{
			GD_DEBUG_ASSERT(GetContentType() == LexemContentType::ConstantCharacter, "Lexem`s content type is not character constant");
			return ProcessedDataCharacter; 
		}
		
		//! Returns integer data, located inside this lexem if this lexem is integer contant.
		GDINL UInt64 GetProcessedDataInt()   const 
		{ 
			GD_DEBUG_ASSERT(GetContentType() == LexemContentType::ConstantInteger, "Lexem`s content type is not integer constant");
			return ProcessedDataInteger; 
		}
		
		//! Returns float data, located inside this lexem if this lexem is float contant.
		GDINL Float64 GetProcessedDataFloat() const 
		{ 
			GD_DEBUG_ASSERT(GetContentType() == LexemContentType::ConstantFloat, "Lexem`s content type is not float constant");
			return ProcessedDataFloat; 
		}
		
		//! Returns some ID data, located inside this lexem if this lexem can has ID.
		GDINL StreamedLexerID GetProcessedDataID() const 
		{ 
			if ((GetContentType() != LexemContentType::Keyword) && (GetContentType() != LexemContentType::Operator)) {
				return 0;
			}

			return ProcessedDataID;
		}
	};	// struct Lexem

	//! Runtime error that caused be some tool of toolchain.
	class ToolchainException  : public Exception
	{
		public : 
			GDINL explicit ToolchainException(void*, String const& ErrorMessage, ...) : Exception(ErrorMessage) { }
			GDINL explicit ToolchainException(String const& ErrorMessage, ...) : Exception(ErrorMessage) { }
			GDINL virtual ~ToolchainException() { } 
			GDINL virtual CStr GetExceptionType() const override  { return "ToolchainException"; }
	}; ;

	class InputStream;

	//! @brief Provides lexer information about syntax in compilable data.
	//! @note Instance of this class is quiet a "heavy" object, so it is recommended to make a static object. 
	//! @note Before creating options instance check if there is a default options for your language. 
	struct StreamedLexerOptions final : public IUncopiable
	{
	private:
		String SpecialCharactersAlphabet;

	public:
		StreamedLexerKeywordsList  const KeywordsDeclarations;					///< Keywords declarations. Such as 'class' or 'if' and etc.
		StreamedLexerOperatorsList const OperatorDeclarations;					///< Operators declarations. Such as '+', '-' and etc.
		String                     const SingleLineCommentDeclaration;			///< One line comment declaration. '//' basically 
		String                     const MultipleLineCommentBeginning;			///< Multiple line comment declaration. '/*' or '<!--' basically 
		String                     const MultipleLineCommentEnding;				///< Multiple line comment ending. '*/' or '-->' basically 
		Char                       const IntegerHexadecimalNotationDelimiter;	///< Character that provides translating this integer into hexadecimal notation. (E.g. 0xFF08A3)
		Char                       const IntegerOctalNotationDelimiter;			///< Character that provides translating this integer into octal notation. (E.g. 0c166672)
		Char                       const IntegerBinaryNotationDelimiter;		///< Character that provides translating this integer into binary notation.	(E.g. 0b10010110)
		Char                       const FloatingPointDelimiter;				///< Character that provides breaking floating point value into integral and fractional parts. (E.g. -0.46)

		//! @brief Creates options instance and validates parameters (in debug builds only).
		//!        Each operator, comment declarations should be unique.
		GDAPI StreamedLexerOptions(
			StreamedLexerKeywordsList const& KeywordsDeclarations,
			StreamedLexerOperatorsList const& OperatorDeclarations,
			String const& SingleLineCommentDeclaration,
			String const& MultipleLineCommentBeginning,
			String const& MultipleLineCommentEnding,
			Char const IntegerHexadecimalNotationDelimiter,
			Char const IntegerOctalNotationDelimiter,
			Char const IntegerBinaryNotationDelimiter,
			Char const FloatingPointDelimiter
		);

		//! @brief Returns true if this character exists alphabet of special characters this options instance uses.
		GDINL bool IsInSpecialCharactersAlphabet(Char const Character) const 
		{	// Searching for specified character inside alphabet. 
			return (SpecialCharactersAlphabet.Find(Character) != -1); 
		}

	private:
		GDINL void AppendToSpecialCharactersAlphabet(Char const Character)
		{	// Extending alphabet if character does not already exists there .
			if (!IsInSpecialCharactersAlphabet(Character)) SpecialCharactersAlphabet += Character;
		}
	};	// struct StreamedLexerOptions

	//! @brief List of keywords used in C++-like languages.
	namespace CPPKeywords
	{
		enum TValue : StreamedLexerKeyword
		{
			KeywordUnknown = 0                        ,
			KeywordAsm                                ,        ///< "asm" keyword.
			KeywordAuto                               ,        ///< "auto" keyword.
			KeywordBool                               ,        ///<
			KeywordBreak                              ,        ///<
			KeywordCase                               ,        ///<
			KeywordCatch                              ,        ///<
			KeywordChar                               ,        ///<
			KeywordClass                              ,        ///<
			KeywordConst                              ,        ///<
			KeywordConstexpr                          ,        ///<
			KeywordConstCast                          ,        ///<
			KeywordContinue                           ,        ///<
			KeywordDecltype                           ,        ///<
			KeywordDefault                            ,        ///<
			KeywordDelete                             ,        ///<
			KeywordDo                                 ,        ///<
			KeywordDouble                             ,        ///<
			KeywordDynamicCast                        ,        ///<
			KeywordElse                               ,        ///<
			KeywordEnum                               ,        ///<
			KeywordExplicit                           ,        ///<
			KeywordExport                             ,        ///<
			KeywordExtern                             ,        ///<
			KeywordFalse                              ,        ///<
			KeywordFinal                              ,        ///<
			KeywordFinally                            ,        ///<
			KeywordFloat                              ,        ///<
			KeywordFor                                ,        ///<
			KeywordFriend                             ,        ///<
			KeywordGoto                               ,        ///<
			KeywordIf                                 ,        ///<
			KeywordInline                             ,        ///<
			KeywordInt                                ,        ///<
			KeywordLong                               ,        ///<
			KeywordMutable                            ,        ///<
			KeywordNamespace                          ,        ///<
			KeywordNew                                ,        ///<
			KeywordNoexpect                           ,        ///<
			KeywordNullptr                            ,        ///<
			KeywordOperator                           ,        ///<
			KeywordPrivate                            ,        ///<
			KeywordProtected                          ,        ///<
			KeywordPublic                             ,        ///<
			KeywordRegister                           ,        ///<
			KeywordReinterpretCast                    ,        ///<
			KeywordReturn                             ,        ///<
			KeywordShort                              ,        ///<
			KeywordSigned                             ,        ///<
			KeywordSizeOf                             ,        ///<
			KeywordStatic                             ,        ///<
			KeywordStaticAssert                       ,        ///<
			KeywordStaticCast                         ,        ///<
			KeywordStruct                             ,        ///<
			KeywordSwitch                             ,        ///<
			KeywordTemplate                           ,        ///<
			KeywordThis                               ,        ///<
			KeywordThreadLocal                        ,        ///<
			KeywordThrow                              ,        ///<
			KeywordTrue                               ,        ///<
			KeywordTry                                ,        ///<
			KeywordTypedef                            ,        ///<
			KeywordTypeID                             ,        ///<
			KeywordTypename                           ,        ///<
			KeywordUnion                              ,        ///<
			KeywordUnsigned                           ,        ///<
			KeywordUsing                              ,        ///<
			KeywordVirtual                            ,        ///<
			KeywordVoid                               ,        ///<
			KeywordVolatile                           ,        ///<
			KeywordWhile                              ,        ///<
		};	// enum ValueType
	};	// namespace CPPKeywords

	//! @brief List of keywords used in C++-like languages.
	namespace CPPOperators
	{
		enum TValue : StreamedLexerOperator
		{
			OperatorUnknown = 0                       ,
			OperatorAssing                            ,        ///< "="
			OperatorAdd                               ,        ///< "+"
			OperatorSub                               ,        ///< "-"
			OperatorMul                               ,        ///< "*"
			OperatorDiv                               ,        ///< "/"
			OperatorMod                               ,        ///< "%"
			OperatorInc                               ,        ///< "++"
			OperatorDec                               ,        ///< "--"
			OperatorEquals                            ,        ///< "=="
			OperatorNotEquals                         ,        ///< "!="
			OperatorGreater                           ,        ///< "<"
			OperatorLess                              ,        ///< ">"
			OperatorGreaterEquals                     ,        ///< "<="
			OperatorLessEquals                        ,        ///< ">="
			OperatorNot                               ,        ///< "!"
			OperatorAnd                               ,        ///< "&&"
			OperatorOr                                ,        ///< "||"
			OperatorBitwisiNot                        ,        ///< "~"
			OperatorBitwiseAnd                        ,        ///< "&"
			OperatorBitwiseOr                         ,        ///< "|"
			OperatorBitwiseXor                        ,        ///< "^"
			OperatorBitwiseLeftShift                  ,        ///< "<<"
			OperatorBitwiseRightShift                 ,        ///< ">>"
			OperatorAddAssign                         ,        ///< "+="
			OperatorSubAssign                         ,        ///< "-="
			OperatorMulAssign                         ,        ///< "*="
			OperatorDivAssign                         ,        ///< "/="
			OperatorModAssign                         ,        ///< "%="
			OperatorBitwiseAndAssign                  ,        ///< "&="
			OperatorBitwiseOrAssign                   ,        ///< "|="
			OperatorBitwiseXorAssign                  ,        ///< "^="
			OperatorBitwiseLeftShiftAssign            ,        ///< "<<="
			OperatorBitwiseRightShiftAssign           ,        ///< ">>="
			OperatorIndexBegin                        ,        ///< "["
			OperatorIndexEnd                          ,        ///< "]"
			OperatorScopeBegin                        ,        ///< "{"
			OperatorScopeEnd                          ,        ///< "}"
			OperatorParamsBegin                       ,        ///< "("
			OperatorParamsEnd                         ,        ///< ")"
			OperatorStructDereferencing               ,        ///< "->"
			OperatorStructReferencing                 ,        ///< "."
			OperatorMemberPtrDereferencing            ,        ///< "->*"
			OperatorMemberPtrReferencing              ,        ///< ".*"
			OperatorComma                             ,        ///< ","
			OperatorColon                             ,        ///< ":"
			OperatorSemicolon                         ,        ///< ";"
			OperatorTernary                           ,        ///< "?"
			OperatorScopeResolution                   ,        ///< "::"
			OperatorParamPack                         ,        ///< "..."
			OperatorPreprocessor                      ,        ///< "#"
			OperatorPreprocessorGlue                  ,        ///< "##"
			OperatorDereferencing  = OperatorMul      ,        ///< "*"
			OperatorReferencing = OperatorBitwiseAnd  ,        ///< "&" 
			OperatorTemplateBegin = OperatorGreater   ,        ///< "<"
			OperatorTemplateEnd   = OperatorLess      ,        ///< ">"
		};	// enum ValueType
	};	// namespace CPPOperators

	//! @brief Class provides streamable tokenization for input stream.
	//! @note Instance of this class is quiet a "heavy" object, so it is recommended to make a static object. 
	class StreamedLexer final 
	{ 
	private:
		UniquePtr<class StreamedLexerImpl> Implementation;
		StreamedLexerOptions const& Options;

	public:
		//! Initializes new streamed lexer.
		//! @param Stream input stream on which lexer would work.
		//! @param Options Packed lexing options list.
		GDAPI StreamedLexer(InputStream* Stream, StreamedLexerOptions const& Options);
		GDAPI ~StreamedLexer();

		//! Extracts next lexem from input stream into output. Throws an exception into toolchain if error occurs. Can be ran in simple and advanced mode.
		//! @param OutputLexem Pointer to output lexem variable.
		//! @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
		GDAPI bool GetNextLexem(Lexem* const OutputLexem);

		//! Extracts next character from input stream and wraps line breaks, etc.
		//! @returns Extracted character.
		GDAPI Char GetNextCharacter();

		//! Pushes specified character back to lexer and make it parsable.
		//! @param Character Character that would be reverted.
		GDAPI void RevertCharacter(Char const Character);
	};	// class StreamedLexer

	//! Returns predefined tokenizer options for C++-like languages (C, C++, C#, JavaScript, HLSL, GLSL, Cg)
	GDAPI extern StreamedLexerOptions const& GetDefaultOptionsForCpp();

	//! Returns predefined tokenizer options for XML-like languages (XML, (X)HTML)
	GDAPI extern StreamedLexerOptions const& GetDefaultOptionsForXml();

GD_NAMESPACE_END
