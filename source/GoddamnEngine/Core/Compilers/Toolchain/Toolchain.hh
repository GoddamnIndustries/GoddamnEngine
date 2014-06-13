//////////////////////////////////////////////////////////////////////////
/// Toolchain.hh - toolchain element interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_COMPILERS_TOOLCHAIN
#define GD_CORE_COMPILERS_TOOLCHAIN

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

GD_NAMESPACE_BEGIN

	class Stream;
	class IToolchainTool;
	typedef char ToolchainMessage;

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

	/// Integer values that provides exception for toolchain.
	/// Each toolchain exception is computed using @c "(is_fatal ? 1 : -1)  * exc_abs + exc_code".
	/// Each toolchain tool has it`s own module value and set of exceptions.
	typedef long ToolchainException;

	/// Returns true if this exception is in this module.
	GDINL bool IsMyToolchainException(ToolchainException const Exception, ToolchainException const Module)
	{	
		return (labs(Exception / Module) == 1L);
	}

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

		GDINL void RaiseException(ToolchainException const Exception) 
		{
			if (Exception == 1L)
				GD_ASSERT(true, "");
			GD_ASSERT((self->LastException == 0L), "Toolchain already contains uncaught exception");
			self->LastException = Exception;
		}
		
		GDINL ToolchainException CatchException()
		{
			ToolchainException LastException = self->LastException;
			self->LastException = 0L;
			return LastException;
		}

		GDAPI bool WasExceptionRaised() const 
		{ 
			return (self->LastException != 0L); 
		}

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
		inline void RaiseWarning(ToolchainMessage const* const Warning, ...)
		{
			GD_UNUSED(Warning);
		}
		inline void RaiseError(ToolchainMessage const* const Error, ...)
		{ 
			self->RaiseExceptionWithCode(1l); 
			GD_UNUSED(Error);
		}
		inline void RaiseFatalError(ToolchainMessage const* const Error, ...) 
		{
			self->RaiseExceptionWithCode(1l);
			GD_UNUSED(Error);
		}
		inline void RaiseExceptionWithCode(long const ExceptionCode) 
		{ 
			self->Toolchain->RaiseException(ToolchainException(ExceptionCode)); 
		}
	};	// class IToolchainTool

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_COMPILERS_TOOLCHAIN
