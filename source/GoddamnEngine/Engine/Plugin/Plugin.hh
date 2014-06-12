#ifndef GD_ENGINE_PLUGIN_PLUGIN
#define GD_ENGINE_PLUGIN_PLUGIN

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Object/Object.hh>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.hh>

GD_NAMESPACE_BEGIN

	typedef UInt8 PluginType;
	enum PluginTypeEnumeration : PluginType
	{
		GD_PLUGIN_TYPE_UNKNOWN = 0,	///< Unknown plugin type. Do not use this 
		GD_PLUGIN_TYPE_RENDERER,	///< This plugin provides custom renderer for GD
		GD_PLUGIN_TYPE_PHYSICS,		///< This plugin provides custom physics for GD
		GD_PLUGIN_TYPE_WEB,			///< This plugin provides custom web services for GD 
		GD_PLUGIN_TYPE_EDITOR,		///< This plugin is for editor
		GD_PLUGIN_TYPES_CUSTOM,		///< This is custom plugin
		GD_PLUGIN_TYPES_COUNT		///< Total number of plugin types available
	};

	/// Pointer to this structure is passed to describe the plugin
	struct PluginDescription
	{
		typedef UInt8 PluginLoadingFlags;
		static_assert(((8 * sizeof(PluginLoadingFlags)) >= GD_PLUGIN_TYPES_COUNT),
			"PluginLoadingFlags`s type is not large enough to represent flags for plugins flags");

		TypeInformation const*	PluginTypeInformation;	///< Type information about plugin 
		char const*				Description;			///< String description of plugin
		PluginType				Type;					///< Type of this plugin
	};

	typedef bool(*PluginEntryProc)(PluginDescription* const pluginDescription);

	template<PluginType const SpecifiedPluginType>
	class Plugin : public Object
	{
		static_assert(((SpecifiedPluginType != GD_PLUGIN_TYPES_COUNT) 
			&& (SpecifiedPluginType != GD_PLUGIN_TYPE_UNKNOWN)),
			"Specification plugins for 'GD_PLUGIN_TYPES_COUNT/UNKNOWN' are not allowed");

	public:
		/// Type of this plugin is
		static const PluginType Type = SpecifiedPluginType;

	protected:
		GDINL Plugin() {}
		GDINL virtual ~Plugin() {}
	};

	class PluginManager final : public Object,
								public Singleton<PluginManager>
	{
	private:
		PluginDescription::PluginLoadingFlags pluginLoadingFlags;

	public:
		GDAPI PluginManager();
		GDAPI ~PluginManager();
	};

GD_NAMESPACE_END

#endif
