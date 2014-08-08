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
		self->OnTransfromedEvent += self;
		self->lockingFlags = 0;
		self->parent = nullptr;
		self->position = Vector3Fast(0.0f);
		self->rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		self->scale = Vector3Fast(1.0f, 1.0f, 1.0f);
	}

	Transform::~Transform()
	{
	}

	void Transform::LockTransformation(const LockingFlags lockingFlags)
	{
		GD_ASSERT((Application::GetInstance().GetApplicationState() == ApplicationState::Starting),
			"Method 'Transform::LockTransformation' is avaliable only on application initialization state");

		self->lockingFlags |= lockingFlags;
	}

	/// ==========================================================================================
	// Getters / Setters
	/// ==========================================================================================

	void Transform::SetPosition(const Vector3Fast& position)
	{ 
		self->OnTransfromedEvent.TriggerEvent();
		GD_ASSERT((self->lockingFlags & Transform::LockTranslation) == 0,
			"'Transform::SetPosition' error: Translation for this object "
			"was locked by 'Transform::LockTransformation'");

		self->position = position; 
	}

	void Transform::SetGlobalPosition(const Vector3Fast& position)
	{
		Vector3Fast const offset = (position - self->GetGlobalPosition());
		self->Translate(offset);
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{ 
		GD_ASSERT((self->lockingFlags & Transform::LockRotation) == 0,
			"'Transform::SetRotation' error: Rotation for this object "
			"was locked by 'Transform::LockTransformation'");

		self->OnTransfromedEvent.TriggerEvent();
		self->rotation = rotation; 
	}

	void Transform::SetScale(const Vector3Fast& scale)
	{
		/**/ if ((self->lockingFlags & Transform::LockNonUniformScale) != 0)
		{
			GD_ASSERT((GD_COMPARE_FLOATS(scale.x, scale.y) 
				&& GD_COMPARE_FLOATS(scale.y, scale.z) 
				&& GD_COMPARE_FLOATS(scale.z, scale.x)),
				"'Transform::SetScale' error: Non-uniform scaling for this object was "
				"locked by 'Transform::LockTransformation', but 'Transform::SetScale' "
				"was attempted to scale with non-uniform scaling vector");
		}
		else if ((self->lockingFlags & Transform::LockScale) != 0)
		{
			GD_ASSERT(false,
				"'Transform::SetScale' error: Scaling for this object was locked "
				"by 'Transform::LockTransformation'");
		}

		GD_ASSERT(((scale.x > 0) && (scale.y > 0) && (scale.z > 0)),
			"'Transform::SetScale' error: All components of scaling vector should be "
			"positive real values");

		self->OnTransfromedEvent.TriggerEvent();
		self->scale = scale; 
	}

	void Transform::SetParent(Transform* const parent)
	{
		self->OnTransfromedEvent.TriggerEvent();
		GD_ASSERT((self->lockingFlags == Transform::LockNothing),
			"'Transform::SetParent' error: unable to set parent for object with "
			"locked transformation.");

		// Disconnecting with old parent
		if ((self->parent != nullptr) && (self->parent != nullptr))
		{
			self->parent->OnTransfromedEvent -= self;
		}

		// Connecting to new parent
		if ((parent != nullptr) && (parent != nullptr))
		{
			parent->OnTransfromedEvent += self;
		}

		self->parent = parent;
	}

	void Transform::OnInitializeSelf()
	{
		self->SetParent(nullptr);
		self->OnTransfromedEvent.TriggerEvent();
		self->OnTransfromedEvent.LaunchEvent();
	}

	void Transform::OnStartSelf()
	{
		self->OnTransfromedEvent.TriggerEvent();
		self->OnTransfromedEvent.LaunchEvent();
	}

	void Transform::OnDestroySelf()
	{
		self->OnTransfromedEvent -= self;
		if (self->parent != nullptr)
		{
			self->parent->OnTransfromedEvent -= self;
		}
	}

	void Transform::OnUpdateSelf()
	{
		self->OnTransfromedEvent.LaunchEvent();
	}

	void Transform::OnTransformed(Component* const transformer)
	{
		Matrix4x4 const Scaling = Matrix4x4().Scale(self->scale);
		Matrix4x4 const Rotation = Matrix4x4().Rotate(self->rotation);
		Matrix4x4 const Translation = Matrix4x4().Translate(self->position);

		self->transformMatrix = Scaling * Rotation * Translation;
	}

GD_NAMESPACE_END
