#include <GoddamnEngine/Engine/Renderer/Format/Format.h>

GD_NAMESPACE_BEGIN

	///// ==========================================================================================
	///// ==========================================================================================
	//extern void FormatAbstractValueFromString(
	//	String const& valueString,
	//	size_t const valueElementSize,
	//	size_t const valueElementsCount,
	//	FormatType const valueFormatType,
	//	handle const value
	//)
	//{
	//	String formatString = "";
	//	switch (valueFormatType)
	//	{
	//	case GD_FORMAT_TYPE_UINT:
	//		switch (valueElementSize)
	//		{
	//		case GD_FORMAT_SIZE_8BITS:  formatString = "%hhu"; break;
	//		case GD_FORMAT_SIZE_16BITS: formatString = "%hu";  break;
	//		case GD_FORMAT_SIZE_32BITS: formatString = "%u";   break;
	//		case GD_FORMAT_SIZE_64BITS: formatString = "%lu";  break;
	//		default: GD_DEBUG_ASSERT_FALSE("Invalid element size specified");
	//		} break;
	//	case GD_FORMAT_TYPE_SINT:
	//		switch (valueElementSize)
	//		{
	//		case GD_FORMAT_SIZE_8BITS:  formatString = "%hhd"; break;
	//		case GD_FORMAT_SIZE_16BITS: formatString = "%hd";  break;
	//		case GD_FORMAT_SIZE_32BITS: formatString = "%d";   break;
	//		case GD_FORMAT_SIZE_64BITS: formatString = "%ld";  break;
	//		default: GD_DEBUG_ASSERT_FALSE("Invalid element size specified");
	//		} break;
	//	case GD_FORMAT_TYPE_FLOAT:
	//		switch (valueElementSize)
	//		{
	//		// case GD_FORMAT_SIZE_8BITS:  formatString = "%hhf"; break;
	//		// case GD_FORMAT_SIZE_16BITS: formatString = "%hf";  break;
	//		case GD_FORMAT_SIZE_32BITS: formatString = "%f";   break;
	//		case GD_FORMAT_SIZE_64BITS: formatString = "%lf";  break;
	//		default: GD_DEBUG_ASSERT_FALSE("Invalid element size specified");
	//		} break;
	//	default: GD_DEBUG_ASSERT_FALSE("Invalid element type type specified");
	//	}

	//	String valueStringCopy = valueString;
	//	for (size_t cnt = 0; cnt < valueElementsCount; cnt += 1)
	//	{
	//		String const valueStringCurrent = valueStringCopy.Split(valueStringCopy.Find(' ') + 1);
	//		GD_ASSERT((sscanf(&valueStringCurrent[0], &formatString[0],
	//			reinterpret_cast<UInt8*>(value) + (cnt * valueElementSize / 8)) == 1), "");
	//	}
	//}

	/// ==========================================================================================
	/// ==========================================================================================
	/*extern String FormatAbstractValueToString(
		size_t const valueElementSize,
		size_t const valueElementCount,
		FormatType const valueFormatType,
		chandle const value
	)
	{
		GD_NOT_USED(valueElementSize);
		GD_NOT_USED(valueElementCount);
		GD_NOT_USED(valueFormatType);
		GD_NOT_USED(value);
		GD_NOT_IMPLEMENTED();
		return "";
	}*/


GD_NAMESPACE_END
