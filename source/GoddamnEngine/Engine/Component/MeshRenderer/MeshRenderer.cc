#include <GoddamnEngine/Engine/Component/MeshRenderer/MeshRenderer.hh>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.hh>
#include <GoddamnEngine/Engine/Component/Transform/Transform.hh>
#include <GoddamnEngine/Engine/Resource/Resource.hh>

#include <GoddamnEngine/Engine/Renderer/Renderer.hh>

GD_NAMESPACE_BEGIN

	GD_SERIALIZABLE_IMPLEMENTATION(MeshRenderer, Component, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(MeshRenderer);

	MeshRenderer::MeshRenderer() 
	 : GD_EXTENDS_SERIALIZABLE(Component)
	 , OnMeshRendererParamChangedEvent(&IOnMeshRendererParamChangedListener::OnMeshRendererParamChanged)
	 , RendererLinkagePoint           ( HRInterface::GetInstance().CreateLinkagePoint()                )
	{
	}

	MeshRenderer::~MeshRenderer()
	{
		self->RendererLinkagePoint->RemoveReference();
		SafeRelease(self->RendererVertexShaderInstance);
		SafeRelease(self->RendererMatrixMvp);
		SafeRelease(self->RendererMatrixModel);
		SafeRelease(self->RendererMatrixNormal);
	}

	/// ==========================================================================================
	// Messaging functions
	/// ==========================================================================================

	void MeshRenderer::OnMeshRendererParamChanged(Component* const component)
	{
		SafeRelease(self->RendererVertexShaderInstance);
		SafeRelease(self->RendererMatrixMvp);
		SafeRelease(self->RendererMatrixModel);
		SafeRelease(self->RendererMatrixNormal);

		self->RendererVertexShaderInstance = HRInterface::GetInstance().CreateShaderInstance(self->GetMaterial()->GetShaderProgram()->GetProgramVertexShader()->ShaderDesc);

		self->RendererLinkagePoint->SetIndexedShape (self->GetStaticMesh()->GetLodAt(0));
		self->RendererLinkagePoint->SetShaderProgram(self->GetMaterial()->GetShaderProgram());

		self->RendererMatrixMvp    = self->RendererVertexShaderInstance->GetParamByName("VertexMvpMatrix");
		self->RendererMatrixModel  = self->RendererVertexShaderInstance->GetParamByName("VertexModelMatrix");
		self->RendererMatrixNormal = self->RendererVertexShaderInstance->GetParamByName("VertexNormalMatrix");
		
		GD_ASSERT((self->RendererMatrixMvp    != nullptr), "'VertexMvpMatrix' was not found in vertex shader");
		GD_ASSERT((self->RendererMatrixModel  != nullptr), "'VertexModelMatrix' was not found in vertex shader");
		GD_ASSERT((self->RendererMatrixNormal != nullptr), "'VertexNormalMatrix' was not found in vertex shader");

		self->RendererMatrixMvp   ->AddReference();
		self->RendererMatrixModel ->AddReference();
		self->RendererMatrixNormal->AddReference();
	}

	void MeshRenderer::OnTransformed(Component* const transformer)
	{
		if (self->RendererVertexShaderInstance == nullptr)
		{
			// self->OnMeshRendererParamChanged(self);
		}
	}

	void MeshRenderer::OnInitializeSelf()
	{
		self->GetGameObject()->GetTransform()->OnTransfromedEvent += self;
		self->OnMeshRendererParamChangedEvent += self;
		self->OnMeshRendererParamChangedEvent.TriggerAndLaunchEvent();
	}

	void MeshRenderer::OnDestroySelf()
	{
		self->GetGameObject()->GetTransform()->OnTransfromedEvent -= self;
		self->OnMeshRendererParamChangedEvent -= self;
	}
	
	void MeshRenderer::OnRenderSelf(Camera const* const camera)
	{
		if (self->RendererVertexShaderInstance != nullptr)
		{
			Matrix4x4 const& MatrixModel  = self->GetGameObject()->GetTransform()->GetTransformMatrix();
			Matrix4x4 const  MatrixVp     = camera->GetProjectionMatrix() * camera->GetViewMatrix();
			Matrix4x4 const  MatrixMvp    = MatrixVp * MatrixModel;
		//	Matrix3 const  MatrixNormal = Matrix3(Matrix4x4(MatrixModel).Inverse()).Transpose();

			self->RendererMatrixMvp   ->SetValue<Matrix4x4>(MatrixMvp);
			self->RendererMatrixModel ->SetValue<Matrix4x4>(MatrixModel);
		//	self->RendererMatrixNormal->SetValue<Matrix3>(MatrixNormal);
		}

		self->RendererVertexShaderInstance->GetInstanceFirstConstantBuffersLocation()->UploadAllParameters();
		self->GetMaterial()->GetShaderProgram()->GetProgramVertexShader()->BindShader(self->RendererVertexShaderInstance);
		self->GetMaterial()->BindMaterial();
		self->RendererLinkagePoint->RenderSelf();
	}
	
GD_NAMESPACE_END
