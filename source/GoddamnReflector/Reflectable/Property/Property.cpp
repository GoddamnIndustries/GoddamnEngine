/// ==========================================================================================
/// Property.cpp: GoddamnC++ reflectable object's property implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 11.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/Reflectable/Property/Property.h>
#include <GoddamnReflector/Reflectable/Reflectable.h>

GD_NAMESPACE_BEGIN

	/// Parses "$GD_PROPERTY(...)" annotation.
	class CPPReflectablePropertyParser final : public CPPAnnotationParser
	{
	public /*Class members & constructor / destructor*/:
		CPPReflectable* const CurrentReflectable;
		SharedPtr<CPPReflectableProperty> CurrentProperty;

		GDINL explicit CPPReflectablePropertyParser(handle const Args) : CPPAnnotationParser(Args), CurrentReflectable(reinterpret_cast<CPPReflectable*>(Args)) { }
		GDINL virtual ~CPPReflectablePropertyParser() { }

	protected /*Class API*/:
		/// @see CPPAnnotationParser::SpawnParamParser
		GDINT virtual UniquePtr<CPPAnnotationParamParser> SpawnParamParser(String const& ParamName) override final;
		/// @see CPPAnnotationParser::ParseAnnotation
		GDINT virtual void ParseAnnotation(CPPBaseParser* const BaseParser) override final;
	};	// class CPPReflectablePropertyAnnotationParser

	/// Parses "$GD_PROPERTY(...)" annotation`s "ReadOnly" argument.
	class CPPReflectablePropertyReadOnlyParamParser final : public CPPAnnotationParamParser
	{
	public /*Class API*/:
		GDINL explicit CPPReflectablePropertyReadOnlyParamParser(handle const Args) : CPPAnnotationParamParser(Args) { }
		GDINL virtual ~CPPReflectablePropertyReadOnlyParamParser() { }
		/// @see CPPAnnotationParamParser::ParseArgument
		GDINT virtual void ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const AnnotationParser, String const& ParamValue) override final;
	};	// class CPPEnumerationParamTypeParser

	/// Parses "$GD_PROPERTY(...)" annotation`s "DefinedAs" argument.
	class CPPReflectablePropertyDefinedAsParamParser final : public CPPAnnotationParamParser
	{
	public /*Class API*/:
		GDINL explicit CPPReflectablePropertyDefinedAsParamParser(handle const Args) : CPPAnnotationParamParser(Args) { }
		GDINL virtual ~CPPReflectablePropertyDefinedAsParamParser() { }
		/// @see CPPAnnotationParamParser::ParseArgument
		GDINT virtual void ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const AnnotationParser, String const& ParamValue) override final;
	};	// class CPPEnumerationParamTypeParser

	/// Parses "$GD_PROPERTY(...)" annotation`s "AlsoHas" argument.
	class CPPReflectablePropertyAlsoHasParamParser final : public CPPAnnotationParamParser
	{
	public /*Class API*/:
		GDINL explicit CPPReflectablePropertyAlsoHasParamParser(handle const Args) : CPPAnnotationParamParser(Args) { }
		GDINL virtual ~CPPReflectablePropertyAlsoHasParamParser() { }
		/// @see CPPAnnotationParamParser::ParseArgument
		GDINT virtual void ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const AnnotationParser, String const& ParamValue) override final;
	};	// class CPPEnumerationParamTypeParser

	/// ==========================================================================================
	/// CPPReflectablePropertyAnnotationParser class.
	/// Parses "$GD_PROPERTY(...)" annotation.
	/// ==========================================================================================

	/// Spanwer node for this type.
	CPPAnnotationParserSpawner::Node<CPPReflectablePropertyParser> static const CPPReflectablePropertyParserSpawnerNode("$GD_PROPERTY");

	/// ------------------------------------------------------------------------------------------
	/// Protected class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	/// @see CPPAnnotationParser::SpawnParamParser
	UniquePtr<CPPAnnotationParamParser> CPPReflectablePropertyParser::SpawnParamParser(String const& ParamName)
	{
		CPPAnnotationParamParserSpawner static Spawner; {
			CPPAnnotationParamParserSpawner::Node<CPPReflectablePropertyReadOnlyParamParser > static const ReadOnlyParamSpawner (Spawner, "ReadOnly");
			CPPAnnotationParamParserSpawner::Node<CPPReflectablePropertyDefinedAsParamParser> static const DefinedAsParamSpawner(Spawner, "DefinedAs");
			CPPAnnotationParamParserSpawner::Node<CPPReflectablePropertyAlsoHasParamParser  > static const AlsoHasParamSpawner  (Spawner, "AlsoHas");
		}

		return Spawner.SpawnAnnotationParamParser(ParamName, nullptr);
	}

	/// @see CPPAnnotationParser::ParseAnnotation
	void CPPReflectablePropertyParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{	/// Initialization.
		this->CurrentProperty = new CPPReflectableProperty();
		this->CPPAnnotationParser::ParseAnnotation(BaseParser);
	}

	/// ==========================================================================================
	/// CPPReflectablePropertyReadOnlyParamParser class.
	/// Parses "$GD_PROPERTY(...)" annotation`s "ReadOnly" argument.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	void CPPReflectablePropertyReadOnlyParamParser::ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const SomeAnnotationParser, String const& ParamValue)
	{
	}

	/// ==========================================================================================
	/// CPPReflectablePropertyDefinedAsParamParser class.
	/// Parses "$GD_PROPERTY(...)" annotation`s "DefinedAs" argument.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	void CPPReflectablePropertyDefinedAsParamParser::ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const SomeAnnotationParser, String const& ParamValue)
	{
	}

	/// ==========================================================================================
	/// CPPReflectablePropertyDefinedAsParamParser class.
	/// Parses "$GD_PROPERTY(...)" annotation`s "AlsoHas" argument.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Constructors / Destructor):
	/// ------------------------------------------------------------------------------------------

	void CPPReflectablePropertyAlsoHasParamParser::ParseArgument(CPPBaseParser* const BaseParser, CPPAnnotationParser* const SomeAnnotationParser, String const& ParamValue)
	{
	}

GD_NAMESPACE_END
