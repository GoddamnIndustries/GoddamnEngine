#ifndef GD_CORE_MATH_HAVOK_BINDINGS
#define GD_CORE_MATH_HAVOK_BINDINGS

#include <havok/Common/Base/hkBase.h>
#include <havok/Common/Base/Math/Quaternion/hkQuaternion.h>

#include <GoddamnEngine/Core/Math.h>
#define GD_VEC3_TO_HKVEC4(vec3)					(hkVector4f(vec3.x, vec3.y, vec3.z))

#define GD_HKVEC4_TO_VEC3(hkVec4)				(Vector3(hkVec4(0), hkVec4(1), hkVec4(2)))

#define GD_QUAT_TO_HKQUAT(quat)					(hkQuaternionf(quat.x, quat.y, quat.z, quat.w))
#define GD_HKQUAT_TO_QUAT(hkQuat)				(Quaternion(hkQuat(0), hkQuat(1), hkQuat(2), hkQuat(3)))


#endif