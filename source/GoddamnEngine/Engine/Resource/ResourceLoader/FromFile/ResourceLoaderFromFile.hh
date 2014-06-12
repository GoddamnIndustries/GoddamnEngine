#ifndef GD_RESOURCES_RESOURCE_LOADER_RESOURCE_LOADER_FROM_FILE
#define GD_RESOURCES_RESOURCE_LOADER_RESOURCE_LOADER_FROM_FILE

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Engine/Resource/ResourceLoader/ResourceLoader.hh>

GD_NAMESPACE_BEGIN

	class ResourceLoaderFromFile final : public ResourceLoader
	{
	private:
		struct ResourceLoaderFromFileInternalFileHandle;
		ResourceLoaderFromFileInternalFileHandle* fileHandle;
		mutable size_t fileSize;

	public:
		GDAPI ResourceLoaderFromFile(String const& resourceFilePath, 
			ResourceLoader::ResourceLoaderFlags const resourceLoaderFlags);

		GDAPI virtual ~ResourceLoaderFromFile();

#if !defined(GD_DOCUMENTATION)
	public:
		GDAPI virtual bool IsValidResourcePath() const;
		GDAPI virtual size_t GetResourceDataSize() const;
		GDAPI virtual void Read(handle const destination, size_t const size);
		GDAPI virtual void Write(handle const source, size_t const size);
		GDAPI virtual size_t Seek(long const seekFromCurrent, long const seekFromStart = -1);
#endif
	};

GD_NAMESPACE_END

#endif