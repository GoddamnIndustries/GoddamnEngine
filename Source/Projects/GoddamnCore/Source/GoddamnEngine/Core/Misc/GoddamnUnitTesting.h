// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnUnitTesting.h
//! Independent single-header unit testing framework.
#ifndef __goddamn_unit_testing_h_included__
#define __goddamn_unit_testing_h_included__ 1

#include <string>
#include <memory>
#include <vector>
#include <map>

#include <functional>
#include <sstream>
#include <thread>

namespace goddamn_unit_testing
{
	namespace detail
	{
#define GDUT_PASS(...) __VA_ARGS__
#define GDUT_TO_STRING_(...) #__VA_ARGS__
#define GDUT_TO_STRING(...) GDUT_TO_STRING_(__VA_ARGS__)

		//////////////////////////////////////////////////////////////////////////
		// Helper types.
		//////////////////////////////////////////////////////////////////////////

		struct uncopiable_t
		{
			inline uncopiable_t() = default;
			inline uncopiable_t(uncopiable_t &&) = delete;
			inline uncopiable_t(uncopiable_t const&) = delete;
			inline uncopiable_t& operator= (uncopiable_t &&) = delete;
			inline uncopiable_t& operator= (uncopiable_t const&) = delete;
		};	// class uncopiable_t
	}	// namespace detail

	//////////////////////////////////////////////////////////////////////////
	// unit_test_check* types.
	//////////////////////////////////////////////////////////////////////////

	enum struct unit_test_check_type_t
	{
		check_regural,
		check_fatal,
	};	// enum struct unit_test_check_type_t

	struct unit_test_check : detail::uncopiable_t
	{
		bool					check_result;
		unit_test_check_type_t	check_type;
		std::string				check_expression;
		std::string				check_filename;
		std::string				check_function;
		std::size_t				check_line;

		inline unit_test_check(bool const Check_result, unit_test_check_type_t const Check_type, char const* const Check_expression
			, char const* const Check_filename = nullptr, char const* const Check_function = nullptr, std::size_t const Check_line = 0)
			: check_result(Check_result), check_type(Check_type), check_expression(Check_expression), check_filename(Check_filename)
			, check_function(Check_function), check_line(Check_line) {}
	};	// struct unit_test_check

	typedef std::unique_ptr<unit_test_check const> unit_test_check_p;
	typedef std::vector<unit_test_check_p> unit_test_check_array_t;

#define GDUT_MAKE_UNIT_TEST_CHECK_REGURAL(Check_result, ...) (std::make_unique<unit_test_check const>(!!(Check_result) \
	, unit_test_check_type_t::check_regural, GDUT_TO_STRING(__VA_ARGS__), __FILE__, __func__, __LINE__))
#define GDUT_MAKE_UNIT_TEST_CHECK_FATAL(Check_result, ...)   (std::make_unique<unit_test_check const>(!!(Check_result) \
	, unit_test_check_type_t::check_fatal, GDUT_TO_STRING(__VA_ARGS__), __FILE__, __func__, __LINE__))

	//////////////////////////////////////////////////////////////////////////
	// unit_test* types.
	//////////////////////////////////////////////////////////////////////////

	struct unit_test;
	typedef unit_test const* unit_test_p;
	typedef std::map<std::string, unit_test_p> unit_test_map_t;

	struct unit_test final : detail::uncopiable_t
	{
		bool					test_result;
		std::string				test_name;
		unit_test_check_array_t	test_checks;

		inline ~unit_test() { get_tests().erase(this->test_name); }
		inline explicit unit_test(char const* Test_name, std::function<bool(unit_test_check_array_t&)> Test_function)
			: test_result(Test_function(test_checks)), test_name(Test_name)	{ get_tests()[this->test_name] = this; }

		inline static unit_test_map_t& get_tests()
		{
			unit_test_map_t static tests;
			return tests;
		}

	};	// struct unit_test

}	// namespace goddamn_unit_testing

#endif	//	ifndef __goddamn_unit_testing_h_included__
