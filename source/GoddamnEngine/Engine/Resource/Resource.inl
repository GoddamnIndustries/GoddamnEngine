
GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// URI class
	/// ==========================================================================================
	
	GDINL RSURIType RSURI::GetIdentifierType() const
	{
		return self->IdentifierType;
	}

	GDINL RSURIProtocolType RSURI::GetProtocol() const 
	{
		return self->Protocol;
	}

	GDINL String const& RSURI::GetPathToFile() const
	{
		GD_ASSERT(((self->GetIdentifierType() == RSURIType::PathToRealFile) || self->GetIdentifierType() == RSURIType::PathToVirtualFile), "Attempted to get path to real file for inlined-file");
		return self->PathToFile;
	}

	GDINL String const& RSURI::GetPathInFile() const
	{
		GD_ASSERT((self->GetIdentifierType() == RSURIType::PathToVirtualFile), "Attempted to get path to virtual file for real file");
		return self->PathInFile;
	}

	GDINL StringBuilder const& RSURI::GetInlinedData() const
	{
		GD_ASSERT((self->GetIdentifierType() == RSURIType::InlinedData), "Attempted to get path to virtual file for real file");
		return self->InlinedData;
	}

	GDINL String RSURI::GetIdentifierString() const
	{
		StringBuilder IdentifierBuilder;
		if ((self->GetIdentifierType() == RSURIType::PathToRealFile) || self->GetIdentifierType() == RSURIType::PathToVirtualFile) {
			IdentifierBuilder.AppendFormat("%s://%s", RSURIProtocolTypeToStr(self->GetProtocol()), self->GetPathToFile().CStr());
			if (self->GetIdentifierType() == RSURIType::PathToVirtualFile)
				IdentifierBuilder.AppendFormat("?%s", self->GetPathInFile().CStr());
		} else {
			IdentifierBuilder.Append("inline://~~~some~unique~data~~~");
		}

		return IdentifierBuilder.ToString();
	}

	GDINL bool RSURI::operator== (HashCode const Hash) const
	{
		return (self->Hash == Hash);
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
		static_assert((TypeTraits::IsBaseType<Resource, ResourceType>::Value), "Type 'ResourceType' should be derived from 'Resource'");
		return (object_cast<ResourceType*>(self->LoadImmediately(ID, ResourceType::GetClassTypeInformation()).Release()));
	}

	template<typename ResourceType>
	GDINL RefPtr<ResourceType> RSStreamer::ProcessStreaming(String const& ID)
	{
		static_assert((TypeTraits::IsBaseType<Resource, ResourceType>::Value), "Type 'ResourceType' should be derived from 'Resource'");
		return (object_cast<ResourceType*>(RSStreamer::GetInstance().ProcessStreaming(ID, ResourceType::GetClassTypeInformation()).Release()));
	}


GD_NAMESPACE_END
