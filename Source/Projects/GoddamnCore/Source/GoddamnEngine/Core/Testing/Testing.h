// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Testing.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Contains unit tests framework.
 */
#pragma once

/*!
 * Defines an assert, that should replace application's default asserts.
 * It is used to test the correct behavior for incorrect input data that application should fail on. 
 */
#define gd_testing_assert(expression, message, ...) \
	do { \
		__analysis_assume(expression); \
		if (!(expression)) { \
			throw goddamn_testing::testing_exception(message, __FILE__, __FUNCTION__, __LINE__, #expression); \
		} \
	} while (false); 

/*!
 * Defines an assert, that should replace application's default fatal asserts.
 * It is used to test the correct behavior for incorrect input data that application should fail on. 
 */
#define gd_testing_assert_false(message, ...) \
	do { \
		throw goddamn_testing::testing_exception(message, __FILE__, __FUNCTION__, __LINE__); \
	} while (false); 

#define gd_testing_unit_test(test_name, ...) \
	static goddamn_testing::unit_test unit_test ## test_name = (goddamn_testing::unit_test_function)[](goddamn_testing::unit_test_results const&)

#define gd_testing_verify(...) \
	do { \
		if (!(__VA_ARGS__)) { \
			throw 0; \
		} \
	} while (false); 

namespace goddamn_testing
{
	struct testing_exception final
	{
	public:
		testing_exception(...) {}
	};	// struct testing_exception

	struct unit_test_results final
	{
	};	// struct unit_test_results

	typedef void(*unit_test_function)(unit_test_results const&);

	struct unit_test final
	{
	private:
		unit_test_results m_test_results;

	public:

		unit_test(unit_test_function const test_function)
		{
			try
			{
				test_function(m_test_results);
			} 
			catch(testing_exception const&)
			{
				throw 0;
			}
		}

	};	// struct unit_test

}	// namespace goddamn_testing
