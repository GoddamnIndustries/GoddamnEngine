// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLParser.cpp
 * File HLSL parser implementation for the shader cross compiler.
 */
#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_DESKTOP

#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLParser.h>
#include <GoddamnEngine/Engine/Interface/Graphics/GraphicsHLSLCrossCompiler/HLSLTokenizer.h>

#include <GoddamnEngine/Core/Containers/StringBuilder.h>
#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>
#include <GoddamnEngine/Core/UniquePtr.h>

GD_NAMESPACE_BEGIN

	typedef ToolchainException HLSLParserErrorException;

	static StreamedLexerOptions const& GetDefaultOptionsForHLSL();
	static HLSLSemanticType HLSLSemanticTypeFromString(String const& Semantic);

	// ------------------------------------------------------------------------------------------
	//! Checks, whether the specified keyword is one of the type.
	//! @param Keyword Keyword to test.
	//! @param Type Type of the keyword.
#define GD_HLSL_IS_KEYWORD_OF_TYPE(Keyword, Type) (((Keyword) > HLSLKeywords::Keyword_##Type##_BEGIN__) && ((Keyword) < HLSLKeywords::Keyword_##Type##_END__))
	namespace HLSLKeywords
	{
		enum TValue : StreamedLexerKeyword
		{
#if !GD_DOCUMENTATION
			KeywordUNKNOWN = 0,
			Keyword_DEFINING_BEGIN__,
#endif	// if !GD_DOCUMENTATION
			KeywordStruct,
			KeywordCbuffer,
			KeywordTypedef,
#if !GD_DOCUMENTATION
			Keyword_DEFINING_END__,
			Keyword_TYPE_MODIFGDR_BEGIN__,
#endif	// if !GD_DOCUMENTATION
			KeywordConst,
			RowMajor,
			ColumnMajor,
#if !GD_DOCUMENTATION
			Keyword_TYPE_MODIFGDR_END__,
			Keyword_ARGUMENTS_BEGIN__,
#endif	// if !GD_DOCUMENTATION
			In,
			Out,
			InOut,
#if !GD_DOCUMENTATION
			Keyword_ARGUMENTS_END__,
			Keyword_OTHER_BEGIN__,
#endif	// if !GD_DOCUMENTATION
			KeywordRegister,
			KeywordPackOffset,
#if !GD_DOCUMENTATION
			Keyword_OTHER_END__,
#endif	// if !GD_DOCUMENTATION
		//	Keyword,
		}; // enum ValueType
	}; // namespace HLSLKeywords

	namespace HLSLOperators
	{
		using CPPOperators::OperatorIndexBegin;
		using CPPOperators::OperatorIndexEnd;
		using CPPOperators::OperatorScopeBegin;
		using CPPOperators::OperatorScopeEnd;
		using CPPOperators::OperatorParamsBegin;
		using CPPOperators::OperatorParamsEnd;
		using CPPOperators::OperatorTemplateBegin;
		using CPPOperators::OperatorTemplateEnd;
		using CPPOperators::OperatorColon;
		using CPPOperators::OperatorSemicolon;
		using CPPOperators::OperatorComma;
	}; // namespace HLSLOperators

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSL supported types information RAII objects.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! A global object that contains all HLSL supported built-in types.
	//! @{
	struct HLSLSuperGlobalScopeType final : public HLSLScope
	{
		GDINT ~HLSLSuperGlobalScopeType()
		{
			InnerDefinitions.Clear();
		}
	} static HLSLSuperGlobalScope;
#define GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE() \
	do { \
		HLSLSuperGlobalScope.InnerDefinitions.InsertLast(this); \
	} while(false)
	//! @}

	// ------------------------------------------------------------------------------------------
	//! HLSL Basic types.
	//! @{
#define GD_HLSL_DEFINE_BASIC_TYPE(_Name, _HLSLName, _Class) \
	struct _Name final : public HLSLType { \
		GDINL _Name() : HLSLType(_Class) { \
			Name = _HLSLName; \
			GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE(); \
		} \
	} static const _Name
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLVoid        , "void"        , GD_HLSL_TYPE_CLASS_VOID );
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLTexture2D   , "Texture2D"   , GD_HLSL_TYPE_CLASS_TEXTURE2D );
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLTextureCube , "TextureCube" , GD_HLSL_TYPE_CLASS_TEXTURECUBE);
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLTexture3D   , "Texture3D"   , GD_HLSL_TYPE_CLASS_TEXTURE3D);
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLSamplerState, "SamplerState", GD_HLSL_TYPE_CLASS_SAMPLER );
#undef GD_HLSL_DEFINE_BASIC_TYPE
	//! @}

	// ------------------------------------------------------------------------------------------
	//! HLSL Scalar types.
	//! @{
#define GD_HLSL_DEFINE_SCALAR_TYPE(_Name, _DataType) \
	struct _Name final : public HLSLScalarType { \
		GDINL _Name() { \
			Name = #_DataType; \
			DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE(); \
		} \
	} static const _Name
	GD_HLSL_DEFINE_SCALAR_TYPE(HLSLBool   , bool  );
	GD_HLSL_DEFINE_SCALAR_TYPE(HLSLInt    , int   );
	GD_HLSL_DEFINE_SCALAR_TYPE(HLSLUInt   , uint  );
	GD_HLSL_DEFINE_SCALAR_TYPE(HLSLFloat  , float );
	GD_HLSL_DEFINE_SCALAR_TYPE(HLSLDouble , double);
	GD_HLSL_DEFINE_SCALAR_TYPE(HLSLDword  , dword );
#undef GD_HLSL_DEFINE_SCALAR_TYPE
	//! @}

	// ------------------------------------------------------------------------------------------
	//! HLSL Vector types.
	//! @{
#define GD_HLSL_DEFINE_VECTOR_TYPE(_Name, _DataType, _ComponentsNumber) \
	struct _Name##_ComponentsNumber final : public HLSLVectorType{ \
		GDINL _Name##_ComponentsNumber() { \
			Name             = #_DataType#_ComponentsNumber; \
			DataType         = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			ComponentsNumber = _ComponentsNumber; \
			GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE(); \
		} \
	} static const _Name##_ComponentsNumber
