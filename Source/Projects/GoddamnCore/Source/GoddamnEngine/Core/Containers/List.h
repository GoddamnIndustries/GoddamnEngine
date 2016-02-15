// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Containers/List.h
 * Double-linked list class.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	template<typename TElement>
	class ListNodeAllocator
	{
		struct DataChunkElement final
		{
			Byte   DataElement[sizeof(TElement)];
			UInt32 DataChunkIndex;
			UInt32 DataElementIndex;
		};	// struct DataChunkElement

		struct DataChunk final
		{
			UInt64 ChunkUsedElements;
			DataChunkElement ChunkElements[64];
		};	// struct DataChunk

	private:
		Vector<DataChunk> m_Chunks;

		template<typename... TArguments>
		GDINL TElement* Allocate(TArguments&&... arguments)
		{
			for (UInt32 chunkIndex = 0; chunkIndex < m_Chunks.GetLength(); ++chunkIndex)
			{
				// Finding a chunk that contains free slots.
				auto& chunk = m_Chunks[chunkIndex];
				if (chunk.ChunkUsedElements != UInt64Max)
				{
					for (UInt32 elementIndex = 0; elementIndex < 64; ++elementIndex)
					{
						UInt64 const chunkElementMask = 1 << elementIndex;
						if ((chunk.ChunkUsedElements & chunkElementMask) == 0)
						{
							// Found a free slot.
							chunk.ChunkUsedElements |= chunkElementMask;
							auto& chunkElement = chunk.ChunkElements[elementIndex];
							new (chunkElement->DataElement) TElement(Forward<TArguments>(arguments)...);
							return reinterpret_cast<TElement*>(chunkElement->DataElement);
						}
					}
				}
			}
			return nullptr;
		}

	};	// class ListNodeAllocator




GD_NAMESPACE_END
