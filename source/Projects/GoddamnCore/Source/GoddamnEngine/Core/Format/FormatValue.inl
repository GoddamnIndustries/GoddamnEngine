
GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	GDINL void FormatValueFromString(
		String const& valueString,
		Format const valueFormat,
		handle const value
	)
	{
		FormatAbstractValueFromString(
			valueString, 
			GD_FORMAT_SIZE_EXTRACT(valueFormat),
			GD_FORMAT_COUNT_EXTRACT(valueFormat),
			GD_FORMAT_TYPE_EXTRACT(valueFormat),
			value
		);
	}

	/// ==========================================================================================
	GDINL String FormatValueToString(
		Format const valueFormat,
		chandle const value
	)
	{
		return FormatAbstractValueToString(
			GD_FORMAT_SIZE_EXTRACT(valueFormat),
			GD_FORMAT_COUNT_EXTRACT(valueFormat),
			GD_FORMAT_TYPE_EXTRACT(valueFormat),
			value
		);
	}

GD_NAMESPACE_END
