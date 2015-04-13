// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftDateTime.cpp
//! Microsoft date/time definitions.

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_API_MICROSOFT

#include <GoddamnEngine/Core/Platform/DateTime.h>

GD_NAMESPACE_BEGIN

	//! @brief Initializes a date/time class.
	GDAPI void MicrosoftDateTime::InitializeObject_Inst()
	{
		LARGE_INTEGER LargeInt = { 0 };
		QueryPerformanceFrequency(&LargeInt);
		this->TimeSecondsPerCycle = 1.0 / static_cast<Float64>(LargeInt.QuadPart);
		QueryPerformanceCounter(&LargeInt);
		this->TimeStartTime = this->TimeSecondsPerCycle * static_cast<Float64>(LargeInt.QuadPart);
	}

	//! @brief Returns local time in UTC.
	//! @returns Local time in UTC.
	GDAPI DateTimeTp MicrosoftDateTime::GetDateTimeUTC_Inst()
	{
		SYSTEMTIME UTCTime;
		GetSystemTime(&UTCTime);

		DateTimeTp DateAndTime;
		DateAndTime.Year		= static_cast<Int32>(UTCTime.wYear);
		DateAndTime.Month		= static_cast<MonthOfYearTp>(UTCTime.wMonth);
		DateAndTime.WeekDay		= (UTCTime.wDayOfWeek != 0) ? static_cast<DayOfWeekTp>(UTCTime.wDayOfWeek) : DayOfWeek::Sunday;
		DateAndTime.Day			= static_cast<UInt8>(UTCTime.wDay);
		DateAndTime.Hour		= static_cast<UInt8>(UTCTime.wHour);
		DateAndTime.Minute		= static_cast<UInt8>(UTCTime.wMinute);
		DateAndTime.Second		= static_cast<UInt8>(UTCTime.wSecond);
		DateAndTime.Millisecond	= static_cast<UInt16>(UTCTime.wMilliseconds);

		return DateAndTime;
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
