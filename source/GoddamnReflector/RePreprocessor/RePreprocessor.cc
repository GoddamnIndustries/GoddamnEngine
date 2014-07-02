//////////////////////////////////////////////////////////////////////////
/// RePreprocessor.hh: GoddamnC++ preprocessor directives (re)parser implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 01.07.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnReflector/RePreprocessor/RePreprocessor.hh>

#if (defined(GD_DEBUG))
#	include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#endif	// if (defined(GD_DEBUG))

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
		if (!BaseParser->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, StreamedLexerDefaultOptions::GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PREPROCESSOR))
			return false;

		// Reading all directive into a string.
		if (!BaseParser->ExpectNextLexem()) return false;
		String PreprocessorDirective = BaseParser->GetCurrentLexem().GetRawData();
		for (;;)
		{	
			Char const Character = BaseParser->GetInputStream()->Read<Char>();
			if ((Character == Char('\n')) || (Character == Char('\r')))
				break;
			PreprocessorDirective.PushLast(Character);
		};

		if (!BaseParser->ExpectNextLexem()) return false;

		// '#endif' directive parsing.
		if (strncmp(PreprocessorDirective.CStr(), "endif", sizeof("endif") - 1) == 0)
		{	// Leaving this block.
			self->CurrentBlock->PostCondition = Move(PreprocessorDirective);
			if ((self->CurrentBlock = self->CurrentBlock->ParentBlock) == nullptr)
			{	// Unexpected '#endif' directive. No corresponding '#if' was not found.
				CPPBaseParserErrorDesc static const UnexpectedEndifDirectiveError("unexpected '#endif' directive. No corresponding '#if' was not found.");
				BaseParser->RaiseError(UnexpectedEndifDirectiveError);
				BaseParser->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

				return false;
			}

			return true;
		} 

		// '#if*', '#elif*' and '#else' directive parsing.
		else if ((!self->CurrentBlock->Elements.IsEmpty()) || (!self->CurrentBlock->PreCondition.IsEmpty()))
		{	// This block already has condition. Now lets see what we can do:
			if (strncmp(PreprocessorDirective.CStr(), "if", sizeof("if") - 1) == 0)
			{	// Creating a new block (in scope of current).
				self->CurrentBlock->InnerBlocks.PushLast(SharedPtr<Block>(new Block()));
				
				Block* const NewBlock = self->CurrentBlock->InnerBlocks.GetLastElement().GetPointer();
				NewBlock->ParentBlock = self->CurrentBlock;
				NewBlock->PreCondition = Move(PreprocessorDirective);
				
				self->CurrentBlock = NewBlock;
				return true;
			}
			else if ((strncmp(PreprocessorDirective.CStr(), "else", sizeof("else") - 1) == 0) || (strncmp(PreprocessorDirective.CStr(), "elif", sizeof("elif") - 1) == 0))
			{	// Creating a new block (sibling current).
				if (self->TopBlock == self->CurrentBlock)
				{	// We are in top block. We need to wrap it with other top.
					Block* const OldTopBlock = self->TopBlock.Release();
					self->CurrentBlock = OldTopBlock;
					self->TopBlock.Reset(new Block());
					self->TopBlock->InnerBlocks.PushLast(SharedPtr<Block>(OldTopBlock));
					OldTopBlock->ParentBlock = self->TopBlock.GetPointer();
				}

				self->CurrentBlock->ParentBlock->InnerBlocks.PushLast(SharedPtr<Block>(new Block()));
				
				Block* const NewBlock = self->CurrentBlock->ParentBlock->InnerBlocks.GetLastElement().GetPointer();
				NewBlock->ParentBlock = self->CurrentBlock->ParentBlock;
				NewBlock->PreCondition = Move(PreprocessorDirective);
				
				self->CurrentBlock = NewBlock;
				return true;
			}
		}
		else
		{	// Current block has empty pre-condition. Only '#if' is valid.
			if ((strncmp(PreprocessorDirective.CStr(), "else", sizeof("else") - 1) == 0) || (strncmp(PreprocessorDirective.CStr(), "elif", sizeof("elif") - 1) == 0))
			{	// Unexpected '#***' directive. No corresponding '#if' was not found.
				CPPBaseParserErrorDesc static const UnexpectedDirectiveError("unexpected '#%s' directive. No corresponding '#if' was not found.");
				BaseParser->RaiseError(UnexpectedDirectiveError, PreprocessorDirective.CStr());
				BaseParser->RaiseExceptionWithCode(GD_CPP_REFLECTOR_EXCEPTION_SYNTAX);

				return false;
			}
			else if (strncmp(PreprocessorDirective.CStr(), "if", sizeof("if") - 1) == 0)
			{
				self->CurrentBlock->PreCondition = Move(PreprocessorDirective);
				return true;
			}
		}

		// Pushing unkown directive back.
		BaseParser->GetInputStream()->Seek(-static_cast<ptrdiff_t>(PreprocessorDirective.GetSize()), GD_SEEK_ORIGIN_CURRENT);
		return true;
	}

#if (defined(GD_DEBUG))
	/// Prints parsed stuff to output.
	void CPPRePreprocessorDefinitions::DebugVislualizeBlock()
	{
		class CPPRePreprocessorDefinitionsWriter final
		{
		private:
			size_t IdentsCount = SIZE_MAX;
		public:
			StringBuilder Builder;

		public:
			GDINT StringBuilder& WriteBlock(Block const* SomeBlock)
			{
				++self->IdentsCount;
				String const Idents(nullptr, self->IdentsCount, Char('\t'));
				Builder.AppendFormat("\n%sBlock <Level %u> {", Idents.CStr(), self->IdentsCount);
				Builder.AppendFormat("\n%s\tPreCondition : %s", Idents.CStr(), (!SomeBlock->PreCondition.IsEmpty() ? String::Format(R"("%s")", SomeBlock->PreCondition.CStr()).CStr() : "None"));
				Builder.AppendFormat("\n%s\tPostCondition : %s", Idents.CStr(), (!SomeBlock->PostCondition.IsEmpty() ? String::Format(R"("%s")", SomeBlock->PostCondition.CStr()).CStr() : "None"));
				Builder.AppendFormat("\n%s\tElements <Count %u> [", Idents.CStr(), SomeBlock->Elements.GetSize());
				for (auto const Element : SomeBlock->Elements)
					Builder.AppendFormat("\n%s\t\t\"(0x%x)\",", Idents.CStr(), Element.GetPointer());
				Builder.AppendFormat("\n%s\t]", Idents.CStr());
				for (auto const InnerBlock : SomeBlock->InnerBlocks)
					self->WriteBlock(InnerBlock.GetPointer());
				Builder.AppendFormat("\n%s}", Idents.CStr());
				--self->IdentsCount;

				return self->Builder;
			}
		};	// class CPPRePreprocessorDefinitionsWriter
		printf("%s", CPPRePreprocessorDefinitionsWriter().WriteBlock(self->TopBlock.GetPointer()).GetPointer());
	}
#endif	// if (defined(GD_DEBUG))

GD_NAMESPACE_END
