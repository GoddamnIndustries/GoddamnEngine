// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/ObjectTests.cpp
 * File contains unit tests for entities subsystem.
 */
#include <GoddamnEngine/Core/Object/Object.h>

GD_NAMESPACE_BEGIN

#if GD_TESTING_ENABLED 

	class TestBase1Class : public Object
	{
		GD_DECLARE_OBJECT(TestBase1Class, Object)
		GD_PROPERTY(public, int, FirstPoperty) = 1;
		GD_PROPERTY(private, float, SecondPoperty) = 2.0f;

	public:
		virtual void DoNothing() GD_PURE_VIRTUAL;
	};	// class TestBase1Class
	GD_IMPLEMENT_OBJECT(TestBase1Class)

	class TestBase2Class : public Object
	{
		GD_DECLARE_OBJECT(TestBase2Class, Object)
	};	// class TestBase2Class
	GD_IMPLEMENT_OBJECT(TestBase2Class)

	class TestDerived1Class : public TestBase1Class
	{
		GD_DECLARE_OBJECT(TestDerived1Class, TestBase1Class)
		GD_PROPERTY(protected, String, ThirdProperty) = "3";

	public:
		void DoNothing() override {}
	};	// class TestDerived1Class
	GD_IMPLEMENT_OBJECT(TestDerived1Class)

	class TestDerived2Class : public TestBase2Class
	{
		GD_DECLARE_OBJECT(TestDerived2Class, TestBase2Class)
	};	// class TestDerived2Class
	GD_IMPLEMENT_OBJECT(TestDerived2Class)

	gd_testing_unit_test(ObjectCreateObject)
	{
		{
			// These should succeed.
			auto const instance1 = TestDerived1Class::CreateObject();
			auto const instance2 = TestDerived1Class::CreateObject();

			gd_testing_verify(instance1 != instance2);
			gd_testing_verify(instance1->GetGUID() != instance2->GetGUID());
			gd_testing_verify(instance1->GetClass() == instance2->GetClass());
		}

		{
			// This should succeed.
			auto const randomGuid = GUID::New();
			auto const instance3 = Object::CreateOrFindClassRelatedObjectByGUID<TestDerived1Class>(randomGuid);

			gd_testing_verify(instance3->GetGUID() == randomGuid);
		}

		try
		{
			// This should fail: creating object of the abstract type.
			TestBase1Class::CreateObject();
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test(ObjectCreateOrFindClassRelatedObjectByGUID)
	{
		auto const createdInstance = TestDerived1Class::CreateObject();
		
		{
			// This should succeed.
			auto const foundInstance = Object::CreateOrFindClassRelatedObjectByGUID<TestDerived1Class>(createdInstance->GetGUID());
			gd_testing_verify(foundInstance == createdInstance);
		}
		{
			// This should succeed.
			auto const foundBaseInstance = Object::CreateOrFindClassRelatedObjectByGUID<TestBase1Class>(createdInstance->GetGUID());
			gd_testing_verify(foundBaseInstance == createdInstance);
		}

		{
			// This should succeed.
			auto const randomGuid = GUID::New();
			auto const notFoundInstance = Object::CreateOrFindClassRelatedObjectByGUID<TestDerived1Class>(randomGuid);
			gd_testing_verify(notFoundInstance != createdInstance);
		}

		try
		{
			// This should fail: incorrect type was specified.
			Object::CreateOrFindClassRelatedObjectByGUID<TestBase2Class>(createdInstance->GetGUID());
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}

		try
		{
			// This should fail: incorrect type was specified.
			Object::CreateOrFindClassRelatedObjectByGUID<TestBase2Class>(createdInstance->GetGUID());
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test(ObjectFindObjects)
	{
		//auto const instance1 = TestBase2Class::CreateObject();
		//auto const instance2 = TestDerived2Class::CreateObject();
		//auto const instance3 = TestDerived1Class::CreateObject();

		//{
		//	// This should succeed.
		//	auto const foundInstances = Object::FindClassRelatedObjects(TestBase2Class::GetStaticClass());
		//	gd_testing_verify(foundInstances.GetLength() == 2);
		//	gd_testing_verify(foundInstances.FindFirst(instance1) != SizeTpMax);
		//	gd_testing_verify(foundInstances.FindFirst(instance2) != SizeTpMax);
		//}

		//{
		//	// This should succeed.
		//	auto const foundInstances = Object::FindObjectsByClass(TestDerived2Class::GetStaticClass());
		//	gd_testing_verify(foundInstances.GetLength() == 1);
		//	gd_testing_verify(foundInstances.FindFirst(instance2) != SizeTpMax);
		//}
	};

#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_END
