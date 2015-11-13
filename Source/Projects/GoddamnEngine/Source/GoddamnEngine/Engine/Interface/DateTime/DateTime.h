// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/DateTime/DateTime.h
 * Cross-platform date/time definitions.
 */
#pragma once
#define GD_INSIDE_DATETIME_H 1

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Interface/UniqueInterface.h>
#include <GoddamnEngine/Core/Singleton.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN
	
	// ------------------------------------------------------------------------------------------
	//! Enumerates days in a week (starting from Monday).
	GD_DENUM()
	enum DDayOfWeek : UInt8
	{
		IDATETIME_DAY_OF_WEEK_UNKNOWN  = 0,
		IDATETIME_DAY_OF_WEEK_MONDAY   = 1,
		IDATETIME_DAY_OF_WEEK_TUESTADY = 2,
		IDATETIME_DAY_OF_WEEK_WENSDAY  = 3,
		IDATETIME_DAY_OF_WEEK_THIRSDAY = 4,
		IDATETIME_DAY_OF_WEEK_FRIDAY   = 5,
		IDATETIME_DAY_OF_WEEK_SATURDAY = 6,
		IDATETIME_DAY_OF_WEEK_SUNDAY   = 7,
	};	// DDayOfWeek DDayOfWeek

	// ------------------------------------------------------------------------------------------
	//! Enumerates months in a year.
	GD_DENUM()
	enum DMonthOfYear : UInt8
	{
		IDATETIME_MONTH_UNKNOWN   = 0,
		IDATETIME_MONTH_JANUARY   = 1,
		IDATETIME_MONTH_FEBRUARY  = 2,
		IDATETIME_MONTH_MARCH     = 3,
		IDATETIME_MONTH_APRIL     = 4,
		IDATETIME_MONTH_MAY       = 5,
		IDATETIME_MONTH_JUNE      = 6,
		IDATETIME_MONTH_JULY      = 7,
		IDATETIME_MONTH_AUGUST    = 8,
		IDATETIME_MONTH_SEPTEMBER = 9,
		IDATETIME_MONTH_OCTOBER   = 10,
		IDATETIME_MONTH_NOVEMBER  = 11,
		IDATETIME_MONTH_DECEMBER  = 12,
	};	// enum DMonthOfYear
	
	// ------------------------------------------------------------------------------------------
	//! Represents date/time type.
	GD_DSTRUCT(CopyFields)
	struct IDateTimeInfo final 
	{
	public:
		Int32			DateYear		= 0;
		DMonthOfYear	DateMonth		= IDATETIME_MONTH_UNKNOWN;
		DDayOfWeek		DateWeekDay		= IDATETIME_DAY_OF_WEEK_UNKNOWN;
		UInt8			DateDay			= 0;
		UInt8			DateHour		= 0;
		UInt8			DateMinute		= 0;
		UInt8 			DateSecond		= 0;
		UInt16 			DateMillisecond	= 0;
	};	// struct IDateTimeInfo

	// ------------------------------------------------------------------------------------------
	//! Defines a generic date/time manager class.
	GD_DINTERFACE()
	class IDateTime : public IRuntimeUniqueInterface
#if !GD_D_REFLECTOR	// DReflector should not parse singletons declarations.
		, public Singleton<IDateTime>
#endif	// if !GD_D_REFLECTOR
	{
	protected:
		Float64 timeStartTime       = 0.0;	//!< Start time for a engine.
		Float64 timeSecondsPerCycle = 0.0;	//!< Time it takes to perform one CPU cycle.
		Float64 timeFrameStartTime  = 0.0;	//!< Time at the beginning of the frame.
		Float64 timeFrameDeltaTime  = 0.0;	//!< Time it took to complete previous frame.
		UInt32  timeFramesCount		= 0;	//!< Total amount of frames rendered (mod 2^32-1).

	public:
		GDAPI virtual IResult OnRuntimeInitialize() override;
		GDAPI virtual void OnRuntimeUpdate() override;

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns time in seconds passed till Engine launch.
		//! @returns Time in seconds passed till Engine launch.
		GD_DFUNCTION()
		GDAPI virtual Float64 GetSeconds()
		{
			auto const timeTicksCount = ::clock();
			return timeSecondsPerCycle * static_cast<Float64>(timeTicksCount) - timeStartTime;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns time in seconds passed till Engine launch as a pretty formatted string.
		//! @returns Time in seconds passed till Engine launch as a pretty formatted string.
		GD_DFUNCTION()
		GDAPI virtual String GetSecondsString();

		// ------------------------------------------------------------------------------------------
		//! Returns total amount of frames rendered.
		//! @returns Total amount of frames rendered.
		GD_DFUNCTION()
		GDAPI virtual UInt32 GetFramesCount()
		{
			return timeFramesCount;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns time at the beginning of the frame.
		//! @returns Time at the beginning of the frame.
		GD_DFUNCTION()
		GDAPI virtual Float64 GetFrameStartTime()
		{
			return timeFrameStartTime;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns time it took to complete previous frame.
		//! @returns Time it took to complete previous frame.
		GD_DFUNCTION()
		GDAPI virtual Float64 GetFrameDeltaTime()
		{
			return timeFrameDeltaTime;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns local time in UTC.
		//! @returns Local time in UTC.
		//! @note Not all implementations can return full time. Unresolved fields would be set to zero.
		GD_DFUNCTION()
		GDAPI virtual IDateTimeInfo GetDateTimeUTC();

		// ------------------------------------------------------------------------------------------
		//! Queries local date in UTC.
		//! @returns Local date in UTC as a string.
		GD_DFUNCTION()
		GDAPI virtual String GetDateUTCString();

		// ------------------------------------------------------------------------------------------
		//! Queries local time in UTC.
		//! @returns Local time in UTC as a string.
		GD_DFUNCTION()
		GDAPI virtual String GetTimeUTCString();

		// ------------------------------------------------------------------------------------------
		//! Queries local date and time in UTC.
		//! @returns Local date and time in UTC as a string.
		GD_DFUNCTION()
		GDAPI virtual String GetDateTimeUTCString();

	};	// class GenericDateTime

	// ------------------------------------------------------------------------------------------
	//! Global date/time interface pointer.
	GD_DSTATIC_VARIABLE()
	GDAPI extern IUniqueInterfacePointer<IDateTime> DateTime;

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Interface/DateTime/DateTimeMicrosoft.h>
#undef GD_INSIDE_DATETIME_H
