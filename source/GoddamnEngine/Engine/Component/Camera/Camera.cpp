#include <GoddamnEngine/Engine/Component/Camera/Camera.h>
#include <GoddamnEngine/Engine/Component/Static/Screen/Screen.h>
#include <GoddamnEngine/Engine/Component/MeshRenderer/MeshRenderer.h>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.h>
#include <GoddamnEngine/Engine/Component/Transform/Transform.h>
#include <GoddamnEngine/Engine/Component/Static/Input/Input.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>

#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Engine/Scene/Scene.h>

#include <GoddamnEngine/Engine/Resource/ResourceStreamer/ResourceStreamer.h>
#include <GoddamnEngine/Engine/Resource/Impl/ShaderProgram/ShaderProgramResource.h>
#include <GoddamnEngine/Engine/Resource/Impl/Texture2D/Texture2DResource.h>

#include <GoddamnEngine/Engine/HUD/HUD.h>
// #include <GoddamnEngine/Engine/Resource/StaticMesh/StaticMeshVertexBufferObject/StaticMeshVertexBufferObject.h>
// #include <GoddamnEngine/Platform/RenderTargetObject.h>

GD_NAMESPACE_BEGIN

	static HRIRenderTarget* RenderTarget = nullptr;
	static HRIIndexedShape* indexedShape = nullptr;
	static HRILinkagePoint* mountingPoint = nullptr;
	static HRIShaderInstance* fxLocation = nullptr;
	static HRITexture2D* RenderTargetColorTexture = nullptr;
	static HRITexture2D* RenderTargetPosTexture = nullptr;
	static HRITexture2D* RenderTargetNormTexture = nullptr;
	static bool enableLights = true;

	static HUDResources* HUD = nullptr;
	static HUDElementPanel* HUDPanel = nullptr;

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
		GD_DEBUG_ASSERT((self->GetParentObject() != nullptr), "Invalid image effect pass specified");
		GD_DEBUG_ASSERT((self->PassShaderProgram != nullptr), "Invalid image shader program specified");

		self->PassRenderTarget = HRInterface::GetInstance().CreateRenderTarget(passRenderTargetInfo);
		self->PassShaderInstance = HRInterface::GetInstance().CreateShaderInstance(self->PassShaderProgram->GetProgramPixelShader()->ShaderDesc);

		HRIShaderParam* const passColorMap = self->PassShaderInstance->GetParamByName("ColorMap");
		if (passColorMap)
		{
			auto const previousImageEffectPass = object_cast<ImageEffectPass const*>(self->GetPreviousSiblingObject());
			passColorMap->SetValue<HRITexture2D const*>(
				(previousImageEffectPass != nullptr)
				? previousImageEffectPass->PassRenderTarget->RenderTargetTextures[0]
				: RenderTargetColorTexture
			);
		}

		HRIShaderParam* const passPositionMap = self->PassShaderInstance->GetParamByName("PositionMap");
		if (passPositionMap)
		{
			passPositionMap->SetValue<HRITexture2D const*>(RenderTargetPosTexture);
		}

		HRIShaderParam* const passNormalMap = self->PassShaderInstance->GetParamByName("FNormalMap");
		if (passNormalMap)
		{
			passNormalMap->SetValue<HRITexture2D const*>(RenderTargetNormTexture);
		}
	}

    static ImageEffectBase* const imageEffect = nullptr;

	/// ==========================================================================================
	ImageEffectPass::~ImageEffectPass(
	)
	{
		self->PassRenderTarget->RemoveReference();
		self->PassShaderInstance->RemoveReference();
		self->PassShaderProgram->RemoveReference();
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
		OnProjectionMatrixChangedEvent(&IOnProjectionMatrixChangedListener::OnProjectionMatrixChanged, self)
	{
		self->ClearColor			= Color(0.00f, 0.00f, 0.56f, 1.00f);
		self->Viewport				= Rectangle(0.0f, 0.0f, 1.0f, 1.0f);
		self->Projection			= GD_CAMERA_PROJECTION_PERSPECTIVE;
		self->ClearType				= GD_CAMERA_CLEAR_TYPE_ALL;
		self->ClippingPlanes		= CameraClippingPlanes(0.3f, 1000.0f);
        self->RenderTarget    		= nullptr;
		self->FieldOfView			= 60.0f;
		self->AspectRatio			= 1.0f;
		self->ProjectionMatrix		= Matrix4x4(1.0f);
		self->ViewMatrix			= Matrix4x4(1.0f);
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
		self->GetGameObject()->GetTransform()->OnTransfromedEvent += self;
		self->OnProjectionMatrixChangedEvent += self;

		self->OnProjectionMatrixChangedEvent.TriggerEvent();
		self->OnProjectionMatrixChangedEvent.LaunchEvent();

		RenderTarget = HRInterface::GetInstance().CreateRenderTarget(
			HRIRenderTargetCtorInfo(
				Resolution(1280l, 720l),
				RenderTargetColorTexture = HRInterface::GetInstance().CreateTexture2D(
					HRITexture2DCtorInfo(
						Resolution(1280l, 720l),
						GD_FORMAT_R8G8B8A8_UINT,
						GD_HRI_TEXTURE_2D_MODE_RenderTarget
					), nullptr, 0
				),
				RenderTargetPosTexture = HRInterface::GetInstance().CreateTexture2D(
					HRITexture2DCtorInfo(
						Resolution(1280l, 720l),
						GD_FORMAT_R16G16B16A16_FLOAT,
						GD_HRI_TEXTURE_2D_MODE_RenderTarget
					), nullptr, 0
				), 
				RenderTargetNormTexture = HRInterface::GetInstance().CreateTexture2D(
					HRITexture2DCtorInfo(
						Resolution(1280l, 720l),
						GD_FORMAT_R16G16B16A16_FLOAT,
						GD_HRI_TEXTURE_2D_MODE_RenderTarget
					), nullptr, 0
				), nullptr
			)
		);
		mountingPoint = HRInterface::GetInstance().CreateLinkagePoint();
		mountingPoint->SetIndexedShape(HRInterface::GetInstance().CreateIndexedShape());
		mountingPoint->SetShaderProgram(ResourceStreamer::GetInstance().LoadImmediately<ShaderProgramResource>("file://test_image_effect.hlsl_desc.xml")->GetProgram());
		{
			static Float32 const ScreenQuadDataVertives[] = {
				-1.0f, -1.0f,  0.0f,	1.0f,  1.0f,  0.0f,		1.0f, -1.0f, 0.0f,
				 1.0f,  1.0f,  0.0f,   -1.0f, -1.0f,  0.0f,	   -1.0f,  1.0f, 0.0f,
			};

			mountingPoint->GetIndexedShape()->SetVertexData(GD_HRI_SEMANTIC_POSITION, ScreenQuadDataVertives, GD_ARRAY_SIZE(ScreenQuadDataVertives));
		}
		{
			UInt16 const screenQuadDataIndices[] = {
				0, 1, 2,
				3, 4, 5
			};

			mountingPoint->GetIndexedShape()->SetIndexData(screenQuadDataIndices, GD_ARRAY_SIZE(screenQuadDataIndices), sizeof(screenQuadDataIndices));
		}

		fxLocation = HRInterface::GetInstance().CreateShaderInstance(mountingPoint->GetShaderProgram()->GetProgramPixelShader()->ShaderDesc);
		fxLocation->GetParamByName("ColorTexture")->SetValue<HRITexture2D const*>(RenderTargetColorTexture);
		fxLocation->GetParamByName("PositionTexture")->SetValue<HRITexture2D const*>(RenderTargetPosTexture);
		fxLocation->GetParamByName("NormalTexture")->SetValue<HRITexture2D const*>(RenderTargetNormTexture);
		fxLocation->GetInstanceResourcesLocation()->UploadAllParameters();

		/*HUD = new HUDResources();
		HUDPanel = new HUDElementPanel(nullptr, 
			new HUDTextureAtlas(ResourceStreamer::GetInstance().LoadImmediately<HRITexture2DResource>("file://CommonResources/Textures/HUDButton.tif")->GetHRITexture2D(), UInt32x2(1, 1), Vector2(0.0f)),
			new HUDCharactersAtlas(ResourceStreamer::GetInstance().LoadImmediately<HRITexture2DResource>("file://CommonResources/Textures/HUDFontAtlas.tif")->GetHRITexture2D(), UInt32x2(16, 16), 1.0f)
		);

		HUDPanel->SetElementRectangle(Rectangle(0.5f, 0.5f, 0.1f, 0.1f * 1280.0f / 720.0f));
		HUDPanel->SetElementRotationDegrees(30.0f);*/
	}

	/// ==========================================================================================
	void Camera::OnDestroySelf()
	{
		self->GetGameObject()->GetTransform()->OnTransfromedEvent -= self;
		self->OnProjectionMatrixChangedEvent -= self;
	}

	void Camera::OnUpdateSelf()
	{
		if (Input::GetInstance().IsKeyUp(KeyCode::L)) enableLights = !enableLights;

		if (enableLights)
		{
			RenderTarget->BindRenderTarget();
		}
		HRInterface::GetInstance().ClearContext(Rectangle(), Color(1.0f, 0.0f, 1.0f, 1.0f));
		Scene::GetInstance().OnRender(self);

		if (enableLights)
		{
			RenderTarget->UnbindRenderTarget();
			HRInterface::GetInstance().ClearContext(Rectangle(0, 0, 1280, 720), Color(1.0f, 0.0f, 1.0f, 1.0f));
			//fxLocation->GetParamByName("CameraPosition")->SetValue<Vector3>(self->GetGameObject()->GetTransform()->GetPosition());
			mountingPoint->GetShaderProgram()->GetProgramVertexShader()->BindShader(nullptr);
			mountingPoint->GetShaderProgram()->GetProgramPixelShader()->BindShader(fxLocation);
			mountingPoint->RenderSelf();
		}

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
		Transform const* const   transform = self->GetGameObject()->GetTransform();
		Vector3Fast const upVec = transform->RotateVector(Vector3Fast(0.0f, 1.0f, 0.0f)).Normalize();
		Vector3Fast const lookAt = transform->TransformVector(Vector3Fast(0.0, 0.0, 1.0f));
		Vector3Fast const position = transform->GetGlobalPosition();
		
		self->ViewMatrix = Matrix4x4(1.0f).LookAtLh(position, lookAt, upVec);
	}

	/// ==========================================================================================
	void Camera::OnProjectionMatrixChanged(
		_In_ Camera const* const camera 
	)
	{
		switch (self->Projection)
		{
		case GD_CAMERA_PROJECTION_PERSPECTIVE:
			{
				Float32 const AspectRatio = ((self->Viewport.Width - self->Viewport.Left) * static_cast<Float32>(Screen::GetResolutionWidth()))
										  / ((self->Viewport.Height - self->Viewport.Top) * static_cast<Float32>(Screen::GetResolutionHeight()));
			
				self->ProjectionMatrix = Matrix4x4(1.0f).PerspectiveLh(
					self->FieldOfView, AspectRatio,
					self->ClippingPlanes.ZNear, self->ClippingPlanes.ZFar
				);
			}	break;
		case GD_CAMERA_PROJECTION_ORTHOGRAPHIC:
			{
				self->ProjectionMatrix = Matrix4x4(1.0f).OrthoLh(
					self->Viewport.Left, self->Viewport.Left + self->Viewport.Width, 
					self->Viewport.Top + self->Viewport.Height, self->Viewport.Top, 
					self->ClippingPlanes.ZNear, self->ClippingPlanes.ZFar
				);
			}	break;
		default: GD_ASSERT_FALSE("Invalid projection mode specified"); break;
		}
	}

GD_NAMESPACE_END
