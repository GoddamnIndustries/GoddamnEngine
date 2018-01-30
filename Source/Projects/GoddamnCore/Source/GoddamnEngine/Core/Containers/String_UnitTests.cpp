// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/String.h
 * Constantly sized string class.
 */
#include <GoddamnEngine/Core/Containers/String.h>
#if GD_TESTING_ENABLED

GD_NAMESPACE_BEGIN

	gd_testing_unit_test_foreach(StringCtorsBase, T, Char, WideChar)
	{
		BaseString<T> const string(GD_TEXT(T, "ab"));
		gd_testing_verify(string.CStr()[0] == GD_TEXT(T, 'a')
			&& string.CStr()[1] == GD_TEXT(T, 'b')
			&& string.CStr()[2] == GD_TEXT(T, '\0')
			);
	};

	gd_testing_unit_test(StringEqualsNotEqualsOperators)
	{
		String const a("first string.");
		String const b("second string.");
		String const c("first string.");

		gd_testing_verify(a != b);
		gd_testing_verify(a == c);

		gd_testing_verify(a != 's');
		gd_testing_verify(a != "second string.");
		gd_testing_verify(a == "first string.");

		gd_testing_verify('s' != a);
		gd_testing_verify("second string." != a);
		gd_testing_verify("first string." == a);
	};

	gd_testing_unit_test(StringLessGreaterOperators)
	{
		String const a("aaa");
		String const b("bbb");
		String const c("ccc");

		gd_testing_verify(a < b);
		gd_testing_verify(c > b);
	};

	gd_testing_unit_test(StringAssigmentOperators)
	{
		String stringOriginal("original");
		String stringCopy;
		String stringMoved;

		stringCopy = stringOriginal;
		gd_testing_verify(stringOriginal == "original" && stringOriginal == stringCopy && stringOriginal.CStr() != stringCopy.CStr());

		stringMoved = Utils::Move(stringOriginal);
		gd_testing_verify(stringOriginal == "" && stringMoved == stringCopy && stringMoved != stringOriginal);
	};

	gd_testing_unit_test(StringCtorsDtor)
	{
		{
			String const emptyString;
			gd_testing_verify(emptyString == "");
		}

		{
			String const characterString('a');
			gd_testing_verify(characterString == 'a');
			gd_testing_verify(characterString == "a");
		}

		{
			String const filledString(3, 'a');
			gd_testing_verify(filledString == "aaa");
		}

		{
			String const cstringString("hello, world");
			gd_testing_verify(cstringString == "hello, world");
		}

		{
			String const cstringStringChopped("hello, world", 5);
			gd_testing_verify(cstringStringChopped == "hello");
		}

		{
			String stringOriginal("original");
			String const stringCopy(stringOriginal);
			gd_testing_verify(stringOriginal == "original" && stringOriginal == stringCopy && stringOriginal.CStr() != stringCopy.CStr());

			String const stringMoved(Utils::Move(stringOriginal));
			gd_testing_verify(stringOriginal == "" && stringMoved == stringCopy && stringMoved != stringOriginal);
		}
	};

	gd_testing_unit_test(StringIteration)
	{
		CStr cstring = "hello, world";
		String const string(cstring);
		for (auto iter = string.Begin(); iter != string.End(); ++iter)
		{
			gd_testing_verify(*iter == *cstring++);
		}

		try
		{
			// This should fail.
			(void)*(string.End() + 1);
			gd_testing_verify(false);
		}
		catch (goddamn_testing::testing_exception const&)
		{
			gd_testing_verify(true);
		}
	};

	gd_testing_unit_test(StringLengthAccess)
	{
		{
			String const string("hello");
			gd_testing_verify(string.GetLength() == 5);
			gd_testing_verify(!string.IsEmpty());
		}

		{
			String const emptyString;
			gd_testing_verify(emptyString.IsEmpty());
		}
	};

	gd_testing_unit_test(StringCharactersAccess)
	{
		{
			String const string("ab");
			gd_testing_verify(string.GetLast() == 'b');

			try
			{
				// These should succeed.
				string[0];
				string[1];
				string[2];
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(false);
			}

			try
			{
				// This should fail.
				string[3];
				gd_testing_verify(false);
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(true);
			}
		}

		{
			String const a("first string.");
			String const b("second string.");
			String const c("first string.");

			gd_testing_verify(a.GetHashCode() != b.GetHashCode());
			gd_testing_verify(a.GetHashCode() == c.GetHashCode());
		}
	};

	gd_testing_unit_test(StringConcatenations)
	{
		// Appending for string, C string and characters is implemented differently - new must test all three.
		{
			String const firstString("hello, ");
			String const secondString("world");
			String const appendResult = firstString.Append(secondString);
			String const prependResult = secondString.Prepend(firstString);
			gd_testing_verify(appendResult == "hello, world");
			gd_testing_verify(prependResult == "hello, world");
		}

		{
			String const firstString("hello, ");
			String const secondString("world");
			String const appendResult = firstString.Append("world");
			String const prependResult = secondString.Prepend("hello, ");
			gd_testing_verify(appendResult == "hello, world");
			gd_testing_verify(prependResult == "hello, world");
		}

		{
			String const string("b");
			String const appendResult = string.Append('c');
			String const prependResult = string.Prepend('a');
			gd_testing_verify(appendResult == "bc");
			gd_testing_verify(prependResult == "ab");
		}
	};

	gd_testing_unit_test(StringSubstringing)
	{
		String const string("0123456789");
		{
			// These should succeed.
			String const firstCorrectSubstring = string.Substring(1, 4);
			gd_testing_verify(firstCorrectSubstring == "1234");

			String const secondCorrectSubstring = string.Substring(7);
			gd_testing_verify(secondCorrectSubstring == "789");

			String const thirdCorrectSubstring = string.SubstringLen(2, 3);
			gd_testing_verify(thirdCorrectSubstring == "234");

			try
			{
				// This should fail.
				String const firstIncorrectSubstring = string.Substring(1, 100);
				gd_testing_verify(false);
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(true);
			}

			try
			{
				// This should fail.
				String const secondIncorrectSubstring = string.Substring(100, 1);
				gd_testing_verify(false);
			}
			catch (goddamn_testing::testing_exception const&)
			{
				gd_testing_verify(true);
			}
		}

		{
			String const choppedFrontString = string.ChopFront(8);
			gd_testing_verify(choppedFrontString == "89");

			String const choppedEndString = string.ChopEnd(8);
			gd_testing_verify(choppedEndString == "01");
		}

		{
			String const firstRemovedString = string.Remove(1, 8);
			gd_testing_verify(firstRemovedString == "09");
		
			String const secondRemovedString = string.Remove(2);
			gd_testing_verify(secondRemovedString == "01");
		
			String const thirdRemovedString = string.RemoveLen(2, 6);
			gd_testing_verify(thirdRemovedString == "0189");
		}
	};

	gd_testing_unit_test_foreach(StringFinds, T, Char, WideChar)
	{
		{
			// ----------------------------------->0123456789
			BaseString<T> const string(GD_TEXT(T, "____45_345_"));
			{
				SizeTp const foundChar = string.Find(GD_TEXT(T, '4'));
				gd_testing_verify(foundChar == 4);

				SizeTp const notFoundChar = string.Find(GD_TEXT(T, '%'));
				gd_testing_verify(notFoundChar == String::Npos);

				SizeTp const foundCStr = string.Find(GD_TEXT(T, "45"));
				gd_testing_verify(foundCStr == 4);

				SizeTp const notFoundCStr = string.Find(GD_TEXT(T, "43"));
				gd_testing_verify(notFoundCStr == String::Npos);
			}

			{
				SizeTp const foundReverseChar = string.ReverseFind(GD_TEXT(T, '4'));
				gd_testing_verify(foundReverseChar == 8);

				SizeTp const notFoundReverseChar = string.ReverseFind(GD_TEXT(T, '%'));
				gd_testing_verify(notFoundReverseChar == String::Npos);

				SizeTp const foundReverseCStr = string.ReverseFind(GD_TEXT(T, "45"));
				gd_testing_verify(foundReverseCStr == 8);

				SizeTp const notFoundReverseCStr = string.ReverseFind(GD_TEXT(T, "43"));
				gd_testing_verify(notFoundReverseCStr == String::Npos);
			}
		}

		{
			BaseString<T> const string(GD_TEXT(T, "hello, world!"));
			gd_testing_verify(string.StartsWith(string));
			gd_testing_verify(string.StartsWith(GD_TEXT(T, "hello")));
			gd_testing_verify(string.StartsWith(GD_TEXT(T, 'h')));
			gd_testing_verify(!string.StartsWith(GD_TEXT(T, "world")));
			gd_testing_verify(!string.StartsWith(GD_TEXT(T, 'w')));

			gd_testing_verify(string.EndsWith(string));
			gd_testing_verify(string.EndsWith(GD_TEXT(T, "world!")));
			gd_testing_verify(string.EndsWith(GD_TEXT(T, '!')));
			gd_testing_verify(!string.EndsWith(GD_TEXT(T, "hello")));
			gd_testing_verify(!string.EndsWith(GD_TEXT(T, 'h')));
		}
	};

	gd_testing_unit_test(StringConversions)
	{
		{
			String lowerCaseString("__AnnihilatoR__666");
			String lowerToUpperCaseString = lowerCaseString.ToUpper();
			gd_testing_verify(lowerToUpperCaseString == "__ANNIHILATOR__666");

			String upperCaseString("~String");
			String upperToLowerCaseString = upperCaseString.ToLower();
			gd_testing_verify(upperToLowerCaseString == "~string");
		}

		{
			bool result;

			String const booleanStringTrue("true");
			bool const booleanStringTrueConv = booleanStringTrue.ToBool(&result);
			gd_testing_verify(booleanStringTrueConv && result);

			String const booleanStringFalse("false");
			bool const booleanStringFalseConv = booleanStringFalse.ToBool(&result);
			gd_testing_verify(!booleanStringFalseConv && result);

			String const booleanStringInvalid("naturlich");
			bool const booleanStringInvalidConv = booleanStringInvalid.ToBool(&result);
			gd_testing_verify(!booleanStringInvalidConv && !result);
		}

		{
			bool result;

			String const integerStringMinus1234("-1234");
			Int64 const integerStringMinus1234Conv = integerStringMinus1234.ToInt64(&result);
			gd_testing_verify(integerStringMinus1234Conv == -1234 && result);

			String const integerStringMinus1234UFO("-1234UFO");
			Int64 const integerStringMinus1234UFOConv = integerStringMinus1234UFO.ToInt64(&result);
			gd_testing_verify(integerStringMinus1234Conv == -1234 && !result);

			String const integerStringUFOMinus1234("UFO-1234");
			Int64 const integerStringUFOMinus1234Conv = integerStringUFOMinus1234.ToInt64(&result);
			gd_testing_verify(integerStringUFOMinus1234Conv == 0 && !result);

			/*String const hexIntegerStringMinus1234("-1234F");
			Int64 const hexIntegerStringMinus1234Conv = hexIntegerStringMinus1234.ToInt64(&result, 16);
			gd_testing_verify(hexIntegerStringMinus1234Conv == -0x1234F && result);*/
		}

		{
			bool result;

			String const floatStringMinus1234Dot567("-1234.567");
			Float64 const floatStringMinus1234Dot567Conv = floatStringMinus1234Dot567.ToFloat64(&result);
			gd_testing_verify(floatStringMinus1234Dot567Conv == -1234.567 && result);
		}
	};

	gd_testing_unit_test(StringMiscellaneous)
	{
		String const wildcardString("a*d*12?");
		String const wildcardCorrectString("abcdef123");
		String const wildcardIncorrectString("abcef123");
		gd_testing_verify(wildcardCorrectString.MatchesWildcard(wildcardString));
		gd_testing_verify(!wildcardIncorrectString.MatchesWildcard(wildcardString));
	};

	gd_testing_unit_test(StringFormatting)
	{
		String const formatString("%c-%d-%X-%.1f-%s");
		String const fortmattedString = String::Format(formatString.CStr(), '%', -3, 10, 0.5f, "wasted");
		gd_testing_verify(fortmattedString == "%--3-A-0.5-wasted");
	};


GD_NAMESPACE_END
#endif	// if GD_TESTING_ENABLED
