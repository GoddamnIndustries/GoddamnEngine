//////////////////////////////////////////////////////////////////////////
/// RePreprocessor.hh: GoddamnC++ preprocessor directives (re)parser interface. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.07.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>
#include <GoddamnEngine/Core/Text/String/String.hh>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.hh>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.hh>

#include <GoddamnReflector/Reflector.hh>

GD_NAMESPACE_BEGIN

	/// While parsing GoddamnC++ annotations we meet preprocessor directives. So we need to handle them.
	/// But be cannot just preprocess macros - there are lots of compiler/enviroment/target dependent code,
	/// so only real compiler with used in project flags may generate correct preprocessed code. But then
	/// we have to support different compilers and etc. 
	/// The common idea of this implementation is to store all conditional preprocessor directives 
	/// and also output them to generated code.
	/// @see "docs/RePreprocessor memory layout examples.txt" file for more detailed memory layout.

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
		};	// struct struct Block;
		UniquePtr<Block> TopBlock;
		Block* CurrentBlock = nullptr;

	public /*Class API*/:
		GDINT ~CPPRePreprocessorDefinitions() { }
		GDINL  CPPRePreprocessorDefinitions() : TopBlock(new Block()), CurrentBlock(self->TopBlock.GetPointer()) { }

		/// Enumerates all definitions inside containter.
		/// @param IterationPredicate iteration element callback predicate.
		template<typename IterationPredicateType>
		inline void ForEach(IterationPredicateType const& IterationPredicate) const
		{
			for (auto const Element : self->Elements)
				IterationPredicate(Element);
			for (auto const InnerBlock : self->InnerBlocks)
				InnerBlocks->ForEach(IterationPredicate);
		}

		/// Forces block to consider some prepending preprocessor directive.
		/// @param BaseParser Parser that provides low lever source parsing.
		GDINT bool ÑonsiderPreprocessorDirective(CPPBaseParser* const BaseParser);

		/// Appends definition to this block.
		/// @param Definition Definition that would be appended.
		GDINL void AppendDefinition(SharedPtr<CPPDefinition> const& Definition)
		{
			self->CurrentBlock->Elements.PushLast(Definition);
		}

#if (defined(GD_DEBUG))
		/// Prints parsed stuff to output.
		GDINT void DebugVislualizeBlock();
#endif	// if (defined(GD_DEBUG))
	};	// class CPPRePreprocessorDefinitions

GD_NAMESPACE_END
