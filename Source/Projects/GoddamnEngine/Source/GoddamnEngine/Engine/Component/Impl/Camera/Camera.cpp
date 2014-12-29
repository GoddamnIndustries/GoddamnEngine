#include <GoddamnEngine/Engine/Component/Impl/Camera/Camera.h>
#include <GoddamnEngine/Engine/Component/Static/Screen/Screen.h>
#include <GoddamnEngine/Engine/Component/Impl/MeshRenderer/MeshRenderer.h>
#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>
#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>

#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Scene/Scene.h>

#include <GoddamnEngine/Engine/Resource/Impl/Effect/Effect.h>
#include <GoddamnEngine/Engine/Resource/Impl/Texture2D/Texture2DResource.h>

#include <GoddamnEngine/Engine/HUD/HUD.h>
// #include <GoddamnEngine/Engine/Resource/StaticMesh/StaticMeshVertexBufferObject/StaticMeshVertexBufferObject.h>
// #include <GoddamnEngine/Platform/RenderTargetObject.h>

#include <DirectXMath.h>

GD_NAMESPACE_BEGIN

	//static HRIRenderTarget* RenderTarget = nullptr;
	//static HRIIndexedShape* indexedShape = nullptr;
	//static HRILinkagePoint* mountingPoint = nullptr;
	//static HRIShaderInstance* fxLocation = nullptr;
	//static HRITexture2D* RenderTargetColorTexture = nullptr;
	//static HRITexture2D* RenderTargetPosTexture = nullptr;
	//static HRITexture2D* RenderTargetNormTexture = nullptr;
	//static bool enableLights = true;

	//static HUDResources* HUD = nullptr;
	//static HUDElementPanel* HUDPanel = nullptr;

	/// ==========================================================================================
	/// ImageEffectBase class
	/// ==========================================================================================

	class ImageEffectBase;
	class ImageEffectPass;

	/// ==========================================================================================
	class ImageEffectPass : public HRIObject
	{
	public:
		HRIRenderTarget* PassRenderTarget = nullptr;
		HRIShaderProgram* const PassShaderProgram = nullptr;
		HRIShaderInstance* PassShaderInstance = nullptr;
		HRITexture2D* PassOutputTexture = nullptr;

		/// ==========================================================================================
		GDINL ImageEffectPass(
			_In_ ImageEffectBase* const passImageEffect,
			_In_ HRIRenderTargetCtorInfo const& passRenderTargetInfo,
			_In_ HRIShaderProgram* const passShaderProgram
		);

		/// ==========================================================================================
		GDINL virtual ~ImageEffectPass(
		);
	};

	/// ==========================================================================================
	class ImageEffectBase : public HRIObject
	{
	};

	/// ==========================================================================================
	ImageEffectPass::ImageEffectPass(
		_In_ ImageEffectBase* const passImageEffect,
		_In_ HRIRenderTargetCtorInfo const& passRenderTargetInfo,
		_In_ HRIShaderProgram* const passShaderProgram
	) : HRIObject(HRIObject::TreeLockingFlagsAll, passImageEffect),
		PassShaderProgram(passShaderProgram)
	{
	/*	GD_DEBUG_ASSERT((this->GetParentObject() != nullptr), "Invalid image effect pass specified");
		GD_DEBUG_ASSERT((this->PassShaderProgram != nullptr), "Invalid image shader program specified");

		this->PassRenderTarget = HRInterface::GetInstance().CreateRenderTarget(passRenderTargetInfo);
		this->PassShaderInstance = HRInterface::GetInstance().CreateShaderInstance(this->PassShaderProgram->GetProgramPixelShader()->ShaderDesc);

		HRIShaderParam* const passColorMap = this->PassShaderInstance->GetParamByName("ColorMap");
		if (passColorMap)
		{
			auto const previousImageEffectPass = object_cast<ImageEffectPass const*>(this->GetPreviousSiblingObject());
			passColorMap->SetValue<HRITexture2D const*>(
				(previousImageEffectPass != nullptr)
				? previousImageEffectPass->PassRenderTarget->RenderTargetTextures[0]
				: RenderTargetColorTexture
			);
		}

		HRIShaderParam* const passPositionMap = this->PassShaderInstance->GetParamByName("PositionMap");
		if (passPositionMap)
		{
			passPositionMap->SetValue<HRITexture2D const*>(RenderTargetPosTexture);
		}

		HRIShaderParam* const passNormalMap = this->PassShaderInstance->GetParamByName("FNormalMap");
		if (passNormalMap)
		{
			passNormalMap->SetValue<HRITexture2D const*>(RenderTargetNormTexture);
		}*/
	}

    static ImageEffectBase* const imageEffect = nullptr;

	/// ==========================================================================================
	ImageEffectPass::~ImageEffectPass(
	)
	{
		this->PassRenderTarget->RemoveReference();
		this->PassShaderInstance->RemoveReference();
		this->PassShaderProgram->RemoveReference();
	}

	/// ==========================================================================================
	/// Depth of field class
	/// ==========================================================================================

	/// ==========================================================================================
	// Camera class
	/// ==========================================================================================

	/// ==========================================================================================
	GD_SERIALIZABLE_IMPLEMENTATION(Camera, Component, GDAPI);

	/// ==========================================================================================
	Camera::Camera() :
		GD_EXTENDS_SERIALIZABLE(Component),
		OnProjectionMatrixChangedEvent(&IOnProjectionMatrixChangedListener::OnProjectionMatrixChanged, this)
	{
		this->ClearColor			= Color(0.00f, 0.00f, 0.56f, 1.00f);
		this->Viewport				= Rectangle(0.0f, 0.0f, 1.0f, 1.0f);
		this->Projection			= GD_CAMERA_PROJECTION_PERSPECTIVE;
		this->ClearType				= GD_CAMERA_CLEAR_TYPE_ALL;
		this->ClippingPlanes		= CameraClippingPlanes(0.3f, 1000.0f);
        this->RenderTarget    		= nullptr;
		this->FieldOfView			= 60.0f;
		this->AspectRatio			= 1.0f;
		this->ProjectionMatrix		= Matrix4x4(1.0f);
		this->ViewMatrix			= Matrix4x4(1.0f);
	}

	/// ==========================================================================================
	// Render targets management
	/// ==========================================================================================

	/// ==========================================================================================
	void Camera::UseRenderTarget(const bool useRenderTarget)
	{
	}

	/// ==========================================================================================
	// Messaging functions
	/// ==========================================================================================

	/// ==========================================================================================
	void Camera::OnInitializeSelf()
	{
		this->GetGameObject()->GetTransform()->OnTransfromedEvent += this;
		this->OnProjectionMatrixChangedEvent += this;

		this->OnProjectionMatrixChangedEvent.TriggerEvent();
		this->OnProjectionMatrixChangedEvent.LaunchEvent();

		//RenderTarget = HRInterface::GetInstance().CreateRenderTarget(
		//	HRIRenderTargetCtorInfo(
		//		Resolution(1280l, 720l),
		//		RenderTargetColorTexture = HRInterface::GetInstance().CreateTexture2D(
		//			HRITexture2DCtorInfo(
		//				Resolution(1280l, 720l),
		//				GD_FORMAT_R8G8B8A8_UINT,
		//				GD_HRI_TEXTURE_2D_MODE_RenderTarget
		//			), nullptr, 0
		//		),
		//		RenderTargetPosTexture = HRInterface::GetInstance().CreateTexture2D(
		//			HRITexture2DCtorInfo(
		//				Resolution(1280l, 720l),
		//				GD_FORMAT_R16G16B16A16_FLOAT,
		//				GD_HRI_TEXTURE_2D_MODE_RenderTarget
		//			), nullptr, 0
		//		), 
		//		RenderTargetNormTexture = HRInterface::GetInstance().CreateTexture2D(
		//			HRITexture2DCtorInfo(
		//				Resolution(1280l, 720l),
		//				GD_FORMAT_R16G16B16A16_FLOAT,
		//				GD_HRI_TEXTURE_2D_MODE_RenderTarget
		//			), nullptr, 0
		//		), nullptr
		//	)
		//);
		//mountingPoint = HRInterface::GetInstance().CreateLinkagePoint();
		//mountingPoint->SetIndexedShape(HRInterface::GetInstance().CreateIndexedShape());
		//mountingPoint->SetShaderProgram(RSStreamer::GetInstance().LoadImmediately<RSEffect>("file://test_image_effect.hlsl_desc.xml")->GetEffectShaderProgram().GetPointer());
		//{
		//	static Float32 const ScreenQuadDataVertives[] = {
		//		-1.0f, -1.0f,  0.0f,	1.0f,  1.0f,  0.0f,		1.0f, -1.0f, 0.0f,
		//		 1.0f,  1.0f,  0.0f,   -1.0f, -1.0f,  0.0f,	   -1.0f,  1.0f, 0.0f,
		//	};

		//	mountingPoint->GetIndexedShape()->SetVertexData(GD_HRI_SEMANTIC_POSITION, ScreenQuadDataVertives, GD_ARRAY_SIZE(ScreenQuadDataVertives));
		//}
		//{
		//	UInt16 const screenQuadDataIndices[] = {
		//		0, 1, 2,
		//		3, 4, 5
		//	};

		//	mountingPoint->GetIndexedShape()->SetIndexData(screenQuadDataIndices, GD_ARRAY_SIZE(screenQuadDataIndices), sizeof(screenQuadDataIndices));
		//}

		//fxLocation = HRInterface::GetInstance().CreateShaderInstance(mountingPoint->GetShaderProgram()->GetProgramPixelShader()->ShaderDesc);
		//fxLocation->GetParamByName("ColorTexture")->SetValue<HRITexture2D const*>(RenderTargetColorTexture);
		//fxLocation->GetParamByName("PositionTexture")->SetValue<HRITexture2D const*>(RenderTargetPosTexture);
		//fxLocation->GetParamByName("NormalTexture")->SetValue<HRITexture2D const*>(RenderTargetNormTexture);
		//fxLocation->GetInstanceResourcesLocation()->UploadAllParameters();

		/*HUD = new HUDResources();
		HUDPanel = new HUDElementPanel(nullptr, 
			new HUDTextureAtlas(RSStreamer::GetInstance().LoadImmediately<HRITexture2DResource>("file://CommonResources/Textures/HUDButton.tif")->GetHRITexture2D(), UInt32x2(1, 1), Vector2(0.0f)),
			new HUDCharactersAtlas(RSStreamer::GetInstance().LoadImmediately<HRITexture2DResource>("file://CommonResources/Textures/HUDFontAtlas.tif")->GetHRITexture2D(), UInt32x2(16, 16), 1.0f)
		);

		HUDPanel->SetElementRectangle(Rectangle(0.5f, 0.5f, 0.1f, 0.1f * 1280.0f / 720.0f));
		HUDPanel->SetElementRotationDegrees(30.0f);*/
	}

	/// ==========================================================================================
	void Camera::OnDestroySelf()
	{
		this->GetGameObject()->GetTransform()->OnTransfromedEvent -= this;
		this->OnProjectionMatrixChangedEvent -= this;
	}

	void Camera::OnUpdateSelf()
	{
		/*if (Input::GetInstance().IsKeyUp(KeyCode::L)) enableLights = !enableLights;

		if (enableLights)
		{
			RenderTarget->BindRenderTarget();
		}*/
		HRInterface::GetInstance().ClearContext(Rectangle(0.0f, 0.0f, 1.0f, 1.0f), Color(1.0f, 0.0f, 1.0f, 1.0f));
		Scene::GetInstance().OnRenderSelf(this);

		//if (enableLights)
		//{
		//	RenderTarget->UnbindRenderTarget();
		//	HRInterface::GetInstance().ClearContext(Rectangle(0, 0, 1280, 720), Color(1.0f, 0.0f, 1.0f, 1.0f));
		//	//fxLocation->GetParamByName("CameraPosition")->SetValue<Vector3>(this->GetGameObject()->GetTransform()->GetPosition());
		//	mountingPoint->GetShaderProgram()->GetProgramVertexShader()->BindShader(nullptr);
		//	mountingPoint->GetShaderProgram()->GetProgramPixelShader()->BindShader(fxLocation);
		//	mountingPoint->RenderSelf();
		//}

		//nterface::GetInstance().ClearContext(Rectangle(0, 0, 1280, 720), Color(1.0f, 0.0f, 1.0f, 1.0f));
		//HUD->RenderPanel(HUDPanel);

		HRInterface::GetInstance().SwapBuffers();
	}

	/// ==========================================================================================
	// Signals / Slots
	/// ==========================================================================================

	/// ==========================================================================================
	void Camera::OnTransformed(
		_In_ Component* const transformer
	)
	{
		RefPtr<Transform const> const  TheTransform(this->GetGameObject()->GetTransform());
		Vector3Fast const UpVec = TheTransform->RotateVector(Vector3Fast(0.0f, 1.0f, 0.0f)).Normalize();
		Vector3Fast const LookAt = TheTransform->TransformVector(Vector3Fast(0.0, 0.0, 1.0f));
		Vector3Fast const Position = TheTransform->GetGlobalPosition();
		Matrix4x4::MakeLookAtLH(this->ViewMatrix, Position, LookAt, UpVec);
	//	using namespace DirectX;
	//	XMMATRIX m = XMMatrixLookAtLH(position.ElementsVector, lookAt.ElementsVector, upVec.ElementsVector);
	}

	/// ==========================================================================================
	void Camera::OnProjectionMatrixChanged(
		_In_ Camera const* const camera 
	)
	{
		switch (this->Projection)
		{
		case GD_CAMERA_PROJECTION_PERSPECTIVE:
			{
				Float32 const AspectRatio = ((this->Viewport.Width - this->Viewport.Left) * static_cast<Float32>(Screen::GetResolutionWidth()))
										  / ((this->Viewport.Height - this->Viewport.Top) * static_cast<Float32>(Screen::GetResolutionHeight()));
				this->ProjectionMatrix = Matrix4x4(1.0f).PerspectiveLH(
					this->FieldOfView, AspectRatio,
					this->ClippingPlanes.ZNear, this->ClippingPlanes.ZFar
				);

			//	using namespace DirectX;
			//	XMMATRIX m = XMMatrixPerspectiveFovLH(this->FieldOfView / 57.295779513082320876798154814105f, AspectRatio, this->ClippingPlanes.ZNear, this->ClippingPlanes.ZFar);
			}	break;
		case GD_CAMERA_PROJECTION_ORTHOGRAPHIC:
			{
				this->ProjectionMatrix = Matrix4x4(1.0f).OrthoLH(
					this->Viewport.Left, this->Viewport.Left + this->Viewport.Width, 
					this->Viewport.Top + this->Viewport.Height, this->Viewport.Top, 
					this->ClippingPlanes.ZNear, this->ClippingPlanes.ZFar
				);
			}	break;
		default: GD_DEBUG_ASSERT_FALSE("Invalid projection mode specified"); break;
		}
	}

GD_NAMESPACE_END