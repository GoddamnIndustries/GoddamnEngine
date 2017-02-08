// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Base.h
 * File contains base declaration for entities system.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	#define GD_OBJECT_KERNEL	//!< Use this macro to mark declarations inside Object system, that couldn't be implemented outside Object.cpp
	#define GD_OBJECT_HELPER	//!< Use this macro to mark declarations inside Object system, that use kernel API and relate to the Object system.
	#define GD_OBJECT_GENERATED	//!< Use this macro to mark declarations inside Object system, that would be generated.

	struct ObjectVisitor;
	struct ObjectClassVisitor;
	struct Struct;
	struct StructMetaInfo;
	struct PropertyMetaInfo;
	class  Object;
	struct ObjectCtorInfo;
	class  Class;
	class  Object;
	class  Package;

	/*!
	 * Represents constructor procedure pointer for objects.
	 * @param objectCtorInfo Construction information for the object.
	 */
	typedef GD_OBJECT_KERNEL Object*(*ObjectCtorProc)(ObjectCtorInfo const& objectCtorInfo);

	template<typename TObject>
	GD_OBJECT_HELPER struct ObjectCtorGetter final : public TNonCreatable
	{
	public:
		template<typename TObjectCtorProc = typename DisableIf<TypeTraits::IsAbstract<TObject>::Value, ObjectCtorProc>::Type>
		GDINL static TObjectCtorProc Get()
		{
			return [](ObjectCtorInfo const& objectCtorInfo) -> Object* { return gd_new TObject(objectCtorInfo); };
		}
	};	// struct ObjectCtorGetter

	/*!
	 * Represents pointer to class information.
	 */
	typedef GD_OBJECT_HELPER /*NotNull<*/ Class const* /*>*/ ClassPtr;

	template<typename TObjectSource, typename TObjectDest>
	GDINL GD_OBJECT_KERNEL static TObjectDest* object_cast(TObjectSource* const sourceObject);

GD_NAMESPACE_END
