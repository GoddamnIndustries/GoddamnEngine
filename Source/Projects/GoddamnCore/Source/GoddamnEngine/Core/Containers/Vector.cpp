// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/Vector.h
 * Dynamically sized array class.
 */
#include <GoddamnEngine/Core/Containers/Vector.h>
#if GD_TESTING_ENABLED
#	include <array>
#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_BEGIN

#if GD_TESTING_ENABLED

	using VectorInt = Vector<int>;

	gd_testing_unit_test(VectorIntBaseCtors)
	{
		VectorInt vector{ 0, 1 };
		gd_testing_verify(vector.GetData()[0] == 0 && vector.GetData()[1] == 1);
	};

	gd_testing_unit_test(VectorIntEqualsNotEqualsOperators)
	{
		VectorInt a{ 0, 1, 2 };
		VectorInt b{ 1, 2, 3 };
		VectorInt c{ 0, 1, 2 };

		gd_testing_verify(a != b);
		gd_testing_verify(a == c);
	};

	gd_testing_unit_test(VectorIntLessGreaterOperators)
	{
		VectorInt a{ 0, 1, 2 };
		VectorInt b{ 0, 2, 3 };
		VectorInt c{ 1, 1, 2 };

		gd_testing_verify(a < b);
		gd_testing_verify(c > b);
	};

	gd_testing_unit_test(VectorIntAssigmentOperators)
	{
		VectorInt vectorOriginal{ 0, 1, 2 };
		VectorInt vectorCopy;
		VectorInt vectorMoved;

		vectorCopy = vectorOriginal;
		gd_testing_verify(vectorOriginal == VectorInt{ 0, 1, 2 } && vectorOriginal == vectorCopy && vectorOriginal.GetData() != vectorCopy.GetData());

		vectorMoved = Utils::Move(vectorOriginal);
		gd_testing_verify(vectorOriginal == VectorInt{} && vectorMoved == vectorCopy && vectorMoved != vectorOriginal);
	};

	gd_testing_unit_test(VectorIntCtorsDtor)
	{
		{
			VectorInt const emptyVector;
			gd_testing_verify(emptyVector == VectorInt{});
		}

		{
			VectorInt const filledVector(3);
			gd_testing_verify(filledVector == VectorInt{ 0, 0, 0 });
		}

		{
			std::array<short, 3> const stdArray{ 0, 1, 2 };
			VectorInt const initializedWithIterators(stdArray.begin(), stdArray.end());
			gd_testing_verify(initializedWithIterators == VectorInt{ 0, 1, 2 });
		}

		{
			VectorInt vectorOriginal{ 0, 1, 2 };
			VectorInt vectorCopy(vectorOriginal);

			vectorCopy = vectorOriginal;
			gd_testing_verify(vectorOriginal == VectorInt{ 0, 1, 2 } && vectorOriginal == vectorCopy && vectorOriginal.GetData() != vectorCopy.GetData());

			VectorInt vectorMoved;
			vectorMoved = Utils::Move(vectorOriginal);
			gd_testing_verify(vectorOriginal == VectorInt{} && vectorMoved == vectorCopy && vectorMoved != vectorOriginal);
		};
	};

	gd_testing_unit_test(VectorIntIteration)
	{
		int const cArray[] = { 0, 1, 2 };
		int const* cArrayPtr = cArray;
		VectorInt const vector(cArray, cArray + GetLength(cArray));
		for (auto iter = vector.Begin(); iter != vector.End(); ++iter)
		{
			gd_testing_verify(*iter == *cArrayPtr++);
		}

		try
		{
			// This should fail.
			*(vector.End() + 1);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::testing_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test(VectorIntLengthAccess)
	{
		{
			VectorInt vector{ 0, 1, 2 };
			gd_testing_verify(vector.GetLength() == 3);
			gd_testing_verify(vector.GetCapacity() >= 3);
			gd_testing_verify(!vector.IsEmpty());
		}

		{
			VectorInt const emptyVector;
			gd_testing_verify(emptyVector.GetCapacity() >= 0);
			gd_testing_verify(emptyVector.IsEmpty());
		}

		{
			VectorInt vector{ 0, 1, 2 };
			vector.Resize(4);
			gd_testing_verify(vector.GetCapacity() >= 4);
			gd_testing_verify(vector == VectorInt{ 0, 1, 2, 0 });

			vector.Resize(2);
			vector.Resize(4);
			gd_testing_verify(vector == VectorInt{ 0, 1, 0, 0 });

			vector.Resize(2);
			vector.Reserve(6);
			gd_testing_verify(vector.GetCapacity() == 6);
			gd_testing_verify(vector == VectorInt{ 0, 1 });
		}
	};

	gd_testing_unit_test(VectorIntElementsAccess)
	{
		{
			VectorInt vector{ 0, 1 };
			gd_testing_verify(vector.GetLast() == 1);

			try
			{
				// These should succeed.
				vector[0];
				vector[1];
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(false);
			}

			try
			{
				// This should fail.
				vector[2];
				gd_testing_verify(false);
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(true);
			}
		}

		{
			VectorInt vector{ 0, 1, 3 };
			vector.InsertAt(2, 2);
			gd_testing_verify(vector == VectorInt{ 0, 1, 2, 3 });

			vector.InsertLast(4);
			gd_testing_verify(vector == VectorInt{ 0, 1, 2, 3, 4 });

			vector.EraseAt(1, 2);
			gd_testing_verify(vector == VectorInt{ 0, 3, 4 });

			vector.EraseLast();
			gd_testing_verify(vector == VectorInt{ 0, 3 });
		}

		{
			VectorInt vector{ 0, 1, 2, 0, 1, 2 };
			gd_testing_verify(vector.FindFirst(2) == 2);
		}
	};

	// We need to provide similar tests set for Boolean vector since it is implemented in different manner.
	using VectorBool = Vector<bool>;

    #define VECTORBOOL_TESTING_SEQUENCE_A \
	    0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, \
	    0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, \
	    0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, \
	    1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1 \

    #define VECTORBOOL_TESTING_SEQUENCE_B \
		1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, \
		1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, \
		1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, \
		1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0 \

    gd_testing_unit_test(VectorBoolEqualsNotEqualsOperators)
	{
		VectorBool a{ VECTORBOOL_TESTING_SEQUENCE_A };
		VectorBool b{ VECTORBOOL_TESTING_SEQUENCE_B };
		VectorBool c{ VECTORBOOL_TESTING_SEQUENCE_A };

		gd_testing_verify(a != b);
		gd_testing_verify(a == c);
	};

	gd_testing_unit_test(VectorBoolLessGreaterOperators)
	{
		VectorBool a{ VECTORBOOL_TESTING_SEQUENCE_A, VECTORBOOL_TESTING_SEQUENCE_A };
		VectorBool b{ VECTORBOOL_TESTING_SEQUENCE_A, VECTORBOOL_TESTING_SEQUENCE_B };
		VectorBool c{ VECTORBOOL_TESTING_SEQUENCE_A };

		gd_testing_verify(a < b);
		gd_testing_verify(b > c);
	};

	gd_testing_unit_test(VectorBoolAssigmentOperators)
	{
		VectorBool vectorOriginal{ VECTORBOOL_TESTING_SEQUENCE_A };
		VectorBool vectorCopy;
		VectorBool vectorMoved;

		vectorCopy = vectorOriginal;
		gd_testing_verify(vectorOriginal == VectorBool{ VECTORBOOL_TESTING_SEQUENCE_A } && vectorOriginal == vectorCopy);

		vectorMoved = Utils::Move(vectorOriginal);
		gd_testing_verify(vectorOriginal == VectorBool{} && vectorMoved == vectorCopy && vectorMoved != vectorOriginal);
	};

	gd_testing_unit_test(VectorBoolCtorsDtor)
	{
		{
			VectorBool const emptyVector;
			gd_testing_verify(emptyVector == VectorBool{});
		}

		{
			VectorBool const filledVector(3);
			gd_testing_verify(filledVector == VectorBool{ 0, 0, 0 });
		}

		{
			std::array<short, 3> const stdArray{ 0, 1, 1 };
			VectorBool const initializedWithIterators(stdArray.begin(), stdArray.end());
			gd_testing_verify(initializedWithIterators == VectorBool{ 0, 1, 1 });
		}

		{
			VectorBool vectorOriginal{ VECTORBOOL_TESTING_SEQUENCE_B };
			VectorBool vectorCopy(vectorOriginal);

			vectorCopy = vectorOriginal;
			gd_testing_verify(vectorOriginal == VectorBool{ VECTORBOOL_TESTING_SEQUENCE_B } && vectorOriginal == vectorCopy);

			VectorBool vectorMoved;
			vectorMoved = Utils::Move(vectorOriginal);
			gd_testing_verify(vectorOriginal == VectorBool{} && vectorMoved == vectorCopy && vectorMoved != vectorOriginal);
		}
	};

	gd_testing_unit_test(VectorBoolIteration)
	{
		bool const cArray[] = { VECTORBOOL_TESTING_SEQUENCE_A };
		bool const* cArrayPtr = cArray;
		VectorBool const vector(cArray, cArray + GetLength(cArray));
		for (auto iter = vector.Begin(); iter != vector.End(); ++iter)
		{
			gd_testing_verify(*iter == *cArrayPtr++);
		}

		try
		{
			// This should fail.
			*(vector.End() + 1);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::testing_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test(VectorBoolLengthAccess)
	{
		{
			VectorBool vector{ 0, 1, 1 };
			gd_testing_verify(vector.GetLength() == 3);
			gd_testing_verify(vector.GetCapacity() >= 3);
			gd_testing_verify(!vector.IsEmpty());
		}

		{
			VectorBool const emptyVector;
			gd_testing_verify(emptyVector.GetCapacity() >= 0);
			gd_testing_verify(emptyVector.IsEmpty());
		}

		{
			#define VECTORBOOL_TESTING_SEQUENCE_FIRST_LINE 1, 0, 1, 1, 0
			#define VECTORBOOL_TESTING_SEQUENCE                                                                                                                                                \
				VECTORBOOL_TESTING_SEQUENCE_FIRST_LINE, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, \
					1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, \
					1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, \
					0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0

			#define VECTORBOOL_TESTING_SEQUENCE_WITH_FIRST_LINE                                                                                                                                \
				VECTORBOOL_TESTING_SEQUENCE_FIRST_LINE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

	        VectorBool vector{ VECTORBOOL_TESTING_SEQUENCE };
			vector.Resize(203);
			gd_testing_verify(vector.GetCapacity() >= 203);
			gd_testing_verify(vector == VectorBool{ VECTORBOOL_TESTING_SEQUENCE, 0, 0, 0 });

			vector.Resize(5);
			vector.Resize(200);
			gd_testing_verify(vector == VectorBool{ VECTORBOOL_TESTING_SEQUENCE_WITH_FIRST_LINE });

			vector.Resize(5);
			vector.Reserve(6);
			gd_testing_verify(vector.GetCapacity() == 6);
			gd_testing_verify(vector == VectorBool{ VECTORBOOL_TESTING_SEQUENCE_FIRST_LINE });

			vector.Resize(1);
			gd_testing_verify(vector == VectorBool{ 1 });

			#undef VECTORBOOL_TESTING_SEQUENCE_WITH_FIRST_LINE
			#undef VECTORBOOL_TESTING_SEQUENCE
			#undef VECTORBOOL_TESTING_SEQUENCE_FIRST_LINE
		}
	};

	gd_testing_unit_test(VectorBoolElementsAccess)
	{
		{
			VectorBool vector{ 0, 1 };
			gd_testing_verify(vector.GetLast() == 1);

			try
			{
				// These should succeed.
				gd_testing_verify(vector[0] == 0);
				gd_testing_verify(vector[1] == 1);
				vector[0] = 1;
				vector[1] = 0;
				gd_testing_verify(vector == VectorBool{ 1, 0 });
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(false);
			}

			try
			{
				// This should fail.
				vector[201];
				gd_testing_verify(false);
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(true);
			}
		}

		{
			VectorBool vector{ 0, 1, 1 };
			vector.InsertAt(2, 0);
			gd_testing_verify(vector == VectorBool{ 0, 1, 0, 1 });

			vector.InsertLast(1);
			gd_testing_verify(vector == VectorBool{ 0, 1, 0, 1, 1 });

			vector.EraseAt(1);
			gd_testing_verify(vector == VectorBool{ 0, 0, 1, 1 });

			vector.EraseLast();
			gd_testing_verify(vector == VectorBool{ 0, 0, 1 });
		}
	};

#endif	// if GD_TESTING_ENABLED

GD_NAMESPACE_END
