#ifndef GD_CORE_FORMAT_VALUE
#define GD_CORE_FORMAT_VALUE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Format/Format.h>
#include <GoddamnEngine/Core/Containers/String.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable string (specific format) into memory
	GDAPI extern void FormatAbstractValueFromString(
		String const& valueString,
		size_t const valueElementSize,
		size_t const valueElementsCount,
		FormatType const valueFormatType,
		handle const value
	);

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable string (specific format) into memory
	GDAPI extern String FormatAbstractValueToString(
		size_t const valueElementSize,
		size_t const valueElementCount,
		FormatType const valueFormatType,
		chandle const value
	);

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable string into memory
	GDINL void FormatValueFromString(
		String const& valueString,
		Format const valueFormat,
		handle const value
	);

	/// ==========================================================================================
	/// ==========================================================================================
	/// Loads value of formattable memory into string
	GDINL String FormatValueToString(
		Format const valueFormat,
		chandle const value
	);

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Format/FormatValue.inl>

#endif