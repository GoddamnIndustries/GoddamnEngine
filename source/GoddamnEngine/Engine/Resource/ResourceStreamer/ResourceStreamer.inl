
GD_NAMESPACE_BEGIN

	GDINL ResourceStreamer::ResourceStreamer() :
		resourceStreamerMode(ResourceStreamerMode::NotLaunched)
	{
	}

	GDINL ResourceStreamer::~ResourceStreamer()
	{
	}

	template<class ResourceType>
	GDINL ResourceType* ResourceStreamer::LoadImmediately(String const& identifier)
	{
		static_assert((TypeTraits::IsBaseType<Resource, ResourceType>::Value),
			"Type 'ResourceType' should be derived from 'Resource'");

		return ((ResourceType*)ResourceStreamer::GetInstance().LoadImmediately(identifier, 
			ResourceType::GetClassTypeInformation()));
	}

	template<class ResourceType>
	GDINL ResourceType* ResourceStreamer::ProcessStreaming(String const& identifier)
	{
		static_assert((TypeTraits::IsBaseType<Resource, ResourceType>::Value),
			"Type 'ResourceType' should be derived from 'Resource'");

		return ((ResourceType*)ResourceStreamer::GetInstance().ProcessStreaming(identifier, 
			ResourceType::GetClassTypeInformation()));
	}

GD_NAMESPACE_END
