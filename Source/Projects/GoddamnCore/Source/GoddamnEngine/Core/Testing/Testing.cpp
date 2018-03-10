// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
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
#include "GoddamnEngine/Include.h"

GD_NAMESPACE_BEGIN
	class TestPlatformAssert final : public IPlatformAssert
	{
		GDINT virtual AssertDialogResult RunAssertDialog(AssertData const* const assertData) override final
		{
			throw goddamn_testing::assertion_exception(assertData->AssertFileName, assertData->AssertFunctionName, assertData->AssertLineNumber, assertData->AssertExpression, assertData->AssertMessage);
		}
		GDINT virtual void ReportAndExit(AssertData const* const assertData) override final
		{
			GD_NOT_USED_L(assertData);
			exit(-1);
		}
	};	// class TestPlatformAssert
GD_NAMESPACE_END

namespace goddamn_testing
{

	gdt_api assertion_exception::assertion_exception(char const* const file, char const* const function
		, unsigned line, char const* const expression, char const* const message, ...)
	{
		(void)message, file, function, line, expression;
	}

	gdt_api fatal_assertion_exception::fatal_assertion_exception(char const* const file, char const* const function
		, unsigned line, char const* const message, ...)
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
			static GD::TestPlatformAssert TestPlatformAssert;
			GD::IPlatformAssert::Push(TestPlatformAssert);
			test_function(m_test_results);
			GD::IPlatformAssert::Pop();
		//}
		//catch (testing_exception const&)
		//{
		//	throw "Some test has failed. Maybe I need to collect stats of something, but this is for later.";
		//}
	}

}	// namespace goddamn_testing
