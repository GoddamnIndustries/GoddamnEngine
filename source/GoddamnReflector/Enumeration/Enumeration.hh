//////////////////////////////////////////////////////////////////////////
/// Enumeration.hh: GoddamnC++ enumeration interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

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

#define LockFreeList Vector

GD_NAMESPACE_BEGIN

	/// Describes enumeration types.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | Goddamn)
	enum CPPEnumerationType : UInt8
	{
		GD_CPP_ENUMERATION_TYPE_UNKNOWN                             = 0, ///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_TYPE_DEFAULT                             = 1, ///< Default enumeration type (enumeration).
		GD_CPP_ENUMERATION_TYPE_ENUMERATION                         = 1, ///< This enumeration contains values that can be represented as Bitset (array of booleans).
		GD_CPP_ENUMERATION_TYPE_BITSET                              = 2, ///< This enumeration contains values that can be represented as List.
	};	// enum CPPEnumerationType

	/// Describes enumeration values strinigfication Chopped policy.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | Goddamn)
	enum CPPEnumerationStringificationChoppingPolicy : UInt8
	{
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_UNKNOWN  = 0, ///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_DEFAULT  = 1, ///< Default stringification choppping policy (stubbed).
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING = 1, ///< Enumeration values common parts would be stubbed when stringified. E.g. (MY_ENUM_A, MY_ENUM_B) would be stringified as ("A", "B").
		GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_FULL     = 2, ///< Enumeration values would be strinigfied fully. E.g. (MY_ENUM_A, MY_ENUM_B) would be stringified as ("MY_ENUM_A", "MY_ENUM_B").
	};	// enum CPPEnumerationStringificationChoppingPolicy

	/// Describes enumeration values strinigfication case policy.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | Goddamn)
	enum CPPEnumerationStringificationCasePolicy : UInt8
	{
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UNKNOWN      = 0, ///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_DEFAULT      = 1, ///< Default stringification case policy (Goddamn casing policy).
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN      = 1, ///< Enumeration values would be strinigfied using Goddamn casing policy (PascalCase like).
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_AS_IS        = 2, ///< Enumeration values would be strinigfied as is.
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_LOWER_CASE   = 3, ///< Enumeration values would be strinigfied in lower case.
		GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UPPER_CASE   = 4, ///< Enumeration values would be strinigfied in upper case.
	};	// enum CPPEnumerationStringificationCasePolicy

	/// Describes enumeration stringiciation methods export policy.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | Goddamn)
	enum CPPEnumerationStringificationExportPolicy : UInt8
	{
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_UNKNOWN    = 0,	///< Unknown (internal usage only).
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_DEFAULT    = 1, ///< Default stringiciation methods export policy (public).
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC     = 1, ///< '*ToStr' and '*FromStr' methods would be publically exported from generated code.
		GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_INTERNAL   = 2, ///< '*ToStr' and '*FromStr' methods stay internal for generated code.
	};	// enum CPPEnumerationStringificationExportPolicy

	/// Describes GoddamnC++ enumeration definition.
	struct CPPEnumeration final : public CPPDefinition
	{
		String EnumerationName;                                                                                                      ///< Name of this enumeration.
		String EnumerationBaseTypeName;                                                                                              ///< Name of enumeration base type name (if does not inherits from other enum).
		UnorderedMap<String, UInt64> EnumerationValues;                                                                              ///< List of enumeration definition names and values.
		SharedPtr<CPPEnumeration const> EnumerationBaseEnumeration;                                                                  ///< Pointer to base enum of this.
		CPPEnumerationType EnumerationType = GD_CPP_ENUMERATION_TYPE_DEFAULT;                                                        ///< Describes enumeration types.
		struct {
			CPPEnumerationStringificationChoppingPolicy ChoppingPolicy = GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_DEFAULT; ///< Describes enumeration values strinigfication Chopped policy.
			CPPEnumerationStringificationCasePolicy     CasePolicy     = GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_DEFAULT;     ///< Describes enumeration values strinigfication case policy.
			CPPEnumerationStringificationExportPolicy   ExportPolicy   = GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_DEFAULT;   ///< Describes enumeration stringiciation methods export policy.
		} EnumerationStringification;	                                                                                             ///< Describes enumeration values strinigfication policy.
	};	// struct CPPEnumeration

	/// Parses 
	class CPPEnumerationParser final : public IToolchainTool
	{
	public:
		GDINT explicit CPPEnumerationParser(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINT virtual ~CPPEnumerationParser() { }

		GDAPI SharedPtr<CPPEnumeration const> ParseEnumerationDefinition();
	};	// class CPPEnumerationParser

	/// Contains a whole list of enumerations that were parsed in this header.
	LockFreeList<SharedPtr<CPPEnumeration const>> extern const& CPPEnumerationsList;

GD_NAMESPACE_END

#endif
