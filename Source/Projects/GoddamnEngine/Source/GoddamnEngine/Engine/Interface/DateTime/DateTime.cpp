// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#if 0

/*!
 * @file GoddamnEngine/Engine/Interface/DateTime/DateTime.cpp
 * Cross-platform date/time implementations.
 */
#include <GoddamnEngine/Engine/Interface/DateTime/DateTime.h>
//#include <GoddamnEngine/Engine/Pipeline/Pipeline.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IDateTime interface.
	// ==========================================================================================

	GDAPI extern IDateTime* CreateIDateTimeInstance();

	// ------------------------------------------------------------------------------------------
	//! Global input interface pointer.
	//GDAPI IUniqueInterfacePointer<IDateTime> DateTime(CreateIDateTimeInstance(), PipelineManager->GetMainThreadPipeline());
	GD_SINGLETON_IMPLEMENTATION(IDateTime);

	// ------------------------------------------------------------------------------------------
	//! Initializes a date/time class.
	GDAPI IResult IDateTime::OnRuntimeInitialize()
	{
		timeSecondsPerCycle = static_cast<Float64>(CLOCKS_PER_SEC);
		timeStartTime = static_cast<Float64>(::clock()) * timeSecondsPerCycle;

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Ticks a frame.
	GDAPI void IDateTime::OnRuntimeUpdate()
	{
		auto const CurrentTime = GetSeconds();
		timeFrameDeltaTime = CurrentTime - timeFrameStartTime;
		timeFrameStartTime = CurrentTime;
		timeFramesCount++;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns time in seconds passed till Engine launch as a pretty formatted string.
	//! @returns Time in seconds passed till Engine launch as a pretty formatted string.
	GDAPI String IDateTime::GetSecondsString()
	{
		auto const QueriedSeconds = GetSeconds();
		if (QueriedSeconds < 1.0)
		{
			auto const Milliseconds = static_cast<Int32>(QueriedSeconds * 1000.0);
			return String::Format("%d ms", Milliseconds);
		}
		if (QueriedSeconds < 10.0)
		{
			auto const Seconds      = static_cast<Int32>(QueriedSeconds);
			auto const Milliseconds = static_cast<Int32>(QueriedSeconds * 1000.0) - Seconds;
			return String::Format("%d.%02d sec", Seconds, Milliseconds / 10);
		}
		if (QueriedSeconds < 60.0)
		{
			auto const Seconds      = static_cast<Int32>(QueriedSeconds);
			auto const Milliseconds = static_cast<Int32>(QueriedSeconds * 1000.0) - Seconds;
			return String::Format("%d.%d sec", Seconds, Milliseconds / 100);
		}
		if (QueriedSeconds < 60.0 * 60.0)
		{
			auto const Minutes = static_cast<Int32>(QueriedSeconds / 60.0);
			auto const Seconds = static_cast<Int32>(QueriedSeconds) - (Minutes * 60);
			return String::Format("%d:%02d min", Minutes, Seconds);
		}

		auto const Hours   = static_cast<Int32>(QueriedSeconds / (60.0 * 60.0));
		auto const Minutes = static_cast<Int32>(QueriedSeconds / 60.0) - (Hours * 60);
		auto const Seconds = static_cast<Int32>(QueriedSeconds) - (Minutes * 60) - (Hours * 60 * 60);
		return String::Format("%d:%02d:%02d hours", Hours, Minutes, Seconds);
	}

	// ------------------------------------------------------------------------------------------
	//! Returns local time in UTC.
	//! @returns Local time in UTC.
	GDAPI IDateTimeInfo IDateTime::GetDateTimeUTC()
	{
		auto const RawCurrentTime = ::time(nullptr);
		tm UTCTime = { 0 };
#if GD_PLATFORM_WINDOWS
		::gmtime_s(&UTCTime, &RawCurrentTime);
#else	// if GD_PLATFORM_WINDOWS
		::gmtime_r(&UTCTime, &RawCurrentTime);
#endif	// if GD_PLATFORM_WINDOWS
		IDateTimeInfo DateAndTime;
		DateAndTime.DateYear		= static_cast<Int32>(UTCTime.tm_year);
		DateAndTime.DateMonth		= static_cast<DMonthOfYear>(UTCTime.tm_mon);
		DateAndTime.DateWeekDay		= UTCTime.tm_wday != 0 ? static_cast<DDayOfWeek>(UTCTime.tm_wday) : IDATETIME_DAY_OF_WEEK_SUNDAY;
		DateAndTime.DateDay			= static_cast<UInt8>(UTCTime.tm_mday);
		DateAndTime.DateHour		= static_cast<UInt8>(UTCTime.tm_hour);
		DateAndTime.DateMinute		= static_cast<UInt8>(UTCTime.tm_min);
		DateAndTime.DateSecond		= static_cast<UInt8>(UTCTime.tm_sec);
		DateAndTime.DateMillisecond = 0;

		return DateAndTime;
	}

	// ------------------------------------------------------------------------------------------
	//! Queries local date in UTC.
	//! @returns Local date in UTC as a string.
	GDAPI String IDateTime::GetDateUTCString()
	{
		auto DateAndTime = GetDateTimeUTC();
		return String::Format("%02d:%02d:%02d"
			, static_cast<Int32>(DateAndTime.DateDay),  static_cast<Int32>(DateAndTime.DateMonth),  static_cast<Int32>(DateAndTime.DateYear));
	}

	// ------------------------------------------------------------------------------------------
	//! Queries local time in UTC.
	//! @returns Local time in UTC as a string.
	GDAPI String IDateTime::GetTimeUTCString()
	{
		auto DateAndTime = GetDateTimeUTC();
		return String::Format("%02d:%02d:%02d"
			, static_cast<Int32>(DateAndTime.DateHour), static_cast<Int32>(DateAndTime.DateMinute), static_cast<Int32>(DateAndTime.DateSecond));
	}

	// ------------------------------------------------------------------------------------------
	//! Queries local date and time in UTC.
	//! @returns Local date and time in UTC as a string.
	GDAPI String IDateTime::GetDateTimeUTCString()
	{
		auto DateAndTime = GetDateTimeUTC();
		return String::Format("%02d/%02d/%02d %02d:%02d:%02d"
			, static_cast<Int32>(DateAndTime.DateDay),  static_cast<Int32>(DateAndTime.DateMonth),  static_cast<Int32>(DateAndTime.DateYear)
			, static_cast<Int32>(DateAndTime.DateHour), static_cast<Int32>(DateAndTime.DateMinute), static_cast<Int32>(DateAndTime.DateSecond));
	}

GD_NAMESPACE_END

#endif
