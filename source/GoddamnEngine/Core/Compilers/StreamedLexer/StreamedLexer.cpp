/// ==========================================================================================
/// StreamedLexer.cpp - tokenizer implementation
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	/// Represents lexer error description.
	struct StreamedLexerErrorDesc final : public ToolchainMessageDesc
	{
	public:
		GDINL explicit StreamedLexerErrorDesc(String const& Message) : ToolchainMessageDesc(Message) { }
	};	// struct StreamedLexerErrorDesc

	/// Fatal parsing error exception
	class StreamedLexerFatalErrorException : public ToolchainException
	{
	public /*Public API*/:
		GDINL explicit StreamedLexerFatalErrorException(String const& Message) : ToolchainException(Message.CStr()) { }
		GDINL virtual ~StreamedLexerFatalErrorException() { }
	};	// class ToolchainException

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

		StreamedLexerOptions const& Options						;
		UniquePtr<InputStream>		Stream						;

		bool						WasLexemCommited			= false;
		Lexem*                      CurrentLexem                = nullptr;						// Lexem state & info
		size_t                      CurrentLine                 = 1;					    	// Lexem location
		size_t                      CurrentSymbol               = 0;
		CharAnsi                    RewindedCharacter			= CharAnsi('\0');	

		StreamedLexerState          CurrentState                = GD_LEXER_STATE_UNKNOWN;
		CharAnsi                    CurrentCharacter            = CharAnsi('\0');				// Character meta
		StreamedLexerCharType       CurrentCharacterType        = GD_CHARACTER_TYPE_UNKNOWN;
		size_t static constexpr     DefaultLexemIntegerNotation = 10;							// Numeric meta
		size_t                      CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		size_t                      CurrentMatchingIndex        = 0;				 			// Operators and comments meta

	public:
		GDINL BasicStreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, UniquePtr<InputStream>&& Stream)
			: IToolchainTool(Toolchain)
			, Stream(Forward<UniquePtr<InputStream>>(Stream))
			, Options(Options)
		{
		}

		GDINL BasicStreamedLexerImpl(BasicStreamedLexerImpl const& Other) = delete;
		GDINL BasicStreamedLexerImpl(BasicStreamedLexerImpl     && Other)
			: IToolchainTool(Other.Toolchain)
			, Stream(Forward<UniquePtr<InputStream>>(Other.Stream))
			, RewindedCharacter(Other.RewindedCharacter)
			, CurrentSymbol(Other.CurrentSymbol)
			, CurrentLine(Other.CurrentLine)
			, Options(Other.Options)
		{
		}

		GDINL virtual ~BasicStreamedLexerImpl()
		{
		}

		/// Reads next lexem from Input stream.
		/// @returns True if lexem was extracted succesfully.
		GDINT bool GetNextLexem(Lexem* const OutputLexem);

		/// Reads next character from input stream. 
		/// @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
		GDINT CharAnsi GetNextCharacter();

		/// Pushes character back to input stream/caches character.
		/// @param Character Character that would be reverted.
		GDINT void RevertCharacter(CharAnsi const Character);

	private /*Internal class API*/:
		/// Processes single character from input stream.
		GDINT void ProcessUpcomingCharacter();

	protected /*Internal class API*/:
		/// Resets lexer implementation.
		GDINT virtual void Reset();

		/// Processes integer constants inside input stream.
		GDINT virtual void ProcessIntegerConstant();

		/// Processes floating-point constants inside input stream.
		GDINT virtual void ProcessFloatConstant();

		/// Processes string constants inside input stream.
		GDINT virtual void ProcessStringConstant();

		/// Processes character constants inside input stream.
		GDINT virtual void ProcessCharacterConstant();

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
	private /*Class Types and Members*/:
		enum StreamedLexerESMode : UInt8
		{
			GD_LEXER_ESCAPE_SEQUENCE_MODE_WAITING,
			GD_LEXER_ESCAPE_SEQUENCE_MODE_OCTAL_CHARACTER,
			GD_LEXER_ESCAPE_SEQUENCE_MODE_HEXADECIMAL_CHARACTER,
			GD_LEXER_ESCAPE_SEQUENCE_MODE_UNICODE8,
			GD_LEXER_ESCAPE_SEQUENCE_MODE_UNICODE16,
			GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN,
		};	// enum StreamedLexerEscapeSequenceMode

		size_t                      CurrentLexemFloatExponent   = 1;
		Vector<String const*>       CurrentMatches              ;
		StreamedLexerESMode			CurrentESMode				= GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN;
		bool						CurrentlyWasCharWritten		= false;

	public /*CLass API*/:
		GDINL StreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, UniquePtr<InputStream>&& Stream)
			: BasicStreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream))
		{
		}

		GDINL StreamedLexerImpl(BasicStreamedLexerImpl const& Other) = delete;
		GDINL StreamedLexerImpl(BasicStreamedLexerImpl     && Other)
			: BasicStreamedLexerImpl(Forward<BasicStreamedLexerImpl>(Other))
		{
		}

		GDINL virtual ~StreamedLexerImpl()
		{
		}

	private /*Internal class API*/:
		/// Resets lexer implementation.
		GDINT virtual void Reset() final override;

		/// Processes integer constants inside input stream.
		GDINT virtual void ProcessIntegerConstant() final override;
		
		/// Processes floating-point constants inside input stream.
		GDINT virtual void ProcessFloatConstant() final override;

		/// Processes escape sequence in string or character constant.
		GDINT void ProcessEscapeSequenceInStringOrCharacterConstant();

		/// Processes string constants inside input stream.
		GDINT virtual void ProcessStringConstant() final override;

		/// Processes character constants inside input stream.
		GDINT virtual void ProcessCharacterConstant() final override;

		/// Processes operators inside input stream and handles switching to multiple line comment parsing.
		GDINT virtual void ProcessOperatorOrComment() final override;
		
		/// Processes identifiers and keywords inside input stream.
		GDINT virtual void ProcessIdentifierOrKeyword() final override;
	};	// class StreamedLexerImpl

	/// ==========================================================================================
	/// BasicStreamedLexerImpl class.
	/// Implementation of a basic streaming lexer. 
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Internal class API:
	/// ------------------------------------------------------------------------------------------

	/// Resets lexer implementation.
	void BasicStreamedLexerImpl::Reset()
	{
		self->WasLexemCommited			  = false;
		self->CurrentState                = GD_LEXER_STATE_UNKNOWN;
		self->CurrentCharacter            = CharAnsi('\0');				
		self->CurrentCharacterType        = GD_CHARACTER_TYPE_UNKNOWN;
		self->CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		self->CurrentMatchingIndex        = 0;			
	}

	/// Reads next character from input stream. If stream ends, that returns '\0'. Also handles '\n' and '\r' sequences.
	CharAnsi BasicStreamedLexerImpl::GetNextCharacter()
	{	// Testing if we have rewinded character.
		if (self->RewindedCharacter == CharAnsi('\0'))
		{
			try
			{	// Trying to read next character:
				self->CurrentCharacter = self->Stream->Read();
				if (self->CurrentCharacter == CharAnsi('\r'))
				{	// Trying to handle '\r\n' sequence.
					CharAnsi NextCharacter = self->Stream->Read();
					if (NextCharacter != CharAnsi('\n'))
						self->RewindedCharacter = NextCharacter;

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
			catch (IOException const&)
			{	// We have failed to read.
				self->CurrentCharacter = CharAnsi('\0');
			}
		}
		else
		{
			self->CurrentCharacter = self->RewindedCharacter;
			self->RewindedCharacter = CharAnsi('\0');
		}

		return self->CurrentCharacter;
	}

	/// Pushes character back to input stream/caches character.
	/// @param Character Character that would be reverted.
	void BasicStreamedLexerImpl::RevertCharacter(CharAnsi const Character)
	{
		if (Character == CharAnsi('\n'))
		{	// Reverting line break.
			self->CurrentSymbol = SIZE_MAX;
			self->CurrentLine -= 1;
		}
		else
		{	// Reverting just last character.
			self->CurrentSymbol -= 1;
		}

		if (self->RewindedCharacter == CharAnsi('\0'))
			self->RewindedCharacter = Character;
		else
			self->Stream->Seek(-1, SeekOrigin::End);
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
		self->GetNextCharacter();
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
			StreamedLexerErrorDesc static const InvalidCharacterParsingError("invalid character with code 0x%x was read from input stream.");
			throw StreamedLexerFatalErrorException(InvalidCharacterParsingError.ToString(self->CurrentLexem, size_t(self->CurrentCharacter)));
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
				self->WasLexemCommited = true;
				return;
			}
		}

		struct LexemStatesDesc final { bool RequiresClosingStatement; void (BasicStreamedLexerImpl::* SpecialProcessorPtr)(); };
		static LexemStatesDesc const LexemStatesDescTable[GD_LEXER_STATES_COUNT] = {
			/* GD_LEXER_STATE_READING_CONSTANT_STRING	    */ {  true, &BasicStreamedLexerImpl::ProcessStringConstant },
			/* GD_LEXER_STATE_READING_CONSTANT_CHARACTER    */ {  true, &BasicStreamedLexerImpl::ProcessCharacterConstant },
			/* GD_LEXER_STATE_READING_CONSTANT_INTEGER	    */ { false, &BasicStreamedLexerImpl::ProcessIntegerConstant },
			/* GD_LEXER_STATE_READING_CONSTANT_FLOAT	    */ { false, &BasicStreamedLexerImpl::ProcessFloatConstant },
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
					throw LogicException("Internal error: invalid CurrentCharacterType");
				} break;
			}
		}
		else if (self->CurrentCharacter == CharAnsi('\0'))
		{	// End of stream reached.
			if (LexemStatesDescTable[size_t(self->CurrentState)].RequiresClosingStatement)
			{	// End of stream reached, but we were parsing token that requires terminator. Error:
				StreamedLexerErrorDesc static const UnexpectedEndOfStream("unexpected end of stream while parsing %s.");
				throw StreamedLexerFatalErrorException(UnexpectedEndOfStream.ToString(self->CurrentLexem, LexemContentTypeToString(self->CurrentLexem->ContentType)));
			}

			self->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_EOS;
			self->WasLexemCommited = true;
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
	void BasicStreamedLexerImpl::ProcessIntegerConstant()
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
				self->WasLexemCommited = true;
				return;
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
				self->WasLexemCommited = true;
				return;
			}
		}	break;
		default: // OR Throwing debug Assertation if internal error exists
			throw LogicException("Some strange things happened, unknown condition for ProcessIntegerConstant");
		}
	}

	/// Processes floating-point constants inside input stream.
	void BasicStreamedLexerImpl::ProcessFloatConstant()
	{	// Nothing to do here.
	}

	/// Processes string constants inside input stream.
	void BasicStreamedLexerImpl::ProcessStringConstant()
	{	// Parsing string contants in basics mode: so no espace sequences.
		if (self->CurrentCharacter == Char('\"'))
			self->WasLexemCommited = true;
	}

	/// Processes character constants inside input stream.
	void BasicStreamedLexerImpl::ProcessCharacterConstant()
	{	// Parsing character contants in basics mode: so no espace sequences.
		if (self->CurrentCharacter == Char('\''))
			self->WasLexemCommited = true;
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
			self->WasLexemCommited = true;
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
				self->WasLexemCommited = true;
				return;
			}
		}
	}

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	/// Reads next lexem from Input stream.
	/// @returns True if lexem was extracted succesfully.
	bool BasicStreamedLexerImpl::GetNextLexem(Lexem* const OutputLexem)
	{	// Restoring default tokenizer values
		self->Reset();
		self->CurrentLexem = OutputLexem;
		self->CurrentLexem->ResetLexem();
		while (!self->WasLexemCommited)
			self->ProcessUpcomingCharacter();
		return true;
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
		self->CurrentMatches			.Emptify();
		self->CurrentLexemFloatExponent	= 1;
		self->CurrentESMode				= GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN;
		self->CurrentlyWasCharWritten	= false;
	}

	/// Processes integer constants inside input stream.
	void StreamedLexerImpl::ProcessIntegerConstant()
	{
		self->BasicStreamedLexerImpl::ProcessIntegerConstant();
		if (self->CurrentState == GD_LEXER_STATE_READING_CONSTANT_INTEGER)
		{	// Processing character if it is digit in current notation.
			UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
			self->CurrentLexem->ProcessedDataInteger *= self->CurrentLexemIntegerNotation;
			self->CurrentLexem->ProcessedDataInteger += CurrentDigit;
		}
	}

	/// Processes floating-point constants inside input stream.
	void StreamedLexerImpl::ProcessFloatConstant()
	{	
		self->BasicStreamedLexerImpl::ProcessFloatConstant();
		UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
		self->CurrentLexem->ProcessedDataFloat += (static_cast<Float64>(CurrentDigit) / static_cast<Float64>(self->CurrentLexemFloatExponent));
		self->CurrentLexemFloatExponent += 1;
	}

	/// Processes escape sequence in string or character constant.
	void StreamedLexerImpl::ProcessEscapeSequenceInStringOrCharacterConstant()
	{
		switch (self->CurrentESMode)
		{
		case GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN:
			if (self->CurrentCharacter == CharAnsi('\\'))
				self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_WAITING;
			break;

		case GD_LEXER_ESCAPE_SEQUENCE_MODE_WAITING:
			self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN;
			self->CurrentlyWasCharWritten = true;
			switch (self->CurrentCharacter)
			{	// For more details read here: http://en.wikipedia.org/wiki/Escape_sequences_in_C
			case CharAnsi('a'):		// Alarm (Beep, Bell)
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\a');
				return;
			case CharAnsi('b'):		// Backspace
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\b');
				return;
			case CharAnsi('f'):		// Formfeed
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\f');
				return;
			case CharAnsi('n'):		// Newline (Line Feed)
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\n');
				return;
			case CharAnsi('r'):		// Carriage Return
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\r');
				return;
			case CharAnsi('t'):		// Horizontal Tab
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\t');
				return;
			case CharAnsi('v'):		// Vertical Tab
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\v');
				return;
			case CharAnsi('\\'):	// Vertical Tab
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\\');
				return;
			case CharAnsi('\''):	// Single quotation mark
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\'');
				return;
			case CharAnsi('\"'):	// Double quotation mark
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\"');
				return;
			case CharAnsi('\?'):	// Question mark
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\?');
				return;
			case CharAnsi('x'):		// The character whose numerical value is given by hh interpreted as a hexadecimal number
				self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_HEXADECIMAL_CHARACTER;
				self->CurrentlyWasCharWritten = false;
				return;
			case CharAnsi('u'):		// Unicode characters in string literals (UTF8)
				self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_UNICODE8;
				self->CurrentlyWasCharWritten = false;
				return;
			case CharAnsi('U'):		// Unicode characters in string literals (UTF16)
				self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_UNICODE16;
				self->CurrentlyWasCharWritten = false;
				return;
			default:
				if (!CharAnsiHelpers::IsDigit(self->CurrentCharacter, 8))
				{	// Unrecognized character escape sequence.
					StreamedLexerErrorDesc static const UnrecognizedCharacterEscapeSequenceError("unrecognized character escape sequence '\\%c' in string/character constant.");
					throw StreamedLexerFatalErrorException(UnrecognizedCharacterEscapeSequenceError.ToString(self->CurrentLexem, self->CurrentCharacter));
				}

				// The character whose numerical value is given by nnn interpreted as an octal number
				self->CurrentlyWasCharWritten = false;
				self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_OCTAL_CHARACTER;
				self->CurrentLexem->ProcessedDataCharacter = CharAnsi('\0');
				goto ProcessOctalDigitNow;
			}	

		case GD_LEXER_ESCAPE_SEQUENCE_MODE_OCTAL_CHARACTER:
		case GD_LEXER_ESCAPE_SEQUENCE_MODE_HEXADECIMAL_CHARACTER:
		{	
		ProcessOctalDigitNow:;
			UInt8 const CurrentNotation = ((self->CurrentESMode == GD_LEXER_ESCAPE_SEQUENCE_MODE_OCTAL_CHARACTER) ? 8 : 16);
			if (!CharAnsiHelpers::IsDigit(self->CurrentCharacter, CurrentNotation))
			{	// Here is not a not a digit character.
				if ((self->CurrentLexem->GetRawData().GetSize() < 2) && (CurrentNotation == 16))
				{	// Hex constants must have at least one hex digit (now raw data contains string "'\").
					StreamedLexerErrorDesc static const EmptyHexadecimalCharacterError("empty string/character constant with '\\x'");
					throw StreamedLexerFatalErrorException(EmptyHexadecimalCharacterError.ToString(self->CurrentLexem));
				}

				self->CurrentESMode = GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN;
				self->CurrentlyWasCharWritten = true;
				return;
			}

			// Processing next character
			UInt8 const CurrentDigit = CharAnsiHelpers::ToDigit(self->CurrentCharacter);
			if ((static_cast<size_t>(self->CurrentLexem->ProcessedDataCharacter) * CurrentNotation + CurrentDigit) > UCHAR_MAX)
			{	// Too big for character error.
				StreamedLexerErrorDesc static const TooBigNumberForCharacterError("too big number for character.");
				throw StreamedLexerFatalErrorException(TooBigNumberForCharacterError.ToString(self->CurrentLexem));
			}

			self->CurrentLexem->ProcessedDataCharacter *= CurrentNotation;
			self->CurrentLexem->ProcessedDataCharacter += CurrentDigit;
			return;
		}
		
		default:
			throw NotImplementedException("Parsing this StreamedLexerImpl::CurrentESMode is not implemented");
		}
	}

	/// Processes string constants inside input stream.
	void StreamedLexerImpl::ProcessStringConstant()
	{
		throw NotImplementedException("'StreamedLexerImpl::ProcessStringConstant' is not implemented");
	}

	/// Processes character constants inside input stream.
	void StreamedLexerImpl::ProcessCharacterConstant()
	{	
		self->ProcessEscapeSequenceInStringOrCharacterConstant();
		if (self->CurrentESMode == GD_LEXER_ESCAPE_SEQUENCE_MODE_UNKNOWN)
		{	// We are sure that escape sequence does not exists or was succesfully parsed.
			if (self->CurrentCharacter == CharAnsi('\''))
			{	// Here is unescaped "'" character.
				if (!self->CurrentlyWasCharWritten)
				{	// Empty character constant error. (now raw data contains quote mark ("'") and parsed ).
					StreamedLexerErrorDesc static const EmptyCharacterConstantError("empty character constant.");
					throw StreamedLexerFatalErrorException(EmptyCharacterConstantError.ToString(self->CurrentLexem));
				}
			
				self->WasLexemCommited = true;
				return;
			}
			
			if (self->CurrentlyWasCharWritten)
			{	// Multiple characters inside character constant.
				StreamedLexerErrorDesc static const MultiCharacterCharacterConstantError("multiple characters in character constant %s%c'.");
				throw StreamedLexerFatalErrorException(MultiCharacterCharacterConstantError.ToString(self->CurrentLexem, self->CurrentLexem->GetRawData().CStr(), self->CurrentCharacter));
			}

			// Here is our character.
			self->CurrentLexem->ProcessedDataCharacter = self->CurrentCharacter;
			self->CurrentlyWasCharWritten = true;
		}
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
		Vector<String const*> CurrentMatches(0, self->CurrentMatches.GetSize());
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

			self->WasLexemCommited = true;
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
		Vector<String const*> CurrentMatches(0, self->CurrentMatches.GetSize());
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
	/// @param Stream      Input stream on which lexer would work.
	/// @param Options     Packed lexing options list.
	StreamedLexer::StreamedLexer(IToolchain* const Toolchain, UniquePtr<InputStream>&& Stream, StreamedLexerOptions const& Options, StreamedLexerMode const LexerMode /* = GD_LEXER_MODE_DEFAULT */)
		: IToolchainTool(Toolchain)
		, ImplementationMode(LexerMode)
		, Options(Options)
	{
		self->Implementation = ((self->ImplementationMode == StreamedLexerMode::Basic) 
			? new BasicStreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream))
			: new      StreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream)));
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
			GD_DEBUG_ASSERT(LexerMode < StreamedLexerMode::Count, "Invalid lexer mode specfied");
			self->ImplementationMode = LexerMode;
			self->Implementation = ((LexerMode == StreamedLexerMode::Basic)
				? new BasicStreamedLexerImpl(Move(*self->Implementation)) 
				: new      StreamedLexerImpl(Move(*self->Implementation)));
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

	/// Exctracts next character from input stream and wraps line breaks, etc.
	/// @returns Exctracted character.
	CharAnsi StreamedLexer::GetNextCharacter()
	{	// We are using implementation class to prevent dozens of private fields in class.
		return self->Implementation->GetNextCharacter();
	}

	/// Pushes specified character back to lexer and make it parsable.
	/// @param Character Character that would be reverted.
	void StreamedLexer::RevertCharacter(CharAnsi const Character)
	{	// We are using implementation class to prevent dozens of private fields in class.
		return Implementation->RevertCharacter(Character);
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
		, IntegerHexadecimalNotationDelimiter((IntegerHexadecimalNotationDelimiter != CharAnsi('\0')) ? CharAnsiHelpers::ToUpperCase(IntegerHexadecimalNotationDelimiter) : CharAnsi('\0'))
		, IntegerOctalNotationDelimiter((IntegerOctalNotationDelimiter != CharAnsi('\0')) ? CharAnsiHelpers::ToUpperCase(IntegerOctalNotationDelimiter) : CharAnsi('\0'))
		, IntegerBinaryNotationDelimiter((IntegerBinaryNotationDelimiter != CharAnsi('\0')) ? CharAnsiHelpers::ToUpperCase(IntegerBinaryNotationDelimiter) : CharAnsi('\0'))
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
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FINAL,            "final" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FINALLY,          "finally" },
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

GD_NAMESPACE_END
