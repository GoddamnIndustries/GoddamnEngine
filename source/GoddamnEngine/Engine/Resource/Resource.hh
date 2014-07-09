/// ==========================================================================================
/// Resource.hh - base resource interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2012 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_RESOURCE_RESOURCE
#define GD_RESOURCE_RESOURCE

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Object/Object.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/Reflection/TypeInformation/TypeInformation.hh>

#define GD_RESOURCE_ARGS (*(String*)Argument)

GD_NAMESPACE_BEGIN

	class ResourceStreamer;

	/// Type of URI - describes if URI points to some virtual data, real file, etc..
	enum URIType : size_t
	{
		GD_URI_TYPE_PATH_TO_REAL_FILE,		///< URI points to file in file system. E.g. "@c file://path_to_file"
		GD_URI_TYPE_PATH_TO_VIRTUAL_FILE,	///< URI points to data in file. E.g. "@c package://path_to_package?path_to_file_in_package"
		GD_URI_TYPE_PATH_TO_SERVER,			///< URI points to external server. E.g. "@c https://your_domain.com/your_file" 
		GD_URI_TYPE_INLINED_DATA,			///< URI points to data inlined into itself. E.g. "@c inline://plain-text?Hello, World!"
		GD_URI_TYPE_UNKNOWN,				///< Unknown URI type (internalusage only).
		GD_URI_TYPES_COUNT					= GD_URI_TYPE_UNKNOWN,
	};	// enum URIType

	/// Type of protocol this resource uses
	enum URIProtocolType : size_t
	{
		GD_URI_PROTOCOL_FILE,				///< @c file:// protocol
		GD_URI_PROTOCOL_PACKAGE,			///< @c package:// protocol
		GD_URI_PROTOCOL_HTTP,				///< @c http:// protocol
		GD_URI_PROTOCOL_HTTPS,				///< @c https:// protocol
		GD_URI_PROTOCOL_INLINE,				///< @c inline:// protocol
		GD_URI_PROTOCOL_UNKNOWN,			///< Unknown protocol (internalusage only).
		GD_URI_PROTOCOLS_COUNT				= GD_URI_PROTOCOL_UNKNOWN,
	};	// enum URIProtocolType

	/// Converts URIProtocolType to C string.
	GDAPI extern Str URIProtocolTypeToStr(URIProtocolType const Protocol);

	/// Initializes URIProtocolType using C string.
	GDAPI extern URIProtocolType URIProtocolTypeFromStr(Str const URIProtocolTypeStr);

	/// Parsing identifiers of objects (known as URI, universal resource identifiers).
	class URI final
	{
	private:
		GD_CLASS_UNASSIGNABLE(URI);
		GD_CLASS_UNCOPIABLE  (URI);

	private /*Class members*/:
		URIType IdentifierType;
		URIProtocolType Protocol;
		StringBuilder InlinedData;
		String PathToFile;
		String PathInFile;
		HashSumm Hash;

	public /*Constructor / Destructor*/:
		/// Parses URI
		GDAPI explicit URI(String const& URI);
		GDAPI         ~URI() { }

	public /*Class API*/:
		/// Returns input stream that reads this resource.
		GDAPI UniquePtr<InputStream> OpenInputStream() const;

		/// Returns type of path in URI.
		GDINL URIType GetIdentifierType() const;

		/// Returns string value for protocol.
		/// @code
		///		"package://packs/my_package.gdp?my_file"
		///		 |-----| // <- Protocol
		/// @endcode
		GDINL URIProtocolType GetProtocol() const;

		/// Returns string value for path to file, where this resource is located.
		/// @code
		///		"package://packs/my_package.gdp?my_file"
		///		           |------------------| // <- Path to file 
		/// @endcode
		/// @note This function is allowed only for real and virtual file URI types.
		GDINL String const& GetPathToFile() const;

		/// Returns string value for path in file, where this resource is located.
		/// @code
		///		"package://packs/my_package.gdp?some_dir/my_file.ext"
		///		                               |------------------| // <- Path to file 'some_dir/my_file.ext' inside package 'packs/my_package.gdp'
		/// @endcode
		/// @note This function is allowed only for virtual file URI type.
		GDINL String const& GetPathInFile() const;

		/// Returns data stored in this URI if it is inlined data.
		/// @code
		///		"inline://bytecode-bas64?R05VIFN1Y2tzIQ=="
		///			                     |--------------| // <- Data, inlined in this source.
		/// @endcode
		/// @note This function is allowed only for inline data URI type.
		GDINL StringBuilder const& GetInlinedData() const;

		/// Rebuild identifier string representation .
		GDINL String GetIdentifierString() const;

	public /*Operators*/:
		GDINL bool operator== (HashSumm const Hash) const;
	};	// class URI

	///	Describing state of Resource
	enum class ResourceState : UInt8
	{
		Requested,	///< The resource was requested and now is waiting to be loaded  
		Loading,	///< The resource is loading now
		Loaded,		///< The resource was loaded is ready to use 
		Unknown		///< The resource is in unknown state
	};

	/// Describes interface of loadable object.
	class Resource : public Object
	{
		friend class ResourceStreamer;

	private /*Support stuff*/:
		GD_TYPEINFORMATION_DEFINITION(Resource,  Object, GDAPI);
		GD_CLASS_UNASSIGNABLE(Resource);
		GD_CLASS_UNCOPIABLE  (Resource);
		
	private /*Class members*/:
		ResourceState State;	///< State of resource

	public:
		URI const Identifier;	///< Unique identifier if this resource

	protected /*Constructor / Destructor*/:
		GDINL explicit Resource(String const& identifier);
		GDINL virtual ~Resource() { }

	public /*Class API*/:
		/// @name Resource cooking.
		/// Some types of resources may be cooked. Loading cooked resource takes much less time than raw ones.
		/// E.g. HLSL shaders can be compiled to bytecode or translated to GLSL only once and than be loaded.
		/// @{

		/// Attempts to cook resource and write cooked data to output.
		/// @param   CookedDataOutput Output for cooked resource data.
		/// @returns True if resource was cooked succesfully.
		GDINT virtual bool TryCook(OutputStream* const CookedDataOutput) const { return false; }

		/// Returns true if this resource can be cooked.
		GDINT virtual bool CanBeCooked() const { return false; }

		/// Returns true if is used cooked version of this resource.
		GDINT virtual bool IsCooked() const { return false; }

		/// @}

	protected:
		/// Function been invoked when resource is being loaded.
		/// @param InputResourceData Pointer to data that contains source of resource. Stream may point to original resource files
		/// or to cooked data.
		GDINT virtual void OnResourceLoad(InputStream* const InputResourceData) abstract;

		/// Function been invoked when resource is being destroyed.
		GDINT virtual void OnResourceUnload() abstract;
	};	// class Resource
	
	/// ==========================================================================================
	/// ==========================================================================================
	/// Resource that handles text files
	class BinaryResource final : public Resource
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(BinaryResource, Resource, GDAPI);

		Vector<UInt8> BinaryData;

		/// ==========================================================================================
		GDINL BinaryResource(
			_In_ String const& identifier
		) : Resource(identifier) { }

		/// ==========================================================================================
		GDINL virtual ~BinaryResource(
		) { }

	public:
		/// ==========================================================================================
		/// Returns reference on text stored in this resource file
		GDINL Vector<UInt8> const& GetBinaryData(
		) const { return self->BinaryData; }

	protected:
		GDINL virtual void OnResourceLoad(InputStream* const InputResourceData)
		{
			self->BinaryData.Resize(InputResourceData->GetLength() + 1);
			InputResourceData->ReadCheck(&self->BinaryData[0], 0, self->BinaryData.GetSize() - 1);
			self->BinaryData.GetLastElement() = '\0';
		}

		/// ==========================================================================================
		GDINL virtual void OnResourceUnload() 
		{
			self->BinaryData.Clear();
		}
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Resource/Resource.inl>

#endif