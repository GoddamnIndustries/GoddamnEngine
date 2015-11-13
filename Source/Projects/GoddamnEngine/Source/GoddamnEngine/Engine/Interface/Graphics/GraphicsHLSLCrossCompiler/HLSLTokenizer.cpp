//! ==========================================================================================
//! StreamedLexer.cpp - tokenizer implementation
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! ==========================================================================================
//! 
//!  ! ! !  P L E A S E  R E A D  T H I S  C A R E F U L L Y  ! ! !
//! Code below is the world's fastest streamed lexer.  But it is very
//! hard to debug it. I spent months creating/refactoring. Please, do
//! not try to modify code without understanding what it really does.
//! This code can kill you. Seriously.
//!
//! But if you need to touch this code: pray and let God be with you!
//!                 
//! 
//!                              +------+
//!                             /      /|
//!                            +------+ |
//!                            |      | |
//!                            |      | |
//!                   +--------|      | +----------+
//!                  /         |      |/          /|
//!                 +----------+      +----------+ |
//!                 |                            | /
//!                 |                            |/
//!                 +----------+      +----------+
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | |
//!                            |      | /
//!                            |______|/
//! 
//!                                                             Amen.
//!                                         (J. Jhuighuy, 01.04.2015)
//! 
//! 

#include "HLSLTokenizer.h"
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/IO/IO.h>

