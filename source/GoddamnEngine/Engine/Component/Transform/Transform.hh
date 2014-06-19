#ifndef GD_TRANSFORM
#define GD_TRANSFORM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Engine/Component/Component.hh>
#include <GoddamnEngine/Engine/Component/GameObject/GameObject.hh>

#include <GoddamnEngine/Core/Math/Math.hh>
#include <GoddamnEngine/Core/Math/Vector3Fast.hh>
#include <GoddamnEngine/Core/Math/Vector4.hh>
#include <GoddamnEngine/Core/Math/Matrix4x4.hh>
#include <GoddamnEngine/Core/Math/Quaternion.hh>
#include <GoddamnEngine/Core/Containers/Pointer/RefPtr.hh>

#define GD_COMPARE_FLOATS(a, b) (fabs(a - b) < (1e-6f)) 

GD_NAMESPACE_BEGIN

	enum class Space : byte
	{
		Self,
		World
	};
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// @brief		Interface that describes listener to
	///				'OnTransformed' event by 'Transform' objects
	class IOnTransformedListener
	{
	public:
		/// @brief	Method would be invoked when corresponding 
		///			event-object would be transformed
		/// @param	param Unused parameter
		GDAPI virtual void OnTransformed(Component* const transformer) abstract;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// @todo add description here
	class Transform final : public Component,
							private IOnTransformedListener
	{
		typedef byte LockingFlags;

	private:
		GD_SERIALIZABLE_DEFINITION(Transform, Component, GDAPI);

		GD_SERIALIZATION_BEGIN(Transform, Component);
			GD_FGDLD(Quaternion     	,	rotation);
			GD_FGDLD(Vector3Fast	,	position);
			GD_FGDLD(Vector3Fast ,   scale);
			GD_FGDLD(Transform*	        ,	parent);
		GD_SERIALIZATION_END();

		LockingFlags lockingFlags;

		Matrix4x4 transformMatrix;
		Matrix3 normalMatrix;

		GDAPI Transform();
		GDAPI virtual ~Transform();

	public:
		EventQueue<IOnTransformedListener, Component*> OnTransfromedEvent;

		/// @name Transformation locking
		/// @{

		enum : LockingFlags
		{
			LockNothing,									
			LockTranslation			= GD_BIT(0),									///< This transform is not movable
			LockRotation			= GD_BIT(1),									///< This transform is not rotatable
			LockNonUniformScale		= GD_BIT(2),									///< This transform is not uniformly scalable
			LockScale				= GD_BIT(3) | Transform::LockNonUniformScale	///< This transform is not scalable
		};

		/// @brief			Locks some ways object may be transformed
		/// @param flags	Flags describing what ways of transformation should be locked
		/// @note			This method is available only on component`s initialization state
		GDAPI void LockTransformation(const LockingFlags flags);

		/// @}

		/// @name Getters/Setters
		/// @{

		/// @brief			Returns position of this game object
		/// @returns		Position of this game object
		/// @note			Parent object is treated as origin
		GDINL const Vector3Fast& GetPosition() const
		{ 
			return self->position; 
		}

		/// @brief			Changes position of this game object
		/// @param position	New position of game object
		/// @note			Parent object is treated as origin
		GDAPI void SetPosition(const Vector3Fast& position);

		/// @brief			Returns global position of this game object
		/// @returns		Global position of this game object
		GDINL Vector3Fast GetGlobalPosition() const
		{ 
			return self->TransformVector(Vector3Fast(0.0f));
		}

		/// @brief			Changes global position of this game object
		/// @param position	New global position of this game object
		/// @note			This function moves only current game object and not parent ones
		GDAPI void SetGlobalPosition(const Vector3Fast& position);

		/// @brief			Returns rotation of this game object in quaternions
		/// @returns		Rotation of this game object in quaternions
		GDINL const Quaternion&	GetRotation() const 
		{ 
			return self->rotation; 
		}

		/// @brief			Changes rotation of this game object in quaternions
		/// @param rotation	New rotation of this game object in quaternions
		GDAPI void SetRotation(const Quaternion& rotation);

		/// @brief			Changes rotation of this game object in degrees
		/// @param rotation	New rotation of this game object in degrees
		GDAPI void SetRotation(const Vector3Fast& degrees)
		{ 
			self->rotation.SetEulerDegrees(degrees); 
			self->OnTransfromedEvent.TriggerEvent();
		}

		/// @brief			Returns	scale of this game object
		/// @returns		Scale of this game object
		GDINL const Vector3Fast& GetScale() const
		{ 
			return self->scale; 
		}

		/// @brief			Rescales this game object
		/// @param scale	New scale of game object 
		GDAPI void SetScale(const Vector3Fast& scale);

		/// @brief			Returns pointer to transform of parent game object
		/// @returns		Pointer to transform of parent game object
		GDINL Transform* GetParent() const
		{ 
			return self->parent; 
		}

		/// @brief			Detaches this game object from it`s parent and attaches
		///					to other one.
		/// @param transform Pointer to transform of new parent
		GDAPI void SetParent(Transform* const parent);

		/// @brief			Returns left-handed transformation matrix of this game object
		/// @returns		Left-handed transformation matrix of this game object
		GDINL const Matrix4x4& GetTransformMatrix() const 
		{ 
			return self->transformMatrix; 
		}

		/// @}

		/// @brief					Translates game object
		/// @param delatPosition	Delta position on which we are going to transform our game object
		/// @param space			Space in which we are going to translate : world or self
		GDINL void Translate(const Vector3Fast& deltaPosition, const Space space = Space::Self)
		{
			self->SetPosition(self->GetPosition() + ((space == Space::Self) 
				? self->GetRotation() * deltaPosition
				: deltaPosition
			)); 
		}
		
		/// @brief					Rotates game object
		/// @param deltaDegrees		Delta rotation on which we are going to transform our game object
		GDINL void Rotate(const Vector3Fast& deltaDegrees)
		{ 
			self->SetRotation(self->GetRotation() * Quaternion().SetEulerDegrees(deltaDegrees)); 
		}
		
		/// @brief			Transforms vector to this game object`s transformation
		/// @param vector	Vector to transform
		GDAPI Vector3Fast TransformVector(const Vector3Fast& vector) const
		{
			const Vector4 result = (self->GetTransformMatrix() * Vector4(vector, 1.0f));
			return Vector3Fast(result.x, result.y, result.z);
		}

		/// @brief			Rotates vector to this game object`s transformation
		/// @param vector	Vector to rotate
		GDAPI Vector3Fast RotateVector(const Vector3Fast& vector) const
		{
			return self->TransformVector(vector) - self->TransformVector(Vector3Fast(0.0f));
		}

	private:

		GDAPI virtual void OnInitializeSelf();
		GDAPI virtual void OnStartSelf();
		GDAPI virtual void OnDestroySelf();
		GDAPI virtual void OnUpdateSelf();

		GDAPI virtual void OnTransformed(Component* const transformer);
	};

GD_NAMESPACE_END

#endif