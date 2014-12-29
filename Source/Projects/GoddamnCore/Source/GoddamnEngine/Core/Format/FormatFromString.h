#ifndef GD_CORE_FORMAT_FROM_STRING
#define GD_CORE_FORMAT_FROM_STRING

#include <GoddamnEngine/Core/Format/Format.h>
#include <GoddamnEngine/Core/Text/String/String.h>

GD_NAMESPACE_BEGIN

	GDINL Format FormatFromString(String const& format)
	{
		return
			(format == "GD_FORMAT_R8_UINT") ? GD_FORMAT_R8_UINT :
			(format == "GD_FORMAT_R16_UINT") ? GD_FORMAT_R16_UINT :
			(format == "GD_FORMAT_R32_UINT") ? GD_FORMAT_R32_UINT :
			(format == "GD_FORMAT_R64_UINT") ? GD_FORMAT_R64_UINT :

			(format == "GD_FORMAT_R8G8_UINT") ? GD_FORMAT_R8G8_UINT :
			(format == "GD_FORMAT_R16G16_UINT") ? GD_FORMAT_R16G16_UINT :
			(format == "GD_FORMAT_R32G32_UINT") ? GD_FORMAT_R32G32_UINT :
			(format == "GD_FORMAT_R64G64_UINT") ? GD_FORMAT_R64G64_UINT :

			(format == "GD_FORMAT_R8G8B8_UINT") ? GD_FORMAT_R8G8B8_UINT :
			(format == "GD_FORMAT_R16G16B16_UINT") ? GD_FORMAT_R16G16B16_UINT :
			(format == "GD_FORMAT_R32G32B32_UINT") ? GD_FORMAT_R32G32B32_UINT :
			(format == "GD_FORMAT_R64G64B64_UINT") ? GD_FORMAT_R64G64B64_UINT :

			(format == "GD_FORMAT_R8G8B8A8_UINT") ? GD_FORMAT_R8G8B8A8_UINT :
			(format == "GD_FORMAT_R16G16B16A16_UINT") ? GD_FORMAT_R16G16B16A16_UINT :
			(format == "GD_FORMAT_R32G32B32A32_UINT") ? GD_FORMAT_R32G32B32A32_UINT :
			(format == "GD_FORMAT_R64G64B64A64_UINT") ? GD_FORMAT_R64G64B64A64_UINT :

			(format == "GD_FORMAT_R8_SINT") ? GD_FORMAT_R8_SINT :
			(format == "GD_FORMAT_R16_SINT") ? GD_FORMAT_R16_SINT :
			(format == "GD_FORMAT_R32_SINT") ? GD_FORMAT_R32_SINT :
			(format == "GD_FORMAT_R64_SINT") ? GD_FORMAT_R64_SINT :

			(format == "GD_FORMAT_R8G8_SINT") ? GD_FORMAT_R8G8_SINT :
			(format == "GD_FORMAT_R16G16_SINT") ? GD_FORMAT_R16G16_SINT :
			(format == "GD_FORMAT_R32G32_SINT") ? GD_FORMAT_R32G32_SINT :
			(format == "GD_FORMAT_R64G64_SINT") ? GD_FORMAT_R64G64_SINT :

			(format == "GD_FORMAT_R8G8B8_SINT") ? GD_FORMAT_R8G8B8_SINT :
			(format == "GD_FORMAT_R16G16B16_SINT") ? GD_FORMAT_R16G16B16_SINT :
			(format == "GD_FORMAT_R32G32B32_SINT") ? GD_FORMAT_R32G32B32_SINT :
			(format == "GD_FORMAT_R64G64B64_SINT") ? GD_FORMAT_R64G64B64_SINT :

			(format == "GD_FORMAT_R8G8B8A8_SINT") ? GD_FORMAT_R8G8B8A8_SINT :
			(format == "GD_FORMAT_R16G16B16A16_SINT") ? GD_FORMAT_R16G16B16A16_SINT :
			(format == "GD_FORMAT_R32G32B32A32_SINT") ? GD_FORMAT_R32G32B32A32_SINT :
			(format == "GD_FORMAT_R64G64B64A64_SINT") ? GD_FORMAT_R64G64B64A64_SINT :

			(format == "GD_FORMAT_R8_FLOAT") ? GD_FORMAT_R8_FLOAT :
			(format == "GD_FORMAT_R16_FLOAT") ? GD_FORMAT_R16_FLOAT :
			(format == "GD_FORMAT_R32_FLOAT") ? GD_FORMAT_R32_FLOAT :
			(format == "GD_FORMAT_R64_FLOAT") ? GD_FORMAT_R64_FLOAT :

			(format == "GD_FORMAT_R8G8_FLOAT") ? GD_FORMAT_R8G8_FLOAT :
			(format == "GD_FORMAT_R16G16_FLOAT") ? GD_FORMAT_R16G16_FLOAT :
			(format == "GD_FORMAT_R32G32_FLOAT") ? GD_FORMAT_R32G32_FLOAT :
			(format == "GD_FORMAT_R64G64_FLOAT") ? GD_FORMAT_R64G64_FLOAT :

			(format == "GD_FORMAT_R8G8B8_FLOAT") ? GD_FORMAT_R8G8B8_FLOAT :
			(format == "GD_FORMAT_R16G16B16_FLOAT") ? GD_FORMAT_R16G16B16_FLOAT :
			(format == "GD_FORMAT_R32G32B32_FLOAT") ? GD_FORMAT_R32G32B32_FLOAT :
			(format == "GD_FORMAT_R64G64B64_FLOAT") ? GD_FORMAT_R64G64B64_FLOAT :

			(format == "GD_FORMAT_R8G8B8A8_FLOAT") ? GD_FORMAT_R8G8B8A8_FLOAT :
			(format == "GD_FORMAT_R16G16B16A16_FLOAT") ? GD_FORMAT_R16G16B16A16_FLOAT :
			(format == "GD_FORMAT_R32G32B32A32_FLOAT") ? GD_FORMAT_R32G32B32A32_FLOAT :
			(format == "GD_FORMAT_R64G64B64A64_FLOAT") ? GD_FORMAT_R64G64B64A64_FLOAT :

			GD_FORMAT_UNKNOWN;
	}

GD_NAMESPACE_END

#endif