#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.hh>

GD_NAMESPACE_BEGIN
	
	GD_SINGLETON_IMPLEMENTATION(ResourceStreamer);

	//////////////////////////////////////////////////////////////////////////
	void ResourceStreamer::LockResourceStreamer() const
	{
		if (self->resourceStreamerMode != ResourceStreamerMode::LaunchedStreaming)
		{
			return;
		}

		//!
	}

	//////////////////////////////////////////////////////////////////////////
	void ResourceStreamer::UnlockResourceStreamer() const 
	{
		if (self->resourceStreamerMode != ResourceStreamerMode::LaunchedStreaming)
		{
			return;
		}

		//!
	}

	//////////////////////////////////////////////////////////////////////////
	Resource* ResourceStreamer::CreateResource(String const& identifier, TypeInformation const* const typeInformation)
	{
		GD_ASSERT((typeInformation->VirtualConstructor != nullptr),
			"Attempted creation of abstract resource type");

		self->LockResourceStreamer();

		Resource* const 
			resource = ((Resource*)typeInformation->VirtualConstructor(nullptr, ((handle)&identifier)));
			resource->State = ResourceState::Requested;
			resource->AttachToObject(self);

		self->UnlockResourceStreamer();
		return resource;
	}

	//////////////////////////////////////////////////////////////////////////
	Resource* ResourceStreamer::SearchForResource(String const& identifier, const bool searchInRequests /* = true */) const
	{
		self->LockResourceStreamer();

		HashSumm const identifierHash = identifier.GetHashSumm();
		for (Resource const*
			resource = ((Resource const*)self->GetLastChildObject()); resource != nullptr;
			resource = ((Resource const*)resource->GetPreviousSiblingObject()))
		{
			if (resource->Identifier == identifierHash)
			{
				if (((resource->State == ResourceState::Requested) && searchInRequests)
					|| (resource->State != ResourceState::Requested))
				{
					self->UnlockResourceStreamer();
					return ((Resource*)resource);
				}
			}
		}

		self->UnlockResourceStreamer();
		return ((Resource*)nullptr);
	}

	//////////////////////////////////////////////////////////////////////////
	Resource* ResourceStreamer::LoadImmediately(Resource* const resource)
	{
		GD_ASSERT((resource != nullptr), 
			"Resource passed is nullptr");

		/*GD_ASSERT((resource->State == ResourceState::Requested),
			"Resource passed is already loaded");*/
		if (!(resource->State == ResourceState::Requested)) 
			return resource;
		else
		{
			UniquePtr<InputStream> Stream = resource->Identifier.OpenInputStream();
			resource->OnResourceLoad(Stream.GetPointer());
			resource->State = ResourceState::Loaded;
		}
		
		return resource;
	}

	//////////////////////////////////////////////////////////////////////////
	Resource* ResourceStreamer::LoadImmediately(String const& identifier, TypeInformation const* const typeInformation)
	{
		return self->LoadImmediately(self->ProcessStreaming(identifier, typeInformation));
	}

	//////////////////////////////////////////////////////////////////////////
	Resource* ResourceStreamer::ProcessStreaming(String const& identifier, TypeInformation const* const typeInformation)
	{
		Resource* resource = self->SearchForResource(identifier);
		if (resource != nullptr)
		{
			GD_ASSERT((resource->GetTypeInformation()->IsDerivedFrom(typeInformation)), 
				"Requested resource exists, but it's type is incompatible with passed one.");
		}
		else
		{
			resource = self->CreateResource(identifier, typeInformation);
		}

		return resource;
	}

	void ResourceStreamer::WaitForLoading()
	{
		switch(self->resourceStreamerMode)
		{
		case ResourceStreamerMode::NotLaunched:
			{
				/*size_t const coresCount = DeviceConfiguration::GetInstance().GetCoresCount();
				CpuArchitecture const cpuArchitecture = DeviceConfiguration::GetInstance().GetCpuArchitecture();

				String rendererName; 
				DeviceConfiguration::GetInstance().GetRenderingSystemName(rendererName);

				if (((coresCount >= 2) 
				&& (!((rendererName == "OGL") && (String(GD_PLATFORM_NAME) == "Windows")))
				&& (cpuArchitecture != CpuArchitecture::Arm)))*/
				{
					//class ResourceStreamerThread : public Thread,
					//							   public Singleton<ResourceStreamerThread>
					//{
					//public:
					//	GDINT ResourceStreamerThread() :
					//		Thread("Resource Streamer", ThreadFlags::UsingRenderer, ThreadPriority::Normal)
					//	{
					//	}

					//private:
					//	GDINT virtual void OnExecutionBegan() const
					//	{
					//		Debug::Log("ResourceStreamer launched");

					//		while (true/*Application::GetInstance().GetApplicationState() == ApplicationState::Running*/)
					//		{
					//			
					//		}

					//		Debug::Log("ResourceStreamer being disabled");
					//	}
					//};

					//Debug::Log("Launching ResourceStreamer in separate thread");
					//self->resourceStreamerMode = ResourceStreamerMode::LaunchedStreaming;
					//
					//new ResourceStreamerThread()->Execute();
				}
				//else
				{
					// Debug::Log("Resource streaming is unsupported: resources are loaded in main thread");
					self->resourceStreamerMode = ResourceStreamerMode::LaunchedNormalLoading;
				}

				//Engine/Resource/reamer::GetInstance().WaitForLoading();
			} break;

		case ResourceStreamerMode::LaunchedNormalLoading:
			{
				//while (self->requestedResources.GetSize() != 0)
				//{
				//	Resource* const resource = self->requestedResources.GetFirstElement().Value;
				//	ResourceStreamer::GetInstance().LoadImmediately(resource);
				//}
			} break;

		case ResourceStreamerMode::LaunchedStreaming:
			{
				//while (self->requestedResources.GetSize() != 0);
			} break;
		}
	}

GD_NAMESPACE_END
