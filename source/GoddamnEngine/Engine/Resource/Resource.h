#ifndef GD_RESOURCE_RESOURCE
#define GD_RESOURCE_RESOURCE

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Reflection/TypeInformation/TypeInformation.h>
#include <GoddamnEngine/Core/Object/Object.h>

#include <GoddamnEngine/Engine/Resource/ResourceIdentifier/ResourceIdentifier.h>

#define GD_RESOURCE_ARGS (*(String*)Argument)

GD_NAMESPACE_BEGIN

	class ResourceLoader;
	class ResourceStreamer;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	///	Enumeration describing state of Resource
	enum class ResourceState : byte
	{
		Requested,	///< The resource was requested and now is waiting to be loaded  
		Loading,	///< The resource is loading now
		Loaded,		///< The resource was loaded is ready to use 
		Unknown		///< The resource is in unknown state
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Resource interface
	class Resource : public Object
	{
		friend class ResourceStreamer;

	private:
		GD_TYPEINFORMATION_DEFINITION(Resource,  Object, GDAPI);
		GD_CLASS_UNASSIGNABLE(Resource);
		GD_CLASS_UNCOPIABLE(Resource);
		
	public:
		ResourceIdentifier const Identifier;	///< Unique identifier if this resource
		ResourceState State;					///< State of resource

	protected:
		GDINL Resource(String const& identifier);

		GDINL virtual ~Resource();

		//////////////////////////////////////////////////////////////////////////
		/// Function been invoked then resource is being loaded
		GDINT virtual void OnResourceLoad(ResourceLoader* const resourceLoader) abstract;

		//////////////////////////////////////////////////////////////////////////
		/// Function been invoked then resource is being destroyed
		GDINT virtual void OnResourceUnload() abstract;
	};
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Resource that handles text files
	class BinaryResource final : public Resource
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(BinaryResource, Resource, GDAPI);

		Vector<UInt8> BinaryData;

		//////////////////////////////////////////////////////////////////////////
		GDINL BinaryResource(
			_In_ String const& identifier
		) : Resource(identifier) { }

		//////////////////////////////////////////////////////////////////////////
		GDINL virtual ~BinaryResource(
		) { }

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Returns reference on text stored in this resource file
		GDINL Vector<UInt8> const& GetBinaryData(
		) const { return self->BinaryData; }

	protected:
		//////////////////////////////////////////////////////////////////////////
		GDINL virtual void OnResourceLoad(ResourceLoader* const resourceLoader)
		{
			self->BinaryData.Resize(resourceLoader->GetResourceDataSize());
			resourceLoader->Read(&self->BinaryData[0], self->BinaryData.GetSize());
		}

		//////////////////////////////////////////////////////////////////////////
		GDINL virtual void OnResourceUnload() 
		{
			self->BinaryData.Clear();
		}
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Resource/Resource.inl>

#endif