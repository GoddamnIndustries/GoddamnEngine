/// ==========================================================================================
/// CompilerBase.h - compilers common interface
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 27.03.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_COMPILERS_COMPILER_BASE
#define GD_CORE_COMPILERS_COMPILER_BASE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Pair.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Diagnostics/Exception/Exception.h>

GD_NAMESPACE_BEGIN

	/// Describes all supported lexem types.
	enum class LexemContentType : UInt8
	{
		Identifier,			///< Lexem contains identifier data (string data type).
		Keyword,			///< Lexem contains keyword data (string data type, has ID).
		Operator,			///< Lexem contains operator data (string data type, has ID).
		Comment,			///< Lexem contains comment data (string data type).
		ConstantString,		///< Lexem contains string constant (string data type).
		ConstantCharacter,	///< Lexem contains character constant (character data type).
		ConstantInteger,	///< Lexem contains integer constant (integer data type).
		ConstantFloat,		///< Lexem contains floating-point constant (float data type).
		Unknown,			///< Lexem contains unknown data. (Internal usage only).
		EndOfStream
	};	// enum LexemContentType
	size_t static const LexemContentTypesCount = static_cast<size_t>(LexemContentType::Unknown);

	/// Returns human readable string for lexem content type
	GDAPI extern Str LexemContentTypeToString(LexemContentType const ContentType);
	
	typedef UInt16 StreamedLexerID;
	typedef StreamedLexerID StreamedLexerKeyword;
	typedef StreamedLexerID StreamedLexerOperator;
	typedef Pair<StreamedLexerKeyword, String> StreamedLexerKeywordDecl;
	typedef Pair<StreamedLexerOperator, String> StreamedLexerOperatorDecl;
	typedef Vector<StreamedLexerKeywordDecl> StreamedLexerKeywordsList;
	typedef Vector<StreamedLexerOperatorDecl> StreamedLexerOperatorsList;

	/// @brief Describes information about one lexem.
	struct Lexem final
	{
	private:
		friend class StreamedLexerImpl;
		friend class BasicStreamedLexerImpl;
		GD_CLASS_UNASSIGNABLE(Lexem);
		GD_CLASS_UNSWAPPABLE (Lexem);
		GD_CLASS_UNCOPIABLE  (Lexem);
		GD_CLASS_UNMOVABLE   (Lexem);

		size_t Line = 0;
		size_t Symbol = 0;
		LexemContentType ContentType = LexemContentType::Unknown;
		String RawData;
		union {
			CharAnsi  ProcessedDataCharacter;
			UInt64    ProcessedDataInteger;
			Float64   ProcessedDataFloat;
			StreamedLexerID ProcessedDataID;
			UInt8     Padding[8];
		};	

		GDINL void ResetLexem()
		{
			this->Line = 0;
			this->Symbol = 0;
			this->ContentType = LexemContentType::Unknown;
			this->RawData = "";
			memset(&this->Padding[0], 0, sizeof(this->Padding));
		}

	public:
		GDINL  Lexem() { }
		GDINL ~Lexem() { this->ResetLexem(); }
		
		/// Returns position of this lexem in input stream.
		GDINL size_t GetLine()      const { return this->Line; }
		GDINL size_t GetSymbol()    const { return this->Symbol; }

		/// Returns type of content that is located inside lexem.
		GDINL LexemContentType GetContentType()  const { return this->ContentType; }

		/// Returns raw lexem data that is 2015ed as string.
		GDINL String const& GetRawData()   const { return this->RawData; }

		/// Returns character data, located inside this lexem if this lexem is character contant.
		GDINL CharAnsi GetProcessedDataChar()  const 
		{
			GD_DEBUG_ASSERT(this->GetContentType() == LexemContentType::ConstantCharacter, "Lexem`s content type is not character constant");
			return this->ProcessedDataCharacter; 
		}
		
		/// Returns integer data, located inside this lexem if this lexem is integer contant.
		GDINL UInt64 GetProcessedDataInt()   const 
		{ 
			GD_DEBUG_ASSERT(this->GetContentType() == LexemContentType::ConstantInteger, "Lexem`s content type is not integer constant");
			return this->ProcessedDataInteger; 
		}
		
		/// Returns float data, located inside this lexem if this lexem is float contant.
		GDINL Float64 GetProcessedDataFloat() const 
		{ 
			GD_DEBUG_ASSERT(this->GetContentType() == LexemContentType::ConstantFloat, "Lexem`s content type is not float constant");
			return this->ProcessedDataFloat; 
		}
		
		/// Returns some ID data, located inside this lexem if this lexem can has ID.
		GDINL StreamedLexerID GetProcessedDataID() const 
		{ 
			if ((this->GetContentType() != LexemContentType::Keyword) && (this->GetContentType() != LexemContentType::Operator)) {
				return 0;
			}

			return this->ProcessedDataID;
		}
	};	// struct Lexem

GD_NAMESPACE_END

#endif	
