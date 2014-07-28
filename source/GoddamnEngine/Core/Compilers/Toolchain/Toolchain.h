/// ==========================================================================================
/// Toolchain.h - toolchain element interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_COMPILERS_TOOLCHAIN
#define GD_CORE_COMPILERS_TOOLCHAIN

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Compilers/CompilerBase.h>
#include <cstdarg>

GD_NAMESPACE_BEGIN

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
		String const Message;
		GDINL explicit ToolchainMessageDesc(String const& Message) : Message(Message) { }
		GDINT ToolchainMessageDesc& operator= (ToolchainMessageDesc const& Other) = delete;
	public:
		GDINL String ToString(Lexem const* const CurrentLexem, ...) const
		{
			va_list List = va_list(); va_start(List, CurrentLexem);
			String FormattedMessage = String::FormatVa(self->Message.CStr(), List);
			va_end(List);

			if (CurrentLexem != nullptr) {
				return String::Format("(%u, %u): error GD0000: %s", CurrentLexem->GetLine(), CurrentLexem->GetSymbol(), FormattedMessage.CStr());
			}
			return String::Format("error GD0000: %s", FormattedMessage.CStr());
		}
	};	// struct ToolchainMessageDesc

	class IToolchain
	{
	private:
		GD_CLASS_UNASSIGNABLE(IToolchain);
		GD_CLASS_UNSWAPPABLE (IToolchain);
		GD_CLASS_UNCOPIABLE  (IToolchain);
		GD_CLASS_UNMOVABLE   (IToolchain);

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
