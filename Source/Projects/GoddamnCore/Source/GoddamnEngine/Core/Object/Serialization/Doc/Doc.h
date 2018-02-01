// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Document content interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>

GD_NAMESPACE_BEGIN

    class IDoc;
    class IDocValue;
    class IDocObject;

    using DocPtr = SharedPtr<IDoc>;
	using DocValuePtr = SharedPtr<IDocValue>;
	using DocObjectPtr = SharedPtr<IDocObject>;
	using DocValueVector = Vector<DocValuePtr>;

    /*!
     * Document value type.
     */
    enum class DocValueTypeInfo
    {
        Unknown,
        Null,
        Bool    = 'b',
        Int8    = 'c',
        UInt8   = 'C',
        Int16   = 's',
        UInt16  = 'S',
        Int32   = 'i',
        UInt32  = 'I',
        Int64   = 'l',
        UInt64  = 'L',
        Float32 = 'f',
        Float64 = 'd',
        String  = 'Z',
        GUID    = 'G',
        Array   = 'A',
        Struct  = 'O',
        Object  = Struct,
    };  // enum class DocTypeInfo

	// **------------------------------------------------------------------------------------------**
	//! Abstract doucment value.
	// **------------------------------------------------------------------------------------------**
	class IDocValue : public ReferenceTarget
	{
	public:

		/*!
		 * Returns type of current property.
		 */
		GDINT virtual DocValueTypeInfo _GetTypeInfo() const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Getter operations - should be implemented.
		// ------------------------------------------------------------------------------------------

        /*!
         * Reads property.
         *
         * @param value Property value.
         * @returns True if property value was successfully read.
         */
        //! @{
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
        
		GDINT virtual bool _TryGetValue(DocValueVector& value) GD_PURE_VIRTUAL;

		GDINT virtual bool _TryGetValue(DocObjectPtr& value) GD_PURE_VIRTUAL;
        //! @}
		
	};	// class IDocValue

	// **------------------------------------------------------------------------------------------**
	//! Abstract document object.
	// **------------------------------------------------------------------------------------------**
	class IDocObject : public ReferenceTarget
	{
	public:

		/*!
		 * Finds value property by name.
		 *
		 * @param name The name of the property.
		 * @returns Pointer to the found value or null pointer.
		 */
		GDINT virtual DocValuePtr _GetProperty(String const& name) GD_PURE_VIRTUAL;
		
	};	// class IDocObject

	// **------------------------------------------------------------------------------------------**
	//! Abstract document.
	// **------------------------------------------------------------------------------------------**
	class IDoc : public ReferenceTarget
	{
	public:
		struct Result
		{
			DocObjectPtr RootObject;
			String ErrorDesc;
		};	// struct Result

		/*!
		 * Tries to parse a document.
		 *
		 * @param docInputStream Input stream that represents document data.
		 * @returns Struct that represents document parsing result.
		 */
		GDINT virtual Result _TryParseDocument(InputStream& docInputStream) GD_PURE_VIRTUAL;
		
	};	// class IDoc

GD_NAMESPACE_END
