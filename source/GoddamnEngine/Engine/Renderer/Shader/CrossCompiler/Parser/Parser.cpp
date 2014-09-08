/// ==========================================================================================
/// Parser.h - HLSL parser implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 14.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Include.h>
#if (defined(GD_PLATFORM_DESKTOP))

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.h>
#include <GoddamnEngine/Core/Containers/Pointer/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/Map/Map.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>

/// Define this to make our parser parse things that cannot be translated into GLSL
// #define GD_HRI_SHADERCC_PARSE_UNTRANSLATABLE 1

GD_NAMESPACE_BEGIN
	
	using namespace StreamedLexerDefaultOptions;

	/// Represents lexer error description.
	struct HLSLParserErrorDesc final : public ToolchainMessageDesc
	{
	public:
		GDINL explicit HLSLParserErrorDesc(String const& Message) : ToolchainMessageDesc(Message) { }
	};	// struct StreamedLexerErrorDesc

	/// Fatal parsing error exception
	class HLSLParserErrorException : public ToolchainException
	{
	public /*Public API*/:
		GDINL explicit HLSLParserErrorException(String const& Message) : ToolchainException(Message.CStr()) { }
		GDINL virtual ~HLSLParserErrorException() { }
	};	// class ToolchainException

	static StreamedLexerOptions const& GetDefaultOptionsForHLSL();
	static HLSLSemanticType HLSLSemanticTypeFromString(String const& Semantic);

	enum HLSLOperator : StreamedLexerOperator
	{
		GD_HLSL_OPERATOR_UNKNOWN = 0,
		GD_HLSL_OPERATOR_INDEX_BEGIN = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_BEGIN,
		GD_HLSL_OPERATOR_INDEX_END = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_INDEX_END,
		GD_HLSL_OPERATOR_SCOPE_BEGIN = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_BEGIN,
		GD_HLSL_OPERATOR_SCOPE_END = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SCOPE_END,
		GD_HLSL_OPERATOR_PARAMS_BEGIN = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_BEGIN,
		GD_HLSL_OPERATOR_PARAMS_END = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_PARAMS_END,
		GD_HLSL_OPERATOR_TEMPLATE_BEGIN = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_BEGIN,
		GD_HLSL_OPERATOR_TEMPLATE_END = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_TEMPLATE_END,
		GD_HLSL_OPERATOR_COLON = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COLON,
		GD_HLSL_OPERATOR_SEMICOLON = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_SEMICOLON,
		GD_HLSL_OPERATOR_COMMA = GD_STREAMED_LEXER_OPTIONS_CPP_OPERATOR_COMMA,
	};	// enum HLSLOperator

	enum HLSLKeyword : StreamedLexerKeyword
	{
		GD_HLSL_KEYWORD_UNKNOWN = 0,
		GD_HLSL_KEYWORD_DEFINING_BEGIN__,
			GD_HLSL_KEYWORD_STRUCT,
			GD_HLSL_KEYWORD_CBUFFER,
			GD_HLSL_KEYWORD_TYPEDEF,
		GD_HLSL_KEYWORD_DEFINING_END__,
		GD_HLSL_KEYWORD_TYPE_MODIFIER_BEGIN__,
			GD_HLSL_KEYWORD_TYPE_MODIFIER_CONST,
			GD_HLSL_KEYWORD_TYPE_MODIFIER_ROW_MAJOR,
			GD_HLSL_KEYWORD_TYPE_MODIFIER_COLUMN_MAJOR,
		GD_HLSL_KEYWORD_TYPE_MODIFIER_END__,
		GD_HLSL_KEYWORD_ARGUMENTS_BEGIN__,
			GD_HLSL_KEYWORD_IN,
			GD_HLSL_KEYWORD_OUT,
			GD_HLSL_KEYWORD_INOUT,
		GD_HLSL_KEYWORD_ARGUMENTS_END__,
		GD_HLSL_KEYWORD_OTHER_BEGIN__,
			GD_HLSL_KEYWORD_REGISTER,
			GD_HLSL_KEYWORD_PACKOFFSET,
		GD_HLSL_KEYWORD_OTHER_END__,
	//	GD_HLSL_KEYWORD_,
	};	// enum HLSLKeyword

#define GD_HLSL_IS_KEYWORD_OF_TYPE(Keyword, Type) (((Keyword) > GD_HLSL_KEYWORD_##Type##_BEGIN__) && ((Keyword) < GD_HLSL_KEYWORD_##Type##_END__))

	struct HLSLSuperGlobalScopeType final : public HLSLScope 
	{
		GDINT ~HLSLSuperGlobalScopeType()
		{
			this->InnerDefinitions.Clear();
		}
	} static HLSLSuperGlobalScope;
#define GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE() \
	do { \
		HLSLSuperGlobalScope.InnerDefinitions.PushLast(this); \
	} while(false)

	// Basic types.
#define GD_HLSL_DEFINE_BASIC_TYPE(_Name, _HLSLName, _Class) \
	struct _Name final : public HLSLType { \
		GDINL _Name() : HLSLType(_Class) { \
			this->Name = _HLSLName; \
			GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE(); \
		} \
	} static const _Name
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLVoid,         "void",         GD_HLSL_TYPE_CLASS_VOID       );
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLTexture2D,    "Texture2D",    GD_HLSL_TYPE_CLASS_TEXTURE2D  );
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLTextureCube,  "TextureCube",  GD_HLSL_TYPE_CLASS_TEXTURECUBE);
	GD_HLSL_DEFINE_BASIC_TYPE(HLSLSamplerState, "SamplerState", GD_HLSL_TYPE_CLASS_SAMPLER    );
#undef GD_HLSL_DEFINE_BASIC_TYPE

	// Scalar types.
