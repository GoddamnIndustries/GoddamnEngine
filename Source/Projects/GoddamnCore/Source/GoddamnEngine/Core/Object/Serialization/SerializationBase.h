// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/SerializationBase.h
 * File contains base type information enumeration.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN
	
	enum class SerializationTypeInfo : UInt8
	{
		Unknown,
		Null,	// specially for JSON.
		Bool    = 'b',
		Int8    = 'c',
		UInt8   = 'C',
		Int16   = 's',
		UInt16  = 'S',
		Int32   = 'i',
		UInt32  = 'I',
		Int64   = 'l',
		UInt64  = 'L',
		Float32 = 'f',
		Float64 = 'd',
		String  = 'Z',
		GUID    = 'G',
		Array   = 'A',
		Struct  = 'O',
		Object  = Struct,
	};	// enum class SerializationTypeInfo

GD_NAMESPACE_END
