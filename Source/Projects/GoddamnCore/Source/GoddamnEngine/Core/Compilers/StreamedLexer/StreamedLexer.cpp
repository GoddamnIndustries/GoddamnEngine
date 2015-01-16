/// ==========================================================================================
/// StreamedLexer.cpp - tokenizer implementation
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================
/// 
///  ! ! !  P L E A S E  R E A D  T H I S  C A R E F U L L Y  ! ! !
/// Code below is the world's fastest streamed lexer.  But it is very
/// hard to debug it. I spent months creating/refactoring. Please, do
/// not try to modify without understanding what it really does.
/// This code can kill you. Seriously.
///
/// But if you need to touch this code: pray and let God be with you!
///                 
/// 
///                              +------+
///                             /      /|
///                            +------+ |
///                            |      | |
///                            |      | |
///                   +--------|      | +----------+
///                  /         |      |/          /|
///                 +----------+      +----------+ |
///                 |                            | /
///                 |                            |/
///                 +----------+      +----------+
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | |
///                            |      | /
///                            |______|/
/// 
///                                                             Amen.
///                                         (J. Jhuighuy, 01.04.2015)
/// 
/// 

#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Templates/Utility.h>

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
	class StreamedLexerImpl : public IToolchainTool
	{
	protected /*Class Types and members*/:
		enum class StreamedLexerState : UInt8
		{
			ReadingConstantString,
			ReadingConstantChar,
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

		enum class StreamedLexerESMode : UInt8
		{
			Waiting,
			OctalChar,
			HexadecimalChar,
			Unicode8,
			Unicode16,
			Unknown,
		};	// enum class StreamedLexerEscapeSequenceMode

		StreamedLexerOptions const& Options;
		UniquePtr<InputStream>		Stream;

		bool					 WasLexemCommited            = false;
		Lexem*                   CurrentLexem                = nullptr                       ; // Lexem state & info
		size_t                   CurrentLine                 = 1                             ; // Lexem location
		size_t                   CurrentSymbol               = 0                             ;
		CharAnsi				 RewindedChar                = CharAnsi('\0')                ;	
		StreamedLexerState       CurrentState                = StreamedLexerState::Unknown   ;
		CharAnsi                 CurrentChar                 = CharAnsi('\0')                ; // character meta
		StreamedLexerCharType    CurrentCharType             = StreamedLexerCharType::Unknown;
		size_t static const      DefaultLexemIntegerNotation = 10                            ; // Numeric meta
		size_t                   CurrentLexemIntegerNotation = DefaultLexemIntegerNotation   ;
		size_t                   CurrentMatchingIndex        = 0                             ; // Operators and comments meta
		size_t                   CurrentLexemFloatExponent   = 1                             ;
		Vector<String const*>    CurrentMatches                                              ;
		StreamedLexerESMode		 CurrentESMode               = StreamedLexerESMode::Unknown  ;
		bool					 CurrentlyWasCharWritten     = false                         ;

	public:
		GDINL StreamedLexerImpl(IToolchain* const Toolchain, StreamedLexerOptions const& Options, UniquePtr<InputStream>&& Stream)
			: IToolchainTool(Toolchain)
			, Stream(Forward<UniquePtr<InputStream>>(Stream))
			, Options(Options)
		{
		}

		GDINL StreamedLexerImpl(StreamedLexerImpl const& Other) = delete;
		GDINL StreamedLexerImpl(StreamedLexerImpl  && Other)
			: IToolchainTool(Other.Toolchain)
			, Stream(Forward<UniquePtr<InputStream>>(Other.Stream))
			, RewindedChar(Other.RewindedChar)
			, CurrentSymbol(Other.CurrentSymbol)
			, CurrentLine(Other.CurrentLine)
			, Options(Other.Options)
		{
		}

		GDINL virtual ~StreamedLexerImpl()
		{
		}

		/// @brief Reads next lexem from Input stream.
		/// @returns True if lexem was extracted successfully.
		GDINT bool GetNextLexem(Lexem* const OutputLexem)
		{	
			// Restoring default tokenizer values
			this->Reset();
			this->CurrentLexem = OutputLexem;
			this->CurrentLexem->ResetLexem();

			while (!this->WasLexemCommited) {
				this->ProcessUpcomingChar();
			}

			return true;
		}


	private:

		/// Resets lexer implementation.
		GDINT void Reset()
		{
			WasLexemCommited            = false                         ;
			CurrentLexem                = nullptr                       ; // Lexem state & info
			CurrentLine                 = 1                             ; // Lexem location
			CurrentSymbol               = 0                             ;
		//	RewindedChar                = CharAnsi('\0')                ;
			CurrentState                = StreamedLexerState::Unknown   ;
			CurrentChar                 = CharAnsi('\0')                ; // character meta
			CurrentCharType             = StreamedLexerCharType::Unknown;
			CurrentLexemIntegerNotation = DefaultLexemIntegerNotation   ;
			CurrentMatchingIndex        = 0                             ; // Operators and comments meta
			CurrentLexemFloatExponent   = 1                             ;
			CurrentMatches				.Emptify()						;
			CurrentESMode               = StreamedLexerESMode::Unknown  ;
			CurrentlyWasCharWritten     = false                         ;
		}

	public:

		/// Reads next character from input stream. 
		/// @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
		GDINT CharAnsi GetNextChar()
		{	
			// Testing if we have rewinded Char.
			if (this->RewindedChar == CharAnsi('\0')) {
				try	{	
					// Trying to read next Char:
					this->CurrentChar = this->Stream->Read();
					if (this->CurrentChar == CharAnsi('\r')) {	
						// Trying to handle '\r\n' sequence.
						CharAnsi NextChar = this->Stream->Read();
						if (NextChar != CharAnsi('\n')) {
							this->RewindedChar = NextChar;
						}

						// New line here.
						this->CurrentChar = CharAnsi('\n');
						this->CurrentSymbol = 0;
						this->CurrentLine += 1;
					} else if (this->CurrentChar == CharAnsi('\n')) {	
						// Handing line termination Chars.
						this->CurrentSymbol = 0;
						this->CurrentLine += 1;
					} else {	
						// Next character on current line.
						this->CurrentSymbol += 1;
					}
				} catch (IOException const&) {	
					// We have failed to read.
					this->CurrentChar = CharAnsi('\0');
				}
			} else {
				this->CurrentChar = this->RewindedChar;
				this->RewindedChar = CharAnsi('\0');
			}

			if (this->CurrentChar == ';') {
				return this->CurrentChar;
			}

			return this->CurrentChar;
		}

		/// Pushes character back to input stream/caches Char.
		/// @param character Char that would be reverted.
		GDINT void RevertChar(CharAnsi const Char)
		{
			if (Char == CharAnsi('\n')) {	// Reverting line break.
				this->CurrentSymbol = SIZE_MAX;
				this->CurrentLine -= 1;
			} else {	// Reverting just last Char.
				this->CurrentSymbol -= 1;
			}

			if (this->RewindedChar == CharAnsi('\0')) {
				this->RewindedChar = Char;
			} else {
				this->Stream->Seek(-1, SeekOrigin::End);
			}
		}

		/// Processes single character from input stream.
		GDINT void ProcessUpcomingChar()
		{	
			// Reading next character from stream and determining the type of new character and possible type switch.
			StreamedLexerCharType const PrevCharType = this->CurrentCharType;
			this->GetNextChar();
			if (this->Options.IsInSpecialCharactersAlphabet(this->CurrentChar)) {
				this->CurrentCharType = StreamedLexerCharType::Special;
			} else if (CharAnsiTraits::IsAlphabetic(this->CurrentChar) || CharAnsiTraits::IsSpecialCharacter(this->CurrentChar)) {
				this->CurrentCharType = StreamedLexerCharType::Alphabetic;
			} else if (CharAnsiTraits::IsDigit(this->CurrentChar, this->CurrentLexemIntegerNotation)) {
				this->CurrentCharType = StreamedLexerCharType::Digit;
			} else if (CharAnsiTraits::IsSpace(this->CurrentChar)) {
				this->CurrentCharType = StreamedLexerCharType::Space;
			} else {
				this->CurrentCharType = StreamedLexerCharType::Unknown;
			}
			if ((this->CurrentCharType == StreamedLexerCharType::Space) && (PrevCharType == StreamedLexerCharType::Unknown)) {
				// Upcoming space that splits lexemes. Just skipping it.
				this->CurrentCharType = StreamedLexerCharType::Unknown;
				return;
			} else if ((this->CurrentCharType == StreamedLexerCharType::Unknown) && (this->CurrentChar != CharAnsi('\0'))) {
				// Unknown character exists in stream (except \0 Char). Error:
				StreamedLexerErrorDesc static const InvalidCharParsingError("invalid character with code 0x%x was read from input stream.");
				throw StreamedLexerFatalErrorException(InvalidCharParsingError.ToString(this->CurrentLexem, static_cast<size_t>(this->CurrentChar)));
			}

			// Now lets handle the type of read Char. Here we have a table that describes if we need to check character type switching
			// or ignore switching in several cases. Building a translation table: 
			struct TestCharTypesSwitchingTable final { bool DoTestCharTypeSwitch; StreamedLexerCharType NotCommitOnSwitchingTo[CharTypeCount + 1]; };
			static TestCharTypesSwitchingTable const TestCharTypeSwitchingTable[LexemContentTypesCount + 1] = {
				/* LexemContentType::Identifier         */{ true , { StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown                                 } },
				/* LexemContentType::Keyword            */{ true , { StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown                                 } },
				/* LexemContentType::Operator           */{ true , { StreamedLexerCharType::Special, StreamedLexerCharType::Unknown                                                                  } },
				/* LexemContentType::Comment            */{ false, { StreamedLexerCharType::Unknown,                                                                                                 } },
				/* LexemContentType::ConstantString     */{ false, { StreamedLexerCharType::Unknown,                                                                                                 } },
				/* LexemContentType::ConstantCharacter	*/{ false, { StreamedLexerCharType::Unknown,                                                                                                 } },
				/* LexemContentType::ConstantInteger    */{ true , { StreamedLexerCharType::Digit, StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Special, StreamedLexerCharType::Unknown } },
				/* LexemContentType::ConstantFloat      */{ true , { StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown                                                                    } },
				/* LexemContentType::Unknown            */{ true , { StreamedLexerCharType::Space, StreamedLexerCharType::Unknown                                                                    } }
			};

			TestCharTypesSwitchingTable const& SwitchingTable = TestCharTypeSwitchingTable[static_cast<size_t>(this->CurrentLexem->ContentType)];
			bool const CharTypeChanged = (PrevCharType != this->CurrentCharType);
			bool const HasPrevCharType = (PrevCharType != StreamedLexerCharType::Unknown);
			if (SwitchingTable.DoTestCharTypeSwitch && CharTypeChanged && HasPrevCharType) {
				// Parsing all conditions where token committing is required.
				// Attempting to switch state. Skipping if Prev type is unknown - tokenizer was launched for the first time 
				StreamedLexerCharType const* NotSwitchOnTypePtr = &SwitchingTable.NotCommitOnSwitchingTo[0];
				for (; (*NotSwitchOnTypePtr) != StreamedLexerCharType::Unknown; NotSwitchOnTypePtr += 1) {
					// Testing if current token is required to commit
					if ((*NotSwitchOnTypePtr) == this->CurrentCharType) {
						// Ignoring the commit and continuing processing
						break;
					}
				}

				// Rewinding character that caused character type switching and committing current Char
				if ((*NotSwitchOnTypePtr) == StreamedLexerCharType::Unknown) {
					this->RewindedChar = this->CurrentChar;
					this->WasLexemCommited = true;
					return;
				}
			}

			struct LexemStatesDesc final { bool RequiresClosingStatement; void (StreamedLexerImpl::* SpecialProcessorPtr)(); };
			static LexemStatesDesc const LexemStatesDescTable[LexerStatesCount] = {
				/* StreamedLexerState::ReadingConstantString	  */ { true,  &StreamedLexerImpl::ProcessStringConstant },
				/* StreamedLexerState::ReadingConstantChar        */ { true,  &StreamedLexerImpl::ProcessCharConstant },
				/* StreamedLexerState::ReadingConstantInteger	  */ { false, &StreamedLexerImpl::ProcessIntegerConstant },
				/* StreamedLexerState::ReadingConstantFloat	      */ { false, &StreamedLexerImpl::ProcessFloatConstant },
				/* StreamedLexerState::ReadingCommentOrOperator   */ { false, &StreamedLexerImpl::ProcessOperatorOrComment },
				/* StreamedLexerState::ReadingCommentSingleLine   */ { false, &StreamedLexerImpl::ProcessSingleLineComment },
				/* StreamedLexerState::ReadingCommentMultipleLine */ { true,  &StreamedLexerImpl::ProcessMultipleLineComment },
				/* StreamedLexerState::ReadingIdentifierOrKeyword */ { false, &StreamedLexerImpl::ProcessIdentifierOrKeyword },
			};

			bool const HasCurrentChar = (this->CurrentChar != CharAnsi('\0'));
			bool const CurrentStateUnknown = (this->CurrentState == StreamedLexerState::Unknown);
			if (HasCurrentChar && CurrentStateUnknown) {
				// Here is first real character of lexem.
				this->CurrentLexem->Line = this->CurrentLine;
				this->CurrentLexem->Symbol = this->CurrentSymbol;

				// Attempting to detect new state after prev commit 
				switch (this->CurrentChar)	{	
					// Detecting opening quote types:
					case CharAnsi('\"'): {
						this->CurrentState = StreamedLexerState::ReadingConstantString;
						this->CurrentLexem->ContentType = LexemContentType::ConstantString;
						//this->CurrentLexem->RawData += CharAnsi('\"');
					} break;

					case CharAnsi('\''): {
						this->CurrentState = StreamedLexerState::ReadingConstantChar;
						this->CurrentLexem->ContentType = LexemContentType::ConstantCharacter;
						//this->CurrentLexem->RawData += CharAnsi('\'');
					} break;

					default: {
						switch (this->CurrentCharType) {	
							// Parsing other character types:
							case StreamedLexerCharType::Space: {
							} return;

							case StreamedLexerCharType::Digit: {
								this->CurrentState = StreamedLexerState::ReadingConstantInteger;
								this->CurrentLexem->ContentType = LexemContentType::ConstantInteger;
							} break;

							case StreamedLexerCharType::Alphabetic: {
								this->CurrentState = StreamedLexerState::ReadingIdentifierOrKeyword;
								this->CurrentLexem->ContentType = LexemContentType::Identifier;
							} break;

							case StreamedLexerCharType::Special: {
								this->CurrentState = StreamedLexerState::ReadingCommentOrOperator;
								this->CurrentLexem->ContentType = LexemContentType::Operator;
							} break;

							default: {
								throw LogicException("Internal error: invalid CurrentCharType");
							}
						}
					} break;
				}
			} else if (!HasCurrentChar) {
				// End of stream reached.
				if (LexemStatesDescTable[static_cast<size_t>(this->CurrentState)].RequiresClosingStatement) {	
					// End of stream reached, but we were parsing token that requires terminator. Error:
					StreamedLexerErrorDesc static const UnexpectedEndOfStream("unexpected end of stream while parsing %s.");
					throw StreamedLexerFatalErrorException(UnexpectedEndOfStream.ToString(this->CurrentLexem, LexemContentTypeToString(this->CurrentLexem->ContentType)));
				}

				this->CurrentLexem->ContentType = LexemContentType::EndOfStream;
				this->WasLexemCommited = true;
				return;
			}
			
			// Parsing current Char
			// this->CurrentState cannot be greater equal to unknown state
			(this->*(LexemStatesDescTable[static_cast<size_t>(this->CurrentState)].SpecialProcessorPtr))();
			if (this->RewindedChar == CharAnsi('\0')) {
				this->CurrentLexem->RawData += this->CurrentChar;
			}
		}

		/// Processes integer constants inside input stream.
		GDINT void ProcessIntegerConstant()
		{
			switch (this->CurrentCharType) {
				// Processing character if it is digit in current notation.
				case StreamedLexerCharType::Digit: {
					UInt8 const CurrentDigit = CharAnsiTraits::ToDigit(this->CurrentChar);
					this->CurrentLexem->ProcessedDataInteger *= this->CurrentLexemIntegerNotation;
					this->CurrentLexem->ProcessedDataInteger += CurrentDigit;
				} break;

				// Switching notation:
				case StreamedLexerCharType::Alphabetic: {
					bool const HasAnyData = (this->CurrentLexem->ProcessedDataInteger == 0);
					bool const IsDefaultNotation = (this->CurrentLexemIntegerNotation == StreamedLexerImpl::DefaultLexemIntegerNotation);
					
					// Checking if notation switching is provided in format "0x...":
					bool WasNotationSwitched = (HasAnyData && IsDefaultNotation);
					if (WasNotationSwitched) {
						CharAnsi const UpperCaseChar = CharAnsiTraits::ToUpper(this->CurrentChar);
						if (UpperCaseChar == Options.IntegerHexadecimalNotationDelimiter) {
							this->CurrentLexemIntegerNotation = 16;
						} else if (UpperCaseChar == Options.IntegerOctalNotationDelimiter) {
							this->CurrentLexemIntegerNotation = 8;
						} else if (UpperCaseChar == Options.IntegerBinaryNotationDelimiter) {
							this->CurrentLexemIntegerNotation = 2;
						} else {
							WasNotationSwitched = false;
						}
					}

					// Found, that prefix does not matches template or it does not follows '0' Char
					if (!WasNotationSwitched) {
						this->RewindedChar = this->CurrentChar;
						this->WasLexemCommited = true;
						return;
					} 
				} break;

				// OR Switching to float parsing:
				case StreamedLexerCharType::Special: {
					if (this->CurrentChar == this->Options.FloatingPointDelimiter)	{	// Floating point delimiter found.
						this->CurrentState = StreamedLexerState::ReadingConstantFloat;
						this->CurrentLexem->ContentType = LexemContentType::ConstantFloat;
						this->CurrentLexem->ProcessedDataFloat = static_cast<Float64>(this->CurrentLexem->ProcessedDataInteger);
					} else {	// Some other special character upcoming, committing token.
						this->RewindedChar = this->CurrentChar;
						this->WasLexemCommited = true;
						return;
					}
				} break;

				// OR Throwing debug Assertation if internal error exists
				default: {
					throw LogicException("Some strange things happened, unknown condition for ProcessIntegerConstant");
				}
			}
		}

		/// Processes floating-point constants inside input stream.
		GDINT void ProcessFloatConstant()
		{
			UInt8 const CurrentDigit = CharAnsiTraits::ToDigit(this->CurrentChar);
			this->CurrentLexem->ProcessedDataFloat += (static_cast<Float64>(CurrentDigit) / static_cast<Float64>(this->CurrentLexemFloatExponent));
			this->CurrentLexemFloatExponent += 1;
		}

		/// Processes escape sequence in string or character constant.
		GDINT void ProcessEscapeSequenceInStringOrCharConstant()
		{
			switch (this->CurrentESMode) {
				case StreamedLexerESMode::Unknown: {
					if (this->CurrentChar == CharAnsi('\\')) {
						this->CurrentESMode = StreamedLexerESMode::Waiting;
					}
				} break;

				case StreamedLexerESMode::Waiting: {
					this->CurrentESMode = StreamedLexerESMode::Unknown;
					this->CurrentlyWasCharWritten = true;
					switch (this->CurrentChar)	{	// For more details read here: http://en.wikipedia.org/wiki/Escape_sequences_in_C
						case CharAnsi('a'):	{	// Alarm (Beep, Bell)
							this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\a');
						} return;
						case CharAnsi('b'):	{	// Backspace
							this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\b');
						} return;
						case CharAnsi('f'):	{	// Form-feed
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
							this->CurrentESMode = StreamedLexerESMode::HexadecimalChar;
							this->CurrentlyWasCharWritten = false;
						} return;
						case CharAnsi('u'):	{	// Unicode Chars in string literals (UTF8)
							this->CurrentESMode = StreamedLexerESMode::Unicode8;
							this->CurrentlyWasCharWritten = false;
						} return;
						case CharAnsi('U'):	{	// Unicode Chars in string literals (UTF16)
							this->CurrentESMode = StreamedLexerESMode::Unicode16;
							this->CurrentlyWasCharWritten = false;
						} return;

						default: {
							if (!CharAnsiTraits::IsDigit(this->CurrentChar, 8)) {	// Unrecognized character escape sequence.
								StreamedLexerErrorDesc static const UnrecognizedCharEscapeSequenceError("unrecognized character escape sequence '\\%c' in string/Char constant.");
								throw StreamedLexerFatalErrorException(UnrecognizedCharEscapeSequenceError.ToString(this->CurrentLexem, this->CurrentChar));
							}

							// The character whose numerical value is given by nnn interpreted as an octal number
							this->CurrentlyWasCharWritten = false;
							this->CurrentESMode = StreamedLexerESMode::OctalChar;
							this->CurrentLexem->ProcessedDataCharacter = CharAnsi('\0');
						}
					}
				} fallthrough;

				case StreamedLexerESMode::OctalChar:
				case StreamedLexerESMode::HexadecimalChar: {
					UInt8 const CurrentNotation = ((this->CurrentESMode == StreamedLexerESMode::OctalChar) ? 8 : 16);
					if (!CharAnsiTraits::IsDigit(this->CurrentChar, CurrentNotation)) {	// Here is not a not a digit Char.
						if ((this->CurrentLexem->GetRawData().GetLength() < 2) && (CurrentNotation == 16)) {	// Hex constants must have at least one hex digit (now raw data contains string "'\").
							StreamedLexerErrorDesc static const EmptyHexadecimalCharError("empty string/character constant with '\\x'");
							throw StreamedLexerFatalErrorException(EmptyHexadecimalCharError.ToString(this->CurrentLexem));
						}

						this->CurrentESMode = StreamedLexerESMode::Unknown;
						this->CurrentlyWasCharWritten = true;
						return;
					}

					// Processing next Char
					UInt8 const CurrentDigit = CharAnsiTraits::ToDigit(this->CurrentChar);
					if ((static_cast<size_t>(this->CurrentLexem->ProcessedDataCharacter) * CurrentNotation + CurrentDigit) > UCHAR_MAX)	{	// Too big for character error.
						StreamedLexerErrorDesc static const TooBigNumberForCharError("too big number for Char.");
						throw StreamedLexerFatalErrorException(TooBigNumberForCharError.ToString(this->CurrentLexem));
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
		GDINT void ProcessStringConstant()
		{
			// Parsing string constants in basics mode: so no escape sequences.
			if (this->CurrentChar == Char('\"')) {
				this->WasLexemCommited = true;
			}
		}

		/// Processes character constants inside input stream.
		GDINT void ProcessCharConstant()
		{
			this->ProcessEscapeSequenceInStringOrCharConstant();
			if (this->CurrentESMode == StreamedLexerESMode::Unknown) {	
				// We are sure that escape sequence does not exists or was successfully parsed.
				if (this->CurrentChar == CharAnsi('\'')) {	
					// Here is unescaped "'" Char.
					if (!this->CurrentlyWasCharWritten)	{	
						// Empty character constant error. (now raw data contains quote mark ("'") and parsed ).
						StreamedLexerErrorDesc static const EmptyCharConstantError("empty character constant.");
						throw StreamedLexerFatalErrorException(EmptyCharConstantError.ToString(this->CurrentLexem));
					}

					this->WasLexemCommited = true;
					return;
				}

				if (this->CurrentlyWasCharWritten) {	// Multiple Chars inside character constant.
					StreamedLexerErrorDesc static const MultiCharCharConstantError("multiple Chars in character constant %s%c'.");
					throw StreamedLexerFatalErrorException(MultiCharCharConstantError.ToString(this->CurrentLexem, this->CurrentLexem->GetRawData().CStr(), this->CurrentChar));
				}

				// Here is our Char.
				this->CurrentLexem->ProcessedDataCharacter = this->CurrentChar;
				this->CurrentlyWasCharWritten = true;
			}
		}

		/// Processes operators inside input stream and handles switching to multiple line comment parsing.
		GDINT void ProcessOperatorOrComment()
		{
			// Filling CurrentMatches with pointers on operators and comments declarations
			if (this->CurrentMatches.GetLength() == 0) {
				this->CurrentMatchingIndex = 0;
				this->CurrentMatches.Reserve(this->Options.OperatorDeclarations.GetLength() + 2);
				this->CurrentMatches.PushLast(&this->Options.SingleLineCommentDeclaration);
				this->CurrentMatches.PushLast(&this->Options.MultipleLineCommentBeginning);
				for (auto const& OperatorDeclaration : this->Options.OperatorDeclarations) {
					this->CurrentMatches.PushLast(&OperatorDeclaration.Second);
				}
			}

			bool HasAnyFullMatch = false;
			Vector<String const*> CurrentMatches(0, this->CurrentMatches.GetLength());
			for (auto const CurrentOperatorOrCommentMatch : this->CurrentMatches) {
				if ((*CurrentOperatorOrCommentMatch)[this->CurrentMatchingIndex] == this->CurrentChar)	{	
					// character matches some declaration at current index.
					if (CurrentOperatorOrCommentMatch->GetLength() == (1 + this->CurrentMatchingIndex)) {	
						// We are having a full match with some operator, so lets identify it now
						/**/ if (CurrentOperatorOrCommentMatch == &this->Options.SingleLineCommentDeclaration) {	
							// Declaration matches single line comment declaration.
							this->CurrentMatchingIndex = 0;
							this->CurrentState = StreamedLexerState::ReadingCommentSingleLine;
							this->CurrentLexem->ContentType = LexemContentType::Comment;
							this->CurrentLexem->ProcessedDataID = 0;
							return;
						} else if (CurrentOperatorOrCommentMatch == &this->Options.MultipleLineCommentBeginning) {	
							// Declaration matches multiple line comment declaration.
							this->CurrentMatchingIndex = 0;
							this->CurrentState = StreamedLexerState::ReadingCommentMultipleLine;
							this->CurrentLexem->ContentType = LexemContentType::Comment;
							this->CurrentLexem->ProcessedDataID = 0;
							return;
						} else {
							// Declaration matches operator declaration.
							size_t const Index = this->Options.OperatorDeclarations.FindFirstElement([&](StreamedLexerOperatorDecl const& OperatorDecl) -> bool {
								return ((&OperatorDecl.Second) == CurrentOperatorOrCommentMatch);
							});
							this->CurrentLexem->ProcessedDataID = this->Options.OperatorDeclarations[Index].First;

							HasAnyFullMatch = true;
							continue;
						}
					}

					CurrentMatches.PushLast(CurrentOperatorOrCommentMatch);
				}
			}

			// No matches in this iteration. That means that current operator ID was set in Prev. 
			if (CurrentMatches.GetLength() == 0) {
				// If there is not full math then some character is breaking the sequence breaking. Rewinding it to handle later.
				if (!HasAnyFullMatch) {
					this->RewindedChar = this->CurrentChar;
				}

				this->WasLexemCommited = true;
				return;
			}

			this->CurrentMatches = Move(CurrentMatches);
			this->CurrentMatchingIndex += 1;
		}

		/// Processes identifiers and keywords inside input stream.
		GDINT void ProcessIdentifierOrKeyword()
		{
			// Filling CurrentMatches with pointers on operators and comments declarations
			if ((this->CurrentMatches.GetLength() == 0) && (this->CurrentMatchingIndex == 0))	{
				this->CurrentMatches.Reserve(this->Options.KeywordsDeclarations.GetLength());
				for (auto const& KeywordsDeclarations : this->Options.KeywordsDeclarations) {
					this->CurrentMatches.PushLast(&KeywordsDeclarations.Second);
				}
			}

			bool HasAnyFullMatch = false;
			Vector<String const*> CurrentMatches(0, this->CurrentMatches.GetLength());
			for (auto const CurrentIdentifierMatch : this->CurrentMatches) {
				if ((*CurrentIdentifierMatch)[this->CurrentMatchingIndex] == this->CurrentChar) {	
					// character matches some declaration at current index
					if (CurrentIdentifierMatch->GetLength() == (1 + this->CurrentMatchingIndex)) {	
						// We are having a full match with some keyword, so lets identify it now.
						size_t const Index = this->Options.KeywordsDeclarations.FindFirstElement([&](StreamedLexerKeywordDecl const& KeywordDecl) -> bool {
							return ((&KeywordDecl.Second) == CurrentIdentifierMatch);
						});

						this->CurrentLexem->ContentType = LexemContentType::Keyword;
						this->CurrentLexem->ProcessedDataID = this->Options.KeywordsDeclarations[Index].First;

						HasAnyFullMatch = true;
						continue;
					}

					CurrentMatches.PushLast(CurrentIdentifierMatch);
				}
			}

			// We are having next character that makes this lexem valid identifier. Restoring identifier content type.
			if (!HasAnyFullMatch) {
				this->CurrentLexem->ProcessedDataID = 0;
				this->CurrentLexem->ContentType = LexemContentType::Identifier;
			}

			this->CurrentMatches = Move(CurrentMatches);
			this->CurrentMatchingIndex += 1;
		}

		/// Processes single line comments inside input stream.
		GDINL void ProcessSingleLineComment()
		{
			if (this->CurrentChar == Char('\n')) {
				this->WasLexemCommited = true;
			}
		}

		/// Processes multiple line comments inside input stream.
		GDINT void ProcessMultipleLineComment()
		{
			if (this->Options.MultipleLineCommentEnding[this->CurrentMatchingIndex] == this->CurrentChar) {	// Matching with multiple line comment ending.
				this->CurrentMatchingIndex += 1;
				if (this->Options.MultipleLineCommentEnding.GetLength() == this->CurrentMatchingIndex) {	// Final match, finalizing comment.
					this->CurrentMatchingIndex = 0;
					this->WasLexemCommited = true;
					return;
				}
			}
		}
	};	// class BasicStreamedLexerImpl

	/// ==========================================================================================
	/// StreamedLexer class.
	/// ==========================================================================================
	
	/// Initializes new streamed lexer.
	/// @param Toolchain   Corresponding toolchain.
	/// @param Stream   Input stream on which lexer would work.
	/// @param Options  Packed lexing options list.
	StreamedLexer::StreamedLexer(IToolchain* const Toolchain, UniquePtr<InputStream>&& Stream, StreamedLexerOptions const& Options)
		: IToolchainTool(Toolchain)
		, Options(Options)
	{
		this->Implementation = new StreamedLexerImpl(Toolchain, Options, Forward<UniquePtr<InputStream>>(Stream));
	}

	StreamedLexer::~StreamedLexer()
	{
	}

	/// Extracts next lexem from input stream into output. Throws an exception into toolchain if error occurs. Can be ran in simple and advanced mode.
	/// More about modes: 
	/// @param OutputLexem Pointer to output lexem variable.
	/// @param LexerMode   Describes available feature set of Streamed Lexer.
	/// @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
	bool StreamedLexer::GetNextLexem(Lexem* const OutputLexem)
	{	// We are using implementation class to prevent dozens of private fields in class.
		return this->Implementation->GetNextLexem(OutputLexem);
	}

	/// Extracts next character from input stream and wraps line breaks, etc.
	/// @returns Extracted Char.
	CharAnsi StreamedLexer::GetNextCharacter()
	{	// We are using implementation class to prevent dozens of private fields in class.
		return this->Implementation->GetNextChar();
	}

	/// Pushes specified character back to lexer and make it parsable.
	/// @param character Char that would be reverted.
	void StreamedLexer::RevertCharacter(CharAnsi const Char)
	{	// We are using implementation class to prevent dozens of private fields in class.
		return Implementation->RevertChar(Char);
	}

GD_NAMESPACE_END
