
GD_NAMESPACE_BEGIN

	GDINL ResourceStreamer::ResourceStreamer() 
		: resourceStreamerMode(ResourceStreamerMode::NotLaunched)
	{
	}

	GDINL ResourceStreamer::~ResourceStreamer()
	{
	}

	template<typename ResourceType>
	GDINL ResourceType* ResourceStreamer::LoadImmediately(String const& ID)
	{
		static_assert((TypeTraits::IsBaseType<Resource, ResourceType>::Value), "Type 'ResourceType' should be derived from 'Resource'");
		return (object_cast<ResourceType*>(ResourceStreamer::GetInstance().LoadImmediately(ID, ResourceType::GetClassTypeInformation())));
	}

	template<typename ResourceType>
	GDINL ResourceType* ResourceStreamer::ProcessStreaming(String const& ID)
	{
		static_assert((TypeTraits::IsBaseType<Resource, ResourceType>::Value), "Type 'ResourceType' should be derived from 'Resource'");
		return (object_cast<ResourceType*>(ResourceStreamer::GetInstance().ProcessStreaming(ID, ResourceType::GetClassTypeInformation())));
	}

GD_NAMESPACE_END
