// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Set.cpp
 * Dynamically sized set class.
 */
#include <GoddamnEngine/Core/Containers/Set.h>
#include <GoddamnEngine/Core/Containers/UnorderedSet.h>
#include <GoddamnEngine/Core/Containers/UnorderedVectorSet.h>

#if GD_TESTING_ENABLED
#	include <array>
#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_BEGIN

#if GD_TESTING_ENABLED

	using SetInt = Set<int>;
	using UnorderedSetInt = UnorderedSet<int>;
	using UnorderedVectorSetInt = UnorderedVectorSet<int>;

	gd_testing_unit_test_foreach(SetIntInsert, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set;
		set.Insert(1);
		set.Insert(2);
		set.Insert(4);
		set.Insert(-1);
		set.Insert(3);

		try
		{
			// This should fail.
			set.Insert(2);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test_foreach(SetIntIteration, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set{ 2, 1, 0, 3 };
		std::array<bool, 4> stdSet = { false };
		for (auto iter = set.Begin(); iter != set.End(); iter += 1)	// '+= 1' intentionally.
		{
			stdSet[*iter] = true;
		}

		for (auto const& value : stdSet)
		{
			gd_testing_verify(value);
		}

		try
		{
			// This should fail.
			*(set.End() + 1);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test_foreach(SetIntLengthAccess, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set1;
		gd_testing_verify(set1.GetLength() == 0);
		gd_testing_verify(set1.IsEmpty());

		TSetType set2{ 2, 3, 4, 5 };
		gd_testing_verify(set2.GetLength() == 4);
		gd_testing_verify(!set2.IsEmpty());
	};

	gd_testing_unit_test_foreach(SetIntMoveCtor, TSetType, SetInt, UnorderedSetInt/*, UnorderedVectorSetInt*/)
	{
		TSetType set1{ 2, 3, 4, 5 };
		TSetType set2(Utils::Move(set1));
		
		gd_testing_verify(set1.IsEmpty());
		gd_testing_verify(!set2.IsEmpty());
		GD_STUBBED(UnorderedVectorSetIntMoveCtor);
	};

	gd_testing_unit_test_foreach(SetIntMoveAssignmentOperator, TSetType, SetInt, UnorderedSetInt/*, UnorderedVectorSetInt*/)
	{
		TSetType set1{ 2, 3, 4, 5 };
		TSetType set2{ 7 };

		set2 = Utils::Move(set1);

		gd_testing_verify(set1.IsEmpty());
		gd_testing_verify(!set2.IsEmpty());
		GD_STUBBED(UnorderedVectorSetIntMoveAssignmentOperator);
	};

	gd_testing_unit_test_foreach(SetIntClear, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set{ 2, 3, 4, 5 };
		gd_testing_verify(set.GetLength() == 4);

		set.Clear();
		gd_testing_verify(set.GetLength() == 0);
		gd_testing_verify(set.IsEmpty());
	};

	gd_testing_unit_test_foreach(SetIntFindIterator, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set{ 1, 2, 3 };
		gd_testing_verify(set.FindIterator(1) != set.End());
		gd_testing_verify(set.FindIterator(2) != set.End());
		gd_testing_verify(set.FindIterator(3) != set.End());
		gd_testing_verify(set.FindIterator(4) == set.End());
	};

	gd_testing_unit_test_foreach(SetIntFind, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set{ 1, 2, 3 };
		gd_testing_verify(set.Find(1) != nullptr);
		gd_testing_verify(set.Find(2) != nullptr);
		gd_testing_verify(set.Find(3) != nullptr);
		gd_testing_verify(set.Find(4) == nullptr);
	};

	gd_testing_unit_test_foreach(SetIntContains, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set{ 1, 2, 3 };
		gd_testing_verify(set.Contains(1));
		gd_testing_verify(set.Contains(2));
		gd_testing_verify(set.Contains(3));
		gd_testing_verify(!set.Contains(4));
	};

	gd_testing_unit_test_foreach(SetIntErase, TSetType, SetInt, UnorderedSetInt, UnorderedVectorSetInt)
	{
		TSetType set{ 1, 2, 3, 4, 5, 6, 7 };
		
		gd_testing_verify(set.Contains(3));
		set.Erase(3);
		gd_testing_verify(!set.Contains(3));

		gd_testing_verify(set.Contains(7));
		set.Erase(7);
		gd_testing_verify(!set.Contains(7));

		gd_testing_verify(set.Contains(1));
		set.Erase(1);
		gd_testing_verify(!set.Contains(1));

		try
		{
			// This should fail.
			set.Erase(19);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::assertion_exception const&)
		{
			gd_testing_verify(true);
		}
	};

#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_END
