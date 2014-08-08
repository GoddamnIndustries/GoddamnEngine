#include <GoddamnEngine/Engine/Component/Impl/MeshRenderer/MeshRenderer.h>
#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>
#include <GoddamnEngine/Engine/Resource/Resource.h>

#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <DirectXMath.h>

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
		self->RendererLinkagePoint->SetShaderProgram(self->GetMaterial()->GetShaderProgram().GetPointer());

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
			self->OnMeshRendererParamChanged(self);
		}
	}

	void MeshRenderer::OnInitializeSelf()
	{
		self->GetGameObject()->GetTransform()->OnTransfromedEvent += self;
		self->OnMeshRendererParamChangedEvent += self;
		self->OnMeshRendererParamChangedEvent.TriggerAndLaunchEvent();
	}

	void MeshRenderer::OnDestroySelf(bool const IsForceDestruction)
	{
		self->GetGameObject()->GetTransform()->OnTransfromedEvent -= self;
		self->OnMeshRendererParamChangedEvent -= self;
	}
	
	void MeshRenderer::OnRenderSelf(RefPtr<Camera> const& TheCamera)
	{
		if (self->RendererVertexShaderInstance != nullptr) {
			Matrix4x4 const& MatrixModel  = self->GetGameObject()->GetTransform()->GetTransformMatrix();
			Matrix4x4 const  MatrixMVP    = MatrixModel * TheCamera->GetViewMatrix() * TheCamera->GetProjectionMatrix();
		//	Matrix3   const  MatrixNormal = Matrix3(Matrix4x4(MatrixModel).Inverse()).Transpose();

		//	using namespace DirectX;
		//	XMMATRIX m = XMMatrixPerspectiveFovLH(60.0f / 57.295779513082320876798154814105f, 16.0f / 9.0f, 0.3f, 1000.0f);
		//	self->RendererMatrixMvp->SetValue(&m.r[0]);
		
			self->RendererMatrixMvp   ->SetValue<Matrix4x4>(MatrixMVP);
		//	self->RendererMatrixModel ->SetValue<Matrix4x4>(MatrixModel);
		//	self->RendererMatrixNormal->SetValue<Matrix3>(MatrixNormal);
		}

		self->RendererVertexShaderInstance->GetInstanceFirstConstantBuffersLocation()->UploadAllParameters();
		self->GetMaterial()->GetShaderProgram()->GetProgramVertexShader()->BindShader(self->RendererVertexShaderInstance);
		self->GetMaterial()->BindMaterial();
		self->RendererLinkagePoint->RenderSelf();
	}
	
GD_NAMESPACE_END
