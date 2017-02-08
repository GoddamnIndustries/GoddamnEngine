// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Class.h
 * File contains classes with RTTI&Reflection information for objects.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/ObjectVisitor.h>
#include <GoddamnEngine/Core/Concurrency/LiteMutex.h>
#include <GoddamnEngine/Core/Containers/Map.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                               'Class' class.                                     ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	// **------------------------------------------------------------------------------------------**
	//! Implements runtime class information.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL class Class : public TNonCopyable
	{
		friend class Object;
		friend class Package;

	public:
		String          ClassName;
		ClassPtr	    ClassSuper;
		ObjectCastFlags ClassCastFlags;	/*!< Cast flags for this type. */

	private:
		Vector<Class*>   m_ClassDerived;
		ObjectCtorProc   m_ClassConstructor;
		Map<GUID, Object*> mutable m_ClassInstancesRegistry;
		LiteMutex          mutable m_ClassInstancesRegistryLock;

	public:

		GDAPI explicit Class(CStr const className, ClassPtr const classSuper, ObjectCastFlags const classCastFlags, ObjectCtorProc const classConstructor)
			: ClassName(className), ClassSuper(classSuper), ClassCastFlags(classCastFlags), m_ClassConstructor(classConstructor)
		{
		}

		GDAPI ~Class() {}

	public:

		/*!
		 * Searches for class by it's name.
		 *
		 * @param className Name of the class we are looking for.
 		 * @returns Pointer to found class or nullptr if no suck class was found.
		 */
		GDAPI static ClassPtr FindClass(String const& className) { return nullptr; }

	};	// class_ Class

GD_NAMESPACE_END
