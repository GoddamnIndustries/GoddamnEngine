// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
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

#if defined(_WIN32)
#define gdt_api __declspec(dllexport)
#define gdt_analysis_assume(...) __analysis_assume(__VA_ARGS__)
#else
#define gdt_api
#define gdt_analysis_assume(...)
#endif

#include <exception>

// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
// ******                                 Testing core.                                    ******
// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

/*!
 * Defines an assert, that should replace application's default asserts.
 * It is used to test the correct behavior for incorrect input data that application should fail on. 
 */
#define gd_testing_assert(expression, ...) \
	do { \
		gdt_analysis_assume(expression); \
		if (!(expression)) { \
			throw ::goddamn_testing::assertion_exception(__FILE__, __FUNCTION__, __LINE__, #expression, ##__VA_ARGS__); \
		} \
	} while (false); 

/*!
 * Defines an assert, that should replace application's default fatal asserts.
 * It is used to test the correct behavior for incorrect input data that application should fail on. 
 */
#define gd_testing_assert_false(...) \
	do { \
		throw ::goddamn_testing::fatal_assertion_exception(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} while (false); 

/*!
 * Defines the verification statement that should succeed for correct testing code.
 */
#define gd_testing_verify(...) \
	do { \
		if (!(__VA_ARGS__)) { \
			throw ::goddamn_testing::verification_exception(__FILE__, __FUNCTION__, __LINE__); \
		} \
	} while (false); 

/*!
 * Defines a unit test.
 * A simple example for using tests:
 * @code
 *     gd_testing_unit_test(AVerySimpleTest)
 *     {
 *         gd_testing_verify(3 > 2);
 *     }
 * @endcode
 */
#define gd_testing_unit_test(test_name, ...) \
	static ::goddamn_testing::unit_test unit_test ## test_name = (::goddamn_testing::unit_test_function)[](::goddamn_testing::unit_test_results const&)

namespace goddamn_testing
{

	/*!
	 * Base testing exception class.
	 */
	struct testing_exception : public std::exception
	{
	public:
		gdt_api testing_exception(...) {}
	};	// struct testing_exception

	/*!
	 * Exception being thrown on the assertion somewhere inside the testing code.
	 * Should be caught if expected, uncaught ones would be treated as failure of the test.
	 */
	struct assertion_exception : testing_exception
	{
	public:
		gdt_api assertion_exception(char const* const file, char const* const function
			, unsigned line, char const* const expression, char const* const message = nullptr, ...);
	};	// struct assertion_exception

	/*!
	 * Exception being thrown on the fatal assertion somewhere inside the testing code.
	 * Should be caught if expected, uncaught would be treated as failure of the test.
	 */
	struct fatal_assertion_exception : testing_exception
	{
	public:
		gdt_api fatal_assertion_exception(char const* const file, char const* const function
			, unsigned line, char const* const message = nullptr, ...);
	};	// struct fatal_assertion_exception

	/*!
	 * Exception being thrown of the failure of the test. 
	 * Are caught internally by the testing engine. 
	 */
	struct verification_exception final : public testing_exception
	{
	public:
		gdt_api verification_exception(char const* const file, char const* const function, unsigned line);
	};	// struct verification_exception

	struct unit_test_results final
	{
	};	// struct unit_test_results
	using unit_test_function = void(*)(unit_test_results const&);

	struct unit_test final
	{
	private:
		unit_test_results m_test_results;

	public:

		gdt_api unit_test(unit_test_function const test_function);

	};	// struct unit_test

}	// namespace goddamn_testing

// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
// ******                              Testing utilities.                                  ******
// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

/*!
 * Defines a series of unit tests for the specified set of types.
 *
 * Sometimes it is useful to test classes with similar interfaces and different implementations,
 * like @c std::set and @c std::unordered_set. This function is intended to ease the process.
 *
 * An example:
 * @code
 *     gd_testing_unit_test_foreach(SetsTest, T, std::set, std::unordered_set)
 *     {
 *         T set;    // Do something with it.
 *     };
 * @endcode
 */
#define gd_testing_unit_test_foreach(test_name, type_name, ...) \
	struct test_name ## _tester final { \
		template<typename type_name> \
		void operator()(::goddamn_testing::template_dummy<type_name> const&) const; \
	}; \
	gd_testing_unit_test(test_name) { \
		::goddamn_testing::template_foreach<__VA_ARGS__>(test_name ## _tester()); \
	}; \
	template<typename type_name> \
	void test_name ## _tester::operator()(::goddamn_testing::template_dummy<type_name> const&) const \

namespace goddamn_testing
{
	template<typename TType>
	struct template_dummy
	{
		using type = TType;
	};	// struct template_dummy

	namespace implementation
	{
		template<typename... TTypes>
		struct template_foreach_impl;

		template<typename TFirstType, typename... TRestTypes>
		struct template_foreach_impl<TFirstType, TRestTypes...> final
		{
			template<typename TFunc>
			static void foreach(TFunc const& func)
			{
				func(template_dummy<TFirstType>());
				template_foreach_impl<TRestTypes...>::foreach(func);
			}
		};	// struct template_foreach_impl<TFirstType, TRestTypes...>
		
		template<>
		struct template_foreach_impl<> final
		{
			template<typename TFunc>
			static void foreach(TFunc const&) {}
		};	// struct template_foreach_impl<>
	}	// namespace implementation

	template<typename... TTypes, typename TFunc>
	void template_foreach(TFunc const& func)
	{
		implementation::template_foreach_impl<TTypes...>::foreach(func);
	}

}	// namespace goddamn_testing
