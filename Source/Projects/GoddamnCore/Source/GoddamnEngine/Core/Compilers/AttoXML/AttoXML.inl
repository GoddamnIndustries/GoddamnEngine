/// ==========================================================================================
/// AttoXML.inl: World`s fastest XML parser/analyzer implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 16.07.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <assert.h>
#include <setjmp.h>

#if (defined(_UNICODE))
#	include <wchar.h>
#	define _AttoText(Text) L##Text
#	define _AttoStrLen  wcslen
#	define _AttoStrNCmp wcsncmp
#	define _AttoSprintf swprintf
#	define _AttoIsAlnum iswalnum
#	define _AttoIsAlpha iswalpha
#	define _AttoIsBlank iswblank
#	else	// if (defined(_UNICODE))
#	include <string.h>
#	define _AttoText(Text) Text
#	define _AttoStrLen  strlen
#	define _AttoStrNCmp strncmp
#	define _AttoSprintf sprintf
#	define _AttoIsAlnum isalnum
#	define _AttoIsAlpha isalpha
#	define _AttoIsBlank isblank
#endif	// if (defined(_UNICODE))

#if ((defined(_MSC_VER)) && (defined(__cplusplus)))
#	pragma warning(push)
#	pragma warning(disable : 4611)	// interaction between '_setjmp' and C++ object destruction is non-portable
#endif	// if ((defined(_MSC_VER)) && (defined(__cplusplus)))

#define _AttoStringLength(String) ((sizeof(String) / sizeof(String[0])) - 1)

inline void AttoStringSliceInitEmpty(AttoStringSlice* const StringSlice) {
	StringSlice->Pointer = ATTO_XML_NULL;
	StringSlice->Length  = 0;
}

inline void AttoStringSliceInitString(AttoStringSlice* const StringSlice, AttoChar const* const String, short unsigned Length) {
	StringSlice->Pointer = String;
	StringSlice->Length  = Length;
}

inline bool AttoStringSliceEquals(AttoStringSlice const* const StringSlice, AttoChar const* const String) {
	size_t Index;
	for (Index = 0; Index < StringSlice->Length; ++Index) {
		if (StringSlice->Pointer[Index] != String[Index]) {
			return (false);
		}
	}

	return (String[StringSlice->Length] == AttoChar('\0'));
}

inline void AttoXMLNodeInitEmpty(AttoXMLNode* const XMLNode) {
	size_t Index;
	AttoStringSliceInitEmpty(&XMLNode->NodeName);
	for (Index = 0; Index < ATTO_XML_MAX_ATTRIBUTES; ++Index) {
		AttoStringSliceInitEmpty(&XMLNode->NodeAttributes[Index].AttributeName);
		AttoStringSliceInitEmpty(&XMLNode->NodeAttributes[Index].AttributeValue);
	}
	XMLNode->NodeAttributesCount = 0;
	XMLNode->NodeRoot = USHRT_MAX;
	memset(XMLNode->NodeChildIndices, 0, sizeof(XMLNode->NodeChildIndices));
}

inline void _AttoXMLContextInitEmpty(AttoXMLContext* const XMLContext) {
	size_t Index;
	XMLContext->ContextData = ATTO_XML_NULL;
	XMLContext->ContextNodesCount = 0;
	for (Index = 0; Index < ATTO_XML_MAX_CONTEXT_NODES; ++Index) {
		AttoXMLNodeInitEmpty(&XMLContext->ContextNodes[Index]);
	}
	memset(XMLContext->ContextNormalizedText, 0, sizeof(XMLContext->ContextNormalizedText));
}

typedef struct _AttoXMLContextInternals {
	jmp_buf   ContextExceptionEnv;
	AttoChar const* ContextData;	             ///<
	short unsigned  ContextNodesCount;	            ///<
} _AttoXMLContextInternals;

inline AttoChar _AttoXMLContextNextExpectCharacter(AttoXMLContext* const XMLContext, _AttoXMLContextInternals* const XMLContextInternals) {
	AttoChar const ExpectedNextCharacter = *(++XMLContext->ContextData);
	if (ExpectedNextCharacter == AttoChar('\0')) {
		_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: Unexpected end of stream.");
		/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
	}

	return ExpectedNextCharacter;
}

inline short unsigned _AttoSkipSpaces(AttoXMLContext* const XMLContext, _AttoXMLContextInternals* const XMLContextInternals) {
	short unsigned DeltaPosition = 0;
	while ((_AttoIsBlank(*XMLContext->ContextData) != 0) || (*XMLContext->ContextData != AttoChar('\n')) || (*XMLContext->ContextData != AttoChar('\r'))) {
		if ((_AttoIsAlnum(*XMLContext->ContextData) == 0) && (*XMLContext->ContextData != AttoChar('-'))) {
			_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: '%c' is not allowed inside idenitifier.", *XMLContext->ContextData);
			/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
		}

		_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals);
		++DeltaPosition;
	}

	return DeltaPosition;
}

