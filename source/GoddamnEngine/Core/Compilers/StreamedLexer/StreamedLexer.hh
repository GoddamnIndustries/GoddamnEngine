//////////////////////////////////////////////////////////////////////////
/// StreamedLexer.hh - lexer interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_COMPILERS_STREAMED_LEXER
#define GD_CORE_COMPILERS_STREAMED_LEXER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Containers/Pair/Pair.hh>
#include <GoddamnEngine/Core/Containers/String/String.hh>
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>

#include <string.h>

GD_NAMESPACE_BEGIN

	class Stream;

	/// Describes all supported lexem types.
	enum LexemContentType : UInt8
	{
		GD_LEXEM_CONTENT_TYPE_IDENTIFGDR,			///< Lexem contains identifier data (string data type).
		GD_LEXEM_CONTENT_TYPE_KEYWORD,			    ///< Lexem contains keyword data (string data type, has ID).
		GD_LEXEM_CONTENT_TYPE_OPERATOR,				///< Lexem contains operator data (string data type, has ID).
		GD_LEXEM_CONTENT_TYPE_COMMENT,				///< Lexem contains comment data (string data type).
		GD_LEXEM_CONTENT_TYPE_CONSTANT_STRING,		///< Lexem contains string constant (string data type).
		GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER,	///< Lexem contains character constant (character data type).
		GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER,		///< Lexem contains integer constant (integer data type).
		GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT,		///< Lexem contains floating-point constant (float data type).
		GD_LEXEM_CONTENT_TYPE_UNKNOWN,				///< Lexem contains unknown data. (Internal usage only).
		GD_LEXEM_CONTENT_TYPES_COUNT				= GD_LEXEM_CONTENT_TYPE_UNKNOWN,
		GD_LEXEM_CONTENT_TYPE_EOS
	};	// enum LexemContentType

	/// Returns numan readable string for lexem content type
	GDAPI extern Str LexemContentTypeToString(LexemContentType const ContentType);
	
	typedef UInt16 StreamedLexerId;
	typedef StreamedLexerId StreamedLexerKeyword;
	typedef StreamedLexerId StreamedLexerOperator;
	typedef Pair<StreamedLexerKeyword, String> StreamedLexerKeywordDecl;
	typedef Pair<StreamedLexerOperator, String> StreamedLexerOperatorDecl;
	typedef Vector<StreamedLexerKeywordDecl> StreamedLexerKeywordsList;
	typedef Vector<StreamedLexerOperatorDecl> StreamedLexerOperatorsList;

	/// Describes information about one lexem.
	struct Lexem final
	{
	private:
		friend class StreamedLexerImpl;
		GD_CLASS_UNASSIGNABLE(Lexem);
		GD_CLASS_UNSWAPPABLE (Lexem);
		GD_CLASS_UNCOPIABLE  (Lexem);
		GD_CLASS_UNMOVABLE   (Lexem);

		size_t              Line = 0;
		size_t              Symbol = 0;
		LexemContentType    ContentType = GD_LEXEM_CONTENT_TYPE_UNKNOWN;
		String              RawData;
		union
		{
			CharAnsi        ProcessedDataCharacter;
			UInt64          ProcessedDataInteger;
			Float64         ProcessedDataFloat;
			StreamedLexerId ProcessedDataId;
			UInt8           Padding[8];
		};	

		GDINL void ResetLexem()
		{
			self->Line = 0;
			self->Symbol = 0;
			self->ContentType = GD_LEXEM_CONTENT_TYPE_UNKNOWN;
			self->RawData.Emptify();
			memset(&self->Padding[0], 0, sizeof(self->Padding));
		}

	public:
		GDINL  Lexem() { }
		GDINL ~Lexem() { self->ResetLexem(); }
		
		/// Returns position of this lexem in input stream.
		GDINL size_t           GetLine()               const { return self->Line; }
		GDINL size_t           GetSymbol()             const { return self->Symbol; }

		/// Returns type of content that is located inside lexem.
		GDINL LexemContentType GetContentType()        const { return self->ContentType; }

		/// Returns raw lexem data that is presented as string.
		GDINL String const&    GetRawData()            const { return self->RawData; }

		/// Returns character data, located inside this lexem if this lexem is character contant.
		GDINL CharAnsi         GetProcessedDataChar()  const 
		{
			GD_DEBUG_ASSERT((self->GetContentType() == GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER), "Lexem`s content type is not character constant");
			return self->ProcessedDataCharacter; 
		}
		
		/// Returns integer data, located inside this lexem if this lexem is integer contant.
		GDINL UInt64           GetProcessedDataInt()   const 
		{ 
			GD_DEBUG_ASSERT((self->GetContentType() == GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER), "Lexem`s content type is not integer constant");
			return self->ProcessedDataInteger; 
		}
		
		/// Returns float data, located inside this lexem if this lexem is float contant.
		GDINL Float64          GetProcessedDataFloat() const 
		{ 
			GD_DEBUG_ASSERT((self->GetContentType() == GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT), "Lexem`s content type is not float constant");
			return self->ProcessedDataFloat; 
		}
		
		/// Returns some ID data, located inside this lexem if this lexem can has ID.
		GDINL StreamedLexerId  GetProcessedDataId()    const 
		{ 
			if ((self->GetContentType() != GD_LEXEM_CONTENT_TYPE_KEYWORD) && (self->GetContentType() != GD_LEXEM_CONTENT_TYPE_OPERATOR)) return 0;
			return self->ProcessedDataId;
		}
	};	// struct Lexem

	/// Contains codes of exceptions that streamed lexer throws.
	enum StreamedLexerExceptionCodes : ToolchainException
	{
		GD_LEXER_EXCEPTION_MODULE =  10L,							 ///< Streamed lexer module.
		GD_LEXER_EXCEPTION_EOS	  = -GD_LEXER_EXCEPTION_MODULE - 1L, ///< End-Of-Stream.
		GD_LEXER_EXCEPTION_COMMIT = -GD_LEXER_EXCEPTION_MODULE - 2L, ///< Token was commited.
		GD_LEXER_EXCEPTION_SYNTAX = +GD_LEXER_EXCEPTION_MODULE + 1L, ///< Syntax error.
	};	// enum StreamedLexerExceptionCodes

	/// Provides lexer information about syntax in compilable data.
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
		StreamedLexerKeywordsList  const KeywordsDeclarations;			///< Keywords declarations. Such as 'class' or 'if' and etc.
		StreamedLexerOperatorsList const OperatorDeclarations;			///< Operators declarations. Such as '+', '-' and etc.
			               String  const SingleLineCommentDeclaration;	///< One line comment declaration. '//' basically 
			               String  const MultipleLineCommentBeginning;	///< Multiple line comment declaration. '/*' or '<!--' basically 
			               String  const MultipleLineCommentEnding;		///< Multiple line comment ending. '*/' or '-->' basically 

		CharAnsi const IntegerHexadecimalNotationDelimiter;	///< Character that provides translating this integer into hexadecimal notation. (E.g. 0xFF08A3)
		CharAnsi const IntegerOctalNotationDelimiter;		///< Character that provides translating this integer into octal notation. (E.g. 0c166672)
		CharAnsi const IntegerBinaryNotationDelimiter;		///< Character that provides translating this integer into binary notation.	(E.g. 0b10010110)

		CharAnsi const FloatingPointDelimiter;				///< Character that provides breaking floating point value into integral and fractional parts. (E.g. -0.46)

		/// Creates options instance and validates parameters (in debug builds only).
		/// Each operator, comment declarations should be unique.
		GDAPI StreamedLexerOptions(
			_In_ StreamedLexerKeywordsList  && KeywordsDeclarations,
			_In_ StreamedLexerOperatorsList && OperatorDeclarations,
			_In_ String	                    && SingleLineCommentDeclaration,
			_In_ String	                    && MultipleLineCommentBeginning,
			_In_ String	                    && MultipleLineCommentEnding,
			_In_ CharAnsi const                IntegerHexadecimalNotationDelimiter,
			_In_ CharAnsi const                IntegerOctalNotationDelimiter,
			_In_ CharAnsi const                IntegerBinaryNotationDelimiter,
			_In_ CharAnsi const                FloatingPointDelimiter
		);

		/// Returns true if this character exists alphabet of special characters this options instance uses.
		GDINL bool IsInSpecialCharactersAlphabet(CharAnsi const Character) const 
		{	// Searching for specified character inside alphabet. 
			return (self->SpecialCharactersAlphabet.Find(Character) != -1); 
		}

	private:
		GDINL void AppendToSpecialCharactersAlphabet(CharAnsi const Character)
		{	// Extending alphabet if character does not already exists there .
			if (!self->IsInSpecialCharactersAlphabet(Character)) self->SpecialCharactersAlphabet += Character;
		}
	};	// struct StreamedLexerOptions

	/// Class provides streamable tokenization for input stream.
	/// @note Instance of this class is quiet a "heavy" object, so it is recommended to make a static object. 
	class StreamedLexer final : public IToolchainTool
	{ 
	private:
		GD_CLASS_UNASSIGNABLE(StreamedLexer);
		GD_CLASS_UNSWAPPABLE (StreamedLexer);
		GD_CLASS_UNCOPIABLE  (StreamedLexer);
		GD_CLASS_UNMOVABLE   (StreamedLexer);

		StreamedLexerOptions const& Options;
		size_t CurrentLine = 1;
		size_t CurrentSymbol = 0;

	public:
		GDINL  StreamedLexer(IToolchain* const Toolchain, StreamedLexerOptions const& Options) : IToolchainTool(Toolchain), Options(Options) { }
		GDINL ~StreamedLexer() { }

		/// Extracts next lexem from input stream into output. Throws an exception into toolchain if error accures. 
		/// @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
		GDAPI bool GetNextLexem(Stream* const InputStream, Lexem* const OutputLexem);
	};	// class StreamedLexer

	/// Contains some predefined options for streamed lexer.
	namespace StreamedLexerDefaultOptions
	{
		/// List of keywords used in C++-like languages.
		enum StreamedLexerOptionsCppKeywords : StreamedLexerKeyword
		{
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNKNOWN = 0,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ASM,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_AUTO,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_BOOL,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_BREAK,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CASE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CATCH,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CHAR,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CLASS,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONSTEXPR,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST_CAST,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONTINUE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DECLTYPE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DEFAULT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DELETE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DO,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DOUBLE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DYNAMIC_CAST,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ELSE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ENUM,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXPLICIT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXPORT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXTERN,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FALSE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FLOAT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FOR,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FRGDND,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_GOTO,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_IF,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_INLINE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_INT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_LONG,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_MUTABLE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NAMESPACE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NEW,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NOEXPECT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NULLPTR,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_OPERATOR,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PRIVATE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PROTECTED,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PUBLIC,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_REGISTER,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_REINTERPRET_CAST,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_RETURN,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SHORT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIGNED,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIZEOF,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC_ASSERT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC_CAST,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STRUCT,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SWITCH,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TEMPLATE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THIS,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THREAD_LOCAL,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THROW,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TRUE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TRY,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPEDEF,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPEID,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPENAME,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNION,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNSIGNED,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_USING,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VIRTUAL,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOID,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOLATILE,
			GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_WHILE,
		};	// enum StreamedLexerOptionsCppKeywords

		/// List of keywords used in C++-like languages.
		enum StreamedLexerOptionsCppOperators : StreamedLexerOperator
		{
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_UNKNOWN = 0,
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN,                     ///< "="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ADD,                        ///< "+"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SUB,                        ///< "-"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL,                        ///< "*"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DIV,                        ///< "/"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MOD,                        ///< "%"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INC,                        ///< "++"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DEC,                        ///< "--"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_EQUALS,                     ///< "=="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_NOT_EQUALS,                 ///< "!="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER,                    ///< "<"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS,                       ///< ">"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER_EQUAL,              ///< "<="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS_EQUALS,                ///< ">="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_NOT,                        ///< "!"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_AND,                        ///< "&&"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_OR,                         ///< "||"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_NOT,                ///< "~"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND,                ///< "&"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR,                 ///< "|"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_XOR,                ///< "^"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_LEFT_SHIFT,         ///< "<<"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT,        ///< ">>"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ADD_ASSIGN,                 ///< "+="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SUB_ASSIGN,                 ///< "-="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL_ASSIGN,                 ///< "*="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DIV_ASSIGN,                 ///< "/="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MOD_ASSIGN,                 ///< "%="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND_ASSIGN,         ///< "&="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR_ASSIGN,          ///< "|="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_XOR_ASSIGN,         ///< "^="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_LEFT_SHIFT_ASSIGN,  ///< "<<="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT_ASSIGN, ///< ">>="
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_BEGIN,                ///< "["
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_END,                  ///< "]"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN,                ///< "{"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END,                  ///< "}"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN,               ///< "("
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END,                 ///< ")"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_STRUCT_DEREFERENCING,       ///< "->"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_STRUCT_REFERENCING,         ///< "."
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MEMBER_PTR_DEREFERENCING,   ///< "->*"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MEMBER_PTR_REFERENCING,     ///< ".*"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA,                      ///< ","
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON,                      ///< ":"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SEMICOLON,                  ///< ";"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TERNARY,                    ///< "?"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_RESOLUTION,           ///< "::"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAM_PACK,                 ///< "..."
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR,               ///< "#"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR_GLUE,          ///< "##"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DEREFERENCING  = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL,         ///< "*"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_REFERENCING    = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND, ///< "&" 
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_BEGIN = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER,     ///< "<"
			GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_END   = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS,        ///< ">"
		};	// enum StreamedLexerOptionsCppOperators

		/// Returns predefined tokenizer options for C++-like languages (C, C++, C#, Javascript, HLSL, GLSL, Cg)
		GDAPI extern StreamedLexerOptions const& GetDefaultOptionsForCpp();

		/// Returns predefined tokenizer options for XML-like languages (XML, (X)HTML)
		GDAPI extern StreamedLexerOptions const& GetDefaultOptionsForXml();
	}	// namespace StreamedLexerDefaultOptions

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_COMPILERS_STREAMED_LEXER
