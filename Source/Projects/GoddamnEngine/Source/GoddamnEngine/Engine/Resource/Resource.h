/// ==========================================================================================
/// Resource.h - base resource interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2012 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_RESOURCE_RESOURCE
#define GD_RESOURCE_RESOURCE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.h>
#include <GoddamnEngine/Core/Threading/CriticalSection/CriticalSection.h>
#include <GoddamnEngine/Core/Templates/RefPtr.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>
#include <GoddamnEngine/Core/Reflection/TypeInformation.h>

#define GD_RESOURCE_ARGS (*(reinterpret_cast<String const*>(Argument)))

GD_NAMESPACE_BEGIN

	/// Type of URI - describes if URI points to some virtual data, real file, etc..
	enum class RSURIType : size_t
	{
		PathToRealFile,						///< URI points to file in file system. E.g. "@c file://path_to_file"
		PathToVirtualFile,					///< URI points to data in file. E.g. "@c package://path_to_package?path_to_file_in_package"
		PathToServer,						///< URI points to external server. E.g. "@c https://your_domain.com/your_file" 
		InlinedData,						///< URI points to data inlined into itthis. E.g. "@c inline://plain-text?Hello, World!"
		Unknown,							///< Unknown URI type (internalusage only).
		Count								= Unknown,
	};	// enum RSURIType

	/// Type of protocol this resource uses
	enum class RSURIProtocolType : size_t
	{
		File,								///< @c file:// protocol
		Package,							///< @c package:// protocol
		HTTP,								///< @c http:// protocol
		HTTPS,								///< @c https:// protocol
		Inline,								///< @c inline:// protocol
		Unknown,							///< Unknown protocol (internalusage only).
		Count								= Unknown,
	};	// enum class RSURIProtocolType

	/// Converts RSURIProtocolType to C string.
	GDAPI extern Str RSURIProtocolTypeToStr(RSURIProtocolType const Protocol);

	/// Initializes RSURIProtocolType using C string.
	GDAPI extern RSURIProtocolType RSURIProtocolTypeFromStr(Str const RSURIProtocolTypeStr);

	/// Provides parsing identifiers of objects (known as RSURI, universal resource identifiers).
	class RSURI final
	{
	private:
		GD_CLASS_UNASSIGNABLE(RSURI);
		GD_CLASS_UNCOPIABLE  (RSURI);

	private /*Class members*/:
		RSURIType IdentifierType;
		RSURIProtocolType Protocol;
		StringBuilder InlinedData;
		String PathToFile;
		String PathInFile;
		HashCode Hash;

	public /*Constructor / Destructor*/:
		/// Parses RSURI
		GDAPI explicit RSURI(String const& URI);
		GDAPI   ~RSURI() { }

	public /*Class API*/:
		/// Returns input stream that reads this resource.
		GDAPI UniquePtr<InputStream> OpenInputStream() const;

		/// Returns type of path in RSURI.
		GDINL RSURIType GetIdentifierType() const;

		/// Returns string value for protocol.
		/// @code
		///		"package://packs/my_package.gdp?my_file"
		///		 |-----| // <- Protocol
		/// @endcode
		GDINL RSURIProtocolType GetProtocol() const;

		/// Returns string value for path to file, where this resource is located.
		/// @code
		///		"package://packs/my_package.gdp?my_file"
		///		     |------------------| // <- Path to file 
		/// @endcode
		/// @note This function is allowed only for real and virtual file URI types.
		GDINL String const& GetPathToFile() const;

		/// Returns string value for path in file, where this resource is located.
		/// @code
		///		"package://packs/my_package.gdp?some_dir/my_file.ext"
		///		          |------------------| // <- Path to file 'some_dir/my_file.ext' inside package 'packs/my_package.gdp'
		/// @endcode
		/// @note This function is allowed only for virtual file URI type.
		GDINL String const& GetPathInFile() const;

		/// Returns data stored in this URI if it is inlined data.
		/// @code
		///		"inline://bytecode-bas64?R05VIFN1Y2tzIQ=="
		///			      |--------------| // <- Data, inlined in this source.
		/// @endcode
		/// @note This function is allowed only for inline data URI type.
		GDINL StringBuilder const& GetInlinedData() const;

		/// Rebuild identifier string representation .
		GDINL String GetIdentifierString() const;

	public /*Operators*/:
		GDINL bool operator== (HashCode const Hash) const;
	};	// class RSURI

	///	Describing state of Resource
	enum class RSState : UInt8
	{
		Requested,	///< The resource was requested and now is waiting to be loaded  
		Loading,	///< The resource is loading now
		Loaded,		///< The resource was loaded is ready to use 
		Unknown		///< The resource is in unknown state
	};	// enum class RSState 

	/// Runtime error that notifies that somerthing went wrong while loading some resource.
	GD_DEFINE_EXCPETION_FINAL(RSLoadingException);

	/// Runtime error that notifies that somerthing went wrong while cooking some resource.
	GD_DEFINE_EXCPETION_FINAL(RSCookingException);

	/// Describes interface of loadable object.
	class Resource : public Object
	{
		friend class RSStreamer;

	private /*Support stuff*/:
		GD_TYPEINFORMATION_DEFINITION(Resource,  Object, GDAPI);
		GD_CLASS_UNASSIGNABLE(Resource);
		GD_CLASS_UNCOPIABLE  (Resource);
		
	private /*Class members*/:
		RSState State;	///< State of resource

	public:
		RSURI const Identifier;	///< Unique identifier if this resource

	protected /*Constructor / Destructor*/:
		GDINL explicit Resource(String const& ID);
		GDINL virtual ~Resource() { }

	public /*Class API*/:
#if (defined(GD_PLATFORM_DESKTOP))
		/// @name Resource cooking.
		/// Some types of resources may be cooked. Loading cooked resource takes much less time than raw ones.
		/// E.g. HLSL shaders can be compiled to bytecode or translated to GLSL only once and than be loaded.
		/// @{

		/// Attempts to cook resource and write cooked data to output.
		/// @param   CookedDataOutput Output for cooked resource data.
		/// @returns True if resource was cooked succesfully.
		GDINT virtual void TryCook(UniquePtr<InputStream> const& InputResourceData, UniquePtr<OutputStream> const& CookedDataOutput) const { }

		/// Returns true if this resource can be cooked.
		GDINT virtual bool CanBeCooked() const { return false; }

		/// Returns true if is used cooked version of this resource.
		GDINT virtual bool IsCooked() const { return false; }

		/// @}
#endif	// if (defined(GD_PLATFORM_DESKTOP))

	protected:
		/// Function been invoked when resource is being loaded.
		/// @param InputResourceData Pointer to data that contains source of resource. Stream may point to original resource files
		/// or to cooked data.
		GDINT virtual void OnResourceLoad(UniquePtr<InputStream> const& InputResourceData) abstract;

		/// Function been invoked when resource is being destroyed.
		GDINT virtual void OnResourceUnload() abstract;
	};	// class Resource
	
	/// Provides resource streaming and immediate loading.
	/// Streamer can be launched in several modifications: as streamer - resources are loaded asynchronously in background
	/// and normally - resources are loaded when it is requires. Modification is being selected on application start
	/// and depends on cores count in PC, platform, etc.
	class RSStreamer final : public Object, public Singleton<RSStreamer>
	{
	private /*Class types and members*/:
		enum class ResourceStreamerMode : UInt8 {
			NotLaunched,
			LaunchedStreaming,
			LaunchedNormalLoading
		};	// enum class ResourceStreamerMode

		CriticalSection mutable StreamerLock;
		ResourceStreamerMode StreamerMode = ResourceStreamerMode::NotLaunched;

	public /*Constructor/Destructor*/:
		GDINL    RSStreamer() { }
		GDINL virtual ~RSStreamer() { }

	private /*Class API*/:
		/// Searches for resource with specified identifier in list and returns it if found nullptr otherwise. 
		/// @param ID     Idenitifer of the reqiured resource.
		/// @param ITypeInformation Type informartion of type that would be created if original was not found. 
		/// @param DoSearchInRequests If is set, then searching is also done in requests list.
		/// @returns First found resource that matches specified criteria or nullptr.
		GDAPI RefPtr<Resource> FindOrCreateResource(String const& ID, ITypeInformation const* const ITypeInformation);

	public /*Class API*/:
		/// Loads in to memory and/or to GPU already existing resource.
		/// @param TheResource Pointer to resource that requires to be loaded.
		/// @returns Specified resource.
		GDAPI RefPtr<Resource>&& LoadImmediately(RefPtr<Resource>&& TheResource);

		/// Creates resource (or uses existing found by identifier) and loads it.
		/// @param ID     Idenitifer of the reqiured resource.
		/// @param ITypeInformation Type informartion of type that would be created. 
		/// @returns Possible created or found immediately loaded resource.
		GDAPI RefPtr<Resource> LoadImmediately(String const& ID, ITypeInformation const* const ITypeInformation);

		/// @see RSStreamer::LoadImmediately
		template<typename ResourceType>
		GDINL RefPtr<ResourceType> LoadImmediately(String const& ID);

		/// Creates a resource with specified type and identifier (or  uses existing found by identifier) and 
		/// adds them to resource list. If streaming is enabled, than resource would be loaded in background. 
		/// Recommended using RSStreamer::WaitForLoading function to ensure, that all resources in queue are loaded.
		///	If resource streaming is disabled, than calling RSStreamer::WaitForLoading is required.
		///	to load all unloaded resource in current thread.
		GDAPI RefPtr<Resource> ProcessStreaming(String const& ID, ITypeInformation const* const ITypeInformation);

		/// @see RSStreamer::ProcessStreaming
		template<typename ResourceType>
		GDINL RefPtr<ResourceType> ProcessStreaming(String const& ID);

		/// This function waits for resources request list to be empty.
		///	If streaming is disabled, than automatically loads all unloaded resources.
		GDAPI void WaitForLoading();
	};	// class RSStreamer

	/// Resource that handles text files
	class RSBinary final : public Resource
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(RSBinary, Resource, GDAPI);

		Vector<UInt8> BinaryData;

		GDINL explicit RSBinary(String const& Identifier) : Resource(Identifier) { }
		GDINL virtual ~RSBinary() { }

	public:
		/// Returns reference on text stored in this resource file
		GDINL Vector<UInt8> const& GetBinaryData() const { return this->BinaryData; }

	protected:
		GDINL virtual void OnResourceLoad(UniquePtr<InputStream> const& InputResourceData)
		{
			this->BinaryData.Resize(InputResourceData->GetLength() + 1);
			InputResourceData->Read(&this->BinaryData[0], 1, this->BinaryData.GetLength() - 1);
			this->BinaryData.GetLastElement() = '\0';
		}

		GDINL virtual void OnResourceUnload() 
		{
			this->BinaryData.Clear();
		}
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Resource/Resource.inl>

#endif