#define GD_HLSL_DEFINE_SCALAR_TYPE(_Name, _DataType) \
	struct _Name final : public HLSLScalarType { \
		GDINL _Name() { \
			this->Name = #_DataType; \
			this->DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
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

	// Vector types
#define GD_HLSL_DEFINE_VECTOR_TYPE(_Name, _DataType, _ComponentsNumber) \
	struct _Name##_ComponentsNumber final : public HLSLVectorType{ \
		GDINL _Name##_ComponentsNumber() { \
			this->Name = #_DataType#_ComponentsNumber; \
			this->DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			this->ComponentsNumber = _ComponentsNumber; \
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

	// Matrix types. 
#define GD_HLSL_DEFINE_MATRIX_TYPE(_Name, _DataType, _RowsNumber, _ColumnsNumber) \
	struct _Name##_RowsNumber##x##_ColumnsNumber final : public HLSLMatrixType{ \
		GDINL _Name##_RowsNumber##x##_ColumnsNumber() { \
			this->Name = #_DataType#_RowsNumber"x"#_ColumnsNumber; \
			this->DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			this->RowsNumber = _RowsNumber; \
			this->ColumnsNumber = _ColumnsNumber; \
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

#undef GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE

	class HLSLParserImpl final : public IToolchainTool
	{
	private:
		StreamedLexer Lexer;
		Lexem CurrentLexem;

		ptrdiff_t AnonymousIdentifiersCount = -1;
			   HLSLScope*  GlobalScope;
		Vector<HLSLScope*> GlobalScopesStack;

	public /*Class API*/:
		GDINT ~HLSLParserImpl()	{ }
		GDINT  HLSLParserImpl(IToolchain* const  Toolchain, UniquePtr<InputStream>&& Stream)
			: IToolchainTool(Toolchain), Lexer(Toolchain, Forward<UniquePtr<InputStream>>(Stream), GetDefaultOptionsForHLSL())
		{	// Registering default types.
			this->EnterScope(&HLSLSuperGlobalScope);
			this->EnterScope(this->GlobalScope = new HLSLScope());
		}

		GDINT HLSLScope const* ParseShader();

	private:
		GDINT bool TryReadNextLexem  (                                                            );
		GDINT bool TryExpectLexem    (LexemContentType const ContentType                          );
		GDINT void    ExpectLexem    (LexemContentType const ContentType                          );
		GDINT bool TryExpectLexem    (LexemContentType const ContentType, StreamedLexerID const ID);
		GDINT void    ExpectLexem    (LexemContentType const ContentType, StreamedLexerID const ID);
		GDINT bool TryExpectNextLexem(                                                            );
		GDINT void    ExpectNextLexem(                                                            );
		GDINT bool TryExpectNextLexem(LexemContentType const ContentType                          );
		GDINT void    ExpectNextLexem(LexemContentType const ContentType                          );
		GDINT bool TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID);
		GDINT void    ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID);

		// Scope management:
		GDINL void EnterScope(HLSLScope* const CurrentScope) { this->GlobalScopesStack.PushLast(CurrentScope); }
		GDINL void LeaveScope(                             ) { this->GlobalScopesStack.PopLast(); }

		GDINT HLSLDefinition const* FindDefinitionInScope(String const& DefinitionName) const;
		GDINT HLSLDefinition const* FindDefinition       (String const& DefinitionName) const;
		GDINT HLSLType       const* FindTypeInScope      (String const& TypeName      ) const;
		GDINT HLSLType       const* FindType             (String const& TypeName      ) const;
		GDINT HLSLVariable   const* FindVariableInScope  (String const& VariableName  ) const;
		GDINT HLSLVariable   const* FindVariable         (String const& VariableName  ) const;
		GDINT HLSLFunction   const* FindFunctionInScope  (String const& FunctionName  ) const;
		GDINT HLSLFunction   const* FindFunction         (String const& FunctionName  ) const;
		
		GDINT void ValidateIdentifier(String const& Identifier);

		GDINT HLSLExpression const* ParseExpression(HLSLExpressionType const ExpectedExperssions                                           );
		GDINT HLSLDefinition const* ParseDefinition(HLSLDefinitionType const ExpectedDefinitions                                           );
		GDINT void                  ParseScope     (HLSLDefinitionType const ExpectedDefinitions, HLSLScope* const Scope = nullptr         );
		GDINT HLSLType       const* ParseType      (HLSLDefinitionType const ExpectedDefinitions, bool* const IsDefinition = nullptr       );
		GDINT HLSLType       const* ParseTypeName  (                                              bool  const DoIgnoreUndefinedType = false);
		GDINT HLSLCBuffer    const* ParseCBuffer   (                                                                                       );
		GDINT HLSLVariable   const* ParseVariable  (HLSLDefinition&& PrependingDefinition, bool const DoExpectSemicolon = true             );
		GDINT HLSLFunction   const* ParseFunction  (HLSLDefinition&& PrependingDefinition                                                  );
		GDINT HLSLExprColon  const* ParseExprColon (HLSLExprColonType const ExpectedExprColons                                             );
	};	// class HLSLParserImpl

	/// ==========================================================================================
	/// HLSLParserImpl class.
	/// ==========================================================================================
	
	/// ------------------------------------------------------------------------------------------
	/// Public class API.
	/// ------------------------------------------------------------------------------------------

	/// Parses all shader content.
	/// Input lexer position:  first lexem of shader.
	/// Output lexer position: End-Of-Stream.
	HLSLScope const* HLSLParserImpl::ParseShader()
	{
		this->TryReadNextLexem();
		for (;;) {
			if (this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_EOS) {
				break;
			}
			
			UniquePtr<HLSLDefinition const> Definition(this->ParseDefinition(HLSLDefinitionType(0
				| GD_HLSL_DEFINITION_TYPE
				| GD_HLSL_DEFINITION_VARIABLE
				| GD_HLSL_DEFINITION_FUNCTION 
				| GD_HLSL_DEFINITION_CONSTANTBUFFER)));
			this->GlobalScopesStack.GetLastElement()->InnerDefinitions.PushLast(Definition.Release());
		}

		return this->GlobalScope;
	}

	/// ------------------------------------------------------------------------------------------
	/// Internal class API. Scope parsing.
	/// ------------------------------------------------------------------------------------------

	/// Parses all content inside scope.
	/// Input lexer position:  first lexem of shader.
	/// Output lexer position: End-Of-Stream.
	void HLSLParserImpl::ParseScope(HLSLDefinitionType const ExpectedDefinitions, HLSLScope* const Scope /* = nullptr */)
	{
		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN);
		this->ExpectNextLexem(                                                        );
		for (;;) {	// Scanning our definition
			UniquePtr<HLSLDefinition const> Definition;
			if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_END)) {
				return;
			}
			Definition.Reset(this->ParseDefinition(ExpectedDefinitions));
			
			// Placing it into scope
			Scope->InnerDefinitions.PushLast(Definition.Release());
		}
	}

	/// Parses variable, function or structure definition.
	/// Input lexer position:  first lexem of definition.
	/// Output lexer position: first lexem after definition.
	HLSLDefinition const* HLSLParserImpl::ParseDefinition(HLSLDefinitionType const ExpectedDefinitions)
	{	// Lets check if this declaration contains an attribute (if it generally supports attribute declaration).
		// Common syntax is something like: [FirstAttributeName(Args, ...)] [ [SecondAttributeName(Args, ...)] ].
#if 0
		if ((ExpectedDefinitions & GD_HLSL_DEFINITION_ATTRIBUTE) != 0) {	// Yes, here is attribute. Definitions contain multiple attributes, so we are parsing them all.
			GD_NOT_IMPLEMENTED();
		}
#endif	// if 0

		if (this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD) {	// Lets determine what definition we have here.
			if ((!GD_HLSL_IS_KEYWORD_OF_TYPE(this->CurrentLexem.GetProcessedDataID(), DEFINING)) && (!GD_HLSL_IS_KEYWORD_OF_TYPE(this->CurrentLexem.GetProcessedDataID(), TYPE_MODIFIER)))	{
				HLSLParserErrorDesc static const UnexpectedKeywordError("unexpected keyword '%s'");
				throw HLSLParserErrorException(UnexpectedKeywordError.ToString(&this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
			} else if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_KEYWORD_TYPEDEF) {	// We are having a typedef here:
				if ((ExpectedDefinitions & GD_HLSL_DEFINITION_TYPEDEF) == 0) {	// Unexpected type definition.
					HLSLParserErrorDesc static const UnexpectedTypedefDefinition("unexpected type definition.");
					throw HLSLParserErrorException(UnexpectedTypedefDefinition.ToString(&this->CurrentLexem));
				}

				GD_NOT_IMPLEMENTED();
				// return this->ParseTypedef();
			} else if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_KEYWORD_CBUFFER) {	// We are having a constant buffer here:
				if ((ExpectedDefinitions & GD_HLSL_DEFINITION_CONSTANTBUFFER) == 0) {	// Unexpected constant buffer definition.
					HLSLParserErrorDesc static const UnexpectedCBufferDefinition("unexpected constant buffer definition.");
					throw HLSLParserErrorException(UnexpectedCBufferDefinition.ToString(&this->CurrentLexem));
				}

				return this->ParseCBuffer();
			}
		}

		// Parsing definition type modifier, if it exists,
		HLSLTypeModifier TypeModifier = GD_HLSL_TYPE_MODIFIER_UNKNOWN;
		if (this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD) {
			while (GD_HLSL_IS_KEYWORD_OF_TYPE(this->CurrentLexem.GetProcessedDataID(), TYPE_MODIFIER)) {
				TypeModifier = HLSLTypeModifier(TypeModifier | GD_BIT(this->CurrentLexem.GetProcessedDataID() - GD_HLSL_KEYWORD_TYPE_MODIFIER_BEGIN__ - 1));
				this->ExpectNextLexem();
			}
		}

		// Here we have a variable, function or struct definition.
		bool IsTypeDefinition = false;
		UniquePtr<HLSLType const> Type(this->ParseType(HLSLDefinitionType(ExpectedDefinitions & GD_HLSL_DEFINITION_TYPE), &IsTypeDefinition));
		UniquePtr<HLSLDefinition> Definition;
		if (this->CurrentLexem.GetContentType() != GD_LEXEM_CONTENT_TYPE_IDENTIFIER) {	// Was defined a structure so it is non required declare a instance.
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SEMICOLON);
			if (!IsTypeDefinition) {	// Something strange happened..
				HLSLParserErrorDesc static const IdentiferOrSemicolonExpectedError("identifier or ';' expected");
				throw HLSLParserErrorException(IdentiferOrSemicolonExpectedError.ToString(&this->CurrentLexem));
			}

			// Here is just a type declaration, nothing more.
			this->ExpectNextLexem();
			return Type.Release();
		} else {	// We are having a instance of this type here.
			Definition.Reset(new HLSLDefinition());
			Definition->Name = this->CurrentLexem.GetRawData();
			Definition->Type = Type.Release();
			Definition->TypeModifier = TypeModifier;
		}

		this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR);
		if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_OPERATOR_PARAMS_BEGIN) {	// Parsing function declaration here? which start with '(' operator.
			if ((ExpectedDefinitions & GD_HLSL_DEFINITION_FUNCTION) == 0) {	// Unexpected function definition.
				HLSLParserErrorDesc static const UnexpectedFunctionDefinition("unexpected function definition.");
				throw HLSLParserErrorException(UnexpectedFunctionDefinition.ToString(&this->CurrentLexem));
			}

			return this->ParseFunction(Move(*Definition.GetPointer()));
		} else {	// Parsing variable declaration here:
			if ((ExpectedDefinitions & (GD_HLSL_DEFINITION_VARIABLE | GD_HLSL_DEFINITION_ARGUMENT)) == 0) {	// Unexpected variable definition.
				HLSLParserErrorDesc static const UnexpectedVariableDefinition("unexpected variable definition.");
				throw HLSLParserErrorException(UnexpectedVariableDefinition.ToString(&this->CurrentLexem));
			}

			if (Type == &HLSLVoid) {	// Variables cannot use 'void' as type.
				HLSLParserErrorDesc static const VoidVariableTypeError("'void' variable type is not allowed.");
				throw HLSLParserErrorException(VoidVariableTypeError.ToString(&this->CurrentLexem));
			}

			return this->ParseVariable(Move(*Definition.GetPointer()), ((ExpectedDefinitions & GD_HLSL_DEFINITION_ARGUMENT) == 0));
		}
	}
	
	/// ------------------------------------------------------------------------------------------
	/// Specific code structures parsing.
	/// ------------------------------------------------------------------------------------------

	/// Parses type that is defined in current lexem.
	/// Input lexer position: first lexem of type. 
	/// Output lexer position: last lexem of type.
	HLSLType const* HLSLParserImpl::ParseType(HLSLDefinitionType const ExpectedDefinitions, bool* const IsDefinition /* = nullptr */)
	{	// Lets test if our type is something that touches structures or classes:
		if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_KEYWORD,GD_HLSL_KEYWORD_STRUCT)) {	// Type if structure or class. Lets check what is it:
			// 1) A structure declaration:   struct [ StructName ] [ : BaseClass ] { } [ InstanceDecl ] 
			// 2) A C-style struct instance: struct   StructName
			// First variant is valid only if struct declaration is expected.
			UniquePtr<HLSLStruct> Struct;
			this->ExpectNextLexem();
			if ( this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_IDENTIFIER) {	// We have named structure declaration C-style struct instance:
				String const TypeName = this->CurrentLexem.GetRawData();
				HLSLType const* const AlreadyDefinedType = this->ParseTypeName(true);
				 if (AlreadyDefinedType != nullptr)	{	// Type was already declared. We are having C-style struct instance or a redefinition error.
					if (AlreadyDefinedType->Class != GD_HLSL_KEYWORD_STRUCT) {	// Redifinition of type using struct/class keyword.
						HLSLParserErrorDesc static const RedefinitonAsStructError("redifinition of type '%s' using struct/class keyword.");
						throw HLSLParserErrorException(RedefinitonAsStructError.ToString(&this->CurrentLexem, TypeName.CStr()));
					}

					this->ExpectNextLexem();
					return AlreadyDefinedType;
				} else if ((ExpectedDefinitions & GD_HLSL_DEFINITION_TYPE) == 0) {	// We haven't expected a declaration. That means that if struct/class keyword used, that after it should come a type.
					HLSLParserErrorDesc static const UnexpectedStructDeclError("unexpected struct/class declaration.");
					throw HLSLParserErrorException(UnexpectedStructDeclError.ToString(&this->CurrentLexem));
				}

				// We need to validate identifier.
				this->ValidateIdentifier(TypeName);
				this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN);
				Struct.Reset(new HLSLStruct());
				Struct->Name = TypeName;
			} else if ((this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN))) {	// We have anonymous structure declaration.
				Struct.Reset(new HLSLStruct());
				Struct->Name = String::Format("__AnonymousStruct_%d", ++this->AnonymousIdentifiersCount);
			} else {	// Something that does not match our cases.
				HLSLParserErrorDesc static const IdentiferOrBraceExpectedError("identifier or '{'. expected.");
				throw HLSLParserErrorException(IdentiferOrBraceExpectedError.ToString(&this->CurrentLexem));
			}

			// Now we can expect only a structure type declaration.
			// Parsing struct body here.
			if (IsDefinition != nullptr) (*IsDefinition) = true;
			this->ParseScope(GD_HLSL_DEFINITION_VARIABLE, Struct.GetPointer());
			this->ExpectNextLexem();
			return Struct.Release();
		}
		else
		{	// Searching for predefined type.
			if (IsDefinition != nullptr) (*IsDefinition) = false;
			return this->ParseTypeName();
		}
	}

	/// Parses name of uxsed type.
	/// Input lexer position: first lexem of type name. 
	/// Output lexer position: first lexem after type name. 
	HLSLType const* HLSLParserImpl::ParseTypeName(bool const DoIgnoreUndefinedType /* = false */)
	{
		HLSLType const* HLSLDefinitionType = nullptr;
		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
		if (((HLSLDefinitionType = this->FindType(this->CurrentLexem.GetRawData())) == nullptr) && (!DoIgnoreUndefinedType)) {	// Type was not declared declared.
			HLSLParserErrorDesc static const TypeNotFoundError("type '%s' was not defined.");
			throw HLSLParserErrorException(TypeNotFoundError.ToString(&this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
		}

		this->ExpectNextLexem();
		return HLSLDefinitionType;
	}

	/// Parses variable that is defined in current lexem.
	/// Input lexer position: first lexem after variable name. 
	/// Output lexer position: first lexem after variable definition. 
	HLSLVariable const* HLSLParserImpl::ParseVariable(HLSLDefinition&& PrependingDefinition, bool const DoExpectSemicolon /* = true */)
	{
		UniquePtr<HLSLVariable> Variable(new HLSLVariable(Forward<HLSLDefinition>(PrependingDefinition)));
		if (this->TryExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_INDEX_BEGIN)) {	// Parsing array definitions.
			this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER);
			if ((Variable->ArrayIndex = this->CurrentLexem.GetProcessedDataInt()) == 0) {	// Zero-sized arrays are not allowed.
				HLSLParserErrorDesc static const ZeroSizedArrayError("Zero-Sized arrays are not allowed");
				throw HLSLParserErrorException(ZeroSizedArrayError.ToString(&this->CurrentLexem));
			}

			this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_INDEX_END);
			this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR                            );
		} else if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_OPERATOR_COLON) {	    // Pasring variable semantic, register and pack offset here.
			HLSLExprColonType ExpectedColonExprs = HLSLExprColonType(GD_HLSL_EXPRCOLON_TYPE_REGISTER | GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET);
			if ((Variable->Type->Class == GD_HLSL_TYPE_CLASS_SCALAR) || (Variable->Type->Class == GD_HLSL_TYPE_CLASS_VECTOR))
				ExpectedColonExprs = HLSLExprColonType(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC | ExpectedColonExprs);

			Variable->ExprColon = this->ParseExprColon(ExpectedColonExprs);
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR);
		}
		
		if (this->CurrentLexem.GetRawData() == '<') {	// Parsing annotations here.
			GD_NOT_IMPLEMENTED();
			return nullptr;
		}

		if (this->CurrentLexem.GetRawData() == '=')	{	// Parsing default values here.
			//     Variable->InitialValue = this->ParseExpression(GD_HLSL_EXPRESSION_UNKNOWN); // ???
			// if (Variable->InitialValue == nullptr;
			GD_NOT_IMPLEMENTED();
			return nullptr;
		}

		if (DoExpectSemicolon) {	
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SEMICOLON);
			this->ExpectNextLexem(                                                      );
		}

		return Variable.Release();
	}

	/// Parses function that is defined in current lexem.
	/// Input lexer position: first lexem after function name ('(' lexem). 
	/// Output lexer position: first lexem after function definition. 
	HLSLFunction const* HLSLParserImpl::ParseFunction(HLSLDefinition&& PrependingDefinition)
	{	// Parsing function arguments here.
		UniquePtr<HLSLFunction> Function(new HLSLFunction(Forward<HLSLDefinition>(PrependingDefinition)));
		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_PARAMS_BEGIN);
		for (;;) {	// Parsing function arguments here:
			HLSLArgumentAccessType ArgumentAccessType = GD_HLSL_ARGUMENT_IN;
			this->ExpectNextLexem();
			if ((this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_OPERATOR) && (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_OPERATOR_PARAMS_END)) break;
			if ((this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD ) && (GD_HLSL_IS_KEYWORD_OF_TYPE(this->CurrentLexem.GetProcessedDataID(), ARGUMENTS)))
			{	// Here we have a keyword that describes argument access type: 
				StackUnorderedMap<StreamedLexerKeyword, HLSLArgumentAccessType, 3> AccessTypeTranslationTable = {
					{ GD_HLSL_KEYWORD_IN   , GD_HLSL_ARGUMENT_IN    },
					{ GD_HLSL_KEYWORD_OUT  , GD_HLSL_ARGUMENT_OUT   },
					{ GD_HLSL_KEYWORD_INOUT, GD_HLSL_ARGUMENT_INOUT },
				};

				size_t const ArgumentAccessTypeIndex = AccessTypeTranslationTable.FindFirstElement(this->CurrentLexem.GetProcessedDataID());
				ArgumentAccessType = AccessTypeTranslationTable.GetElementAt(ArgumentAccessTypeIndex).Second;
				this->ExpectNextLexem();
			}

			UniquePtr<HLSLVariable> ArgumentDefinition(static_cast<HLSLVariable*>(const_cast<HLSLDefinition*>(this->ParseDefinition(GD_HLSL_DEFINITION_ARGUMENT))));
			HLSLArgument* Argument = new HLSLArgument(Move(*ArgumentDefinition.GetPointer()));
			Argument->AccsessType = ArgumentAccessType;
			Function->Arguments.PushLast(Argument);
			if ((this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_OPERATOR) && (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_OPERATOR_PARAMS_END)) break;
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_COMMA);
		}

		this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR);
		if ( this->CurrentLexem.GetProcessedDataID() == GD_HLSL_OPERATOR_COLON) {	// Pasring function semantic here.
			if (Function->Type == &HLSLVoid) {	// 'void' functions cannot have semantics.
				HLSLParserErrorDesc static const VoidFunctionSemanticError("'void' functions cannot have semantics.");
				throw HLSLParserErrorException(VoidFunctionSemanticError.ToString(&this->CurrentLexem));
			}

		    Function->Semantic = static_cast<HLSLSemantic const*>(this->ParseExprColon(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC));
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR);
		}

		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN);
		// We are not parsing anything here. Just copying a function body.
		BaseStringBuilder<CharAnsi> FunctionBody;
		FunctionBody.Append(CharAnsi('{'));
		for (size_t BracesBalance = 1; BracesBalance != 0; ) {	
			CharAnsi Character = this->Lexer.GetNextCharacter();
			switch (Character) {
				case CharAnsi('{'): BracesBalance += 1; break;
				case CharAnsi('}'): BracesBalance -= 1; break;
			}

			FunctionBody.Append(Character);
		}
		Function->Body = FunctionBody.ToString();
		this->ExpectNextLexem();
		return Function.Release();
	}

	/// Parses constant buffer definition.
	/// Input lexer position: 'cbuffer' lexem. 
	/// Output lexer position: first lexem after constant buffer definition.
	HLSLCBuffer const* HLSLParserImpl::ParseCBuffer()
	{
		this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER  );
		this->ValidateIdentifier(this->CurrentLexem.GetRawData());

		UniquePtr<HLSLCBuffer> ConstantBuffer(new HLSLCBuffer());
		ConstantBuffer->Name = this->CurrentLexem.GetRawData();

		this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR);
		if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_OPERATOR_COLON) {	    // Pasring constant buffer registeRHere.
			    ConstantBuffer->Register = static_cast<HLSLRegister const*>(this->ParseExprColon(GD_HLSL_EXPRCOLON_TYPE_REGISTER));
			if (ConstantBuffer->Register == nullptr)                return nullptr;
			if (ConstantBuffer->Register->Register != GD_HLSL_REGISTER_B)
			{	// Constant buffers may locate only in 'b' registers.
				HLSLParserErrorDesc static const InvalidCBufferRegisterError("constant buffers may locate only in 'b' registers.");
				throw HLSLParserErrorException(InvalidCBufferRegisterError.ToString(&this->CurrentLexem));
			}
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR);
		}

		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN);
		this->ParseScope (GD_HLSL_DEFINITION_VARIABLE, ConstantBuffer.GetPointer()    ); 
		this->ExpectNextLexem(                                                        );
		return ConstantBuffer.Release();
	}

	/// Parses expresseion after ':' operator.
	/// Input lexer position: ':' lexem. 
	/// Output lexer position: first lexem after parsed expression.
	HLSLExprColon const* HLSLParserImpl::ParseExprColon(HLSLExprColonType const ExpectedExprColons)
	{
		this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_COLON);
		this->ExpectNextLexem();
		if (this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD) {	// Here comes the 'register' or 'packoffset'
			if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_KEYWORD_REGISTER) {	// Here is register. Lets check if it was expected and parse it.
				if ((ExpectedExprColons & GD_HLSL_EXPRCOLON_TYPE_REGISTER) == 0) {	// Registers are not allowed here.
					HLSLParserErrorDesc static const RegisterIsNotAllowedError("'register' is not allowed here.");
					throw HLSLParserErrorException(RegisterIsNotAllowedError.ToString(&this->CurrentLexem));
				}

				this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_PARAMS_BEGIN);
				this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER                             );
				
				static StackUnorderedMap<CharAnsi, HLSLRegisterType, 4> const RegistersTranslationTable = {
					{ 'b', GD_HLSL_REGISTER_B },
					{ 't', GD_HLSL_REGISTER_T },
					{ 'c', GD_HLSL_REGISTER_C },
					{ 's', GD_HLSL_REGISTER_S },
				};

				size_t RegisterIndex = RegistersTranslationTable.FindFirstElement(this->CurrentLexem.GetRawData()[0]);
				if ((this->CurrentLexem.GetRawData().GetSize() == 2) ? ((RegisterIndex != SIZE_MAX) && (!CharAnsiHelpers::IsDigit(this->CurrentLexem.GetRawData()[1]))) : true)	{
					HLSLParserErrorDesc static const InvalidRegisterError("register '%s' is invalid.");
					throw HLSLParserErrorException(InvalidRegisterError.ToString(&this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
				}

				UniquePtr<HLSLRegister> Register(new HLSLRegister());
				Register->Register   = RegistersTranslationTable.GetElementAt(RegisterIndex).Second;
				Register->RegisterID = size_t(CharAnsiHelpers::ToDigit(this->CurrentLexem.GetRawData()[1]));

				this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_PARAMS_END);
				this->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR                             );
				return Register.Release();
			} else if (this->CurrentLexem.GetProcessedDataID() == GD_HLSL_KEYWORD_PACKOFFSET) {	// Here is register. Lets check if it was expected and parse it.
				if ((ExpectedExprColons & GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET) == 0) {	// Registers are not allowed here.
					HLSLParserErrorDesc static const PackOffsetIsNotAllowedError("'packoffset' is not allowed here.");
					throw HLSLParserErrorException(PackOffsetIsNotAllowedError.ToString(&this->CurrentLexem));
				}

				GD_NOT_IMPLEMENTED();
			}
		} else {	// Here comes semantic. 
			/// @todo Rewrite semantic name and ID extraction after 'String' class refactoring.
			this->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER);
			UniquePtr<HLSLSemantic> Semantic(new HLSLSemantic());
			bool const HasSemanticID = CharAnsiHelpers::IsDigit(this->CurrentLexem.GetRawData().GetLastElement());
			Semantic->SemanticID = (HasSemanticID ? size_t(CharAnsiHelpers::ToDigit(this->CurrentLexem.GetRawData().GetLastElement())) : 0);
			Semantic->Semantic   = HLSLSemanticTypeFromString(this->CurrentLexem.GetRawData().GetSubstring(0, this->CurrentLexem.GetRawData().GetSize() - (HasSemanticID ? 1 : 0)));
			/**/ if ( Semantic->Semantic == GD_HLSL_SEMANTIC_UNKNOWN) {	// Invalid semantic specified.
				HLSLParserErrorDesc static const InvalidSemanticError("Unknown semantic '%s'.");
				throw HLSLParserErrorException(InvalidSemanticError.ToString(&this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
			} else if ((ExpectedExprColons & GD_HLSL_EXPRCOLON_TYPE_SEMANTIC) == 0) {	// No semantics allowed here.
				HLSLParserErrorDesc static const SemanticIsNotAllowedError("semantic is not allowed here.");
				throw HLSLParserErrorException(SemanticIsNotAllowedError.ToString(&this->CurrentLexem));
			}

			this->ExpectNextLexem();
			return Semantic.Release();
		}

		return nullptr;
	}

	/// ------------------------------------------------------------------------------------------
	// bool HLSLParserImpl::Validate***(String const& ***)
	/// ------------------------------------------------------------------------------------------

	void HLSLParserImpl::ValidateIdentifier(String const& Identifier)
	{
		if (this->FindDefinitionInScope(Identifier) != nullptr) {	// Identifier was already declared in this scope.
			HLSLParserErrorDesc static const IdentifierWasAlreadyDefinedError("identifier '%s' was already defined in current scope.");
			throw HLSLParserErrorException(IdentifierWasAlreadyDefinedError.ToString(&this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
		}
	}

	/// ------------------------------------------------------------------------------------------
	/// Basic Pipeline and expectings.
	/// ------------------------------------------------------------------------------------------

	bool HLSLParserImpl::TryReadNextLexem()
	{
		if (this->Lexer.GetNextLexem(&this->CurrentLexem)) {	// Reading succeded, we need to skip comments comments until read token.
			if (this->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_COMMENT) {
				return this->TryReadNextLexem();
			}

			return true;
		}

		return false;
	}

	bool HLSLParserImpl::TryExpectLexem(LexemContentType const ContentType)
	{
		return (this->CurrentLexem.GetContentType() == ContentType);
	}

	void HLSLParserImpl::ExpectLexem(LexemContentType const ContentType)
	{
		if (!this->TryExpectLexem(ContentType)) {	// Unexpected lexem type.
			HLSLParserErrorDesc static const UnexpectedLexemError("unexpected %s. Expected %s.");
			throw HLSLParserErrorException(UnexpectedLexemError.ToString(&this->CurrentLexem, &this->CurrentLexem, LexemContentTypeToString(this->CurrentLexem.GetContentType()), LexemContentTypeToString(ContentType)));
		}
	}

	bool HLSLParserImpl::TryExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (this->TryExpectLexem(ContentType)) {
			if (this->CurrentLexem.GetProcessedDataID() == ID) {
				return true;
			}
		}
		return false;
	}

	void HLSLParserImpl::ExpectLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		this->ExpectLexem(ContentType);

		// Next lexem exists and has expected content type.
		if (this->CurrentLexem.GetProcessedDataID() != ID)	{	// Unexpected lexem value.
			HLSLParserErrorDesc static const UnexpectedLexemValueError("unexpected '%s'.");
			throw HLSLParserErrorException(UnexpectedLexemValueError.ToString(&this->CurrentLexem, &this->CurrentLexem, this->CurrentLexem.GetRawData().CStr()));
		}
	}

	bool HLSLParserImpl::TryExpectNextLexem()
	{
		return this->TryReadNextLexem();
	}

	void HLSLParserImpl::ExpectNextLexem()
	{
		if (!this->TryExpectNextLexem()) {	// Unexpected end of stream while reading lexem.
			HLSLParserErrorDesc static const EndOfStreamInVariableDeclError("unexpected End-Of-Stream.");
			throw HLSLParserErrorException(EndOfStreamInVariableDeclError.ToString(&this->CurrentLexem, &this->CurrentLexem));
		}
	}

	bool HLSLParserImpl::TryExpectNextLexem(LexemContentType const ContentType)
	{
		this->ExpectNextLexem();
		return this->TryExpectLexem(ContentType);
	}

	void HLSLParserImpl::ExpectNextLexem(LexemContentType const ContentType)
	{
		this->ExpectNextLexem();
		this->ExpectLexem(ContentType);
	}

	bool HLSLParserImpl::TryExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		if (this->TryExpectNextLexem(ContentType)) {
			if (this->CurrentLexem.GetProcessedDataID() == ID) {
				return true;
			}
		}
		return false;
	}

	void HLSLParserImpl::ExpectNextLexem(LexemContentType const ContentType, StreamedLexerID const ID)
	{
		this->ExpectNextLexem();
		this->ExpectLexem(ContentType, ID);
	}

	/// ==========================================================================================
	// HLSL*** const* HLSLScope::Find***
	/// ==========================================================================================

	HLSLDefinition const* HLSLScope::FindDefinition(String const& DefinitionName) const
	{	// Searching in specified scope.
		size_t const RequiredDefinitionIndex = this->InnerDefinitions.FindFirstElement([&DefinitionName](HLSLDefinition const* const Definition) -> bool {
			return (Definition->Name == DefinitionName);
		});

		return ((RequiredDefinitionIndex != SIZE_MAX) ? this->InnerDefinitions[RequiredDefinitionIndex] : nullptr);
	}

	HLSLType const* HLSLScope::FindType(String const& TypeName) const
	{
		return HLSLDynamicCast<HLSLType const*>(this->FindDefinition(TypeName));
	}

	HLSLVariable const* HLSLScope::FindVariable(String const& VariableName) const
	{
		return HLSLDynamicCast<HLSLVariable const*>(this->FindDefinition(VariableName));
	}

	HLSLFunction const* HLSLScope::FindFunction(String const& FunctionName) const
	{
		return HLSLDynamicCast<HLSLFunction const*>(this->FindDefinition(FunctionName));
	}

	/// ==========================================================================================
	// HLSL*** const* HLSLParserImpl::Find***[InScope]
	/// ==========================================================================================

	HLSLDefinition const* HLSLParserImpl::FindDefinition(String const& DefinitionName) const
	{	// Searching in all stack of our declarations.
		for (auto const& DefinitionScope : Reverse(this->GlobalScopesStack))
		{	// Definition was not found in current scope, searching in lower one.
			HLSLDefinition const* const FoundDefinition = DefinitionScope->FindDefinition(DefinitionName);
			if (FoundDefinition != nullptr) return FoundDefinition;
		}

		return nullptr;
	}

	HLSLType const* HLSLParserImpl::FindType(String const& TypeName) const
	{
		return HLSLDynamicCast<HLSLType const*>(this->FindDefinition(TypeName));
	}

	HLSLVariable const* HLSLParserImpl::FindVariable(String const& VariableName) const
	{
		return HLSLDynamicCast<HLSLVariable const*>(this->FindDefinition(VariableName));
	}

	HLSLFunction const* HLSLParserImpl::FindFunction(String const& FunctionName) const
	{
		return HLSLDynamicCast<HLSLFunction const*>(this->FindDefinition(FunctionName));
	}

	HLSLDefinition const* HLSLParserImpl::FindDefinitionInScope(String const& DefinitionName) const
	{	// Searching in top scope.
		auto const& TopDefinitionScope = this->GlobalScopesStack.GetLastElement();
		return TopDefinitionScope->FindDefinition(DefinitionName);
	}

	HLSLType const* HLSLParserImpl::FindTypeInScope(String const& TypeName) const
	{
		auto const& TopTypeScope = this->GlobalScopesStack.GetLastElement();
		return TopTypeScope->FindType(TypeName);
	}

	HLSLVariable const* HLSLParserImpl::FindVariableInScope(String const& VariableName) const
	{	
		auto const& TopDefinitionScope = this->GlobalScopesStack.GetLastElement();
		return TopDefinitionScope->FindVariable(VariableName);
	}

	HLSLFunction const* HLSLParserImpl::FindFunctionInScope(String const& FunctionName) const
	{
		auto const& TopFunctionScope = this->GlobalScopesStack.GetLastElement();
		return TopFunctionScope->FindFunction(FunctionName);
	}

	/// ==========================================================================================//
	/// HLSLParser class
	/// ==========================================================================================//

	HLSLScope const* HLSLParser::ParseShader(UniquePtr<InputStream>&& Stream)
	{
		return HLSLParserImpl(this->Toolchain, Forward<UniquePtr<InputStream>>(Stream)).ParseShader();
	}

	Str HLSLSemanticToStr(HLSLSemanticType const Semantic)
	{
		static Str const SemanticStrings[] = {
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
		return SemanticStrings[static_cast<size_t>(Semantic)];
	}

	static HLSLSemanticType HLSLSemanticTypeFromString(String const& Semantic)
	{
		static StackUnorderedMap<HashCode, HLSLSemanticType, GD_HLSL_SEMANTICS_COUNT> const SemanticsList = {
#	  define GD_HLSL_DEFINE_SEMANTIC(Semantic) { String(#Semantic).GetHashCode(), GD_HLSL_SEMANTIC_##Semantic }
			 GD_HLSL_DEFINE_SEMANTIC(BINORMAL),
			 GD_HLSL_DEFINE_SEMANTIC(BLENDINDICES),
			 GD_HLSL_DEFINE_SEMANTIC(BLENDWEIGHT),
			 GD_HLSL_DEFINE_SEMANTIC(COLOR),
			 GD_HLSL_DEFINE_SEMANTIC(NORMAL),
			 GD_HLSL_DEFINE_SEMANTIC(POSITION),
	//		 GD_HLSL_DEFINE_SEMANTIC(POSITIONT),
	//		 GD_HLSL_DEFINE_SEMANTIC(PSIZE),
			 GD_HLSL_DEFINE_SEMANTIC(TANGENT),
			 GD_HLSL_DEFINE_SEMANTIC(TEXCOORD),
	//		 GD_HLSL_DEFINE_SEMANTIC(FOG),
			 GD_HLSL_DEFINE_SEMANTIC(TESSFACTOR),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_ClipDistance),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_CullDistance),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_Coverage),
			 GD_HLSL_DEFINE_SEMANTIC(SV_Depth),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_DispatchThreadID),
			 GD_HLSL_DEFINE_SEMANTIC(SV_DomainLocation),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_GroupID),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_GroupIndex),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_GroupThreadID),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_GSInstanceID),
			 GD_HLSL_DEFINE_SEMANTIC(SV_InsideTessFactor),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_IsFrontFace),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_OutputControlPointID),
			 GD_HLSL_DEFINE_SEMANTIC(SV_Position),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_RenderTargetArrayIndex),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_SampleIndex),
			 GD_HLSL_DEFINE_SEMANTIC(SV_Target),
			 GD_HLSL_DEFINE_SEMANTIC(SV_TessFactor),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_ViewportArrayIndex),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_InstanceID),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_PrimitiveID),
	//		 GD_HLSL_DEFINE_SEMANTIC(SV_VertexID),
