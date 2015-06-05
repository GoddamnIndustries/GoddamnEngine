// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Misc/MarshalAs.h
//! Types marshaling and etc.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	//! Converts instance of some type into JSON-like intermediate string representation.
	//! @param Value Instance to be marshaled as string.
	//! @returns JSON-like intermediate string representation of this instance.
	//! @{
	template<typename Tp>
	GDINL static String MarshalAsString(Tp const& Value)
	{
		GD_NOT_USED(Value);
		GD_ASSERT_FALSE("Marshalling is not supported for this type. Please, implement it directly.");
	}
	template<typename Tp>
	GDINL static String MarshalAsStringRaw(CHandle const Value)
	{
		return MarshalAsString(*reinterpret_cast<Tp const*>(Value));
	}
	typedef String(*MarshalAsStringProc)(CHandle const Value);
	//! @}

	//! Converts JSON-like intermediate string representation into instance of some type.
	//! @param Value Instance to be initializes with marshaled string.
	//! @param Data String data to be marshaled as instance.
	//! @{
	template<typename Tp>
	GDINL static void MarshalStringAs(Tp& Value, String const& Data)
	{
		GD_NOT_USED(Value);
		GD_NOT_USED(Data);
		GD_ASSERT_FALSE("Marshalling is not supported for this type. Please, implement it directly.");
	}
	template<typename Tp>
	GDINL static void MarshalStringAsRaw(Handle const Value, String const& Data)
	{
		MarshalStringAsRaw(*reinterpret_cast<Tp*>(Value), Data);
	}
	typedef void (*MarshalStringAsProc)(Handle const Value, String const& Data);
	//! @}

GD_NAMESPACE_END
