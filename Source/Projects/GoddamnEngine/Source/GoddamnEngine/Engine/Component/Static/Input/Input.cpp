#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>

GD_NAMESPACE_BEGIN

	GDINL Object* CreateInput(Object* parent, handle const argument)
	{
		IClassInfo const* const inputTypeInfo = Reflection::FindType("Input_WIN32_WINAPI");
		GD_ASSERT((inputTypeInfo != nullptr), "No input class specified");

		//return inputTypeInfo->VirtualConstructor(parent, argument);
		return nullptr;
	}

	GD_CLASSINFO_IMPLEMENTATION(Input, StaticComponent, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(Input);

GD_NAMESPACE_END
