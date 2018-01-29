// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Map.cpp
 * Dynamically sized associative container class.
 */
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/UnorderedMap.h>
#include <GoddamnEngine/Core/Containers/UnorderedVectorMap.h>

#if GD_TESTING_ENABLED
#	include <array>
#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_BEGIN

#if GD_TESTING_ENABLED

	using MapPairIntFloat = MapPair<int, float>;

	gd_testing_unit_test(MapPairEqualsNotEqualsOperator)
	{
		MapPairIntFloat a = { 1, 23.34f };
		MapPairIntFloat b = { 2, 45.56f };
		MapPairIntFloat c = { 1, 67.78f };

		gd_testing_verify(a != b);
		gd_testing_verify(a == c);
	};

	gd_testing_unit_test(MapPairLessGreaterOperator)
	{
		MapPairIntFloat a = { 0, 23.34f };
		MapPairIntFloat b = { 1, 45.56f };

		gd_testing_verify(a < b);
		gd_testing_verify(b > a);
	};

	using MapIntFloat = Map<int, float>;
	using UnorderedMapIntFloat = UnorderedMap<int, float>;
	using UnorderedVectorMapIntFloat = UnorderedVectorMap<int, float>;

	gd_testing_unit_test_foreach(MapIntFloatInsert, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map;
		map.Insert(1, 10.0f);
		map.Insert(2, 20.0f);
		map.Insert(4, 40.0f);
		map.Insert(-1, -10.0f);
		map.Insert(3, 30.0f);

		try
		{
			// This should fail.
			map.Insert(2, 19.9f);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test_foreach(MapIntFloatIteration, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map{ { 2, 20.0f }, { 1, 10.0f }, { 0, 0.0f }, { 3, 30.0f } };
		std::array<float, 4> stdMap = { -100.0f };
		for (auto iter = map.Begin(); iter != map.End(); iter += 1)	// '+= 1' intentionally.
		{
			stdMap[iter->Key] = iter->Value;
		}

		for (auto i = 0; i < stdMap.size(); ++i)
		{
			gd_testing_verify(stdMap[i] == i * 10.0f);
		}

		try
		{
			// This should fail.
			*(map.End() + 1);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test_foreach(MapIntFloatLengthAccess, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map1;
		gd_testing_verify(map1.GetLength() == 0);
		gd_testing_verify(map1.IsEmpty());

		TMapType map2{ { 2, 20.0f }, { 3, 30.0f }, { 4, 40.0f }, { 5, 50.0f } };
		gd_testing_verify(map2.GetLength() == 4);
		gd_testing_verify(!map2.IsEmpty());
	};

	gd_testing_unit_test_foreach(MapIntFloatMoveCtor, TMapType, MapIntFloat, UnorderedMapIntFloat/*, UnorderedVectorMapIntFloat*/)
	{
		TMapType map1{ { 2, 20.0f }, { 3, 30.0f }, { 4, 40.0f }, { 5, 50.0f } };
		TMapType map2(Utils::Move(map1));

		gd_testing_verify(map1.IsEmpty());
		gd_testing_verify(!map2.IsEmpty());
		GD_STUBBED(UnorderedVectorMapIntFloatMoveCtor);
	};

	gd_testing_unit_test_foreach(MapIntFloatMoveAssignmentOperator, TMapType, MapIntFloat, UnorderedMapIntFloat/*, UnorderedVectorMapIntFloat*/)
	{
		TMapType map1{ { 2, 20.0f }, { 3, 30.0f }, { 4, 40.0f }, { 5, 50.0f } };
		TMapType map2{ { 7, 70.0f } };

		map2 = Utils::Move(map1);

		gd_testing_verify(map1.IsEmpty());
		gd_testing_verify(!map2.IsEmpty());
		GD_STUBBED(UnorderedVectorMapIntFloatMoveAssignmentOperator);
	};

	gd_testing_unit_test_foreach(MapIntFindIterator, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map{ { 1, 10.0f }, { 2, 20.0f }, { 3, 30.0f } };
		gd_testing_verify(map.FindIterator(1)->Value == 10.0f);
		gd_testing_verify(map.FindIterator(2)->Value == 20.0f);
		gd_testing_verify(map.FindIterator(3)->Value == 30.0f);
		gd_testing_verify(map.FindIterator(4) == map.End());
	};

	gd_testing_unit_test_foreach(MapIntFind, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map{ { 1, 10.0f }, { 2, 20.0f }, { 3, 30.0f } };
		gd_testing_verify(*map.Find(1) == 10.0f);
		gd_testing_verify(*map.Find(2) == 20.0f);
		gd_testing_verify(*map.Find(3) == 30.0f);
		gd_testing_verify( map.Find(4) == nullptr);
	};

	gd_testing_unit_test_foreach(MapIntContains, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map{ { 1, 10.0f }, { 2, 20.0f }, { 3, 30.0f } };
		gd_testing_verify(map.Contains(1));
		gd_testing_verify(map.Contains(2));
		gd_testing_verify(map.Contains(3));
		gd_testing_verify(!map.Contains(4));
	};

	gd_testing_unit_test_foreach(MapIntErase, TMapType, MapIntFloat, UnorderedMapIntFloat, UnorderedVectorMapIntFloat)
	{
		TMapType map{ { 1, 10.0f }, { 2, 20.0f }, { 3, 30.0f },
					  { 4, 40.0f }, { 5, 50.0f }, { 6, 60.0f }, 
					  { 7, 70.0f } };

		gd_testing_verify(map.Contains(3));
		map.Erase(3);
		gd_testing_verify(!map.Contains(3));

		gd_testing_verify(map.Contains(7));
		map.Erase(7);
		gd_testing_verify(!map.Contains(7));

		gd_testing_verify(map.Contains(1));
		map.Erase(1);
		gd_testing_verify(!map.Contains(1));

		try
		{
			// This should fail.
			map.Erase(19);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_END
