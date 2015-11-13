// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Object.h
 * File contains base interfaces for engine entities.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Type.h>

GD_NAMESPACE_BEGIN

	class Object : public IClass
	{
		GD_DEFINE_CLASS_INFORMATION(Object, IClass);

	protected:
		GDINL Object(...) {}

	public:
	//	GD_DEFINE_CLASS_INFORMATION(Object, IClass);

		GDAPI virtual void OnInit()
		{
		}

		GDAPI virtual void OnStart()
		{
		}

		GDAPI virtual void OnUpdate()
		{
		}

		GDAPI virtual void OnRender()
		{
		}
	};	// class Object

//	GD_IMPLEMENT_CLASS_INFORMATION(Object);
#define GD_SERIALIZABLE_DEFINITION(A, B, C) GD_DEFINE_CLASS_INFORMATION(A, B)
#define GD_SERIALIZABLE_IMPLEMENTATION(A, B, C) GD_IMPLEMENT_CLASS_INFORMATION(A)
#define GD_TYPEINFORMATION_DEFINITION(A, B, C) GD_DEFINE_CLASS_INFORMATION(A, B)
#define GD_TYPEINFORMATION_IMPLEMENTATION(A, B, C) GD_IMPLEMENT_CLASS_INFORMATION(A)
#define GD_TYPEINFORMATION_IMPLEMENTATION_C(A, B, C, D) GD_SERIALIZABLE_IMPLEMENTATION(A, B, C)
#define GD_SERIALIZATION_BEGIN(...)
#define GD_SERIALIZATION_END(...)
#define GD_FIELD(A, B) A B
#define GD_TYPEID(A) A::GetTypeClassInformation()
	typedef UInt32 uint;

GD_NAMESPACE_END
