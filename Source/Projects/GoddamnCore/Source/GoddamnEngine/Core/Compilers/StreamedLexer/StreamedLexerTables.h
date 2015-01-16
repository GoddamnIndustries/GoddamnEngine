/// ==========================================================================================
/// StreamedLexerTables.h - lexer data tables.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Compilers/CompilerBase.h>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.h>

GD_NAMESPACE_BEGIN
	
	/// @brief Provides lexer information about syntax in compilable data.
	/// @note Instance of this class is quiet a "heavy" object, so it is recommended to make a static object. 
	/// @note Before creating options instance check if there is a default options for your language. 
	struct StreamedLexerOptions final
	{
	private:
		GD_CLASS_UNASSIGNABLE(StreamedLexerOptions);
		GD_CLASS_UNSWAPPABLE (StreamedLexerOptions);
		GD_CLASS_UNCOPIABLE  (StreamedLexerOptions);
		GD_CLASS_UNMOVABLE   (StreamedLexerOptions);

		String SpecialCharactersAlphabet;

	public:
		StreamedLexerKeywordsList  const KeywordsDeclarations;		///< Keywords declarations. Such as 'class' or 'if' and etc.
		StreamedLexerOperatorsList const OperatorDeclarations;		///< Operators declarations. Such as '+', '-' and etc.
		String const SingleLineCommentDeclaration;					///< One line comment declaration. '//' basically 
		String const MultipleLineCommentBeginning;					///< Multiple line comment declaration. '/*' or '<!--' basically 
		String const MultipleLineCommentEnding;						///< Multiple line comment ending. '*/' or '-->' basically 
		CharAnsi const IntegerHexadecimalNotationDelimiter;			///< Character that provides translating this integer into hexadecimal notation. (E.g. 0xFF08A3)
		CharAnsi const IntegerOctalNotationDelimiter;				///< Character that provides translating this integer into octal notation. (E.g. 0c166672)
		CharAnsi const IntegerBinaryNotationDelimiter;				///< Character that provides translating this integer into binary notation.	(E.g. 0b10010110)
		CharAnsi const FloatingPointDelimiter;						///< Character that provides breaking floating point value into integral and fractional parts. (E.g. -0.46)

		/// @brief Creates options instance and validates parameters (in debug builds only).
		///        Each operator, comment declarations should be unique.
		GDAPI StreamedLexerOptions(
			StreamedLexerKeywordsList const& KeywordsDeclarations,
			StreamedLexerOperatorsList const& OperatorDeclarations,
			String const& SingleLineCommentDeclaration,
			String const& MultipleLineCommentBeginning,
			String const& MultipleLineCommentEnding,
			CharAnsi const IntegerHexadecimalNotationDelimiter,
			CharAnsi const IntegerOctalNotationDelimiter,
			CharAnsi const IntegerBinaryNotationDelimiter,
			CharAnsi const FloatingPointDelimiter
		);

		/// @brief Returns true if this character exists alphabet of special characters this options instance uses.
		GDINL bool IsInSpecialCharactersAlphabet(CharAnsi const Character) const 
		{	// Searching for specified character inside alphabet. 
			return (this->SpecialCharactersAlphabet.Find(Character) != -1); 
		}

	private:
		GDINL void AppendToSpecialCharactersAlphabet(CharAnsi const Character)
		{	// Extending alphabet if character does not already exists there .
			if (!this->IsInSpecialCharactersAlphabet(Character)) this->SpecialCharactersAlphabet += Character;
		}
	};	// struct StreamedLexerOptions

	/// @brief List of keywords used in C++-like languages.
	namespace CPPKeywords
	{
		enum ValueType : StreamedLexerKeyword
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

	/// @brief List of keywords used in C++-like languages.
	namespace CPPOperators
	{
		enum ValueType : StreamedLexerOperator
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


GD_NAMESPACE_END
