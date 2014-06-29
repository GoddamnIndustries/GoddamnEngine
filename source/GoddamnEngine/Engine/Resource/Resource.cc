//////////////////////////////////////////////////////////////////////////
/// Resource.hh - base resource implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.01.2012 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Resource/Resource.hh>
#include <GoddamnEngine/Core/IO/Stream/StringStream/StringStream.hh>
#include <GoddamnEngine/Core/IO/Stream/ServerStream/ServerStream.hh>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.hh>
#include <GoddamnEngine/Core/Text/Base64/Base64.hh>

GD_NAMESPACE_BEGIN
	
	//////////////////////////////////////////////////////////////////////////
	/// URI class
	//////////////////////////////////////////////////////////////////////////

	extern Str URIProtocolTypeToStr(URIProtocolType const Protocol)
	{	
		Str static const URIProtocolType2Str[] = {
			/* GD_URI_PROTOCOL_FILE    = */ "file",
			/* GD_URI_PROTOCOL_PACKAGE = */ "package",
			/* GD_URI_PROTOCOL_HTTP    = */ "http",
			/* GD_URI_PROTOCOL_HTTPS   = */ "https",
			/* GD_URI_PROTOCOL_INLINE  = */ "inline",
		};

		GD_DEBUG_ASSERT(Protocol < GD_URI_PROTOCOLS_COUNT, "Invalid protocol specified");
		return URIProtocolType2Str[Protocol];
	}	

	extern URIProtocolType URIProtocolTypeFromStr(Str const URIProtocolTypeStr)
	{
		if (strncmp(URIProtocolTypeStr, "file",    sizeof("file")   ) == 0) return GD_URI_PROTOCOL_FILE;
		if (strncmp(URIProtocolTypeStr, "package", sizeof("package")) == 0) return GD_URI_PROTOCOL_PACKAGE;
		if (strncmp(URIProtocolTypeStr, "http",    sizeof("http")   ) == 0) return GD_URI_PROTOCOL_HTTP;
		if (strncmp(URIProtocolTypeStr, "https",   sizeof("https")  ) == 0) return GD_URI_PROTOCOL_HTTPS;
		if (strncmp(URIProtocolTypeStr, "inline",  sizeof("inline") ) == 0) return GD_URI_PROTOCOL_INLINE;
		return GD_URI_PROTOCOL_UNKNOWN;
	}
	
	URI::URI(String const& URI)
	{
		String static const ProtocolDelimiterString = "://";
		size_t        const ProtocolDelimiter = URI.Find(ProtocolDelimiterString);
		GD_ASSERT((ProtocolDelimiter != -1), "URI parsing failed: string doesn't matches 'protocol://value0[?value1]'");

		self->Hash = URI.GetHashSumm();
		self->Protocol = URIProtocolTypeFromStr(URI.GetSubstring(0, ProtocolDelimiter).CStr());
		switch (self->Protocol)
		{
		case GD_URI_PROTOCOL_FILE:
			{
				self->IdentifierType = GD_URI_TYPE_PATH_TO_REAL_FILE;
				self->PathToFile = URI.GetSubstring(ProtocolDelimiter + ProtocolDelimiterString.GetSize());
				self->PathInFile = "/";
			}	break;

		case GD_URI_PROTOCOL_PACKAGE:
			{
				size_t const QueryDelimiter = URI.Find('?');
				GD_ASSERT(  (QueryDelimiter != -1), "URI parsing failed: specified 'package' protocol, but no virtual path specified");

				self->IdentifierType = GD_URI_TYPE_PATH_TO_VIRTUAL_FILE;
				self->PathToFile = URI.GetSubstring(ProtocolDelimiter + ProtocolDelimiterString.GetSize(), QueryDelimiter);
				self->PathInFile = URI.GetSubstring(QueryDelimiter + 1);
			}	break;

		case GD_URI_PROTOCOL_HTTP:
		case GD_URI_PROTOCOL_HTTPS:
			{
				self->IdentifierType = GD_URI_TYPE_PATH_TO_SERVER;
				self->PathToFile = URI;
				self->PathInFile = "/";
			}	break;

		case GD_URI_PROTOCOL_INLINE:
			{
				self->IdentifierType = GD_URI_TYPE_INLINED_DATA;
				size_t const QueryDelimiter = URI.Find('?');
				size_t const DataTextSize   = URI.GetSize() - (ProtocolDelimiter + ProtocolDelimiterString.GetSize());
				Str    const DataText       = URI.CStr() + ((QueryDelimiter != -1) ? (QueryDelimiter + 1) : (ProtocolDelimiter + ProtocolDelimiterString.GetSize()));
				Str    const DataType       =               (QueryDelimiter != -1) ? URI.CStr() + ProtocolDelimiter + ProtocolDelimiterString.GetSize() : "";

				/**/ if (strncmp(DataType, "bytecode-raw", sizeof("bytecode-raw")) == 0)
				{
					GD_ASSERT((DataTextSize % 2) == 0, "Invalid inline data length: unable to parse plain bytecode");
					Char DoubleByte[2] = { '\0' };
					for (Str DataTextMut = DataText; *(DataTextMut) != Char('\0'); DataTextMut += 1)
					{
						size_t const Index = DataTextMut - DataText;
						/**/ if ((*DataTextMut >= 'A') && (*DataTextMut <= 'F')) DoubleByte[Index % 2] = *DataTextMut - 'A';
						else if ((*DataTextMut >= '0') && (*DataTextMut <= '9')) DoubleByte[Index % 2] = *DataTextMut - '0';
						else continue;

						if ((Index % 2) == 0)
						{
							Char const RawCharacter = (((DoubleByte[0] & 0x0F) << 4) | (DoubleByte[1] & 0x0F));
							self->InlinedData.Append(RawCharacter);
						}
					}

				}
				else if (strncmp(DataType, "bytecode-base64", sizeof("bytecode-base64")) == 0)
				{
					StringInputStream Input(DataText, DataTextSize);
					StringOutputStream Output(self->InlinedData);
					Base64::Decode(&Input, &Output);
				}
				else if (strncmp(DataType, "plain-text", sizeof("plain-text")) == 0)
				{
					self->InlinedData.Resize(DataTextSize);
					memcpy(self->InlinedData.GetPointer(), DataText, DataTextSize * sizeof(Char));
				}
				else 
					GD_ASSERT_FALSE("URI parsing failed: parsing inlined data failed: unknown data type");
			}	break;

		default:
			{
				GD_ASSERT_FALSE("URI parsing failed: unknown protocol");
			}	break;
		}
	}

	UniquePtr<InputStream> URI::OpenInputStream() const
	{
		switch (self->Protocol)
		{
		case GD_URI_PROTOCOL_PACKAGE:
			{
			//	Package* const Pak = ResourceStreamer::GetInstance().LoadImmdiately<Package>(URI("file://" + self->GetPathInFile));
			//	Pak->
				GD_NOT_IMPLEMENTED();
			}	break;

		case GD_URI_PROTOCOL_FILE:
			{
				return UniquePtr<InputStream>(new FileInputStream(self->PathToFile));
			}	break;

		case GD_URI_PROTOCOL_HTTP:
		case GD_URI_PROTOCOL_HTTPS:
			{
				GD_NOT_IMPLEMENTED();
			}	break;

		case GD_URI_PROTOCOL_INLINE:
			{
				return UniquePtr<InputStream>(new StringInputStream(self->InlinedData));
			}	break;

		default:
			GD_ASSERT_FALSE("URI parsing failed: unknown protocol");
			break;
		}

		return UniquePtr<InputStream>(nullptr);
	}

	GD_TYPEINFORMATION_IMPLEMENTATION_C(Resource, Object, GDAPI, nullptr);
	GD_TYPEINFORMATION_IMPLEMENTATION(BinaryResource, Resource, GDAPI, GD_RESOURCE_ARGS);

GD_NAMESPACE_END
