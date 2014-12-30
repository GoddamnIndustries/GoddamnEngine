/// ==========================================================================================
/// RePreprocessor.h: GoddamnC++ preprocessor directives (re)parser interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.h>

#include <GoddamnReflector/Reflector.h>

GD_NAMESPACE_BEGIN

	/// While parsing Goddamn C++ annotations we meet preprocessor directives. So we need to handle them. 
	/// But be not just preprocessor the code - there are lots of compiler/environment/target-dependent branches, 
	/// so only real compiler, that used in project, could generate correct preprocessed code. In order to preserve 
	/// compatibility, reflectors parses all conditional preprocessor branches, and recreates them in the generated C++ code.
	/// @see "docs/Preprocessor memory layout examples.txt" file for more detailed memory layout.

	/// Represents internal memory layout for parsable code with preprocessor directoives inside.
	class CPPRePreprocessorDefinitions final
	{
	private /*Class types and members*/:
		struct Block final
		{
			String PreCondition;
			String PostCondition;
			Vector<SharedPtr<CPPDefinition>> Elements;
			Vector<SharedPtr<Block>> InnerBlocks;
			Block* ParentBlock = nullptr;
		};	// struct Block;
		UniquePtr<Block> TopBlock;
		Block* CurrentBlock = nullptr;

	public /*Class API*/:
		GDINT ~CPPRePreprocessorDefinitions() { }
		GDINL  CPPRePreprocessorDefinitions() : TopBlock(new Block()), CurrentBlock(this->TopBlock.GetPointer()) { }

		/// @name Preprocessor directoves parsing.
		/// @{

		/// Forces block to consider some prepending preprocessor directive.
		/// @param BaseParser Parser that provides low lever source parsing.
		GDINT bool ÑonsiderPreprocessorDirective(CPPBaseParser* const BaseParser);

		/// Forces block to consider all prepending preprocessor directive.
		/// @param BaseParser Parser that provides low lever source parsing.
		GDINL bool ÑonsiderPreprocessorDirectives(CPPBaseParser* const BaseParser)
		{
			while (this->ÑonsiderPreprocessorDirective(BaseParser));
			return true;
		}

#if (defined(GD_DEBUG))
		/// Prints parsed stuff to output.
		GDINT void DebugVislualizeBlock();
#endif	// if (defined(GD_DEBUG))

		/// @}

		/// @name Elements management.
		/// @{

		/// Enumerates all definitions inside containter.
		/// @param IterationPredicate iteration element callback predicate.
		template<typename IterationPredicateType>
		inline void ForEach(IterationPredicateType const& IterationPredicate) const
		{
			for (auto const Element : this->Elements) {
				IterationPredicate(Element);
			}

			for (auto const InnerBlock : this->InnerBlocks) {
				InnerBlocks->ForEach(IterationPredicate);
			}
		}

		/// Appends definition to this block.
		/// @param Definition Definition that would be appended.
		GDINL void AppendElement(SharedPtr<CPPDefinition> const& Definition)
		{
			this->CurrentBlock->Elements.PushLast(Definition);
		}

		/// @}
	};	// class CPPRePreprocessorDefinitions

GD_NAMESPACE_END
