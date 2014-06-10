
GD_NAMESPACE_BEGIN

	//////////////////////////////////////////////////////////////////////////
	GDINL ResourceIdentifierType ResourceIdentifier::GetPathType() const
	{
		return self->type;
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL String const& ResourceIdentifier::GetProtocol() const 
	{
		return self->protocol;
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL String const& ResourceIdentifier::GetPathToFile() const
	{
		GD_ASSERT(((self->GetPathType() == ResourceIdentifierType::PathToRealFile)
			|| self->GetPathType() == ResourceIdentifierType::PathToVirtualFile), 
			"Attempted to get path to real file for inlined-file");

		return self->pathToFile;
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL String const& ResourceIdentifier::GetPathInFile() const
	{
		GD_ASSERT((self->GetPathType() == ResourceIdentifierType::PathToVirtualFile), 
			"Attempted to get path to virtual file for real file");

		return self->pathInFile;
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL const Vector<byte>& ResourceIdentifier::GetInlinedData() const
	{
		GD_ASSERT((self->GetPathType() == ResourceIdentifierType::PathToInlineData), 
			"Attempted to get path to virtual file for real file");

		return self->inlinedData;
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL String ResourceIdentifier::GetIdentifierString() const
	{
		String identifierString = String::Format("%s://%s", &self->GetProtocol()[0], &self->GetPathToFile()[0]);
		if (self->GetPathType() == ResourceIdentifierType::PathToVirtualFile)
		{
			identifierString += String::Format("?%s", &self->GetPathInFile()[0]);
		}

		return identifierString;
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL bool ResourceIdentifier::operator== (HashSumm const hashSumm) const
	{
		return (self->hashSumm == hashSumm);
	}

GD_NAMESPACE_END