#define GD_HLSL_DEFINE_VECTOR_TYPES(_Name, _DataType) \
	GD_HLSL_DEFINE_VECTOR_TYPE(_Name, _DataType, 4); \
	GD_HLSL_DEFINE_VECTOR_TYPE(_Name, _DataType, 3); \
	GD_HLSL_DEFINE_VECTOR_TYPE(_Name, _DataType, 2) 
	GD_HLSL_DEFINE_VECTOR_TYPES(HLSLBool   , bool  );
	GD_HLSL_DEFINE_VECTOR_TYPES(HLSLInt    , int   );
	GD_HLSL_DEFINE_VECTOR_TYPES(HLSLUInt   , uint  );
	GD_HLSL_DEFINE_VECTOR_TYPES(HLSLFloat  , float );
	GD_HLSL_DEFINE_VECTOR_TYPES(HLSLDouble , double);
#undef GD_HLSL_DEFINE_VECTOR_TYPES
#undef GD_HLSL_DEFINE_VECTOR_TYPE
	//! @}

	// ------------------------------------------------------------------------------------------
	//! Matrix types. 
	//! @{
#define GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, _RowsNumber, _ColumnsNumber) \
	struct _Name##_RowsNumber##x##_ColumnsNumber final : public HLSLMatrixType{ \
		GDINL _Name##_RowsNumber##x##_ColumnsNumber() { \
			Name = #_DataType#_RowsNumber"x"#_ColumnsNumber; \
			DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			RowsNumber = _RowsNumber; \
			ColumnsNumber = _ColumnsNumber; \
			GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE(); \
		} \
	} static const _Name##_RowsNumber##x##_ColumnsNumber
#define GD_HLSL_DEFINE_MATRIX_TYPES(_Name, _DataType) \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 4, 4); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 4, 3); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 3, 4); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 3, 3); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 3, 2); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 2, 3); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 2, 2); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 2, 1); \
	GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, 1, 2)
	GD_HLSL_DEFINE_MATRIX_TYPES(HLSLFloat  , float );
	GD_HLSL_DEFINE_MATRIX_TYPES(HLSLDouble , double);
#undef GD_HLSL_DEFINE_MATRIX_TYPES
#undef GD_HLSL_DEFINE_MATRIX_TYPE
	//! @}

