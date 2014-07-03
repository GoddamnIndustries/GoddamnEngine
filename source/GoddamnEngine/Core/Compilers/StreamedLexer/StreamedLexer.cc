//////////////////////////////////////////////////////////////////////////
/// StreamedLexer.cc - tokenizer implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>
#include <GoddamnEngine/Core/Utility.hh>

GD_NAMESPACE_BEGIN

	typedef Str ParsingErrorDesc;

	/// Implementation of a basic streaming lexer. 
	class BasicStreamedLexerImpl : public IToolchainTool
	{
	protected /*Class Types and members*/:
		enum StreamedLexerState : UInt8
		{
			GD_LEXER_STATE_READING_CONSTANT_STRING,
			GD_LEXER_STATE_READING_CONSTANT_CHARACTER,
			GD_LEXER_STATE_READING_CONSTANT_INTEGER,
			GD_LEXER_STATE_READING_CONSTANT_FLOAT,
			GD_LEXER_STATE_READING_COMMENT_OR_OPERATOR,
			GD_LEXER_STATE_READING_COMMENT_SINGLELINE,
			GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE,
			GD_LEXER_STATE_READING_IDENTIFIER_OR_KEYWORD,
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

		Lexem*                      CurrentLexem                = nullptr;						// Lexem state & info
		size_t                      CurrentLine                 = 1;					    	// Lexem location
		size_t                      CurrentSymbol               = 0;
		CharAnsi                    RewindedCharacter			= CharAnsi('\0');	

		StreamedLexerState          CurrentState                = GD_LEXER_STATE_UNKNOWN;
		CharAnsi                    CurrentCharacter            = CharAnsi('\0');				// Character meta
		StreamedLexerCharType       CurrentCharacterType        = GD_CHARACTER_TYPE_UNKNOWN;
		size_t static const         DefaultLexemIntegerNotation = 10;							// Numeric meta
		size_t                      CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		size_t                      CurrentMatchingIndex        = 0;				 			// Operators and comments meta

	public:
		GDINL BasicStreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, Stream* const InputStream)
			: IToolchainTool(Toolchain)
			, Options(Options)
			, InputStream(InputStream)
		{
		}

		GDINL BasicStreamedLexerImpl(BasicStreamedLexerImpl const& Other)
			: IToolchainTool(Other.Toolchain)
			, Options(Other.Options)
			, InputStream(Other.InputStream)
			, CurrentLine(Other.CurrentLine)
			, CurrentSymbol(Other.CurrentSymbol)
			, RewindedCharacter(Other.RewindedCharacter)
		{
		}

		GDINL virtual ~BasicStreamedLexerImpl()
		{
		}

		/// Reads next lexem from Input stream.
		/// @returns True if lexem was extracted succesfully.
		GDINT bool GetNextLexem(Lexem* const OutputLexem);

	private /*Internal class API*/:
		/// Processes single character from input stream.
		GDINT void ProcessUpcomingCharacter();

		/// Reads next character from input stream. If stream ends, that returns '\0'. Also handles '\n' and '\r' sequences.
		GDINT void ReadCharacterFromStream();

	protected /*Internal class API*/:
		/// Resets lexer implementation.
		GDINT virtual void Reset();

		/// Processes integer constants inside input stream.
		GDINT virtual void ProcessInteger();

		/// Processes floating-point constants inside input stream.
		GDINT virtual void ProcessFloat();

		/// Processes string constants inside input stream.
		GDINT virtual void ProcessString();

		/// Processes character constants inside input stream.
		GDINT virtual void ProcessCharacter();

		/// Processes operators inside input stream and handles switching to multiple line comment parsing.
		GDINT virtual void ProcessOperatorOrComment();

		/// Processes identifiers and keywords inside input stream.
		GDINT virtual void ProcessIdentifierOrKeyword();

		/// Processes single line comments inside input stream.
		GDINL virtual void ProcessSingleLineComment();

		/// Processes multiple line comments inside input stream.
		GDINT virtual void ProcessMultipleLineComment();
	};	// class BasicStreamedLexerImpl

	/// Implementation of a full-featured streaming lexer. 
	class StreamedLexerImpl final : public BasicStreamedLexerImpl
	{
	private /*Class members*/:
		size_t                      CurrentLexemFloatExponent   = 1;
		Vector<String const*>       CurrentMatches              ;

	public /*CLass API*/:
		GDINL StreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, Stream* const InputStream)
			: BasicStreamedLexerImpl(Toolchain, Options, InputStream)
		{
		}

		GDINL StreamedLexerImpl(BasicStreamedLexerImpl const& Other)
			: BasicStreamedLexerImpl(Other)
		{
		}

		GDINL virtual ~StreamedLexerImpl()
		{
		}

	private /*Internal class API*/:
		/// Resets lexer implementation.
		GDINT virtual void Reset() final override;

		/// Processes integer constants inside input stream.
		GDINT virtual void ProcessInteger() final override;
		
		/// Processes floating-point constants inside input stream.
		GDINT virtual void ProcessFloat() final override;
		
		/// Processes operators inside input stream and handles switching to multiple line comment parsing.
		GDINT virtual void ProcessOperatorOrComment() final override;
		
		/// Processes identifiers and keywords inside input stream.
		GDINT virtual void ProcessIdentifierOrKeyword() final override;
	};	// class StreamedLexerImpl

	/// ==========================================================================================
	/// BasicStreamedLexerImpl class.
	/// Implementation of a basic streaming lexer. 
	/// ==========================================================================================

	/// Reads next lexem from Input stream.
	/// @returns True if lexem was extracted succesfully.
	bool BasicStreamedLexerImpl::GetNextLexem(Lexem* const OutputLexem)
	{	// Restoring default tokenizer values
		self->Reset();
		self->CurrentLexem = OutputLexem;
		self->CurrentLexem->ResetLexem();
		while (!self->Toolchain->WasExceptionRaised())
		{	// Now all commit conditions checked, no commit required, processing current character.
			self->ProcessUpcomingCharacter();
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

	/// ------------------------------------------------------------------------------------------
	/// Internal class API:
	/// ------------------------------------------------------------------------------------------

	/// Resets lexer implementation.
	void BasicStreamedLexerImpl::Reset()
	{
		self->CurrentState                = GD_LEXER_STATE_UNKNOWN;
		self->CurrentCharacter            = CharAnsi('\0');				
		self->CurrentCharacterType        = GD_CHARACTER_TYPE_UNKNOWN;
		self->CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		self->CurrentMatchingIndex        = 0;			
	}

	/// Reads next character from input stream. If stream ends, that returns '\0'. Also handles '\n' and '\r' sequences.
	void BasicStreamedLexerImpl::ReadCharacterFromStream()
	{	// Testing if we have rewinded character.
		if (self->RewindedCharacter == CharAnsi('\0'))
		{	// Trying to read next character:
			/**/ if (self->InputStream->Read(&self->CurrentCharacter, 0, sizeof(self->CurrentCharacter)) != sizeof(self->CurrentCharacter))
			{	// Here comes the end of stream.
				self->CurrentCharacter = CharAnsi('\0');
			}
			else if (self->CurrentCharacter == CharAnsi('\r'))
			{	// Trying to handle '\r\n' sequence.
				CharAnsi NextCharacter = CharAnsi('\0');
				if (self->InputStream->Read(&NextCharacter, 0, sizeof(NextCharacter)) == sizeof(NextCharacter))
				{	// Next character exists. Checking sequence:
					if (NextCharacter != CharAnsi('\n'))
						self->RewindedCharacter = NextCharacter;
				}

				// New line here.
				self->CurrentCharacter = CharAnsi('\n');
				self->CurrentSymbol = 0;
				self->CurrentLine += 1;
			}
			else if (self->CurrentCharacter == CharAnsi('\n'))
			{	// Handing line termination characters.
				self->CurrentSymbol = 0;
				self->CurrentLine += 1;
			}
			else
			{	// Next character on current line.
				self->CurrentSymbol += 1;
			}
		}
		else
		{
			self->CurrentCharacter = self->RewindedCharacter;
			self->RewindedCharacter = CharAnsi('\0');
		}
	}

	/// Processes single character from input stream.
	void BasicStreamedLexerImpl::ProcessUpcomingCharacter()
	{	// Now lets handle the type of read character. Here we have a table that describes if we need to check character type switching
		// or ignore switching in several cases. Building a translation table: 
		struct TestCharacterTypesSwitchingTable final { bool DoTestCharacterTypeSwitch; StreamedLexerCharType NotCommitOnSwitchingTo[GD_CHARACTER_TYPE_UNKNOWN + 1]; };
		static TestCharacterTypesSwitchingTable const TestCharacterTypeSwitchingTable[GD_LEXEM_CONTENT_TYPES_COUNT + 1] = {
			/* GD_LEXEM_CONTENT_TYPE_IDENTIFIER			*/ { true,  { GD_CHARACTER_TYPE_ALPHABETIC, GD_CHARACTER_TYPE_DIGIT, GD_CHARACTER_TYPE_UNKNOWN } },
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
			return;
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
				self->RewindedCharacter = self->CurrentCharacter;
				self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
				return;
			}
		}

		struct LexemStatesDesc final { bool RequiresClosingStatement; void (BasicStreamedLexerImpl::* SpecialProcessorPtr)(); };
		static LexemStatesDesc const LexemStatesDescTable[GD_LEXER_STATES_COUNT] = {
			/* GD_LEXER_STATE_READING_CONSTANT_STRING	    */ {  true, &BasicStreamedLexerImpl::ProcessString },
			/* GD_LEXER_STATE_READING_CONSTANT_CHARACTER    */ {  true, &BasicStreamedLexerImpl::ProcessCharacter },
			/* GD_LEXER_STATE_READING_CONSTANT_INTEGER	    */ { false, &BasicStreamedLexerImpl::ProcessInteger },
			/* GD_LEXER_STATE_READING_CONSTANT_FLOAT	    */ { false, &BasicStreamedLexerImpl::ProcessFloat },
			/* GD_LEXER_STATE_READING_COMMENT_OR_OPERATOR   */ { false, &BasicStreamedLexerImpl::ProcessOperatorOrComment },
			/* GD_LEXER_STATE_READING_COMMENT_SINGLELINE    */ { false, &BasicStreamedLexerImpl::ProcessSingleLineComment },
			/* GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE  */ {  true, &BasicStreamedLexerImpl::ProcessMultipleLineComment },
			/* GD_LEXER_STATE_READING_IDENTIFIER_OR_KEYWORD */ { false, &BasicStreamedLexerImpl::ProcessIdentifierOrKeyword },
		};

		/**/ if ((self->CurrentCharacter != CharAnsi('\0')) && (self->CurrentState == GD_LEXER_STATE_UNKNOWN))
		{	// Here is first real character of lexem.
			self->CurrentLexem->Line = self->CurrentLine;
			self->CurrentLexem->Symbol = self->CurrentSymbol;

			// Attempting to detect new state after previous commit 
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
					self->CurrentState = GD_LEXER_STATE_READING_IDENTIFIER_OR_KEYWORD;
					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_IDENTIFIER;
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
		if (self->RewindedCharacter == CharAnsi('\0'))
			self->CurrentLexem->RawData += self->CurrentCharacter;
	}

	/// Processes integer constants inside input stream.
	void BasicStreamedLexerImpl::ProcessInteger()
	{
		switch (self->CurrentCharacterType)
		{
		case GD_CHARACTER_TYPE_DIGIT:
		{	// Nothing to do here.
		}	break;
		case GD_CHARACTER_TYPE_ALPHABETIC:
		{	// Switching notation:
			bool WasNotationSwitched = ((self->CurrentLexem->ProcessedDataInteger == 0) && (self->CurrentLexemIntegerNotation == StreamedLexerImpl::DefaultLexemIntegerNotation));
			if (WasNotationSwitched)
			{	// Checking if notation switching is provided in format "0x...":
				CharAnsi const UpperCaseCharacter = CharAnsiHelpers::ToUpperCase(self->CurrentCharacter);
				/**/ if (UpperCaseCharacter == Options.IntegerHexadecimalNotationDelimiter)	self->CurrentLexemIntegerNotation = 16;
				else if (UpperCaseCharacter == Options.IntegerOctalNotationDelimiter)		self->CurrentLexemIntegerNotation = 8;
				else if (UpperCaseCharacter == Options.IntegerBinaryNotationDelimiter)		self->CurrentLexemIntegerNotation = 2;
				else WasNotationSwitched = false;
			}

			if (!WasNotationSwitched)
			{	// Found, that prefix does not matches template or it does not follows '0' character
				self->RewindedCharacter = self->CurrentCharacter;
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
				self->RewindedCharacter = self->CurrentCharacter;
				self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
			}
		}	break;
		default: // OR Throwing debug Assertation if internal error exists
			GD_DEBUG_ASSERT_FALSE("Some strange things happened, unknown condition for ProcessInteger");
			break;
		}
	}

	/// Processes floating-point constants inside input stream.
	void BasicStreamedLexerImpl::ProcessFloat()
	{	// Nothing to do here.
	}

	/// Processes string constants inside input stream.
	void BasicStreamedLexerImpl::ProcessString()
	{
		GD_NOT_IMPLEMENTED();
	}

	/// Processes character constants inside input stream.
	void BasicStreamedLexerImpl::ProcessCharacter()
	{
		GD_NOT_IMPLEMENTED();
	}

	/// Processes operators inside input stream and handles switching to multiple line comment parsing.
	void BasicStreamedLexerImpl::ProcessOperatorOrComment()
	{	
		if (   (self->Options.SingleLineCommentDeclaration[self->CurrentMatchingIndex] == self->CurrentCharacter)
			|| (self->Options.MultipleLineCommentBeginning[self->CurrentMatchingIndex] == self->CurrentCharacter))
		{	
			self->CurrentMatchingIndex += 1;
			// Matching with single line comment ending.
			if (self->Options.SingleLineCommentDeclaration.GetSize() == self->CurrentMatchingIndex)
			{	// Final match, finalizing comment.
				self->CurrentMatchingIndex = 0;
				self->CurrentState = GD_LEXER_STATE_READING_COMMENT_SINGLELINE;
				self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
				self->CurrentLexem->ProcessedDataID = 0;
				return;
			}

			// Matching with multiple line comment ending.
			if (self->Options.MultipleLineCommentBeginning.GetSize() == self->CurrentMatchingIndex)
			{	// Final match, finalizing comment.
				self->CurrentMatchingIndex = 0;
				self->CurrentState = GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE;
				self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
				self->CurrentLexem->ProcessedDataID = 0;
				return;
			}
		}
	}

	/// Processes identifiers and keywords inside input stream.
	void BasicStreamedLexerImpl::ProcessIdentifierOrKeyword()
	{	// Nothing to do here.
	}

	/// Processes single line comments inside input stream.
	void BasicStreamedLexerImpl::ProcessSingleLineComment()
	{
		if (self->CurrentCharacter == Char('\n'))
			self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
	}

	/// Processes multiple line comments inside input stream.
	void BasicStreamedLexerImpl::ProcessMultipleLineComment()
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

	/// ==========================================================================================
	/// StreamedLexerImpl class.
	/// Implementation of a full-featured streaming lexer. 
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Internal class API:
	/// ------------------------------------------------------------------------------------------

	/// Resets lexer implementation.
	void StreamedLexerImpl::Reset()
	{
		self->BasicStreamedLexerImpl::Reset();
		self->CurrentLexemFloatExponent = 1;
		self->CurrentMatches.Emptify();
	}

	/// Processes integer constants inside input stream.
	void StreamedLexerImpl::ProcessInteger()
	{
		self->BasicStreamedLexerImpl::ProcessInteger();
		if (self->CurrentState == GD_LEXER_STATE_READING_CONSTANT_INTEGER)
		{	// Processing character if it is digit in current notation.
			UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
			self->CurrentLexem->ProcessedDataInteger *= self->CurrentLexemIntegerNotation;
			self->CurrentLexem->ProcessedDataInteger += CurrentDigit;
		}
	}

	/// Processes floating-point constants inside input stream.
	void StreamedLexerImpl::ProcessFloat()
	{	
		self->BasicStreamedLexerImpl::ProcessFloat();
		UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
		self->CurrentLexem->ProcessedDataFloat += (static_cast<Float64>(CurrentDigit) / static_cast<Float64>(self->CurrentLexemFloatExponent));
		self->CurrentLexemFloatExponent += 1;
	}

	/// Processes operators inside input stream and handles switching to multiple line comment parsing.
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

		bool HasAnyFullMatch = false;
		Vector<String const*> CurrentMatches(0, nullptr, self->CurrentMatches.GetSize());
		for (auto const CurrentOperatorOrCommentMatch : self->CurrentMatches)
		{	
			if ((*CurrentOperatorOrCommentMatch)[self->CurrentMatchingIndex] == self->CurrentCharacter)
			{	// Character matches some declaration at current index.
				if (CurrentOperatorOrCommentMatch->GetSize() == (1 + self->CurrentMatchingIndex))
				{	// We are having a full match with some operator, so lets identify it now
					/**/ if (CurrentOperatorOrCommentMatch == &self->Options.SingleLineCommentDeclaration)
					{	// Declaration matches single line comment declaration.
						self->CurrentMatchingIndex = 0;
						self->CurrentState = GD_LEXER_STATE_READING_COMMENT_SINGLELINE;
						self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
						self->CurrentLexem->ProcessedDataID = 0;
						return;
					}
					else if (CurrentOperatorOrCommentMatch == &self->Options.MultipleLineCommentBeginning)
					{	// Declaration matches multiple line comment declaration.
						self->CurrentMatchingIndex = 0;
						self->CurrentState = GD_LEXER_STATE_READING_COMMENT_MULTIPLELINE;
						self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
						self->CurrentLexem->ProcessedDataID = 0;
						return;
					}

					// Declaration matches operator declaration.
					size_t const Index = self->Options.OperatorDeclarations.FindFirstElement([&](StreamedLexerOperatorDecl const& OperatorDecl) -> bool {
						return ((&OperatorDecl.Second) == CurrentOperatorOrCommentMatch);
					});
					self->CurrentLexem->ProcessedDataID = self->Options.OperatorDeclarations[Index].First;

					HasAnyFullMatch = true;
					continue;
				}
				
				CurrentMatches.PushLast(CurrentOperatorOrCommentMatch);
			}
		}

		if (CurrentMatches.GetSize() == 0)
		{	// No matches in this iteration. That means that currect operator ID was set in previos. 
			if (!HasAnyFullMatch)
			{	// If there is not full math then some character is breaking the sequence breaking. Rewinding it to handle later.
				self->RewindedCharacter = self->CurrentCharacter;
			}

			// Committing.
			self->RaiseExceptionWithCode(GD_LEXER_EXCEPTION_COMMIT);
			return;
		}

		self->CurrentMatches = Move(CurrentMatches);
		self->CurrentMatchingIndex += 1;
	}

	/// Processes identifiers and keywords inside input stream.
	void StreamedLexerImpl::ProcessIdentifierOrKeyword()
	{
		if ((self->CurrentMatches.GetSize() == 0) && (self->CurrentMatchingIndex == 0))
		{	// Filling CurrentMatches with pointers on operators and comments declarations
			self->CurrentMatches.Reserve(self->Options.KeywordsDeclarations.GetSize());
			for (auto const& KeywordsDeclarations : self->Options.KeywordsDeclarations)
				self->CurrentMatches.PushLast(&KeywordsDeclarations.Second);
		}

		bool HasAnyFullMatch = false;
		Vector<String const*> CurrentMatches(0, nullptr, self->CurrentMatches.GetSize());
		for (auto const CurrentIdentifierMatch : self->CurrentMatches)
		{	
			if ((*CurrentIdentifierMatch)[self->CurrentMatchingIndex] == self->CurrentCharacter)
			{	// Character matches some declaration at current index
				if (CurrentIdentifierMatch->GetSize() == (1 + self->CurrentMatchingIndex))
				{	// We are having a full match with some keyword, so lets identify it now.
					size_t const Index = self->Options.KeywordsDeclarations.FindFirstElement([&](StreamedLexerKeywordDecl const& KeywordDecl) -> bool {
						return ((&KeywordDecl.Second) == CurrentIdentifierMatch);
					});

					self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_KEYWORD;
					self->CurrentLexem->ProcessedDataID = self->Options.KeywordsDeclarations[Index].First;
					
					HasAnyFullMatch = true;
					continue;
				}

				CurrentMatches.PushLast(CurrentIdentifierMatch);
			}
		}

		if (!HasAnyFullMatch)
		{	// We are heving next chracter that makes this lexem valid identifier.
			// Restoring identifer content type.
			self->CurrentLexem->ProcessedDataID = 0;
			self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_IDENTIFIER;
		}

		self->CurrentMatches = Move(CurrentMatches);
		self->CurrentMatchingIndex += 1;
	}

	/// ==========================================================================================
	/// StreamedLexer class.
	/// Class provides streamable tokenization for input stream.
	/// ==========================================================================================
	
	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	/// Initializes new streamed lexer.
	/// @param Toolchain   Corresponding toolchain.
	/// @param InputStream Stream on which lexer would work.
	/// @param Options     Packed lexing options list.
	StreamedLexer::StreamedLexer(IToolchain* const Toolchain, Stream* const InputStream, StreamedLexerOptions const& Options, StreamedLexerMode const LexerMode /* = GD_LEXER_MODE_DEFAULT */)
		: IToolchainTool(Toolchain)
		, ImplementationMode(LexerMode)
		, Options(Options)
	{
		self->Implementation = ((self->ImplementationMode == GD_LEXER_MODE_BASIC) 
			? new BasicStreamedLexerImpl(Toolchain, Options, InputStream)
			: new      StreamedLexerImpl(Toolchain, Options, InputStream));
	}

	StreamedLexer::~StreamedLexer()
	{
	}

	/// Switches lexer features set.
	/// @param LexerMode Describes avaliable feature set of Streamed Lexer.
	void StreamedLexer::SwitchMode(StreamedLexerMode const LexerMode /* = GD_LEXER_MODE_DEFAULT */)
	{
		if (LexerMode != self->ImplementationMode)
		{	// Lexer mode chagned, we need to switch used implementation.
			GD_DEBUG_ASSERT(LexerMode < GD_LEXER_MODES_COUNT, "Invalid lexer mode specfied");
			self->ImplementationMode = LexerMode;
			self->Implementation = ((LexerMode == GD_LEXER_MODE_BASIC) ? new BasicStreamedLexerImpl(*self->Implementation) : new StreamedLexerImpl(*self->Implementation));
		}
	}

	/// Extracts next lexem from input stream into output. Throws an exception into toolchain if error accures. Can be ran in simple and advanced mode.
	/// More about modes: 
	/// @param OutputLexem Pointer to output lexem variable.
	/// @param LexerMode   Describes avaliable feature set of Streamed Lexer.
	/// @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
	bool StreamedLexer::GetNextLexem(Lexem* const OutputLexem)
	{	// We are using implementation class to prevent dozens of private fields in class.
		return self->Implementation->GetNextLexem(OutputLexem);
	}

	/// ==========================================================================================
	/// StreamedLexerOptions class.
	/// Provides lexer information about syntax in compilable data.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

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
	)	: KeywordsDeclarations(Forward<StreamedLexerKeywordsList>(KeywordsDeclarations))
		, OperatorDeclarations(Forward<StreamedLexerOperatorsList>(OperatorDeclarations))
		, SingleLineCommentDeclaration(Forward<String>(SingleLineCommentDeclaration))
		, MultipleLineCommentBeginning(Forward<String>(MultipleLineCommentBeginning))
		, MultipleLineCommentEnding(Forward<String>(MultipleLineCommentEnding))
		, IntegerHexadecimalNotationDelimiter(CharAnsiHelpers::ToUpperCase(IntegerHexadecimalNotationDelimiter))
		, IntegerOctalNotationDelimiter(CharAnsiHelpers::ToUpperCase(IntegerOctalNotationDelimiter))
		, IntegerBinaryNotationDelimiter(CharAnsiHelpers::ToUpperCase(IntegerBinaryNotationDelimiter))
		, FloatingPointDelimiter(FloatingPointDelimiter)
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

	/// ==========================================================================================
	/// StreamedLexerDefaultOptions namespace.
	/// Contains some predefined options for streamed lexer.
	/// ==========================================================================================

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

	/// ==========================================================================================
	///	LexemContentType enum.
	/// ==========================================================================================

	extern Str LexemContentTypeToString(LexemContentType const ContentType)
	{
		static Str const LexemContentTypeStrings[] = {
			/* GD_LEXEM_CONTENT_TYPE_IDENTIFIER			= */ "identifier",
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

	//StreamedLexerOptions const& StreamedLexerDefaultOptions::GetDefaultOptionsForXml()
	//{
	//	static StreamedLexerOptions const XmlLexerOptions({
	//		"<", ">", "</", "/>", "<?", "?>",									// Nodes and header
	//		"=", "&", ";"														// Attributes and special characters parsing
	//	}, "", "<!--", "-->", 'x', '\0', '\0', '.');

	//	return XmlLexerOptions;
	//}

GD_NAMESPACE_END
