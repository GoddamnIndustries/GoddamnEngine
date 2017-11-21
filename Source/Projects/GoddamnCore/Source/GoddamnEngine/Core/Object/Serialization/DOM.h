// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/DOM.h
 * File contains base interface to the Document-Object-Model.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/IO/Stream.h>
#include <GoddamnEngine/Core/Object/Serialization/SerializationBase.h>

GD_NAMESPACE_BEGIN

	class IDom;
	class IDomValue;
	class IDomObject;

	using DomTypeInfo = SerializationTypeInfo;

	using DomPtr = SharedPtr<IDom>;
	using DomValuePtr = SharedPtr<IDomValue>;
	using DomObjectPtr = SharedPtr<IDomObject>;
	using DomValueVector = Vector<DomValuePtr>;

	// **------------------------------------------------------------------------------------------**
	//! Interface for DOM abstract value - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class IDomValue : public ReferenceTarget
	{
	public:

		/*!
		 * Returns type of current property.
		 */
		GDINT virtual DomTypeInfo _GetTypeInfo() const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Getter operations - should be implemented.
		// ------------------------------------------------------------------------------------------

		GDINT virtual bool _TryGetValue(bool& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(Int8& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(UInt8& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(Int16& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(UInt16& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(Int32& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(UInt32& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(Int64& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(UInt64& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(Float32& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(Float64& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(String& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(DomValueVector& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(DomObjectPtr& value) GD_PURE_VIRTUAL;
		
	};	// class IDomValue

	// **------------------------------------------------------------------------------------------**
	//! Interface for DOM abstract object - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class IDomObject : public ReferenceTarget
	{
	public:

		/*!
		 * Finds DOM value property by name.
		 *
		 * @param name The name of the property.
		 * @returns Pointer to the found value or null pointer.
		 */
		GDINT virtual DomValuePtr _GetProperty(String const& name) GD_PURE_VIRTUAL;
		
	};	// class IDomObject

	// **------------------------------------------------------------------------------------------**
	//! Interface for DOM abstract document - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class IDom : public ReferenceTarget
	{
	public:
		struct Result
		{
			DomObjectPtr RootObject;
			String ErrorDesc;
		};	// struct Result

		/*!
		 * Tries to parse a DOM document.
		 *
		 * @param domInputStream Input stream that represents DOM data.
		 * @returns Struct that represents DOM parsing result.
		 */
		GDINT virtual Result _TryParseDOM(SharedPtr<InputStream> const domInputStream) GD_PURE_VIRTUAL;
		
	};	// class IDom

GD_NAMESPACE_END
