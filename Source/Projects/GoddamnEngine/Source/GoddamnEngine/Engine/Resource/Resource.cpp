/// ==========================================================================================
/// Resource.h - base resource implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2012 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Resource/Resource.h>
#include <GoddamnEngine/Engine/Application/Application.h>
#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.h>
#include <GoddamnEngine/Core/IO/Stream/ServerStream/ServerStream.h>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>
#include <GoddamnEngine/Core/Text/Base64/Base64.h>

GD_NAMESPACE_BEGIN
	
	GD_CLASSINFO_IMPLEMENTATION_C(Resource, Object, GDAPI, nullptr);
	
	/// ==========================================================================================
	/// RSStreamer class.
	/// Provides resource streaming and immediate loading.
	/// ==========================================================================================

	GD_SINGLETON_IMPLEMENTATION(RSStreamer);

	/// ------------------------------------------------------------------------------------------
	/// Class API.
	/// ------------------------------------------------------------------------------------------

	/// Searches for resource with specified identifier in list and returns it if found nullptr otherwise. 
	/// @param ID     Idenitifer of the reqiured resource.
	/// @param IClassInfo Type informartion of type that would be created if original was not found. 
	/// @param DoSearchInRequests If is set, then searching is also done in requests list.
	/// @returns First found resource that matches specified criteria or nullptr.
	GDAPI RefPtr<Resource> RSStreamer::FindOrCreateResource(String const& ID, IClassInfo const* const IClassInfo) 
	{
		GD_DEBUG_ASSERT((IClassInfo != nullptr), "Nullptr Type information specified.");
		ScopedLock const Lock(this->StreamerLock);
		HashCode   const IDHash = ID.GetHashCode();
		for (auto const Child : IterateChildObjects<Resource>(this)) {
			if (Child->Identifier == IDHash) {
				GD_ASSERT(Child->GetClassInfo()->IsDerivedFrom(IClassInfo), "Requested resource exists, but it's type is incompatible with passed one.");
				return Child;
			}
		}
		
		/*GD_DEBUG_ASSERT((IClassInfo->VirtualConstructor != nullptr), "Attempted creation of instance of an abstract resource type");
		RefPtr<Resource> const CreatedResource(object_cast<Resource*>(IClassInfo->VirtualConstructor(nullptr, const_cast<String*>(&ID))));
		CreatedResource->AttachToObject(this);
		CreatedResource->State = RSState::Requested;

		return CreatedResource;*/
		return nullptr;
	}

	/// Loads in to memory and/or to GPU already existing resource.
	/// @param TheResource Pointer to resource that requires to be loaded.
	RefPtr<Resource>&& RSStreamer::LoadImmediately(RefPtr<Resource>&& TheResource)
	{
		GD_DEBUG_ASSERT((TheResource != nullptr), "Resource passed is nullptr");
		String const TheResourceID = TheResource->Identifier.GetIdentifierString();
		if (TheResource->State == RSState::Requested) {
			try	{	// Trying to load our resource.
				UniquePtr<InputStream> const InputResourceData = TheResource->Identifier.OpenInputStream();
#if (defined(GD_PLATFORM_DESKTOP)) && 0 /// @todo Cooking is not implemented.
				if (TheResource->CanBeCooked()) { // This resource can be cooked. We can reduce it`s loading time in several times.
					String const CookedDataOutputPath = "???";
					try	{
						Debug::Log(String::Format("Cooking resource with id %s", TheResourceID.CStr()));
						TheResource->TryCook(InputResourceData, UniquePtr<OutputStream>(new FileOutputStream(CookedDataOutputPath)));

						Debug::Log(String::Format("Loading cooked resource with id %s", TheResourceID.CStr()));
						TheResource->OnResourceLoad(UniquePtr<InputStream>(new FileInputStream(CookedDataOutputPath)));
					} catch (RSCookingException const& ResourceCookingExc) {	// Failed to cook. We need just to try to load resource normally.
						Debug::Warning(String::Format("Failed to cook Resource with id %s because of: %s. Loading raw", TheResourceID.CStr(), ResourceCookingExc.GetErrorMessage().CStr()));
						TheResource->OnResourceLoad(InputResourceData);
					}
				} else {
#else	// if (defined(GD_PLATFORM_DESKTOP))
				/*Cooking is not supported on this platform*/ {
#endif	// if (defined(GD_PLATFORM_DESKTOP))
					Debug::Log(String::Format("Loading raw resource with id %s", TheResource->Identifier.GetIdentifierString().CStr()));
					TheResource->OnResourceLoad(InputResourceData);
				}
			} catch (RSLoadingException const& ResourceLoadingExc) {
				throw Exception(String::Format("Resource loading failed because of: %s", ResourceLoadingExc.GetErrorMessage().CStr()));
			} catch (IOException const& IOExc) {
				throw Exception(String::Format("Resource loading failed because following resource could not be loaded properly. %s", IOExc.GetErrorMessage().CStr()));
			}
		}

		TheResource->State = RSState::Loaded;
		return Forward<RefPtr<Resource>>(TheResource);
	}

	/// Creates resource (or uses existing found by identifier) and loads it.
	/// @param ID Idenitifer of the reqiured resource.
	/// @param IClassInfo Type informartion of type that would be created. 
	/// @returns Possible created or found immediately loaded resource.
	RefPtr<Resource> RSStreamer::LoadImmediately(String const& ID, IClassInfo const* const IClassInfo)
	{
		return this->LoadImmediately(this->FindOrCreateResource(ID, IClassInfo));
	}

	RefPtr<Resource> RSStreamer::ProcessStreaming(String const& ID, IClassInfo const* const IClassInfo)
	{	// Resource would loaded in some time.
		return this->FindOrCreateResource(ID, IClassInfo);
	}

	void RSStreamer::WaitForLoading()
	{
		switch(this->StreamerMode) {
			case ResourceStreamerMode::NotLaunched:	{
				if (false) {
				} else {
					Debug::Log("Resource streaming is unsupported: resources are loaded in main thread");
					this->StreamerMode = ResourceStreamerMode::LaunchedNormalLoading;
				}
			} break;
			case ResourceStreamerMode::LaunchedNormalLoading: {
				//while (this->requestedResources.GetLength() != 0)
				//{
				//	RefPtr<Resource> const resource = this->requestedResources.GetFirstElement().Value;
				//	RSStreamer::GetInstance().LoadImmediately(resource);
				//}
			} break;
			case ResourceStreamerMode::LaunchedStreaming: {
				//while (this->requestedResources.GetLength() != 0);
			} break;
		}
	}
	
	GD_CLASSINFO_IMPLEMENTATION(RSBinary, Resource, GDAPI, GD_RESOURCE_ARGS);

	/// ==========================================================================================
	/// URI class.
	/// Provides parsing identifiers of objects (known as RSURI, universal resource identifiers).
	/// ==========================================================================================

	extern Str RSURIProtocolTypeToStr(RSURIProtocolType const Protocol)
	{	
		Str static const RSURIProtocolType2Str[] = {
			/* RSURIProtocolType::File = */ "file",
			/* RSURIProtocolType::Package = */ "package",
			/* RSURIProtocolType::HTTP = */ "http",
			/* RSURIProtocolType::HTTPS   = */ "https",
			/* RSURIProtocolType::Inline  = */ "inline",
		};

		GD_DEBUG_ASSERT(Protocol < RSURIProtocolType::Count, "Invalid protocol specified");
		return RSURIProtocolType2Str[static_cast<size_t>(Protocol)];
	}	

	extern RSURIProtocolType RSURIProtocolTypeFromStr(Str const RSURIProtocolTypeStr)
	{
		if (strncmp(RSURIProtocolTypeStr, "file", sizeof("file")   ) == 0) return RSURIProtocolType::File;
		if (strncmp(RSURIProtocolTypeStr, "package", sizeof("package")) == 0) return RSURIProtocolType::Package;
		if (strncmp(RSURIProtocolTypeStr, "http", sizeof("http")   ) == 0) return RSURIProtocolType::HTTP;
		if (strncmp(RSURIProtocolTypeStr, "https",   sizeof("https")  ) == 0) return RSURIProtocolType::HTTPS;
		if (strncmp(RSURIProtocolTypeStr, "inline",  sizeof("inline") ) == 0) return RSURIProtocolType::Inline;
		return RSURIProtocolType::Unknown;
	}
	
	RSURI::RSURI(String const& URI)
	{
		String static const ProtocolDelimiterString = "://";
		size_t  const ProtocolDelimiter = URI.Find(ProtocolDelimiterString.CStr());
		GD_ASSERT((ProtocolDelimiter != -1), "URI parsing failed: string doesn't matches 'protocol://value0[?value1]'");

		this->Hash = URI.GetHashCode();
		this->Protocol = RSURIProtocolTypeFromStr(URI.Substring(0, ProtocolDelimiter).CStr());
		switch (this->Protocol)
		{
			case RSURIProtocolType::File: {
				this->IdentifierType = RSURIType::PathToRealFile;
				this->PathToFile = Application::GetInstance().GetEnvironmentPath() + URI.Substring(ProtocolDelimiter + ProtocolDelimiterString.GetLength());
				this->PathInFile = "/";
			}	break;

			case RSURIProtocolType::Package: {
				size_t const QueryDelimiter = URI.Find('?');
				GD_ASSERT((QueryDelimiter != -1), "URI parsing failed: specified 'package' protocol, but no virtual path specified");

				this->IdentifierType = RSURIType::PathToVirtualFile;
				this->PathToFile = Application::GetInstance().GetEnvironmentPath() + URI.Substring(ProtocolDelimiter + ProtocolDelimiterString.GetLength(), QueryDelimiter);
				this->PathInFile = URI.Substring(QueryDelimiter + 1);
			}	break;

			case RSURIProtocolType::HTTP:
			case RSURIProtocolType::HTTPS: {
				this->IdentifierType = RSURIType::PathToServer;
				this->PathToFile = URI;
				this->PathInFile = "/";
			}	break;

			case RSURIProtocolType::Inline:	{
					this->IdentifierType = RSURIType::InlinedData;
					size_t const QueryDelimiter = URI.Find('?');
					size_t const DataTextSize   = URI.GetLength() - (ProtocolDelimiter + ProtocolDelimiterString.GetLength());
					Str const DataText    = URI.CStr() + ((QueryDelimiter != -1) ? (QueryDelimiter + 1) : (ProtocolDelimiter + ProtocolDelimiterString.GetLength()));
					Str const DataType    =      (QueryDelimiter != -1) ? URI.CStr() + ProtocolDelimiter + ProtocolDelimiterString.GetLength() : "";

					if (strncmp(DataType, "bytecode-raw", sizeof("bytecode-raw") - 1) == 0)	{
						GD_ASSERT((DataTextSize % 2) == 0, "Invalid inline data length: unable to parse plain bytecode");
						Char DoubleByte[2] = { '\0' };
						for (Str DataTextMut = DataText; *(DataTextMut) != Char('\0'); DataTextMut += 1) {
							size_t const Index = DataTextMut - DataText;
							/**/ if ((*DataTextMut >= 'A') && (*DataTextMut <= 'F')) DoubleByte[Index % 2] = *DataTextMut - 'A';
							else if ((*DataTextMut >= '0') && (*DataTextMut <= '9')) DoubleByte[Index % 2] = *DataTextMut - '0';
							else continue;

							if ((Index % 2) == 0) {
								Char const RawCharacter = (((DoubleByte[0] & 0x0F) << 4) | (DoubleByte[1] & 0x0F));
								this->InlinedData.Append(RawCharacter);
							}
						}
					} else if (strncmp(DataType, "bytecode-base64", sizeof("bytecode-base64") - 1) == 0) {
						StringInputStream Input(DataText, DataTextSize);
						StringOutputStream Output(this->InlinedData);
						Base64::Decode(&Input, &Output);
					} else if (strncmp(DataType, "plain-text", sizeof("plain-text") - 1) == 0) {
						this->InlinedData.Resize(DataTextSize);
						memcpy(this->InlinedData.CStr(), DataText, DataTextSize * sizeof(Char));
					} else 
						GD_DEBUG_ASSERT_FALSE("URI parsing failed: parsing inlined data failed: unknown data type");
				}	break;

			default: {
					GD_DEBUG_ASSERT_FALSE("URI parsing failed: unknown protocol");
				}	break;
		}
	}

	UniquePtr<InputStream> RSURI::OpenInputStream() const
	{
		switch (this->Protocol)
		{
		case RSURIProtocolType::Package:
				GD_NOT_IMPLEMENTED();
		case RSURIProtocolType::File:
				return UniquePtr<InputStream>(new FileInputStream(this->PathToFile));
		case RSURIProtocolType::HTTP:
		case RSURIProtocolType::HTTPS:
				GD_NOT_IMPLEMENTED();
		case RSURIProtocolType::Inline:
				return UniquePtr<InputStream>(new StringInputStream(this->InlinedData));

		default:
			GD_DEBUG_ASSERT_FALSE("URI parsing failed: unknown protocol");
			break;
		}
	}

GD_NAMESPACE_END
