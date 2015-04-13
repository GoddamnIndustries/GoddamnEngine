// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Microsoft/MicrosoftDateTime.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Core/Platform/DateTime.h> instead.
//! Microsoft date/time definitions.
#pragma once
#if !defined(GD_INSIDE_DATETIME_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Core/Platform/DateTime.h> instead.
#endif	// if !defined(GD_INSIDE_DATETIME_H)

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Platform/Platform.h>
#include <GoddamnEngine/Core/Platform/DateTime.h>

#if !GD_PLATFORM_API_MICROSOFT
#	error Current implementation of output device is not supported on this platform.
#endif	// if !GD_PLATFORM_API_MICROSOFT

#include <Windows.h>

GD_NAMESPACE_BEGIN

	//! @brief Defines a Microsoft date/time manager class.
	class MicrosoftDateTime : public IGenericDateTime
	{
	public:

		//! @brief Initializes a date/time class.
		GDINL MicrosoftDateTime() { }

		//! @brief Deinitializes a date/time class.
		GDAPI virtual ~MicrosoftDateTime() { }

	protected:

		//! @brief Initializes a date/time class.
		GD_PLATFORM_OVERRIDE_METHOD0(void, InitializeObject, override);

	public:

		//! @brief Returns time in seconds passed till Engine launch.
		//! @returns Time in seconds passed till Engine launch.
		GD_PLATFORM_OVERRIDE_METHOD0(Float64, GetSeconds, override
		{
			LARGE_INTEGER TimeTicksCount;
			QueryPerformanceCounter(&TimeTicksCount);
			return this->TimeSecondsPerCycle * static_cast<Float64>(TimeTicksCount.QuadPart) - this->TimeStartTime;
		});

		//! @brief Returns local time in UTC.
		//! @returns Local time in UTC.
		//! @note Not all implementations can return full time. Unresolved fields would be set to zero.
		GD_PLATFORM_OVERRIDE_METHOD0(DateTimeTp, GetDateTimeUTC);

	};	// class MicrosoftDateTime

GD_NAMESPACE_END
