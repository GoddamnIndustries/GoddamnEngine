// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS_OPENGL_IMPL 1

/*!
 * @file GoddamnEngine/GraphicsVulkan/GraphicsVulkanTextures.cpp
 * File contains implementation for Vulkan graphics interface: sampler states and textures.
 */
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkan.h>
#include <GoddamnEngine/GraphicsVulkan/GraphicsVulkanFormat.h>

GD_NAMESPACE_BEGIN
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         IGraphicsSampler interface.                         ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Creates a GPU sampler state with specified parameters.
	 *
	 * @param gfxSamplerPtr Pointer for output.
	 * @param gfxSamplerCreateInfo Pointer to the sampler state creation information.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithSamplers::GfxImm_SamplerCreate(IGraphicsSampler** const gfxSamplerPtr
		, IGraphicsSamplerCreateInfo const* const gfxSamplerCreateInfo)
	{
	    GD_DEBUG_VERIFY(gfxSamplerPtr != nullptr);
	    GD_DEBUG_VERIFY(gfxSamplerCreateInfo != nullptr);

		VkSamplerCreateInfo vkSamplerCreateInfo = {};
		vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		// Setting up the specified minification and magnification filtering..
		switch (gfxSamplerCreateInfo->SamplerMinFilter)
		{
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_NEAREST:
				vkSamplerCreateInfo.minFilter = VK_FILTER_NEAREST;
				vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_LINEAR:
				vkSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
				vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}
		switch (gfxSamplerCreateInfo->SamplerMagFilter)
		{
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_NEAREST:
				vkSamplerCreateInfo.magFilter = VK_FILTER_NEAREST;
				break;
			case IGRAPHICS_SAMPLER_MINMAG_FILTER_LINEAR:
				vkSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the wrap modes..
		switch (gfxSamplerCreateInfo->SamplerAddressModeU)
		{
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO:
				vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT:
				vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR:
				vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}
		switch (gfxSamplerCreateInfo->SamplerAddressModeV)
		{
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO:
				vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT:
				vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR:
				vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}
		switch (gfxSamplerCreateInfo->SamplerAddressModeW)
		{
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_CLAMP_TO_ZERO:
				vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_REPEAT:
				vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
			case IGRAPHICS_SAMPLER_ADDRESS_MODE_MIRROR:
				vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the border color..
		switch (gfxSamplerCreateInfo->SamplerBorderColor)
		{
			case IGRAPHICS_SAMPLER_BORDER_COLOR_TRANSPARENT_BLACK:
				vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
				break;
			case IGRAPHICS_SAMPLER_BORDER_COLOR_OPAQUE_BLACK:
				vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
				break;
			case IGRAPHICS_SAMPLER_BORDER_COLOR_OPAQUE_WHITE:
				vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the comparison function..
		vkSamplerCreateInfo.compareEnable = VK_TRUE;
		switch (gfxSamplerCreateInfo->SamplerCompFunc)
		{
			case IGRAPHICS_COMPARISON_MODE_NEVER:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
				break;
			case IGRAPHICS_COMPARISON_MODE_ALWAYS:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
				break;
			case IGRAPHICS_COMPARISON_MODE_EQUAL:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_EQUAL;
				break;
			case IGRAPHICS_COMPARISON_MODE_NOT_EQUAL:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_NOT_EQUAL;
				break;
			case IGRAPHICS_COMPARISON_MODE_LESS:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_LESS;
				break;
			case IGRAPHICS_COMPARISON_MODE_LESS_OR_EQUAL:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
				break;
			case IGRAPHICS_COMPARISON_MODE_GREATER:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_GREATER;
				break;
			case IGRAPHICS_COMPARISON_MODE_GREATER_OR_EQUAL:
				vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
				break;
#if GD_DEBUG
			default:
				return IResult::InvalidArguments;
#endif	// if GD_DEBUG
		}

		// Setting up the MIP level-of-details bias..
		float const vkMipLodBias = gfxSamplerCreateInfo->SamplerMipLodBias;
		vkSamplerCreateInfo.mipLodBias = vkMipLodBias;

		// Setting up the anisotropic filtering mode..
		VkBool32 const vkSamplerAnisotropyEnabled = gfxSamplerCreateInfo->SamplerAnisotropyEnabled;
		float const vkSamplerMaxAnisotropy = gfxSamplerCreateInfo->SamplerAnisotropyMax;
		vkSamplerCreateInfo.anisotropyEnable = vkSamplerAnisotropyEnabled;
		vkSamplerCreateInfo.maxAnisotropy = vkSamplerMaxAnisotropy;

		// Setting up the level-of-details-parameters and border color..
		float const vkSamplerMinLod = gfxSamplerCreateInfo->SamplerLodMin;
		float const vkSamplerMaxLod = gfxSamplerCreateInfo->SamplerLodMax;
		vkSamplerCreateInfo.minLod = vkSamplerMinLod;
		vkSamplerCreateInfo.maxLod = vkSamplerMaxLod;

		VkSampler vkSamplerID = VK_NULL_HANDLE;
		vkThrowIfFailed(vkCreateSampler(m_VkDevice, &vkSamplerCreateInfo, nullptr, &vkSamplerID));

		*gfxSamplerPtr = gd_new IGraphicsVulkanSampler(gfxSamplerCreateInfo, vkSamplerID);
		return IResult::Ok;
	}

	/*!
	 * Destroys the GPU sampler state.
	 *
	 * @param gfxSampler Sampler state to destroy.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanWithSamplers::GfxImm_SamplerDestroy(IGraphicsSampler* const gfxSampler)
	{
		GD_DEBUG_VERIFY(gfxSampler != nullptr);

		auto const vkSampler = static_cast<IGraphicsVulkanSampler*>(gfxSampler);
		auto const vkSamplerID = vkSampler->m_VKSamplerID;
		gd_delete vkSampler;

		vkDestroySampler(m_VkDevice, vkSamplerID, nullptr);

		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Shader Resources common.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Initializes an empty shader resource view.
	 */
	GDAPI IGraphicsVulkanShaderResourceView::IGraphicsVulkanShaderResourceView(IGraphicsShaderResourceType const gfxResourceType)
	{
	}

	/*!
	 * Deletes the shader resource view.
	 */
	GDAPI IGraphicsVulkanShaderResourceView::~IGraphicsVulkanShaderResourceView()
	{
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 2D Textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/*!
	 * Creates a GPU 2D texture with specified parameters.
	 *
	 * @param gfxTexture2DPtr Pointer for output.
	 * @param gfxTexture2DCreateInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture2DInitialData Initial data for our texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IGraphicsVulkanTexture2D::IGraphicsVulkanTexture2D(IGraphicsTexture2DCreateInfo const* const gfxTexture2DCreateInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
		: IGraphicsTexture2D(gfxTexture2DCreateInfo)
	{
	}

	/*! 
	 * Deletes current GPU 2D texture.
	 */
	GDAPI IGraphicsVulkanTexture2D::~IGraphicsVulkanTexture2D()
	{
	}

	/*!
	 * Queries the shader resource for current resource.
	 * @returns Pointer to the shader resource view. Should be released manually.
	 */
	GDAPI IGraphicsShaderResourceView const* IGraphicsVulkanTexture2D::CPU_GetShaderResourceView() const
	{
		return {};
	}

	/*!
	 * Updates data stored inside texture.
	 *
	 * @param gfxTexture2DData New data that would be uploaded to GPU.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanTexture2D::Imm_UpdateData(IGraphicsTexture2DData const* const gfxTexture2DData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTexture2DData != nullptr);
#endif	// if GD_DEBUG

		return IResult::Ok;
	}

	/*!
	 * Creates a GPU 2D texture with specified parameters.
	 *
	 * @param gfxTexture2DPtr Pointer for output.
	 * @param gfxTexture2DCreateInfo Pointer to the 2D texture creation information.
	 * @param gfxTexture2DInitialData Initial data for our texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseVulkanWithTextures2D::GfxImm_Texture2DCreate(IGraphicsTexture2D** const gfxTexture2DPtr
		, IGraphicsTexture2DCreateInfo const* const gfxTexture2DCreateInfo
		, IGraphicsTexture2DData const* const gfxTexture2DInitialData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTexture2DPtr != nullptr);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo != nullptr);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo->TextureWidth > 0);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo->TextureHeight > 0);
		GD_ARG_VERIFY(gfxTexture2DCreateInfo->TextureCompressionMode < IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN);
		GD_ARG_VERIFY(IGraphicsFormatGetIndex(gfxTexture2DCreateInfo->TexturePixelFormat)
			< IGRAPHICS_FORMATS_COUNT);
#endif	// if GD_DEBUG

		*gfxTexture2DPtr = gd_new IGraphicsVulkanTexture2D(gfxTexture2DCreateInfo, gfxTexture2DInitialData);
		return IResult::Ok;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Cube map textures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IGraphicsVulkanTextureCube);

	/*!
	 * Creates a GPU Cube texture with specified parameters.
	 *
	 * @param gfxTextureCubePtr Pointer for output.
	 * @param gfxTextureCubeCreateInfo Pointer to the Cube texture creation information.
	 * @param gfxTextureInitialCubeData Initial data for our texture.
	 */
	GDAPI IGraphicsVulkanTextureCube::IGraphicsVulkanTextureCube(IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
		: IGraphicsTextureCube(gfxTextureCubeCreateInfo)
	{
		GD_DEBUG_VERIFY(false, "Test this correctly!");
	}

	/*!
	 * Deletes current GPU Cube texture.
	 */
	GDAPI IGraphicsVulkanTextureCube::~IGraphicsVulkanTextureCube()
	{
	}

	/*!
	 * Queries the shader resource for current resource.
	 * @returns Pointer to the shader resource view. Should be released manually.
	 */
	GDAPI IGraphicsShaderResourceView const* IGraphicsVulkanTextureCube::CPU_GetShaderResourceView() const
	{
		return {};
	}

	/*!
	 * Updates data stored inside texture.
	 *
	 * @param gfxTextureCubeData New data that would be uploaded to GPU.
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsVulkanTextureCube::Imm_UpdateData(IGraphicsTextureCubeData const* /*const*/ gfxTextureCubeData)
	{
		if (gfxTextureCubeData == nullptr)
		{
			IGraphicsTextureCubeData static const gfxEmptyTextureCubeData = {};
			gfxTextureCubeData = &gfxEmptyTextureCubeData;
		}
		
		return IResult::Ok;
	}

	/*!
	 * Creates a GPU Cube texture with specified parameters.
	 *
	 * @param gfxTextureCubePtr Pointer for output.
	 * @param gfxTextureCubeCreateInfo Pointer to the Cube texture creation information.
	 * @param gfxTextureInitialCubeData Initial data for our texture.
	 *
	 * @returns Non-negative value if the operation succeeded.
	 */
	GDAPI IResult IGraphicsBaseVulkanWithTexturesCube::GfxImm_TextureCubeCreate(IGraphicsTextureCube** const gfxTextureCubePtr
		, IGraphicsTextureCubeCreateInfo const* const gfxTextureCubeCreateInfo
		, IGraphicsTextureCubeData const* const gfxTextureCubeInitialData)
	{
#if GD_DEBUG
		GD_ARG_VERIFY(gfxTextureCubePtr != nullptr);
		GD_ARG_VERIFY(gfxTextureCubeCreateInfo != nullptr);
		for (SizeTp i = 0; i < 6; ++i)
		{
			GD_ARG_VERIFY(gfxTextureCubeCreateInfo->CubeSides[i].TextureWidth > 0);
			GD_ARG_VERIFY(gfxTextureCubeCreateInfo->CubeSides[i].TextureHeight > 0);
			GD_ARG_VERIFY(gfxTextureCubeCreateInfo->CubeSides[i].TextureCompressionMode < IGRAPHICS_TEXTURE_2D_COMPRESSION_UNKNOWN);
			GD_ARG_VERIFY(IGraphicsFormatGetIndex(gfxTextureCubeCreateInfo->CubeSides[i].TexturePixelFormat) 
				< IGRAPHICS_FORMATS_COUNT);
		}
#endif	// if GD_DEBUG

		*gfxTextureCubePtr = gd_new IGraphicsVulkanTextureCube(gfxTextureCubeCreateInfo, gfxTextureCubeInitialData);
		return IResult::Ok;
	}

GD_NAMESPACE_END