inline void _AttoXMLContextParseHeader(AttoXMLContext* const XMLContext, _AttoXMLContextInternals* const XMLContextInternals) {
	if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('<')) {
		if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('?')) {
			if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('x')) {
				if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('m')) {
					if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('l')) {
						_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: XML document should start with '<?xml' text.");
						/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
					}
				}
			}
		}
	}

	for (;;) {	// Searching Header end.
		if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('?')) {
			if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) != AttoChar('>')) {	// Since this is very light parser we do not parse header.
				_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: '?%c' combination is not allowed inside XML comment.", *XMLContext->ContextData);
				/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
			}

			break;
		}
	}

	_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals);
}

inline void _AttoXMLContextParseComment(AttoXMLContext* const XMLContext, _AttoXMLContextInternals* const XMLContextInternals) {
	if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('-')) {
		if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) != AttoChar('-')) {
			_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: Invalid comment start was located.");
			/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
		}
	}

	for (;;) {	// Skipping to comment end.
		if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('-')) {
			if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) == AttoChar('-')) {
				if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) != AttoChar('>')) {
					_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: '--' combination is not allowed inside XML comment.");
					/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
				}

				break;
			}
		}
	}
}

inline void _AttoXMLContextParseNodeBegin(AttoXMLContext* const XMLContext, _AttoXMLContextInternals* const XMLContextInternals) {
	if (_AttoIsAlpha(*XMLContext->ContextData) == 0) {
		_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: '%c' is not allowed as first character of node name.", *XMLContext->ContextData);
		/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
	}

	AttoXMLNode* const NewXMLNode = &XMLContext->ContextNodes[XMLContext->ContextNodesCount];
	if ((++XMLContext->ContextNodesCount) == ATTO_XML_MAX_CONTEXT_NODES) {
		_AttoSprintf(XMLContext->ContextErrorText, "Internal error: Too many nodes inside context.");
		/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
	}

	NewXMLNode->NodeName.Pointer = XMLContext->ContextData;
	NewXMLNode->NodeName.Length = _AttoSkipSpaces(XMLContext, XMLContextInternals);

	for (;;) {
		if ((*XMLContext->ContextData) == AttoChar('>')) {
			assert(0 && "Not implemented.");
		}
		else if ((*XMLContext->ContextData) == AttoChar('/')) {
			if (_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals) != AttoChar('>')) {
				_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: '/%c' combination is not allowed in node definition.", *XMLContext->ContextData);
				/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
			}

			assert(0 && "Not implemented.");
		}

		if (_AttoIsAlpha(_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals)) != 0) {
			AttoXMLAttribute* const NewXMLAttribute = &NewXMLNode->NodeAttributes[NewXMLNode->NodeAttributesCount];
			if ((++NewXMLNode->NodeAttributesCount) == ATTO_XML_MAX_ATTRIBUTES) {
				_AttoSprintf(XMLContext->ContextErrorText, "Internal error: Too many attributes inside node.");
				/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
			}

			while ((_AttoIsBlank(*XMLContext->ContextData) != 0) && (*XMLContext->ContextData != AttoChar('\n')) && (*XMLContext->ContextData != AttoChar('\r')) && (*XMLContext->ContextData != AttoChar('='))) {
				if ((_AttoIsAlnum(*XMLContext->ContextData) == 0) && (*XMLContext->ContextData != AttoChar('-'))) {
					_AttoSprintf(XMLContext->ContextErrorText, "Parsing error: '%c' is not allowed as character of node name.", *XMLContext->ContextData);
					/*throw*/ longjmp(XMLContextInternals->ContextExceptionEnv, 1);
				}

				_AttoXMLContextNextExpectCharacter(XMLContext, XMLContextInternals);
				++NewXMLNode->NodeName.Length;
			}


		}
	}
}

inline void _AttoXMLContextParseNodeEnd(AttoXMLContext* const XMLContext, _AttoXMLContextInternals* const XMLContextInternals) {
	assert(0 && "Not implemented.");
}

inline bool AttoXMLContextParse(AttoXMLContext* const XMLContext, AttoChar const* const String) {
	_AttoXMLContextInternals XMLContextInternals;
	_AttoXMLContextInitEmpty(XMLContext);
	XMLContext->ContextData = String;
	/*try {*/ if (setjmp(XMLContextInternals.ContextExceptionEnv) == 0) {
		_AttoXMLContextParseHeader(XMLContext, &XMLContextInternals);
		for (; (*XMLContext->ContextData) != AttoChar('\0'); ++XMLContext->ContextData) {
			switch (*String) {
				case AttoChar('<'): {	// Begin of node character.
					switch (_AttoXMLContextNextExpectCharacter(XMLContext, &XMLContextInternals)) {
						case AttoChar('!'):
							_AttoXMLContextParseComment(XMLContext, &XMLContextInternals);
							break;
						case AttoChar('/'):
							_AttoXMLContextParseNodeEnd(XMLContext, &XMLContextInternals);
							break;
						default: 
							_AttoXMLContextParseNodeBegin(XMLContext, &XMLContextInternals);
							break;
					}
				} break;
			default:
				assert(0 && "Not implemented.");
				break;
			}
		}
		return true;
	/*} catch(...) {*/ } else {
		return false;
	/*}*/ }
}

#if ((defined(_MSC_VER)) && (defined(__cplusplus)))
#	pragma warning(pop)
#endif	// if ((defined(_MSC_VER)) && (defined(__cplusplus)))
