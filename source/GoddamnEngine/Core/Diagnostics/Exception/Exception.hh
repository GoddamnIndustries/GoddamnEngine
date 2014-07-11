/// ==========================================================================================
/// Exception.hh: Exception base class interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 04.07.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_DIAGNOSTICS_EXCEPTION
#define GD_CORE_DIAGNOSTICS_EXCEPTION

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>

GD_NAMESPACE_BEGIN

	/// Represents runtime error.
	class Exception
	{
	private /*Class members*/:
		Str Message;

	public /*Class API*/:
		/// Initializes new exception with specified error message.
		/// @param Message Error message.
		GDINL explicit Exception(Str const Message) : Message(Message) { }
		GDINL virtual ~Exception() { }

		/// Returns message of thrown exception.
		/// @returns Thrown error message.
		GDINL virtual Str const& GetErrorMessage() const { return self->Message; }
	};	// class Exception

	/// Runtime error that notifies that somerthing went wrong due programmer`s fault.
	class LogicException : public Exception
	{
	public /*Public API*/:
		GDINL explicit LogicException(Str const ErrorMessage) : Exception(ErrorMessage) { }
		GDINL virtual ~LogicException() { }
	};	// class LogicException

	/// Runtime error that notifies that some action is not supported.
	class NotSupportedException : public Exception
	{
	public /*Public API*/:
		GDINL explicit NotSupportedException(Str const ErrorMessage) : Exception(ErrorMessage) { }
		GDINL virtual ~NotSupportedException() { }
	};	// class NotSupportedException

	/// Runtime error that notifies that some code part was not implemented.
	class NotImplementedException : public Exception
	{
	public /*Public API*/:
		GDINL explicit NotImplementedException(Str const ErrorMessage) : Exception(ErrorMessage) { }
		GDINL virtual ~NotImplementedException() { }
	};	// class NotImplementedException

	/// Runtime error that notifies that some code part was not implemented.
	class IndexOutOfBoundsException : public Exception
	{
	public /*Public API*/:
		GDINL explicit IndexOutOfBoundsException(Str const ErrorMessage) : Exception(ErrorMessage) { }
		GDINL virtual ~IndexOutOfBoundsException() { }
	};	// class IndexOutOfBoundsException

	/// Throws exception of specified type on if condition is false.
	/// @param Condition Condition on which exception would be thrown.
	/// @param Arguments Exception construction arguments.
	template<typename ExceptionType, typename... ExceptionConstructorArgumentTypes>
	GDINL void ConditionalThrow(bool const Condition, ExceptionConstructorArgumentTypes&&... Arguments)
	{
		static_assert(TypeTraits::IsBaseType<Exception, ExceptionType>::Value, "'ConditionalThrow<T>' error: specified type should be Exception-derived.");
		if (!Condition)
			throw ExceptionType(Forward<ExceptionConstructorArgumentTypes>(Arguments)...);
	}

#if (defined(GD_RELEASE))
	/// Throws exception of specified type on if condition is false.
	/// @param Condition Condition on which exception would be thrown.
	/// @param Arguments Exception construction arguments.
	template<typename... ExceptionConstructorArgumentTypes>
	GDINL void ConditionalThrow<LogicException, ExceptionConstructorArgumentTypes...>(bool const Condition, ExceptionConstructorArgumentTypes&&... Arguments)
	{	// We do not need assertions in release build.
		GD_UNUSED(Condition, Arguments...);
	}
#endif	// if (defined(GD_RELEASE))

GD_NAMESPACE_END

#endif
