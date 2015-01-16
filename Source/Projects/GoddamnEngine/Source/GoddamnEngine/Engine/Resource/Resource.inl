
GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// URI class
	/// ==========================================================================================
	
	GDINL RSURIType RSURI::GetIdentifierType() const
	{
		return this->IdentifierType;
	}

	GDINL RSURIProtocolType RSURI::GetProtocol() const 
	{
		return this->Protocol;
	}

	GDINL String const& RSURI::GetPathToFile() const
	{
		GD_ASSERT(((this->GetIdentifierType() == RSURIType::PathToRealFile) || this->GetIdentifierType() == RSURIType::PathToVirtualFile), "Attempted to get path to real file for inlined-file");
		return this->PathToFile;
	}

	GDINL String const& RSURI::GetPathInFile() const
	{
		GD_ASSERT((this->GetIdentifierType() == RSURIType::PathToVirtualFile), "Attempted to get path to virtual file for real file");
		return this->PathInFile;
	}

	GDINL StringBuilder const& RSURI::GetInlinedData() const
	{
		GD_ASSERT((this->GetIdentifierType() == RSURIType::InlinedData), "Attempted to get path to virtual file for real file");
		return this->InlinedData;
	}

	GDINL String RSURI::GetIdentifierString() const
	{
		StringBuilder IdentifierBuilder;
		if ((this->GetIdentifierType() == RSURIType::PathToRealFile) || this->GetIdentifierType() == RSURIType::PathToVirtualFile) {
			IdentifierBuilder.AppendFormat("%s://%s", RSURIProtocolTypeToStr(this->GetProtocol()), this->GetPathToFile().CStr());
			if (this->GetIdentifierType() == RSURIType::PathToVirtualFile)
				IdentifierBuilder.AppendFormat("?%s", this->GetPathInFile().CStr());
		} else {
			IdentifierBuilder.Append("inline://~~~some~unique~data~~~");
		}

		return IdentifierBuilder.ToString();
	}

	GDINL bool RSURI::operator== (HashCode const Hash) const
	{
		return (this->Hash == Hash);
	}

	GDINL Resource::Resource(String const& identifier) 
		: Object(Object::TreeLockingFlagsDefaultResource)
		, State(RSState::Unknown)
		, Identifier(identifier)
	{
	}

	/// ==========================================================================================
	/// RSStreamer class.
	/// Provides resource streaming and immediate loading.
	/// ==========================================================================================

	template<typename ResourceType>
	GDINL RefPtr<ResourceType> RSStreamer::LoadImmediately(String const& ID)
	{
		static_assert((TypeTraits::IsBase<Resource, ResourceType>::Value), "Type 'ResourceType' should be derived from 'Resource'");
		return (object_cast<ResourceType*>(this->LoadImmediately(ID, ResourceType::GetClassTypeInformation()).Release()));
	}

	template<typename ResourceType>
	GDINL RefPtr<ResourceType> RSStreamer::ProcessStreaming(String const& ID)
	{
		static_assert((TypeTraits::IsBase<Resource, ResourceType>::Value), "Type 'ResourceType' should be derived from 'Resource'");
		return (object_cast<ResourceType*>(RSStreamer::GetInstance().ProcessStreaming(ID, ResourceType::GetClassTypeInformation()).Release()));
	}


GD_NAMESPACE_END
