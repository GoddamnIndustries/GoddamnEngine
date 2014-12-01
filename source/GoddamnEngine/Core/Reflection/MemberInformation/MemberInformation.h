/// ==========================================================================================
/// MemberInformation.h: Common interface for member information
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 07.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#ifndef GD_CORE_REFLECTION_MEMBERINFORMATION
#define GD_CORE_REFLECTION_MEMBERINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Text/String/String.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>

#define GD_MEMBERINFORMATION_IMPLEMENT_NAME(ThisType)					virtual String const& GetName() const { static String const TypeInfoName(#ThisType); return TypeInfoName; }
#define GD_MEMBERINFORMATION_IMPLEMENT_ATTRIBUTES(ThisType, Attributes)	virtual MemberAttributes GetAttributes() const { return MemberAttributes((Attributes)); }
#define GD_MEMBERINFORMATION_IMPLEMENT_ATTRIBUTES_DEFAULT(ThisType)		virtual MemberAttributes GetAttributes() const { return MemberAttributes(0); }
// #define GD_MEMBERINFORMATION_IMPLEMENT_CUSTOM_ATTRIBUTES_BEGIN(ThisType)virtual Vector<ICustomAttribute const*> const& GetCustomAttributes() const { static const 

GD_NAMESPACE_BEGIN

	class IReflectable;
	class ICustomAttribute;
	class ITypeInformation;

	template<class Type> ITypeInformation const* TypeOf() { return nullptr; }

	enum MemberAttributes : UInt64
	{
		GD_MEMBER_ATTRIBUTE_IS_CONST     = GD_BIT(0),
		GD_MEMBER_ATTRIBUTE_IS_PROTECTED = GD_BIT(1),
		GD_MEMBER_ATTRIBUTE_IS_PUBLIC    = GD_BIT(1) | GD_MEMBER_ATTRIBUTE_IS_PROTECTED,
		GD_MEMBER_ATTRIBUTE_IS_STATIC    = GD_BIT(2),
	};	// enum MemberAttributes

	/// Obtains information about the attributes of a member
	class IMemberInformation
	{
	public:
		/// Gets the name of the current member.
		GDAPI virtual String const& GetName() const abstract;

		/// Gets a flags that contain this member attributes.
		GDAPI virtual MemberAttributes GetAttributes() const abstract;

		// /// Gets a collection that contains this member's custom attributes.
		// GDAPI virtual Vector<ICustomAttribute const*> const& GetCustomAttributes() const abstract;
	};	// class IMemberInformation

	/// Obtains information about the attributes of a member and defines methods for accessing member value
	class IAccessableMemberInformation : public IMemberInformation
	{
	public:
		/// Copies value of the field to output.
		GDAPI virtual void GetValue(_In_ IReflectable const* const Reflectable, _Out_ handle const OutputValuePtr) const abstract;

		/// Copies value of input into field.
		GDAPI virtual void SetValue(_In_ IReflectable* const Reflectable, _In_ handle const InputValuePtr) const abstract;
	};	// class IAccessableMemberInformation

GD_NAMESPACE_END

#endif