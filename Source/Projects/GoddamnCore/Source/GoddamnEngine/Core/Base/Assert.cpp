// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Assert.cpp
 * Contains error-checking mechanism definitions.
 */
#if 0
#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Provides inner functionality for handling fatal asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GD_NORETURN GDAPI extern void DebugHandleFatalAssertDataVa(DebugAssertData const* const data, va_list const args)
	{
		Debug::HandleFatalAssertData(data, args);
	}

	/*!
	 * Provides inner functionality for handling regular asserts. Should not be invoked directly.
	 *
	 * @param data Pointer to assertion description data structure.
	 * @param args Arguments for formatting Message string.
	 */
	GDAPI extern DebugAssertDialogResult DebugHandleAssertDataVa(DebugAssertData const* const data, va_list const args)
	{
		return Debug::HandleAssertData(data, args);
	}

GD_NAMESPACE_END
#endif
