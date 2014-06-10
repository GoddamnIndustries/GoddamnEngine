#include <GoddamnEngine/Engine/Resource/ResourceIdentifier/ResourceIdentifier.h>

GD_NAMESPACE_BEGIN

	ResourceIdentifier::ResourceIdentifier(String const& uri)
	{
		const String protocolDelimiterString = "://";
		const size_t protocolDelimiter = uri.Find(protocolDelimiterString);
		GD_ASSERT((protocolDelimiter != -1), "URI parsing failed: string doesn't matches 'protocol://value0[?value1]'");

		self->hashSumm = uri.GetHashSumm();
		self->protocol = uri.GetSubstring(0, protocolDelimiter);
		/**/ if (self->protocol == "file")
		{
			self->type = ResourceIdentifierType::PathToRealFile;
			self->pathToFile = uri.GetSubstring(protocolDelimiter + protocolDelimiterString.GetSize());
			self->pathInFile = "/";
		}
		else if (self->protocol == "package")
		{
			const size_t queryDelimiter = uri.Find('?');
			GD_ASSERT((protocolDelimiter != -1), "URI parsing failed: specified 'package' protocol, but no virtual path specified");

			self->type = ResourceIdentifierType::PathToVirtualFile;
			self->pathToFile = uri.GetSubstring(protocolDelimiter + protocolDelimiterString.GetSize(), queryDelimiter);
			self->pathInFile = uri.GetSubstring(queryDelimiter + 1);
		}
		else if ((self->protocol == "http") || (self->protocol == "https"))
		{
			self->type = ResourceIdentifierType::PathToWeb;
			self->pathToFile = uri;

			GD_ASSERT(false, "URI parsing failed: web is currently unsupported");
		}
		else if (self->protocol == "inline")
		{
			self->type = ResourceIdentifierType::PathToInlineData;

			const size_t queryDelimiter = uri.Find('?');

			const String dataText = uri.GetSubstring((queryDelimiter != -1) ? (queryDelimiter + 1) : (protocolDelimiter + protocolDelimiterString.GetSize()));
			const String dataType = (queryDelimiter != -1) ? 
				uri.GetSubstring(protocolDelimiter + protocolDelimiterString.GetSize(), queryDelimiter) 
				: "plain-text";

			/**/ if (dataType == "plain-text")
			{
				self->inlinedData.Resize(dataText.GetSize());
				memcpy(&self->inlinedData[0], &dataText[0], dataText.GetSize());
			}
			else if (dataType == "bytecode-raw")
			{
				Char doubleByte[2] = { '\0' };
				byte index = 0;
				for (size_t cnt = 0; cnt < dataText.GetSize(); cnt += 1)
				{
					const Char character = dataText[cnt];
					/**/ if ((character >= 'A') && (character <= 'F')) 
					{
						doubleByte[index++] = character - 'A';
					}
					else if ((character >= '0') && (character <= '9')) 
					{
						doubleByte[index++] = character - '0';
					}
					else
					{
						continue;
					}

					if (index == 2)
					{
						index = 0;
						Char rawCharacter = (doubleByte[0] << 4) | doubleByte[1];
						self->inlinedData.PushLast(rawCharacter);
					}
				}
			}
			else if (dataType == "bytecode-base64")
			{
				// String::DecodeBase64(inlinedData, dataText);
				GD_NOT_IMPLEMENTED();
			}
			else
			{
				GD_ASSERT(false, "URI parsing failed: parsing inlined data failed: unknown data type");
			}
		}
		else
		{
			GD_ASSERT(false, "URI parsing failed: unknown protocol");
		}
	}

GD_NAMESPACE_END
