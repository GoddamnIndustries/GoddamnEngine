#include <GoddamnEngine/Engine/Resource/Impl/Texture2D/Texture2DResource.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>

#include <tiffio.h>

GD_NAMESPACE_BEGIN

	GD_TYPEINFORMATION_IMPLEMENTATION(Texture2DResource, Resource, GDAPI, GD_RESOURCE_ARGS);

	/// ==========================================================================================
	void Texture2DResource::OnResourceLoad(
		_In_ UniquePtr<InputStream> const& InputResourceData
	)
	{
		Vector<UInt8> texture2DData(InputResourceData->GetSize());
		InputResourceData->Read(&texture2DData[0], 0, texture2DData.GetSize());
		/*std::ostringstream texture2DStream;
		texture2DStream.write(
			reinterpret_cast<char const*>(&texture2DData[0]),
			texture2DData.GetSize() / sizeof(char)
		);*/

		TIFF* const tiff = TIFFOpen(&self->Identifier.GetPathToFile()[0], "rb");//!!! TIFFStreamOpen("Tiff", &texture2DStream);
		UInt32 texture2DWidth = 0; TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &texture2DWidth);
		UInt32 texture2DHeight = 0; TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &texture2DHeight);
		Vector<Color32> texture2DPixels(static_cast<size_t>(texture2DWidth * texture2DHeight) * sizeof(Color32));
		TIFFReadRGBAImage(tiff, 
			texture2DWidth, texture2DHeight, 
			reinterpret_cast<UInt32*>(&texture2DPixels[0])
		);
		TIFFClose(tiff);

		self->Texture = HRInterface::GetInstance().CreateTexture2D(
			HRITexture2DCtorInfo(
				Resolution(static_cast<Dimension>(texture2DWidth), static_cast<Dimension>(texture2DHeight)),
				GD_FORMAT_R8G8B8A8_UINT, GD_HRI_TEXTURE_2D_MODE_STATIC, 
				GD_HRI_TEXTURE_2D_WRAP_MODE_CLAMP, GD_HRI_TEXTURE_2D_FILTERING_MODE_BILINEAR
			), 
			&texture2DPixels[0], texture2DPixels.GetSize()
		);
	}

	/// ==========================================================================================
	void Texture2DResource::OnResourceUnload(
	)
	{
		SafeRelease(self->Texture);
	}

GD_NAMESPACE_END
