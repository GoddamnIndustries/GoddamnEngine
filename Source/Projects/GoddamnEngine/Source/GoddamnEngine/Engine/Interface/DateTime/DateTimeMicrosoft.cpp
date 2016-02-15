// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/DateTime/DateTimeMicrosoft.cpp
 * Microsoft date/time implementation.
 */
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_API_MICROSOFT && 0

#include <GoddamnEngine/Engine/Interface/DateTime/DateTime.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IDateTimeMicrosoft interface.
	// ==========================================================================================

	// ------------------------------------------------------------------------------------------
	//! Initializes a date/time class.
	GDAPI IResult IDateTimeMicrosoft::OnRuntimeInitialize()
	{
		LARGE_INTEGER largeInt = { 0 };
		QueryPerformanceFrequency(&largeInt);
		timeSecondsPerCycle = 1.0 / static_cast<Float64>(largeInt.QuadPart);
		QueryPerformanceCounter(&largeInt);
		timeStartTime = timeSecondsPerCycle * static_cast<Float64>(largeInt.QuadPart);

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns local time in UTC.
	//! @returns Local time in UTC.
	GDAPI IDateTimeInfo IDateTimeMicrosoft::GetDateTimeUTC()
	{
		SYSTEMTIME utcTime;
		GetSystemTime(&utcTime);

		IDateTimeInfo dateAndTime;
		dateAndTime.DateYear		= static_cast<Int32>(utcTime.wYear);
		dateAndTime.DateMonth		= static_cast<DMonthOfYear>(utcTime.wMonth);
		dateAndTime.DateWeekDay		= utcTime.wDayOfWeek != 0 ? static_cast<DDayOfWeek>(utcTime.wDayOfWeek) : IDATETIME_DAY_OF_WEEK_SUNDAY;
		dateAndTime.DateDay			= static_cast<UInt8>(utcTime.wDay);
		dateAndTime.DateHour		= static_cast<UInt8>(utcTime.wHour);
		dateAndTime.DateMinute		= static_cast<UInt8>(utcTime.wMinute);
		dateAndTime.DateSecond		= static_cast<UInt8>(utcTime.wSecond);
		dateAndTime.DateMillisecond	= static_cast<UInt16>(utcTime.wMilliseconds);

		return dateAndTime;
	}

	GDEXP IDateTime* CreateIDateTimeInstance()
	{
		return GD_NEW(IDateTimeMicrosoft);
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
