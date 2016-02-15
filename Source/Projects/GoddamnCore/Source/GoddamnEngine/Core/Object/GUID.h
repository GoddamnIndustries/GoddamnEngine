// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/GUID.h
 * File contains global unique IDs for objects.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Object Global Unique Identifier.
	// **------------------------------------------------------------------------------------------**
	struct GUID final
	{
	private:
		union {
			UInt64 m_Parts[2];
			struct {
				UInt32 m_A;
				UInt16 m_B, m_C;
				UInt8  m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K;
			};
		};

	public:

		/*!
		 * Defines formatting attributes for string conversion.
		 */
		enum class Format
		{
			/*!
			 * 32 hexadecimal digits.
			 * Example: "00000000000000000000000000000000".
			 */
			HexDigits = 1,

			/*!
			 * 32 hexadecimal digits with dashes.
			 * Example: "00000000-0000-0000-0000-000000000000".
			 */
			HexDigitsWithHyphens,

			/*!
			 * 32 hexadecimal digits with hyphens in braces.
			 * Example: "{00000000-0000-0000-0000-000000000000}".
			 */
			HexDigitsWithHyphensInBraces,

			/*!
			 * 32 hexadecimal digits with hyphens in parentheses.
			 * Example: "(00000000-0000-0000-0000-000000000000)".
			 */
			HexDigitsWithHyphensInParentheses,

			/*!
			 * 12 hexadecimal comma-separated values in braces.
			 * Example: "{0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}".
			 */
			HexValuesInBraces,

			/*!
			 * 32 hexadecimal digits with dashes.
			 * This format is currently used by the Goddamn Engine.
			 * Example: "0000000000000000-0000000000000000".
			 */
			HexDigits64BitWithHyphen,
			
		};	// enum class Format

	public:

		/*!
		 * Initializes an empty GUID.
		 */
		GDINL explicit GUID() 
			: m_Parts{ 0, 0 } 
		{
		}

		/*!
		 * Initializes GUID with specified bytes (system endian).
		 * @param bytes Byte array with data.
		 */
		GDINL explicit GUID(Byte const bytes[16])
			: m_A(*reinterpret_cast<UInt32 const*>(bytes + 0))
			, m_B(*reinterpret_cast<UInt16 const*>(bytes + 4)), m_C(*reinterpret_cast<UInt16 const*>(bytes + 6))
			, m_D(bytes[8]), m_E(bytes[9]), m_F(bytes[10]), m_G(bytes[11]), m_H(bytes[12]), m_I(bytes[13]), m_J(bytes[14]), m_K(bytes[15])
		{
		}

		/*!
		 * Initializes GUID with specified components.
		 *
		 * @param a First 4 bytes of the GUID.
		 * @param b Next 2 bytes of the GUID.
		 * @param c Next 2 bytes of the GUID.
		 * @param d Next byte of the GUID. And so on.
		 */
		GDINL explicit GUID(UInt32 const a, UInt16 const b, UInt16 const c, UInt8 const d, UInt8 const e, UInt8 const f, UInt8 const g, UInt8 const h, UInt8 const i, UInt8 const j, UInt8 const k)
			: m_A(a)
			, m_B(b), m_C(c)
			, m_D(d), m_E(e), m_F(f), m_G(g), m_H(h), m_I(i), m_J(j), m_K(k)
		{
		}

		/*!
		 * Initializes GUID with specified components.
		 *
		 * @param a First 4 bytes of the GUID.
		 * @param b Next 2 bytes of the GUID.
		 * @param c Next 2 bytes of the GUID.
		 * @param d Next 8 bytes of the GUID.
		 */
		GDINL explicit GUID(UInt32 const a, UInt16 const b, UInt16 const c, UInt8 const d[8])
			: m_A(a)
			, m_B(b), m_C(c)
			, m_D(d[0]), m_E(d[1]), m_F(d[2]), m_G(d[3]), m_H(d[4]), m_I(d[5]), m_J(d[6]), m_K(d[7])
		{
		}

		/*!
		 * Initializes GUID with specified components.
		 *
		 * @param firstPart First part.
		 * @param secondPart Second part.
		 */
		GDINL explicit GUID(UInt64 const firstPart, UInt64 secondPart)
			: m_Parts{ firstPart, secondPart }
		{
		}

	public:

		/*!
		 * Generates a new GUID.
		 * @returns A brand new GUID.
		 */
		GDAPI static GUID New();

		/*!
		 * Tries to parse string to the GUID representation in the exact format.
		 *
		 * @param string The string GUID representation.
		 * @param format The exact format that this string should match.
		 * @param result Reference to output.
		 *
		 * @returns True if this string matches GUID.
		 */
		GDAPI static bool TryParseExact(String const& string, Format const format, GUID& result);

		/*!
		 * Tries to parse string to the GUID representation.
		 *
		 * @param string The string GUID representation.
		 * @param result Reference to output.
		 *
		 * @returns True if this string matches GUID.
		 */
		GDAPI static bool TryParse(String const& string, GUID& result);

		/*!
		 * Parses string to the GUID representation in the exact format.
		 *
		 * @param string The string GUID representation.
		 * @param format The exact format that this string should match.
		 *
		 * @returns Parsed GUID or the invalid GUID.
		 */
		GDAPI static GUID ParseExact(String const& string, Format const format);

		/*!
		 * Parses string to the GUID representation in the exact format.
		 *
		 * @param string The string GUID representation.
		 * @returns Parsed GUID or the invalid GUID.
		 */
		GDAPI static GUID Parse(String const& string);

	public:

		/*!
		 * Returns true if this GUID is valid.
		 */
		GDINL bool IsValid() const
		{
			return ((*this)[0] | (*this)[1]) != 0;
		}

		/*!
		 * Invalidates this GUID.
		 */
		GDINL void Invalidate() 
		{
			(*this)[0] = 0;
			(*this)[1] = 0;
		}

		/*!
		 * Returns string representation of this GUID in string format.
		 * @param format The format of the GUID string representation.
		 */
		GDAPI String ToString(Format const format = Format::HexDigits64BitWithHyphen) const;

	public:

		/*!
		 * Checks whether two GUIDs are equal.
		 */
		GDINL friend bool operator== (GUID const& a, GUID const& b)
		{
			return (a[0] ^ b[0] | a[1] ^ b[1]) == 0;
		}

		/*!
		 * Checks whether two GUIDs are different.
		 */
		GDINL friend bool operator!= (GUID const& a, GUID const& b)
		{
			return (a[0] ^ b[0] | a[1] ^ b[1]) != 0;
		}

		/*!
		 * Checks whether first GUID is greater then second.
		 */
		GDINL friend bool operator> (GUID const& a, GUID const& b)
		{
			return a[0] > b[0] ? true : (a[0] < b[0] ? false : a[1] > b[1]);
		}

	private:
		GDINL UInt64 const& operator[] (SizeTp const index) const
		{
			GD_ASSERT(index < 2, "Index is out of bounds");
			return m_Parts[index];
		}
		GDINL UInt64& operator[] (SizeTp const index)
		{
			return const_cast<UInt64&>(const_cast<GUID const&>(*this)[index]);
		}

	};	// struct GUID

	/*!
	 * Global constant for the empty GUID.
	 */
	static GUID const EmptyGUID;

GD_NAMESPACE_END
