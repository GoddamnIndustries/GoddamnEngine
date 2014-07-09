/// ==========================================================================================
/// StreamedPreprocessor.hh - C-style preprocessor interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 06.05.2014 - Created by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_COMPILERS_STREAMED_PREPROCESSOR
#define GD_CORE_COMPILERS_STREAMED_PREPROCESSOR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.hh>

GD_NAMESPACE_BEGIN

	class Stream;

	/// Contains codes of exceptions that streamed lexer throws.
	enum StreamedPreprocessorExceptionCodes : ToolchainException
	{
		GD_PREPROCESSOR_EXCEPTION_MODULE =  100L,							       ///< Streamed lexer module.
		GD_PREPROCESSOR_EXCEPTION_EOS    = -GD_PREPROCESSOR_EXCEPTION_MODULE - 1L, ///< End-Of-Stream.
		GD_PREPROCESSOR_EXCEPTION_COMMIT = -GD_PREPROCESSOR_EXCEPTION_MODULE - 2L, ///< Token was commited.
		GD_PREPROCESSOR_EXCEPTION_SYNTAX = +GD_PREPROCESSOR_EXCEPTION_MODULE + 1L, ///< Syntax error.
	};	// enum StreamedPreprocessorExceptionCodes

	class StreamedPreprocessor final : public IToolchainTool
	{
	private:
		GD_CLASS_UNASSIGNABLE(StreamedPreprocessor);
		GD_CLASS_UNSWAPPABLE (StreamedPreprocessor);
		GD_CLASS_UNCOPIABLE  (StreamedPreprocessor);
		GD_CLASS_UNMOVABLE   (StreamedPreprocessor);

		StreamedLexer Lexer;

	public:
		GDINL  StreamedPreprocessor(IToolchain* const Toolchain, StreamedLexerOptions const& Options) : IToolchainTool(Toolchain), Lexer(Toolchain, Options) { }
		GDINL ~StreamedPreprocessor() { }

		GDAPI bool GetNextLexem(Stream* const InputStream, Lexem* const OutputLexem);
	};	// class StreamedPreprocessor final

GD_NAMESPACE_END

#endif
