/// ==========================================================================================
/// StreamedLexerTables.cpp - lexer data tables.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexerTables.h>

GD_NAMESPACE_BEGIN

		StreamedLexerOptions::StreamedLexerOptions(
		StreamedLexerKeywordsList const& KeywordsDeclarations,
		StreamedLexerOperatorsList const& OperatorDeclarations,
		String const& SingleLineCommentDeclaration,
		String const& MultipleLineCommentBeginning,
		String const& MultipleLineCommentEnding,
		CharAnsi const IntegerHexadecimalNotationDelimiter,
		CharAnsi const IntegerOctalNotationDelimiter,
		CharAnsi const IntegerBinaryNotationDelimiter,
		CharAnsi const FloatingPointDelimiter
	)	: KeywordsDeclarations(KeywordsDeclarations)
		, OperatorDeclarations(OperatorDeclarations)
		, SingleLineCommentDeclaration(SingleLineCommentDeclaration)
		, MultipleLineCommentBeginning(MultipleLineCommentBeginning)
		, MultipleLineCommentEnding(MultipleLineCommentEnding)
		, IntegerHexadecimalNotationDelimiter((IntegerHexadecimalNotationDelimiter != CharAnsi('\0')) ? CharAnsiTraits::ToUpper(IntegerHexadecimalNotationDelimiter) : CharAnsi('\0'))
		, IntegerOctalNotationDelimiter((IntegerOctalNotationDelimiter != CharAnsi('\0')) ? CharAnsiTraits::ToUpper(IntegerOctalNotationDelimiter) : CharAnsi('\0'))
		, IntegerBinaryNotationDelimiter((IntegerBinaryNotationDelimiter != CharAnsi('\0')) ? CharAnsiTraits::ToUpper(IntegerBinaryNotationDelimiter) : CharAnsi('\0'))
		, FloatingPointDelimiter(FloatingPointDelimiter)
	{
		this->AppendToSpecialCharactersAlphabet(this->FloatingPointDelimiter);
		for (auto const Char : this->SingleLineCommentDeclaration) {
			this->AppendToSpecialCharactersAlphabet(Char);
		}
		for (auto const Char : this->MultipleLineCommentBeginning) {
			this->AppendToSpecialCharactersAlphabet(Char);
		}
		for (auto const& OperatorDeclaration : this->OperatorDeclarations) {
			String const& OperatorDeclarationString = OperatorDeclaration.Second;
			for (auto const Character : OperatorDeclarationString) {	// Checking if operator contains only special characters
				this->AppendToSpecialCharactersAlphabet(Character);
			}
		}
	}

	/// ==========================================================================================
	/// StreamedLexerDefaultOptions namespace.
	/// Contains some predefined options for streamed lexer.
	/// ==========================================================================================

	StreamedLexerOptions const& GetDefaultOptionsForCpp()
	{
#if (!defined(__INTELLISENSE__))
		using namespace CPPKeywords;
		using namespace CPPOperators;
		static StreamedLexerOptions const CppLexerOptions(
			StreamedLexerKeywordsList({
				{ KeywordAsm                        ,                        "asm"             },
				{ KeywordAuto                       ,                        "auto"            },
				{ KeywordBool                       ,                        "bool"            },
				{ KeywordBreak                      ,                        "break"           },
				{ KeywordCase                       ,                        "case"            },
				{ KeywordCatch                      ,                        "catch"           },
				{ KeywordChar                       ,                        "char"            },
				{ KeywordClass                      ,                        "class"           },
				{ KeywordConst                      ,                        "const"           },
				{ KeywordConstexpr                  ,                        "constexpr"       },
				{ KeywordConstCast                  ,                        "const_cast"      },
				{ KeywordContinue                   ,                        "continue"        },
				{ KeywordDecltype                   ,                        "decltype"        },
				{ KeywordDefault                    ,                        "default"         },
				{ KeywordDelete                     ,                        "delete"          },
				{ KeywordDo                         ,                        "do"              },
				{ KeywordDouble                     ,                        "double"          },
				{ KeywordDynamicCast                ,                        "dynamic_cast"    },
				{ KeywordElse                       ,                        "else"            },
				{ KeywordEnum                       ,                        "enum"            },
				{ KeywordExplicit                   ,                        "explicit"        },
				{ KeywordExport                     ,                        "export"          },
				{ KeywordExtern                     ,                        "extern"          },
				{ KeywordFalse                      ,                        "false"           },
				{ KeywordFinal                      ,                        "final"           },
				{ KeywordFinally                    ,                        "finally"         },
				{ KeywordFor                        ,                        "for"             },
				{ KeywordFriend                     ,                        "friend"          },
				{ KeywordGoto                       ,                        "goto"            },
				{ KeywordIf                         ,                        "if"              },
				{ KeywordInline                     ,                        "inline"          },
				{ KeywordInt                        ,                        "int"             },
				{ KeywordLong                       ,                        "long"            },
				{ KeywordMutable                    ,                        "mutable"         },
				{ KeywordNamespace                  ,                        "namespace"       },
				{ KeywordNew                        ,                        "new"             },
				{ KeywordNoexpect                   ,                        "noexpect"        },
				{ KeywordNullptr                    ,                        "nullptr"         },
				{ KeywordOperator                   ,                        "operator"        },
				{ KeywordPrivate                    ,                        "private"         },
				{ KeywordProtected                  ,                        "protected"       },
				{ KeywordPublic                     ,                        "public"          },
				{ KeywordRegister                   ,                        "register"        },
				{ KeywordReinterpretCast            ,                        "reinterpet_cast" },
				{ KeywordReturn                     ,                        "return"          },
				{ KeywordShort                      ,                        "short"           },
				{ KeywordSigned                     ,                        "signed"          },
				{ KeywordSizeOf                     ,                        "sizeof"          },
				{ KeywordStatic                     ,                        "static"          },
				{ KeywordStaticAssert               ,                        "static_assert"   },
				{ KeywordStaticCast                 ,                        "static_cast"     },
				{ KeywordStruct                     ,                        "struct"          },
				{ KeywordSwitch                     ,                        "switch"          },
				{ KeywordTemplate                   ,                        "template"        },
				{ KeywordThis                       ,                        "this"            },
				{ KeywordThreadLocal                ,                        "thread_local"    },
				{ KeywordThrow                      ,                        "throw"           },
				{ KeywordTrue                       ,                        "true"            },
				{ KeywordTry                        ,                        "try"             },
				{ KeywordTypedef                    ,                        "typedef"         },
				{ KeywordTypeID                     ,                        "typeid"          },
				{ KeywordTypename                   ,                        "typename"        },
				{ KeywordUnion                      ,                        "union"           },
				{ KeywordUnsigned                   ,                        "unsigned"        },
				{ KeywordUsing                      ,                        "using"           },
				{ KeywordVirtual                    ,                        "virtual"         },
				{ KeywordVoid                       ,                        "void"            },
				{ KeywordVolatile                   ,                        "volatile"        },
				{ KeywordWhile                      ,                        "while"           },
			}),
			StreamedLexerOperatorsList({
				{ OperatorAssing                    ,                        "="               },
				{ OperatorAdd                       ,                        "+"               },
				{ OperatorSub                       ,                        "-"               },
				{ OperatorMul                       ,                        "*"               },
				{ OperatorDiv                       ,                        "/"               },
				{ OperatorMod                       ,                        "%"               },
				{ OperatorInc                       ,                        "++"              },
				{ OperatorDec                       ,                        "--"              },
				{ OperatorEquals                    ,                        "=="              },
				{ OperatorNotEquals                 ,                        "!="              },
				{ OperatorGreater                   ,                        "<"               },
				{ OperatorLess                      ,                        ">"               },
				{ OperatorGreaterEquals             ,                        "<="              },
				{ OperatorLessEquals                ,                        ">="              },
				{ OperatorNot                       ,                        "!"               },
				{ OperatorAnd                       ,                        "&&"              },
				{ OperatorOr                        ,                        "||"              },
				{ OperatorBitwisiNot                ,                        "~"               },
				{ OperatorBitwiseAnd                ,                        "&"               },
				{ OperatorBitwiseOr                 ,                        "|"               },
				{ OperatorBitwiseXor                ,                        "^"               },
				{ OperatorBitwiseLeftShift          ,                        "<<"              },
				{ OperatorBitwiseRightShift         ,                        ">>"              },
				{ OperatorAddAssign                 ,                        "+="              },
				{ OperatorSubAssign                 ,                        "-="              },
				{ OperatorMulAssign                 ,                        "*="              },
				{ OperatorDivAssign                 ,                        "/="              },
				{ OperatorModAssign                 ,                        "%="              },
				{ OperatorBitwiseAndAssign          ,                        "&="              },
				{ OperatorBitwiseOrAssign           ,                        "|="              },
				{ OperatorBitwiseXorAssign          ,                        "^="              },
				{ OperatorBitwiseLeftShiftAssign    ,                        "<<="             },
				{ OperatorBitwiseRightShiftAssign   ,                        ">>="             },
				{ OperatorIndexBegin                ,                        "["               },
				{ OperatorIndexEnd                  ,                        "]"               },
				{ OperatorScopeBegin                ,                        "{"               },
				{ OperatorScopeEnd                  ,                        "}"               },
				{ OperatorParamsBegin               ,                        "("               },
				{ OperatorParamsEnd                 ,                        ")"               },
				{ OperatorStructDereferencing       ,                        "->"              },
				{ OperatorStructReferencing         ,                        "."               },
				{ OperatorMemberPtrDereferencing    ,                        "->*"             },
				{ OperatorMemberPtrReferencing      ,                        ".*"              },
				{ OperatorComma                     ,                        ","               },
				{ OperatorColon                     ,                        ":"               },
				{ OperatorSemicolon                 ,                        ";"               },
				{ OperatorTernary                   ,                        "?"               },
				{ OperatorScopeResolution           ,                        "::"              },
				{ OperatorParamPack                 ,                        "..."             },
				{ OperatorPreprocessor              ,                        "#"               },
				{ OperatorPreprocessorGlue          ,                        "##"              },
			}),
			"//", "/*", "*/", 
			'x', '\0', 'b', '.'
		);

		return CppLexerOptions;
#endif	// if (!defined(__INTELLISENSE__))
	}

	/// ==========================================================================================
	///	LexemContentType enum.
	/// ==========================================================================================

	extern Str LexemContentTypeToString(LexemContentType const ContentType)
	{
		static Str const LexemContentTypeStrings[] = {
			/* LexemContentType::Identifier			= */ "identifier",
			/* LexemContentType::Keyword			= */ "keyword",
			/* LexemContentType::Operator			= */ "operator",
			/* LexemContentType::Comment			= */ "comment",
			/* LexemContentType::ConstantString	    = */ "string constant",
			/* LexemContentType::ConstantCharacter	= */ "Char constant",
			/* LexemContentType::ConstantInteger	= */ "integer constant",
			/* LexemContentType::ConstantFloat		= */ "float constant",
			/* LexemContentType::Unknown			= */ "unknown lexem"
		};

		GD_DEBUG_ASSERT(ContentType < LexemContentType::Unknown, "Invalid ContentType specified");
		return LexemContentTypeStrings[size_t(ContentType)];
	}

GD_NAMESPACE_END
