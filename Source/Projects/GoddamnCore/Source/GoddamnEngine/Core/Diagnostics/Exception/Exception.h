/// ==========================================================================================
/// Exception.h: Exception base class interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_DIAGNOSTICS_EXCEPTION
#define GD_CORE_DIAGNOSTICS_EXCEPTION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	/// Represents runtime error.
	class Exception
	{
	private /*Class members*/:
		String Message;

	public /*Class API*/:
		/// Initializes new exception with specified error message.
		/// @param Message Error message.
		GDINL explicit Exception(String const& Message) : Message(Message) { }
		GDINL virtual ~Exception() { }

		/// Returns message of thrown exception.
		/// @returns Thrown error message.
		GDINL virtual String const& GetErrorMessage() const { return this->Message; }

		/// Returns type of exception was thrown.
		/// @returns Type of exception was thrown.
		GDINL virtual Str GetExceptionType() const { return "Exception"; }
	};	// class Exception

	// Defines new exception class with specified base class, inheritance attributes and overridden 'GetErrorMessage' proc.
#define GD_DEFINE_EXCPETION_BAM(ExceptionClass, BaseExceptionClass, Attributes, MessageGetterFunc) \
	class ExceptionClass Attributes : public BaseExceptionClass \
	{ \
	public /*Public API*/: \
		GDINL explicit ExceptionClass(String const& ErrorMessage) : BaseExceptionClass(ErrorMessage) { } \
		GDINL virtual ~ExceptionClass() { } \
		GDINL virtual Str GetExceptionType() const override Attributes { return (#ExceptionClass); } \
		MessageGetterFunc\
	};	// class ExceptionClass

#define GD_DEFINE_EXCPETION_DERIVED_FINAL(ExceptionClass, BaseExceptionClass) GD_DEFINE_EXCPETION_BAM(ExceptionClass, BaseExceptionClass, final, /**/)
#define GD_DEFINE_EXCPETION_DERIVED(ExceptionClass, BaseExceptionClass) GD_DEFINE_EXCPETION_BAM(ExceptionClass, BaseExceptionClass, /**/, /**/)
#define GD_DEFINE_EXCPETION_FINAL(ExceptionClass) GD_DEFINE_EXCPETION_BAM(ExceptionClass, Exception, final, /**/)
#define GD_DEFINE_EXCPETION(ExceptionClass) GD_DEFINE_EXCPETION_BAM(ExceptionClass, Exception, /**/, /**/)

	/// Runtime error that notifies that something went wrong due programmer`s fault.
	GD_DEFINE_EXCPETION_FINAL(LogicException);

	/// Runtime error that notifies that some action is not supported.
	GD_DEFINE_EXCPETION_FINAL(NotSupportedException);

	/// Runtime error that notifies that some code part was not implemented.
	GD_DEFINE_EXCPETION_FINAL(NotImplementedException);

	/// Runtime error that notifies that some code part was not implemented.
	GD_DEFINE_EXCPETION_FINAL(IndexOutOfBoundsException);

GD_NAMESPACE_END

#endif
