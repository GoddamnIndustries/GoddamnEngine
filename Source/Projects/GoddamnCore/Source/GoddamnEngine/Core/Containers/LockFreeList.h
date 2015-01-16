/// ==========================================================================================
/// LockFreeList.h - Lock-free list class selector.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_LOCKFREE
#define GD_CORE_CONTAINERS_LOCKFREE

#include <GoddamnEngine/Include.h>

#if (defined(GD_PLATFORM_HAS_MULTITHREADING))
#	include <GoddamnEngine/Core/Containers/LockFreeList/LockFreeList.h>
#else // if (defined(GD_PLATFORM_HAS_MULTITHREADING))
#	include <GoddamnEngine/Core/Containers/List.h>

GD_NAMESPACE_BEGIN
	
	/// @brief This type is supposed to be a lock-free list. 
	/// 	   Because of no multithreading support on target platform standart lists were used instead.
	/// @tparam ElementType Container element type.
	template<typename ElementType>
	using LockFreeList = DoubleLinkedList<ElementType>;

GD_NAMESPACE_END

#endif	// if (defined(GD_PLATFORM_HAS_MULTITHREADING))

#endif	// ifndef GD_CORE_CONTAINERS_LOCKFREE
