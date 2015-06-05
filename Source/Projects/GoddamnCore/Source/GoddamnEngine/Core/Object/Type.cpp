// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Object/Type.cpp
//! Common interface for runtime type information and reflection.
#include <GoddamnEngine/Core/Object/Type.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Variant.h>
#include <GoddamnEngine/Core/Platform/Threading.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Private global tables.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if 0
	// ------------------------------------------------------------------------------------------
	//! Returns reference on the global types table. 
	//!        This reference should stay private across this file.
	//! @returns Reference on the global types table.
	GDINT static Map<HashCode, UniquePtr<TypeInfo const>>& GetGlobalTypesTable()
	{
		Map<HashCode, UniquePtr<TypeInfo const>> static GlobalTypesTable;
		return GlobalTypesTable;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns reference on the global types table lock. 
	//!        This reference should stay private across this file.
	//! @returns Reference on the global types table lock.
	GDINL static CriticalSection& GetGlobalTypesTableLock()
	{
		CriticalSection static GlobalTypesLock;
		return GlobalTypesLock;
	}
#endif

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors and destructor.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes a new type information for a primitive data type.
	//! @param TypeName Raw name of the type.
	//! @param TypeSize Size of the type in bytes.
	//! @param TypeOriginative The originative of the type.
	//! @param TypeMetaInfo Meta information about this type.
	GDAPI TypeInfo::TypeInfo(String const& TypeName, SizeTp const	TypeSize, TypeInfoOriginative const TypeOriginative)
		: TypeSize(TypeSize), TypeName(TypeName), TypeSimpleName(), TypeNamespaceName(), TypeOriginative(TypeOriginative)
		, TypePrimitiveCategory(GD_TYPEINFO_PRIMITIVE_CATEGORY_UNDEFINED)/*,
	//	, TypePointerCategory(GD_TYPEINFO_PTR_CATEGORY_UNDEFINED), TypeEnumUnderlyingType(nullptr)
		, TypeClassBaseType(nullptr)*/
	{
	//	auto& GlobalTypesTable = GetGlobalTypesTable();
	//	auto& GlobalTypesTableLock = GetGlobalTypesTableLock();
	//	{
	//		ScopedLock Lock(&GlobalTypesTableLock);
	//		GlobalTypesTable.InsertKeyValue(this->TypeID, this);
	//	}
	}

	// ------------------------------------------------------------------------------------------
	//! Deinitializes a type information.
	GDAPI TypeInfo::~TypeInfo()
	{
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Extended type traits.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Global types database access.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Retrieves information for type with specified name, if such type was registered or
	//!        null pointer otherwise.
	//! @param TypeName Name of the desired type. Please, read more about naming convection.
	//! @returns The type information for type with specified name or null pointer.
	GDAPI TypeInfoPtr TypeInfo::ForName(String const& TypeName)
	{
	//	auto const& GlobalTypesTable = GetGlobalTypesTable();
	//	auto const QueiredTypeInfoIter = GlobalTypesTable.QueryIteratorWithKey(TypeName.GetHashCode());
	//	if (QueiredTypeInfoIter != GlobalTypesTable.End())
	//		return TypeInfoPtr(QueiredTypeInfoIter->Value.GetPointer());
		return nullptr;
	}

GD_NAMESPACE_END
