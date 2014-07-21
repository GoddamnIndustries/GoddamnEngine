#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>

GD_NAMESPACE_BEGIN

	GDINL Object* CreateInput(Object* parent, handle const argument)
	{
		TypeInformation const* const inputTypeInfo = TypeInformation::SearchForType("Input_WIN32_WINAPI");
		GD_ASSERT((inputTypeInfo != nullptr), "No input class specified");

		return inputTypeInfo->VirtualConstructor(parent, argument);
	}

	GD_TYPEINFORMATION_IMPLEMENTATION_C(Input, StaticComponent, GDAPI, CreateInput);
	GD_SINGLETON_IMPLEMENTATION(Input);

GD_NAMESPACE_END
