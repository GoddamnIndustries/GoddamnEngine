//////////////////////////////////////////////////////////////////////////
/// StreamedLexer.cpp - tokenizer implementation
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	typedef Str ParsingErrorDesc;

	//////////////////////////////////////////////////////////////////////////
	///	LexemContentType enum
	//////////////////////////////////////////////////////////////////////////

	extern Str LexemContentTypeToString(LexemContentType const ContentType)
	{
		static Str const LexemContentTypeStrings[] = {
			/* GD_LEXEM_CONTENT_TYPE_IDENTIFGDR			= */ "identifier",
			/* GD_LEXEM_CONTENT_TYPE_KEYWORD			= */ "keyword",
			/* GD_LEXEM_CONTENT_TYPE_OPERATOR			= */ "operator",
			/* GD_LEXEM_CONTENT_TYPE_COMMENT			= */ "comment",
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_STRING	= */ "string constant",
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER	= */ "character constant",
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER	= */ "integer constant",
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT		= */ "float constant",
			/* GD_LEXEM_CONTENT_TYPE_UNKNOWN			= */ "unknown lexem"
		};

		GD_DEBUG_ASSERT(ContentType < GD_LEXEM_CONTENT_TYPE_UNKNOWN, "Invalid ContentType specified");
		return LexemContentTypeStrings[size_t(ContentType)];
	}

	//////////////////////////////////////////////////////////////////////////
	/// StreamedLexerImlp class
	//////////////////////////////////////////////////////////////////////////

	class StreamedLexerImpl final : public IToolchainTool
	{
	private:
		enum StreamedLexerState : UInt8
		{
			GD_LEXER_STATE_READING_CONSTANT_STRING,
			GD_LEXER_STATE_READING_CONSTANT_CHARACTER,
			GD_LEXER_STATE_READING_CONSTANT_INTEGER,
			GD_LEXER_STATE_READING_CONSTANT_FLOAT,
			GD_LEXER_STATE_READING_COMMENT_OR_OPERATOR,
			GD_LEXER_STATE_READING_COMMENT_SINGLELINE,
			GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE,
			GD_LEXER_STATE_READING_IDENTIFGDR_OR_KEYWORD,
			GD_LEXER_STATE_UNKNOWN,
			GD_LEXER_STATES_COUNT = GD_LEXER_STATE_UNKNOWN
		};	// enum StreamedLexerState

		enum StreamedLexerCharType : UInt8
		{
			GD_CHARACTER_TYPE_SPACE,
			GD_CHARACTER_TYPE_DIGIT,
			GD_CHARACTER_TYPE_SPECIAL,
			GD_CHARACTER_TYPE_ALPHABETIC,
			GD_CHARACTER_TYPE_UNKNOWN,
		};	// enum StreamedLexerCharType

		StreamedLexerOptions const& Options;
		Stream*              const  InputStream                 = nullptr;
		size_t&                     CurrentLine                 ;							// Lexem location
		size_t&                     CurrentSymbol               ;
		Lexem*               const  CurrentLexem                = nullptr;						// Lexem state & info
		StreamedLexerState          CurrentState                = GD_LEXER_STATE_UNKNOWN;
		CharAnsi                    CurrentCharacter            = CharAnsi('\0');				// Character meta
		StreamedLexerCharType       CurrentCharacterType        = GD_CHARACTER_TYPE_UNKNOWN;
		static size_t        const  DefaultLexemIntegerNotation = 10;							// Numeric meta
		size_t                      CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		size_t                      CurrentLexemFloatExponent   = 1;
		size_t                      CurrentMatchingIndex        = 0;				 			// Operators and comments meta
		Vector<String const*>       CurrentMatches;
		bool                        WasMatchingTested           = false;

	public:
		GDINT StreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, Stream* const InputStream, Lexem* const OutputLexem,
			size_t& CurrentLine, size_t& CurrentSymbol)
			: IToolchainTool(Toolchain), Options(Options), InputStream(InputStream), CurrentLexem(OutputLexem), CurrentLine(CurrentLine), CurrentSymbol(CurrentSymbol)
		{
		}

		GDINT bool GetNextLexem();

	private /*Internal class API*/:
		GDINT CharAnsi ReadCharacterFromStream();
		
		GDINT void ProcessCharacter();
		GDINT void ProcessInteger();
		GDINT void ProcessFloat();
		GDINT void ProcessCharacterConstant();
		GDINT void ProcessString();
		GDINT void ProcessOperatorOrComment();
		GDINT void ProcessOperator();
		GDINT void ProcessMultipleLineComment();
		GDINT void ProcessSingleLineComment() { if (self->CurrentCharacter == '\n') self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT); }
		GDINT void ProcessIdentifierOrKeyword();
	};	// class StreamedLexerImpl

	/// Reads next character from input stream. If stream ends, that returns '\0'.
	CharAnsi StreamedLexerImpl::ReadCharacterFromStream()
	{
		CharAnsi Character = CharAnsi('\0');
		/**/ if (self->InputStream->Read(&Character, 0, sizeof(Character)) != sizeof(Character))
		{	// Here comes the end of stream.
			return (self->CurrentCharacter = CharAnsi('\0'));
		}
	//	else if ((Character == CharAnsi('\n')) || (Character == CharAnsi('\r')))
	//	{	// Handing line termination characters.
	//		/// @warning Here are parsed both "\r\n" and "\n\r" escapes. If it is not correct that fix it here or remove this warning.
	//		CharAnsi const NextCharacter = self->ReadCharacterFromStream();
	//		if ( ! ((Character == CharAnsi('\n')) && (NextCharacter == CharAnsi('\r'))) 
	//			|| ((Character == CharAnsi('\r')) && (NextCharacter == CharAnsi('\n')))) self->InputStream->Seek(-1, GD_SEEK_ORIGIN_CURRENT);
	//	
	//		// New line here.
	//		self->CurrentSymbol = 0;
	//		self->CurrentLine  += 1;
	//		return (self->CurrentCharacter = CharAnsi('\n'));
	//	}
			
		// Next character on current line.
		self->CurrentSymbol += 1;
		return (self->CurrentCharacter = Character);
	}

	bool StreamedLexerImpl::GetNextLexem()
	{	// Restoring default tokenizer values
		self->CurrentLexem->ResetLexem();
		self->CurrentLexem->Line = self->CurrentLine;
		self->CurrentLexem->Symbol = self->CurrentSymbol;
		while (!self->Toolchain->WasExceptionRaised())
		{	// Now lets handle the type of read character. Here we have a table that describes if we need to check character type switching
			// or ignore switching in several cases. Building a translation table: 
			struct TestCharacterTypesSwitchingTable final { bool DoTestCharacterTypeSwitch; StreamedLexerCharType NotCommitOnSwitchingTo[GD_CHARACTER_TYPE_UNKNOWN + 1]; };
			static TestCharacterTypesSwitchingTable const TestCharacterTypeSwitchingTable[GD_LEXEM_CONTENT_TYPES_COUNT + 1] = {
				/* GD_LEXEM_CONTENT_TYPE_IDENTIFGDR			*/ { true,  { GD_CHARACTER_TYPE_ALPHABETIC, GD_CHARACTER_TYPE_DIGIT, GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_KEYWORD			*/ { true,  { GD_CHARACTER_TYPE_ALPHABETIC, GD_CHARACTER_TYPE_DIGIT, GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_OPERATOR			*/ { true,	{ GD_CHARACTER_TYPE_SPECIAL, GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_COMMENT			*/ { false, { GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_CONSTANT_STRING	*/ { false, { GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER	*/ { false, { GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER	*/ { true,	{ GD_CHARACTER_TYPE_DIGIT, GD_CHARACTER_TYPE_ALPHABETIC, GD_CHARACTER_TYPE_SPECIAL, GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT		*/ { true,	{ GD_CHARACTER_TYPE_DIGIT, GD_CHARACTER_TYPE_UNKNOWN } },
				/* GD_LEXEM_CONTENT_TYPE_UNKNOWN			*/ { true,	{ GD_CHARACTER_TYPE_SPACE, GD_CHARACTER_TYPE_UNKNOWN } }
			};

			// Reading next character from stream and determining the type of new character and possible type switch.
			StreamedLexerCharType const PreviousCharacterType = self->CurrentCharacterType;
			self->ReadCharacterFromStream();
			self->CurrentCharacterType	// Searching in custom special symbols list and treating all others as valid identifier characters
				= (self->Options.IsInSpecialCharactersAlphabet(self->CurrentCharacter))	                                                 ? GD_CHARACTER_TYPE_SPECIAL 
				: (CharAnsiHelpers::IsAlphabetic(self->CurrentCharacter) || CharAnsiHelpers::IsSpecialCharacter(self->CurrentCharacter)) ? GD_CHARACTER_TYPE_ALPHABETIC
				: (CharAnsiHelpers::IsDigit     (self->CurrentCharacter, self->CurrentLexemIntegerNotation))                             ? GD_CHARACTER_TYPE_DIGIT
				: (CharAnsiHelpers::IsSpace     (self->CurrentCharacter))                                                                ? GD_CHARACTER_TYPE_SPACE : GD_CHARACTER_TYPE_UNKNOWN; 
			/**/ if ((self->CurrentCharacterType == GD_CHARACTER_TYPE_SPACE) && (PreviousCharacterType == GD_CHARACTER_TYPE_UNKNOWN))
			{	// Upcoming space that splits lexems. Just skipping it.
				self->CurrentCharacterType = GD_CHARACTER_TYPE_UNKNOWN;
				continue;
			}
			else if ((self->CurrentCharacterType == GD_CHARACTER_TYPE_UNKNOWN) && (self->CurrentCharacter != CharAnsi('\0')))
			{	// Unknown character exists in stream (except \0 character). Error:
				static ParsingErrorDesc const InvalidCharacterParsingError("invalid character with code 0x%x was read from input stream.");
				self->RaiseFatalError(InvalidCharacterParsingError, size_t(self->CurrentCharacter));
				self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_SYNTAX);
			}

			TestCharacterTypesSwitchingTable const& CurrentTypeSwitchingTable = TestCharacterTypeSwitchingTable[size_t(self->CurrentLexem->ContentType)];
			if (CurrentTypeSwitchingTable.DoTestCharacterTypeSwitch && (PreviousCharacterType != self->CurrentCharacterType) && (PreviousCharacterType != GD_CHARACTER_TYPE_UNKNOWN))
			{	// Parsing all conditions where token committing is required.
				// Attempting to switch state. Skipping if previous type is unknown - tokenizer was launched for the first time 
				StreamedLexerCharType const* NotSwitchOnTypePtr = &CurrentTypeSwitchingTable.NotCommitOnSwitchingTo[0];
				for (; (*NotSwitchOnTypePtr) != GD_CHARACTER_TYPE_UNKNOWN; NotSwitchOnTypePtr += 1)
				{	// Testing if current token is required to commit
					if ((*NotSwitchOnTypePtr) == self->CurrentCharacterType)
					{	// Ignoring the commit and continuing processing
						break;
					}
				}

				if ((*NotSwitchOnTypePtr) == GD_CHARACTER_TYPE_UNKNOWN)
				{	// Rewinding character that caused character type switching and committing current character
					self->InputStream->Seek(-1, GD_SEEK_ORIGIN_CURRENT);
					self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
					continue;
				}
			}

			// Now all commit conditions checked, no commit required, processing current character 
			self->ProcessCharacter();
		}

		ToolchainException const Exception = self->Toolchain->CatchException();
		if ((!IsMyToolchainException(Exception, GD_LEXER_EXCEPTION_MODULE)) || (Exception > 0))
		{	// This is fatal exception or it is not thrown by us.
			self->RaiseExceptionWithCode(Exception);
			return false;
		}

		/// We should return false on End-Of-Stream.
		if (Exception == GD_LEXER_EXCEPTION_EOS) self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_EOS;
		return true;
	}

	void StreamedLexerImpl::ProcessCharacter()
	{
		struct LexemStatesDesc final { bool RequiresClosingStatement; void (StreamedLexerImpl::* SpecialProcessorPtr)(); };
		static LexemStatesDesc const LexemStatesDescTable[GD_LEXER_STATES_COUNT] = {
			/* GD_LEXER_STATE_READING_CONSTANT_STRING	    */ {  true, &StreamedLexerImpl::ProcessString },
			/* GD_LEXER_STATE_READING_CONSTANT_CHARACTER    */ {  true, &StreamedLexerImpl::ProcessCharacter },
			/* GD_LEXER_STATE_READING_CONSTANT_INTEGER	    */ { false, &StreamedLexerImpl::ProcessInteger },
			/* GD_LEXER_STATE_READING_CONSTANT_FLOAT	    */ { false, &StreamedLexerImpl::ProcessFloat },
			/* GD_LEXER_STATE_READING_COMMENT_OR_OPERATOR   */ { false, &StreamedLexerImpl::ProcessOperatorOrComment },
			/* GD_LEXER_STATE_READING_COMMENT_SINGLELINE    */ { false, &StreamedLexerImpl::ProcessSingleLineComment },
			/* GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE  */ {  true, &StreamedLexerImpl::ProcessMultipleLineComment },
			/* GD_LEXER_STATE_READING_IDENTIFGDR_OR_KEYWORD */ { false, &StreamedLexerImpl::ProcessIdentifierOrKeyword },
		};

		/**/ if ((self->CurrentCharacter != CharAnsi('\0')) && (self->CurrentState == GD_LEXER_STATE_UNKNOWN))
		{	// Attempting to detect new state after previous commit 
			switch (self->CurrentCharacter)
			{	// Detecting opening quote types:
			case CharAnsi('\"'): 
				self->CurrentState = GD_LEXER_STATE_READING_CONSTANT_STRING;
				self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_STRING;
				self->CurrentLexem->RawData += CharAnsi('\"');
				return;
			case CharAnsi('\''): 
				self->CurrentState = GD_LEXER_STATE_READING_CONSTANT_CHARACTER; 
				self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER;
				self->CurrentLexem->RawData += CharAnsi('\'');
				return;
			default: switch (self->CurrentCharacterType)
				{	// Parsing other character types:
				case GD_CHARACTER_TYPE_SPACE:																  
					return;
				case GD_CHARACTER_TYPE_DIGIT: 
					self->CurrentState = GD_LEXER_STATE_READING_CONSTANT_INTEGER;
					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER;
					break;
				case GD_CHARACTER_TYPE_ALPHABETIC: 
					self->CurrentState = GD_LEXER_STATE_READING_IDENTIFGDR_OR_KEYWORD;
					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_IDENTIFGDR;
					break;
				case GD_CHARACTER_TYPE_SPECIAL: 
					self->CurrentState = GD_LEXER_STATE_READING_COMMENT_OR_OPERATOR;
					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_OPERATOR;
					break;
				default: 
					GD_DEBUG_ASSERT_FALSE("Internal error: invalid CurrentCharacterType");
					break;
				} break;
			}
		}
		else if (self->CurrentCharacter == CharAnsi('\0'))
		{	// End of stream reached.
			if (LexemStatesDescTable[size_t(self->CurrentState)].RequiresClosingStatement)
			{	// End of stream reached, but we were parsing token that requires terminator. Error:
				static ParsingErrorDesc const UnexpectedEndOfStream("unexpected end of stream while parsing %s.");
				self->RaiseFatalError(UnexpectedEndOfStream, LexemContentTypeToString(self->CurrentLexem->ContentType));
				self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_SYNTAX);
				return;
			}

			self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_EOS);
			return;
		}
		
		// Parsing current character
		// self->CurrentState cannot be greater equal to unknown state
		GD_WARNING_SUPPRESS(6385);
		(self->*(LexemStatesDescTable[size_t(self->CurrentState)].SpecialProcessorPtr))();
		 self->CurrentLexem->RawData += self->CurrentCharacter;
	}

	void StreamedLexerImpl::ProcessInteger()
	{
		switch (self->CurrentCharacterType)
		{
		case GD_CHARACTER_TYPE_DIGIT:
			{	// Processing characters: may just append it or switch into octal notation.
				if ((self->CurrentLexemIntegerNotation == StreamedLexerImpl::DefaultLexemIntegerNotation) && (self->CurrentLexem->ProcessedDataInteger == 0) && (self->CurrentCharacter != CharAnsi('0')))
				{	// Force switching into octal notation like in C++-like languages syntax. E.g. 074 = 60
					self->CurrentLexemIntegerNotation = 8;
				}
				
				// Processing character if it is digit in current notation
				UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
				self->CurrentLexem->ProcessedDataInteger *= self->CurrentLexemIntegerNotation;
				self->CurrentLexem->ProcessedDataInteger += CurrentDigit;
			}	break;
		case GD_CHARACTER_TYPE_ALPHABETIC:
			{	// OR Switching notation:
				bool WasNotationSwitched = ((self->CurrentLexem->ProcessedDataInteger == 0) && (self->CurrentLexemIntegerNotation == StreamedLexerImpl::DefaultLexemIntegerNotation));
				if  (WasNotationSwitched)
				{	// Checking if notation switching is provided in format "0x...":
					CharAnsi const UpperCaseCharacter = CharAnsiHelpers::ToUpperCase(self->CurrentCharacter);
					/**/ if (UpperCaseCharacter == Options.IntegerHexadecimalNotationDelimiter)	self->CurrentLexemIntegerNotation = 16;
					else if (UpperCaseCharacter == Options.IntegerOctalNotationDelimiter)		self->CurrentLexemIntegerNotation = 8;
					else if (UpperCaseCharacter == Options.IntegerBinaryNotationDelimiter)		self->CurrentLexemIntegerNotation = 2;
					else WasNotationSwitched = false;
				}
			
				if (!WasNotationSwitched)
				{	// Found, that prefix does not matches template or it does not follows '0' character
					self->InputStream->Seek(-1, GD_SEEK_ORIGIN_CURRENT);
					self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
				}
			}	break;
		case GD_CHARACTER_TYPE_SPECIAL:
			{	// OR Switching to float parsing:
				if (self->CurrentCharacter == self->Options.FloatingPointDelimiter)
				{	// Floating point delimeter found.
					self->CurrentState = GD_LEXER_STATE_READING_CONSTANT_FLOAT;
					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT;
					self->CurrentLexem->ProcessedDataFloat = static_cast<Float64>(self->CurrentLexem->ProcessedDataInteger);
				}
				else
				{	// Some other special character upcoming, commiting token.
					self->InputStream->Seek(-1, GD_SEEK_ORIGIN_CURRENT);
					self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
				}
			}	break;
		default: // OR Throwing debug Assertation if internal error exists
			GD_DEBUG_ASSERT_FALSE("Some strange things happened, unknown condition for ProcessInteger");
			break;
		}
	}

	void StreamedLexerImpl::ProcessFloat()
	{	
		UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
		self->CurrentLexem->ProcessedDataFloat += (static_cast<Float64>(CurrentDigit) / static_cast<Float64>(self->CurrentLexemFloatExponent));
		self->CurrentLexemFloatExponent += 1;
	}

	void StreamedLexerImpl::ProcessString()
	{
		GD_NOT_IMPLEMENTED();
	}

	void StreamedLexerImpl::ProcessCharacterConstant()
	{
		GD_NOT_IMPLEMENTED();
	}

	void StreamedLexerImpl::ProcessOperatorOrComment()
	{
		if (self->CurrentMatches.GetSize() == 0)
		{	// Filling CurrentMatches with pointers on operators and comments declarations
			self->CurrentMatchingIndex = 0;
			self->CurrentMatches.Reserve ( self->Options.OperatorDeclarations.GetSize() + 2);
			self->CurrentMatches.PushLast(&self->Options.SingleLineCommentDeclaration);
			self->CurrentMatches.PushLast(&self->Options.MultipleLineCommentBeginning);
			for (auto const& OperatorDeclaration : self->Options.OperatorDeclarations)
				self->CurrentMatches.PushLast(&OperatorDeclaration.Second);
		}

		Vector<String const*> CurrentMatches(0, nullptr, self->CurrentMatches.GetSize());
		for (auto const CurrentOperatorOrCommentMatch : self->CurrentMatches)
		{	
			if ((*CurrentOperatorOrCommentMatch)[self->CurrentMatchingIndex] == self->CurrentCharacter)
			{	// Character matches some declaration at current index
				CurrentMatches.PushLast(CurrentOperatorOrCommentMatch);
				if ((*CurrentOperatorOrCommentMatch).GetSize() == (1 + self->CurrentMatchingIndex))
				{	// We are having a full match with some operator, so lets identify it now
					/**/ if (CurrentMatches[0] == &self->Options.SingleLineCommentDeclaration)
					{	// Declaration matches single line comment declaration.
						self->CurrentState = GD_LEXER_STATE_READING_COMMENT_SINGLELINE;
						self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
						self->CurrentMatchingIndex = 0;
					}
					else if (CurrentMatches[0] == &self->Options.MultipleLineCommentBeginning)
					{	// Declaration matches multiple line comment declaration.
						self->CurrentState = GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE;
						self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
						self->CurrentMatchingIndex = 0;
					}
					else
					{	// Declaration matches operator declaration.
						size_t const Index = reinterpret_cast<StreamedLexerOperatorDecl const*>(CurrentMatches[0]) - &self->Options.OperatorDeclarations[0];
						self->CurrentLexem->ProcessedDataId = self->Options.OperatorDeclarations[Index].First;
					}
				}
			}
		}

		/**/ if (CurrentMatches.GetSize() == 1)
		{	// Seams we have a match here. 
			if ((CurrentMatches[0]->GetSize() == (self->CurrentMatchingIndex + 1)) && (self->CurrentLexem->GetContentType() == GD_LEXEM_CONTENT_TYPE_OPERATOR))
			{	// Lets check if this is a full match and skip to full if not.
				self->CurrentMatchingIndex = 0;
				self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
				return;
			}
		}
		else if (CurrentMatches.GetSize() == 0)
		{	// Just throwing an error for now. For C++-like languages this branch of code is never executed
			// because they have a ladder of of operators: <, <<, <<=. There are no multiple characters operators
			// that do not have matching operator that is equal to this without last character.
			static ParsingErrorDesc const UnexpectedSpecialCharacter("unexpected special character while parsing operator");
			self->RaiseFatalError(UnexpectedSpecialCharacter);
			self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_SYNTAX);
		}
		
		self->CurrentMatches = Move(CurrentMatches);
		self->CurrentMatchingIndex += 1;
	}

	void StreamedLexerImpl::ProcessIdentifierOrKeyword()
	{
		if ((self->CurrentMatches.GetSize() == 0) && (!self->WasMatchingTested))
		{	// Filling CurrentMatches with pointers on operators and comments declarations
			self->WasMatchingTested = true;
			self->CurrentMatchingIndex = 0;
			self->CurrentMatches.Reserve(self->Options.KeywordsDeclarations.GetSize());
			for (auto const& KeywordsDeclarations : self->Options.KeywordsDeclarations)
				self->CurrentMatches.PushLast(&KeywordsDeclarations.Second);
		}

		Vector<String const*> CurrentMatches(0, nullptr, self->CurrentMatches.GetSize());
		for (auto const CurrentOperatorOrCommentMatch : self->CurrentMatches)
		{	/// @todo Rewrite this code using CopyIf
			if ((*CurrentOperatorOrCommentMatch)[self->CurrentMatchingIndex] == self->CurrentCharacter)
			{	// Character matches some declaration at current index
				CurrentMatches.PushLast(CurrentOperatorOrCommentMatch);
				if ((*CurrentOperatorOrCommentMatch).GetSize() == (1 + self->CurrentMatchingIndex))
				{	// We are having a full match with some operator, so lets identify it now
					size_t const Index = reinterpret_cast<StreamedLexerKeywordDecl const*>(CurrentMatches[0]) - &self->Options.KeywordsDeclarations[0];
					self->CurrentLexem->ProcessedDataId = self->Options.KeywordsDeclarations[Index].First;
					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_KEYWORD;
				}
			}
		}

		if (CurrentMatches.GetSize() == 0)
		{	// We are heving next chracter that makes this lexem valid identifier.
			// Restoring identifer content type.
			self->CurrentLexem->ProcessedDataId = 0;
			self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_IDENTIFGDR;
		}

		self->CurrentMatches = Move(CurrentMatches);
		self->CurrentMatchingIndex += 1;
	}

	void StreamedLexerImpl::ProcessMultipleLineComment()
	{
		if (self->Options.MultipleLineCommentEnding[self->CurrentMatchingIndex] == self->CurrentCharacter)
		{	// Matching with multiple line comment ending.
			self->CurrentMatchingIndex += 1;
			if (self->Options.MultipleLineCommentEnding.GetSize() == self->CurrentMatchingIndex)
			{	// Final match, finalizing comment.
				self->CurrentMatchingIndex = 0;
				self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// StreamedLexer class
	//////////////////////////////////////////////////////////////////////////

	bool StreamedLexer::GetNextLexem(Stream* const InputStream, Lexem* const OutputLexem)
	{	// We are using implementation class to prevent dozens of private fields in class.
		return StreamedLexerImpl(self->Toolchain, Options, InputStream, OutputLexem, self->CurrentLine, self->CurrentSymbol).GetNextLexem();
	}

	//////////////////////////////////////////////////////////////////////////
	/// StreamedLexerOptions class
	//////////////////////////////////////////////////////////////////////////

	StreamedLexerOptions::StreamedLexerOptions(
		_In_ StreamedLexerKeywordsList  && KeywordsDeclarations,
		_In_ StreamedLexerOperatorsList && OperatorDeclarations,
		_In_ String	                    && SingleLineCommentDeclaration,
		_In_ String	                    && MultipleLineCommentBeginning,
		_In_ String	                    && MultipleLineCommentEnding,
		_In_ CharAnsi const                IntegerHexadecimalNotationDelimiter,
		_In_ CharAnsi const                IntegerOctalNotationDelimiter,
		_In_ CharAnsi const                IntegerBinaryNotationDelimiter,
		_In_ CharAnsi const                FloatingPointDelimiter
	) : KeywordsDeclarations(Forward<StreamedLexerKeywordsList>(KeywordsDeclarations)),
		OperatorDeclarations(Forward<StreamedLexerOperatorsList>(OperatorDeclarations)),
		SingleLineCommentDeclaration(Forward<String>(SingleLineCommentDeclaration)),
		MultipleLineCommentBeginning(Forward<String>(MultipleLineCommentBeginning)),
		MultipleLineCommentEnding(Forward<String>(MultipleLineCommentEnding)),
		IntegerHexadecimalNotationDelimiter(CharAnsiHelpers::ToUpperCase(IntegerHexadecimalNotationDelimiter)),
		IntegerOctalNotationDelimiter(CharAnsiHelpers::ToUpperCase(IntegerOctalNotationDelimiter)),
		IntegerBinaryNotationDelimiter(CharAnsiHelpers::ToUpperCase(IntegerBinaryNotationDelimiter)),
		FloatingPointDelimiter(FloatingPointDelimiter)
	{
		// Checking if all delimiters are alphabetic characters.
		/// @todo Add check for all notation switching characters have unique or \0 values.
		GD_DEBUG_ASSERT(((self->IntegerHexadecimalNotationDelimiter == '\0') || CharAnsiHelpers::IsAlphabetic(self->IntegerHexadecimalNotationDelimiter)),
			"'IntegerHexadecimalNotationDelimiter' should be alphabetic character. Recommended value is 'x'.");
		GD_DEBUG_ASSERT(((self->IntegerOctalNotationDelimiter == '\0') || CharAnsiHelpers::IsAlphabetic(self->IntegerOctalNotationDelimiter)),
			"'IntegerOctalNotationDelimiter' should be alphabetic character. Recommended value is 'c'.");
		GD_DEBUG_ASSERT(((self->IntegerBinaryNotationDelimiter == '\0') || CharAnsiHelpers::IsAlphabetic(self->IntegerBinaryNotationDelimiter)),
			"'IntegerBinaryNotationDelimiter' should be alphabetic character. Recommended value is 'b'.");

		// Validating value of floating pointer delimiter.
		// We should not forget to append floating point delimiter either it would be treated as alphabetic character.
		GD_DEBUG_ASSERT(((self->FloatingPointDelimiter == '\0') || CharAnsiHelpers::IsSpecialCharacter(self->FloatingPointDelimiter)),
			"FloatingPointDelimiter should be special character. Recommended values are '.' or ','.");
		self->AppendToSpecialCharactersAlphabet(self->FloatingPointDelimiter);

		// Checking if all operators/comment declarators have unique values and extending alphabet.
		GD_DEBUG_ASSERT((self->SingleLineCommentDeclaration != self->MultipleLineCommentBeginning), "Multiple line comment declaration should not be equal to single line comment declaration");
		for (auto const Character : self->SingleLineCommentDeclaration)
		{	// Checking if single line comment contains only special characters and extending alphabet.
			GD_DEBUG_ASSERT((CharAnsiHelpers::IsSpecialCharacter(Character)), "Single line comment string should contain only special characters");
			self->AppendToSpecialCharactersAlphabet(Character);
		}
		for (auto const Character : self->MultipleLineCommentBeginning)
		{	// Checking if multiple line comment contains only special characters and extending alphabet.
			GD_DEBUG_ASSERT((CharAnsiHelpers::IsSpecialCharacter(Character)), "Multiple line comment string should contain only special characters");
			self->AppendToSpecialCharactersAlphabet(Character);
		}

		// Extending alphabet with characters from operator declarations.
		for (auto const& OperatorDeclaration : self->OperatorDeclarations)
		{	// Checking if none comment declarations is same to operator declaration.
			/// @todo Add check for all operators have unique values.
			String const& OperatorDeclarationString = OperatorDeclaration.Second;
			GD_DEBUG_ASSERT((self->SingleLineCommentDeclaration != OperatorDeclarationString), "Operator declaration should not be equal to single line comment declaration");
			GD_DEBUG_ASSERT((self->MultipleLineCommentBeginning != OperatorDeclarationString), "Operator declaration should not be equal to multiple line comment declaration");
			for (auto const Character : OperatorDeclarationString)
			{	// Checking if operator contains only special characters
				GD_DEBUG_ASSERT((CharAnsiHelpers::IsSpecialCharacter(Character)), "Operator string should contain only special characters");
				self->AppendToSpecialCharactersAlphabet(Character);
			}
		}
	}

	StreamedLexerOptions const& StreamedLexerDefaultOptions::GetDefaultOptionsForCpp()
	{
#if (!defined(__INTELLISENSE__))
		static StreamedLexerOptions const CppLexerOptions(
			StreamedLexerKeywordsList({
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ASM,              "asm" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_AUTO,             "auto" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_BOOL,             "bool" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_BREAK,            "break" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CASE,             "case" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CATCH,            "catch" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CHAR,             "char" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CLASS,            "class" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST,            "const" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONSTEXPR,        "constexpr" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST_CAST,       "const_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONTINUE,         "continue" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DECLTYPE,         "decltype" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DEFAULT,          "default" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DELETE,           "delete" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DO,               "do" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DOUBLE,           "double" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DYNAMIC_CAST,     "dynamic_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ELSE,             "else" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ENUM,             "enum" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXPLICIT,         "explicit" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXPORT,           "export" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXTERN,           "extern" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FALSE,            "false" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FOR,              "for" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FRGDND,           "friend" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_GOTO,             "goto" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_IF,               "if" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_INLINE,           "inline" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_INT,              "int" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_LONG,             "long" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_MUTABLE,          "mutable" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NAMESPACE,        "namespace" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NEW,              "new" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NOEXPECT,         "noexpect" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NULLPTR,          "nullptr" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_OPERATOR,         "operator" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PRIVATE,          "private" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PROTECTED,        "protected" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PUBLIC,           "public" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_REGISTER,         "register" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_REINTERPRET_CAST, "reinterpet_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_RETURN,           "return" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SHORT,            "short" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIGNED,           "signed" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIZEOF,           "sizeof" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC,           "static" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC_ASSERT,    "static_assert" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC_CAST,      "static_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STRUCT,           "struct" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SWITCH,           "switch" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TEMPLATE,         "template" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THIS,             "this" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THREAD_LOCAL,     "thread_local" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THROW,            "throw" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TRUE,             "true" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TRY,              "try" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPEDEF,          "typedef" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPEID,           "typeid" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPENAME,         "typename" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNION,            "union" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNSIGNED,         "unsigned" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_USING,            "using" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VIRTUAL,          "virtual" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOID,             "void" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOLATILE,         "volatile" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_WHILE,            "while" },
			}),
			StreamedLexerOperatorsList({
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN,                    "="   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ADD,                       "+"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SUB,                       "-"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL,                       "*"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DIV,                       "/"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MOD,                       "%"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INC,                       "++"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DEC,                       "--"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_EQUALS,                    "=="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_NOT_EQUALS,                "!="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER,                   "<"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS,                      ">"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER_EQUAL,             "<="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS_EQUALS,               ">="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_NOT,                       "!"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_AND,                       "&&"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_OR,                        "||"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_NOT,               "~"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND,               "&"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR,                "|"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_XOR,               "^"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_LEFT_SHIFT,        "<<"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT,       ">>"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ADD_ASSIGN,                "+="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SUB_ASSIGN,                "-="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL_ASSIGN,                "*="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DIV_ASSIGN,                "/="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MOD_ASSIGN,                "%="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND_ASSIGN,        "&="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR_ASSIGN,         "|="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_XOR_ASSIGN,        "^="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_LEFT_SHIFT_ASSIGN, "<<=" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT_ASSIGN,">>=" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_BEGIN,               "["   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_END,                 "]"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN,               "{"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END,                 "}"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN,              "("   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END,                ")"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_STRUCT_DEREFERENCING,      "->"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_STRUCT_REFERENCING,        "."   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MEMBER_PTR_DEREFERENCING,  "->*" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MEMBER_PTR_REFERENCING,    ".*"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA,                     ","   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON,                     ":"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SEMICOLON,                 ";"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TERNARY,                   "?"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_RESOLUTION,          "::"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAM_PACK,                "..." },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR,              "#"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR_GLUE,         "##"  },
			}),
			"//", "/*", "*/", 
			'x', '\0', 'b', '.'
		);

		return CppLexerOptions;
#endif	// if (!defined(__INTELLISENSE__))
	}

	//StreamedLexerOptions const& StreamedLexerDefaultOptions::GetDefaultOptionsForXml()
	//{
	//	static StreamedLexerOptions const XmlLexerOptions({
	//		"<", ">", "</", "/>", "<?", "?>",									// Nodes and header
	//		"=", "&", ";"														// Attributes and special characters parsing
	//	}, "", "<!--", "-->", 'x', '\0', '\0', '.');

	//	return XmlLexerOptions;
	//}

GD_NAMESPACE_END
