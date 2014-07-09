
GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// URI class
	/// ==========================================================================================
	
	GDINL URIType URI::GetIdentifierType() const
	{
		return self->IdentifierType;
	}

	GDINL URIProtocolType URI::GetProtocol() const 
	{
		return self->Protocol;
	}

	GDINL String const& URI::GetPathToFile() const
	{
		GD_ASSERT(((self->GetIdentifierType() == GD_URI_TYPE_PATH_TO_REAL_FILE) || self->GetIdentifierType() == GD_URI_TYPE_PATH_TO_VIRTUAL_FILE), "Attempted to get path to real file for inlined-file");
		return self->PathToFile;
	}

	GDINL String const& URI::GetPathInFile() const
	{
		GD_ASSERT((self->GetIdentifierType() == GD_URI_TYPE_PATH_TO_VIRTUAL_FILE), "Attempted to get path to virtual file for real file");
		return self->PathInFile;
	}

	GDINL StringBuilder const& URI::GetInlinedData() const
	{
		GD_ASSERT((self->GetIdentifierType() == GD_URI_TYPE_INLINED_DATA), "Attempted to get path to virtual file for real file");
		return self->InlinedData;
	}

	GDINL String URI::GetIdentifierString() const
	{
		GD_ASSERT(((self->GetIdentifierType() == GD_URI_TYPE_PATH_TO_REAL_FILE) || self->GetIdentifierType() == GD_URI_TYPE_PATH_TO_VIRTUAL_FILE), "Attempted to get path to real file for inlined-file");
		StringBuilder IdentifierBuilder;
		IdentifierBuilder.AppendFormat("%s://%s", URIProtocolTypeToStr(self->GetProtocol()), self->GetPathToFile().CStr());
		if (self->GetIdentifierType() == GD_URI_TYPE_PATH_TO_VIRTUAL_FILE)
			IdentifierBuilder.AppendFormat("?%s", self->GetPathInFile().CStr());

		return IdentifierBuilder.ToString();
	}

	GDINL bool URI::operator== (HashSumm const Hash) const
	{
		return (self->Hash == Hash);
	}

	GDINL Resource::Resource(String const& identifier) 
		: Object(Object::TreeLockingFlagsDefaultResource)
		, State(ResourceState::Unknown)
		, Identifier(identifier)
	{
	}

GD_NAMESPACE_END
