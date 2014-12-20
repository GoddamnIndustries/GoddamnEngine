#include <GoddamnEngine/Engine/Component/Impl/Transform/Transform.h>
#include <GoddamnEngine/Engine/Application/Application.h>
#include <GoddamnEngine/Engine/Component/Static/DeviceConfiguration/DeviceConfiguration.h>
	
GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	// Transform class
	/// ==========================================================================================

	GD_SERIALIZABLE_IMPLEMENTATION(Transform, Component, GDAPI);

	Transform::Transform() :
		GD_EXTENDS_SERIALIZABLE(Component),
		OnTransfromedEvent(&IOnTransformedListener::OnTransformed)
	{
		this->OnTransfromedEvent += this;
		this->lockingFlags = 0;
		this->parent = nullptr;
		this->position = Vector3Fast(0.0f);
		this->rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		this->scale = Vector3Fast(1.0f, 1.0f, 1.0f);
	}

	Transform::~Transform()
	{
	}

	void Transform::LockTransformation(const LockingFlags lockingFlags)
	{
		GD_ASSERT((Application::GetInstance().GetApplicationState() == ApplicationState::Starting),
			"Method 'Transform::LockTransformation' is avaliable only on application initialization state");

		this->lockingFlags |= lockingFlags;
	}

	/// ==========================================================================================
	// Getters / Setters
	/// ==========================================================================================

	void Transform::SetPosition(const Vector3Fast& position)
	{ 
		this->OnTransfromedEvent.TriggerEvent();
		GD_ASSERT((this->lockingFlags & Transform::LockTranslation) == 0,
			"'Transform::SetPosition' error: Translation for this object "
			"was locked by 'Transform::LockTransformation'");

		this->position = position; 
	}

	void Transform::SetGlobalPosition(const Vector3Fast& position)
	{
		Vector3Fast const offset = (position - this->GetGlobalPosition());
		this->Translate(offset);
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{ 
		GD_ASSERT((this->lockingFlags & Transform::LockRotation) == 0,
			"'Transform::SetRotation' error: Rotation for this object "
			"was locked by 'Transform::LockTransformation'");

		this->OnTransfromedEvent.TriggerEvent();
		this->rotation = rotation; 
	}

	void Transform::SetScale(const Vector3Fast& scale)
	{
		/**/ if ((this->lockingFlags & Transform::LockNonUniformScale) != 0)
		{
			GD_ASSERT((GD_COMPARE_FLOATS(scale.x, scale.y) 
				&& GD_COMPARE_FLOATS(scale.y, scale.z) 
				&& GD_COMPARE_FLOATS(scale.z, scale.x)),
				"'Transform::SetScale' error: Non-uniform scaling for this object was "
				"locked by 'Transform::LockTransformation', but 'Transform::SetScale' "
				"was attempted to scale with non-uniform scaling vector");
		}
		else if ((this->lockingFlags & Transform::LockScale) != 0)
		{
			GD_ASSERT(false,
				"'Transform::SetScale' error: Scaling for this object was locked "
				"by 'Transform::LockTransformation'");
		}

		GD_ASSERT(((scale.x > 0) && (scale.y > 0) && (scale.z > 0)),
			"'Transform::SetScale' error: All components of scaling vector should be "
			"positive real values");

		this->OnTransfromedEvent.TriggerEvent();
		this->scale = scale; 
	}

	void Transform::SetParent(Transform* const parent)
	{
		this->OnTransfromedEvent.TriggerEvent();
		GD_ASSERT((this->lockingFlags == Transform::LockNothing),
			"'Transform::SetParent' error: unable to set parent for object with "
			"locked transformation.");

		// Disconnecting with old parent
		if ((this->parent != nullptr) && (this->parent != nullptr))
		{
			this->parent->OnTransfromedEvent -= this;
		}

		// Connecting to new parent
		if ((parent != nullptr) && (parent != nullptr))
		{
			parent->OnTransfromedEvent += this;
		}

		this->parent = parent;
	}

	void Transform::OnInitializeSelf()
	{
		this->SetParent(nullptr);
		this->OnTransfromedEvent.TriggerEvent();
		this->OnTransfromedEvent.LaunchEvent();
	}

	void Transform::OnStartSelf()
	{
		this->OnTransfromedEvent.TriggerEvent();
		this->OnTransfromedEvent.LaunchEvent();
	}

	void Transform::OnDestroySelf()
	{
		this->OnTransfromedEvent -= this;
		if (this->parent != nullptr)
		{
			this->parent->OnTransfromedEvent -= this;
		}
	}

	void Transform::OnUpdateSelf()
	{
		this->OnTransfromedEvent.LaunchEvent();
	}

	void Transform::OnTransformed(Component* const transformer)
	{
		Matrix4x4 const Scaling = Matrix4x4().Scale(this->scale);
		Matrix4x4 const Rotation = Matrix4x4().Rotate(this->rotation);
		Matrix4x4 const Translation = Matrix4x4().Translate(this->position);

		this->transformMatrix = Scaling * Rotation * Translation;
	}

GD_NAMESPACE_END
