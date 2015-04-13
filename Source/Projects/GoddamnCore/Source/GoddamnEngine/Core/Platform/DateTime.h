// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/DateTime.h
//! Cross-platform date/time definitions.
#pragma once
#define GD_INSIDE_DATETIME_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN
	
	//! @brief Enumerates days in a week (starting from Monday).
	//! @todo Names of this enumeration are rather self-descriptive. So remove this, or document them.
	//! @{
	struct DayOfWeek
	{
		typedef UInt8 Type;
		Type static const Monday	= 1;
		Type static const Tuesday	= 2;
		Type static const Wensday	= 3;
		Type static const Thursday	= 4;
		Type static const Friday	= 5;
		Type static const Saturday	= 6;
		Type static const Sunday	= 7;
	};	// struct DayOfWeek
	typedef DayOfWeek::Type DayOfWeekTp;
	//! @}

	//! @brief Enumerates months in a year.
	//! @todo Names of this enumeration are rather self-descriptive. So remove this, or document them.
	//! @{
	struct MonthOfYear
	{
		typedef UInt8 Type;
		Type static const January	= 1;
		Type static const February	= 2;
		Type static const March		= 3;
		Type static const April		= 4;
		Type static const May		= 5;
		Type static const June		= 6;
		Type static const July		= 7;
		Type static const August	= 8;
		Type static const September	= 9;
		Type static const October	= 10;
		Type static const November	= 11;
		Type static const December	= 12;
	};	// struct MonthOfYear
	typedef MonthOfYear::Type MonthOfYearTp;
	//! @}
	
	//! @brief Represents date/time type.
	struct DateTimeTp
	{
		Int32			Year		= 0;	//!< @brief Current year.
		MonthOfYearTp	Month		= 0;	//!< @brief Current month.
		DayOfWeekTp		WeekDay		= 0;	//!< @brief Current day of week.
		UInt8			Day			= 0;	//!< @brief Current day.
		UInt8			Hour		= 0;	//!< @brief Current hour.
		UInt8			Minute		= 0;	//!< @brief Current minute.
		UInt8 			Second		= 0;	//!< @brief Current second.
		UInt16 			Millisecond	= 0;	//!< @brief Current millisecond.
	};	// struct DateTimeTp

	//! @brief Defines a generic date/time manager class.
	class IGenericDateTime : public IPlatformObject<IGenericDateTime, PlatformObjectFlags::FlagsNone>
	{
	protected:
		Float64 TimeStartTime;				//!< @brief Start time for a engine.
		Float64 TimeSecondsPerCycle;		//!< @brief Time it takes to perform one CPU cycle.
				
		UInt32  TimeFramesCount;			//!< @brief Total amount of frames rendered (mod 2^32).
		Float64 TimeFrameStartTime;			//!< @brief Time at the beginning of the frame.
		Float64 TimeFrameDeltaTime;			//!< @brief Time it took to complete previous frame.

	protected:

		//! @brief Initializes a date/time class.
		GDINL IGenericDateTime()
			: TimeStartTime      (0.0)
			, TimeSecondsPerCycle(0.0)
			, TimeFramesCount    (0)
			, TimeFrameStartTime (0.0)
			, TimeFrameDeltaTime (0.0)
		{
		}

		//! @brief Deinitializes a date/time class.
		GDINL virtual ~IGenericDateTime() {	}

	protected:

		//! @brief Initializes a date/time class.
		GD_PLATFORM_DEFINE_METHOD0(void, InitializeObject, override);

		//! @brief Ticks a frame.
		GD_PLATFORM_DEFINE_METHOD0(void, TickObject, override);

	public:
		//! @brief Returns time in seconds passed till Engine launch.
		//! @returns Time in seconds passed till Engine launch.
		GD_PLATFORM_DEFINE_METHOD0(Float64, GetSeconds, 
		{
			clock_t TimeTicksCount = std::clock();
			return (this->TimeSecondsPerCycle * static_cast<Float64>(TimeTicksCount)) - this->TimeStartTime;
		});

		//! @brief Returns time in seconds passed till Engine launch as a pretty formatted string.
		//! @returns Time in seconds passed till Engine launch as a pretty formatted string.
		GD_PLATFORM_DEFINE_METHOD0(String, GetSecondsString);

		//! @brief Returns total amount of frames rendered.
		//! @returns Total amount of frames rendered.
		GD_PLATFORM_DEFINE_METHOD0(UInt32, GetFramesCount, { return this->TimeFramesCount; });

		//! @brief Returns time at the beginning of the frame.
		//! @returns Time at the beginning of the frame.
		GD_PLATFORM_DEFINE_METHOD0(Float64, GetFrameStartTime, { return this->TimeFrameStartTime; });

		//! @brief Returns time it took to complete previous frame.
		//! @returns Time it took to complete previous frame.
		GD_PLATFORM_DEFINE_METHOD0(Float64, GetFrameDeltaTime, { return this->TimeFrameDeltaTime; });

		//! @brief Returns local time in UTC.
		//! @returns Local time in UTC.
		//! @note Not all implementations can return full time. Unresolved fields would be set to zero.
		GD_PLATFORM_DEFINE_METHOD0(DateTimeTp, GetDateTimeUTC);

		//! @brief Queries local date in UTC.
		//! @returns Local date in UTC as a string.
		GD_PLATFORM_DEFINE_METHOD0(String, GetDateUTCString);

		//! @brief Queries local time in UTC.
		//! @returns Local time in UTC as a string.
		GD_PLATFORM_DEFINE_METHOD0(String, GetTimeUTCString);

		//! @brief Queries local date and time in UTC.
		//! @returns Local date and time in UTC as a string.
		GD_PLATFORM_DEFINE_METHOD0(String, GetDateTimeUTCString);

	};	// class GenericDateTime

GD_NAMESPACE_END

#include GD_PLATFORM_INCLUDE(GoddamnEngine/Core/Platform, DateTime.h)
#undef GD_INSIDE_DATETIME_H
