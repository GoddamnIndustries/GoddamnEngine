/// ==========================================================================================
/// Property.h: GoddamnC++ reflectable object's property interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 11.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_REFLECTOR_REFLECTABLE_PROPERTY
#define GD_REFLECTOR_REFLECTABLE_PROPERTY

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Reflection/Reflectable.h>

#include <GoddamnReflector/Reflector.h>

GD_NAMESPACE_BEGIN

	/// Describes representation of a property.
	$GD_ENUMERATION(Type = Enumeration, Stringification = Chopped | GoddamnCase)
	enum CPPReflectablePropertyReprentationType : UInt8
	{
		GD_CPP_REFLECTABLE_PROPERTY_REPRESENTATION_TYPE_UNKNOWN		= 0,		///< Unknown property prepresentation mode (internal usage only).
		GD_CPP_REFLECTABLE_PROPERTY_REPRESENTATION_TYPE_FIELD		= 1,		///< Property is represented as field declaration.
		GD_CPP_REFLECTABLE_PROPERTY_REPRESENTATION_TYPE_GETTER		= 2,		///< Property is represented as getter function.
		GD_CPP_REFLECTABLE_PROPERTY_REPRESENTATION_TYPE_SETTER		= 3,		///< Property is represented as setter function.
	};	// enum CPPReflectablePropertyReprentationType

	struct CPPReflectableProperty final : public CPPDefinition
	{	
		bool PropertyIsReadOnly = false;																						///< True if property is read-only.
		String PropertyName;																									///< Name of property.
		String PropertyType;																									///< Type of property.
		CPPReflectablePropertyReprentationType PropertyRepresentedAs = GD_CPP_REFLECTABLE_PROPERTY_REPRESENTATION_TYPE_UNKNOWN;	///< Representation of property.
		bool PropertyHasGetter = false;																							///< True if property also has getter.
		bool PropertyHasSetter = false;																							///< True if property also has setter.
	};	// struct CPPReflectableProperty
		
GD_NAMESPACE_END

#endif
