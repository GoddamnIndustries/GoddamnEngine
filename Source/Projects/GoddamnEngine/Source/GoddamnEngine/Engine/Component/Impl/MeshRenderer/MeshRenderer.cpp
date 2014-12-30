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
		, RendererLinkagePoint     ( HRInterface::GetInstance().CreateLinkagePoint()    )
	{
	}

	MeshRenderer::~MeshRenderer()
	{
		this->RendererLinkagePoint->RemoveReference();
		SafeRelease(this->RendererVertexShaderInstance);
		SafeRelease(this->RendererMatrixMvp);
		SafeRelease(this->RendererMatrixModel);
		SafeRelease(this->RendererMatrixNormal);
	}

	/// ==========================================================================================
	// Messaging functions
	/// ==========================================================================================

	void MeshRenderer::OnMeshRendererParamChanged(Component* const component)
	{
		SafeRelease(this->RendererVertexShaderInstance);
		SafeRelease(this->RendererMatrixMvp);
		SafeRelease(this->RendererMatrixModel);
		SafeRelease(this->RendererMatrixNormal);

		this->RendererVertexShaderInstance = HRInterface::GetInstance().CreateShaderInstance(this->GetMaterial()->GetShaderProgram()->GetProgramVertexShader()->ShaderDesc);

		this->RendererLinkagePoint->SetIndexedShape (this->GetStaticMesh()->GetLodAt(0));
		this->RendererLinkagePoint->SetShaderProgram(this->GetMaterial()->GetShaderProgram().GetPointer());

		this->RendererMatrixMvp = this->RendererVertexShaderInstance->GetParamByName("VertexMvpMatrix");
		this->RendererMatrixModel  = this->RendererVertexShaderInstance->GetParamByName("VertexModelMatrix");
		this->RendererMatrixNormal = this->RendererVertexShaderInstance->GetParamByName("VertexNormalMatrix");
		
		GD_ASSERT((this->RendererMatrixMvp != nullptr), "'VertexMvpMatrix' was not found in vertex shader");
		GD_ASSERT((this->RendererMatrixModel  != nullptr), "'VertexModelMatrix' was not found in vertex shader");
		GD_ASSERT((this->RendererMatrixNormal != nullptr), "'VertexNormalMatrix' was not found in vertex shader");

		this->RendererMatrixMvp   ->AddReference();
		this->RendererMatrixModel ->AddReference();
		this->RendererMatrixNormal->AddReference();
	}

	void MeshRenderer::OnTransformed(Component* const transformer)
	{
		if (this->RendererVertexShaderInstance == nullptr)
		{
			this->OnMeshRendererParamChanged(this);
		}
	}

	void MeshRenderer::OnInitializeSelf()
	{
		this->GetGameObject()->GetTransform()->OnTransfromedEvent += this;
		this->OnMeshRendererParamChangedEvent += this;
		this->OnMeshRendererParamChangedEvent.TriggerAndLaunchEvent();
	}

	void MeshRenderer::OnDestroySelf(bool const IsForceDestruction)
	{
		this->GetGameObject()->GetTransform()->OnTransfromedEvent -= this;
		this->OnMeshRendererParamChangedEvent -= this;
	}
	
	void MeshRenderer::OnRenderSelf(RefPtr<Camera> const& TheCamera)
	{
		if (this->RendererVertexShaderInstance != nullptr) {
			Matrix4x4 const& MatrixModel  = this->GetGameObject()->GetTransform()->GetTransformMatrix();
			Matrix4x4 const  MatrixMVP = MatrixModel * TheCamera->GetViewMatrix() * TheCamera->GetProjectionMatrix();
		//	Matrix3   const  MatrixNormal = Matrix3(Matrix4x4(MatrixModel).Inverse()).Transpose();

		//	using namespace DirectX;
		//	XMMATRIX m = XMMatrixPerspectiveFovLH(60.0f / 57.295779513082320876798154814105f, 16.0f / 9.0f, 0.3f, 1000.0f);
		//	this->RendererMatrixMvp->SetValue(&m.r[0]);
		
			this->RendererMatrixMvp   ->SetValue<Matrix4x4>(MatrixMVP);
		//	this->RendererMatrixModel ->SetValue<Matrix4x4>(MatrixModel);
		//	this->RendererMatrixNormal->SetValue<Matrix3>(MatrixNormal);
		}

		this->RendererVertexShaderInstance->GetInstanceFirstConstantBuffersLocation()->UploadAllParameters();
		this->GetMaterial()->GetShaderProgram()->GetProgramVertexShader()->BindShader(this->RendererVertexShaderInstance);
		this->GetMaterial()->BindMaterial();
		this->RendererLinkagePoint->RenderSelf();
	}
	
GD_NAMESPACE_END
