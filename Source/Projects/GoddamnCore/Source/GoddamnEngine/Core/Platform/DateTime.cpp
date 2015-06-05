// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Platform/DateTime.cpp
//! Cross-platform date/time definitions.

#include <GoddamnEngine/Core/Platform/DateTime.h>

GD_NAMESPACE_BEGIN

	//! Initializes a date/time class.
	GDAPI void IGenericDateTime::InitializeObject_Inst()
	{
		this->TimeSecondsPerCycle = static_cast<Float64>(CLOCKS_PER_SEC);
		this->TimeStartTime = static_cast<Float64>(std::clock()) * this->TimeSecondsPerCycle;
	}

	//! Ticks a frame.
	GDAPI void IGenericDateTime::TickObject_Inst()
	{
		Float64 CurrentTime = DateTime::GetSeconds();
		this->TimeFrameDeltaTime = CurrentTime - this->TimeFrameStartTime;
		this->TimeFrameStartTime = CurrentTime;
		this->TimeFramesCount++;
	}

	//! Returns time in seconds passed till Engine launch as a pretty formatted string.
	//! @returns Time in seconds passed till Engine launch as a pretty formatted string.
	GDAPI String IGenericDateTime::GetSecondsString_Inst()
	{
		auto const QueriedSeconds = DateTime::GetSeconds();
		if (QueriedSeconds < 1.0)
		{
			auto const Milliseconds = static_cast<Int32>(QueriedSeconds * 1000.0);
			return String::Format("%d ms", Milliseconds);
		}
		else if (QueriedSeconds < 10.0)
		{
			auto const Seconds      = static_cast<Int32>(QueriedSeconds);
			auto const Milliseconds = static_cast<Int32>(QueriedSeconds * 1000.0) - Seconds;
			return String::Format("%d.%02d sec", Seconds, Milliseconds / 10);
		}
		else if (QueriedSeconds < 60.0)
		{
			auto const Seconds      = static_cast<Int32>(QueriedSeconds);
			auto const Milliseconds = static_cast<Int32>(QueriedSeconds * 1000.0) - Seconds;
			return String::Format("%d.%d sec", Seconds, Milliseconds / 100);
		}
		else if (QueriedSeconds < 60.0 * 60.0)
		{
			auto const Minutes = static_cast<Int32>(QueriedSeconds / 60.0);
			auto const Seconds = static_cast<Int32>(QueriedSeconds) - (Minutes * 60);
			return String::Format("%d:%02d min", Minutes, Seconds);
		}
		else
		{
			auto const Hours   = static_cast<Int32>(QueriedSeconds / (60.0 * 60.0));
			auto const Minutes = static_cast<Int32>(QueriedSeconds / 60.0) - (Hours * 60);
			auto const Seconds = static_cast<Int32>(QueriedSeconds) - (Minutes * 60) - (Hours * 60 * 60);
			return String::Format("%d:%02d:%02d hours", Hours, Minutes, Seconds);
		}
	}

	//! Returns local time in UTC.
	//! @returns Local time in UTC.
	GDAPI DateTimeTp IGenericDateTime::GetDateTimeUTC_Inst()
	{
		auto const RawCurrentTime = std::time(nullptr);
		tm UTCTime = { 0 };
#if GD_PLATFORM_WINDOWS
		::gmtime_s(&UTCTime, &RawCurrentTime);
#else	// if GD_PLATFORM_WINDOWS
		::gmtime_r(&UTCTime, &RawCurrentTime);
#endif	// if GD_PLATFORM_WINDOWS
		DateTimeTp DateAndTime;
		DateAndTime.Year		= static_cast<Int32>(UTCTime.tm_year);
		DateAndTime.Month		= static_cast<MonthOfYearTp>(UTCTime.tm_mon);
		DateAndTime.WeekDay		= (UTCTime.tm_wday != 0) ? static_cast<DayOfWeekTp>(UTCTime.tm_wday) : DayOfWeek::Sunday;
		DateAndTime.Day			= static_cast<UInt8>(UTCTime.tm_mday);
		DateAndTime.Hour		= static_cast<UInt8>(UTCTime.tm_hour);
		DateAndTime.Minute		= static_cast<UInt8>(UTCTime.tm_min);
		DateAndTime.Second		= static_cast<UInt8>(UTCTime.tm_sec);
		DateAndTime.Millisecond = 0;

		return DateAndTime;
	}

	//! Queries local date in UTC.
	//! @returns Local date in UTC as a string.
	GDAPI String IGenericDateTime::GetDateUTCString_Inst()
	{
		auto DateAndTime = this->GetDateTimeUTC();
		return String::Format("%02d:%02d:%02d"
			, static_cast<Int32>(DateAndTime.Day),  static_cast<Int32>(DateAndTime.Month),  static_cast<Int32>(DateAndTime.Year));
	}

	//! Queries local time in UTC.
	//! @returns Local time in UTC as a string.
	GDAPI String IGenericDateTime::GetTimeUTCString_Inst()
	{
		auto DateAndTime = this->GetDateTimeUTC();
		return String::Format("%02d:%02d:%02d"
			, static_cast<Int32>(DateAndTime.Hour), static_cast<Int32>(DateAndTime.Minute), static_cast<Int32>(DateAndTime.Second));
	}

	//! Queries local date and time in UTC.
	//! @returns Local date and time in UTC as a string.
	GDAPI String IGenericDateTime::GetDateTimeUTCString_Inst()
	{
		auto DateAndTime = this->GetDateTimeUTC();
		return String::Format("%02d/%02d/%02d %02d:%02d:%02d"
			, static_cast<Int32>(DateAndTime.Day),  static_cast<Int32>(DateAndTime.Month),  static_cast<Int32>(DateAndTime.Year)
			, static_cast<Int32>(DateAndTime.Hour), static_cast<Int32>(DateAndTime.Minute), static_cast<Int32>(DateAndTime.Second));
	}

GD_NAMESPACE_END
