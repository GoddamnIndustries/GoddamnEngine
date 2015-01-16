/// ==========================================================================================
/// MemberInformation.h: Common interface for member information.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_REFLECTION_MEMBERINFORMATION
#define GD_CORE_REFLECTION_MEMBERINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	class IReflectable;
	class ICustomAttribute;
	class ITypeInformation;

	template<class Type> 
	ITypeInformation const* TypeOf()
	{
		return nullptr;
	}

	enum MemberAttributes : UInt64
	{
		GD_MEMBER_ATTRIBUTE_IS_CONST     = GD_BIT(0),
		GD_MEMBER_ATTRIBUTE_IS_PROTECTED = GD_BIT(1),
		GD_MEMBER_ATTRIBUTE_IS_PUBLIC    = GD_BIT(1) | GD_MEMBER_ATTRIBUTE_IS_PROTECTED,
		GD_MEMBER_ATTRIBUTE_IS_STATIC    = GD_BIT(2),
	};	// enum MemberAttributes

	/// @brief Obtains information about the attributes of a member
	class IMemberInformation
	{
	public:
		/// @brief Returns the name of the current member.
		/// @returns The name of the current member.
		GDAPI virtual String const& GetName() const abstract;

	//	/// @brief Returns a flags that contain this member attributes.
	//	/// @returns A flags that contain this member attributes.
	//	GDAPI virtual MemberAttributes GetAttributes() const abstract;

	//	/// @brief Returns a collection that contains this member's custom attributes.
	//	/// @returns A collection that contains this member's custom attributes.
	//	GDAPI virtual Vector<ICustomAttribute const*> const& GetCustomAttributes() const abstract;
	};	// class IMemberInformation

	/// @brief Obtains information about the attributes of a member and defines methods for accessing member value
	class IAccessableMemberInformation : public IMemberInformation
	{
	public:
		/// @brief Copies value of the member to output.
		/// @param Reflectable Reflectable object we are operating on.
		/// @param OutputValuePtr Here would be copied value of the member.
		GDAPI virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const abstract;

		/// @brief Copies value of input into member.
		/// @param Reflectable Reflectable object we are operating on.
		/// @param InputValuePtr New value of the member would be copied out of here.
		GDAPI virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const abstract;
	};	// class IAccessableMemberInformation

GD_NAMESPACE_END

#endif