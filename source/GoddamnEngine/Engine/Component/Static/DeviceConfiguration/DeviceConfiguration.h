#ifndef GD_SYSTEM_DEVICE_CONFIGURATION
#define GD_SYSTEM_DEVICE_CONFIGURATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Component/Static/StaticComponent.h>

GD_NAMESPACE_BEGIN

	enum class CpuArchitecture : UInt8
	{
		Intel,		///< Intel x86 arch
		Amd64,		///< AMD64 arch
		Ia64,		///< Intel Itanium-based arch
		Arm,		///< ARM arch
		Unknown		///< Unknown CPU arch
	};

	/// @brief		Detects hardware and software preferences
	///				This class represents configuration of device engine running on,
	///				such as number of cores, total RAM size, preferred rendering system,
	///				operating system name and version, etc.
	class DeviceConfiguration :	public StaticComponent,
								public Singleton<DeviceConfiguration>
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(DeviceConfiguration, StaticComponent, GDAPI);

	public:
		GDINT DeviceConfiguration();
		GDINT virtual ~DeviceConfiguration();

		/// @brief		Returns total number of cores this device has
		/// @returns	Number of cores that CPU of this device contains
		GDAPI size_t GetCoresCount() const;

		/// @brief		Returns the arch of current device
		/// @returns	The arch of current device
		GDAPI CpuArchitecture GetCpuArchitecture() const;

		/// @brief		Returns total number of RAM installed on this device
		/// @returns	Total number of RAM installed on this device in megabytes
		GDAPI size_t GetRamSize() const;

		/// @brief		Returns name of user-preferred (or hardware) rendering system
		/// @param name	Output for name of user-preferred (or hardware) rendering system
		GDAPI void GetRenderingSystemName(String& name);
	};

#if 0
	template<class RenderingSystemElement>
	class RenderingSystemElementConstructor
	{
	public:
		GDINL static Object* ConstructElement(Object* const parent, handle const argument)
		{
			// String systemPostfix = "";
			// DeviceConfiguration::GetInstance().GetRenderingSystemName(systemPostfix);
			String const typeName = RenderingSystemElement::GetClassTypeInformation()->Name + 
				/*"_" + systemPostfix*/ "_D3D11";
			
			TypeInformation const* const typeInformation = TypeInformation::SearchForType(typeName);
			GD_ASSERT((typeInformation != nullptr), 
				"No type information specified for type '%s'", &typeName[0]);
			GD_ASSERT((typeInformation->VirtualConstructor != nullptr), 
				"'%s' is not constructible", &typeName[0]);

			TypeInformation* const 
				thisTypeInformation = (TypeInformation*)RenderingSystemElement::GetClassTypeInformation();
				thisTypeInformation->VirtualConstructor = typeInformation->VirtualConstructor;

			return typeInformation->VirtualConstructor(parent, argument);
		}
	};
#endif

GD_NAMESPACE_END

#endif