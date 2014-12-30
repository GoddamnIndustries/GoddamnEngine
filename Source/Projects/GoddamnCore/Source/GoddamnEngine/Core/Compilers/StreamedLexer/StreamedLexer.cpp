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

#include <climits>

GD_NAMESPACE_BEGIN

	/// Represents lexer error description.
	struct StreamedLexerErrorDesc final : public ToolchainMessageDesc
	{
	public:
		GDINL explicit StreamedLexerErrorDesc(String const& Message) : ToolchainMessageDesc(Message) { }
	};	// struct StreamedLexerErrorDesc

	/// Fatal parsing error exception.
	GD_DEFINE_EXCPETION_DERIVED_FINAL(StreamedLexerFatalErrorException, ToolchainException);

	/// Implementation of a basic streaming lexer. 
	class BasicStreamedLexerImpl : public IToolchainTool
	{
	protected /*Class Types and members*/:
		enum class StreamedLexerState : UInt8
		{
			ReadingConstantString,
			ReadingConstantCharacter,
			ReadingConstantInteger,
			ReadingConstantFloat,
			ReadingCommentOrOperator,
			ReadingCommentSingleLine,
			ReadingCommentMultipleLine,
			ReadingIdentifierOrKeyword,
			Unknown,
		};	// enum class StreamedLexerState
		size_t static const LexerStatesCount = static_cast<size_t>(StreamedLexerState::Unknown);

		enum class StreamedLexerCharType : UInt8
		{
			Space,
			Digit,
			Special,
			Alphabetic,
			Unknown,
		};	// enum class StreamedLexerCharType
		size_t static const CharTypeCount = static_cast<size_t>(StreamedLexerCharType::Unknown);

		StreamedLexerOptions const& Options						;
		UniquePtr<InputStream>		Stream						;

		bool						WasLexemCommited			= false;
		Lexem*       CurrentLexem    = nullptr;						// Lexem state & info
		size_t       CurrentLine     = 1;					 	// Lexem location
		size_t       CurrentSymbol      = 0;
		CharAnsi     RewindedCharacter			= CharAnsi('\0');	

		StreamedLexerState    CurrentState    = StreamedLexerState::Unknown;
		CharAnsi     CurrentCharacter   = CharAnsi('\0');				// Character meta
		StreamedLexerCharType    CurrentCharacterType  = StreamedLexerCharType::Unknown;
		size_t static constexpr  DefaultLexemIntegerNotation = 10;							// Numeric meta
		size_t       CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		size_t       CurrentMatchingIndex  = 0;				 			// Operators and comments meta

	public:
		GDINL BasicStreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, UniquePtr<InputStream>&& Stream)
			: IToolchainTool(Toolchain)
			, Stream(Forward<UniquePtr<InputStream>>(Stream))
			, Options(Options)
		{
		}

		GDINL BasicStreamedLexerImpl(BasicStreamedLexerImpl const& Other) = delete;
		GDINL BasicStreamedLexerImpl(BasicStreamedLexerImpl  && Other)
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
		enum class StreamedLexerESMode : UInt8
		{
			Waiting,
			OctalCharacter,
			HexadecimalCharacter,
			Unicode8,
			Unicode16,
			Unknown,
		};	// enum class StreamedLexerEscapeSequenceMode
		  
		size_t       CurrentLexemFloatExponent   = 1;
		Vector<String const*>    CurrentMatches     ;
		StreamedLexerESMode			CurrentESMode				= StreamedLexerESMode::Unknown;
		bool						CurrentlyWasCharWritten		= false;

	public /*CLass API*/:
		GDINL StreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, UniquePtr<InputStream>&& Stream)
			: BasicStreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream))
		{
		}

		GDINL StreamedLexerImpl(BasicStreamedLexerImpl const& Other) = delete;
		GDINL StreamedLexerImpl(BasicStreamedLexerImpl  && Other)
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
		this->WasLexemCommited			  = false;
		this->CurrentState    = StreamedLexerState::Unknown;
		this->CurrentCharacter   = CharAnsi('\0');				
		this->CurrentCharacterType  = StreamedLexerCharType::Unknown;
		this->CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		this->CurrentMatchingIndex  = 0;			
	}

	/// Reads next character from input stream. If stream ends, that returns '\0'. Also handles '\n' and '\r' sequences.
	CharAnsi BasicStreamedLexerImpl::GetNextCharacter()
	{	// Testing if we have rewinded character.
		if (this->RewindedCharacter == CharAnsi('\0')) {
			try	{	// Trying to read next character:
				this->CurrentCharacter = this->Stream->Read();
				if (this->CurrentCharacter == CharAnsi('\r')) {	// Trying to handle '\r\n' sequence.
					CharAnsi NextCharacter = this->Stream->Read();
					if (NextCharacter != CharAnsi('\n'))
						this->RewindedCharacter = NextCharacter;

					// New line here.
					this->CurrentCharacter = CharAnsi('\n');
					this->CurrentSymbol = 0;
					this->CurrentLine += 1;
				} else if (this->CurrentCharacter == CharAnsi('\n')) {	// Handing line termination characters.
					this->CurrentSymbol = 0;
					this->CurrentLine += 1;
				} else {	// Next character on current line.
					this->CurrentSymbol += 1;
				}
			} catch (IOException const&) {	// We have failed to read.
				this->CurrentCharacter = CharAnsi('\0');
			}
		} else {
			this->CurrentCharacter = this->RewindedCharacter;
			this->RewindedCharacter = CharAnsi('\0');
		}

		return this->CurrentCharacter;
	}

	/// Pushes character back to input stream/caches character.
	/// @param Character Character that would be reverted.
	void BasicStreamedLexerImpl::RevertCharacter(CharAnsi const Character)
	{
		if (Character == CharAnsi('\n')) {	// Reverting line break.
			this->CurrentSymbol = SIZE_MAX;
			this->CurrentLine -= 1;
		} else {	// Reverting just last character.
			this->CurrentSymbol -= 1;
		}

		if (this->RewindedCharacter == CharAnsi('\0')) {
			this->RewindedCharacter = Character;
		} else {
			this->Stream->Seek(-1, SeekOrigin::End);
		}
	}

	/// Processes single character from input stream.
	void BasicStreamedLexerImpl::ProcessUpcomingCharacter()
	{	// Now lets handle the type of read character. Here we have a table that describes if we need to check character type switching
		// or ignore switching in several cases. Building a translation table: 
		struct TestCharacterTypesSwitchingTable final { bool DoTestCharacterTypeSwitch; StreamedLexerCharType NotCommitOnSwitchingTo[CharTypeCount + 1]; };
		static TestCharacterTypesSwitchingTable const TestCharacterTypeSwitchingTable[GD_LEXEM_CONTENT_TYPES_COUNT + 1] = {
			/* GD_LEXEM_CONTENT_TYPE_IDENTIFIER			*/ { true,  { StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_KEYWORD			*/ { true,  { StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_OPERATOR			*/ { true,	{ StreamedLexerCharType::Special, StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_COMMENT			*/ { false, { StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_STRING	*/ { false, { StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER	*/ { false, { StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER	*/ { true,	{ StreamedLexerCharType::Digit, StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Special, StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT		*/ { true,	{ StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown } },
			/* GD_LEXEM_CONTENT_TYPE_UNKNOWN			*/ { true,	{ StreamedLexerCharType::Space, StreamedLexerCharType::Unknown } }
		};

		// Reading next character from stream and determining the type of new character and possible type switch.
		StreamedLexerCharType const PreviousCharacterType = this->CurrentCharacterType;
		this->GetNextCharacter();
		this->CurrentCharacterType	// Searching in custom special symbols list and treating all others as valid identifier characters
			= (this->Options.IsInSpecialCharactersAlphabet(this->CurrentCharacter))	             ? StreamedLexerCharType::Special 
			: (CharAnsiTraits::IsAlphabetic(this->CurrentCharacter) || CharAnsiTraits::IsSpecialCharacter(this->CurrentCharacter)) ? StreamedLexerCharType::Alphabetic
			: (CharAnsiTraits::IsDigit  (this->CurrentCharacter, this->CurrentLexemIntegerNotation))        ? StreamedLexerCharType::Digit
			: (CharAnsiTraits::IsSpace  (this->CurrentCharacter))                ? StreamedLexerCharType::Space : StreamedLexerCharType::Unknown; 
		if ((this->CurrentCharacterType == StreamedLexerCharType::Space) && (PreviousCharacterType == StreamedLexerCharType::Unknown)) {	// Upcoming space that splits lexems. Just skipping it.
			this->CurrentCharacterType = StreamedLexerCharType::Unknown;
			return;
		} else if ((this->CurrentCharacterType == StreamedLexerCharType::Unknown) && (this->CurrentCharacter != CharAnsi('\0'))) {	// Unknown character exists in stream (except \0 character). Error:
			StreamedLexerErrorDesc static const InvalidCharacterParsingError("invalid character with code 0x%x was read from input stream.");
			throw StreamedLexerFatalErrorException(InvalidCharacterParsingError.ToString(this->CurrentLexem, size_t(this->CurrentCharacter)));
		}

		TestCharacterTypesSwitchingTable const& CurrentTypeSwitchingTable = TestCharacterTypeSwitchingTable[size_t(this->CurrentLexem->ContentType)];
		if (CurrentTypeSwitchingTable.DoTestCharacterTypeSwitch && (PreviousCharacterType != this->CurrentCharacterType) && (PreviousCharacterType != StreamedLexerCharType::Unknown)) {	
			// Parsing all conditions where token committing is required.
			// Attempting to switch state. Skipping if previous type is unknown - tokenizer was launched for the first time 
			StreamedLexerCharType const* NotSwitchOnTypePtr = &CurrentTypeSwitchingTable.NotCommitOnSwitchingTo[0];
			for (; (*NotSwitchOnTypePtr) != StreamedLexerCharType::Unknown; NotSwitchOnTypePtr += 1) {	// Testing if current token is required to commit
				if ((*NotSwitchOnTypePtr) == this->CurrentCharacterType) {	// Ignoring the commit and continuing processing
					break;
				}
			}

			if ((*NotSwitchOnTypePtr) == StreamedLexerCharType::Unknown) {	// Rewinding character that caused character type switching and committing current character
				this->RewindedCharacter = this->CurrentCharacter;
				this->WasLexemCommited = true;
				return;
			}
		}

		struct LexemStatesDesc final { bool RequiresClosingStatement; void (BasicStreamedLexerImpl::* SpecialProcessorPtr)(); };
		static LexemStatesDesc const LexemStatesDescTable[LexerStatesCount] = {
			/* StreamedLexerState::ReadingConstantString	  */ {  true, &BasicStreamedLexerImpl::ProcessStringConstant   },
			/* StreamedLexerState::ReadingConstantCharacter   */ {  true, &BasicStreamedLexerImpl::ProcessCharacterConstant   },
			/* StreamedLexerState::ReadingConstantInteger	  */ { false, &BasicStreamedLexerImpl::ProcessIntegerConstant  },
			/* StreamedLexerState::ReadingConstantFloat	   */ { false, &BasicStreamedLexerImpl::ProcessFloatConstant    },
			/* StreamedLexerState::ReadingCommentOrOperator   */ { false, &BasicStreamedLexerImpl::ProcessOperatorOrComment   },
			/* StreamedLexerState::ReadingCommentSingleLine   */ { false, &BasicStreamedLexerImpl::ProcessSingleLineComment   },
			/* StreamedLexerState::ReadingCommentMultipleLine */ {  true, &BasicStreamedLexerImpl::ProcessMultipleLineComment },
			/* StreamedLexerState::ReadingIdentifierOrKeyword */ { false, &BasicStreamedLexerImpl::ProcessIdentifierOrKeyword },
		};

		/**/ if ((this->CurrentCharacter != CharAnsi('\0')) && (this->CurrentState == StreamedLexerState::Unknown)) {	// Here is first real character of lexem.
			this->CurrentLexem->Line = this->CurrentLine;
			this->CurrentLexem->Symbol = this->CurrentSymbol;

			// Attempting to detect new state after previous commit 
			switch (this->CurrentCharacter)	{	// Detecting opening quote types:
				case CharAnsi('\"'): {
					this->CurrentState = StreamedLexerState::ReadingConstantString;
					this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_STRING;
					this->CurrentLexem->RawData += CharAnsi('\"');
				} return;
				case CharAnsi('\''): {
					this->CurrentState = StreamedLexerState::ReadingConstantCharacter;
					this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_CHARACTER;
					this->CurrentLexem->RawData += CharAnsi('\'');
				} return;
				default: {
					switch (this->CurrentCharacterType) {	// Parsing other character types:
						case StreamedLexerCharType::Space: {
						} return;
						case StreamedLexerCharType::Digit: {
							this->CurrentState = StreamedLexerState::ReadingConstantInteger;
							this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER;
						} break;
						case StreamedLexerCharType::Alphabetic: {
							this->CurrentState = StreamedLexerState::ReadingIdentifierOrKeyword;
							this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_IDENTIFIER;
						} break;
						case StreamedLexerCharType::Special: {
							this->CurrentState = StreamedLexerState::ReadingCommentOrOperator;
							this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_OPERATOR;
						} break;
						default: {
							throw LogicException("Internal error: invalid CurrentCharacterType");
						}
					}
				} break;
			}
		} else if (this->CurrentCharacter == CharAnsi('\0')) {	// End of stream reached.
			if (LexemStatesDescTable[size_t(this->CurrentState)].RequiresClosingStatement) {	// End of stream reached, but we were parsing token that requires terminator. Error:
				StreamedLexerErrorDesc static const UnexpectedEndOfStream("unexpected end of stream while parsing %s.");
				throw StreamedLexerFatalErrorException(UnexpectedEndOfStream.ToString(this->CurrentLexem, LexemContentTypeToString(this->CurrentLexem->ContentType)));
			}

			this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_EOS;
			this->WasLexemCommited = true;
			return;
		}
		
		// Parsing current character
		// this->CurrentState cannot be greater equal to unknown state
		(this->*(LexemStatesDescTable[size_t(this->CurrentState)].SpecialProcessorPtr))();
		if (this->RewindedCharacter == CharAnsi('\0')) {
			this->CurrentLexem->RawData += this->CurrentCharacter;
		}
	}

	/// Processes integer constants inside input stream.
	void BasicStreamedLexerImpl::ProcessIntegerConstant()
	{
		switch (this->CurrentCharacterType) {
			case StreamedLexerCharType::Digit: { } break;	// Just skipping digits.
			case StreamedLexerCharType::Alphabetic: {	// Switching notation:
				bool WasNotationSwitched = ((this->CurrentLexem->ProcessedDataInteger == 0) && (this->CurrentLexemIntegerNotation == StreamedLexerImpl::DefaultLexemIntegerNotation));
				if (WasNotationSwitched) {	// Checking if notation switching is provided in format "0x...":
					CharAnsi const UpperCaseCharacter = CharAnsiTraits::ToUpper(this->CurrentCharacter);
					/**/ if (UpperCaseCharacter == Options.IntegerHexadecimalNotationDelimiter)	this->CurrentLexemIntegerNotation = 16;
					else if (UpperCaseCharacter == Options.IntegerOctalNotationDelimiter)		this->CurrentLexemIntegerNotation = 8;
					else if (UpperCaseCharacter == Options.IntegerBinaryNotationDelimiter)		this->CurrentLexemIntegerNotation = 2;
					else WasNotationSwitched = false;
				}

				if (!WasNotationSwitched) {	// Found, that prefix does not matches template or it does not follows '0' character
					this->RewindedCharacter = this->CurrentCharacter;
					this->WasLexemCommited = true;
					return;
				}
			} break;
			case StreamedLexerCharType::Special:	{	// OR Switching to float parsing:
				if (this->CurrentCharacter == this->Options.FloatingPointDelimiter)	{	// Floating point delimeter found.
					this->CurrentState = StreamedLexerState::ReadingConstantFloat;
					this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_CONSTANT_FLOAT;
					this->CurrentLexem->ProcessedDataFloat = static_cast<Float64>(this->CurrentLexem->ProcessedDataInteger);
				} else {	// Some other special character upcoming, commiting token.
					this->RewindedCharacter = this->CurrentCharacter;
					this->WasLexemCommited = true;
					return;
				}
			} break;
			default: { // OR Throwing debug Assertation if internal error exists
				throw LogicException("Some strange things happened, unknown condition for ProcessIntegerConstant");
			}
		}
	}

	/// Processes floating-point constants inside input stream.
	void BasicStreamedLexerImpl::ProcessFloatConstant()
	{	// Nothing to do here.
	}

	/// Processes string constants inside input stream.
	void BasicStreamedLexerImpl::ProcessStringConstant()
	{	// Parsing string contants in basics mode: so no espace sequences.
		if (this->CurrentCharacter == Char('\"')) {
			this->WasLexemCommited = true;
		}
	}

	/// Processes character constants inside input stream.
	void BasicStreamedLexerImpl::ProcessCharacterConstant()
	{	// Parsing character contants in basics mode: so no espace sequences.
		if (this->CurrentCharacter == Char('\'')) {
			this->WasLexemCommited = true;
		}
	}

	/// Processes operators inside input stream and handles switching to multiple line comment parsing.
	void BasicStreamedLexerImpl::ProcessOperatorOrComment()
	{	
		if (   (this->Options.SingleLineCommentDeclaration[this->CurrentMatchingIndex] == this->CurrentCharacter)
			|| (this->Options.MultipleLineCommentBeginning[this->CurrentMatchingIndex] == this->CurrentCharacter)) {	
			this->CurrentMatchingIndex += 1;
			// Matching with single line comment ending.
			if (this->Options.SingleLineCommentDeclaration.GetLength() == this->CurrentMatchingIndex) {	// Final match, finalizing comment.
				this->CurrentMatchingIndex = 0;
				this->CurrentState = StreamedLexerState::ReadingCommentSingleLine;
				this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
				this->CurrentLexem->ProcessedDataID = 0;
				return;
			}

			// Matching with multiple line comment ending.
			if (this->Options.MultipleLineCommentBeginning.GetLength() == this->CurrentMatchingIndex)	{	// Final match, finalizing comment.
				this->CurrentMatchingIndex = 0;
				this->CurrentState = StreamedLexerState::ReadingCommentMultipleLine;
				this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
				this->CurrentLexem->ProcessedDataID = 0;
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
		if (this->CurrentCharacter == Char('\n')) {
			this->WasLexemCommited = true;
		}
	}

	/// Processes multiple line comments inside input stream.
	void BasicStreamedLexerImpl::ProcessMultipleLineComment()
	{
		if (this->Options.MultipleLineCommentEnding[this->CurrentMatchingIndex] == this->CurrentCharacter) {	// Matching with multiple line comment ending.
			this->CurrentMatchingIndex += 1;
			if (this->Options.MultipleLineCommentEnding.GetLength() == this->CurrentMatchingIndex) {	// Final match, finalizing comment.
				this->CurrentMatchingIndex = 0;
				this->WasLexemCommited = true;
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
		this->Reset();
		this->CurrentLexem = OutputLexem;
		this->CurrentLexem->ResetLexem();
		
		while (!this->WasLexemCommited) {
			this->ProcessUpcomingCharacter();
		}

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
		this->BasicStreamedLexerImpl::Reset();
		this->CurrentMatches			.Emptify();
		this->CurrentLexemFloatExponent	= 1;
		this->CurrentESMode				= StreamedLexerESMode::Unknown;
		this->CurrentlyWasCharWritten	= false;
	}

	/// Processes integer constants inside input stream.
	void StreamedLexerImpl::ProcessIntegerConstant()
	{
		this->BasicStreamedLexerImpl::ProcessIntegerConstant();
		if ((this->CurrentState == StreamedLexerState::ReadingConstantInteger) && (this->CurrentCharacterType == StreamedLexerCharType::Digit)) {	
			// Processing character if it is digit in current notation.
			UInt8 const CurrentDigit = CharAnsiTraits::ToDigit(this->CurrentCharacter);
			this->CurrentLexem->ProcessedDataInteger *= this->CurrentLexemIntegerNotation;
			this->CurrentLexem->ProcessedDataInteger += CurrentDigit;
		}
	}

	/// Processes floating-point constants inside input stream.
	void StreamedLexerImpl::ProcessFloatConstant()
	{	
		this->BasicStreamedLexerImpl::ProcessFloatConstant();
		UInt8 const CurrentDigit = CharAnsiTraits::ToDigit(this->CurrentCharacter);
		this->CurrentLexem->ProcessedDataFloat += (static_cast<Float64>(CurrentDigit) / static_cast<Float64>(this->CurrentLexemFloatExponent));
		this->CurrentLexemFloatExponent += 1;
	}

	/// Processes escape sequence in string or character constant.
	void StreamedLexerImpl::ProcessEscapeSequenceInStringOrCharacterConstant()
	{
		switch (this->CurrentESMode) {
			case StreamedLexerESMode::Unknown: {
				if (this->CurrentCharacter == CharAnsi('\\')) {
					this->CurrentESMode = StreamedLexerESMode::Waiting;
				}
			} break;

			case StreamedLexerESMode::Waiting: {
				this->CurrentESMode = StreamedLexerESMode::Unknown;
				this->CurrentlyWasCharWritten = true;
				switch (this->CurrentCharacter)	{	// For more details read here: http://en.wikipedia.org/wiki/Escape_sequences_in_C
					case CharAnsi('a'):	{	// Alarm (Beep, Bell)
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\a');
					} return;
					case CharAnsi('b'):	{	// Backspace
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\b');
					} return;
					case CharAnsi('f'):	{	// Formfeed
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\f');
					} return;
					case CharAnsi('n'):	{	// Newline (Line Feed)
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\n');
					} return;
					case CharAnsi('r'):	{	// Carriage Return
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\r');
					} return;
					case CharAnsi('t'):	{	// Horizontal Tab
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\t');
					} return;
					case CharAnsi('v'):	{	// Vertical Tab
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\v');
					} return;
					case CharAnsi('\\'): {	// Vertical Tab
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\\');
					} return;
					case CharAnsi('\''): {	// Single quotation mark
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\'');
					} return;
					case CharAnsi('\"'): {	// Double quotation mark
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\"');
					} return;
					case CharAnsi('\?'): {	// Question mark
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\?');
					} return;
					case CharAnsi('x'):	{	// The character whose numerical value is given by hh interpreted as a hexadecimal number
						this->CurrentESMode = StreamedLexerESMode::HexadecimalCharacter;
						this->CurrentlyWasCharWritten = false;
					} return;
					case CharAnsi('u'):	{	// Unicode characters in string literals (UTF8)
						this->CurrentESMode = StreamedLexerESMode::Unicode8;
						this->CurrentlyWasCharWritten = false;
					} return;
					case CharAnsi('U'):	{	// Unicode characters in string literals (UTF16)
						this->CurrentESMode = StreamedLexerESMode::Unicode16;
						this->CurrentlyWasCharWritten = false;
					} return;

					default: {
						if (!CharAnsiTraits::IsDigit(this->CurrentCharacter, 8)) {	// Unrecognized character escape sequence.
							StreamedLexerErrorDesc static const UnrecognizedCharacterEscapeSequenceError("unrecognized character escape sequence '\\%c' in string/character constant.");
							throw StreamedLexerFatalErrorException(UnrecognizedCharacterEscapeSequenceError.ToString(this->CurrentLexem, this->CurrentCharacter));
						}

						// The character whose numerical value is given by nnn interpreted as an octal number
						this->CurrentlyWasCharWritten = false;
						this->CurrentESMode = StreamedLexerESMode::OctalCharacter;
						this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\0');
					}
				} 
			} fallthrough;

			case StreamedLexerESMode::OctalCharacter:
			case StreamedLexerESMode::HexadecimalCharacter: {	
				UInt8 const CurrentNotation = ((this->CurrentESMode == StreamedLexerESMode::OctalCharacter) ? 8 : 16);
				if (!CharAnsiTraits::IsDigit(this->CurrentCharacter, CurrentNotation)) {	// Here is not a not a digit character.
					if ((this->CurrentLexem->GetRawData().GetLength() < 2) && (CurrentNotation == 16)) {	// Hex constants must have at least one hex digit (now raw data contains string "'\").
						StreamedLexerErrorDesc static const EmptyHexadecimalCharacterError("empty string/character constant with '\\x'");
						throw StreamedLexerFatalErrorException(EmptyHexadecimalCharacterError.ToString(this->CurrentLexem));
					}

					this->CurrentESMode = StreamedLexerESMode::Unknown;
					this->CurrentlyWasCharWritten = true;
					return;
				}

				// Processing next character
				UInt8 const CurrentDigit = CharAnsiTraits::ToDigit(this->CurrentCharacter);
				if ((static_cast<size_t>(this->CurrentLexem->ProcessedDataCharacter) * CurrentNotation + CurrentDigit) > UCHAR_MAX)	{	// Too big for character error.
					StreamedLexerErrorDesc static const TooBigNumberForCharacterError("too big number for character.");
					throw StreamedLexerFatalErrorException(TooBigNumberForCharacterError.ToString(this->CurrentLexem));
				}

				this->CurrentLexem->ProcessedDataCharacter *= CurrentNotation;
				this->CurrentLexem->ProcessedDataCharacter += CurrentDigit;
			} return;
		
			default: {
				throw NotImplementedException("Parsing this StreamedLexerImpl::CurrentESMode is not implemented");
			}
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
		this->ProcessEscapeSequenceInStringOrCharacterConstant();
		if (this->CurrentESMode == StreamedLexerESMode::Unknown) {	// We are sure that escape sequence does not exists or was succesfully parsed.
			if (this->CurrentCharacter == CharAnsi('\'')) {	// Here is unescaped "'" character.
				if (!this->CurrentlyWasCharWritten)	{	// Empty character constant error. (now raw data contains quote mark ("'") and parsed ).
					StreamedLexerErrorDesc static const EmptyCharacterConstantError("empty character constant.");
					throw StreamedLexerFatalErrorException(EmptyCharacterConstantError.ToString(this->CurrentLexem));
				}
			
				this->WasLexemCommited = true;
				return;
			}
			
			if (this->CurrentlyWasCharWritten) {	// Multiple characters inside character constant.
				StreamedLexerErrorDesc static const MultiCharacterCharacterConstantError("multiple characters in character constant %s%c'.");
				throw StreamedLexerFatalErrorException(MultiCharacterCharacterConstantError.ToString(this->CurrentLexem, this->CurrentLexem->GetRawData().CStr(), this->CurrentCharacter));
			}

			// Here is our character.
			this->CurrentLexem->ProcessedDataCharacter = this->CurrentCharacter;
			this->CurrentlyWasCharWritten = true;
		}
	}

	/// Processes operators inside input stream and handles switching to multiple line comment parsing.
	void StreamedLexerImpl::ProcessOperatorOrComment()
	{
		if (this->CurrentMatches.GetLength() == 0) {	// Filling CurrentMatches with pointers on operators and comments declarations
			this->CurrentMatchingIndex = 0;
			this->CurrentMatches.Reserve ( this->Options.OperatorDeclarations.GetLength() + 2);
			this->CurrentMatches.PushLast(&this->Options.SingleLineCommentDeclaration);
			this->CurrentMatches.PushLast(&this->Options.MultipleLineCommentBeginning);
			for (auto const& OperatorDeclaration : this->Options.OperatorDeclarations) {
				this->CurrentMatches.PushLast(&OperatorDeclaration.Second);
			}
		}

		bool HasAnyFullMatch = false;
		Vector<String const*> CurrentMatches(0, this->CurrentMatches.GetLength());
		for (auto const CurrentOperatorOrCommentMatch : this->CurrentMatches) {	
			if ((*CurrentOperatorOrCommentMatch)[this->CurrentMatchingIndex] == this->CurrentCharacter)	{	// Character matches some declaration at current index.
				if (CurrentOperatorOrCommentMatch->GetLength() == (1 + this->CurrentMatchingIndex)) {	// We are having a full match with some operator, so lets identify it now
					/**/ if (CurrentOperatorOrCommentMatch == &this->Options.SingleLineCommentDeclaration) {	// Declaration matches single line comment declaration.
						this->CurrentMatchingIndex = 0;
						this->CurrentState = StreamedLexerState::ReadingCommentSingleLine;
						this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
						this->CurrentLexem->ProcessedDataID = 0;
						return;
					} else if (CurrentOperatorOrCommentMatch == &this->Options.MultipleLineCommentBeginning) {	// Declaration matches multiple line comment declaration.
						this->CurrentMatchingIndex = 0;
						this->CurrentState = StreamedLexerState::ReadingCommentMultipleLine;
						this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_COMMENT;
						this->CurrentLexem->ProcessedDataID = 0;
						return;
					}

					// Declaration matches operator declaration.
					size_t const Index = this->Options.OperatorDeclarations.FindFirstElement([&](StreamedLexerOperatorDecl const& OperatorDecl) -> bool {
						return ((&OperatorDecl.Second) == CurrentOperatorOrCommentMatch);
					});
					this->CurrentLexem->ProcessedDataID = this->Options.OperatorDeclarations[Index].First;

					HasAnyFullMatch = true;
					continue;
				}
				
				CurrentMatches.PushLast(CurrentOperatorOrCommentMatch);
			}
		}

		if (CurrentMatches.GetLength() == 0) {	// No matches in this iteration. That means that currect operator ID was set in previos. 
			if (!HasAnyFullMatch) {	// If there is not full math then some character is breaking the sequence breaking. Rewinding it to handle later.
				this->RewindedCharacter = this->CurrentCharacter;
			}

			this->WasLexemCommited = true;
			return;
		}

		this->CurrentMatches = Move(CurrentMatches);
		this->CurrentMatchingIndex += 1;
	}

	/// Processes identifiers and keywords inside input stream.
	void StreamedLexerImpl::ProcessIdentifierOrKeyword()
	{
		if ((this->CurrentMatches.GetLength() == 0) && (this->CurrentMatchingIndex == 0))	{	// Filling CurrentMatches with pointers on operators and comments declarations
			this->CurrentMatches.Reserve(this->Options.KeywordsDeclarations.GetLength());
			for (auto const& KeywordsDeclarations : this->Options.KeywordsDeclarations) {
				this->CurrentMatches.PushLast(&KeywordsDeclarations.Second);
			}
		}

		bool HasAnyFullMatch = false;
		Vector<String const*> CurrentMatches(0, this->CurrentMatches.GetLength());
		for (auto const CurrentIdentifierMatch : this->CurrentMatches) {	
			if ((*CurrentIdentifierMatch)[this->CurrentMatchingIndex] == this->CurrentCharacter) {	// Character matches some declaration at current index
				if (CurrentIdentifierMatch->GetLength() == (1 + this->CurrentMatchingIndex)) {	// We are having a full match with some keyword, so lets identify it now.
					size_t const Index = this->Options.KeywordsDeclarations.FindFirstElement([&](StreamedLexerKeywordDecl const& KeywordDecl) -> bool {
						return ((&KeywordDecl.Second) == CurrentIdentifierMatch);
					});

					this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_KEYWORD;
					this->CurrentLexem->ProcessedDataID = this->Options.KeywordsDeclarations[Index].First;
					
					HasAnyFullMatch = true;
					continue;
				}

				CurrentMatches.PushLast(CurrentIdentifierMatch);
			}
		}

		if (!HasAnyFullMatch) {	// We are heving next chracter that makes this lexem valid identifier. Restoring identifer content type.
			this->CurrentLexem->ProcessedDataID = 0;
			this->CurrentLexem->ContentType = GD_LEXEM_CONTENT_TYPE_IDENTIFIER;
		}

		this->CurrentMatches = Move(CurrentMatches);
		this->CurrentMatchingIndex += 1;
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
	/// @param Stream   Input stream on which lexer would work.
	/// @param Options  Packed lexing options list.
	StreamedLexer::StreamedLexer(IToolchain* const Toolchain, UniquePtr<InputStream>&& Stream, StreamedLexerOptions const& Options, StreamedLexerMode const LexerMode /* = GD_LEXER_MODE_DEFAULT */)
		: IToolchainTool(Toolchain)
		, ImplementationMode(LexerMode)
		, Options(Options)
	{
		this->Implementation = ((this->ImplementationMode == StreamedLexerMode::Basic) 
			? new BasicStreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream))
			: new   StreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream)));
	}

	StreamedLexer::~StreamedLexer()
	{
	}

	/// Switches lexer features set.
	/// @param LexerMode Describes avaliable feature set of Streamed Lexer.
	void StreamedLexer::SwitchMode(StreamedLexerMode const LexerMode /* = GD_LEXER_MODE_DEFAULT */)
	{
		if (LexerMode != this->ImplementationMode) {	// Lexer mode chagned, we need to switch used implementation.
			GD_DEBUG_ASSERT(LexerMode < StreamedLexerMode::Count, "Invalid lexer mode specfied");
			this->ImplementationMode = LexerMode;
			this->Implementation = ((LexerMode == StreamedLexerMode::Basic)
				? new BasicStreamedLexerImpl(Move(*this->Implementation)) 
				: new   StreamedLexerImpl(Move(*this->Implementation)));
		}
	}

	/// Extracts next lexem from input stream into output. Throws an exception into toolchain if error accures. Can be ran in simple and advanced mode.
	/// More about modes: 
	/// @param OutputLexem Pointer to output lexem variable.
	/// @param LexerMode   Describes avaliable feature set of Streamed Lexer.
	/// @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
	bool StreamedLexer::GetNextLexem(Lexem* const OutputLexem)
	{	// We are using implementation class to prevent dozens of private fields in class.
		return this->Implementation->GetNextLexem(OutputLexem);
	}

	/// Exctracts next character from input stream and wraps line breaks, etc.
	/// @returns Exctracted character.
	CharAnsi StreamedLexer::GetNextCharacter()
	{	// We are using implementation class to prevent dozens of private fields in class.
		return this->Implementation->GetNextCharacter();
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
		StreamedLexerKeywordsList  && KeywordsDeclarations,
		StreamedLexerOperatorsList && OperatorDeclarations,
		String	     && SingleLineCommentDeclaration,
		String	     && MultipleLineCommentBeginning,
		String	     && MultipleLineCommentEnding,
		CharAnsi const    IntegerHexadecimalNotationDelimiter,
		CharAnsi const    IntegerOctalNotationDelimiter,
		CharAnsi const    IntegerBinaryNotationDelimiter,
		CharAnsi const    FloatingPointDelimiter
	)	: KeywordsDeclarations(Forward<StreamedLexerKeywordsList>(KeywordsDeclarations))
		, OperatorDeclarations(Forward<StreamedLexerOperatorsList>(OperatorDeclarations))
		, SingleLineCommentDeclaration(Forward<String>(SingleLineCommentDeclaration))
		, MultipleLineCommentBeginning(Forward<String>(MultipleLineCommentBeginning))
		, MultipleLineCommentEnding(Forward<String>(MultipleLineCommentEnding))
		, IntegerHexadecimalNotationDelimiter((IntegerHexadecimalNotationDelimiter != CharAnsi('\0')) ? CharAnsiTraits::ToUpper(IntegerHexadecimalNotationDelimiter) : CharAnsi('\0'))
		, IntegerOctalNotationDelimiter((IntegerOctalNotationDelimiter != CharAnsi('\0')) ? CharAnsiTraits::ToUpper(IntegerOctalNotationDelimiter) : CharAnsi('\0'))
		, IntegerBinaryNotationDelimiter((IntegerBinaryNotationDelimiter != CharAnsi('\0')) ? CharAnsiTraits::ToUpper(IntegerBinaryNotationDelimiter) : CharAnsi('\0'))
		, FloatingPointDelimiter(FloatingPointDelimiter)
	{
		// Checking if all delimiters are alphabetic characters.
		/// @todo Add check for all notation switching characters have unique or \0 values.
		GD_DEBUG_ASSERT(((this->IntegerHexadecimalNotationDelimiter == '\0') || CharAnsiTraits::IsAlphabetic(this->IntegerHexadecimalNotationDelimiter)),
			"'IntegerHexadecimalNotationDelimiter' should be alphabetic character. Recommended value is 'x'.");
		GD_DEBUG_ASSERT(((this->IntegerOctalNotationDelimiter == '\0') || CharAnsiTraits::IsAlphabetic(this->IntegerOctalNotationDelimiter)),
			"'IntegerOctalNotationDelimiter' should be alphabetic character. Recommended value is 'c'.");
		GD_DEBUG_ASSERT(((this->IntegerBinaryNotationDelimiter == '\0') || CharAnsiTraits::IsAlphabetic(this->IntegerBinaryNotationDelimiter)),
			"'IntegerBinaryNotationDelimiter' should be alphabetic character. Recommended value is 'b'.");

		// Validating value of floating pointer delimiter.
		// We should not forget to append floating point delimiter either it would be treated as alphabetic character.
		GD_DEBUG_ASSERT(((this->FloatingPointDelimiter == '\0') || CharAnsiTraits::IsSpecialCharacter(this->FloatingPointDelimiter)),
			"FloatingPointDelimiter should be special character. Recommended values are '.' or ','.");
		this->AppendToSpecialCharactersAlphabet(this->FloatingPointDelimiter);

		// Checking if all operators/comment declarators have unique values and extending alphabet.
		GD_DEBUG_ASSERT((this->SingleLineCommentDeclaration != this->MultipleLineCommentBeginning), "Multiple line comment declaration should not be equal to single line comment declaration");
		for (auto const Character : this->SingleLineCommentDeclaration) {	// Checking if single line comment contains only special characters and extending alphabet.
			GD_DEBUG_ASSERT((CharAnsiTraits::IsSpecialCharacter(Character)), "Single line comment string should contain only special characters");
			this->AppendToSpecialCharactersAlphabet(Character);
		}
		for (auto const Character : this->MultipleLineCommentBeginning) {	// Checking if multiple line comment contains only special characters and extending alphabet.
			GD_DEBUG_ASSERT((CharAnsiTraits::IsSpecialCharacter(Character)), "Multiple line comment string should contain only special characters");
			this->AppendToSpecialCharactersAlphabet(Character);
		}

		// Extending alphabet with characters from operator declarations.
		for (auto const& OperatorDeclaration : this->OperatorDeclarations) {	// Checking if none comment declarations is same to operator declaration.
			/// @todo Add check for all operators have unique values.
			String const& OperatorDeclarationString = OperatorDeclaration.Second;
			GD_DEBUG_ASSERT((this->SingleLineCommentDeclaration != OperatorDeclarationString), "Operator declaration should not be equal to single line comment declaration");
			GD_DEBUG_ASSERT((this->MultipleLineCommentBeginning != OperatorDeclarationString), "Operator declaration should not be equal to multiple line comment declaration");
			for (auto const Character : OperatorDeclarationString) {	// Checking if operator contains only special characters
				GD_DEBUG_ASSERT((CharAnsiTraits::IsSpecialCharacter(Character)), "Operator string should contain only special characters");
				this->AppendToSpecialCharactersAlphabet(Character);
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
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ASM,     "asm" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_AUTO,    "auto" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_BOOL,    "bool" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_BREAK,   "break" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CASE,    "case" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CATCH,   "catch" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CHAR,    "char" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CLASS,   "class" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST,   "const" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONSTEXPR,  "constexpr" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONST_CAST,    "const_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_CONTINUE,   "continue" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DECLTYPE,   "decltype" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DEFAULT,    "default" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DELETE,     "delete" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DO,      "do" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DOUBLE,     "double" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_DYNAMIC_CAST,  "dynamic_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ELSE,    "else" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_ENUM,    "enum" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXPLICIT,   "explicit" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXPORT,     "export" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_EXTERN,     "extern" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FALSE,   "false" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FINAL,   "final" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FINALLY,    "finally" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FOR,     "for" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_FRGDND,     "friend" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_GOTO,    "goto" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_IF,      "if" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_INLINE,     "inline" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_INT,     "int" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_LONG,    "long" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_MUTABLE,    "mutable" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NAMESPACE,  "namespace" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NEW,     "new" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NOEXPECT,   "noexpect" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_NULLPTR,    "nullptr" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_OPERATOR,   "operator" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PRIVATE,    "private" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PROTECTED,  "protected" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_PUBLIC,     "public" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_REGISTER,   "register" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_REINTERPRET_CAST, "reinterpet_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_RETURN,     "return" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SHORT,   "short" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIGNED,     "signed" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SIZEOF,     "sizeof" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC,     "static" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC_ASSERT, "static_assert" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STATIC_CAST,   "static_cast" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_STRUCT,     "struct" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_SWITCH,     "switch" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TEMPLATE,   "template" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THIS,    "this" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THREAD_LOCAL,  "thread_local" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_THROW,   "throw" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TRUE,    "true" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TRY,     "try" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPEDEF,    "typedef" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPEID,     "typeid" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_TYPENAME,   "typename" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNION,   "union" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_UNSIGNED,   "unsigned" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_USING,   "using" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VIRTUAL,    "virtual" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOID,    "void" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_VOLATILE,   "volatile" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_KEYWORD_WHILE,   "while" },
			}),
			StreamedLexerOperatorsList({
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ASSIGN,     "="   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ADD,        "+"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SUB,        "-"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL,        "*"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DIV,        "/"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MOD,        "%"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INC,        "++"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DEC,        "--"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_EQUALS,     "=="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_NOT_EQUALS,    "!="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER,       "<"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS,       ">"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_GREATER_EQUAL,    "<="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_LESS_EQUALS,      ">="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_NOT,        "!"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_AND,        "&&"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_OR,      "||"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_NOT,      "~"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND,      "&"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR,    "|"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_XOR,      "^"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_LEFT_SHIFT,  "<<"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT,    ">>"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_ADD_ASSIGN,    "+="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SUB_ASSIGN,    "-="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MUL_ASSIGN,    "*="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_DIV_ASSIGN,    "/="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MOD_ASSIGN,    "%="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_AND_ASSIGN,  "&="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_OR_ASSIGN,   "|="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_XOR_ASSIGN,  "^="  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_LEFT_SHIFT_ASSIGN, "<<=" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_BITWISE_RIGHT_SHIFT_ASSIGN,">>=" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_BEGIN,      "["   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_END,     "]"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN,      "{"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END,     "}"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN,     "("   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END,    ")"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_STRUCT_DEREFERENCING,   "->"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_STRUCT_REFERENCING,  "."   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MEMBER_PTR_DEREFERENCING,  "->*" },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_MEMBER_PTR_REFERENCING, ".*"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA,      ","   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON,      ":"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SEMICOLON,     ";"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TERNARY,       "?"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_RESOLUTION,    "::"  },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAM_PACK,    "..." },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR,     "#"   },
				{ GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR_GLUE,   "##"  },
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
