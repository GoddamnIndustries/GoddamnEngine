/// ==========================================================================================
/// AttoXML.h: World`s fastest XML parser/analyzer interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 16.07.2014  - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef ATTO_XML_INCLUDED
#define ATTO_XML_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#if (defined(__cplusplus))
extern "C" {
#endif	// if (defined(__cplusplus))

#define ATTO_XML_INLINE inline

#if (defined(__cplusplus))
#	define ATTO_XML_NULL nullptr
#else	// if (defined(__cplusplus))
#	define ATTO_XML_NULL NULL
#endif	// if (defined(__cplusplus))

#if (defined(_UNICODE))
	typedef wchar_t AttoChar;
#else	// if (defined(_UNICODE))
	typedef char AttoChar;
#endif	// if (defined(_UNICODE))

	/// Represents a slice from some string.
	typedef struct AttoStringSlice {
		AttoChar const* Pointer;	///< Pointer from start of string data.
		short unsigned  Length;		///< Length of slice of a string.
	} AttoStringSlice;

	/// @brief Compares a slice and a standalone string.
	/// @param StringSlice Slice to compare.
	/// @param String   Standalone string to compare.
	/// @returns True if slice and string equals.
	ATTO_XML_INLINE bool AttoStringSliceEquals(AttoStringSlice const* const StringSlice, AttoChar const* const String);

	/// Represents single XML attribute.
	typedef struct AttoXMLAttribute {
		AttoStringSlice AttributeName;
		AttoStringSlice AttributeValue;
	} AttoXMLAttribute;

	/// Defines a limit for node attributes count.
#define ATTO_XML_MAX_ATTRIBUTES  4
	/// Defines a limit for node children count.
#define ATTO_XML_MAX_CHILD_NODES 8

	/// Represents a single XML node.
	typedef struct AttoXMLNode {
		AttoStringSlice  NodeName;	         ///< Slice for node name.
		AttoXMLAttribute NodeAttributes[ATTO_XML_MAX_ATTRIBUTES];	 ///< List of node attributes.
		short unsigned   NodeAttributesCount;	      ///< Number of attributes.
		AttoStringSlice  NodeInnerText;	        ///< Inner text of a node.
		short unsigned   NodeChildIndices[ATTO_XML_MAX_CHILD_NODES]; ///< Indices of child nodes in context list.
		short unsigned   NodeChildIndicesCount;	      ///< Number of child nodes this contains.
		short unsigned   NodeParent;								 ///< Index of parent node in context list.
	} AttoXMLNode;

	enum { _AttoXMLNodeSize = sizeof(AttoXMLNode) };

#define ATTO_XML_MAX_CONTEXT_NODES   128
#define ATTO_XML_MAX_CONTEXT_ERROR_TEXT 256
	typedef struct AttoXMLContext {
		union {
			AttoXMLNode ContextNodes[ATTO_XML_MAX_CONTEXT_NODES];	      ///< List of nodes. Root node is located at index 0.
			AttoChar ContextErrorText[ATTO_XML_MAX_CONTEXT_ERROR_TEXT];	    ///< Text of error message if parsing failed.
		};
	} AttoXMLContext;
	
	enum { _AttoXMLContextSize = sizeof(AttoXMLContext) };

	/// @brief Parses XML string.
	/// Parses XML string into simple context. This parser does skippes comments, XML header and parses special characters like "&amp;".
	/// @param XMLContext Pointer to context into which data would be parsed or error description stored.
	/// @param String  Source XML string.
	/// @returns True if parseing finished successfully, false otherwise. If returned false, than error descrption is also stored inside context in ContextErrorText field.
	ATTO_XML_INLINE bool AttoXMLContextParse(AttoXMLContext* const XMLContext, AttoChar const* const String);

	/// @brief Pointer to procedure that writes data to output.
	/// @param String C String that requires to be written.
	/// @param Length Length of string to write.
	/// @param Argument Pointer to some useful object.
	/// @returns True if written successfully.
	typedef bool(*AttoXMLContextWriteProc)(AttoChar const* const String, short unsigned const Length, void const* const Argument);

	/// @brief Writes XML context using custom write procedure.
	/// @param XMLContext Pointer to context that would be written.
	/// @param WriteProc  Pointer to procedure that writes data to output.
	/// @param Argument   Pointer to some useful object.
	/// @returns True if written successfully.
	ATTO_XML_INLINE bool AttoXMLContextWrite(AttoXMLContext const* const XMLContext, AttoXMLContextWriteProc const WriteProc, void const* const Argument);

#if (defined(__cplusplus))
}
#endif	// if (defined(__cplusplus))

#include "AttoXML.inl"
#endif	// ifndef ATTO_XML_INCLUDED
