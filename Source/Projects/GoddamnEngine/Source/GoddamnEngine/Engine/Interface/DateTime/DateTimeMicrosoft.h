// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================


/*! 
 * @file GoddamnEngine/Engine/Interface/DateTime/DateTimeMicrosoft.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Engine/Interface/DateTime/DateTime.h> instead.
 * Microsoft date/time definitions.
 */
#pragma once
#if !defined(GD_INSIDE_DATETIME_H)
//#	error This file should be never directly included, please consider using <GoddamnEngine/Engine/Interface/DateTime.h> instead.
#endif	// if !defined(GD_INSIDE_DATETIME_H)

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_API_MICROSOFT

#include <GoddamnEngine/Engine/Interface/UniqueInterface.h>
#include <GoddamnEngine/Engine/Interface/DateTime/DateTime.h>

#include <Windows.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Defines a Microsoft date/time manager class.
	GD_DINTERFACE()
	class IDateTimeMicrosoft : public IDateTime
	{
	public:
		GDINL IDateTimeMicrosoft() { }
		GDAPI virtual ~IDateTimeMicrosoft() { }

	protected:

		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimeInitialize() override final;

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns time in seconds passed till Engine launch.
		//! @returns Time in seconds passed till Engine launch.
		GD_DFUNCTION()
		GDAPI virtual Float64 GetSeconds() override final
		{
			LARGE_INTEGER TimeTicksCount;
			QueryPerformanceCounter(&TimeTicksCount);
			return timeSecondsPerCycle * static_cast<Float64>(TimeTicksCount.QuadPart) - timeStartTime;
		}

		// ------------------------------------------------------------------------------------------
		//! Returns local time in UTC.
		//! @returns Local time in UTC.
		//! @note Not all implementations can return full time. Unresolved fields would be set to zero.
		GD_DFUNCTION()
		GDAPI virtual IDateTimeInfo GetDateTimeUTC() override final;

	};	// class IDateTimeMicrosoft

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
