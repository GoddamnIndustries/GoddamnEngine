#include <GoddamnEngine/Engine/Component/Static/DeviceConfiguration/DeviceConfiguration.h>

#if defined(GD_PLATFORM_WINDOWS)
#include <Windows.h>

GD_NAMESPACE_BEGIN

	GD_CLASSINFO_IMPLEMENTATION(DeviceConfiguration, StaticComponent, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(DeviceConfiguration);

	DeviceConfiguration::DeviceConfiguration() :
		GD_EXTENDS_SERIALIZABLE(StaticComponent)
	{
	}

	DeviceConfiguration::~DeviceConfiguration()
	{
	}

	// Implementation here is use on this base of this StackOverflow question:
	// http://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
	// I say great thanks to 'user1182474' and 'ceretullis'
	size_t DeviceConfiguration::GetCoresCount() const
	{
		SYSTEM_INFO sysinfo = { 0 };
		GetSystemInfo(&sysinfo);

		return (size_t)sysinfo.dwNumberOfProcessors;
	}

	CpuArchitecture DeviceConfiguration::GetCpuArchitecture() const
	{
		SYSTEM_INFO sysinfo = { 0 };
		GetSystemInfo(&sysinfo);

		switch(sysinfo.wProcessorArchitecture)	
		{
		case PROCESSOR_ARCHITECTURE_INTEL:	return CpuArchitecture::Intel;
		case PROCESSOR_ARCHITECTURE_AMD64:	return CpuArchitecture::Amd64;
		case PROCESSOR_ARCHITECTURE_ARM:	return CpuArchitecture::Arm;
		case PROCESSOR_ARCHITECTURE_UNKNOWN:
		default:							return CpuArchitecture::Unknown;
		}
	}

	size_t DeviceConfiguration::GetRamSize() const
	{
		MEMORYSTATUSEX 
			stateEx = { 0 };
			stateEx.dwLength = sizeof(stateEx);
		
		GlobalMemoryStatusEx(&stateEx);

		return (size_t)stateEx.ullAvailPhys / 1024;
	}

	void DeviceConfiguration::GetRenderingSystemName(String& name)
	{
		//// Attempting to initialize DirectX11
		//DWORD directxMajorVersion = 0;
		//DWORD directxMinorVersion = 0;
		//if (DirectXSetupGetVersion(&directxMajorVersion, &directxMajorVersion) != 0)
		//{
			name = "D3D11";
		//	return;
		//}

		//name = "OGL";
	}

GD_NAMESPACE_END

#endif