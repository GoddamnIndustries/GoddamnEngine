// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Transform.cpp
 * File contains Transform component class.
 */
#include <GoddamnEngine/Engine/Entity/Transform.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT_INTRINSIC(Transform);
	
	// ------------------------------------------------------------------------------------------
	// Local transformations.
	// ------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------
	// Global transformations.
	// ------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------------
	// Delta transformations.
	// ------------------------------------------------------------------------------------------

	gd_testing_unit_test(Transforms)
	{
		auto const parentTransform = Object::CreateOrFindClassRelatedObjectByGUID<Transform>(EmptyGUID);
		auto const childTransform = Object::CreateOrFindClassRelatedObjectByGUID<Transform>(EmptyGUID);

		parentTransform->SetEulerAngles(Vector4(0.0f, 90.0f, 0.0f, 1.0f));
		//parentTransform->SetLocalScale(Vector3Fast(12.0f, 2.0f, 3.0f));

		childTransform->SetParent(parentTransform);
		childTransform->SetLocalPosition(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

		auto const gp = childTransform->GetPosition();

		int i = 0;
	};

GD_NAMESPACE_END
