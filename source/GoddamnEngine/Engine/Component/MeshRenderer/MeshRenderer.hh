#ifndef GD_ENGINE_COMPONENT_MESH_RENDERER
#define	GD_ENGINE_COMPONENT_MESH_RENDERER

#include <GoddamnEngine/Include.hh>

#include <GoddamnEngine/Engine/Resource/Resource.hh>
#include <GoddamnEngine/Engine/Resource/Impl/StaticMesh/StaticMesh.hh>
#include <GoddamnEngine/Engine/Resource/Impl/Material/Material.hh>

#include <GoddamnEngine/Engine/Component/Component.hh>
#include <GoddamnEngine/Engine/Component/Camera/Camera.hh>

GD_NAMESPACE_BEGIN

	class HRILinkagePoint;
	class HRIShaderInstance;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// 
	class IOnMeshRendererParamChangedListener
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		///
		GDAPI virtual void OnMeshRendererParamChanged(
			_In_ Component* const component
		) abstract;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Renders mesh
	class MeshRenderer : public Component,
						 private IOnMeshRendererParamChangedListener,
						 private IOnTransformedListener
	{
		friend class Camera;

	private:
		GD_SERIALIZABLE_DEFINITION(MeshRenderer, Component, GDAPI);

		GD_SERIALIZATION_BEGIN(MeshRenderer, Component);
			GD_FGDLD(StaticMesh*,	RendererMesh);			
			GD_FGDLD(Material*,		RendererMaterial);
			GD_FGDLD(bool,			RendererCastShadows); 
			GD_FGDLD(bool,			RendererRecieveShadows); 
		GD_SERIALIZATION_END();

		HRILinkagePoint  * RendererLinkagePoint         = nullptr;
		HRIShaderInstance* RendererVertexShaderInstance = nullptr;
		HRIShaderParam   * RendererMatrixMvp            = nullptr;
		HRIShaderParam   * RendererMatrixModel          = nullptr;
		HRIShaderParam   * RendererMatrixNormal         = nullptr;

	protected:
		GDINT MeshRenderer();
		GDINT virtual ~MeshRenderer();

	public:
		EventQueue<IOnMeshRendererParamChangedListener, Component*> OnMeshRendererParamChangedEvent;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// @name Getters / Setters
		/// @{

		//////////////////////////////////////////////////////////////////////////
		/// Makes mesh cast if this enabled
		GDINL bool GetCastShadows(
		) const { return self->RendererCastShadows; }

		//////////////////////////////////////////////////////////////////////////
		/// Makes mesh cast if this enabled
		GDINL void SetCastShadows(
			_In_ bool const castShadows
		) { self->RendererCastShadows = castShadows; /*self->OnParameterChanged.TriggerEvent();*/ }

		//////////////////////////////////////////////////////////////////////////
		/// Makes mesh receive shadows on it's surface if this enabled
		GDINL bool GetReceiveShadows(
		) const { return self->RendererRecieveShadows; }

		//////////////////////////////////////////////////////////////////////////
		/// Makes mesh receive shadows on it's surface if this enabled
		GDINL void SetReceiveShadows(
			_In_ bool const receiveShadows
		) { self->RendererRecieveShadows = receiveShadows; /*self->OnParameterChanged.TriggerEvent();*/ } 	

		//////////////////////////////////////////////////////////////////////////
		/// Returns static mesh attached to this mesh renderer
		GDINL StaticMesh* GetStaticMesh(
		) const	{ return self->RendererMesh; }

		//////////////////////////////////////////////////////////////////////////
		/// Changes static mesh attached to this object
		GDINL void SetStaticMesh(StaticMesh* const mesh)
		{
			GD_ASSERT((mesh != nullptr), "'mesh' could not be 'nullptr'");
			self->RendererMesh = mesh;
			self->OnMeshRendererParamChangedEvent.TriggerEvent();
		}

		//////////////////////////////////////////////////////////////////////////
		/// Returns material attached to this mesh renderer
		GDINL Material* GetMaterial(
		) const	{ return self->RendererMaterial; }

		//////////////////////////////////////////////////////////////////////////
		/// Changes material attached to this mesh renderer
		GDINL void SetMaterial(
			_In_ Material* const material
		)
		{
			GD_ASSERT((material != nullptr), "'material' could not be 'nullptr'");
			self->RendererMaterial = material;
			self->OnMeshRendererParamChangedEvent.TriggerEvent();
		}

		/// @}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		GDAPI virtual void RenderWithMaterial(Camera* const camera, Material* const material) const {};

	protected:
		GDAPI virtual void OnInitializeSelf();
		GDAPI virtual void OnDestroySelf();
		GDAPI virtual void OnRenderSelf(
			_In_ Camera const* const camera
		);
		GDAPI virtual void OnMeshRendererParamChanged(
			_In_ Component* const component
		);

		GDAPI virtual void OnTransformed(
			_In_ Component* const transformer
		);
	};

GD_NAMESPACE_END

#endif