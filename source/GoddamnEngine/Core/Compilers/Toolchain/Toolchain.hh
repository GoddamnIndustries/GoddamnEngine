/// ==========================================================================================
/// Toolchain.hh - toolchain element interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_COMPILERS_TOOLCHAIN
#define GD_CORE_COMPILERS_TOOLCHAIN

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Compilers/CompilerBase.hh>
#include <cstdarg>

GD_NAMESPACE_BEGIN

	class Stream;
	class IToolchainTool;
	typedef char ToolchainMessage;

	/// Runtime error that caused be some tool of toolchain.
	class ToolchainException : public Exception
	{
	public /*Public API*/:
		GDINL explicit ToolchainException(Str const ErrorMessage) : Exception(ErrorMessage) { }
		GDINL virtual ~ToolchainException() { }
	};	// class ToolchainException

	struct ToolchainMessageDesc
	{
	protected /*Class members and API*/:
		Str const Message;
		GDAPI size_t static GlobalID;
		size_t const ID;
		GDINL explicit ToolchainMessageDesc(Str const Message) : Message(Message), ID(++GlobalID) { }
		GDINT ToolchainMessageDesc& operator= (ToolchainMessageDesc const& Other) = delete;
	public:
		GDINL String ToString(Lexem const* const CurrentLexem, ...) const
		{
			va_list List = va_list(); va_start(List, CurrentLexem);
			String FormattedMessage = String::FormatVa(self->Message, List);
			va_end(List);

			return String::Format("~~SomeFile~~(%u, %u): error GD%u: %s", CurrentLexem->GetLine(), CurrentLexem->GetSymbol(), self->ID, FormattedMessage.CStr());
		}
	};	// struct ToolchainMessageDesc

	/// Information about source code provider.
	struct ToolchainSourceProvider final
	{
	private:
		GD_CLASS_UNASSIGNABLE(ToolchainSourceProvider);
		
	public:
		Str		const SourceName;	///< Name of this source to display.
		Stream* const SourceStream; ///< Stream where this source is located.

		GDINL ToolchainSourceProvider(Str const SourceName, Stream* const SourceStream) : SourceName(SourceName),          SourceStream(SourceStream)          { }
		GDINL ToolchainSourceProvider(ToolchainSourceProvider const& Provider)          : SourceName(Provider.SourceName), SourceStream(Provider.SourceStream) { }
	};	// struct ToolchainSourceProvider

	/// Provides information about location inside source code.
	struct ToolchainSourceLocation final
	{
	private:
		GD_CLASS_UNASSIGNABLE(ToolchainSourceProvider);
		
		ToolchainSourceProvider* Provider;
		size_t Line = 1, Symbol = 1;

	public:
	};

	class IToolchain
	{
	private:
		GD_CLASS_UNASSIGNABLE(IToolchain);
		GD_CLASS_UNSWAPPABLE (IToolchain);
		GD_CLASS_UNCOPIABLE  (IToolchain);
		GD_CLASS_UNMOVABLE   (IToolchain);

		ToolchainException LastException = 0L;

	public:
		GDINL IToolchain() { }

		GDINL bool DoTreatWarningsAsError() const { return true; }
	};	// class IToolchain

	class IToolchainTool
	{
	private:
		GD_CLASS_UNASSIGNABLE(IToolchainTool);
		GD_CLASS_UNSWAPPABLE (IToolchainTool);
		GD_CLASS_UNCOPIABLE  (IToolchainTool);
		GD_CLASS_UNMOVABLE   (IToolchainTool);

	protected:
		IToolchain* const Toolchain;
		GDINL IToolchainTool(IToolchain* const Toolchain) : Toolchain(Toolchain) { }
	public:
		/*inline void RaiseMessage(ToolchainMessage const* const Message, ...)
		{
			va_list List = va_list(); va_start(List, Message);
			vprintf(Message, List);
			va_end(List);
		}
		inline void RaiseWarning(ToolchainMessage const* const Warning, ...)
		{
			va_list List = va_list(); va_start(List, Warning);
			vprintf(Warning, List);
			va_end(List);
		}
		inline void RaiseError(ToolchainMessage const* const Error, ...)
		{ 
			va_list List = va_list(); va_start(List, Error);
			vprintf(Error, List);
			va_end(List);
			self->RaiseExceptionWithCode(1l); 
		}
		inline void RaiseFatalError(ToolchainMessage const* const Error, ...) 
		{
			va_list List = va_list(); va_start(List, Error);
			vprintf(Error, List);
			va_end(List);
			self->RaiseExceptionWithCode(1l);
		}*/
		/*inline void RaiseExceptionWithCode(long const ExceptionCode) 
		{ 
			self->Toolchain->RaiseException(ToolchainException(ExceptionCode)); 
		}*/
	};	// class IToolchainTool

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_COMPILERS_TOOLCHAIN