#	undef    GD_HLSL_DEFINE_SEMANTIC
		};

		size_t const SemanticIndex  = SemanticsList.FindFirstElement(Semantic.GetHashCode());
		return     ((SemanticIndex != SIZE_MAX) ? SemanticsList.GetElementAt(SemanticIndex).Second : GD_HLSL_SEMANTIC_UNKNOWN);
	}

	extern HRISemantic HLSLSemanticToHRI(HLSLSemanticType const Semantic)
	{
		static HRISemantic const HLSL2HRITranslationsTable[] = {
			GD_HRI_SEMANTIC_UNKNOWN,    // GD_HLSL_SEMANTIC_UNKNOWN
			GD_HRI_SEMANTIC_BINORMAL,
			GD_HRI_SEMANTIC_BLENDINDICES,
			GD_HRI_SEMANTIC_BLENDWEIGHT,
			GD_HRI_SEMANTIC_COLOR,
			GD_HRI_SEMANTIC_NORMAL,
			GD_HRI_SEMANTIC_POSITION,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_POSITIONT,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_PSIZE,
			GD_HRI_SEMANTIC_TANGENT,
			GD_HRI_SEMANTIC_TEXCOORD,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_FOG,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_TESSFACTOR,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_ClipDistance,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_CullDistance,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_Coverage,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_Depth,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_DispatchThreadID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_DomainLocation,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_GroupID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_GroupIndex,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_GroupThreadID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_GSInstanceID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_InsideTessFactor,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_IsFrontFace,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_OutputControlPointID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_Position,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_RenderTargetArrayIndex,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_SampleIndex,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_Target
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_TessFactor,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_ViewportArrayIndex,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_InstanceID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_PrimitiveID,
			GD_HRI_SEMANTIC_UNKNOWN,	// GD_HRI_SEMANTIC_SV_VertexID,
		};

		GD_ASSERT(Semantic < GD_HLSL_SEMANTICS_COUNT, "Invalid semantic specified");
		return HLSL2HRITranslationsTable[static_cast<size_t>(Semantic)];
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
		return HRI2HLSLTranslationsTable[static_cast<size_t>(Semantic)];
	}

	GDINT static StreamedLexerOptions const& GetDefaultOptionsForHLSL()
	{
#if (!defined(__INTELLISENSE__)) // IntelliSence marks following code with errors. Just let in not parse it.
		static StreamedLexerOptions const HlslLexerOptions(
			StreamedLexerKeywordsList({
				{ GD_HLSL_KEYWORD_STRUCT,                     "struct" },
				{ GD_HLSL_KEYWORD_TYPEDEF,                    "typedef" },
				{ GD_HLSL_KEYWORD_CBUFFER,                    "cbuffer" },
				{ GD_HLSL_KEYWORD_TYPE_MODIFIER_CONST,        "const" },
				{ GD_HLSL_KEYWORD_TYPE_MODIFIER_ROW_MAJOR,    "row_major" },
				{ GD_HLSL_KEYWORD_TYPE_MODIFIER_COLUMN_MAJOR, "column_major" },
				{ GD_HLSL_KEYWORD_IN,                         "in" },
				{ GD_HLSL_KEYWORD_OUT,                        "out" },
				{ GD_HLSL_KEYWORD_INOUT,                      "inout" },
				{ GD_HLSL_KEYWORD_REGISTER,                   "register" },
				{ GD_HLSL_KEYWORD_PACKOFFSET,                 "packoffset" },
			}),
			StreamedLexerOperatorsList({
				{ GD_HLSL_OPERATOR_INDEX_BEGIN,    "[" },
				{ GD_HLSL_OPERATOR_INDEX_END,      "]" },
				{ GD_HLSL_OPERATOR_SCOPE_BEGIN,    "{" },
				{ GD_HLSL_OPERATOR_SCOPE_END,      "}" },
				{ GD_HLSL_OPERATOR_PARAMS_BEGIN,   "(" },
				{ GD_HLSL_OPERATOR_PARAMS_END,     ")" },
				{ GD_HLSL_OPERATOR_TEMPLATE_BEGIN, "<" },
				{ GD_HLSL_OPERATOR_TEMPLATE_END,   ">" },
				{ GD_HLSL_OPERATOR_COLON,          ":" },
				{ GD_HLSL_OPERATOR_SEMICOLON,      ";" },
				{ GD_HLSL_OPERATOR_COMMA,          "," },
			}),
			"//", "/*", "*/",
			'x', '\0', 'b', '.'
		);
		return HlslLexerOptions;
#endif	// if (!defined(__INTELLISENSE__))
	}


GD_NAMESPACE_END

#endif
