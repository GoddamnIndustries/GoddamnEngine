/// ==========================================================================================
/// Enumeration.h: GoddamnC++ enumeration interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_REFLECTOR_ENUMERATION
#define GD_REFLECTOR_ENUMERATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Text/String/String.h>
#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.h>
#include <GoddamnEngine/Core/Reflection/Reflectable.h>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.h>

#include <GoddamnReflector/Reflector.h>
#include <GoddamnReflector/RePreprocessor/RePreprocessor.h>

#define LockFreeList Vector

GD_NAMESPACE_BEGIN

	class OutputStream;

	/// Describes enumeration types.
	enum class CPPEnumerationType : UInt8
	{
		Unknown                             = 0, ///< Unknown (internal usage only).
		Enumeration                         = 1, ///< This enumeration contains values that can be represented as Bitset (array of booleans).
		Bitset                              = 2, ///< This enumeration contains values that can be represented as List.
	};	// enum CPPEnumerationType

	/// Describes enumeration stringiciation methods export policy.
	enum CPPEnumerationExportPolicy : UInt8
	{
		Unknown    = 0,	///< Unknown (internal usage only).
		Public     = 1, ///< '*ToStr' and '*FromStr' methods would be publically exported from generated code.
		Internal   = 2, ///< '*ToStr' and '*FromStr' methods stay internal for generated code.
	};	// enum CPPEnumerationStringificationExportPolicy

	/// Describes GoddamnC++ enumeration element definition.
	struct CPPEnumerationElement final : CPPDefinition
	{
		String Name;
	};	// struct CPPEnumerationElement

	/// Describes GoddamnC++ enumeration definition.
	struct CPPEnumeration final : public CPPDefinition
	{
		String EnumerationName;															///< Name of this enumeration.
		String EnumerationBaseTypeName = "int";                                         ///< Name of enumeration base type name (if does not inherits from other enum).
		CPPRePreprocessorDefinitions EnumerationElements;								///< List of enumeration definition names and values.
		SharedPtr<CPPEnumeration const> EnumerationBaseEnumeration;						///< Pointer to base enum of this.
		CPPEnumerationType EnumerationType = CPPEnumerationType::Unknown;				///< Describes enumeration types.
		CPPEnumerationExportPolicy ExportPolicy = CPPEnumerationExportPolicy::Unknown;	/// Describes enumeration stringiciation methods export policy.
	};	// struct CPPEnumeration

	/// Contains a whole list of enumerations that were parsed in this header.
	LockFreeList<SharedPtr<CPPEnumeration>> extern const& CPPEnumerationsList;

	/// Fills the array with default enumeration values names.
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationFillNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames);

	/// Chops enumeration values common parts E.g. (MY_ENUM_E1RT_A, MY_ENUM_E2RT_A) would be stringified as ("E1RT_A", "E2RT_A").
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationChopNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames);

	/// Translates enumeration values capicalization styles.
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationTranslateCasePolicy(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames);

	/// Generates stringiciators methods for specified enumeration.
	/// @param Enumeration                 Pointer on enumeration description.
	/// @param OutputGeneratedHeaderStream Stream to which stringiciators interfaces would be written.
	/// @param OutputGeneratedSourceStream Stream to which stringiciators implementations would be written.
	extern void CPPEnumerationWriteStringificators(SharedPtr<CPPEnumeration const> const& Enumeration, OutputStream* const OutputGeneratedHeaderStream, OutputStream* const OutputGeneratedSourceStream);

GD_NAMESPACE_END

#endif
