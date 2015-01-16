/// ==========================================================================================
/// StreamedLexer.h - lexer interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_COMPILERS_STREAMED_LEXER
#define GD_CORE_COMPILERS_STREAMED_LEXER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexerTables.h>

GD_NAMESPACE_BEGIN

	class InputStream;

	/// @brief Class provides streamable tokenization for input stream.
	/// @note Instance of this class is quiet a "heavy" object, so it is recommended to make a static object. 
	class StreamedLexer final : public IToolchainTool
	{ 
	private:
		GD_CLASS_UNASSIGNABLE(StreamedLexer);
		GD_CLASS_UNSWAPPABLE (StreamedLexer);
		GD_CLASS_UNCOPIABLE  (StreamedLexer);
		GD_CLASS_UNMOVABLE   (StreamedLexer);

		UniquePtr<class StreamedLexerImpl> Implementation;
		StreamedLexerOptions const& Options;

	public:
		/// Initializes new streamed lexer.
		/// @param Toolchain Corresponding toolchain.
		/// @param Stream Input stream on which lexer would work.
		/// @param Options Packed lexing options list.
		GDAPI StreamedLexer(IToolchain* const Toolchain, UniquePtr<InputStream>&& Stream, StreamedLexerOptions const& Options);
		GDAPI virtual ~StreamedLexer();

		/// Extracts next lexem from input stream into output. Throws an exception into toolchain if error occurs. Can be ran in simple and advanced mode.
		/// @param OutputLexem Pointer to output lexem variable.
		/// @returns True if lexem was successfully extracted. If operation fails (except cases of expected End-Of-Stream) than exception is raised to that toolchain.
		GDAPI bool GetNextLexem(Lexem* const OutputLexem);

		/// Extracts next character from input stream and wraps line breaks, etc.
		/// @returns Extracted character.
		GDAPI CharAnsi GetNextCharacter();

		/// Pushes specified character back to lexer and make it parsable.
		/// @param Character Character that would be reverted.
		GDAPI void RevertCharacter(CharAnsi const Character);
	};	// class StreamedLexer

	/// Returns predefined tokenizer options for C++-like languages (C, C++, C#, Javascript, HLSL, GLSL, Cg)
	GDAPI extern StreamedLexerOptions const& GetDefaultOptionsForCpp();

	/// Returns predefined tokenizer options for XML-like languages (XML, (X)HTML)
	GDAPI extern StreamedLexerOptions const& GetDefaultOptionsForXml();

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_COMPILERS_STREAMED_LEXER
