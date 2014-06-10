#ifndef GD_RESOURCES_RESOURCE_RESOURCE_IDENTIFGDR
#define GD_RESOURCES_RESOURCE_RESOURCE_IDENTIFGDR

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Containers/String/String.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

#include <GoddamnEngine/Engine/Resource/ResourceLoader/ResourceLoader.h>

GD_NAMESPACE_BEGIN

	/// Type for URI
	enum class ResourceIdentifierType : byte
	{
		PathToRealFile,		///< URI points to file in file system. E.g. "@c file://path_to_file"
		PathToVirtualFile,	///< URI points to data in file. E.g. "@c package://path_to_package?path_to_file_in_package"
		PathToInlineData,	///< URI points to data inlined into itself. E.g. "@c inline://plain-text?Hello, World!"
		PathToWeb			///< URI points to external server. E.g. "@c https://your_domain.com/your_file" 
	};

	/// This class provides parsing identifiers of objects (known as URI).
	class ResourceIdentifier final
	{
	private:
		GD_CLASS_UNASSIGNABLE(ResourceIdentifier);
		GD_CLASS_UNCOPIABLE(ResourceIdentifier);

		Vector<byte> inlinedData;
		String protocol;
		String pathToFile;
		String pathInFile;
		HashSumm hashSumm;
		ResourceIdentifierType type;

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Parses URI
		GDAPI ResourceIdentifier(String const& uri);

		//////////////////////////////////////////////////////////////////////////
		/// Returns type of path in URI (protocol type)
		GDINL ResourceIdentifierType GetPathType() const;

		//////////////////////////////////////////////////////////////////////////
		/// Returns string value for protocol
		///	Example: 'package://packs/my_package.iep?my_file'
		///			  |-----| <- Protocol
		GDINL String const& GetProtocol() const; 

		//////////////////////////////////////////////////////////////////////////
		/// Returns string value for path to file, where this resource is located
		///	Example: 'package://packs/my_package.iep?my_file'
		///						|------------------| <- Path to file 
		/// @note This function is allowed only for PathToRealFile, PathToVirtualFile URI types
		GDINL String const& GetPathToFile() const;

		//////////////////////////////////////////////////////////////////////////
		/// Returns string value for path in file, where this resource is located
		///	Example: 'package://packs/my_package.iep?my_file'
		///											 |-----| <- Path in file 'packs/my_package.iep'
		/// @note This function is allowed only for PathToVirtualFile URI type
		GDINL String const& GetPathInFile() const;

		//////////////////////////////////////////////////////////////////////////
		/// Returns data stored in this URI if it is inlined data
		GDINL Vector<byte> const& GetInlinedData() const;

		//////////////////////////////////////////////////////////////////////////
		/// Rebuild identifier string representation 
		GDINL String GetIdentifierString() const;

		GDINL bool operator== (HashSumm const hashSumm) const;
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Resource/ResourceIdentifier/ResourceIdentifier.inl>

#endif