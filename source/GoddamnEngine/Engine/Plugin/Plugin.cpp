#include <GoddamnEngine/Engine/Plugin/Plugin.h>
#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>

#include <dirent.h>

GD_NAMESPACE_BEGIN
	GD_SINGLETON_IMPLEMENTATION(PluginManager);

	PluginManager::PluginManager() :
		pluginLoadingFlags(0)
	{
		DIR* pluginDirectory = opendir("./");
		GD_ASSERT((pluginDirectory != nullptr), 
			"Unable to locate director where plugins are located");

		struct dirent* entity = nullptr;
		while ((entity = readdir(pluginDirectory)) != nullptr)
		{
			String const pluginName = String(&entity->d_name[0]);
			Assembly const* const plugin = Assembly::LoadAssemblyFromPath(pluginName);
			do
			{
				if (!plugin->WasSuccessfullyLoaded())
					break;

				PluginEntryProc const PluginEntryPoint = ((PluginEntryProc)plugin->GetNativeMethod("EnginePluginEntry"));
				if (PluginEntryPoint == nullptr)
					break;

				PluginDescription pluginDescription;
				memset(&pluginDescription, 0, sizeof(pluginDescription));
				if (PluginEntryPoint(&pluginDescription))
				{
					if ((self->pluginLoadingFlags & pluginDescription.Type) != 0)
					{
						Debug::Warning("Plugin with specified type was already loaded");
						break;
					}

					if (pluginDescription.PluginTypeInformation->VirtualConstructor != nullptr)
						pluginDescription.PluginTypeInformation->VirtualConstructor(nullptr, nullptr);

					self->pluginLoadingFlags |= GD_BIT(pluginDescription.Type);
				//	const_cast<Assembly*>(plugin)->AttachToObject(self);
				}
			} while (false);
		//	plugin->RemoveReference();
		}

		closedir(pluginDirectory);
	}

	PluginManager::~PluginManager()
	{
	}

GD_NAMESPACE_END