GD_NAMESPACE_BEGIN
	
	//! Fatal parsing error exception.
	typedef ToolchainException StreamedLexerFatalErrorException;

	//! Implementation of a basic streaming lexer. 
	class StreamedLexerImpl
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
		}; // enum class StreamedLexerState
		SizeTp static const LexerStatesCount = static_cast<SizeTp>(StreamedLexerState::Unknown);

		enum class StreamedLexerCharType : UInt8
		{
			Space,
			Digit,
			Special,
			Alphabetic,
			Unknown,
		}; // enum class StreamedLexerCharType
		SizeTp static const CharTypeCount = static_cast<SizeTp>(StreamedLexerCharType::Unknown);

		enum class StreamedLexerESMode : UInt8
		{
			Waiting,
			OctalChar,
			HexadecimalChar,
			Unicode8,
			Unicode16,
			Unknown,
		}; // enum class StreamedLexerEscapeSequenceMode

		StreamedLexerOptions const& Options;
		UniquePtr<InputStream> Stream;

		bool WasLexemCommited = false;
		Lexem* CurrentLexem = nullptr; // Lexem state & info
		SizeTp CurrentLine = 1; // Lexem location
		SizeTp CurrentSymbol = 0;
		Char RewindedChar = Char('\0');
		StreamedLexerState CurrentState = StreamedLexerState::Unknown;
		Char CurrentChar = Char('\0'); // character meta
		StreamedLexerCharType CurrentCharType = StreamedLexerCharType::Unknown;
		SizeTp static const DefaultLexemIntegerNotation = 10; // Numeric meta
		SizeTp CurrentLexemIntegerNotation = DefaultLexemIntegerNotation;
		SizeTp CurrentMatchingIndex = 0; // Operators and comments meta
		SizeTp CurrentLexemFloatExponent = 1;
		Vector<String const*> CurrentMatches;
		StreamedLexerESMode CurrentESMode = StreamedLexerESMode::Unknown;
		bool CurrentlyWasCharWritten = false;

	public:
		GDINL StreamedLexerImpl(StreamedLexerOptions const& Options, InputStream* Stream)
			: Options(Options), Stream(Stream)
		{
		}

		GDINL StreamedLexerImpl(StreamedLexerImpl const& Other) = delete;
		GDINL StreamedLexerImpl(StreamedLexerImpl&& Other)
			: Options(Other.Options), Stream(Forward<UniquePtr<InputStream>>(Other.Stream)), CurrentLine(Other.CurrentLine)
			, CurrentSymbol(Other.CurrentSymbol), RewindedChar(Other.RewindedChar)
		{
		}

		GDINL virtual ~StreamedLexerImpl()
		{
		}

		//! @brief Reads next lexem from input stream.
		//! @returns True if lexem was extracted successfully.
		GDINT bool GetNextLexem(Lexem* const OutputLexem)
		{
			// Restoring default tokenizer values
			Reset();
			CurrentLexem = OutputLexem;
			CurrentLexem->ResetLexem();

			while (!WasLexemCommited)
			{
				ProcessUpcomingChar();
			}

			return true;
		}


	private:

		//! Resets lexer implementation.
		GDINT void Reset()
		{
			WasLexemCommited = false ;
			CurrentLexem = nullptr ; // Lexem state & info
			CurrentLine = 1 ; // Lexem location
			CurrentSymbol = 0 ;
			//	RewindedChar                = Char('\0')                ;
			CurrentState = StreamedLexerState::Unknown ;
			CurrentChar = Char('\0') ; // character meta
			CurrentCharType = StreamedLexerCharType::Unknown;
			CurrentLexemIntegerNotation = DefaultLexemIntegerNotation ;
			CurrentMatchingIndex = 0 ; // Operators and comments meta
			CurrentLexemFloatExponent = 1 ;
			CurrentMatches.Emptify() ;
			CurrentESMode = StreamedLexerESMode::Unknown ;
			CurrentlyWasCharWritten = false ;
		}

	public:

		//! Reads next character from input stream. 
		//! @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
		GDINT Char GetNextChar()
		{
			// Testing if we have rewinded Char.
			if (RewindedChar == Char('\0'))
			{
				try
				{
					// Trying to read next Char:
					CurrentChar = (Char)Stream->Read();
					if (CurrentChar == Char('\r'))
					{
						// Trying to Handle '\r\n' sequence.
						Char NextChar = (Char)Stream->Read();
						if (NextChar != Char('\n'))
						{
							RewindedChar = NextChar;
						}

						// New line here.
						CurrentChar = Char('\n');
						CurrentSymbol = 0;
						CurrentLine += 1;
					}
					else if (CurrentChar == Char('\n'))
					{
						// Handing line termination Chars.
						CurrentSymbol = 0;
						CurrentLine += 1;
					}
					else
					{
						// Next character on current line.
						CurrentSymbol += 1;
					}
				} catch (IOException const&)
				{
					// We have failed to read.
					CurrentChar = Char('\0');
				}
			}
			else
			{
				CurrentChar = RewindedChar;
				RewindedChar = Char('\0');
			}

			if (CurrentChar == ';')
			{
				return CurrentChar;
			}

			return CurrentChar;
		}

		//! Pushes character back to input stream/caches Char.
		//! @param character Char that would be reverted.
		GDINT void RevertChar(Char const Ch)
		{
			if (Ch == Char('\n'))
			{ // Reverting line break.
				CurrentSymbol = SizeTpMax;
				CurrentLine -= 1;
			}
			else
			{ // Reverting just last Ch.
				CurrentSymbol -= 1;
			}

			if (RewindedChar == Char('\0'))
			{
				RewindedChar = Ch;
			}
			else
			{
				Stream->Seek(-1, SeekOrigin::End);
			}
		}

		//! Processes single character from input stream.
		GDINT void ProcessUpcomingChar()
		{
			// Reading next character from stream and determining the type of new character and possible type switch.
			StreamedLexerCharType const PrevCharType = CurrentCharType;
			GetNextChar();
			if (Options.IsInSpecialCharactersAlphabet(CurrentChar))
			{
				CurrentCharType = StreamedLexerCharType::Special;
			}
			else if (CChar::IsAlphabetic(CurrentChar) || CChar::IsPunctuation(CurrentChar))
			{
				CurrentCharType = StreamedLexerCharType::Alphabetic;
			}
			else if (CChar::IsDigit(CurrentChar, CurrentLexemIntegerNotation))
			{
				CurrentCharType = StreamedLexerCharType::Digit;
			}
			else if (CChar::IsSpace(CurrentChar))
			{
				CurrentCharType = StreamedLexerCharType::Space;
			}
			else
			{
				CurrentCharType = StreamedLexerCharType::Unknown;
			}
			if ((CurrentCharType == StreamedLexerCharType::Space) && (PrevCharType == StreamedLexerCharType::Unknown))
			{
				// Upcoming space that splits lexemes. Just skipping it.
				CurrentCharType = StreamedLexerCharType::Unknown;
				return;
			}
			else if ((CurrentCharType == StreamedLexerCharType::Unknown) && (CurrentChar != Char('\0')))
			{
				// Unknown character exists in stream (except \0 Char). Error:
				throw StreamedLexerFatalErrorException(CurrentLexem, "invalid character with code 0x%x was read from input stream.", static_cast<SizeTp>(CurrentChar));
			}

			// Now lets Handle the type of read Char. Here we have a table that describes if we need to check character type switching
			// or ignore switching in several cases. Building a translation table: 
			struct TestCharTypesSwitchingTable final
			{
				bool DoTestCharTypeSwitch;
				StreamedLexerCharType NotCommitOnSwitchingTo[CharTypeCount + 1];
			};
			static TestCharTypesSwitchingTable const TestCharTypeSwitchingTable[LexemContentTypesCount + 1] = {
				/* LexemContentType::Identifier         */ { true , { StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown } },
				/* LexemContentType::Keyword            */ { true , { StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown } },
				/* LexemContentType::Operator           */ { true , { StreamedLexerCharType::Special, StreamedLexerCharType::Unknown } },
				/* LexemContentType::Comment            */ { false, { StreamedLexerCharType::Unknown, } },
				/* LexemContentType::ConstantString     */ { false, { StreamedLexerCharType::Unknown, } },
				/* LexemContentType::ConstantCharacter	*/ { false, { StreamedLexerCharType::Unknown, } },
				/* LexemContentType::ConstantInteger    */ { true , { StreamedLexerCharType::Digit, StreamedLexerCharType::Alphabetic, StreamedLexerCharType::Special, StreamedLexerCharType::Unknown } },
				/* LexemContentType::ConstantFloat      */ { true , { StreamedLexerCharType::Digit, StreamedLexerCharType::Unknown } },
				/* LexemContentType::Unknown            */ { true , { StreamedLexerCharType::Space, StreamedLexerCharType::Unknown } }
			};

			TestCharTypesSwitchingTable const& SwitchingTable = TestCharTypeSwitchingTable[static_cast<SizeTp>(CurrentLexem->ContentType)];
			bool const CharTypeChanged = (PrevCharType != CurrentCharType);
			bool const HasPrevCharType = (PrevCharType != StreamedLexerCharType::Unknown);
			if (SwitchingTable.DoTestCharTypeSwitch && CharTypeChanged && HasPrevCharType)
			{
				// Parsing all conditions where token committing is required.
				// Attempting to switch state. Skipping if Prev type is unknown - tokenizer was launched for the first time 
				StreamedLexerCharType const* NotSwitchOnTypePtr = &SwitchingTable.NotCommitOnSwitchingTo[0];
				for (; (*NotSwitchOnTypePtr) != StreamedLexerCharType::Unknown; NotSwitchOnTypePtr += 1)
				{
					// Testing if current token is required to commit
					if ((*NotSwitchOnTypePtr) == CurrentCharType)
					{
						// Ignoring the commit and continuing processing
						break;
					}
				}

				// Rewinding character that caused character type switching and committing current Char
				if ((*NotSwitchOnTypePtr) == StreamedLexerCharType::Unknown)
				{
					RewindedChar = CurrentChar;
					WasLexemCommited = true;
					return;
				}
			}

			struct LexemStatesDesc final
			{
				bool RequiresClosingStatement;
				void (StreamedLexerImpl::* SpecialProcessorPtr)();
			};
			static LexemStatesDesc const LexemStatesDescTable[LexerStatesCount] = {
				/* StreamedLexerState::ReadingConstantString	  */ { true, &StreamedLexerImpl::ProcessStringConstant },
				/* StreamedLexerState::ReadingConstantChar        */ { true, &StreamedLexerImpl::ProcessCharConstant },
				/* StreamedLexerState::ReadingConstantInteger	  */ { false, &StreamedLexerImpl::ProcessIntegerConstant },
				/* StreamedLexerState::ReadingConstantFloat	      */ { false, &StreamedLexerImpl::ProcessFloatConstant },
				/* StreamedLexerState::ReadingCommentOrOperator   */ { false, &StreamedLexerImpl::ProcessOperatorOrComment },
				/* StreamedLexerState::ReadingCommentSingleLine   */ { false, &StreamedLexerImpl::ProcessSingleLineComment },
				/* StreamedLexerState::ReadingCommentMultipleLine */ { true, &StreamedLexerImpl::ProcessMultipleLineComment },
				/* StreamedLexerState::ReadingIdentifierOrKeyword */ { false, &StreamedLexerImpl::ProcessIdentifierOrKeyword },
			};

			bool const HasCurrentChar = (CurrentChar != Char('\0'));
			bool const CurrentStateUnknown = (CurrentState == StreamedLexerState::Unknown);
			if (HasCurrentChar && CurrentStateUnknown)
			{
				// Here is first real character of lexem.
				CurrentLexem->Line = CurrentLine;
				CurrentLexem->Symbol = CurrentSymbol;

				// Attempting to detect new state after previous commit 
				switch (CurrentChar)
				{
						// Detecting opening quote types:
					case Char('\"'):
						CurrentState = StreamedLexerState::ReadingConstantString;
						CurrentLexem->ContentType = LexemContentType::ConstantString;
					//	CurrentLexem->RawData += Char('\"');
						break;

					case Char('\''):
						CurrentState = StreamedLexerState::ReadingConstantChar;
						CurrentLexem->ContentType = LexemContentType::ConstantCharacter;
					//	CurrentLexem->RawData += Char('\'');
						break;

					default:
					{
						switch (CurrentCharType)
						{
								// Parsing other character types:
							case StreamedLexerCharType::Space:
							case StreamedLexerCharType::Digit:
								CurrentState = StreamedLexerState::ReadingConstantInteger;
								CurrentLexem->ContentType = LexemContentType::ConstantInteger;
								break;

							case StreamedLexerCharType::Alphabetic:
								CurrentState = StreamedLexerState::ReadingIdentifierOrKeyword;
								CurrentLexem->ContentType = LexemContentType::Identifier;
								break;

							case StreamedLexerCharType::Special:
								CurrentState = StreamedLexerState::ReadingCommentOrOperator;
								CurrentLexem->ContentType = LexemContentType::Operator;
								break;

							default:
								throw LogicException("Internal error: invalid CurrentCharType");
						}
					}
					break;
				}
			}
			else if (!HasCurrentChar)
			{
				// End of stream reached.
				if (LexemStatesDescTable[static_cast<SizeTp>(CurrentState)].RequiresClosingStatement)
				{
					// End of stream reached, but we were parsing token that requires terminator. Error:
					throw StreamedLexerFatalErrorException(CurrentLexem, "unexpected end of stream while parsing %s.", LexemContentTypeToString(CurrentLexem->ContentType));
				}

				CurrentLexem->ContentType = LexemContentType::EndOfStream;
				WasLexemCommited = true;
				return;
			}

			// Parsing current Char
			// CurrentState cannot be greater equal to unknown state
			(this ->* LexemStatesDescTable[static_cast<SizeTp>(CurrentState)].SpecialProcessorPtr)();
			if (RewindedChar == Char('\0'))
			{
				CurrentLexem->RawData += CurrentChar;
			}
		}

		//! Processes integer constants inside input stream.
		GDINT void ProcessIntegerConstant()
		{
			switch (CurrentCharType)
			{
					// Processing character if it is digit in current notation.
				case StreamedLexerCharType::Digit:
				{
					UInt8 const CurrentDigit = CChar::ToDigit(CurrentChar);
					CurrentLexem->ProcessedDataInteger *= CurrentLexemIntegerNotation;
					CurrentLexem->ProcessedDataInteger += CurrentDigit;
				}
					break;

					// Switching notation:
				case StreamedLexerCharType::Alphabetic:
				{
					bool const HasAnyData = (CurrentLexem->ProcessedDataInteger == 0);
					bool const IsDefaultNotation = (CurrentLexemIntegerNotation == StreamedLexerImpl::DefaultLexemIntegerNotation);

					// Checking if notation switching is provided in format "0x...":
					bool WasNotationSwitched = (HasAnyData && IsDefaultNotation);
					if (WasNotationSwitched)
					{
						Char const UpperCaseChar = CChar::ToUpper(CurrentChar);
						if (UpperCaseChar == Options.IntegerHexadecimalNotationDelimiter)
						{
							CurrentLexemIntegerNotation = 16;
						}
						else if (UpperCaseChar == Options.IntegerOctalNotationDelimiter)
						{
							CurrentLexemIntegerNotation = 8;
						}
						else if (UpperCaseChar == Options.IntegerBinaryNotationDelimiter)
						{
							CurrentLexemIntegerNotation = 2;
						}
						else
						{
							WasNotationSwitched = false;
						}
					}

					// Found, that prefix does not matches template or it does not follows '0' Char
					if (!WasNotationSwitched)
					{
						RewindedChar = CurrentChar;
						WasLexemCommited = true;
						return;
					}
				} break;

					// OR Switching to float parsing:
				case StreamedLexerCharType::Special:
				{
					if (CurrentChar == Options.FloatingPointDelimiter)
					{ // Floating point delimiter found.
						CurrentState = StreamedLexerState::ReadingConstantFloat;
						CurrentLexem->ContentType = LexemContentType::ConstantFloat;
						CurrentLexem->ProcessedDataFloat = static_cast<Float64>(CurrentLexem->ProcessedDataInteger);
					}
					else
					{ // Some other special character upcoming, committing token.
						RewindedChar = CurrentChar;
						WasLexemCommited = true;
						return;
					}
				} break;

					// OR Throwing debug Assertion if internal error exists
				default:
				{
					throw LogicException("Some strange things happened, unknown condition for ProcessIntegerConstant");
				}
			}
		}

		//! Processes floating-point constants inside input stream.
		GDINT void ProcessFloatConstant()
		{
			UInt8 const CurrentDigit = CChar::ToDigit(CurrentChar);
			CurrentLexem->ProcessedDataFloat += (static_cast<Float64>(CurrentDigit) / static_cast<Float64>(CurrentLexemFloatExponent));
			CurrentLexemFloatExponent += 1;
		}

		//! Processes escape sequence in string or character constant.
		GDINT void ProcessEscapeSequenceInStringOrCharConstant()
		{
			switch (CurrentESMode)
			{
				case StreamedLexerESMode::Unknown:
				{
					if (CurrentChar == Char('\\'))
					{
						CurrentESMode = StreamedLexerESMode::Waiting;
					}
				}
					break;

				case StreamedLexerESMode::Waiting:
				{
					CurrentESMode = StreamedLexerESMode::Unknown;
					CurrentlyWasCharWritten = true;
					switch (CurrentChar)
					{ // For more details read here: http://en.wikipedia.org/wiki/Escape_sequences_in_C
						case Char('a'):
						{ // Alarm (Beep, Bell)
							CurrentLexem->ProcessedDataCharacter = Char('\a');
						}
							return;
						case Char('b'):
						{ // Backspace
							CurrentLexem->ProcessedDataCharacter = Char('\b');
						}
							return;
						case Char('f'):
						{ // Form-feed
							CurrentLexem->ProcessedDataCharacter = Char('\f');
						}
							return;
						case Char('n'):
						{ // Newline (Line Feed)
							CurrentLexem->ProcessedDataCharacter = Char('\n');
						}
							return;
						case Char('r'):
						{ // Carriage Return
							CurrentLexem->ProcessedDataCharacter = Char('\r');
						}
							return;
						case Char('t'):
						{ // Horizontal Tab
							CurrentLexem->ProcessedDataCharacter = Char('\t');
						}
							return;
						case Char('v'):
						{ // Vertical Tab
							CurrentLexem->ProcessedDataCharacter = Char('\v');
						}
							return;
						case Char('\\'):
						{ // Vertical Tab
							CurrentLexem->ProcessedDataCharacter = Char('\\');
						}
							return;
						case Char('\''):
						{ // Single quotation mark
							CurrentLexem->ProcessedDataCharacter = Char('\'');
						}
							return;
						case Char('\"'):
						{ // Double quotation mark
							CurrentLexem->ProcessedDataCharacter = Char('\"');
						}
							return;
						case Char('\?'):
						{ // Question mark
							CurrentLexem->ProcessedDataCharacter = Char('\?');
						}
							return;
						case Char('x'):
						{ // The character whose numerical value is given by hh interpreted as a hexadecimal number
							CurrentESMode = StreamedLexerESMode::HexadecimalChar;
							CurrentlyWasCharWritten = false;
						}
							return;
						case Char('u'):
						{ // Unicode Chars in string literals (UTF8)
							CurrentESMode = StreamedLexerESMode::Unicode8;
							CurrentlyWasCharWritten = false;
						}
							return;
						case Char('U'):
						{ // Unicode Chars in string literals (UTF16)
							CurrentESMode = StreamedLexerESMode::Unicode16;
							CurrentlyWasCharWritten = false;
						}
							return;

						default:
						{
							if (!CChar::IsDigit(CurrentChar, 8))
							{ 
								// Unrecognized character escape sequence.
								throw StreamedLexerFatalErrorException(CurrentLexem, "unrecognized character escape sequence '\\%c' in string/Char constant.", CurrentChar);
							}
							// The character whose numerical value is given by nnn interpreted as an octal number
							CurrentlyWasCharWritten = false;
							CurrentESMode = StreamedLexerESMode::OctalChar;
							CurrentLexem->ProcessedDataCharacter = Char('\0');
						}
					}
				}
					fallthrough;

				case StreamedLexerESMode::OctalChar:
				case StreamedLexerESMode::HexadecimalChar:
				{
					UInt8 const CurrentNotation = ((CurrentESMode == StreamedLexerESMode::OctalChar) ? 8 : 16);
					if (!CChar::IsDigit(CurrentChar, CurrentNotation))
					{ 
						// Here is not a not a digit Char.
						if (CurrentLexem->GetRawData().GetLength() < 2 && CurrentNotation == 16)
						{ 
							// Hex constants must have at least one hex digit (now raw data contains string "'\").
							throw StreamedLexerFatalErrorException(CurrentLexem, "empty string/character constant with '\\x'");
						}

						CurrentESMode = StreamedLexerESMode::Unknown;
						CurrentlyWasCharWritten = true;
						return;
					}

					// Processing next Char
					UInt8 const CurrentDigit = CChar::ToDigit(CurrentChar);
					if ((static_cast<SizeTp>(CurrentLexem->ProcessedDataCharacter) * CurrentNotation + CurrentDigit) > UCHAR_MAX)
					{ 
						// Too big for character error.
						//! @todo What the fuck is happening here?
						throw StreamedLexerFatalErrorException(CurrentLexem, "too big number for Char.");
					}

					CurrentLexem->ProcessedDataCharacter *= CurrentNotation;
					CurrentLexem->ProcessedDataCharacter += CurrentDigit;
				}
					return;

				default:
				{
					throw NotImplementedException("Parsing this StreamedLexerImpl::CurrentESMode is not implemented");
				}
			}
		}

		//! Processes string constants inside input stream.
		GDINT void ProcessStringConstant()
		{
			// Parsing string constants in basics mode: so no escape sequences.
			if (CurrentChar == Char('\"'))
			{
				WasLexemCommited = true;
			}
		}

		//! Processes character constants inside input stream.
		GDINT void ProcessCharConstant()
		{
			ProcessEscapeSequenceInStringOrCharConstant();
			if (CurrentESMode == StreamedLexerESMode::Unknown)
			{
				// We are sure that escape sequence does not exists or was successfully parsed.
				if (CurrentChar == Char('\''))
				{
					// Here is unescaped "'" Char.
					if (!CurrentlyWasCharWritten)
					{
						// Empty character constant error. (now raw data contains quote mark ("'") and parsed ).
						throw StreamedLexerFatalErrorException(CurrentLexem, "empty character constant.");
					}

					WasLexemCommited = true;
					return;
				}

				if (CurrentlyWasCharWritten)
				{ 
					// Multiple Chars inside character constant.
					throw StreamedLexerFatalErrorException(CurrentLexem, "multiple symbols in character constant '%s%c'.", CurrentLexem->GetRawData().CStr(), CurrentChar);
				}

				// Here is our Char.
				CurrentLexem->ProcessedDataCharacter = CurrentChar;
				CurrentlyWasCharWritten = true;
			}
		}

		//! Processes operators inside input stream and handles switching to multiple line comment parsing.
		GDINT void ProcessOperatorOrComment()
		{
			// Filling CurrentMatches with pointers on operators and comments declarations
			if (CurrentMatches.GetLength() == 0)
			{
				CurrentMatchingIndex = 0;
				CurrentMatches.Reserve(Options.OperatorDeclarations.GetLength() + 2);
				CurrentMatches.InsertLast(&Options.SingleLineCommentDeclaration);
				CurrentMatches.InsertLast(&Options.MultipleLineCommentBeginning);
				for (auto const& OperatorDeclaration : Options.OperatorDeclarations)
				{
					CurrentMatches.InsertLast(&OperatorDeclaration.Value);
				}
			}

			bool HasAnyFullMatch = false;
			Vector<String const*> CurrentMatchesAccum(0, CurrentMatches.GetLength());
			for (auto const CurrentOperatorOrCommentMatch : CurrentMatches)
			{
				if ((*CurrentOperatorOrCommentMatch)[CurrentMatchingIndex] == CurrentChar)
				{
					// character matches some declaration at current m_Index.
					if (CurrentOperatorOrCommentMatch->GetLength() == (1 + CurrentMatchingIndex))
					{
						// We are having a full match with some operator, so lets identify it now
						if (CurrentOperatorOrCommentMatch == &Options.SingleLineCommentDeclaration)
						{
							// Declaration matches single line comment declaration.
							CurrentMatchingIndex = 0;
							CurrentState = StreamedLexerState::ReadingCommentSingleLine;
							CurrentLexem->ContentType = LexemContentType::Comment;
							CurrentLexem->ProcessedDataID = 0;
							return;
						}
						else if (CurrentOperatorOrCommentMatch == &Options.MultipleLineCommentBeginning)
						{
							// Declaration matches multiple line comment declaration.
							CurrentMatchingIndex = 0;
							CurrentState = StreamedLexerState::ReadingCommentMultipleLine;
							CurrentLexem->ContentType = LexemContentType::Comment;
							CurrentLexem->ProcessedDataID = 0;
							return;
						}
						else
						{
							// Declaration matches operator declaration.
							auto const Iterator = FindFirstIf(Options.OperatorDeclarations, [&](StreamedLexerOperatorDecl const& OperatorDecl) -> bool
								{
									return ((&OperatorDecl.Value) == CurrentOperatorOrCommentMatch);
								});
							CurrentLexem->ProcessedDataID = Iterator->Key;

							HasAnyFullMatch = true;
							continue;
						}
					}

					CurrentMatchesAccum.InsertLast(CurrentOperatorOrCommentMatch);
				}
			}

			// No matches in this iteration. That means that current operator ID was set in Prev. 
			if (CurrentMatchesAccum.GetLength() == 0)
			{
				// If there is not full math then some character is breaking the sequence breaking. Rewinding it to Handle later.
				if (!HasAnyFullMatch)
				{
					RewindedChar = CurrentChar;
				}

				WasLexemCommited = true;
				return;
			}

			CurrentMatches = Move(CurrentMatchesAccum);
			CurrentMatchingIndex += 1;
		}

		//! Processes identifiers and keywords inside input stream.
		GDINT void ProcessIdentifierOrKeyword()
		{
			// Filling CurrentMatches with pointers on operators and comments declarations
			if (CurrentMatches.GetLength() == 0 && CurrentMatchingIndex == 0)
			{
				CurrentMatches.Reserve(Options.KeywordsDeclarations.GetLength());
				for (auto const& KeywordsDeclarations : Options.KeywordsDeclarations)
				{
					CurrentMatches.InsertLast(&KeywordsDeclarations.Value);
				}
			}

			bool HasAnyFullMatch = false;
			Vector<String const*> CurrentMatchesAccum(0, CurrentMatches.GetLength());
			for (auto const CurrentIdentifierMatch : CurrentMatches)
			{
				if ((*CurrentIdentifierMatch)[CurrentMatchingIndex] == CurrentChar)
				{
					// character matches some declaration at current m_Index
					if (CurrentIdentifierMatch->GetLength() == 1 + CurrentMatchingIndex)
					{
						// We are having a full match with some keyword, so lets identify it now.
						auto const Iterator = FindFirstIf(Options.KeywordsDeclarations, [&](StreamedLexerKeywordDecl const& KeywordDecl) -> bool
							{ return &KeywordDecl.Value == CurrentIdentifierMatch; });

						CurrentLexem->ContentType = LexemContentType::Keyword;
						CurrentLexem->ProcessedDataID = Iterator->Key;

						HasAnyFullMatch = true;
						continue;
					}
					CurrentMatchesAccum.InsertLast(CurrentIdentifierMatch);
				}
			}

			// We are having next character that makes this lexem valid identifier. Restoring identifier content type.
			if (!HasAnyFullMatch)
			{
				CurrentLexem->ProcessedDataID = 0;
				CurrentLexem->ContentType = LexemContentType::Identifier;
			}

			CurrentMatches = Move(CurrentMatchesAccum);
			CurrentMatchingIndex += 1;
		}

		//! Processes single line comments inside input stream.
		GDINL void ProcessSingleLineComment()
		{
			if (CurrentChar == Char('\n'))
			{
				WasLexemCommited = true;
			}
		}

		//! Processes multiple line comments inside input stream.
		GDINT void ProcessMultipleLineComment()
		{
			if (Options.MultipleLineCommentEnding[CurrentMatchingIndex] == CurrentChar)
			{ // Matching with multiple line comment ending.
				CurrentMatchingIndex += 1;
				if (Options.MultipleLineCommentEnding.GetLength() == CurrentMatchingIndex)
				{ // Final match, finalizing comment.
					CurrentMatchingIndex = 0;
					WasLexemCommited = true;
					return;
				}
			}
		}
	}; // class BasicStreamedLexerImpl

	//! ==========================================================================================
	//! StreamedLexer class.
	//! ==========================================================================================

	//! Initializes new streamed lexer.
	//! @param Toolchain   Corresponding toolchain.
	//! @param Stream   input stream on which lexer would work.
	//! @param Options  Packed lexing options list.
	StreamedLexer::StreamedLexer(InputStream* Stream, StreamedLexerOptions const& Options) : Options(Options)
	{
		Implementation = GD_NEW(StreamedLexerImpl, Options, Stream);
	}

	StreamedLexer::~StreamedLexer()
	{
	}

	//! Extracts next lexem from input stream into output. Throws an exception into toolchain if error occurs. Can be ran in simple and advanced mode.
	//! More about modes: 
	//! @param OutputLexem Pointer to output lexem variable.
	//! @param LexerMode   Describes available feature set of Streamed Lexer.
	//! @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
	bool StreamedLexer::GetNextLexem(Lexem* const OutputLexem)
	{ // We are using implementation class to prevent dozens of private fields in class.
		return Implementation->GetNextLexem(OutputLexem);
	}

	//! Extracts next character from input stream and wraps line breaks, etc.
	//! @returns Extracted Char.
	Char StreamedLexer::GetNextCharacter()
	{ // We are using implementation class to prevent dozens of private fields in class.
		return Implementation->GetNextChar();
	}

	//! Pushes specified character back to lexer and make it parsable.
	//! @param character Char that would be reverted.
	void StreamedLexer::RevertCharacter(Char const Char)
	{ // We are using implementation class to prevent dozens of private fields in class.
		return Implementation->RevertChar(Char);
	}

		StreamedLexerOptions::StreamedLexerOptions(
		StreamedLexerKeywordsList const& KeywordsDeclarations,
		StreamedLexerOperatorsList const& OperatorDeclarations,
		String const& SingleLineCommentDeclaration,
		String const& MultipleLineCommentBeginning,
		String const& MultipleLineCommentEnding,
		Char const IntegerHexadecimalNotationDelimiter,
		Char const IntegerOctalNotationDelimiter,
		Char const IntegerBinaryNotationDelimiter,
		Char const FloatingPointDelimiter
	)	: KeywordsDeclarations(KeywordsDeclarations)
		, OperatorDeclarations(OperatorDeclarations)
		, SingleLineCommentDeclaration(SingleLineCommentDeclaration)
		, MultipleLineCommentBeginning(MultipleLineCommentBeginning)
		, MultipleLineCommentEnding(MultipleLineCommentEnding)
		, IntegerHexadecimalNotationDelimiter((IntegerHexadecimalNotationDelimiter != Char('\0')) ? CChar::ToUpper(IntegerHexadecimalNotationDelimiter) : Char('\0'))
		, IntegerOctalNotationDelimiter((IntegerOctalNotationDelimiter != Char('\0')) ? CChar::ToUpper(IntegerOctalNotationDelimiter) : Char('\0'))
		, IntegerBinaryNotationDelimiter((IntegerBinaryNotationDelimiter != Char('\0')) ? CChar::ToUpper(IntegerBinaryNotationDelimiter) : Char('\0'))
		, FloatingPointDelimiter(FloatingPointDelimiter)
	{
		AppendToSpecialCharactersAlphabet(FloatingPointDelimiter);
		for (auto const Char : SingleLineCommentDeclaration) {
			AppendToSpecialCharactersAlphabet(Char);
		}
		for (auto const Char : MultipleLineCommentBeginning) {
			AppendToSpecialCharactersAlphabet(Char);
		}
		for (auto const& OperatorDeclaration : OperatorDeclarations) {
			String const& OperatorDeclarationString = OperatorDeclaration.Value;
			for (auto const Character : OperatorDeclarationString) {	// Checking if operator contains only special characters
				AppendToSpecialCharactersAlphabet(Character);
			}
		}
	}

	//! ==========================================================================================
	//! StreamedLexerDefaultOptions namespace.
	//! Contains some predefined options for streamed lexer.
	//! ==========================================================================================

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

	//! ==========================================================================================
	///	LexemContentType enum.
	//! ==========================================================================================

	extern CStr LexemContentTypeToString(LexemContentType const ContentType)
	{
		static CStr const LexemContentTypeStrings[] = {
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
		return LexemContentTypeStrings[SizeTp(ContentType)];
	}

GD_NAMESPACE_END
