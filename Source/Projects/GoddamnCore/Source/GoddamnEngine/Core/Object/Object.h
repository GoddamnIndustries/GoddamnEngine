// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Object.h
//! Object class.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Reflection.h>

GD_NAMESPACE_BEGIN
	
	struct ObjectFlags
	{
		typedef UInt32 Type;
	};	// struct ObjectFlags

	typedef ObjectFlags::Type ObjectFlagsTp;

	class ObjectArray final : IUncopiable
	{
	};

	$interface(Object $extends IReflectable)
	private:
		UInt32			ObjReferenceCount;
		SizeTp			ObjIndex;
		ObjectFlagsTp	ObjFlags;
		Object*			ObjParent;
		Object*			ObjSiblingLeft;
		Object*			ObjSiblingRight;
		Object*			ObjChildLast;

	public:

		GDAPI Object(ObjectFlagsTp const ObjFlags, Object* const ObjParent)
			: ObjReferenceCount(1)
			, ObjIndex(SizeTpMax)
			, ObjFlags(ObjFlags)
			, ObjParent(ObjParent)
			, ObjSiblingLeft(nullptr)
			, ObjSiblingRight(nullptr)
			, ObjChildLast(nullptr)
		{
			GD_DEBUG_ASSERT(ObjParent != nullptr, "Null pointer base object specified.");
		}

	$end;	// $interface Object

	enum ObjectStats : SizeTp { ObjectStatsSizeof = sizeof(Object) };

GD_NAMESPACE_END
