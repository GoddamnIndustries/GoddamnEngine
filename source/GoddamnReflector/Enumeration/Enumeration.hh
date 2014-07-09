/// ==========================================================================================
/// Enumeration.hh: GoddamnC++ enumeration interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_REFLECTOR_ENUMERATION
#define GD_REFLECTOR_ENUMERATION

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>
#include <GoddamnEngine/Core/Containers/Map/Map.hh>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.hh>
#include <GoddamnEngine/Core/Reflection/Reflectable.hh>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.hh>

#include <GoddamnReflector/Reflector.hh>
#include <GoddamnReflector/RePreprocessor/RePreprocessor.hh>

#define LockFreeList Vector

GD_NAMESPACE_BEGIN

	class OutputStream;

	/// Describes enumeration types.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | GoddamnCase)
	enum CPPEnumerationType : UInt8
	{
		GD_CPP_ENUMERATION_TYPE_UNKNOWN                             = 0, ///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_TYPE_ENUMERATION                         = 1, ///< This enumeration contains values that can be represented as Bitset (array of booleans).
		GD_CPP_ENUMERATION_TYPE_BITSET                              = 2, ///< This enumeration contains values that can be represented as List.
	};	// enum CPPEnumerationType

	/// Describes enumeration values strinigfication Chopped policy.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | GoddamnCase)
	enum CPPEnumerationStringificationChoppingPolicy : UInt8
	{
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_UNKNOWN  = 0, ///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING = 1, ///< Enumeration values common parts would be stubbed when stringified. E.g. (MY_ENUM_A, MY_ENUM_B) would be stringified as ("A", "B").
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_FULL     = 2, ///< Enumeration values would be strinigfied fully. E.g. (MY_ENUM_A, MY_ENUM_B) would be stringified as ("MY_ENUM_A", "MY_ENUM_B").
	};	// enum CPPEnumerationStringificationChoppingPolicy

	/// Describes enumeration values strinigfication case policy.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | GoddamnCase)
	enum CPPEnumerationStringificationCasePolicy : UInt8
	{
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UNKNOWN      = 0, ///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN      = 1, ///< Enumeration values would be strinigfied using GoddamnCase casing policy (PascalCase like).
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_AS_IS        = 2, ///< Enumeration values would be strinigfied as is.
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_LOWER_CASE   = 3, ///< Enumeration values would be strinigfied in lower case.
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UPPER_CASE   = 4, ///< Enumeration values would be strinigfied in upper case.
	};	// enum CPPEnumerationStringificationCasePolicy

	/// Describes enumeration stringiciation methods export policy.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | GoddamnCase)
	enum CPPEnumerationStringificationExportPolicy : UInt8
	{
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_UNKNOWN    = 0,	///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC     = 1, ///< '*ToStr' and '*FromStr' methods would be publically exported from generated code.
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_INTERNAL   = 2, ///< '*ToStr' and '*FromStr' methods stay internal for generated code.
	};	// enum CPPEnumerationStringificationExportPolicy

	/// Describes GoddamnC++ enumeration element definition.
	struct CPPEnumerationElement final : CPPDefinition
	{
		String Name;
	};	// struct CPPEnumerationElement

	/// Describes GoddamnC++ enumeration definition.
	struct CPPEnumeration final : public CPPDefinition
	{
		String EnumerationName;                                                                                                      ///< Name of this enumeration.
		String EnumerationBaseTypeName = "int";                                                                                      ///< Name of enumeration base type name (if does not inherits from other enum).
		CPPRePreprocessorDefinitions EnumerationElements;                                                                            ///< List of enumeration definition names and values.
		SharedPtr<CPPEnumeration const> EnumerationBaseEnumeration;                                                                  ///< Pointer to base enum of this.
		CPPEnumerationType EnumerationType = GD_CPP_ENUMERATION_TYPE_UNKNOWN;                                                        ///< Describes enumeration types.
		struct {
			CPPEnumerationStringificationChoppingPolicy ChoppingPolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_UNKNOWN; ///< Describes enumeration values strinigfication Chopped policy.
			CPPEnumerationStringificationCasePolicy     CasePolicy     = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UNKNOWN;     ///< Describes enumeration values strinigfication case policy.
			CPPEnumerationStringificationExportPolicy   ExportPolicy   = GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_UNKNOWN;   ///< Describes enumeration stringiciation methods export policy.
		} EnumerationStringification;	                                                                                             ///< Describes enumeration values strinigfication policy.

		/// Sets default values for fiels that were not set.
		/// EnumerationType is set to GD_CPP_ENUMERATION_TYPE_ENUMERATION.
		/// EnumerationStringification.ChoppingPolicy is set to GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING.
		/// EnumerationStringification.CasePolicy is set to GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN.
		/// EnumerationStringification.ExportPolicy is set to GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC.
		inline void SetDefaultsForUnknowns()
		{
			if (self->EnumerationType == GD_CPP_ENUMERATION_TYPE_UNKNOWN)
				self->EnumerationType  = GD_CPP_ENUMERATION_TYPE_ENUMERATION;
			
			if (self->EnumerationStringification.ChoppingPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_UNKNOWN)
				self->EnumerationStringification.ChoppingPolicy  = GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING;
			
			if (self->EnumerationStringification.CasePolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UNKNOWN)
				self->EnumerationStringification.CasePolicy  = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN;
			
			if (self->EnumerationStringification.ExportPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_UNKNOWN)
				self->EnumerationStringification.ExportPolicy  = GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC;
		}
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
