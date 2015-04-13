// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Reflection.h
//! Common interface for RTTI/Introspection/Reflection.

#include <GoddamnEngine/Core/Object/Reflection.h>
#include <GoddamnEngine/Core/Platform/Atomics.h>

#include <GoddamnEngine/Core/Containers/StringBuilder.h>
#include <GoddamnEngine/Core/Containers/Map.h>

#include <GoddamnEngine/Core/Misc/CChar.h>
#include <GoddamnEngine/Core/Misc/CString.h>

GD_NAMESPACE_BEGIN

	GDAPI extern String ReflectionStringToReadableName(String const& Identifier)
	{
		//! @todo Implement this function.
		return Identifier;
	}

	struct ReflectionTypeInternal
	{
	};	// struct ReflectionTypeInternal

	Char static const ReflectionTagsHideInEditor[]           = "HideInEditor";
	Char static const ReflectionTagsNotSerializable[]        = "NotSerializable";
	Char static const ReflectionTagsHasOverriddenName[]      = "ShowName=";
	Char static const ReflectionTagsHasOverriddenCategory[]  = "Category=";

	GDAPI extern ReflectionTags ReflectionTagsFromString(CStr const CString)
	{
		ReflectionTags Result = ReflectionTagsTp::TagsNone;
		if (CString::Strstr(CString, ReflectionTagsHideInEditor) != nullptr) 
		{
			Result |= ReflectionTagsTp::TagsHideInEditor;
		}
		if (CString::Strstr(CString, ReflectionTagsNotSerializable) != nullptr) 
		{
			Result |= ReflectionTagsTp::TagsNotSerializable;
		}
		if (CString::Strstr(CString, ReflectionTagsHasOverriddenName) != nullptr) 
		{
			Result |= ReflectionTagsTp::TagsHasOverriddenName;
		}
		if (CString::Strstr(CString, ReflectionTagsHasOverriddenCategory) != nullptr) 
		{
			Result |= ReflectionTagsTp::TagsHasOverriddenCategory;
		}

		return Result;
	}

	template<SizeTp TagLength>
	GDINL extern void ReflectionTagsParseParam(CStr const TagsString, Char const (&TagCStr)[TagLength], String& TagResult)
	{
		CStr TagBegin = CString::Strstr(TagsString, TagCStr);
		CStr TagEnd   = CString::Strchr(TagBegin, ' ');
		GD_DEBUG_ASSERT(TagBegin != nullptr, "'%s' tag specified, but is was located.", TagCStr);

		TagBegin      = TagBegin + TagLength - 1;
		TagEnd        = TagEnd != nullptr ? TagEnd : TagBegin + CString::Strlen(TagBegin);
		TagResult     = String(TagBegin, TagEnd - TagBegin);
	//	TagResult     = TagResult.Replace('\\', ' ');	//! @todo Uncomment this when Replace function would appear.
	}

	// ------------------------------------------------------------------------------------------
	// ReflectionClass class.
	// ------------------------------------------------------------------------------------------

	//! @brief Returns a link to a map of registered reflection classes.
	//! @returns A link to a map of registered reflection classes.
	GDINT static HashMap<ReflectionClass*>& GetReflectionClasses()
	{
		static HashMap<ReflectionClass*> ReflectionClasses;
		return ReflectionClasses;
	}

	//! @brief Initializes a reflectable class.
	//! @param ClassTagsString Tags of a class.
	//! @param ClassName Name of a class.
	//! @param ClassSuper Super class of a class.
	GDAPI ReflectionClass::ReflectionClass(CStr const ClassTagsString, CStr const ClassName, ReflectionClass const* ClassSuper)
		: ClassTags(ReflectionTagsFromString(ClassTagsString))
		, ClassName(ClassName)
		, ClassSuper(ClassSuper)
		, ClassFirstProperty($empty_property)
		, ClassFirstMethod($empty_method)
	{
		HashCode ClassHash = this->ClassName.GetHashCode();
		HashMap<ReflectionClass*>& ReflectionClasses = GetReflectionClasses();
		if (ReflectionClasses.QueryIterator(ClassHash) == ReflectionClasses.End()) 
		{
			ReflectionClasses.InsertKeyValue(ClassHash, this);
			
			// Setting up property Human Readable Name.
			if ((this->ClassTags & ReflectionTagsTp::TagsHasOverriddenName) != 0) 
			{
				ReflectionTagsParseParam(ClassTagsString, ReflectionTagsHasOverriddenName, ClassHumanReadableName);
			} 
			else 
			{
				this->ClassHumanReadableName = ReflectionStringToReadableName(this->ClassName);
			}
		} 
		else 
		{
			GD_DEBUG_ASSERT_FALSE("Class with name '%s' was already registered.", this->ClassName.CStr());
		}
	}

	//! @brief Deinitializes a reflectable class.
	GDAPI ReflectionClass::~ReflectionClass()
	{
		HashCode ClassHash = this->ClassName.GetHashCode();
		HashMap<ReflectionClass*>& ReflectionClasses = GetReflectionClasses();
		if (ReflectionClasses.QueryIterator(ClassHash) != ReflectionClasses.End()) 
		{
			ReflectionClasses.RemoveElementWithKey(ClassHash);
		} 
		else 
		{
			GD_DEBUG_ASSERT_FALSE("Class with name '%s' was not already registered.", this->ClassName.CStr());
		}
	}

	// ------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------

	GDAPI ReflectionProperty::ReflectionProperty(CStr const PropertyTagsString, ReflectionType const PropertyType, ReflectionClass const* const PropertyOwnerClass, CStr const PropertyName
		, UInt32 const PropertyVectorDimensions, ReflectionPropertyGetter const PropertyGetter, ReflectionPropertySetter const PropertySetter)
		: PropertyTags(ReflectionTagsFromString(PropertyTagsString))
		, PropertyType(PropertyType)
		, PropertyOwnerClass(PropertyOwnerClass), PropertyNext(nullptr)
		, PropertyName(PropertyName)
		, PropertyVectorDimensions(PropertyVectorDimensions)
		, PropertyGetter(PropertyGetter)
		, PropertySetter(PropertySetter)
	{
		// Setting up property Human Readable Name.
		if ((this->PropertyTags & ReflectionTagsTp::TagsHasOverriddenName) != 0) 
		{
			ReflectionTagsParseParam(PropertyTagsString, ReflectionTagsHasOverriddenName, this->PropertyHumanReadableName);
		} 
		else 
		{
			this->PropertyHumanReadableName = ReflectionStringToReadableName(this->PropertyName);
		}

		// Setting up property Category.
		if ((this->PropertyTags & ReflectionTagsTp::TagsHasOverriddenCategory) != 0) 
		{
			ReflectionTagsParseParam(PropertyTagsString, ReflectionTagsHasOverriddenCategory, this->PropertyCategory);
		} 
		else 
		{
			this->PropertyCategory = "<Error: No Category Specified>";
		}
	}

GD_NAMESPACE_END
