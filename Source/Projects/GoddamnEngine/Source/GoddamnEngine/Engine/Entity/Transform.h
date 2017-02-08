// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Entity/Transform.h
 * File contains Transform component class.
 */
#pragma once

#include <GoddamnEngine/Engine/Entity/Component.h>
#include <GoddamnEngine/Core/Math/Vector.h>
#include <GoddamnEngine/Core/Math/Matrix.h>
#include <GoddamnEngine/Core/Math/Quaternion.h>

GD_NAMESPACE_BEGIN

	GD_ENTITY_HELPER enum class Space : UInt32
	{
		Local,
		World,
		Count
	};	// enum class Space

	// **------------------------------------------------------------------------------------------**
	//! Declares a component that handles transformations (position, rotation and scale) of the
	//! entity.
	// **------------------------------------------------------------------------------------------**
	GD_ENTITY_KERNEL class Transform final : public Component
	{
	private:
		GD_DECLARE_OBJECT_INTRINSIC_ENGINE(Transform, Component);

		GD_PROPERTY(private, RefPtr<Transform>, m_Parent);
		GD_PROPERTY(private, Vector<RefPtr<Transform>>, m_Child);

		GD_PROPERTY(private, Vector4,     m_LocalPosition);
		GD_PROPERTY(private, Quaternion,  m_LocalRotation);
		GD_PROPERTY(private, Vector3Fast, m_LocalScale) = {1.0f, 1.0f, 1.0f};

	public:

		GDINL void SetParent(RefPtr<Transform> const p)
		{
			m_Parent = p;
		}

		// ------------------------------------------------------------------------------------------
		// Local transformations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns a relative position to the parent's transform.
		 */
		GDINL Vector4 const& GetLocalPosition() const
		{
			return m_LocalPosition;
		}
		/*!
		 * Sets a new relative position to the parent's transform.
		 * @param localPosition New local position.
		 */
		GDINL void SetLocalPosition(Vector4 const& localPosition)
		{
			m_LocalPosition = localPosition;
		}

		/*!
		 * Returns a relative rotation to the parent's transform.
		 */
		GDINL Quaternion const& GetLocalRotation() const
		{
			return m_LocalRotation;
		}
		/*!
		 * Sets a new relative rotation to the parent's transform.
		 */
		GDINL void SetLocalRotation(Quaternion const& localRotation)
		{
			m_LocalRotation = localRotation;
		}

		/*!
		 * Returns a relative rotation to the parent's transform (in Euler angles, degrees).
		 */
		GDINL Vector4 GetLocalEulerAngles() const
		{
			return m_LocalRotation.ToEulerDegrees();
		}
		/*!
		 * Sets a new relative rotation to the parent's transform.
		 */
		GDINL void SetLocalEulerAngles(Vector4 const& localEulerAngles)
		{
			m_LocalRotation = Quaternion::FromEulerDegrees(localEulerAngles);
		}

		/*!
		 * Returns a relative scale to the parent's transform.
		 */
		GDINL Vector3Fast const& GetLocalScale() const
		{
			return m_LocalScale;
		}
		/*!
		 * Sets a new relative scale to the parent's transform.
		 */
		GDINL void SetLocalScale(Vector3Fast const& localScale)
		{
			m_LocalScale = localScale;
		}

		// ------------------------------------------------------------------------------------------
		// Global transformations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Returns a matrix that transforms a point from local to world coordinates.
		 */
		GDINL Matrix4x4 GetLocalToWorldMatrix() const
		{
			auto const selfToWorld = Matrix4x4::TranslateRotateScale(m_LocalPosition, m_LocalRotation, m_LocalScale);
			return m_Parent == nullptr ? selfToWorld : m_Parent->GetLocalToWorldMatrix() * selfToWorld;
		}
		/*!
		 * Returns a matrix that transforms a point from world to local coordinates.
		 */
		GDINL Matrix4x4 GetWorldToLocalMatrix() const
		{
			return GetLocalToWorldMatrix().Inverse();
		}

		/*!
		 * Returns a position of a transform in a world space.
		 */
		GDINL Vector4 GetPosition() const
		{
			return m_Parent == nullptr ? m_LocalPosition : m_Parent->GetLocalToWorldMatrix() * m_LocalPosition;
		}
		/*!
		 * Sets a new position of a transform in a world space.
		 */
		GDINL void SetPosition(Vector4 const& position)
		{
			m_LocalPosition = m_Parent == nullptr ? position : m_Parent->GetWorldToLocalMatrix() * position;
		}

		/*!
		 * Returns a rotation of a transform in a world space.
		 */
		GDINL Quaternion GetRotation() const
		{
			return m_Parent == nullptr ? m_LocalRotation : m_Parent->GetRotation() * m_LocalRotation;
		}
		/*!
		 * Sets a new rotation of a transform in a world space.
		 * @param rotation New global rotation.
		 */
		GDINL void SetRotation(Quaternion const& rotation)
		{
			m_LocalRotation = m_Parent == nullptr ? rotation : m_Parent->GetRotation().Inverse() * rotation;
		}

		/*!
		 * Returns a rotation of a transform in a world space (in Euler angles, degrees).
		 */
		GDAPI Vector4 GetEulerAngles() const
		{
			return GetRotation().ToEulerDegrees();
		}
		/*!
		 * Sets a new rotation of a transform in a world space.
		 * @param eulerAngles New global rotation (in Euler angles, degrees).
		 */
		GDINL void SetEulerAngles(Vector4 const& eulerAngles)
		{
			SetRotation(Quaternion::FromEulerDegrees(eulerAngles));
		}

		/*!
		 * Returns a global scale of a transform.
		 */
		GDINL Vector3Fast GetScale() const
		{
			return m_Parent == nullptr ? m_LocalScale : m_Parent->GetScale() * m_LocalScale;
		}

		// ------------------------------------------------------------------------------------------
		// Delta transformations.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Transforms a direction from local space to global.
		 * The operation is not affected by scale or position of the transform.
		 * 
		 * @param direction The direction to transform.
		 * @returns Transformed direction.
		 */
		GDINL Vector4 TransformDirection(Vector4 const& direction) const
		{
			return Matrix4x4::Rotate(GetRotation()) * direction;
		}
		/*!
		 * Transforms a direction from local space to local.
		 * The operation is not affected by scale or position of the transform.
		 * 
		 * @param direction The direction to transform.
		 * @returns Transformed direction.
		 */
		GDINL Vector4 InverseTransformDirection(Vector4 const& direction) const
		{
			return Matrix4x4::Rotate(GetRotation().Inverse()) * direction;
		}

		/*!
		 * Transforms a vector from local space to global.
		 * The operation is not affected by position of the transform, but is affected by scale.
		 * 
		 * @param vector The direction to transform.
		 * @returns Transformed vector.
		 */
		GDINL Vector4 TransformVector(Vector4 const& vector) const
		{
			return Matrix4x4::RotateScale(GetRotation(), GetScale()) * vector;
		}
		/*!
		 * Transforms a vector from global space to local.
		 * The operation is not affected by position of the transform, but is affected by scale.
		 * 
		 * @param vector The direction to transform.
		 * @returns Transformed vector.
		 */
		GDINL Vector4 InverseTransformVector(Vector4 const& vector) const
		{
			return Matrix4x4::RotateScale(GetRotation().Inverse(), GetScale().Inverse()) * vector;
		}

		/*!
		 * Transforms a point from local space to global.
		 * The operation is affected by both position and scale of the transform.
		 * 
		 * @param point The direction to transform.
		 * @returns Transformed point.
		 */
		GDINL Vector4 TransformPoint(Vector4 const& point) const
		{
			return GetLocalToWorldMatrix() * point;
		}
		/*!
		 * Transforms a point from global space to local.
		 * The operation is affected by both position and scale of the transform.
		 * 
		 * @param point The direction to transform.
		 * @returns Transformed point.
		 */
		GDINL Vector4 InverseTransformPoint(Vector4 const& point) const
		{
			return GetWorldToLocalMatrix() * point;
		}

		/*!
		 * Moves the transform alongside the direction and on the distance translation.
		 * 
		 * @param translation Direction and distance of the translation.
		 * @param relativeTo Space this translation is relative to.
		 */
		//! @{
		GDINL void Translate(Vector4 const& translation, Space const relativeTo = Space::Local)
		{
			GD_DEBUG_VERIFY(relativeTo < Space::Count, "Invalid parameter 'relativeTo' was specified.");
			SetPosition(GetPosition() + (relativeTo == Space::Local ? translation : TransformDirection(translation)));
		}
		GDINL void Translate(Vector4 const& translation, RefPtr<Transform> const& relativeTo)
		{
			GD_DEBUG_VERIFY(relativeTo != nullptr, "Invalid parameter 'relativeTo' was specified.");
			SetPosition(GetPosition() + relativeTo->TransformDirection(translation));
		}
		//! @}

		/*!
		 * Applies the rotation to the transform.
		 * 
		 * @param translation Direction and distance of the translation.
		 * @param relativeTo Space this translation is relative to.
		 */
		//! @{
		GDINL void Rotate(Vector4 const& translation, Space const relativeTo = Space::Local)
		{
			GD_DEBUG_VERIFY(relativeTo < Space::Count, "Invalid parameter 'relativeTo' was specified.");
			GD_NOT_IMPLEMENTED();
		//	SetPosition(GetPosition() + (relativeTo == Space::Local ? translation : TransformDirection(translation)));
		}
		GDINL void Rotate(Vector4 const& translation, RefPtr<Transform> const& relativeTo)
		{
			GD_DEBUG_VERIFY(relativeTo != nullptr, "Invalid parameter 'relativeTo' was specified.");
			GD_NOT_IMPLEMENTED();
			//	SetPosition(GetPosition() + relativeTo->TransformDirection(translation));
		}
		//! @}

	};	// class Transform

GD_NAMESPACE_END
