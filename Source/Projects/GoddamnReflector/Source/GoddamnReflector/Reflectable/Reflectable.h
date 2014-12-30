/// ==========================================================================================
/// Reflectable.h: GoddamnC++ reflectable type interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 02.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_REFLECTOR_REFLECTABLE
#define GD_REFLECTOR_REFLECTABLE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.h>
#include <GoddamnEngine/Core/Reflection/Reflectable.h>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.h>

#include <GoddamnReflector/Reflector.h>
#include <GoddamnReflector/RePreprocessor/RePreprocessor.h>

#define LockFreeList Vector

GD_NAMESPACE_BEGIN

	class OutputStream;

	struct CPPReflectable final : public CPPDefinition
	{
		String ReflectableName;
		String ReflectableBaseTypeName;
		CPPRePreprocessorDefinitions ReflectableElements;
		bool ReflectableIsFinal = false;

		/// Sets default values for fiels that were not set.
		inline void SetDefaultsForUnknowns()
		{
		}
	};	// class CPPReflectable

	/// Contains a whole list of reflectable types that were parsed in this header.
	LockFreeList<SharedPtr<CPPReflectable>> extern const& CPPReflectablesList;

GD_NAMESPACE_END

#endif
