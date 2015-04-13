// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Symbol.h
//! Stores global table of symbols.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Platform/Atomics.h>

GD_NAMESPACE_BEGIN

	//! @brief Flags of a single entry in global symbol table.
	//! @{
	struct SymbolTableRecordFlags
	{
		typedef UInt8 Type;
		Type static const SymbolIsWide			= GD_BIT(0);
		Type static const SymbolIsInteger		= GD_BIT(1);
		Type static const SymbolIsFloat			= GD_BIT(2);
		Type static const SymbolIsCaseSensitive = GD_BIT(3);
	};	// struct SymbolTableRecordFlags
	typedef SymbolTableRecordFlags::Type SymbolTableRecordFlagsTp;
	//! @}

	//! @brief A single entry in global symbol table.
	class SymbolTableRecord final
	{
		friend class Symbol;

	public:
		SymbolTableRecord*			SymbolTableNext;
		SymbolTableRecordFlagsTp	SymbolFlags;
		HashCode					SymbolHash;
		SizeTp static const			SymbolLengthMax = 1024;
		SizeTp						SymbolLength;

	private:
		union
		{
			Char					SymbolData[SymbolLengthMax];
			WideChar				SymbolDataWide[SymbolLengthMax];
			Int64					SymbolDataInteger;
			Float64					SymbolDataFloat;
		};

	private:
		GDINL SymbolTableRecord() { }
		GDINL ~SymbolTableRecord() { }

		//! @brief Helper function that can compute hash code of an abstract amount of data by flags.
		//! @param SymbolData Symbol data.
		//! @param SymbolFlags Flags of a data.
		//! @returns Unique hash code of a symbol.
		GDAPI static HashCode ComputeHashCode(CHandle const SymbolData, SymbolTableRecordFlagsTp const SymbolFlags);

		//! @brief Allocates a new symbol entry.
		//! @param SymbolData String symbol data.
		//! @param SymbolLength Length of a symbol data.
		//! @param SymbolFlags Initial flags of a symbol.
		//! @returns Allocated name entry.
		GDAPI static SymbolTableRecord* AllocateSymbolTableRecord(CHandle const SymbolData, SizeTp const SymbolLength, SymbolTableRecordFlagsTp const SymbolFlags);

		//! @brief Deallocates a symbol entry.
		//! @param TableRecord A symbol entry to deallocate.
		GDAPI static void DeallocateSymbolTableRecord(SymbolTableRecord* const TableRecord);
	};	// class SymbolTableRecord

	//! @brief Contains a reference to unique string stored in global table. 
	class Symbol final : IContainer
	{
	private:
		SymbolTableRecord const* TableRecord;

	public:

		//! @brief Initializes a symbol with some data, it's length and flags.
		//! @param SymbolData Symbol data.
		//! @param SymbolLength Length of a symbol data.
		//! @param SymbolFlags Flags of a data.
		GDAPI Symbol(CHandle const SymbolData, SizeTp const SymbolLength, SymbolTableRecordFlagsTp const SymbolFlags);

		//! @brief Initializes a symbol with ANSI string data.
		//! @param SymbolData Initial symbol data.
		//! @{
		GDINL explicit Symbol(CStr const SymbolData)
			: Symbol(SymbolData, CString::Strlen(SymbolData), 0)
		{
		}
		template<SizeTp SymbolStringLength>
		GDINL explicit Symbol(Char(&SymbolData)[SymbolStringLength])
			: Symbol(SymbolData, SymbolStringLength - 1, 0)
		{
		}
		//! @}

		//! @brief Initializes a symbol with Wide string data.
		//! @param SymbolDataWide Initial symbol data.
		//! @{
		GDAPI explicit Symbol(WideCStr const SymbolDataWide)
			: Symbol(SymbolDataWide, WideCString::Strlen(SymbolDataWide), SymbolTableRecordFlags::SymbolIsWide)
		{
		}
		template<SizeTp SymbolStringLength>
		GDINL explicit Symbol(WideChar(&SymbolWideData)[SymbolStringLength])
			: Symbol(SymbolWideData, SymbolStringLength - 1, SymbolTableRecordFlags::SymbolIsWide)
		{
		}
		//! @}

		GDINL SymbolTableRecord const* GetTableRecord() const
		{
			return this->TableRecord;
		}

		GDINL bool operator== (Symbol const& Other) const
		{
			return this->TableRecord == Other.TableRecord;
		}

		GDINL bool operator!= (Symbol const& Other) const 
		{
			return this->TableRecord != Other.TableRecord;
		}
	};	// class Symbol

GD_NAMESPACE_END
