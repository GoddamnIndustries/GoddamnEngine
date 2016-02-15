// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/ShaderCrossCompiler/HLSLTokenizer.h
 * Stream tokenizer.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/System/IO/Stream.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                   Lexem class.                                   ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Describes all supported lexem types.
	 */
	//! @{
	enum class LexemContentType : UInt8
	{
		Identifier,			//!< Lexem contains identifier data (string data type).
		Keyword,			//!< Lexem contains keyword data (string data type, has ID).
		Operator,			//!< Lexem contains operator data (string data type, has ID).
		Comment,			//!< Lexem contains comment data (string data type).
		ConstantString,		//!< Lexem contains string constant (string data type).
		ConstantCharacter,	//!< Lexem contains character constant (character data type).
		ConstantInteger,	//!< Lexem contains integer constant (integer data type).
		ConstantFloat,		//!< Lexem contains floating-point constant (float data type).
		Unknown,			//!< Lexem contains unknown data. (Internal usage only).
		EndOfStream
	};	// enum LexemContentType
	SizeTp static const LexemContentTypesCount = static_cast<SizeTp>(LexemContentType::Unknown);
	//! @}

	/*!
	 * Returns human readable string for lexem content type.
	 */
	GDAPI extern CStr LexemContentTypeToString(LexemContentType const ContentType);

	using StreamedLexerID            = UInt16;
	using StreamedLexerKeyword       = StreamedLexerID;
	using StreamedLexerOperator      = StreamedLexerID;
	using StreamedLexerKeywordDecl   = Pair<StreamedLexerKeyword, String>;
	using StreamedLexerOperatorDecl  = Pair<StreamedLexerOperator, String>;
	using StreamedLexerKeywordsList  = Vector<StreamedLexerKeywordDecl>;
	using StreamedLexerOperatorsList = Vector<StreamedLexerOperatorDecl>;

	// **------------------------------------------------------------------------------------------**
	//! Describes information about the single lexem.
	// **------------------------------------------------------------------------------------------**
	struct Lexem final : public TNonCopyable
	{
		friend class StreamedLexerImpl;

	private:
		SizeTp              m_Line = 0;
		SizeTp              m_Symbol = 0;
		LexemContentType    m_ContentType = LexemContentType::Unknown;
		String              m_RawData;
		union {
			Char            m_ProcessedDataCharacter;
			UInt64          m_ProcessedDataInteger;
			Float64         m_ProcessedDataFloat;
			StreamedLexerID m_ProcessedDataID;
			UInt8           m_Padding[8];
		};	

	public:

		/*!
		 * Resets the lexem to it's initial state.
		 */
		GDINL void ResetLexem()
		{
			m_Line = 0;
			m_Symbol = 0;
			m_ContentType = LexemContentType::Unknown;
			m_RawData = "";
			CMemory::Memset(m_Padding, 0, sizeof m_Padding);
		}

		/*!
		 * Returns position of this lexem in input stream.
		 */
		//! @{
		GDINL SizeTp GetLine() const
		{
			return m_Line;
		}
		GDINL SizeTp GetSymbol() const
		{
			return m_Symbol;
		}
		//! @}

		/*!
		 * Returns type of content that is located inside lexem.
		 */
		GDINL LexemContentType GetContentType()  const
		{
			return m_ContentType;
		}

		/*!
		 * Returns raw lexem data that is 2015ed as string.
		 */
		GDINL String const& GetRawData()   const
		{
			return m_RawData;
		}

		/*!
		 * Returns character data, located inside this lexem if this lexem is character constant.
		 */
		GDINL Char GetProcessedDataChar() const
		{
			GD_DEBUG_ASSERT(GetContentType() == LexemContentType::ConstantCharacter, "Lexem`s content type is not a character constant.");
			return m_ProcessedDataCharacter; 
		}
		
		/*!
		 * Returns integer data, located inside this lexem if this lexem is integer constant.
		 */
		GDINL UInt64 GetProcessedDataInt() const
		{ 
			GD_DEBUG_ASSERT(GetContentType() == LexemContentType::ConstantInteger, "Lexem`s content type is not an integer constant.");
			return m_ProcessedDataInteger; 
		}
		
		/*!
		 * Returns float data, located inside this lexem if this lexem is float constant.
		 */
		GDINL Float64 GetProcessedDataFloat() const
		{ 
			GD_DEBUG_ASSERT(GetContentType() == LexemContentType::ConstantFloat, "Lexem`s content type is not a floating-point constant.");
			return m_ProcessedDataFloat; 
		}
		
		/*!
		 * Returns some ID data, located inside this lexem if this lexem can has ID.
		 */
		GDINL StreamedLexerID GetProcessedDataID() const
		{
			return GetContentType() == LexemContentType::Keyword || GetContentType() == LexemContentType::Operator ? m_ProcessedDataID : 0;
		}
	};	// struct Lexem

	//! Runtime error that caused be some tool of toolchain.
	class ToolchainException 
	{
	public: 
		GDINL explicit ToolchainException(void*, String const& ErrorMessage, ...) /*: Exception(ErrorMessage)*/ { }
		GDINL explicit ToolchainException(String const& ErrorMessage, ...) /*: Exception(ErrorMessage)*/ { }
		GDINL virtual ~ToolchainException() { } 
		GDINL virtual CStr GetExceptionType() const { return "ToolchainException"; }
	}; 

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               StreamedLexer class.                               ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Class provides on-stream lexing for input stream.
	// **------------------------------------------------------------------------------------------**
	class StreamedLexer final : public TNonCopyable
	{ 
	private:
		UniquePtr<class StreamedLexerImpl> m_Implementation;
		struct StreamedLexerOptions const& m_Options;

	public:

		/*!
		 * Initializes the lexer with the specified options and input stream.
		 * @param stream Input stream on which lexer would work.
		 * @param options Packed lexing options list.
		 */
		GDAPI explicit StreamedLexer(InputStream* const stream, StreamedLexerOptions const& options /*= StreamedLexerOptions::GetDefaultOptionsForCpp()*/);

		GDAPI ~StreamedLexer();
	
	public:

		/*!
		 * Reads next lexem from input stream.
		 * @param outputLexem Pointer to the output lexem struct.
		 */
		GDAPI void GetNextLexem(Lexem* const outputLexem);

		/*!
		 * Reads next character from input stream. 
		 * @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
		 */
		GDAPI Char GetNextCharacter();

		/*!
		 * Pushes character back to input stream/caches char.
		 * @param character Char that would be reverted.
		 */
		GDAPI void RevertCharacter(Char const character);

	};	// class StreamedLexer

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           StreamedLexerOptions struct.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Provides syntax information for lexing.
	// **------------------------------------------------------------------------------------------**
	struct StreamedLexerOptions final : public TNonCopyable
	{
	private:
		String SpecialCharactersAlphabet;

	public:
		StreamedLexerKeywordsList  const KeywordsDeclarations;					//!< Keywords declarations. Such as 'class' or 'if' and etc.
		StreamedLexerOperatorsList const OperatorDeclarations;					//!< Operators declarations. Such as '+', '-' and etc.
		String                     const SingleLineCommentDeclaration;			//!< One line comment declaration. '//' basically 
		String                     const MultipleLineCommentBeginning;			//!< Multiple line comment declaration. '/*' or '<!--' basically 
		String                     const MultipleLineCommentEnding;				//!< Multiple line comment ending. '*/' or '-->' basically 
		Char                       const IntegerHexadecimalNotationDelimiter;	//!< Character that provides translating this integer into hexadecimal notation. (E.g. 0xFF08A3)
		Char                       const IntegerOctalNotationDelimiter;			//!< Character that provides translating this integer into octal notation. (E.g. 0c166672)
		Char                       const IntegerBinaryNotationDelimiter;		//!< Character that provides translating this integer into binary notation.	(E.g. 0b10010110)
		Char                       const FloatingPointDelimiter;				//!< Character that provides breaking floating point value into integral and fractional parts. (E.g. -0.46)

	public:
		GDAPI StreamedLexerOptions(StreamedLexerKeywordsList const& keywordsDeclarations, StreamedLexerOperatorsList const& operatorDeclarations
			, String const& singleLineCommentDeclaration, String const& multipleLineCommentBeginning, String const& multipleLineCommentEnding
			, Char const integerHexadecimalNotationDelimiter, Char const integerOctalNotationDelimiter, Char const integerBinaryNotationDelimiter
			, Char const floatingPointDelimiter
			);

		/*!
		 * Returns predefined tokenizer options for C++-like languages (C, C++, C#, JavaScript, HLSL, GLSL, Cg).
		 */
		GDAPI static StreamedLexerOptions const& GetDefaultOptionsForCpp();

		/*!
		 * Returns predefined tokenizer options for XML-like languages (XML, (X)HTML)
		 */
		GDAPI static StreamedLexerOptions const& GetDefaultOptionsForXml();

		/*!
		 * Returns true if this character exists alphabet of special characters this options instance uses.
		 */
		GDINL bool IsInSpecialCharactersAlphabet(Char const Character) const
		{	
			return SpecialCharactersAlphabet.Find(Character) != -1; 
		}

	private:
		GDINL void AppendToSpecialCharactersAlphabet(Char const Character)
		{	
			if (!IsInSpecialCharactersAlphabet(Character)) SpecialCharactersAlphabet.Append(Character);
		}
	};	// struct StreamedLexerOptions

	namespace CPPKeywords
	{
		enum TValue : StreamedLexerKeyword
		{
			KeywordUnknown = 0                        ,
			KeywordAsm                                ,        //!< "asm" keyword.
			KeywordAuto                               ,        //!< "auto" keyword.
			KeywordBool                               ,        //!<
			KeywordBreak                              ,        //!<
			KeywordCase                               ,        //!<
			KeywordCatch                              ,        //!<
			KeywordChar                               ,        //!<
			KeywordClass                              ,        //!<
			KeywordConst                              ,        //!<
			KeywordConstexpr                          ,        //!<
			KeywordConstCast                          ,        //!<
			KeywordContinue                           ,        //!<
			KeywordDecltype                           ,        //!<
			KeywordDefault                            ,        //!<
			KeywordDelete                             ,        //!<
			KeywordDo                                 ,        //!<
			KeywordDouble                             ,        //!<
			KeywordDynamicCast                        ,        //!<
			KeywordElse                               ,        //!<
			KeywordEnum                               ,        //!<
			KeywordExplicit                           ,        //!<
			KeywordExport                             ,        //!<
			KeywordExtern                             ,        //!<
			KeywordFalse                              ,        //!<
			KeywordFinal                              ,        //!<
			KeywordFinally                            ,        //!<
			KeywordFloat                              ,        //!<
			KeywordFor                                ,        //!<
			KeywordFriend                             ,        //!<
			KeywordGoto                               ,        //!<
			KeywordIf                                 ,        //!<
			KeywordInline                             ,        //!<
			KeywordInt                                ,        //!<
			KeywordLong                               ,        //!<
			KeywordMutable                            ,        //!<
			KeywordNamespace                          ,        //!<
			KeywordNew                                ,        //!<
			KeywordNoexpect                           ,        //!<
			KeywordNullptr                            ,        //!<
			KeywordOperator                           ,        //!<
			KeywordPrivate                            ,        //!<
			KeywordProtected                          ,        //!<
			KeywordPublic                             ,        //!<
			KeywordRegister                           ,        //!<
			KeywordReinterpretCast                    ,        //!<
			KeywordReturn                             ,        //!<
			KeywordShort                              ,        //!<
			KeywordSigned                             ,        //!<
			KeywordSizeOf                             ,        //!<
			KeywordStatic                             ,        //!<
			KeywordStaticAssert                       ,        //!<
			KeywordStaticCast                         ,        //!<
			KeywordStruct                             ,        //!<
			KeywordSwitch                             ,        //!<
			KeywordTemplate                           ,        //!<
			KeywordThis                               ,        //!<
			KeywordThreadLocal                        ,        //!<
			KeywordThrow                              ,        //!<
			KeywordTrue                               ,        //!<
			KeywordTry                                ,        //!<
			KeywordTypedef                            ,        //!<
			KeywordTypeID                             ,        //!<
			KeywordTypename                           ,        //!<
			KeywordUnion                              ,        //!<
			KeywordUnsigned                           ,        //!<
			KeywordUsing                              ,        //!<
			KeywordVirtual                            ,        //!<
			KeywordVoid                               ,        //!<
			KeywordVolatile                           ,        //!<
			KeywordWhile                              ,        //!<
		};	// enum ValueType
	};	// namespace CPPKeywords

	namespace CPPOperators
	{
		enum TValue : StreamedLexerOperator
		{
			OperatorUnknown = 0                       ,
			OperatorAssing                            ,        //!< "="
			OperatorAdd                               ,        //!< "+"
			OperatorSub                               ,        //!< "-"
			OperatorMul                               ,        //!< "*"
			OperatorDiv                               ,        //!< "/"
			OperatorMod                               ,        //!< "%"
			OperatorInc                               ,        //!< "++"
			OperatorDec                               ,        //!< "--"
			OperatorEquals                            ,        //!< "=="
			OperatorNotEquals                         ,        //!< "!="
			OperatorGreater                           ,        //!< "<"
			OperatorLess                              ,        //!< ">"
			OperatorGreaterEquals                     ,        //!< "<="
			OperatorLessEquals                        ,        //!< ">="
			OperatorNot                               ,        //!< "!"
			OperatorAnd                               ,        //!< "&&"
			OperatorOr                                ,        //!< "||"
			OperatorBitwisiNot                        ,        //!< "~"
			OperatorBitwiseAnd                        ,        //!< "&"
			OperatorBitwiseOr                         ,        //!< "|"
			OperatorBitwiseXor                        ,        //!< "^"
			OperatorBitwiseLeftShift                  ,        //!< "<<"
			OperatorBitwiseRightShift                 ,        //!< ">>"
			OperatorAddAssign                         ,        //!< "+="
			OperatorSubAssign                         ,        //!< "-="
			OperatorMulAssign                         ,        //!< "*="
			OperatorDivAssign                         ,        //!< "/="
			OperatorModAssign                         ,        //!< "%="
			OperatorBitwiseAndAssign                  ,        //!< "&="
			OperatorBitwiseOrAssign                   ,        //!< "|="
			OperatorBitwiseXorAssign                  ,        //!< "^="
			OperatorBitwiseLeftShiftAssign            ,        //!< "<<="
			OperatorBitwiseRightShiftAssign           ,        //!< ">>="
			OperatorIndexBegin                        ,        //!< "["
			OperatorIndexEnd                          ,        //!< "]"
			OperatorScopeBegin                        ,        //!< "{"
			OperatorScopeEnd                          ,        //!< "}"
			OperatorParamsBegin                       ,        //!< "("
			OperatorParamsEnd                         ,        //!< ")"
			OperatorStructDereferencing               ,        //!< "->"
			OperatorStructReferencing                 ,        //!< "."
			OperatorMemberPtrDereferencing            ,        //!< "->*"
			OperatorMemberPtrReferencing              ,        //!< ".*"
			OperatorComma                             ,        //!< ","
			OperatorColon                             ,        //!< ":"
			OperatorSemicolon                         ,        //!< ";"
			OperatorTernary                           ,        //!< "?"
			OperatorScopeResolution                   ,        //!< "::"
			OperatorParamPack                         ,        //!< "..."
			OperatorPreprocessor                      ,        //!< "#"
			OperatorPreprocessorGlue                  ,        //!< "##"
			OperatorDereferencing  = OperatorMul      ,        //!< "*"
			OperatorReferencing = OperatorBitwiseAnd  ,        //!< "&" 
			OperatorTemplateBegin = OperatorGreater   ,        //!< "<"
			OperatorTemplateEnd   = OperatorLess      ,        //!< ">"
		};	// enum ValueType
	};	// namespace CPPOperators

GD_NAMESPACE_END
