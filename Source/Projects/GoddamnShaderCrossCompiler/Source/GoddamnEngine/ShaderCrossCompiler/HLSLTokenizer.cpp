// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/ShaderCrossCompiler/HLSLTokenizer.cpp
 * Stream tokenizer.
 */
#include <GoddamnEngine/ShaderCrossCompiler/HLSLTokenizer.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/System/IO/Stream.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN
	
	typedef ToolchainException StreamedLexerFatalErrorException;

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                                   Lexem class.                                   ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GDAPI extern CStr LexemContentTypeToString(LexemContentType const contentType)
	{
		static CStr const lexemContentTypeStrings[] = {
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

		GD_DEBUG_ASSERT(contentType < LexemContentType::Unknown, "Invalid ContentType specified");
		return lexemContentTypeStrings[SizeTp(contentType)];
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                             StreamedLexerImpl class.                             ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Implementation of the advanced streaming lexer. 
	// **------------------------------------------------------------------------------------------**
	class StreamedLexerImpl final : public TNonCopyable
	{
	public:

		//! @{
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
		SizeTp static const s_LexerStatesCount = static_cast<SizeTp>(StreamedLexerState::Unknown);
		//! @}

		//! @{
		enum class StreamedLexerCharType : UInt8
		{
			Space,
			Digit,
			Special,
			Alphabetic,
			Unknown,
		}; // enum class StreamedLexerCharType
		SizeTp static const CharTypeCount = static_cast<SizeTp>(StreamedLexerCharType::Unknown);
		//! @}

		//! @{
		enum class StreamedLexerEscapeSequenceMode : UInt8
		{
			Waiting,
			OctalChar,
			HexadecimalChar,
			Unicode8,
			Unicode16,
			Unknown,
		}; // enum class StreamedLexerEscapeSequenceMode
		//! @}

	private:
		InputStream*                      m_Stream;
		StreamedLexerOptions const&       m_Options;

		bool                              m_WasLexemCommited                 = false;
		Lexem*                            m_CurrentLexem                     = nullptr; // Lexem state & info
		SizeTp                            m_CurrentLine                      = 1; // Lexem location
		SizeTp                            m_CurrentSymbol                    = 0;
		Char                              m_RewindedChar                     = Char('\0');
		StreamedLexerState                m_CurrentState                     = StreamedLexerState::Unknown;
		Char                              m_CurrentChar                      = Char('\0'); // Character meta
		StreamedLexerCharType             m_CurrentCharType                  = StreamedLexerCharType::Unknown;
		SizeTp static const               s_DefaultLexemIntegerNotation      = 10; // Numeric meta
		SizeTp                            m_CurrentLexemIntegerNotation      = s_DefaultLexemIntegerNotation;
		SizeTp                            m_CurrentMatchingIndex             = 0; // Operators and comments meta
		SizeTp                            m_CurrentLexemFloatExponent        = 1;
		Vector<String const*>             m_CurrentMatches;
		StreamedLexerEscapeSequenceMode   m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Unknown;
		bool                              m_CurrentlyWasCharWritten          = false;

	public:

		/*!
		 * Initializes the lexer implementation with the specified options and input stream.
		 * @param stream Input stream on which lexer would work.
		 * @param options Packed lexing options list.
		 */
		GDINL StreamedLexerImpl(InputStream* const stream, StreamedLexerOptions const& options) : m_Stream(stream), m_Options(options)
		{
		}

	public:

		/*!
		 * Reads next lexem from input stream.
		 * @param outputLexem Pointer to the output lexem struct.
		 */
		GDINT void GetNextLexem(Lexem* const outputLexem);

		/*!
		 * Reads next character from input stream. 
		 * @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
		 */
		GDINT Char GetNextChar();

		/*!
		 * Pushes character back to input stream/caches Char.
		 * @param character Char that would be reverted.
		 */
		GDINT void RevertChar(Char const character);

	private:

		/*!
		 * Processes single character from input stream.
		 */
		GDINT void ProcessUpcomingChar();

		/*!
		 * Processes integer constants inside input stream.
		 */
		GDINT void ProcessIntegerConstant();

		/*!
		 * Processes floating-point constants inside input stream.
		 */
		GDINT void ProcessFloatConstant();

		/*!
		 * Processes escape sequence in string or character constant.
		 */
		GDINT void ProcessEscapeSequenceInStringOrCharConstant();

		/*!
		 * Processes string constants inside input stream.
		 */
		GDINT void ProcessStringConstant();

		/*!
		 * Processes character constants inside input stream.
		 */
		GDINT void ProcessCharConstant();

		/*!
		 * Processes operators inside input stream and handles switching to multiple line comment parsing.
		 */
		GDINT void ProcessOperatorOrComment();

		/*!
		 * Processes identifiers and keywords inside input stream.
		 */
		GDINT void ProcessIdentifierOrKeyword();

		/*!
		 * Processes single line comments inside input stream.
		 */
		GDINL void ProcessSingleLineComment();

		/*!
		 * Processes multiple line comments inside input stream.
		 */
		GDINT void ProcessMultipleLineComment();

	}; // class BasicStreamedLexerImpl

	/*!
	 * Reads next lexem from input stream.
	 * @param outputLexem Pointer to the output lexem struct.
	 */
	void StreamedLexerImpl::GetNextLexem(Lexem* const outputLexem)
	{
		// Restoring default tokenizer values
		m_WasLexemCommited = false;
		m_CurrentLine = 1;
		m_CurrentSymbol = 0;
		m_CurrentState = StreamedLexerState::Unknown;
		m_CurrentChar = Char('\0');
		m_CurrentCharType = StreamedLexerCharType::Unknown;
		m_CurrentLexemIntegerNotation = s_DefaultLexemIntegerNotation;
		m_CurrentMatchingIndex = 0;
		m_CurrentLexemFloatExponent = 1;
		m_CurrentMatches.Emptify();
		m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Unknown;
		m_CurrentlyWasCharWritten = false;
		m_CurrentLexem = outputLexem;
		m_CurrentLexem->ResetLexem();

		// And parsing until the end of lexem.
		while (!m_WasLexemCommited)
		{
			ProcessUpcomingChar();
		}
	}

	/*!
	 * Reads next character from input stream. 
	 * @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
	 */
	GDINT Char StreamedLexerImpl::GetNextChar()
	{
		// Testing if we have rewinded Char.
		if (m_RewindedChar == Char('\0'))
		{
			auto const currentChar = m_Stream->Read();
			if (currentChar == -1)
			{
				return m_CurrentChar = Char('\0');
			}

			// Trying to read next char:
			m_CurrentChar = static_cast<Char>(currentChar);
			if (m_CurrentChar == Char('\r'))
			{
				// Trying to Handle '\r\n' sequence.
				auto nextChar = m_Stream->Read();
				if (nextChar == -1)
				{
					return m_CurrentChar = Char('\0');
				}
				if (nextChar != Char('\n'))
				{
					m_RewindedChar = static_cast<Char>(nextChar);
				}

				// New line here.
				m_CurrentChar = Char('\n');
				m_CurrentSymbol = 0;
				m_CurrentLine += 1;
			}
			else if (m_CurrentChar == Char('\n'))
			{
				// Handing line termination Chars.
				m_CurrentSymbol = 0;
				m_CurrentLine += 1;
			}
			else
			{
				// Next character on current line.
				m_CurrentSymbol += 1;
			}
		}
		else
		{
			m_CurrentChar = m_RewindedChar;
			m_RewindedChar = Char('\0');
		}

		if (m_CurrentChar == ';')
		{
			return m_CurrentChar;
		}

		return m_CurrentChar;
	}

	/*!
	 * Pushes character back to input stream/caches Char.
	 * @param character Char that would be reverted.
	 */
	GDINT void StreamedLexerImpl::RevertChar(Char const character)
	{
		if (character == Char('\n'))
		{ 
			// Reverting line break.
			m_CurrentSymbol = SizeTpMax;
			m_CurrentLine -= 1;
		}
		else
		{ 
			// Reverting just last character.
			m_CurrentSymbol -= 1;
		}

		if (m_RewindedChar == Char('\0'))
		{
			m_RewindedChar = character;
		}
		else
		{
			m_Stream->Seek(-1, SeekOrigin::End);
		}
	}

	/*!
	 * Processes single character from input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessUpcomingChar()
	{
		// Reading next character from stream and determining the type of new character and possible type switch.
		auto const prevCharType = m_CurrentCharType;
		GetNextChar();
		if (m_Options.IsInSpecialCharactersAlphabet(m_CurrentChar))
		{
			m_CurrentCharType = StreamedLexerCharType::Special;
		}
		else if (CChar::IsAlphabetic(m_CurrentChar) || CChar::IsPunctuation(m_CurrentChar))
		{
			m_CurrentCharType = StreamedLexerCharType::Alphabetic;
		}
		else if (CChar::IsDigit(m_CurrentChar, m_CurrentLexemIntegerNotation))
		{
			m_CurrentCharType = StreamedLexerCharType::Digit;
		}
		else if (CChar::IsSpace(m_CurrentChar))
		{
			m_CurrentCharType = StreamedLexerCharType::Space;
		}
		else
		{
			m_CurrentCharType = StreamedLexerCharType::Unknown;
		}
		if (m_CurrentCharType == StreamedLexerCharType::Space && prevCharType == StreamedLexerCharType::Unknown)
		{
			// Upcoming space that splits lexemes. Just skipping it.
			m_CurrentCharType = StreamedLexerCharType::Unknown;
			return;
		}
		if (m_CurrentCharType == StreamedLexerCharType::Unknown && m_CurrentChar != Char('\0'))
		{
			// Unknown character exists in stream (except \0 Char). Error:
			throw StreamedLexerFatalErrorException(m_CurrentLexem, "invalid character with code 0x%x was read from input stream.", static_cast<SizeTp>(m_CurrentChar));
		}

		// Now lets Handle the type of read Char. Here we have a table that describes if we need to check character type switching
		// or ignore switching in several cases. Building a translation table: 
		struct TestCharTypesSwitchingTable final
		{
			bool DoTestCharTypeSwitch;
			StreamedLexerCharType NotCommitOnSwitchingTo[CharTypeCount + 1];
		};	// struct TestCharTypesSwitchingTable
		static TestCharTypesSwitchingTable const testCharTypeSwitchingTable[LexemContentTypesCount + 1] = {
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

		auto const& switchingTable = testCharTypeSwitchingTable[static_cast<SizeTp>(m_CurrentLexem->m_ContentType)];
		auto const charTypeChanged = prevCharType != m_CurrentCharType;
		auto const hasPrevCharType = prevCharType != StreamedLexerCharType::Unknown;
		if (switchingTable.DoTestCharTypeSwitch && charTypeChanged && hasPrevCharType)
		{
			// Parsing all conditions where token committing is required.
			// Attempting to switch state. Skipping if previous type is unknown - tokenizer was launched for the first time 
			auto notSwitchOnTypePtr = &switchingTable.NotCommitOnSwitchingTo[0];
			for (; *notSwitchOnTypePtr != StreamedLexerCharType::Unknown; notSwitchOnTypePtr += 1)
			{
				// Testing if current token is required to commit
				if (*notSwitchOnTypePtr == m_CurrentCharType)
				{
					// Ignoring the commit and continuing processing
					break;
				}
			}

			// Rewinding character that caused character type switching and committing current Char
			if (*notSwitchOnTypePtr == StreamedLexerCharType::Unknown)
			{
				m_RewindedChar = m_CurrentChar;
				m_WasLexemCommited = true;
				return;
			}
		}

		struct LexemStatesDesc final
		{
			bool RequiresClosingStatement;
			void (StreamedLexerImpl::* SpecialProcessorPtr)();
		};
		static LexemStatesDesc const lexemStatesDescTable[s_LexerStatesCount] = {
			/* StreamedLexerState::ReadingConstantString	  */ { true, &StreamedLexerImpl::ProcessStringConstant },
			/* StreamedLexerState::ReadingConstantChar        */ { true, &StreamedLexerImpl::ProcessCharConstant },
			/* StreamedLexerState::ReadingConstantInteger	  */ { false, &StreamedLexerImpl::ProcessIntegerConstant },
			/* StreamedLexerState::ReadingConstantFloat	      */ { false, &StreamedLexerImpl::ProcessFloatConstant },
			/* StreamedLexerState::ReadingCommentOrOperator   */ { false, &StreamedLexerImpl::ProcessOperatorOrComment },
			/* StreamedLexerState::ReadingCommentSingleLine   */ { false, &StreamedLexerImpl::ProcessSingleLineComment },
			/* StreamedLexerState::ReadingCommentMultipleLine */ { true, &StreamedLexerImpl::ProcessMultipleLineComment },
			/* StreamedLexerState::ReadingIdentifierOrKeyword */ { false, &StreamedLexerImpl::ProcessIdentifierOrKeyword },
		};

		auto const hasCurrentChar = m_CurrentChar != Char('\0');
		auto const currentStateUnknown = m_CurrentState == StreamedLexerState::Unknown;
		if (hasCurrentChar && currentStateUnknown)
		{
			// Here is first real character of lexem.
			m_CurrentLexem->m_Line = m_CurrentLine;
			m_CurrentLexem->m_Symbol = m_CurrentSymbol;

			// Attempting to detect new state after previous commit 
			switch (m_CurrentChar)
			{
				// Detecting opening quote types:
				case Char('\"'):
					m_CurrentState = StreamedLexerState::ReadingConstantString;
					m_CurrentLexem->m_ContentType = LexemContentType::ConstantString;
					break;

				case Char('\''):
					m_CurrentState = StreamedLexerState::ReadingConstantChar;
					m_CurrentLexem->m_ContentType = LexemContentType::ConstantCharacter;
					break;

				default:
					switch (m_CurrentCharType)
					{
						// Parsing other character types:
						case StreamedLexerCharType::Space:
						case StreamedLexerCharType::Digit:
							m_CurrentState = StreamedLexerState::ReadingConstantInteger;
							m_CurrentLexem->m_ContentType = LexemContentType::ConstantInteger;
							break;

						case StreamedLexerCharType::Alphabetic:
							m_CurrentState = StreamedLexerState::ReadingIdentifierOrKeyword;
							m_CurrentLexem->m_ContentType = LexemContentType::Identifier;
							break;

						case StreamedLexerCharType::Special:
							m_CurrentState = StreamedLexerState::ReadingCommentOrOperator;
							m_CurrentLexem->m_ContentType = LexemContentType::Operator;
							break;

						default:
							GD_ASSERT_FALSE("Internal error: invalid CurrentCharType");
					}
					break;
			}
		}
		else if (!hasCurrentChar)
		{
			// End of stream reached.
			if (lexemStatesDescTable[static_cast<SizeTp>(m_CurrentState)].RequiresClosingStatement)
			{
				// End of stream reached, but we were parsing token that requires terminator. Error:
				throw StreamedLexerFatalErrorException(m_CurrentLexem, "unexpected end of stream while parsing %s.", LexemContentTypeToString(m_CurrentLexem->m_ContentType));
			}

			m_CurrentLexem->m_ContentType = LexemContentType::EndOfStream;
			m_WasLexemCommited = true;
			return;
		}

		// Parsing current Char
		// CurrentState cannot be greater equal to unknown state
		(this ->* lexemStatesDescTable[static_cast<SizeTp>(m_CurrentState)].SpecialProcessorPtr)();
		if (m_RewindedChar == Char('\0'))
		{
			m_CurrentLexem->m_RawData.Append(m_CurrentChar);
		}
	}

	/*!
	 * Processes integer constants inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessIntegerConstant()
	{
		switch (m_CurrentCharType)
		{
			// Processing character if it is digit in current notation.
			case StreamedLexerCharType::Digit:
			{
				auto const currentDigit = CChar::ToDigit(m_CurrentChar);
				m_CurrentLexem->m_ProcessedDataInteger *= m_CurrentLexemIntegerNotation;
				m_CurrentLexem->m_ProcessedDataInteger += currentDigit;
			} break;

			// Switching notation:
			case StreamedLexerCharType::Alphabetic:
			{
				auto const HasAnyData = m_CurrentLexem->m_ProcessedDataInteger == 0;
				auto const IsDefaultNotation = m_CurrentLexemIntegerNotation == s_DefaultLexemIntegerNotation;

				// Checking whether the notation switching is provided in format "0x...":
				auto WasNotationSwitched = HasAnyData && IsDefaultNotation;
				if (WasNotationSwitched)
				{
					auto const UpperCaseChar = CChar::ToUpper(m_CurrentChar);
					if (UpperCaseChar == m_Options.IntegerHexadecimalNotationDelimiter)
					{
						m_CurrentLexemIntegerNotation = 16;
					}
					else if (UpperCaseChar == m_Options.IntegerOctalNotationDelimiter)
					{
						m_CurrentLexemIntegerNotation = 8;
					}
					else if (UpperCaseChar == m_Options.IntegerBinaryNotationDelimiter)
					{
						m_CurrentLexemIntegerNotation = 2;
					}
					else
					{
						WasNotationSwitched = false;
					}
				}

				// Found, that prefix does not matches template or it does not follows '0' Char
				if (!WasNotationSwitched)
				{
					m_RewindedChar = m_CurrentChar;
					m_WasLexemCommited = true;
				}
			} break;

			// OR Switching to float parsing:
			case StreamedLexerCharType::Special:
			{
				if (m_CurrentChar == m_Options.FloatingPointDelimiter)
				{ 
					// Floating point delimiter found.
					m_CurrentState = StreamedLexerState::ReadingConstantFloat;
					m_CurrentLexem->m_ContentType = LexemContentType::ConstantFloat;
					m_CurrentLexem->m_ProcessedDataFloat = static_cast<Float64>(m_CurrentLexem->m_ProcessedDataInteger);
				}
				else
				{ 
					// Some other special character upcoming, committing token.
					m_RewindedChar = m_CurrentChar;
					m_WasLexemCommited = true;
				}
			} break;

			default: 
				GD_ASSERT_FALSE("Some strange things happened, unknown condition for ProcessIntegerConstant");
		}
	}

	/*!
	 * Processes floating-point constants inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessFloatConstant()
	{
		auto const currentDigit = CChar::ToDigit(m_CurrentChar);
		m_CurrentLexem->m_ProcessedDataFloat += (static_cast<Float64>(currentDigit) / static_cast<Float64>(m_CurrentLexemFloatExponent));
		m_CurrentLexemFloatExponent += 1;
	}

	/*!
	 * Processes escape sequence in string or character constant.
	 */
	GDINT void StreamedLexerImpl::ProcessEscapeSequenceInStringOrCharConstant()
	{
		switch (m_CurrentEscapeSequenceParsingMode)
		{
			case StreamedLexerEscapeSequenceMode::Unknown:
			{
				if (m_CurrentChar == Char('\\'))
				{
					m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Waiting;
				}
			} break;

			case StreamedLexerEscapeSequenceMode::Waiting:
			{
				m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Unknown;
				m_CurrentlyWasCharWritten = true;
				switch (m_CurrentChar)
				{ 
					// For more details read here: http://en.wikipedia.org/wiki/Escape_sequences_in_C
					case Char('a'):
					{ 
						// Alarm (Beep, Bell)
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\a');
					} return;

					case Char('b'):
					{ 
						// Backspace
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\b');
					} return;

					case Char('f'):
					{ 
						// Form-feed
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\f');
					} return;

					case Char('n'):
					{ 
						// Newline (Line Feed)
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\n');
					} return;

					case Char('r'):
					{ 
						// Carriage Return
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\r');
					} return;

					case Char('t'):
					{ 
						// Horizontal Tab
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\t');
					} return;

					case Char('v'):
					{ 
						// Vertical Tab
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\v');
					} return;

					case Char('\\'):
					{ 
						// Vertical Tab
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\\');
					} return;

					case Char('\''):
					{ 
						// Single quotation mark
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\'');
					} return;

					case Char('\"'):
					{ 
						// Double quotation mark
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\"');
					} return;

					case Char('\?'):
					{ 
						// Question mark
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\?');
					} return;

					case Char('x'):
					{ 
						// The character whose numerical value is given by hh interpreted as a hexadecimal number
						m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::HexadecimalChar;
						m_CurrentlyWasCharWritten = false;
					} return;

					case Char('u'):
					{ 
						// Unicode Chars in string literals (UTF8)
						m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Unicode8;
						m_CurrentlyWasCharWritten = false;
					} return;

					case Char('U'):
					{ 
						// Unicode Chars in string literals (UTF16)
						m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Unicode16;
						m_CurrentlyWasCharWritten = false;
					} return;

					default:
					{
						if (!CChar::IsDigit(m_CurrentChar, 8))
						{ 
							// Unrecognized character escape sequence.
							throw StreamedLexerFatalErrorException(m_CurrentLexem, "unrecognized character escape sequence '\\%c' in string/Char constant.", m_CurrentChar);
						}
						// The character whose numerical value is given by nnn interpreted as an octal number
						m_CurrentlyWasCharWritten = false;
						m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::OctalChar;
						m_CurrentLexem->m_ProcessedDataCharacter = Char('\0');
					}
				}
			} fallthrough;

			case StreamedLexerEscapeSequenceMode::OctalChar:
			case StreamedLexerEscapeSequenceMode::HexadecimalChar:
			{
				UInt8 const currentNotation = m_CurrentEscapeSequenceParsingMode == StreamedLexerEscapeSequenceMode::OctalChar ? 8 : 16;
				if (!CChar::IsDigit(m_CurrentChar, currentNotation))
				{ 
					// Here is not a not a digit Char.
					if (m_CurrentLexem->GetRawData().GetLength() < 2 && currentNotation == 16)
					{ 
						// Hex constants must have at least one hex digit (now raw data contains string "'\").
						throw StreamedLexerFatalErrorException(m_CurrentLexem, "empty string/character constant with '\\x'");
					}

					m_CurrentEscapeSequenceParsingMode = StreamedLexerEscapeSequenceMode::Unknown;
					m_CurrentlyWasCharWritten = true;
					return;
				}

				// Processing next Char
				auto const currentDigit = CChar::ToDigit(m_CurrentChar);
				if (static_cast<SizeTp>(m_CurrentLexem->m_ProcessedDataCharacter) * currentNotation + currentDigit > UCHAR_MAX)
				{ 
					// Too big for character error.
					//! @todo What the fuck is going on here?
					throw StreamedLexerFatalErrorException(m_CurrentLexem, "too big number for Char.");
				}

				m_CurrentLexem->m_ProcessedDataCharacter *= currentNotation;
				m_CurrentLexem->m_ProcessedDataCharacter += currentDigit;
			} return;

			default: GD_NOT_IMPLEMENTED();
		}
	}

	/*!
	 * Processes string constants inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessStringConstant()
	{
		// Parsing string constants in basics mode: so no escape sequences.
		if (m_CurrentChar == Char('\"'))
		{
			m_WasLexemCommited = true;
		}
	}

	/*!
	 * Processes character constants inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessCharConstant()
	{
		ProcessEscapeSequenceInStringOrCharConstant();
		if (m_CurrentEscapeSequenceParsingMode == StreamedLexerEscapeSequenceMode::Unknown)
		{
			// We are sure that escape sequence does not exists or was successfully parsed.
			if (m_CurrentChar == Char('\''))
			{
				// Here is unescaped "'" Char.
				if (!m_CurrentlyWasCharWritten)
				{
					// Empty character constant error. (now raw data contains quote mark ("'") and parsed ).
					throw StreamedLexerFatalErrorException(m_CurrentLexem, "empty character constant.");
				}

				m_WasLexemCommited = true;
				return;
			}

			if (m_CurrentlyWasCharWritten)
			{ 
				// Multiple Chars inside character constant.
				throw StreamedLexerFatalErrorException(m_CurrentLexem, "multiple symbols in character constant '%s%c'.", m_CurrentLexem->GetRawData().CStr(), m_CurrentChar);
			}

			// Here is our Char.
			m_CurrentLexem->m_ProcessedDataCharacter = m_CurrentChar;
			m_CurrentlyWasCharWritten = true;
		}
	}

	/*!
	 * Processes operators inside input stream and handles switching to multiple line comment parsing.
	 */
	GDINT void StreamedLexerImpl::ProcessOperatorOrComment()
	{
		// Filling CurrentMatches with pointers on operators and comments declarations
		if (m_CurrentMatches.GetLength() == 0)
		{
			m_CurrentMatchingIndex = 0;
			m_CurrentMatches.Reserve(m_Options.OperatorDeclarations.GetLength() + 2);
			m_CurrentMatches.InsertLast(&m_Options.SingleLineCommentDeclaration);
			m_CurrentMatches.InsertLast(&m_Options.MultipleLineCommentBeginning);
			for (auto const& OperatorDeclaration : m_Options.OperatorDeclarations)
			{
				m_CurrentMatches.InsertLast(&OperatorDeclaration.Second);
			}
		}

		auto hasAnyFullMatch = false;
		Vector<String const*> currentMatchesAccum(0, m_CurrentMatches.GetLength());
		for (auto const currentOperatorOrCommentMatch : m_CurrentMatches)
		{
			if ((*currentOperatorOrCommentMatch)[m_CurrentMatchingIndex] == m_CurrentChar)
			{
				// character matches some declaration at current m_Index.
				if (currentOperatorOrCommentMatch->GetLength() == 1 + m_CurrentMatchingIndex)
				{
					// We are having a full match with some operator, so lets identify it now
					if (currentOperatorOrCommentMatch == &m_Options.SingleLineCommentDeclaration)
					{
						// Declaration matches single line comment declaration.
						m_CurrentMatchingIndex = 0;
						m_CurrentState = StreamedLexerState::ReadingCommentSingleLine;
						m_CurrentLexem->m_ContentType = LexemContentType::Comment;
						m_CurrentLexem->m_ProcessedDataID = 0;
						return;
					}
					else if (currentOperatorOrCommentMatch == &m_Options.MultipleLineCommentBeginning)
					{
						// Declaration matches multiple line comment declaration.
						m_CurrentMatchingIndex = 0;
						m_CurrentState = StreamedLexerState::ReadingCommentMultipleLine;
						m_CurrentLexem->m_ContentType = LexemContentType::Comment;
						m_CurrentLexem->m_ProcessedDataID = 0;
						return;
					}
					else
					{
						// Declaration matches operator declaration.
						auto const iter = FindFirstIf(m_Options.OperatorDeclarations
							, [&](StreamedLexerOperatorDecl const& OperatorDecl) -> bool { return &OperatorDecl.Second == currentOperatorOrCommentMatch; });
						m_CurrentLexem->m_ProcessedDataID = iter->First;

						hasAnyFullMatch = true;
						continue;
					}
				}
				currentMatchesAccum.InsertLast(currentOperatorOrCommentMatch);
			}
		}

		// No matches in this iteration. That means that current operator ID was set in previous. 
		if (currentMatchesAccum.GetLength() == 0)
		{
			// If there is not full math then some character is breaking the sequence breaking. Rewinding it to Handle later.
			if (!hasAnyFullMatch)
			{
				m_RewindedChar = m_CurrentChar;
			}

			m_WasLexemCommited = true;
			return;
		}

		m_CurrentMatches = Move(currentMatchesAccum);
		m_CurrentMatchingIndex += 1;
	}

	/*!
	 * Processes identifiers and keywords inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessIdentifierOrKeyword()
	{
		// Filling CurrentMatches with pointers on operators and comments declarations
		if (m_CurrentMatches.GetLength() == 0 && m_CurrentMatchingIndex == 0)
		{
			m_CurrentMatches.Reserve(m_Options.KeywordsDeclarations.GetLength());
			for (auto const& KeywordsDeclarations : m_Options.KeywordsDeclarations)
			{
				m_CurrentMatches.InsertLast(&KeywordsDeclarations.Second);
			}
		}

		auto hasAnyFullMatch = false;
		Vector<String const*> currentMatchesAccum(0, m_CurrentMatches.GetLength());
		for (auto const currentIdentifierMatch : m_CurrentMatches)
		{
			if ((*currentIdentifierMatch)[m_CurrentMatchingIndex] == m_CurrentChar)
			{
				// character matches some declaration at current m_Index
				if (currentIdentifierMatch->GetLength() == 1 + m_CurrentMatchingIndex)
				{
					// We are having a full match with some keyword, so lets identify it now.
					auto const iter = FindFirstIf(m_Options.KeywordsDeclarations, [&](StreamedLexerKeywordDecl const& KeywordDecl) -> bool
					    { return &KeywordDecl.Second == currentIdentifierMatch; });

					m_CurrentLexem->m_ContentType = LexemContentType::Keyword;
					m_CurrentLexem->m_ProcessedDataID = iter->First;

					hasAnyFullMatch = true;
					continue;
				}
				currentMatchesAccum.InsertLast(currentIdentifierMatch);
			}
		}

		// We are having next character that makes this lexem valid identifier. Restoring identifier content type.
		if (!hasAnyFullMatch)
		{
			m_CurrentLexem->m_ProcessedDataID = 0;
			m_CurrentLexem->m_ContentType = LexemContentType::Identifier;
		}

		m_CurrentMatches = Move(currentMatchesAccum);
		m_CurrentMatchingIndex += 1;
	}

	/*!
	 * Processes single line comments inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessSingleLineComment()
	{
		if (m_CurrentChar == Char('\n'))
		{
			m_WasLexemCommited = true;
		}
	}

	/*!
	 * Processes multiple line comments inside input stream.
	 */
	GDINT void StreamedLexerImpl::ProcessMultipleLineComment()
	{
		if (m_Options.MultipleLineCommentEnding[m_CurrentMatchingIndex] == m_CurrentChar)
		{ 
			// Matching with multiple line comment ending.
			m_CurrentMatchingIndex += 1;
			if (m_Options.MultipleLineCommentEnding.GetLength() == m_CurrentMatchingIndex)
			{ 
				// Final match, finalizing comment.
				m_CurrentMatchingIndex = 0;
				m_WasLexemCommited = true;
			}
		}
	}

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               StreamedLexer class.                               ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	
	// ReSharper disable CppMemberFunctionMayBeConst

	/*!
	 * Initializes the lexer with the specified options and input stream.
	 * @param stream Input stream on which lexer would work.
	 * @param options Packed lexing options list.
	 */
	GDAPI StreamedLexer::StreamedLexer(InputStream* const stream, StreamedLexerOptions const& options /*= StreamedLexerOptions::GetDefaultOptionsForCpp()*/) 
		: m_Implementation(gd_new StreamedLexerImpl (stream, options)), m_Options(options)
	{
	}

	GDAPI StreamedLexer::~StreamedLexer()
	{
	}

	/*!
	 * Reads next lexem from input stream.
	 * @param outputLexem Pointer to the output lexem struct.
	 */
	GDAPI void StreamedLexer::GetNextLexem(Lexem* const OutputLexem)
	{ 
		return m_Implementation->GetNextLexem(OutputLexem);
	}

	/*!
	 * Reads next character from input stream. 
	 * @returns Next character in stream. If stream ends, than returns '\0'. Also handles '\n' and '\r' sequences.
	 */
	GDAPI Char StreamedLexer::GetNextCharacter()
	{ 
		return m_Implementation->GetNextChar();
	}

	/*!
	 * Pushes character back to input stream/caches char.
	 * @param character Char that would be reverted.
	 */
	GDAPI void StreamedLexer::RevertCharacter(Char const character)
	{ 
		return m_Implementation->RevertChar(character);
	}

	// ReSharper restore CppMemberFunctionMayBeConst

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                           StreamedLexerOptions struct.                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GDAPI StreamedLexerOptions::StreamedLexerOptions(StreamedLexerKeywordsList const& keywordsDeclarations, StreamedLexerOperatorsList const& operatorDeclarations
		, String const& singleLineCommentDeclaration, String const& multipleLineCommentBeginning, String const& multipleLineCommentEnding
		, Char const integerHexadecimalNotationDelimiter, Char const integerOctalNotationDelimiter, Char const integerBinaryNotationDelimiter
		, Char const floatingPointDelimiter
	)	: KeywordsDeclarations(keywordsDeclarations)
		, OperatorDeclarations(operatorDeclarations)
		, SingleLineCommentDeclaration(singleLineCommentDeclaration)
		, MultipleLineCommentBeginning(multipleLineCommentBeginning)
		, MultipleLineCommentEnding(multipleLineCommentEnding)
		, IntegerHexadecimalNotationDelimiter(integerHexadecimalNotationDelimiter != Char('\0') ? CChar::ToUpper(integerHexadecimalNotationDelimiter) : Char('\0'))
		, IntegerOctalNotationDelimiter(integerOctalNotationDelimiter != Char('\0') ? CChar::ToUpper(integerOctalNotationDelimiter) : Char('\0'))
		, IntegerBinaryNotationDelimiter(integerBinaryNotationDelimiter != Char('\0') ? CChar::ToUpper(integerBinaryNotationDelimiter) : Char('\0'))
		, FloatingPointDelimiter(floatingPointDelimiter)
	{
		AppendToSpecialCharactersAlphabet(FloatingPointDelimiter);
		for (auto const ch : SingleLineCommentDeclaration) 
		{
			AppendToSpecialCharactersAlphabet(ch);
		}
		for (auto const ch : MultipleLineCommentBeginning) 
		{
			AppendToSpecialCharactersAlphabet(ch);
		}
		for (auto const& operatorDeclaration : OperatorDeclarations) 
		{
			String const& operatorDeclarationString = operatorDeclaration.Second;
			for (auto const ch : operatorDeclarationString) 
			{	
				AppendToSpecialCharactersAlphabet(ch);
			}
		}
	}

	/*!
	 * Returns predefined tokenizer options for C++-like languages (C, C++, C#, JavaScript, HLSL, GLSL, Cg).
	 */
	GDAPI StreamedLexerOptions const& StreamedLexerOptions::GetDefaultOptionsForCpp()
	{
		using namespace CPPKeywords;
		using namespace CPPOperators;
		static StreamedLexerOptions const cppLexerOptions(
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

		return cppLexerOptions;
	}

	/*GDAPI StreamedLexerOptions const& StreamedLexerOptions::GetDefaultOptionsForXml()
	{
	}*/

GD_NAMESPACE_END
