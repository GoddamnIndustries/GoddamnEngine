#ifndef GD_CORE_FORMAT_VALUE
#define GD_CORE_FORMAT_VALUE

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Format/Format.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable string (specific format) into memory
	GDAPI extern void FormatAbstractValueFromString(
		_In_ String const& valueString,
		_In_ size_t const valueElementSize,
		_In_ size_t const valueElementsCount,
		_In_ FormatType const valueFormatType,
		_In_ handle const value
	);

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable string (specific format) into memory
	GDAPI extern String FormatAbstractValueToString(
		_In_ size_t const valueElementSize,
		_In_ size_t const valueElementCount,
		_In_ FormatType const valueFormatType,
		_In_ chandle const value
	);

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable string into memory
	GDINL void FormatValueFromString(
		_In_ String const& valueString,
		_In_ Format const valueFormat,
		_In_ handle const value
	);

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable memory into string
	GDINL String FormatValueToString(
		_In_ Format const valueFormat,
		_In_ chandle const value
	);

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Format/FormatValue.inl>

#endif