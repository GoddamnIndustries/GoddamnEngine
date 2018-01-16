// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkan.cpp
 * File contains implementation for Vulkan graphics interface.
 */
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#include <GoddamnEngine/Core/Interaction/Debug.h>

GD_NAMESPACE_BEGIN

	GDEXP extern IGraphics* CreateIGraphicsInstance()
	{
		return gd_new IGraphicsVulkan();
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime..
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GDAPI IResult IGraphicsVulkan::OnRuntimePreInitialize()
	{
		m_GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_WINDOWED;	// Easier for debugging purposes.
		m_GfxResolutionsList = { {1280,720} };
		m_GfxResolutionSelected = &m_GfxResolutionsList.GetData()[0];

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsVulkan::OnRuntimeInitialize()
	{
		IResult const result = IGraphicsVulkanPlatform::OnRuntimeInitialize();
		if (IFailed(result))
		{
			return result;
		}

		// Filling the application info.
		VkApplicationInfo vkApplicationInfo = {};
		vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vkApplicationInfo.pEngineName = GD_ENGINE_NAME;
		vkApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);

		// Creating a Vulkan instance.
		VkInstanceCreateInfo vkInstanceCreateInfo = {};
		vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vkInstanceCreateInfo.enabledExtensionCount = static_cast<UInt32>(m_VkEnabledExtensions.GetLength());
		vkInstanceCreateInfo.ppEnabledExtensionNames = m_VkEnabledExtensions.GetData();
		vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
		ThrowIfFailed(vkCreateInstance(&vkInstanceCreateInfo, VK_NULL_HANDLE, &m_VkInstance));

		UInt32 vkPhysicalDevicesCount = 0;
		VkPhysicalDeviceProperties vkPhysicalDeviceProperties = {};
		ThrowIfFailed(vkEnumeratePhysicalDevices(m_VkInstance, &vkPhysicalDevicesCount, nullptr));
		ThrowIfFailed(vkEnumeratePhysicalDevices(m_VkInstance, &vkPhysicalDevicesCount, &m_VkPhysicalDevice));
		vkGetPhysicalDeviceProperties(m_VkPhysicalDevice, &vkPhysicalDeviceProperties);

		Float32 vkDeviceQueuePriorities[] = { 1.0f };
		VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo = {};
		vkDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		vkDeviceQueueCreateInfo.queueCount = 1;
		vkDeviceQueueCreateInfo.pQueuePriorities = vkDeviceQueuePriorities;

		VkDeviceCreateInfo vkDeviceCreateInfo = {};
		vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vkDeviceCreateInfo.queueCreateInfoCount = 1;
		vkDeviceCreateInfo.pQueueCreateInfos = &vkDeviceQueueCreateInfo;
		//vkDeviceCreateInfo.enabledExtensionCount = static_cast<UInt32>(m_VkEnabledExtensions.GetLength());
		//vkDeviceCreateInfo.ppEnabledExtensionNames = m_VkEnabledExtensions.GetData();
		ThrowIfFailed(vkCreateDevice(m_VkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &m_VkDevice));
		vkGetDeviceQueue(m_VkDevice, 0, 0, &m_VkQueue);

		VkWin32SurfaceCreateInfoKHR vkSurfaceCreateInfo = {};
		vkSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		vkSurfaceCreateInfo.hinstance = m_HwndHinstance;
		vkSurfaceCreateInfo.hwnd = m_HwndMain;
		ThrowIfFailed(vkCreateWin32SurfaceKHR(m_VkInstance, &vkSurfaceCreateInfo, nullptr, &m_VkSurface));

		UInt32 vkSurfaceFormatsCount = 0;
		VkSurfaceFormatKHR vkSurfaceFormats[128] = {};
		ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurface, &vkSurfaceFormatsCount, nullptr));
		ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurface, &vkSurfaceFormatsCount, vkSurfaceFormats));

		VkFormat vkSurface—olorFormat = vkSurfaceFormats[0].format;
		VkColorSpaceKHR vkSurface—olorSpace = vkSurfaceFormats[0].colorSpace;

		VkSurfaceCapabilitiesKHR vkSurfaceCapabilities = {};
		ThrowIfFailed(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VkPhysicalDevice, m_VkSurface, &vkSurfaceCapabilities));

		UInt32 vkPresentModesCount = 0;
		VkPresentModeKHR vkPresentModes[128] = {};
		ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(m_VkPhysicalDevice, m_VkSurface, &vkPresentModesCount, nullptr));
		ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(m_VkPhysicalDevice, m_VkSurface, &vkPresentModesCount, vkPresentModes));

		VkPresentModeKHR vkPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t i = 0; i < vkPresentModesCount; i++) 
		{
			if (vkPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				vkPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if (vkPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				vkPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		UInt32 VkSwapChainQueueFamilyIndices[] = {0};
		VkSwapchainCreateInfoKHR vkSwapchainCreateInfo = {};
		vkSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vkSwapchainCreateInfo.surface = m_VkSurface;
		vkSwapchainCreateInfo.minImageCount = vkSurfaceCapabilities.minImageCount;
		vkSwapchainCreateInfo.imageFormat = vkSurface—olorFormat;
		vkSwapchainCreateInfo.imageColorSpace = vkSurface—olorSpace;
		vkSwapchainCreateInfo.imageExtent = vkSurfaceCapabilities.currentExtent;
		vkSwapchainCreateInfo.imageArrayLayers = 1;
		vkSwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		vkSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkSwapchainCreateInfo.queueFamilyIndexCount = 1;
		vkSwapchainCreateInfo.pQueueFamilyIndices = VkSwapChainQueueFamilyIndices;
		vkSwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vkSwapchainCreateInfo.presentMode = vkPresentMode;
		ThrowIfFailed(vkCreateSwapchainKHR(m_VkDevice, &vkSwapchainCreateInfo, nullptr, &m_VkSwapchain));

		UInt32 vkSwapchainImagesCount = 0;
		VkImage vkSwapchainImages[128] = {};
		ThrowIfFailed(vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapchain, &vkSwapchainImagesCount, nullptr));
		ThrowIfFailed(vkGetSwapchainImagesKHR(m_VkDevice, m_VkSwapchain, &vkSwapchainImagesCount, vkSwapchainImages));

		VkImageView vkSwapchainImagesView[128] = {};
		VkImageViewCreateInfo vkImageViewCreateInfo = {};
		vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vkImageViewCreateInfo.format = vkSurface—olorFormat;
		vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		vkImageViewCreateInfo.subresourceRange.levelCount = 1;
		vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		vkImageViewCreateInfo.subresourceRange.layerCount = 1;
		for (uint32_t i = 0; i < vkSwapchainImagesCount; i++)
		{
			vkImageViewCreateInfo.image = vkSwapchainImages[i];
			ThrowIfFailed(vkCreateImageView(m_VkDevice, &vkImageViewCreateInfo, nullptr, &vkSwapchainImagesView[i]));
		}

		VkCommandPoolCreateInfo vkCommandPoolCreateInfo = {};
		vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		ThrowIfFailed(vkCreateCommandPool(m_VkDevice, &vkCommandPoolCreateInfo, nullptr, &m_VkCommandPool));

		VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo = {};
		vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vkCommandBufferAllocateInfo.commandPool = m_VkCommandPool;
		vkCommandBufferAllocateInfo.commandBufferCount = 1;
		vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		ThrowIfFailed(vkAllocateCommandBuffers(m_VkDevice, &vkCommandBufferAllocateInfo, &m_VkCommandBuffer));

		return IResult::Ok;
	}

	GDAPI void IGraphicsVulkan::OnRuntimePreUpdate()
	{
		IGraphicsVulkanWindows::OnRuntimePreUpdate();
		VkCommandBufferBeginInfo vkCommandBufferBeginInfo = {};
		vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		ThrowIfFailed(vkBeginCommandBuffer(m_VkCommandBuffer, &vkCommandBufferBeginInfo));

		ThrowIfFailed(vkEndCommandBuffer(m_VkCommandBuffer));

		VkSubmitInfo vkSubmitInfo = {};
		vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		vkSubmitInfo.commandBufferCount = 1;
		vkSubmitInfo.pCommandBuffers = &m_VkCommandBuffer;
		ThrowIfFailed(vkQueueSubmit(m_VkQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE));
		ThrowIfFailed(vkQueueWaitIdle(m_VkQueue));
	}

GD_NAMESPACE_END
	