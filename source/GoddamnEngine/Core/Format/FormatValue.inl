
GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	GDINL void FormatValueFromString(
		_In_ String const& valueString,
		_In_ Format const valueFormat,
		_In_ handle const value
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
		_In_ Format const valueFormat,
		_In_ chandle const value
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