#undef GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE

	// ==========================================================================================
	// HLSL shader parser implementation class.
	// ==========================================================================================

	// ------------------------------------------------------------------------------------------
	//! Provides parsing some subset of the HLSL code.
	class HLSLParserImpl final
	{
	private:
		StreamedLexer      Lexer;
		Lexem              CurrentLexem;
		ptrdiff_t          AnonymousIdentifiersCount = -1;
		HLSLScope*         GlobalScope;
		Vector<HLSLScope*> GlobalScopesStack;

	public /*Class API*/:
		GDINT ~HLSLParserImpl() {}
		GDINT explicit HLSLParserImpl(InputStream* Stream) : Lexer(Stream, GetDefaultOptionsForHLSL())
		{	
			// Registering default types.
			EnterScope(&HLSLSuperGlobalScope);
			EnterScope(GlobalScope = GD_NEW(HLSLScope));
		}

		GDINT HLSLScope const* ParseShader();

	private:

		// ------------------------------------------------------------------------------------------
		// Basic parsing pipeline..
		GDINT bool                  TryReadNextLexem();
		GDINT bool                  TryExpectLexem(LexemContentType const ContentType);
		GDINT void                     ExpectLexem(LexemContentType const ContentType);
		GDINT bool                  TryExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID);
		GDINT void                     ExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID);
		GDINT bool                  TryExpectNextLexem();
		GDINT void                     ExpectNextLexem();
		GDINT bool                  TryExpectNextLexem(LexemContentType const ContentType);
		GDINT void                     ExpectNextLexem(LexemContentType const ContentType);
		GDINT bool                  TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID);
		GDINT void                     ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID);

		// ------------------------------------------------------------------------------------------
		// Scope management..
		GDINL void                  EnterScope(HLSLScope* const CurrentScope) { GlobalScopesStack.InsertLast(CurrentScope); }
		GDINL void                  LeaveScope() { GlobalScopesStack.EraseLast(); }

		// ------------------------------------------------------------------------------------------
		// Searching functions.
		GDINT HLSLDefinition const* FindDefinitionInScope(String const& DefinitionName) const;
		GDINT HLSLDefinition const* FindDefinition(String const& DefinitionName) const;
		GDINT HLSLType       const* FindTypeInScope(String const& TypeName) const;
		GDINT HLSLType       const* FindType(String const& TypeName) const;
		GDINT HLSLVariable   const* FindVariableInScope(String const& VariableName) const;
		GDINT HLSLVariable   const* FindVariable(String const& VariableName) const;
		GDINT HLSLFunction   const* FindFunctionInScope(String const& FunctionName) const;
		GDINT HLSLFunction   const* FindFunction(String const& FunctionName) const;

		// ------------------------------------------------------------------------------------------
		// Parsing pipeline..
		GDINT void                  ValidateIdentifier(String const& Identifier);
	//	GDINT HLSLExpression const* ParseExpression(HLSLExpressionType const ExpectedExperssions);
		GDINT HLSLDefinition const* ParseDefinition(UInt32 const ExpectedDefinitions);
		GDINT void                  ParseScope(UInt32 const ExpectedDefinitions, HLSLScope* const Scope = nullptr);
		GDINT HLSLType       const* ParseType(UInt32 const ExpectedDefinitions, bool* const IsDefinition = nullptr);
		GDINT HLSLType       const* ParseTypeName(bool const DoIgnoreUndefinedType = false);
		GDINT HLSLCBuffer    const* ParseCBuffer();
		GDINT HLSLVariable   const* ParseVariable(HLSLDefinition&& PrependingDefinition, bool const DoExpectSemicolon = true);
		GDINT HLSLFunction   const* ParseFunction(HLSLDefinition&& PrependingDefinition);
		GDINT HLSLBinding  const* ParseBinding(HLSLBindingType const ExpectedBindings);
	}; // class HLSLParserImpl

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Public class API.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Parses HLSL shader into a scope.
	//! @returns Pointer to the HLSL scope object, that contains all declarations inside the scope.
	GDINT HLSLScope const* HLSLParserImpl::ParseShader()
	{
		TryReadNextLexem();
		while (CurrentLexem.GetContentType() != LexemContentType::EndOfStream)
		{
			UniquePtr<HLSLDefinition const> Definition(ParseDefinition(0
				| GD_HLSL_DEFINITION_TYPE
				| GD_HLSL_DEFINITION_VARIABLE
				| GD_HLSL_DEFINITION_FUNCTION
				| GD_HLSL_DEFINITION_CONSTANTBUFFER));
			GlobalScopesStack.GetLast()->InnerDefinitions.InsertLast(Definition.Release());
		}
		return GlobalScope;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Internal class API. Scope parsing.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Parses all content inside scope.
	//! input position: first lexem of shader, output: End-Of-Stream.
	//! @param ExpectedDefinitions Flags for all allowed definitions inside this scope.
	//! @param Scope Scope into which all data would be written.
	GDINT void HLSLParserImpl::ParseScope(UInt32 const ExpectedDefinitions, HLSLScope* const Scope /*= nullptr*/)
	{
		ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorScopeBegin);
		ExpectNextLexem();
		for (;;)
		{ 
			// Scanning our definition..
			UniquePtr<HLSLDefinition const> Definition;
			if (TryExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorScopeEnd))
				return;
			Definition.Reset(ParseDefinition(ExpectedDefinitions));

			// Placing it into scope..
			Scope->InnerDefinitions.InsertLast(Definition.Release());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Parses variable, function or structure definition.
	//! input position: first lexem of shader, output: first lexem after definition.
	//! @param ExpectedDefinitions Flags for all allowed definitions inside this scope.
	//! @returns Pointer to the parsed definition.
	GDINT HLSLDefinition const* HLSLParserImpl::ParseDefinition(UInt32 const ExpectedDefinitions)
	{ 
#if 0
		// Lets check if this declaration contains an attribute (if it generally supports attribute declaration).
		// Common syntax is something like: [FirstAttributeName(Args, ...)] [ [SecondAttributeName(Args, ...)] ].
		if ((ExpectedDefinitions & GD_HLSL_DEFINITION_ATTRIBUTE) != 0) 
		{	
			// Yes, here is attribute. Definitions contain multiple attributes, so we are parsing them all.
			GD_NOT_IMPLEMENTED();
		}
#endif // if 0

		if (CurrentLexem.GetContentType() == LexemContentType::Keyword)
		{ 
			// Lets determine what definition we have here.
			if (!GD_HLSL_IS_KEYWORD_OF_TYPE(CurrentLexem.GetProcessedDataID(), DEFINING) && !GD_HLSL_IS_KEYWORD_OF_TYPE(CurrentLexem.GetProcessedDataID(), TYPE_MODIFGDR))
			{
				throw HLSLParserErrorException(&CurrentLexem, "unexpected keyword '%s'", CurrentLexem.GetRawData().CStr());
			}
			else if (CurrentLexem.GetProcessedDataID() == HLSLKeywords::KeywordTypedef)
			{ 
				// We are having a typedef here:
				if ((ExpectedDefinitions & GD_HLSL_DEFINITION_TYPEDEF) == 0)
				{ 
					// Unexpected type definition.
					throw HLSLParserErrorException(&CurrentLexem, "unexpected type definition.");
				}

				GD_NOT_IMPLEMENTED();
			//	return ParseTypedef();
			}
			else if (CurrentLexem.GetProcessedDataID() == HLSLKeywords::KeywordCbuffer)
			{ 
				// We are having a constant buffer here:
				if ((ExpectedDefinitions & GD_HLSL_DEFINITION_CONSTANTBUFFER) == 0)
				{ 
					// Unexpected constant buffer definition.
					throw HLSLParserErrorException(&CurrentLexem, "unexpected constant buffer definition.");
				}
				return ParseCBuffer();
			}
		}

		// Parsing definition type modifier, if it exists,
		HLSLTypeModifier TypeModifier = GD_HLSL_TYPE_MODIFGDR_UNKNOWN;
		if (CurrentLexem.GetContentType() == LexemContentType::Keyword)
		{
			while (GD_HLSL_IS_KEYWORD_OF_TYPE(CurrentLexem.GetProcessedDataID(), TYPE_MODIFGDR))
			{
				TypeModifier = HLSLTypeModifier(TypeModifier | GD_BIT(CurrentLexem.GetProcessedDataID() - HLSLKeywords::Keyword_TYPE_MODIFGDR_BEGIN__ - 1));
				ExpectNextLexem();
			}
		}

		// Here we have a variable, function or struct definition.
		bool IsTypeDefinition = false;
		UniquePtr<HLSLType const> Type(ParseType(HLSLDefinitionType(ExpectedDefinitions & GD_HLSL_DEFINITION_TYPE), &IsTypeDefinition));
		UniquePtr<HLSLDefinition> Definition;
		if (CurrentLexem.GetContentType() != LexemContentType::Identifier)
		{ 
			// Was defined a structure so it is non required declare a instance.
			ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorSemicolon);
			if (!IsTypeDefinition)
			{ 
				// Something strange happened..
				throw HLSLParserErrorException(&CurrentLexem, "identifier or ';' expected");
			}

			// Here is just a type declaration, nothing more.
			ExpectNextLexem();
			return Type.Release();
		}
		
		// We are having a instance of this type here.
		Definition.Reset(GD_NEW(HLSLDefinition));
		Definition->Name = CurrentLexem.GetRawData();
		Definition->Type = Type.Release();
		Definition->TypeModifier = TypeModifier;

		ExpectNextLexem(LexemContentType::Operator);
		if (CurrentLexem.GetProcessedDataID() == HLSLOperators::OperatorParamsBegin)
		{ 
			// Parsing function declaration here? which start with '(' operator.
			if ((ExpectedDefinitions & GD_HLSL_DEFINITION_FUNCTION) == 0)
			{ 
				// Unexpected function definition.
				throw HLSLParserErrorException(&CurrentLexem, "unexpected function definition.");
			}

			return ParseFunction(Move(*Definition.GetPointer()));
		}
		else
		{	
			// Parsing variable declaration here:
			if ((ExpectedDefinitions & (GD_HLSL_DEFINITION_VARIABLE | GD_HLSL_DEFINITION_ARGUMENT)) == 0)
			{ 
				// Unexpected variable definition.
				throw HLSLParserErrorException(&CurrentLexem, "unexpected variable definition.");
			}

			if (Type == &HLSLVoid)
			{ 
				// Variables cannot use 'void' as type.
				throw HLSLParserErrorException(&CurrentLexem, "'void' variable type is not allowed.");
			}

			return ParseVariable(Move(*Definition.GetPointer()), (ExpectedDefinitions & GD_HLSL_DEFINITION_ARGUMENT) == 0);
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Specific code structures parsing.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Parses type that is defined in current lexem.
	//! input position: first lexem of type, output: last lexem of type.
	//! @param ExpectedDefinitions Flags for all allowed definitions inside this scope.
	//! @param IsDefinition Optional pointer to the parameter into which @c true is written, if the type declaration was met.
	//! @returns Pointer to the parsed type.
	GDINT HLSLType const* HLSLParserImpl::ParseType(UInt32 const ExpectedDefinitions, bool* const IsDefinition /* = nullptr */)
	{
		// Lets test if our type is something that touches structures or classes:
		if (TryExpectLexem(LexemContentType::Keyword, HLSLKeywords::KeywordStruct))
		{ 
			// Type if structure or class. Lets check what is it:
			// 1) A structure declaration:   struct [ StructName ] [ : BaseClass ] { } [ InstanceDecl ] 
			// 2) A C-style struct instance: struct   StructName
			// First variant is valid only if struct declaration is expected.
			UniquePtr<HLSLStruct> Struct;
			ExpectNextLexem();
			if (CurrentLexem.GetContentType() == LexemContentType::Identifier)
			{ // We have named structure declaration C-style struct instance:
				String const TypeName = CurrentLexem.GetRawData();
				HLSLType const* const AlreadyDefinedType = ParseTypeName(true);
				if (AlreadyDefinedType != nullptr)
				{ 
					// Type was already declared. We are having C-style struct instance or a redefinition error.
					if (AlreadyDefinedType->Class != HLSLKeywords::KeywordStruct)
					{ 
						// Redefinition of type using struct/class keyword.
						throw HLSLParserErrorException(&CurrentLexem, "redefinition of type '%s' using struct/class keyword.", TypeName.CStr());
					}

					ExpectNextLexem();
					return AlreadyDefinedType;
				}
				else if ((ExpectedDefinitions & GD_HLSL_DEFINITION_TYPE) == 0)
				{ 
					// We haven't expected a declaration. That means that if struct/class keyword used, that after it should come a type.
					throw HLSLParserErrorException(&CurrentLexem, "unexpected struct/class declaration.");
				}

				// We need to validate identifier.
				ValidateIdentifier(TypeName);
				ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorScopeBegin);
				Struct.Reset(GD_NEW(HLSLStruct));
				Struct->Name = TypeName;
			}
			else if ((TryExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorScopeBegin)))
			{ 
				// We have anonymous structure declaration.
				Struct.Reset(GD_NEW(HLSLStruct));
				Struct->Name = String::Format("__AnonymousStruct_%d", ++AnonymousIdentifiersCount);
			}
			else
			{ 
				// Something that does not match our cases.
				throw HLSLParserErrorException(&CurrentLexem, "identifier or '{'. expected.");
			}

			// Now we can expect only a structure type declaration.
			// Parsing struct body here.
			if (IsDefinition != nullptr) (*IsDefinition) = true;
			ParseScope(GD_HLSL_DEFINITION_VARIABLE, Struct.GetPointer());
			ExpectNextLexem();
			return Struct.Release();
		}
		
		// Searching for predefined type.
		if (IsDefinition != nullptr) 
			*IsDefinition = false;
		return ParseTypeName();
	}

	// ------------------------------------------------------------------------------------------
	//! Parses name of the type.
	//! input position: first lexem of type name, output: first lexem after type name.
	//! @param DoIgnoreUndefinedType If specified as @c true, than parsing the undefined type will not cause an error.
	//! @returns Pointer to the parsed type name.
	GDINT HLSLType const* HLSLParserImpl::ParseTypeName(bool const DoIgnoreUndefinedType /*= false*/)
	{
		HLSLType const* HLSLDefinitionType = nullptr;
		ExpectLexem(LexemContentType::Identifier);
		if ((HLSLDefinitionType = FindType(CurrentLexem.GetRawData())) == nullptr && !DoIgnoreUndefinedType)
		{ 
			// Type was not declared declared.
			throw HLSLParserErrorException(&CurrentLexem, "type '%s' was not defined.", CurrentLexem.GetRawData().CStr());
		}

		ExpectNextLexem();
		return HLSLDefinitionType;
	}

	// ------------------------------------------------------------------------------------------
	//! Parses variable that is defined in current lexem.
	//! input position: first lexem after variable name, output: first lexem after variable definition.
	//! @param PrependingDefinition Definition, into which variable info would be stored.
	//! @param DoExpectSemicolon Do expect the ';' lexem after variable definition.
	//! @returns Pointer to the parsed variable.
	GDINT HLSLVariable const* HLSLParserImpl::ParseVariable(HLSLDefinition&& PrependingDefinition, bool const DoExpectSemicolon /* = true */)
	{
		UniquePtr<HLSLVariable> Variable(GD_NEW(HLSLVariable, Forward<HLSLDefinition>(PrependingDefinition)));
		if (TryExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorIndexBegin))
		{ 
			// Parsing array definitions.
			ExpectNextLexem(LexemContentType::ConstantInteger);
			if ((Variable->ArrayRank = CurrentLexem.GetProcessedDataInt()) == 0)
			{ 
				// Zero-sized arrays are not allowed.
				throw HLSLParserErrorException(&CurrentLexem, "zero-sized arrays are not allowed");
			}

			ExpectNextLexem(LexemContentType::Operator, HLSLOperators::OperatorIndexEnd);
			ExpectNextLexem(LexemContentType::Operator);
		}
		else if (CurrentLexem.GetProcessedDataID() == HLSLOperators::OperatorColon)
		{ 
			// Parsing variable semantic, register and pack offset here.
			HLSLBindingType ExpectedColonExprs = HLSLBindingType(GD_HLSL_EXPRCOLON_TYPE_REGISTER | GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET);
			if (Variable->Type->Class == GD_HLSL_TYPE_CLASS_SCALAR || Variable->Type->Class == GD_HLSL_TYPE_CLASS_VECTOR)
				ExpectedColonExprs = HLSLBindingType(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC | ExpectedColonExprs);

			Variable->Binding = ParseBinding(ExpectedColonExprs);
			ExpectLexem(LexemContentType::Operator);
		}

		if (CurrentLexem.GetRawData() == '<')
		{ 
			// Parsing annotations here.
			GD_NOT_IMPLEMENTED();
			return nullptr;
		}

		if (CurrentLexem.GetRawData() == '=')
		{ 
			// Parsing default values here.
		//  Variable->InitialValue = ParseExpression(GD_HLSL_EXPRESSION_UNKNOWN); // ???
		//	if (Variable->InitialValue == nullptr;
			GD_NOT_IMPLEMENTED();
			return nullptr;
		}

		if (DoExpectSemicolon)
		{
			ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorSemicolon);
			ExpectNextLexem();
		}

		return Variable.Release();
	}

	// ------------------------------------------------------------------------------------------
	//! Parses function that is defined in current lexem.
	//! input position: first lexem after function name ('(' lexem), output:  first lexem after function definition.
	//! @param PrependingDefinition Definition, into which function info would be stored.
	//! @returns Pointer to the parsed function.
	GDINT HLSLFunction const* HLSLParserImpl::ParseFunction(HLSLDefinition&& PrependingDefinition)
	{ 
		// Parsing function arguments here.
		UniquePtr<HLSLFunction> Function(GD_NEW(HLSLFunction, Forward<HLSLDefinition>(PrependingDefinition)));
		ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorParamsBegin);
		for (;;)
		{ 
			// Parsing function arguments here:
			HLSLArgumentAccessType ArgumentAccessType = GD_HLSL_ARGUMENT_IN;
			ExpectNextLexem();
			if (CurrentLexem.GetContentType() == LexemContentType::Operator 
				&& CurrentLexem.GetProcessedDataID() == HLSLOperators::OperatorParamsEnd) 
				break;
			
			if (CurrentLexem.GetContentType() == LexemContentType::Keyword 
				&& (GD_HLSL_IS_KEYWORD_OF_TYPE(CurrentLexem.GetProcessedDataID(), ARGUMENTS)))
			{ 
				// Here we have a keyword that describes argument access type: 
				Map<StreamedLexerKeyword, HLSLArgumentAccessType> static const AccessTypeTranslationTable = {
					{ HLSLKeywords::In   , GD_HLSL_ARGUMENT_IN    },
					{ HLSLKeywords::Out  , GD_HLSL_ARGUMENT_OUT   },
					{ HLSLKeywords::InOut, GD_HLSL_ARGUMENT_INOUT },
				};
				ArgumentAccessType = AccessTypeTranslationTable[CurrentLexem.GetProcessedDataID()];
				ExpectNextLexem();
			}

			UniquePtr<HLSLVariable> ArgumentDefinition(static_cast<HLSLVariable*>(const_cast<HLSLDefinition*>(ParseDefinition(GD_HLSL_DEFINITION_ARGUMENT))));
			HLSLArgument* Argument = GD_NEW(HLSLArgument, Move(*ArgumentDefinition.GetPointer()));
			Argument->AccsessType = ArgumentAccessType;
			Function->Arguments.InsertLast(Argument);
			
			if (CurrentLexem.GetContentType() == LexemContentType::Operator 
				&& CurrentLexem.GetProcessedDataID() == HLSLOperators::OperatorParamsEnd) 
				break;
			ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorComma);
		}

		ExpectNextLexem(LexemContentType::Operator);
		if (CurrentLexem.GetProcessedDataID() == HLSLOperators::OperatorColon)
		{ 
			// Parsing function semantic here.
			if (Function->Type == &HLSLVoid)
			{ 
				// 'void' functions cannot have semantics.
				throw HLSLParserErrorException(&CurrentLexem, "'void' functions cannot have semantics.");
			}

			Function->Semantic = static_cast<HLSLSemantic const*>(ParseBinding(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC));
			ExpectLexem(LexemContentType::Operator);
		}

		ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorScopeBegin);
	
		// We are not parsing anything here. Just copying a function body.
		//! @todo Implement full shader function body analyzer here.
		BaseStringBuilder<Char> FunctionBody;
		FunctionBody.Append(Char('{'));
		for (SizeTp BracesBalance = 1; BracesBalance != 0;)
		{
			Char Character = Lexer.GetNextCharacter();
			switch (Character)
			{
				case Char('{'): BracesBalance += 1;
					break;
				case Char('}'): BracesBalance -= 1;
					break;
			}
			FunctionBody.Append(Character);
		}
		Function->Body = FunctionBody.ToString();
		ExpectNextLexem();
		return Function.Release();
	}

	// ------------------------------------------------------------------------------------------
	//! Parses constant buffer definition.
	//! input position: first lexem of the constant buffer ('cbuffer' lexem), output: first lexem after constant buffer definition.
	//! @returns Pointer to the parsed constant buffer.
	GDINT HLSLCBuffer const* HLSLParserImpl::ParseCBuffer()
	{
		ExpectNextLexem(LexemContentType::Identifier);
		ValidateIdentifier(CurrentLexem.GetRawData());

		UniquePtr<HLSLCBuffer> ConstantBuffer(GD_NEW(HLSLCBuffer));
		ConstantBuffer->Name = CurrentLexem.GetRawData();

		ExpectNextLexem(LexemContentType::Operator);
		if (CurrentLexem.GetProcessedDataID() == HLSLOperators::OperatorColon)
		{ 
			// Parsing constant buffer register..
			ConstantBuffer->Register = static_cast<HLSLRegister const*>(ParseBinding(GD_HLSL_EXPRCOLON_TYPE_REGISTER));
			if (ConstantBuffer->Register == nullptr) return nullptr;
			if (ConstantBuffer->Register->Register != GD_HLSL_REGISTER_B)
			{ 
				// Constant buffers may locate only in 'b' registers.
				throw HLSLParserErrorException(&CurrentLexem, "constant buffers may locate only in 'b' registers.");
			}
			ExpectLexem(LexemContentType::Operator);
		}

		ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorScopeBegin);
		ParseScope(GD_HLSL_DEFINITION_VARIABLE, ConstantBuffer.GetPointer());
		ExpectNextLexem();
		return ConstantBuffer.Release();
	}

	// ------------------------------------------------------------------------------------------
	//! Parses expression after ':' operator.
	//! input position: ':' lexem, output: first lexem after parsed expression.
	//! @param ExpectedBindings Flags for all allowed expression types.
	//! @returns Pointer to the parsed constant expression after ':' operator.
	GDINT HLSLBinding const* HLSLParserImpl::ParseBinding(HLSLBindingType const ExpectedBindings)
	{
		ExpectLexem(LexemContentType::Operator, HLSLOperators::OperatorColon);
		ExpectNextLexem();
		if (CurrentLexem.GetContentType() == LexemContentType::Keyword)
		{ 
			// Here comes the 'register' or 'packoffset'
			if (CurrentLexem.GetProcessedDataID() == HLSLKeywords::KeywordRegister)
			{ 
				// Here is register. Lets check if it was expected and parse it.
				if ((ExpectedBindings & GD_HLSL_EXPRCOLON_TYPE_REGISTER) == 0)
				{ 
					// Registers are not allowed here.
					throw HLSLParserErrorException(&CurrentLexem, "'register' is not allowed here.");
				}

				ExpectNextLexem(LexemContentType::Operator, HLSLOperators::OperatorParamsBegin);
				ExpectNextLexem(LexemContentType::Identifier);

				static Map<Char, HLSLRegisterType> const RegistersTranslationTable = {
					{ 'b', GD_HLSL_REGISTER_B },
					{ 't', GD_HLSL_REGISTER_T },
					{ 'c', GD_HLSL_REGISTER_C },
					{ 's', GD_HLSL_REGISTER_S },
				};

				auto const RegisterIter = RegistersTranslationTable.QueryIterator(*CurrentLexem.GetRawData().CStr());
				if (CurrentLexem.GetRawData().GetLength() == 2 ? RegisterIter != RegistersTranslationTable.End() && !CChar::IsDigit(CurrentLexem.GetRawData()[1]) : true)
				{
					// Current register is a bit invalid.
					throw HLSLParserErrorException(&CurrentLexem, "'register' '%s' is invalid.", CurrentLexem.GetRawData().CStr());
				}

				UniquePtr<HLSLRegister> Register(GD_NEW(HLSLRegister));
				Register->Register = (*RegisterIter).Value;
				Register->RegisterID = SizeTp(CChar::ToDigit(CurrentLexem.GetRawData()[1]));

				ExpectNextLexem(LexemContentType::Operator, HLSLOperators::OperatorParamsEnd);
				ExpectNextLexem(LexemContentType::Operator);
				return Register.Release();
			}
			else if (CurrentLexem.GetProcessedDataID() == HLSLKeywords::KeywordPackOffset)
			{ 
				GD_NOT_IMPLEMENTED();
#if 0
				// Here is register. Lets check if it was expected and parse it.
				if ((ExpectedBindings & GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET) == 0)
				{ 
					// Registers are not allowed here.
					HLSLParserErrorDesc static const PackOffsetIsNotAllowedError("'packoffset' is not allowed here.");
					throw HLSLParserErrorException(PackOffsetIsNotAllowedError.ToString(&CurrentLexem));
				}
#endif	// if 0
			}
		}
		else
		{ 
			// Here comes semantic. 
			//! @todo Rewrite semantic name and ID extraction after 'String' class refactoring.
			ExpectLexem(LexemContentType::Identifier);
			UniquePtr<HLSLSemantic> Semantic(GD_NEW(HLSLSemantic));
			bool const HasSemanticID = CChar::IsDigit(CurrentLexem.GetRawData().GetLast());
			Semantic->SemanticID = HasSemanticID ? SizeTp(CChar::ToDigit(CurrentLexem.GetRawData().GetLast())) : 0;
			Semantic->Semantic = HLSLSemanticTypeFromString(CurrentLexem.GetRawData().Substring(0, CurrentLexem.GetRawData().GetLength() - (HasSemanticID ? 1 : 0)));
			if (Semantic->Semantic == GD_HLSL_SEMANTIC_UNKNOWN)
			{ 
				// Invalid semantic specified.
				throw HLSLParserErrorException(&CurrentLexem, "unknown semantic '%s'.", CurrentLexem.GetRawData().CStr());
			}
			else if ((ExpectedBindings & GD_HLSL_EXPRCOLON_TYPE_SEMANTIC) == 0)
			{ 
				// No semantics allowed here.
				throw HLSLParserErrorException(&CurrentLexem, "semantic is not allowed here.");
			}

			ExpectNextLexem();
			return Semantic.Release();
		}

		return nullptr;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// bool HLSLParserImpl::Validate***(String const& ***)
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Validates, whether specified identifier was not defined in current scope.
	//! @param Identifier Identifier to test.
	void HLSLParserImpl::ValidateIdentifier(String const& Identifier)
	{
		if (FindDefinitionInScope(Identifier) != nullptr)
		{ 
			// Identifier was already declared in this scope.
			throw HLSLParserErrorException(&CurrentLexem, "identifier '%s' was already defined in current scope.", CurrentLexem.GetRawData().CStr());
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Basic PPipeline and expectations.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	bool HLSLParserImpl::TryReadNextLexem()
	{
		if (Lexer.GetNextLexem(&CurrentLexem))
		{ 
			// Reading succeeded, we need to skip comments comments until read token.
			if (CurrentLexem.GetContentType() == LexemContentType::Comment)
				return TryReadNextLexem();
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	bool HLSLParserImpl::TryExpectLexem(LexemContentType const ContentType)
	{
		return CurrentLexem.GetContentType() == ContentType;
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	void HLSLParserImpl::ExpectLexem(LexemContentType const ContentType)
	{
		if (!TryExpectLexem(ContentType))
		{ 
			// Unexpected lexem type.
			throw HLSLParserErrorException(&CurrentLexem, "unexpected %s. Expected %s.", LexemContentTypeToString(CurrentLexem.GetContentType()), LexemContentTypeToString(ContentType));
		}
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	bool HLSLParserImpl::TryExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (TryExpectLexem(ContentType))
		{
			if (CurrentLexem.GetProcessedDataID() == ID)
				return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	void HLSLParserImpl::ExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		ExpectLexem(ContentType);

		// Next lexem exists and has expected content type.
		if (CurrentLexem.GetProcessedDataID() != ID)
		{ 
			// Unexpected lexem value.
			throw HLSLParserErrorException(&CurrentLexem, "unexpected '%s'.", CurrentLexem.GetRawData().CStr());
		}
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	bool HLSLParserImpl::TryExpectNextLexem()
	{
		return TryReadNextLexem();
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	void HLSLParserImpl::ExpectNextLexem()
	{
		if (!TryExpectNextLexem())
		{ 
			// Unexpected end of stream while reading lexem.
			throw HLSLParserErrorException(&CurrentLexem, "unexpected end of stream.");
		}
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	bool HLSLParserImpl::TryExpectNextLexem(LexemContentType const ContentType)
	{
		ExpectNextLexem();
		return TryExpectLexem(ContentType);
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	void HLSLParserImpl::ExpectNextLexem(LexemContentType const ContentType)
	{
		ExpectNextLexem();
		ExpectLexem(ContentType);
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	bool HLSLParserImpl::TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (TryExpectNextLexem(ContentType))
		{
			if (CurrentLexem.GetProcessedDataID() == ID)
				return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	void HLSLParserImpl::ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		ExpectNextLexem();
		ExpectLexem(ContentType, ID);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSL*** const* HLSLScope::Find***
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLDefinition const* HLSLScope::FindDefinition(String const& DefinitionName) const
	{ 
		// Searching in specified scope.
		auto const RequiredDefinitionIter = FindFirstIf(InnerDefinitions, [&DefinitionName](HLSLDefinition const* const Definition) -> bool
			{ return (Definition->Name == DefinitionName); });
		return RequiredDefinitionIter != InnerDefinitions.End() ? *RequiredDefinitionIter : nullptr;
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLType const* HLSLScope::FindType(String const& TypeName) const
	{
		return HLSLDynamicCast<HLSLType const*>(FindDefinition(TypeName));
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLVariable const* HLSLScope::FindVariable(String const& VariableName) const
	{
		return HLSLDynamicCast<HLSLVariable const*>(FindDefinition(VariableName));
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLFunction const* HLSLScope::FindFunction(String const& FunctionName) const
	{
		return HLSLDynamicCast<HLSLFunction const*>(FindDefinition(FunctionName));
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSL*** const* HLSLParserImpl::Find***[InScope]
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLDefinition const* HLSLParserImpl::FindDefinition(String const& DefinitionName) const
	{ 
		// Searching in all stack of our declarations.
		for (auto const& DefinitionScope : Reverse(GlobalScopesStack))
		{ 
			// Definition was not found in current scope, searching in lower one.
			HLSLDefinition const* const FoundDefinition = DefinitionScope->FindDefinition(DefinitionName);
			if (FoundDefinition != nullptr) return FoundDefinition;
		}

		return nullptr;
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLType const* HLSLParserImpl::FindType(String const& TypeName) const
	{
		return HLSLDynamicCast<HLSLType const*>(FindDefinition(TypeName));
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLVariable const* HLSLParserImpl::FindVariable(String const& VariableName) const
	{
		return HLSLDynamicCast<HLSLVariable const*>(FindDefinition(VariableName));
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLFunction const* HLSLParserImpl::FindFunction(String const& FunctionName) const
	{
		return HLSLDynamicCast<HLSLFunction const*>(FindDefinition(FunctionName));
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLDefinition const* HLSLParserImpl::FindDefinitionInScope(String const& DefinitionName) const
	{ 
		// Searching in top scope.
		auto const& TopDefinitionScope = GlobalScopesStack.GetLast();
		return TopDefinitionScope->FindDefinition(DefinitionName);
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLType const* HLSLParserImpl::FindTypeInScope(String const& TypeName) const
	{
		auto const& TopTypeScope = GlobalScopesStack.GetLast();
		return TopTypeScope->FindType(TypeName);
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLVariable const* HLSLParserImpl::FindVariableInScope(String const& VariableName) const
	{
		auto const& TopDefinitionScope = GlobalScopesStack.GetLast();
		return TopDefinitionScope->FindVariable(VariableName);
	}

	// ------------------------------------------------------------------------------------------
	//! @todo Document me.
	HLSLFunction const* HLSLParserImpl::FindFunctionInScope(String const& FunctionName) const
	{
		auto const& TopFunctionScope = GlobalScopesStack.GetLast();
		return TopFunctionScope->FindFunction(FunctionName);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSLParser class
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	HLSLScope const* HLSLParser::ParseShader(InputStream* Stream)
	{
		return HLSLParserImpl(Stream).ParseShader();
	}

	CStr HLSLSemanticToStr(HLSLSemanticType const Semantic)
	{
		static CStr const SemanticStrings[] = {
			nullptr,
			"BINORMAL",
			"BLENDINDICES",
			"BLENDWEIGHT",
			"COLOR",
			"NORMAL",
			"POSITION",
			"POSITIONT",
			"PSIZE",
			"TANGENT",
			"TEXCOORD",
			"FOG",
			"TESSFACTOR",
			"SV_ClipDistance",
			"SV_CullDistance",
			"SV_Coverage",
			"SV_Depth",
			"SV_DispatchThreadID",
			"SV_DomainLocation",
			"SV_GroupID",
			"SV_GroupIndex",
			"SV_GroupThreadID",
			"SV_GSInstanceID",
			"SV_InsideTessFactor",
			"SV_IsFrontFace",
			"SV_OutputControlPointID",
			"SV_Position",
			"SV_RenderTargetArrayIndex",
			"SV_SampleIndex",
			"SV_Target",
			"SV_TessFactor",
			"SV_ViewportArrayIndex",
			"SV_InstanceID",
			"SV_PrimitiveID",
			"SV_VertexID",
		};

		GD_ASSERT(Semantic < GD_HLSL_SEMANTICS_COUNT, "Invalid semantic specified");
		return SemanticStrings[static_cast<SizeTp>(Semantic)];
	}

	static HLSLSemanticType HLSLSemanticTypeFromString(String const& Semantic)
	{
		static Map<HashCode, HLSLSemanticType> const SemanticsList = {
#define GD_HLSL_DEFINE_SEMANTIC(Semantic) { String(#Semantic).GetHashCode(), GD_HLSL_SEMANTIC_##Semantic }
			GD_HLSL_DEFINE_SEMANTIC(BINORMAL),
			GD_HLSL_DEFINE_SEMANTIC(BLENDINDICES),
			GD_HLSL_DEFINE_SEMANTIC(BLENDWEIGHT),
			GD_HLSL_DEFINE_SEMANTIC(COLOR),
			GD_HLSL_DEFINE_SEMANTIC(NORMAL),
			GD_HLSL_DEFINE_SEMANTIC(POSITION),
	//		GD_HLSL_DEFINE_SEMANTIC(POSITIONT),
	//		GD_HLSL_DEFINE_SEMANTIC(PSIZE),
			GD_HLSL_DEFINE_SEMANTIC(TANGENT),
			GD_HLSL_DEFINE_SEMANTIC(TEXCOORD),
	//		GD_HLSL_DEFINE_SEMANTIC(FOG),
			GD_HLSL_DEFINE_SEMANTIC(TESSFACTOR),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_ClipDistance),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_CullDistance),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_Coverage),
			GD_HLSL_DEFINE_SEMANTIC(SV_Depth),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_DispatchThreadID),
			GD_HLSL_DEFINE_SEMANTIC(SV_DomainLocation),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_GroupID),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_GroupIndex),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_GroupThreadID),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_GSInstanceID),
			GD_HLSL_DEFINE_SEMANTIC(SV_InsideTessFactor),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_IsFrontFace),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_OutputControlPointID),
			GD_HLSL_DEFINE_SEMANTIC(SV_Position),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_RenderTargetArrayIndex),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_SampleIndex),
			GD_HLSL_DEFINE_SEMANTIC(SV_Target),
			GD_HLSL_DEFINE_SEMANTIC(SV_TessFactor),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_ViewportArrayIndex),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_InstanceID),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_PrimitiveID),
	//		GD_HLSL_DEFINE_SEMANTIC(SV_VertexID),
#undef GD_HLSL_DEFINE_SEMANTIC
		};

		auto const SemanticIndex = SemanticsList.QueryIterator(Semantic.GetHashCode());
		return SemanticIndex != SemanticsList.End() ? (*SemanticIndex).Value : GD_HLSL_SEMANTIC_UNKNOWN;
	}

	extern HRISemantic HLSLSemanticToHRI(HLSLSemanticType const Semantic)
	{
		static HRISemantic const HLSL2HRITranslationsTable[] = {
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HLSL_SEMANTIC_UNKNOWN
			GD_HRI_SEMANTIC_BINORMAL,
			GD_HRI_SEMANTIC_BLENDINDICES,
			GD_HRI_SEMANTIC_BLENDWEIGHT,
			GD_HRI_SEMANTIC_COLOR,
			GD_HRI_SEMANTIC_NORMAL,
			GD_HRI_SEMANTIC_POSITION,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_POSITIONT,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_PSIZE,
			GD_HRI_SEMANTIC_TANGENT,
			GD_HRI_SEMANTIC_TEXCOORD,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_FOG,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_TESSFACTOR,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_ClipDistance,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_CullDistance,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_Coverage,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_Depth,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_DispatchThreadID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_DomainLocation,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_GroupID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_GroupIndex,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_GroupThreadID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_GSInstanceID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_InsideTessFactor,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_IsFrontFace,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_OutputControlPointID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_Position,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_RenderTargetArrayIndex,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_SampleIndex,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_Target
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_TessFactor,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_ViewportArrayIndex,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_InstanceID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_PrimitiveID,
			GD_HRI_SEMANTIC_UNKNOWN, // GD_HRI_SEMANTIC_SV_VertexID,
		};

		GD_ASSERT(Semantic < GD_HLSL_SEMANTICS_COUNT, "Invalid semantic specified");
		return HLSL2HRITranslationsTable[static_cast<SizeTp>(Semantic)];
	}

	extern HLSLSemanticType HLSLSemanticFromHRI(HRISemantic const Semantic)
	{
		static HLSLSemanticType const HRI2HLSLTranslationsTable[] = {
			GD_HLSL_SEMANTIC_POSITION,
			GD_HLSL_SEMANTIC_NORMAL,
			GD_HLSL_SEMANTIC_TANGENT,
			GD_HLSL_SEMANTIC_BINORMAL,
			GD_HLSL_SEMANTIC_TEXCOORD,
			GD_HLSL_SEMANTIC_BLENDINDICES,
			GD_HLSL_SEMANTIC_BLENDWEIGHT,
			GD_HLSL_SEMANTIC_COLOR,
		};

		GD_ASSERT(Semantic < GD_HRI_SEMANTICS_COUNT, "Invalid semantic specified");
		return HRI2HLSLTranslationsTable[static_cast<SizeTp>(Semantic)];
	}

	GDINT static StreamedLexerOptions const& GetDefaultOptionsForHLSL()
	{
#if (!defined(__INTELLISENSE__)) // IntelliSence marks following code with errors. Just let in not parse it.
		static StreamedLexerOptions const HlslLexerOptions(
			StreamedLexerKeywordsList({
				{ HLSLKeywords::KeywordStruct , "struct" },
				{ HLSLKeywords::KeywordTypedef , "typedef" },
				{ HLSLKeywords::KeywordCbuffer , "cbuffer" },
				{ HLSLKeywords::KeywordConst , "const" },
				{ HLSLKeywords::RowMajor , "row_major" },
				{ HLSLKeywords::ColumnMajor , "column_major" },
				{ HLSLKeywords::In , "in" },
				{ HLSLKeywords::Out , "out" },
				{ HLSLKeywords::InOut , "inout" },
				{ HLSLKeywords::KeywordRegister , "register" },
				{ HLSLKeywords::KeywordPackOffset , "packoffset" },
			}),
			StreamedLexerOperatorsList({
				{ HLSLOperators::OperatorIndexBegin , "[" },
				{ HLSLOperators::OperatorIndexEnd , "]" },
				{ HLSLOperators::OperatorScopeBegin , "{" },
				{ HLSLOperators::OperatorScopeEnd , "}" },
				{ HLSLOperators::OperatorParamsBegin , "(" },
				{ HLSLOperators::OperatorParamsEnd , ")" },
				{ HLSLOperators::OperatorTemplateBegin, "<" },
				{ HLSLOperators::OperatorTemplateEnd , ">" },
				{ HLSLOperators::OperatorColon , ":" },
				{ HLSLOperators::OperatorSemicolon , ";" },
				{ HLSLOperators::OperatorComma , "," },
			}),
			"//", "/*", "*/",
			'x', '\0', 'b', '.'
		);
		return HlslLexerOptions;
#endif // if (!defined(__INTELLISENSE__))
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP
