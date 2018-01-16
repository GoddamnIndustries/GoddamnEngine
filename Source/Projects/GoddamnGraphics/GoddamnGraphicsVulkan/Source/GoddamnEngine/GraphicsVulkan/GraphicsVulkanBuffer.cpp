// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanBuffers.cpp
 * File contains implementation for Vulkan graphics interface: buffer objects.
 */
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            IGraphicsBuffer interface.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Creates a GPU buffer with specified parameters.
	 *
	 * @param gfxBufferPtr Pointer for output.
	 * @param gfxBufferCreateInfo Pointer to the buffer creation information.
	 * @param gfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithBuffers::GfxImm_BufferCreate(IGraphicsBuffer** const gfxBufferPtr
		, IGraphicsBufferCreateInfo const* const gfxBufferCreateInfo)
	{
		GD_DEBUG_VERIFY(gfxBufferPtr != nullptr);
		GD_DEBUG_VERIFY(gfxBufferCreateInfo != nullptr);

		VkDeviceSize const vkBufferSize = gfxBufferCreateInfo->BufferSize;
		VkBufferCreateInfo vkBufferCreateInfo = {};
		vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vkBufferCreateInfo.size = vkBufferSize;
		vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		switch (gfxBufferCreateInfo->BufferType)
		{
			case IGRAPHICS_BUFFER_TYPE_VERTEX:
				vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				break;
			case IGRAPHICS_BUFFER_TYPE_INDEX:
				vkBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				break;
			case IGRAPHICS_BUFFER_TYPE_UNIFORM:
				vkBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		VkBuffer vkBufferID = VK_NULL_HANDLE;
		vkThrowIfFailed(vkCreateBuffer(m_VkDevice, &vkBufferCreateInfo, nullptr, &vkBufferID));

		// Allocating memory..
		VkMemoryRequirements vkBufferMemoryRequirements = {};
		vkGetBufferMemoryRequirements(m_VkDevice, vkBufferID, &vkBufferMemoryRequirements);
		
		VkMemoryAllocateInfo vkBufferMemoryAllocateInfo = {};
		vkBufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vkBufferMemoryAllocateInfo.allocationSize = vkBufferMemoryRequirements.size;
		vkBufferMemoryAllocateInfo.memoryTypeIndex = 0;
		ThrowIfFailed(Cpu_FindMemoryType(vkBufferMemoryAllocateInfo.memoryTypeIndex
			, vkBufferMemoryRequirements.memoryTypeBits
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		
		VkDeviceMemory vkBufferMemory = VK_NULL_HANDLE;
		vkThrowIfFailed(vkAllocateMemory(m_VkDevice, &vkBufferMemoryAllocateInfo, nullptr, &vkBufferMemory));

		*gfxBufferPtr = gd_new IGraphicsVulkanBuffer(gfxBufferCreateInfo, vkBufferID, vkBufferMemory);
		return IResult::Ok;
	}

	/*!
	 * Destroys the GPU buffer.
	 *
	 * @param gfxBuffer Buffer to destroy.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithBuffers::GfxImm_BufferDestroy(IGraphicsBuffer* const gfxBuffer)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);
		
		GfxImm_BufferUnmap(gfxBuffer);

		auto const vkBuffer = static_cast<IGraphicsVulkanBuffer*>(gfxBuffer);
		auto const vkBufferID = vkBuffer->m_VKBufferID;
		auto const vkBufferMemory = vkBuffer->m_VKBufferMemoryID;
		gd_delete vkBuffer;

		vkDestroyBuffer(m_VkDevice, vkBufferID, nullptr);
		vkFreeMemory(m_VkDevice, vkBufferMemory, nullptr);

		return IResult::Ok;
	}

	/*!
	 * Maps all GPU's buffer data on the CPU memory.
	 *
	 * @param gfxBuffer Buffer to map.
	 * @param gfxMapMode Buffer map mode.
	 * @param gfxMappedMemory Pointer to the memory handle.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithBuffers::GfxImm_BufferMap(IGraphicsBuffer* const gfxBuffer
		, IGraphicsResourceMapMode const gfxMapMode, Handle* const gfxMappedMemory)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const vkBuffer = static_cast<IGraphicsVulkanBuffer*>(gfxBuffer);
		auto const vkBufferMemory = vkBuffer->m_VKBufferMemoryID;
		if (vkBuffer->m_VkBufferMappedMemory == nullptr)
		{
			switch (gfxMapMode)
			{
				case IGRAPHICS_RESOURCE_MAP_MODE_READ:
				case IGRAPHICS_RESOURCE_MAP_MODE_WRITE:
				case IGRAPHICS_RESOURCE_MAP_MODE_READ_WRITE:
#if GD_DEBUG
				default:
					return IResult::InvalidArguments;
#endif	// if GD_DEBUG
			}
			
			// Mapping the unmapped buffer in the read-write (no way for read-only for now) mode.
			vkThrowIfFailed(vkMapMemory(m_VkDevice, vkBufferMemory, 0, VK_WHOLE_SIZE, 0, &vkBuffer->m_VkBufferMappedMemory));
		}
		*gfxMappedMemory = vkBuffer->m_VkBufferMappedMemory;
		return IResult::Ok;
	}

	/*!
	 * Unmaps the buffer from the CPU memory.
	 *
	 * @param gfxBuffer Buffer to unmap.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithBuffers::GfxImm_BufferUnmap(IGraphicsBuffer* const gfxBuffer)
	{
		GD_DEBUG_VERIFY(gfxBuffer != nullptr);

		auto const vkBuffer = static_cast<IGraphicsVulkanBuffer*>(gfxBuffer);
		auto const vkBufferMemory = vkBuffer->m_VKBufferMemoryID;
		if (vkBuffer->m_VkBufferMappedMemory != nullptr)
		{
			// Unmapping the mapped memory..
			vkUnmapMemory(m_VkDevice, vkBufferMemory);
		}
		return IResult::Ok;
	}

GD_NAMESPACE_END
