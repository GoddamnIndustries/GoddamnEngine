#ifndef GD_COMPONENT_MESHRENDERER
#define	GD_COMPONENT_MESHRENDERER

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Engine/Resource/Resource.h>
#include <GoddamnEngine/Engine/Resource/ResourceInterfaces/StaticMesh/StaticMesh.h>
#include <GoddamnEngine/Engine/Resource/ResourceInterfaces/Material/Material.h>

#include <GoddamnEngine/Engine/Component/Component.h>
#include <GoddamnEngine/Engine/Component/Camera.h>

GD_NAMESPACE_BEGIN

	/// @brief	Renders mesh
	class MeshRenderer : public Component
	{
		friend class Camera;

	private:
		GD_SERIALIZABLE_DEFINITION(MeshRenderer, Component, GDAPI);

		GD_SERIALIZATION_BEGIN(MeshRenderer, Component);
			GD_FIELD(StaticMesh*,	mesh);			
			GD_FIELD(Material*,		material);
			GD_FIELD(bool,			castShadows); 
			GD_FIELD(bool,			recieveShadows); 
		GD_SERIALIZATION_END();

	protected:
		GDINT MeshRenderer();
		GDINT virtual ~MeshRenderer();

	public:
		
		/// @name Getters / Setters
		/// @{

		/// @brief					Makes mesh cast if this enabled
		/// @returns				Boolean for mesh casts shadows
		GDINL bool GetCastShadows() const
		{ 
			return self->castShadows; 
		}

		/// @brief					Makes mesh cast if this enabled
		/// @param castShadows		Boolean for mesh casts shadows
		GDINL void SetCastShadows(const bool castShadows) 
		{ 
			self->castShadows = castShadows; 
		}

		/// @brief					Makes mesh receive shadows on it's surface if this enabled
		/// @returns				Boolean for mesh can receive shadows on it's surface
		GDINL bool GetReceiveShadows() const 
		{ 
			return self->recieveShadows; 
		}

		/// @brief					Makes mesh receive shadows on it's surface if this enabled
		/// @param receiveShadows	Boolean for mesh can receive shadows on it's surface
		GDINL void SetReceiveShadows(const bool receiveShadows) 
		{ 
			self->recieveShadows = receiveShadows;
			self->EmitSignal("OnParameterChanged");
		} 	

		/// @brief					Returns static mesh attached to this mesh renderer
		/// @returns				Static mesh attached to this mesh renderer
		GDINL StaticMesh* GetStaticMesh() const	
		{ 
			return self->mesh; 
		}

		/// @brief					Changes static mesh attached to this object
		/// @param mesh				Static mesh would be attached to this mesh renderer
		GDINL void SetStaticMesh(StaticMesh* const mesh)
		{
			self->mesh = mesh;
			self->EmitSignal("OnParameterChanged");
		}

		/// @brief					Returns material attached to this mesh renderer
		/// @returns				Material attached to this mesh renderer
		GDINL Material* GetMaterial() const	
		{ 
			return self->material; 
		}

		/// @brief						Changes material attached to this mesh renderer
		/// @param material				Material would be attached to this mesh renderer
		GDINL void SetMaterial(Material* const material)
		{
			GD_ASSERT(material != nullptr, "'material' could not be 'nullptr'");
			self->material = material;
			self->EmitSignal("OnParameterChanged");
		}

		/// @}

		GDAPI virtual void RenderWithMaterial(Camera* const camera, Material* const material) const abstract;

	protected:
		GDINT virtual void OnParameterChanged() abstract;

		GDAPI virtual void OnDestroySelf();
	};

GD_NAMESPACE_END

#endif