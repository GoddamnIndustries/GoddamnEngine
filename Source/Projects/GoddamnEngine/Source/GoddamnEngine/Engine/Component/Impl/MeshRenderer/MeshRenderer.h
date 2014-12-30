#ifndef GD_ENGINE_COMPONENT_MESH_RENDERER
#define	GD_ENGINE_COMPONENT_MESH_RENDERER

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Engine/Resource/Resource.h>
#include <GoddamnEngine/Engine/Resource/Impl/StaticMesh/StaticMesh.h>
#include <GoddamnEngine/Engine/Resource/Impl/Material/Material.h>

#include <GoddamnEngine/Engine/Component/Component.h>
#include <GoddamnEngine/Engine/Component/Impl/Camera/Camera.h>

GD_NAMESPACE_BEGIN

	class HRILinkagePoint;
	class HRIShaderInstance;

	/// ==========================================================================================
	/// ==========================================================================================
	/// 
	class IOnMeshRendererParamChangedListener
	{
	public:
		/// ==========================================================================================
		///
		GDAPI virtual void OnMeshRendererParamChanged(
			Component* const component
		) abstract;
	};

	/// ==========================================================================================
	/// ==========================================================================================
	/// Renders mesh
	class MeshRenderer : public Component,
						 private IOnMeshRendererParamChangedListener,
						 private IOnTransformedListener
	{
		friend class Camera;

	private:
		GD_SERIALIZABLE_DEFINITION(MeshRenderer, Component, GDAPI);

		GD_SERIALIZATION_BEGIN(MeshRenderer, Component);
			GD_FIELD(RefPtr<StaticMesh>, RendererMesh);			
			GD_FIELD(RefPtr<Material>,   RendererMaterial);
			GD_FIELD(bool,			RendererCastShadows); 
			GD_FIELD(bool,			RendererRecieveShadows); 
		GD_SERIALIZATION_END();

		HRILinkagePoint  * RendererLinkagePoint   = nullptr;
		HRIShaderInstance* RendererVertexShaderInstance = nullptr;
		HRIShaderParam   * RendererMatrixMvp   = nullptr;
		HRIShaderParam   * RendererMatrixModel    = nullptr;
		HRIShaderParam   * RendererMatrixNormal   = nullptr;

	protected:
		GDINT MeshRenderer();
		GDINT virtual ~MeshRenderer();

	public:
		EventQueue<IOnMeshRendererParamChangedListener, Component*> OnMeshRendererParamChangedEvent;

		/// ==========================================================================================
		/// ==========================================================================================
		/// @name Getters / Setters
		/// @{

		/// ==========================================================================================
		/// Makes mesh cast if this enabled
		GDINL bool GetCastShadows(
		) const { return this->RendererCastShadows; }

		/// ==========================================================================================
		/// Makes mesh cast if this enabled
		GDINL void SetCastShadows(
			bool const castShadows
		) { this->RendererCastShadows = castShadows; /*this->OnParameterChanged.TriggerEvent();*/ }

		/// ==========================================================================================
		/// Makes mesh receive shadows on it's surface if this enabled
		GDINL bool GetReceiveShadows(
		) const { return this->RendererRecieveShadows; }

		/// ==========================================================================================
		/// Makes mesh receive shadows on it's surface if this enabled
		GDINL void SetReceiveShadows(
			bool const receiveShadows
		) { this->RendererRecieveShadows = receiveShadows; /*this->OnParameterChanged.TriggerEvent();*/ } 	

		/// ==========================================================================================
		/// Returns static mesh attached to this mesh renderer
		GDINL RefPtr<StaticMesh> GetStaticMesh() const { return this->RendererMesh; }

		/// ==========================================================================================
		/// Changes static mesh attached to this object
		GDINL void SetStaticMesh(RefPtr<StaticMesh> const& mesh)
		{
			GD_ASSERT((mesh != nullptr), "'mesh' could not be 'nullptr'");
			this->RendererMesh = mesh;
			this->OnMeshRendererParamChangedEvent.TriggerEvent();
		}

		/// ==========================================================================================
		/// Returns material attached to this mesh renderer
		GDINL RefPtr<Material> GetMaterial(
		) const	{ return this->RendererMaterial; }

		/// ==========================================================================================
		/// Changes material attached to this mesh renderer
		GDINL void SetMaterial(RefPtr<Material> const& material)
		{
			GD_ASSERT((material != nullptr), "'material' could not be 'nullptr'");
			this->RendererMaterial = material;
			this->OnMeshRendererParamChangedEvent.TriggerEvent();
		}

		/// @}
		/// ==========================================================================================
		/// ==========================================================================================

		GDAPI virtual void RenderWithMaterial(Camera* const camera, Material* const material) const {};

	protected:
		GDAPI virtual void OnInitializeSelf() override final;
		GDAPI virtual void OnDestroySelf(bool const IsForceDestruction) override final;
		GDAPI virtual void OnRenderSelf(RefPtr<Camera> const& TheCamera) override final;
		GDAPI virtual void OnMeshRendererParamChanged(Component* const component) override final;
		GDAPI virtual void OnTransformed(Component* const transformer) override final;
	};

GD_NAMESPACE_END

#endif