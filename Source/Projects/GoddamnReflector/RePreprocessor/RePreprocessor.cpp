/// ==========================================================================================
/// RePreprocessor.h: GoddamnC++ preprocessor directives (re)parser implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnReflector/RePreprocessor/RePreprocessor.h>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.h>
#include <GoddamnEngine/Core/Reflection/MethodInformation/MethodInformation.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// CPPRePreprocessorDefinitions class.
	/// Represents internal memory layout for parsable code with macros inside.
	/// ==========================================================================================

	/// ------------------------------------------------------------------------------------------
	/// Public class API:
	/// ------------------------------------------------------------------------------------------

	/// Forces block to consider some prepending preprocessor directive.
	/// @param PrependingDirective Preprocessor directive that prepends the definition.
	bool CPPRePreprocessorDefinitions::ÑonsiderPreprocessorDirective(CPPBaseParser* const BaseParser)
	{	// Checking if this actually is a directive.
		if (!BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, StreamedLexerDefaultOptions::GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR)) {
			return false;
		}

		// Reading all directive into a string.
		BaseParser->ExpectNextLexem();
		StringBuilder PreprocessorDirectiveBuilder;
		PreprocessorDirectiveBuilder.Append(BaseParser->GetCurrentLexem().GetRawData());
		for (;;) {	
			Char const Character = BaseParser->GetLexer()->GetNextCharacter();
			if ((Character == Char('\n')) || (Character == Char('\r'))) {
				break;
			}

			PreprocessorDirectiveBuilder.Append(Character);
		}

		String const PreprocessorDirective = PreprocessorDirectiveBuilder.ToString();
		BaseParser->ExpectNextLexem();

		// '#endif' directive parsing.
		if (std::strncmp(PreprocessorDirective.CStr(), "endif", sizeof("endif") - 1) == 0) {	// Leaving this block.
			this->CurrentBlock->PostCondition = Move(PreprocessorDirective);
			if ((this->CurrentBlock = this->CurrentBlock->ParentBlock) == nullptr) {	// Unexpected '#endif' directive. No corresponding '#if' was not found.
				CPPBaseParserErrorDesc static const UnexpectedEndifDirectiveError("unexpected '#endif' directive. No corresponding '#if' was not found.");
				throw CPPParsingException(UnexpectedEndifDirectiveError.ToString(&BaseParser->GetCurrentLexem()));
			}
		} else if ((!this->CurrentBlock->Elements.IsEmpty()) || (!this->CurrentBlock->PreCondition.IsEmpty()) || (this->TopBlock == this->CurrentBlock)) {
			// '#if*', '#elif*' and '#else' directive parsing.
			// This block already has condition. Now lets see what we can do:
			if (std::strncmp(PreprocessorDirective.CStr(), "if", sizeof("if") - 1) == 0) {	// Creating a new block (in scope of current).
				this->CurrentBlock->InnerBlocks.PushLast(SharedPtr<Block>(new Block()));
				
				Block* const NewBlock = this->CurrentBlock->InnerBlocks.GetLastElement().GetPointer();
				NewBlock->ParentBlock = this->CurrentBlock;
				NewBlock->PreCondition = Move(PreprocessorDirective);
				
				this->CurrentBlock = NewBlock;
			} else if ((std::strncmp(PreprocessorDirective.CStr(), "else", sizeof("else") - 1) == 0) 
				    || (std::strncmp(PreprocessorDirective.CStr(), "elif", sizeof("elif") - 1) == 0)) {
				// Creating a new block (sibling current).
				if (this->TopBlock == this->CurrentBlock) {	// We are in top block. We need to wrap it with other top.
					Block* const OldTopBlock = this->TopBlock.Release();
					this->CurrentBlock = OldTopBlock;
					this->TopBlock.Reset(new Block());
					this->TopBlock->InnerBlocks.PushLast(SharedPtr<Block>(OldTopBlock));
					OldTopBlock->ParentBlock = this->TopBlock.GetPointer();
				}

				this->CurrentBlock->ParentBlock->InnerBlocks.PushLast(SharedPtr<Block>(new Block()));
				
				Block* const NewBlock = this->CurrentBlock->ParentBlock->InnerBlocks.GetLastElement().GetPointer();
				NewBlock->ParentBlock = this->CurrentBlock->ParentBlock;
				NewBlock->PreCondition = Move(PreprocessorDirective);
				
				this->CurrentBlock = NewBlock;
			}
		} else {	// Current block has empty pre-condition. Only '#if' is valid.
			if (   (std::strncmp(PreprocessorDirective.CStr(), "else", sizeof("else") - 1) == 0) 
				|| (std::strncmp(PreprocessorDirective.CStr(), "elif", sizeof("elif") - 1) == 0)) {
				// Unexpected '#***' directive. No corresponding '#if' was not found.
				CPPBaseParserErrorDesc static const UnexpectedDirectiveError("unexpected '#%s' directive. No corresponding '#if' was not found.");
				throw CPPParsingException(UnexpectedDirectiveError.ToString(&BaseParser->GetCurrentLexem()));
			} else if (std::strncmp(PreprocessorDirective.CStr(), "if", sizeof("if") - 1) == 0) {
				this->CurrentBlock->PreCondition = Move(PreprocessorDirective);
			} else {
				// Pushing unkown directive back.
				for (auto const Character : PreprocessorDirective) {
					BaseParser->GetLexer()->RevertCharacter(Character);
				}
			}
		}
		
		return true;
	}

#if (defined(GD_DEBUG))
	/// Prints parsed stuff to output.
	void CPPRePreprocessorDefinitions::DebugVislualizeBlock()
	{
		struct RecursiveBlockWriter final {
			size_t IdentsCount = SIZE_MAX;
			void RecursivelyWriteBlock(Block const* SomeBlock) {
				++this->IdentsCount;
				String const Idents(this->IdentsCount, Char('\t'));
				GD_REFLECTOR_LOG("\n%s/*", Idents.CStr());
				GD_REFLECTOR_LOG("\n%sBlock <Level %u> {", Idents.CStr(), this->IdentsCount);
				GD_REFLECTOR_LOG("\n%s\tPreCondition : %s", Idents.CStr(), (!SomeBlock->PreCondition.IsEmpty() ? String::Format(R"("%s")", SomeBlock->PreCondition.CStr()).CStr() : "None"));
				GD_REFLECTOR_LOG("\n%s\tPostCondition : %s", Idents.CStr(), (!SomeBlock->PostCondition.IsEmpty() ? String::Format(R"("%s")", SomeBlock->PostCondition.CStr()).CStr() : "None"));

				GD_REFLECTOR_LOG("\n%s\tElements <Count %u> [", Idents.CStr(), SomeBlock->Elements.GetSize());
				for (auto const Element : SomeBlock->Elements) {
					GD_REFLECTOR_LOG("\n%s\t\t\"(0x%x)\",", Idents.CStr(), reinterpret_cast<size_t>(Element.GetPointer()));
				}
				GD_REFLECTOR_LOG("\n%s\t]", Idents.CStr());

				for (auto const InnerBlock : SomeBlock->InnerBlocks) {
					this->RecursivelyWriteBlock(InnerBlock.GetPointer());
				}

				GD_REFLECTOR_LOG("\n%s}", Idents.CStr());
				GD_REFLECTOR_LOG("\n%s*/", Idents.CStr());
				--IdentsCount;
			}
		};
		RecursiveBlockWriter().RecursivelyWriteBlock(this->TopBlock.GetPointer());
	}
#endif	// if (defined(GD_DEBUG))

GD_NAMESPACE_END
