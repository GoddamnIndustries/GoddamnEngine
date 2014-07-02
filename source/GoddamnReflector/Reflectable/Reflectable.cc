//////////////////////////////////////////////////////////////////////////
/// Reflectable.cc: GoddamnC++ reflectable type implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 02.07.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnReflector/Reflectable/Reflectable.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

GD_NAMESPACE_BEGIN

	LockFreeList<SharedPtr<CPPReflectable>> static CPPReflectablesListImpl;
	LockFreeList<SharedPtr<CPPReflectable>> const& CPPReflectablesList = CPPReflectablesListImpl;

	/// Parses "$GD_REFLECTABLE(...)" annotation.
	class CPPReflectableParser final : public CPPAnnotationParser
	{
	public /*Class members & constructor / destructor*/:
		SharedPtr<CPPReflectable> CurrentReflectable;

		GDINL explicit CPPReflectableParser(CPPAnnotationCtorArgs const* const Args) : CPPAnnotationParser(Args) { }
		GDINL virtual ~CPPReflectableParser() { }

		/// @see CPPAnnotationParser::SpawnParamParser
		GDINT virtual UniquePtr<CPPAnnotationParamParser> SpawnParamParser(String const& ParamName) override final;
		/// @see CPPAnnotationParser::ParseAnnotation
		GDINT virtual bool ParseAnnotation(CPPBaseParser* const BaseParser) override final;
	};	// class CPPEnumerationParser  

	/// ==========================================================================================
	/// CPPReflectableParser class.
	/// Parses "$GD_REFLECTABLE(...)" annotation.
	/// ==========================================================================================

	/// Spanwer node for this type.
	CPPAnnotationParserSpawner::Node<CPPReflectableParser> static const CPPReflectableParserSpawnerNode("$GD_REFLECTABLE");

	/// ------------------------------------------------------------------------------------------
	/// Public class API (Implemetations):
	/// ------------------------------------------------------------------------------------------

	/// @see CPPAnnotationParser::SpawnParamParser
	UniquePtr<CPPAnnotationParamParser> CPPReflectableParser::SpawnParamParser(String const& ParamName)
	{
		return UniquePtr<CPPAnnotationParamParser>(nullptr);
	}

	/// @see CPPAnnotationParser::ParseAnnotation
	bool CPPReflectableParser::ParseAnnotation(CPPBaseParser* const BaseParser)
	{
		self->CurrentReflectable = new CPPReflectable();
		if (!self->CPPAnnotationParser::ParseAnnotation(BaseParser)) return false;
		self->CurrentReflectable->SetDefaultsForUnknowns();

		CPPReflectablesListImpl.PushLast(self->CurrentReflectable);
		self->CurrentReflectable = nullptr;
		return true;
	}

GD_NAMESPACE_END
