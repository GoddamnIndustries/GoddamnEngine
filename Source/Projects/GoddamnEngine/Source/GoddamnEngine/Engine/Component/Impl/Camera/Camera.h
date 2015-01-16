#ifndef GD_COMPONENT_CAMERA
#define GD_COMPONENT_CAMERA

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Vector2.h>
#include <GoddamnEngine/Core/Math/Vector3.h>
#include <GoddamnEngine/Core/Math/Vector4.h>
#include <GoddamnEngine/Core/Math/Matrix4x4.h>
#include <GoddamnEngine/Engine/Component/Component.h>
#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	class Camera;
	class HRIRenderTarget;
	class RenderTexture;
	typedef size_t RenderTargetTexture;

	/// ==========================================================================================
	/// ==========================================================================================
	/// Interface that describes listener to
	///	'OnTransformed' event by 'Transform' objects
	class IOnProjectionMatrixChangedListener
	{
	public:
		/// ==========================================================================================
		/// @brief	Method would be invoked when projection parameters of
		///			corresponding camera would be changed 
		/// @param	param Unused parameter
		GDAPI virtual void OnProjectionMatrixChanged(
			Camera const* const camera
		) abstract;
	};

	/// ==========================================================================================
	/// Structure describes clipping planes of camera
	struct CameraClippingPlanes
	{
		Float32 ZNear = 0.3f;					///< Z Coordinate of near clipping plane
		Float32 ZFar = 1000.0f;					///< Z Coordinate of far clipping plane

		/// ==========================================================================================
		GDINL explicit CameraClippingPlanes() { }
		GDINL  CameraClippingPlanes(Float32 const zNear, Float32 const zFar) : ZNear(zNear), ZFar(zFar) { }
		GDINL  CameraClippingPlanes(CameraClippingPlanes const& clippingPlanes) : ZNear(clippingPlanes.ZNear), ZFar(clippingPlanes.ZFar) { }
		GDINL ~CameraClippingPlanes() { }

		/// ==========================================================================================
		GDINL CameraClippingPlanes& operator= (CameraClippingPlanes const& clippingPlanes)
		{ 
			this->ZNear = clippingPlanes.ZNear;
			this->ZFar = clippingPlanes.ZFar;
			return *this;
		}
	};

	/// ==========================================================================================
	/// ==========================================================================================
	/// Enumeration describing camera projection mode
	enum CameraProjection : UInt8
	{
		GD_CAMERA_PROJECTION_PERSPECTIVE,		///< This camera renders in perspective projection
		GD_CAMERA_PROJECTION_ORTHOGRAPHIC,		///< This camera renders in orthographic projection
	};

	typedef UInt8 CameraClearType;
	/// ==========================================================================================
	/// ==========================================================================================
	/// Enumeration describing camera clearing type
	enum CameraClearTypeEnumration : CameraClearType
	{
		GD_CAMERA_CLEAR_TYPE_COLOR = GD_BIT(0),	///< This camera clears only color component of screen
		GD_CAMERA_CLEAR_TYPE_DEPTH = GD_BIT(1),	///< This camera clears only depth component of screen
		GD_CAMERA_CLEAR_TYPE_ALL   = (0xFF),	///< This camera clears all components of screen
		GD_CAMERA_CLEAR_TYPE_NONE  = (0),		///< This camera does not clear screen before rendering
	};

	/// ==========================================================================================
	/// ==========================================================================================
	/// Camera class
	class Camera final : public  Component,
						 private IOnTransformedListener,
						 private IOnProjectionMatrixChangedListener
	{
	private:
		GD_SERIALIZABLE_DEFINITION(Camera, Component, GDAPI);

		GD_SERIALIZATION_BEGIN(Camera, Component);
			GD_FIELD(CameraClippingPlanes, ClippingPlanes);
			GD_FIELD(CameraProjection, Projection);
			GD_FIELD(CameraClearType, ClearType);
			GD_FIELD(Rectangle, Viewport);
			GD_FIELD(Float32, FieldOfView);
			GD_FIELD(Float32, AspectRatio);
			GD_FIELD(Color, ClearColor);
		GD_SERIALIZATION_END();

		//StaticVector<RenderTexture*, (size_t)/*RenderTargetTexture::Unknown*/1> RenderTextures;
  HRIRenderTarget* RenderTarget;

		Matrix4x4 ProjectionMatrix;
		Matrix4x4 ViewMatrix;

		GDINT Camera();
		GDINL virtual ~Camera() { }

	public:
		EventQueue<IOnProjectionMatrixChangedListener, Camera const*> OnProjectionMatrixChangedEvent;

		/// ==========================================================================================
		/// ==========================================================================================
		/// @name Getters/Setters
		/// @{

		/// ==========================================================================================
		/// Returns projection mode of this camera
		GDINL CameraProjection GetProjection(
		) const
		{ 
			return this->Projection;
		}

		/// ==========================================================================================
		/// Changes projection mode of this camera
		GDINL void SetProjection(
			CameraProjection const projection
		)
		{ 
			this->Projection = projection; 
			this->OnProjectionMatrixChangedEvent.TriggerEvent();
		}

		/// ==========================================================================================
		/// Returns viewport this camera is rendering to
		GDINL Rectangle const& GetViewport(
		) const
		{ 
			return this->Viewport; 
		}

		/// ==========================================================================================
		/// Changes viewport rectangle this camera is rendering to
		GDINL void SetViewport(
			Rectangle const& viewport
		)
		{ 
			this->Viewport = viewport; 
			this->OnProjectionMatrixChangedEvent.TriggerEvent();
		}
		
		/// ==========================================================================================
		/// Returns color this camera is clearing screen to
		GDINL Color const& GetClearColor(
		) const
		{ 
			return this->ClearColor; 
		}

		/// ==========================================================================================
		/// Changes color this camera is clearing screen to
		GDINL void SetClearColor(
			Color const& clearColor
		)
		{ 
			this->ClearColor = clearColor; 
		}
		
		/// ==========================================================================================
		/// Returns clipping planes of this camera
		GDINL CameraClippingPlanes const& GetClippingPlanes() const
		{ 
			return this->ClippingPlanes; 
		}

		/// ==========================================================================================
		/// Changes clipping planes of this camera
		GDINL void SetClippingPlanes(const CameraClippingPlanes& clippingPlanes)
		{ 
			this->ClippingPlanes = clippingPlanes; 
			this->OnProjectionMatrixChangedEvent.TriggerEvent();
		}
		
		/// ==========================================================================================
		/// Returns field of view of this camera
		GDINL float GetFieldOFView() const 
		{ 
			return this->FieldOfView; 
		}

		/// ==========================================================================================
		/// Changes field of view of this camera
		GDINL void SetFieldOFView(
			float const fieldOfView
		)
		{ 
			this->FieldOfView = fieldOfView;  
			this->OnProjectionMatrixChangedEvent.TriggerEvent();
		}

		/// ==========================================================================================
		/// Returns type how camera is clearing the screen
		GDINL CameraClearType GetClearType(
		) const 
		{ 
			return this->ClearType; 
		}

		/// ==========================================================================================
		/// Changes type how camera is clearing the screen
		GDINL void SetClearType(
			CameraClearType const clearType
		) 
		{ 
			this->ClearType = clearType; 
			this->OnProjectionMatrixChangedEvent.TriggerEvent();
		}

		/// ==========================================================================================
		/// Returns projection matrix of this camera
		GDINL Matrix4x4 const& GetProjectionMatrix(
		) const
		{ 
			return this->ProjectionMatrix; 
		}

		/// ==========================================================================================
		/// Returns view matrix of this camera
		GDINL Matrix4x4 const& GetViewMatrix(
		) const 
		{ 
			return this->ViewMatrix; 
		}

		/// @}
		/// ==========================================================================================
		/// ==========================================================================================

		/// ==========================================================================================
		/// ==========================================================================================
		/// @name Render targets management
		/// @{

		GDINL bool IsUsingRenderTarget() const 
		{
   return (this->RenderTarget != nullptr);
		}

		GDAPI void UseRenderTarget(const bool useRenderTarget);

		GDAPI RenderTexture* GetRenderTargetTexture(const RenderTargetTexture renderTargetTexture) const;

		/// @}
		/// ==========================================================================================
		/// ==========================================================================================

	private:
		// Events
		GDAPI virtual void OnInitializeSelf();
		GDAPI virtual void OnUpdateSelf();
		GDAPI virtual void OnDestroySelf();

		// Slots
		GDAPI virtual void OnTransformed(
			Component* const transformer
		);

		GDAPI virtual void OnProjectionMatrixChanged(
			Camera const* const camera
		);
	};

GD_NAMESPACE_END

#endif
