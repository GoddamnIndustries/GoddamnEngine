// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/Symbol.cpp
//! Stores global table of symbols.

#include <GoddamnEngine/Core/Containers/Symbol.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Platform/Threading.h>
#include <GoddamnEngine/Core/Platform/Atomics.h>

GD_NAMESPACE_BEGIN

	SizeTp static const SymbolTableLength = 1024 * 1024;
	typedef SymbolTableRecord* SymbolTable[SymbolTableLength];
	enum SymbolTableStats { SymbolTableSizeof = sizeof(SymbolTable) };

	GDINT SymbolTable& GetSymbolTable()
	{
		SymbolTable static Table = { nullptr };
		return Table;
	}

	// ------------------------------------------------------------------------------------------
	// SymbolTableRecord class.
	// ------------------------------------------------------------------------------------------

	GDAPI HashCode SymbolTableRecord::ComputeHashCode(CHandle const SymbolData, SymbolTableRecordFlagsTp const SymbolFlags)
	{
		if ((SymbolFlags & (SymbolTableRecordFlags::SymbolIsInteger | SymbolTableRecordFlags::SymbolIsFloat)) != 0)
		{
			static_assert(sizeof(DeclValue<SymbolTableRecord>().SymbolDataInteger) == sizeof(Int64), "Invalid size of integer data.");
			static_assert(sizeof(DeclValue<SymbolTableRecord>().SymbolDataFloat) == sizeof(Float64), "Invalid size of float data.");
			static_assert(sizeof(HashCode) >= sizeof(UInt32), "Invalid size of a hash code.");
			struct SymbolDataParts
			{
#if GD_ENDIANITY_LITTLE
				UInt32 HighPart;
				UInt32 LowPart;
#endif	// if GD_ENDIANITY_LITTLE
#if GD_ENDIANITY_BIG
				UInt32 LowPart;
				UInt32 HighPart;
#endif	// if GD_ENDIANITY_BIG
			};	// struct SymbolDataParts
			SymbolDataParts const* DataParts = reinterpret_cast<SymbolDataParts const*>(SymbolData);
			if (DataParts->HighPart == 0)
			{
				return HashCode(DataParts->LowPart);
			}
			else
			{
				return HashCode((DataParts->HighPart ^ (DataParts->LowPart >> 4)) | ((DataParts->HighPart << 4) ^ DataParts->LowPart));
			}
		}
		if ((SymbolFlags & SymbolTableRecordFlags::SymbolIsWide) != 0)
		{
			return WideString(reinterpret_cast<WideCStr>(SymbolData)).GetHashCode();
		}
		else
		{
			return String(reinterpret_cast<CStr>(SymbolData)).GetHashCode();
		}
	}

	//! @brief Allocates a new symbol entry.
	//! @param SymbolData String symbol data.
	//! @param SymbolLength Length of a symbol data.
	//! @param SymbolFlags Initial flags of a symbol.
	//! @returns Allocated name entry.
	GDAPI SymbolTableRecord* SymbolTableRecord::AllocateSymbolTableRecord(CHandle const SymbolData, SizeTp const SymbolLength, SymbolTableRecordFlagsTp const SymbolFlags)
	{
		GD_DEBUG_ASSERT(SymbolLength < SymbolTableRecord::SymbolLengthMax, "Length of symbol entry data is greater than max (%d)", SymbolTableRecord::SymbolLengthMax);
		
		//! @todo Try to parse string data as Integer/Float

		SizeTp static const TableRecordSizeOf = sizeof(SymbolTableRecord) - sizeof(DeclValue<SymbolTableRecord>().SymbolData);
		SizeTp TableRecordDataSizeOf = 0;
		if ((SymbolFlags & SymbolTableRecordFlags::SymbolIsInteger) != 0)
		{
			TableRecordDataSizeOf = sizeof(DeclValue<SymbolTableRecord>().SymbolDataInteger);
		}
		else if ((SymbolFlags & SymbolTableRecordFlags::SymbolIsFloat) != 0)
		{
			TableRecordDataSizeOf = sizeof(DeclValue<SymbolTableRecord>().SymbolDataFloat);
		}
		else if ((SymbolFlags & SymbolTableRecordFlags::SymbolIsWide) != 0)
		{
			TableRecordDataSizeOf = (SymbolLength + 1) * sizeof(WideChar);
		}
		else
		{
			TableRecordDataSizeOf = (SymbolLength + 1) * sizeof(Char);
		}

		SymbolTableRecord* const TableRecord = reinterpret_cast<SymbolTableRecord*>(GD_MALLOC(TableRecordSizeOf + TableRecordDataSizeOf));
		CMemory::Memcpy(&TableRecord->SymbolData, sizeof(TableRecord->SymbolData), SymbolData, TableRecordDataSizeOf);
		TableRecord->SymbolTableNext = nullptr;
		TableRecord->SymbolFlags = SymbolFlags;
		TableRecord->SymbolLength = SymbolLength;
		TableRecord->SymbolHash = ComputeHashCode(SymbolData, SymbolFlags);
		return TableRecord;
	}

	//! @brief Deallocates a symbol entry.
	//! @param TableRecord A symbol entry to deallocate.
	GDAPI void SymbolTableRecord::DeallocateSymbolTableRecord(SymbolTableRecord* const TableRecord)
	{
		GD_DEALLOC(TableRecord);
	}

	// ------------------------------------------------------------------------------------------
	// Symbol class.
	// ------------------------------------------------------------------------------------------

	//! @brief Initializes a symbol with some data, it's length and flags.
	//! @param SymbolData Symbol data.
	//! @param SymbolLength Length of a symbol data.
	//! @param SymbolFlags Flags of a data.
	GDAPI Symbol::Symbol(CHandle const SymbolData, SizeTp const SymbolLength, SymbolTableRecordFlagsTp const SymbolFlags)
	{
		static CriticalSection SymbolTableSection;
		ScopedLock SymbolTableLock(&SymbolTableSection);

		HashCode SymbolHash = SymbolTableRecord::ComputeHashCode(SymbolData, SymbolFlags);
		SymbolTableRecord*& TableRecord = GetSymbolTable()[SymbolHash.GetValue() % SymbolTableLength];
		if (TableRecord == nullptr)
		{
			TableRecord = SymbolTableRecord::AllocateSymbolTableRecord(SymbolData, SymbolLength, SymbolFlags);
		}
		else
		{
			do
			{
				if (TableRecord->SymbolHash == SymbolHash)
				{
					break;
				}
				else
				{
					TableRecord = TableRecord->SymbolTableNext;
				}
			} while (TableRecord != nullptr);
		}

		this->TableRecord = TableRecord;
	}

GD_NAMESPACE_END
