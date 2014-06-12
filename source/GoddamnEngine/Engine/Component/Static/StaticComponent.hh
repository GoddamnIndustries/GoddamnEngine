#ifndef GD_STATIC_COMPONENT
#define GD_STATIC_COMPONENT

#include <GoddamnEngine/Include.hh>

#include <GoddamnEngine/Core/Object/Singleton/Singleton.hh>

#include <GoddamnEngine/Engine/Component/Component.hh>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.hh>

#define GD_STATIC_COMPONENT_TYPEINFORMATION_DEFINITION(Type, ExpSpec) GD_TYPEINFORMATION_DEFINITION(Type, StaticComponent, ExpSpec) 
#define GD_STATIC_COMPONENT_TYPEINFORMATION_IMPLEMENTATION(Type, ExpSpec) GD_TYPEINFORMATION_IMPLEMENTATION(Type, StaticComponent, ExpSpec)

GD_NAMESPACE_BEGIN

	enum class StaticComponentPriority : byte
	{
		Highest,
		High,
		Normal,
		Low,
		Lowest,
		Unknown
	};

	class StaticComponent : public Component
	{
	private:
		GD_SERIALIZABLE_DEFINITION(StaticComponent, Component, GDAPI);

	public:
		const StaticComponentPriority Priority;

		GDAPI static void InitializeAsHandler(GameObject* const gameObject);

		GDAPI StaticComponent(const StaticComponentPriority priority = StaticComponentPriority::Normal);
	};

GD_NAMESPACE_END

#endif