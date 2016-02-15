// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/System/Threading/LockFreeStack.h
 * File contains cross-platform Lock-Free stack implementation.
 */
#pragma once
#define GD_INSIDE_LOCKFREESTACK_H

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/System/Platform.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Generic Lock-Free stack class.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	template<typename TElement>
	class LockFreeStackGeneric : public TNonCopyable
	{
	public:

		/*!
		 * Initializes an empty lock-free stack.
		 */
		GDINL LockFreeStackGeneric()
		{
		}

		GDINL ~LockFreeStackGeneric()
		{
		}

	public:

		/*!
		 * Appends new element to the lock-free stack.
		 * @param newElement New element that would be inserted into the end of container.
		 */
		//! @{
		GDINL void PushBack(TElement&& newElement = TElement())
		{
			GD_NOT_USED(newElement);
			GD_NOT_IMPLEMENTED();
		}
		GDINL void PushBack(TElement const& newElement)
		{
			GD_NOT_USED(newElement);
			GD_NOT_IMPLEMENTED();
		}
		//! @}

		/*!
		 * Removes last element from the lock-free stack.
		 *
		 * @param outElement Reference for the output.
		 * @returns False if stack is empty.
		 */
		GDINL bool PopBack(TElement& outElement)
		{
			GD_NOT_USED(outElement);
			GD_NOT_IMPLEMENTED();
		}

		/*!
		 * Destroys all elements in the stack.
		 */
		GDINL void Clear()
		{
			GD_NOT_IMPLEMENTED();
		}

	};	// class LockFreeStackGeneric

GD_NAMESPACE_END

// ReSharper disable once CppUnusedIncludeDirective
#include GD_PLATFORM_API_INCLUDE(GoddamnEngine/Core/System/Threading, LockFreeStack)
#undef GD_INSIDE_LOCKFREESTACK_H
