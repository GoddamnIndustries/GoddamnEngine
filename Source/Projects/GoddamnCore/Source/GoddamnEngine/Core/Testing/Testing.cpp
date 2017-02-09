// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Testing.cpp
 * Contains unit tests framework.
 */
#include "Testing.h"

namespace goddamn_testing
{

	gdt_api assertion_exception::assertion_exception(char const* const message, char const* const file
		, char const* const function, unsigned line, char const* const expression)
	{
		(void)message, file, function, line, expression;
	}

	gdt_api fatal_assertion_exception::fatal_assertion_exception(char const* const message, char const* const file
		, char const* const function, unsigned line)
	{
		(void)message, file, function, line;
	}

	gdt_api verification_exception::verification_exception(char const* const file, char const* const function, unsigned line)
	{
		(void)file, function, line;
	}

	gdt_api unit_test::unit_test(unit_test_function const test_function)
	{
		/// @todo Implement a cool testing framework, not just the automated test executing on each engine run.
		//try
		//{
			test_function(m_test_results);
		//}
		//catch (testing_exception const&)
		//{
		//	throw "Some test has failed. Maybe I need to collect stats of something, but this is for later.";
		//}
	}

}	// namespace goddamn_testing
