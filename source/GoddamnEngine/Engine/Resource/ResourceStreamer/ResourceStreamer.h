#ifndef GD_RESOURCES_RESOURCE_STREAMER_RESOURCE_STREAMER
#define GD_RESOURCES_RESOURCE_STREAMER_RESOURCE_STREAMER

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Reflection/TypeInformation/TypeInformation.h>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.h>

#include <GoddamnEngine/Engine/Resource/Resource.h>

#define GD_LOAD_RESOURCE(ResourceId, ResourceType)														\
	((ResourceType*)ResourceStreamer::GetInstance().LoadImmediately<ResourceType>(ResourceId))

GD_NAMESPACE_BEGIN

	/// Class for resource management.
	class ResourceStreamer : public Object,
							 public Singleton<ResourceStreamer>
	{
		friend class Application;

	private:
		enum ResourceStreamerMode
		{
			NotLaunched,
			LaunchedStreaming,
			LaunchedNormalLoading
		};
		ResourceStreamerMode resourceStreamerMode;

		GDINL ResourceStreamer();

		GDINL virtual ~ResourceStreamer();

		GDINT void LockResourceStreamer() const;

		GDINT void UnlockResourceStreamer() const;

		GDINT Resource* CreateResource(String const& identifier, TypeInformation const* const typeInformation);

	public:

		/// Searches for resource with specified identifier in list and returns it if found
		/// nullptr over wise. If searchInRequests is set, then searching is also done in requests list
		GDAPI Resource* SearchForResource(String const& identifier, const bool searchInRequests = true) const;

		/// Searches for first resource in queue that was not loaded but just requested
		GDAPI Resource* SearchForFirstRequestedResource() const;

		/// Creates a resource with specified type and identifier (or  uses existing found by identifier) and 
		/// adds them to resource list. If resource streaming is enabled, than resource would be loaded in background. 
		/// Recommended using ResourceStreamer::WaitForLoading function to ensure, that all resources in queue are loaded.
		///	If resource streaming is disabled, than calling ResourceStreamer::WaitForLoading is required.
		///	to load all unloaded resource in current thread.
		GDAPI Resource* ProcessStreaming(String const& identifier, TypeInformation const* const typeInformation);

		/// @see ResourceStreamer::ProcessStreaming
		template<class ResourceType>
		GDINL ResourceType* ProcessStreaming(String const& identifier);

		/// Loads in to memory and/or to GPU already existing resource
		GDAPI Resource* LoadImmediately(Resource* const resource);

		/// Creates resource (or uses existing found by identifier) and loads resource in 
		GDAPI Resource* LoadImmediately(String const& identifier, TypeInformation const* const typeInformation);

		/// @see ResourceStreamer::LoadImmediately
		template<class ResourceType>
		GDINL ResourceType* LoadImmediately(String const& identifier);

		/// This function waits for resources request list to be empty.
		///	If streaming is disabled, than automatically loads all unloaded resources.
		GDAPI void WaitForLoading();
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.inl>

#endif
