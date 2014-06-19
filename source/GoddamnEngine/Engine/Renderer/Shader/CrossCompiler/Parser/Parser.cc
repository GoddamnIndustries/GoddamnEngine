//////////////////////////////////////////////////////////////////////////
/// Parser.hh - HLSL parser implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 14.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Parser/Parser.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.hh>
#include <GoddamnEngine/Core/Containers/Pointer/UniquePtr.hh>
#include <GoddamnEngine/Core/Containers/Containers.hh>
#include <GoddamnEngine/Core/Containers/Map/Map.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

/// Define this to make our parser parse things that cannot be translated into GLSL
// #define GD_HRI_SHADERCC_PARSE_UNTRANSLATABLE

GD_NAMESPACE_BEGIN
	
	using namespace StreamedLexerDefaultOptions;
	typedef Str HRIShaderCrossCompilerErrorDesc;

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

	static HLSLScope HLSLSuperGlobalScope;
#define GD_HLSL_REGISTER_IN_SUPERGLOBAL_SCOPE() \
	do { \
		HLSLSuperGlobalScope.InnerDefinitions.PushLast(self); \
	} while(false)

	// Basic types.
#define GD_HLSL_DEFINE_BASIC_TYPE(_Name, _HLSLName, _Class) \
	struct _Name final : public HLSLType { \
		GDINL _Name() : HLSLType(_Class) { \
			self->Name = _HLSLName; \
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
			self->Name = #_DataType; \
			self->DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
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
			self->Name = #_DataType#_ComponentsNumber; \
			self->DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			self->ComponentsNumber = _ComponentsNumber; \
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
			self->Name = #_DataType#_RowsNumber"x"#_ColumnsNumber; \
			self->DataType = GD_HLSL_TYPE_DATA_TYPE_##_DataType; \
			self->RowsNumber = _RowsNumber; \
			self->ColumnsNumber = _ColumnsNumber; \
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

	extern Str HLSLSemanticToStr(HLSLSemanticType const Semantic)
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

	extern HLSLSemanticType HLSLSemanticTypeFromString(String const& Semantic)
	{
		static StackUnorderedMap<HashSumm, HLSLSemanticType, GD_HLSL_SEMANTICS_COUNT> const SemanticsList = {
#	  define GD_HLSL_DEFINE_SEMANTIC(Semantic) { String(#Semantic).GetHashSumm(), GD_HLSL_SEMANTIC_##Semantic }
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

		size_t const SemanticIndex  = SemanticsList.FindFirstElement(Semantic.GetHashSumm());
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

	GDINT static StreamedLexerOptions const& GetDefaultOptionsForHlsl()
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

	////////////////////////////////////////////////////////////////////////////
	/// HLSLParserImpl class
	////////////////////////////////////////////////////////////////////////////

	class HLSLParserImpl final : public IToolchainTool
	{
	private:
		Stream* const ShaderSourceInputStream;
		StreamedLexer Lexer;
		Lexem CurrentLexem;

		ptrdiff_t AnonymousIdentifiersCount = -1;
			   HLSLScope*  GlobalScope;
		Vector<HLSLScope*> GlobalScopesStack;

	public /*Class API*/:
		GDINT ~HLSLParserImpl()	{ }
		GDINT  HLSLParserImpl(IToolchain* const  Toolchain, Stream* const  ShaderSourceInputStream)
			: IToolchainTool(Toolchain), ShaderSourceInputStream(ShaderSourceInputStream), Lexer(Toolchain, GetDefaultOptionsForHlsl())
		{	// Registering default types.
			self->EnterScope(&HLSLSuperGlobalScope);
			self->EnterScope(self->GlobalScope = new HLSLScope());
		}

		GDINT HLSLScope const* ParseShader();

	private:
		GDINT bool TryReadNextLexem(                                                      );
		GDINT bool  ExpectNextLexem(                                                      );
		GDINT bool  ExpectNextLexem(LexemContentType ContentType                          );
		GDINT bool  ExpectNextLexem(LexemContentType ContentType, StreamedLexerId const Id);
		GDINT bool      ExpectLexem(LexemContentType ContentType                          );
		GDINT bool      ExpectLexem(LexemContentType ContentType, StreamedLexerId const Id);

		// Scope management:
		GDINL void EnterScope(HLSLScope* const CurrentScope) { self->GlobalScopesStack.PushLast(CurrentScope); }
		GDINL void LeaveScope(                             ) { self->GlobalScopesStack.PopLast(); }

		GDINT HLSLDefinition const* FindDefinitionInScope(String const& DefinitionName) const;
		GDINT HLSLDefinition const* FindDefinition       (String const& DefinitionName) const;
		GDINT HLSLType       const* FindTypeInScope      (String const& TypeName      ) const;
		GDINT HLSLType       const* FindType             (String const& TypeName      ) const;
		GDINT HLSLVariable   const* FindVariableInScope  (String const& VariableName  ) const;
		GDINT HLSLVariable   const* FindVariable         (String const& VariableName  ) const;
		GDINT HLSLFunction   const* FindFunctionInScope  (String const& FunctionName  ) const;
		GDINT HLSLFunction   const* FindFunction         (String const& FunctionName  ) const;
		
		GDINT bool ValidateIdentifier(String const& Identifier);

		GDINT HLSLExpression const* ParseExpression(HLSLExpressionType const ExpectedExperssions);
		GDINT HLSLDefinition const* ParseDefinition(HLSLDefinitionType const ExpectedDefinitions);
		GDINT bool                  ParseScope     (HLSLDefinitionType const ExpectedDefinitions, HLSLScope* const Scope = nullptr);
		GDINT HLSLType       const* ParseType      (HLSLDefinitionType const ExpectedDefinitions, bool* const IsDefinition = nullptr);
		GDINT HLSLType       const* ParseTypeName  (                                              bool  const DoIgnoreUndefinedType = false);
		GDINT HLSLCBuffer    const* ParseCBuffer   (                                            );
		GDINT HLSLVariable   const* ParseVariable  (HLSLDefinition const* const PrependingDefinition, bool const DoExpectSemicolon = true);
		GDINT HLSLFunction   const* ParseFunction  (HLSLDefinition const* const PrependingDefinition);
		GDINT HLSLExprColon  const* ParseExprColon (HLSLExprColonType const ExpectedExprColons);
	};	// class HLSLParserImpl

	//////////////////////////////////////////////////////////////////////////
	/// HLSL** const* HLSLParserImpl::Parse***
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Parses all shader content.
	// Input lexer position:  first lexem of shader.
	// Output lexer position: End-Of-Stream.
	HLSLScope const* HLSLParserImpl::ParseShader()
	{
		if (!self->TryReadNextLexem()) return nullptr;
		while (!self->Toolchain->WasExceptionRaised())
		{
			if (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_EOS) break;
			UniquePtr<HLSLDefinition const> Definition(self->ParseDefinition(HLSLDefinitionType(0
				| GD_HLSL_DEFINITION_TYPE
				| GD_HLSL_DEFINITION_VARIABLE
				| GD_HLSL_DEFINITION_FUNCTION 
				| GD_HLSL_DEFINITION_CONSTANTBUFFER)));
			if (Definition.GetPointer() == nullptr) return nullptr;
			self->GlobalScopesStack.GetLastElement()->InnerDefinitions.PushLast(Definition.Release());
		}

		return self->GlobalScope;
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses all content inside scope.
	// Input lexer position:  first lexem of shader.
	// Output lexer position: End-Of-Stream.
	bool HLSLParserImpl::ParseScope(HLSLDefinitionType const ExpectedDefinitions, HLSLScope* const Scope /* = nullptr */)
	{
		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN)) return false;
		if (!self->ExpectNextLexem(                                                        )) return false;
		for (;;)
		{	// Scanning our definition
			UniquePtr<HLSLDefinition const> Definition;
			/**/ if ((self->CurrentLexem.GetContentType()     == GD_LEXEM_CONTENT_TYPE_OPERATOR)
				  && (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_SCOPE_END))    return true;
			else if ((Definition.Reset(self->ParseDefinition(ExpectedDefinitions))) == nullptr)  return false;
			
			// Placing it into scope
			Scope->InnerDefinitions.PushLast(Definition.Release());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses variable, function or structure definition.
	// Input lexer position:  first lexem of definition.
	// Output lexer position: first lexem after definition.
	HLSLDefinition const* HLSLParserImpl::ParseDefinition(HLSLDefinitionType const ExpectedDefinitions)
	{	// Lets check if this declaration contains an attribute (if it generally supports attribute declaration).
		// Common syntax is something like: [FirstAttributeName(Args, ...)] [ [SecondAttributeName(Args, ...)] ].
#if 0
		if ((ExpectedDefinitions & GD_HLSL_DEFINITION_ATTRIBUTE) != 0)
		{	// Yes, here is attribute. Definitions contain multiple attributes, so we are parsing them all.
			GD_NOT_IMPLEMENTED();
		}
#endif	// if 0

		if (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD)
		{	// Lets determine what definition we have here.
			/**/ if ((!GD_HLSL_IS_KEYWORD_OF_TYPE(self->CurrentLexem.GetProcessedDataId(), DEFINING)) && (!GD_HLSL_IS_KEYWORD_OF_TYPE(self->CurrentLexem.GetProcessedDataId(), TYPE_MODIFIER)))
			{	// This keyword contains something else..
				static HRIShaderCrossCompilerErrorDesc const UnexpectedKeywordError("unexpected keyword '%s'");
				self->RaiseError(UnexpectedKeywordError, &self->CurrentLexem.GetRawData()[0]);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}
			else if (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_KEYWORD_TYPEDEF)
			{	// We are having a typedef here:
				if ((ExpectedDefinitions & GD_HLSL_DEFINITION_TYPEDEF) == 0)
				{	// Unexpected type definition.
					static HRIShaderCrossCompilerErrorDesc const UnexpectedTypedefDefinition("unexpected type definition.");
					self->RaiseError(UnexpectedTypedefDefinition);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

					return nullptr;
				}

				GD_NOT_IMPLEMENTED();
				return nullptr;
				// return self->ParseTypedef();
			}
			else if (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_KEYWORD_CBUFFER)
			{	// We are having a constant buffer here:
				if ((ExpectedDefinitions & GD_HLSL_DEFINITION_CONSTANTBUFFER) == 0)
				{	// Unexpected constant buffer definition.
					static HRIShaderCrossCompilerErrorDesc const UnexpectedCBufferDefinition("unexpected constant buffer definition.");
					self->RaiseError(UnexpectedCBufferDefinition);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

					return nullptr;
				}

				return self->ParseCBuffer();
			}
		}

		// Parsing definition type modifier, if it exists,
		HLSLTypeModifier TypeModifier = GD_HLSL_TYPE_MODIFIER_UNKNOWN;
		if (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD)
			while (GD_HLSL_IS_KEYWORD_OF_TYPE(self->CurrentLexem.GetProcessedDataId(), TYPE_MODIFIER))
			{
				TypeModifier = HLSLTypeModifier(TypeModifier | GD_BIT(self->CurrentLexem.GetProcessedDataId() - GD_HLSL_KEYWORD_TYPE_MODIFIER_BEGIN__ - 1));
				if (!self->ExpectNextLexem()) return nullptr;
			}

		// Here we have a variable, function or struct definition.
		bool IsTypeDefinition = false;
		UniquePtr<HLSLType const> Type(self->ParseType(HLSLDefinitionType(ExpectedDefinitions & GD_HLSL_DEFINITION_TYPE), &IsTypeDefinition));
		UniquePtr<HLSLDefinition> Definition;
		if (Type.GetPointer() == nullptr) return nullptr;
		if (self->CurrentLexem.GetContentType() != GD_LEXEM_CONTENT_TYPE_IDENTIFIER)
		{	// Was defined a structure so it is non required declare a instance.
			if ((!IsTypeDefinition) || (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SEMICOLON)))
			{	// Something strange happened..
				static HRIShaderCrossCompilerErrorDesc const IdentiferOrSemicolonExpectedError("identifier or ';' expected");
				self->RaiseError(IdentiferOrSemicolonExpectedError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			// Here is just a type declaration, nothing more.
			if (!self->ExpectNextLexem()) return nullptr;
			return Type.Release();
		}
		else
		{	// We are having a instance of this type here.
			Definition.Reset(new HLSLDefinition());
			Definition->Name = self->CurrentLexem.GetRawData();
			Definition->Type = Type.Release();
			Definition->TypeModifier = TypeModifier;
		}

		if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) return nullptr;
		if ( self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_PARAMS_BEGIN)
		{	// Parsing function declaration here? which start with '(' operator.
			if ((ExpectedDefinitions & GD_HLSL_DEFINITION_FUNCTION) == 0)
			{	// Unexpected function definition.
				static HRIShaderCrossCompilerErrorDesc const UnexpectedFunctionDefinition("unexpected function definition.");
				self->RaiseError(UnexpectedFunctionDefinition);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			return self->ParseFunction(Definition.GetPointer());
		}
		else
		{	// Parsing variable declaration here:
			if ((ExpectedDefinitions & (GD_HLSL_DEFINITION_VARIABLE | GD_HLSL_DEFINITION_ARGUMENT)) == 0)
			{	// Unexpected variable definition.
				static HRIShaderCrossCompilerErrorDesc const UnexpectedVariableDefinition("unexpected variable definition.");
				self->RaiseError(UnexpectedVariableDefinition);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			if (Type == &HLSLVoid)
			{	// Variables cannot use 'void' as type.
				static HRIShaderCrossCompilerErrorDesc const VoidVariableTypeError("'void' variable type is not allowed.");
				self->RaiseError(VoidVariableTypeError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			return self->ParseVariable(Definition.GetPointer(), ((ExpectedDefinitions & GD_HLSL_DEFINITION_ARGUMENT) == 0));
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Parses type that is defined in current lexem.
	// Input lexer position: first lexem of type. 
	// Output lexer position: last lexem of type.
	HLSLType const* HLSLParserImpl::ParseType(HLSLDefinitionType const ExpectedDefinitions, bool* const IsDefinition /* = nullptr */)
	{	// Lets test if our type is something that touches structures or classes:
		if (   (self->CurrentLexem.GetContentType    () == GD_LEXEM_CONTENT_TYPE_KEYWORD)
			&& (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_KEYWORD_STRUCT))
		{	// Type if structure or class. Lets check what is it:
			// 1) A structure declaration:   struct [ StructName ] [ : BaseClass ] { } [ InstanceDecl ] 
			// 2) A C-style struct instance: struct   StructName
			// First variant is valid only if struct declaration is expected.
			UniquePtr<HLSLStruct> Struct;
			if (!self->ExpectNextLexem()) return nullptr;
			if ( self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_IDENTIFIER)
			{	// We have named structure declaration C-style struct instance:
				String const TypeName = self->CurrentLexem.GetRawData();
				HLSLType const* const AlreadyDefinedType = self->ParseTypeName(true);
				/**/ if (AlreadyDefinedType != nullptr)
				{	// Type was already declared. We are having C-style struct instance or a redefinition error.
					if (AlreadyDefinedType->Class != GD_HLSL_KEYWORD_STRUCT)
					{	// Redifinition of type using struct/class keyword.
						static HRIShaderCrossCompilerErrorDesc const RedefinitonAsStructError("redifinition of type '%s' using struct/class keyword.");
						self->RaiseError(RedefinitonAsStructError, &TypeName[0]);
						self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

						return nullptr;
					}

					if (!self->ExpectNextLexem()) return nullptr;
					return AlreadyDefinedType;
				}
				else if ((ExpectedDefinitions & GD_HLSL_DEFINITION_TYPE) == 0)
				{	// We haven't expected a declaration. That means that if struct/class keyword used, that after it should come a type.
					static HRIShaderCrossCompilerErrorDesc const UnexpectedStructDeclError("unexpected struct/class declaration.");
					self->RaiseError(UnexpectedStructDeclError);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

					return nullptr;
				}

				// We need to validate identifier.
				if (!self->ValidateIdentifier(TypeName))                                              return nullptr;
				if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN)) return nullptr;
				Struct.Reset(new HLSLStruct());
				Struct->Name = TypeName;
			}
			else if ((self->CurrentLexem.GetContentType()     == GD_LEXEM_CONTENT_TYPE_OPERATOR) 
				  && (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_SCOPE_BEGIN))
			{	// We have anonymous structure declaration.
				Struct.Reset(new HLSLStruct());
				Struct->Name = String::Format("__AnonymousStruct_%d", ++self->AnonymousIdentifiersCount);
			}
			else
			{	// Something that does not match our cases.
				static HRIShaderCrossCompilerErrorDesc const IdentiferOrBraceExpectedError("identifier or '{'. expected.");
				self->RaiseError(IdentiferOrBraceExpectedError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			// Now we can expect only a structure type declaration.
			// Parsing struct body here.
			if (IsDefinition != nullptr) (*IsDefinition) = true;
			if (!self->ParseScope(GD_HLSL_DEFINITION_VARIABLE, Struct.GetPointer())) return nullptr;
			if (!self->ExpectNextLexem())                                            return nullptr;
			return Struct.Release();
		}
		else
		{	// Searching for predefined type.
			if (IsDefinition != nullptr) (*IsDefinition) = false;
			return self->ParseTypeName();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses name of used type.
	// Input lexer position: first lexem of type name. 
	// Output lexer position: first lexem after type name. 
	HLSLType const* HLSLParserImpl::ParseTypeName(bool const DoIgnoreUndefinedType /* = false */)
	{
		HLSLType const* HLSLDefinitionType = nullptr;
		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER)) return nullptr;
		if (((HLSLDefinitionType = self->FindType(self->CurrentLexem.GetRawData())) == nullptr) && (!DoIgnoreUndefinedType))
		{	// Type was not declared declared.
			static HRIShaderCrossCompilerErrorDesc const TypeNotFoundError("type '%s' was not defined.");
			self->RaiseError(TypeNotFoundError, &self->CurrentLexem.GetRawData()[0]);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return nullptr;
		}

		return (self->ExpectNextLexem() ? HLSLDefinitionType : nullptr);
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses variable that is defined in current lexem.
	// Input lexer position: first lexem after variable name. 
	// Output lexer position: first lexem after variable definition. 
	HLSLVariable const* HLSLParserImpl::ParseVariable(HLSLDefinition const* const PrependingDefinition, bool const DoExpectSemicolon /* = true */)
	{
		UniquePtr<HLSLVariable> Variable(new HLSLVariable(*PrependingDefinition));
		/**/ if (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_INDEX_BEGIN)
		{	// Parsing array definitions.
			if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_CONSTANT_INTEGER)) return nullptr;
			if ((Variable->ArrayIndex = self->CurrentLexem.GetProcessedDataInt()) == 0)
			{	// Zero-sized arrays are not allowed.
				static HRIShaderCrossCompilerErrorDesc const ZeroSizedArrayError("Zero-Sized arrays are not allowed");
				self->RaiseError(ZeroSizedArrayError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_INDEX_END)) return nullptr;
			if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR                            )) return nullptr;
		}
		else if (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_COLON)
		{	    // Pasring variable semantic, register and pack offset here.
			HLSLExprColonType ExpectedColonExprs = HLSLExprColonType(GD_HLSL_EXPRCOLON_TYPE_REGISTER | GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET);
			if ((Variable->Type->Class == GD_HLSL_TYPE_CLASS_SCALAR) || (Variable->Type->Class == GD_HLSL_TYPE_CLASS_VECTOR))
				ExpectedColonExprs = HLSLExprColonType(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC | ExpectedColonExprs);

			    Variable->ExprColon = self->ParseExprColon(ExpectedColonExprs);
			if (Variable->ExprColon == nullptr)                     return nullptr;
			if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) return nullptr;
		}
		
		if (self->CurrentLexem.GetRawData() == '<')
		{	// Parsing annotations here.
			GD_NOT_IMPLEMENTED();
			return nullptr;
		}

		if (self->CurrentLexem.GetRawData() == '=')
		{	// Parsing default values here.
			//     Variable->InitialValue = self->ParseExpression(GD_HLSL_EXPRESSION_UNKNOWN); // ???
			// if (Variable->InitialValue == nullptr) return nullptr;
			GD_NOT_IMPLEMENTED();
			return nullptr;
		}

		if (DoExpectSemicolon)
		{	
			if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SEMICOLON)) return nullptr;
			if (!self->ExpectNextLexem(                                                      )) return nullptr;
		}

		return Variable.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses function that is defined in current lexem.
	// Input lexer position: first lexem after function name ('(' lexem). 
	// Output lexer position: first lexem after function definition. 
	HLSLFunction const* HLSLParserImpl::ParseFunction(HLSLDefinition const* const PrependingDefinition)
	{	// Parsing function arguments here.
		UniquePtr<HLSLFunction> Function(new HLSLFunction(*PrependingDefinition));
		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_PARAMS_BEGIN)) return nullptr;
		for (;;)
		{	// Parsing function arguments here:
			HLSLArgumentAccessType ArgumentAccessType = GD_HLSL_ARGUMENT_IN;
			if (!self->ExpectNextLexem()) return nullptr;
			if ((self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_OPERATOR) && (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_PARAMS_END)) break;
			if ((self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD ) && (GD_HLSL_IS_KEYWORD_OF_TYPE(self->CurrentLexem.GetProcessedDataId(), ARGUMENTS)))
			{	// Here we have a keyword that describes argument access type: 
				StackUnorderedMap<StreamedLexerKeyword, HLSLArgumentAccessType, 3> AccessTypeTranslationTable = {
					{ GD_HLSL_KEYWORD_IN   , GD_HLSL_ARGUMENT_IN    },
					{ GD_HLSL_KEYWORD_OUT  , GD_HLSL_ARGUMENT_OUT   },
					{ GD_HLSL_KEYWORD_INOUT, GD_HLSL_ARGUMENT_INOUT },
				};

				size_t const ArgumentAccessTypeIndex = AccessTypeTranslationTable.FindFirstElement(self->CurrentLexem.GetProcessedDataId());
				ArgumentAccessType = AccessTypeTranslationTable.GetElementAt(ArgumentAccessTypeIndex).Second;
				if (!self->ExpectNextLexem()) return nullptr;
			}

			UniquePtr<HLSLVariable> ArgumentDefinition(static_cast<HLSLVariable*>(const_cast<HLSLDefinition*>(self->ParseDefinition(GD_HLSL_DEFINITION_ARGUMENT))));
			if (ArgumentDefinition.GetPointer() == nullptr) return nullptr;

			HLSLArgument* Argument = new HLSLArgument(Move(*ArgumentDefinition.GetPointer()));
			Argument->AccsessType = ArgumentAccessType;
			Function->Arguments.PushLast(Argument);
			if ((self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_OPERATOR) && (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_PARAMS_END)) break;
			if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_COMMA)) return nullptr;
		}

		/**/ if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) return nullptr;
		/**/ if ( self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_COLON)
		{	// Pasring function semantic here.
			if (Function->Type == &HLSLVoid)
			{	// 'void' functions cannot have semantics.
				static HRIShaderCrossCompilerErrorDesc const VoidFunctionSemanticError("'void' functions cannot have semantics.");
				self->RaiseError(VoidFunctionSemanticError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			    Function->Semantic = static_cast<HLSLSemantic const*>(self->ParseExprColon(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC));
			if (Function->Semantic == nullptr) return nullptr;
			if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) return nullptr;
		}

		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN)) return nullptr;
		// We are not parsing anything here. Just copying a function body.
		Function->Body += CharAnsi('{');
		for (size_t BracesBalance = 1; BracesBalance != 0; )
		{	
			CharAnsi Character = self->ShaderSourceInputStream->Read<CharAnsi>();
			switch (Character)
			{
			case CharAnsi('{'): BracesBalance += 1; break;
			case CharAnsi('}'): BracesBalance -= 1; break;
			case CharAnsi('\n'): 
				if (Function->Body.GetLastElement() == CharAnsi('\r'))
				{
					Function->Body.SetLastElement(CharAnsi('\n'));
					continue;
				}
				break;

			case CharAnsi('\r'): 
				if (Function->Body.GetLastElement() == CharAnsi('\n'))
					continue;
				break;
			}

			Function->Body += Character;
		};
		
		if (!self->ExpectNextLexem()) return nullptr;
		return Function.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses constant buffer definition.
	// Input lexer position: 'cbuffer' lexem. 
	// Output lexer position: first lexem after constant buffer definition.
	HLSLCBuffer const* HLSLParserImpl::ParseCBuffer()
	{
		if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER  )) return nullptr;
		if (!self->ValidateIdentifier(self->CurrentLexem.GetRawData())) return nullptr;

		UniquePtr<HLSLCBuffer> ConstantBuffer(new HLSLCBuffer());
		ConstantBuffer->Name = self->CurrentLexem.GetRawData();

		if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) return nullptr;
		if ( self->CurrentLexem.GetProcessedDataId() == GD_HLSL_OPERATOR_COLON)
		{	    // Pasring constant buffer registerhere.
			    ConstantBuffer->Register = static_cast<HLSLRegister const*>(self->ParseExprColon(GD_HLSL_EXPRCOLON_TYPE_REGISTER));
			if (ConstantBuffer->Register == nullptr)                return nullptr;
			if (ConstantBuffer->Register->Register != GD_HLSL_REGISTER_B)
			{	// Constant buffers may locate only in 'b' registers.
				static HRIShaderCrossCompilerErrorDesc const InvalidCBufferRegisterError("constant buffers may locate only in 'b' registers.");
				self->RaiseError(InvalidCBufferRegisterError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}
			if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR)) return nullptr;
		}

		if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_SCOPE_BEGIN)) return nullptr;
		if (!self->ParseScope (GD_HLSL_DEFINITION_VARIABLE, ConstantBuffer.GetPointer()    )) return nullptr; 
		if (!self->ExpectNextLexem(                                                        )) return nullptr;
		return ConstantBuffer.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// Parses expresseion after ':' operator.
	// Input lexer position: ':' lexem. 
	// Output lexer position: first lexem after parsed expression.
	HLSLExprColon const* HLSLParserImpl::ParseExprColon(HLSLExprColonType const ExpectedExprColons)
	{
		/**/ if (!self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_COLON)) return false;
		else if (!self->ExpectNextLexem()) return false;
		else if ( self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_KEYWORD)
		{	// Here comes the 'register' or 'packoffset'
			/**/ if (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_KEYWORD_REGISTER)
			{	// Here is register. Lets check if it was expected and parse it.
				if ((ExpectedExprColons & GD_HLSL_EXPRCOLON_TYPE_REGISTER) == 0)
				{	// Registers are not allowed here.
					static HRIShaderCrossCompilerErrorDesc const RegisterIsNotAllowedError("'register' is not allowed here.");
					self->RaiseError(RegisterIsNotAllowedError);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

					return nullptr;
				}

				if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_PARAMS_BEGIN)) return nullptr;
				if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER                             )) return nullptr;
				
				static StackUnorderedMap<CharAnsi, HLSLRegisterType, 4> const RegistersTranslationTable = {
					{ 'b', GD_HLSL_REGISTER_B },
					{ 't', GD_HLSL_REGISTER_T },
					{ 'c', GD_HLSL_REGISTER_C },
					{ 's', GD_HLSL_REGISTER_S },
				};

				size_t RegisterIndex = RegistersTranslationTable.FindFirstElement(self->CurrentLexem.GetRawData()[0]);
				if ((self->CurrentLexem.GetRawData().GetSize() == 2) ? ((RegisterIndex != SIZE_MAX) && (!CharAnsiHelpers::IsDigit(self->CurrentLexem.GetRawData()[1]))) : true)
				{	// We have an invalid register here.
					static HRIShaderCrossCompilerErrorDesc const InvalidRegisterError("register '%s' is invalid.");
					self->RaiseError(InvalidRegisterError, &self->CurrentLexem.GetRawData()[0]);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

					return nullptr;
				}

				UniquePtr<HLSLRegister> Register(new HLSLRegister());
				Register->Register   = RegistersTranslationTable.GetElementAt(RegisterIndex).Second;
				Register->RegisterID = size_t(CharAnsiHelpers::ToDigit(self->CurrentLexem.GetRawData()[1]));

				if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR, GD_HLSL_OPERATOR_PARAMS_END)) return nullptr;
				if (!self->ExpectNextLexem(GD_LEXEM_CONTENT_TYPE_OPERATOR                             )) return nullptr;
				return Register.Release();
			}
			else if (self->CurrentLexem.GetProcessedDataId() == GD_HLSL_KEYWORD_PACKOFFSET)
			{	// Here is register. Lets check if it was expected and parse it.
				if ((ExpectedExprColons & GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET) == 0)
				{	// Registers are not allowed here.
					static HRIShaderCrossCompilerErrorDesc const PackOffsetIsNotAllowedError("'packoffset' is not allowed here.");
					self->RaiseError(PackOffsetIsNotAllowedError);
					self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

					return nullptr;
				}

				GD_NOT_IMPLEMENTED();
				return nullptr;
			}
		}
		else if (self->ExpectLexem(GD_LEXEM_CONTENT_TYPE_IDENTIFIER))
		{	// Here comes semantic. 
			/// @todo Rewrite semantic name and ID extraction after 'String' class refactoring
			UniquePtr<HLSLSemantic> Semantic(new HLSLSemantic());
			bool const HasSemanticID = CharAnsiHelpers::IsDigit(self->CurrentLexem.GetRawData().GetLastElement());
			Semantic->SemanticID = (HasSemanticID ? size_t(CharAnsiHelpers::ToDigit(self->CurrentLexem.GetRawData().GetLastElement())) : 0);
			Semantic->Semantic   = HLSLSemanticTypeFromString(self->CurrentLexem.GetRawData().GetSubstring(0, self->CurrentLexem.GetRawData().GetSize() - (HasSemanticID ? 1 : 0)));
			/**/ if ( Semantic->Semantic == GD_HLSL_SEMANTIC_UNKNOWN)
			{	// Invalid semantic specified.
				static HRIShaderCrossCompilerErrorDesc const InvalidSemanticError("Unknown semantic '%s'.");
				self->RaiseError(InvalidSemanticError, &self->CurrentLexem.GetRawData()[0]);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}
			else if ((ExpectedExprColons & GD_HLSL_EXPRCOLON_TYPE_SEMANTIC) == 0)
			{	// No semantics allowed here.
				static HRIShaderCrossCompilerErrorDesc const SemanticIsNotAllowedError("semantic is not allowed here.");
				self->RaiseError(SemanticIsNotAllowedError);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return nullptr;
			}

			if (!self->ExpectNextLexem()) return nullptr;
			return Semantic.Release();
		}

		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	// bool HLSLParserImpl::Validate***(String const& ***)
	//////////////////////////////////////////////////////////////////////////

	bool HLSLParserImpl::ValidateIdentifier(String const& Identifier)
	{
		if (self->FindDefinitionInScope(Identifier) != nullptr)
		{	// Identifier was already declared in this scope.
			static HRIShaderCrossCompilerErrorDesc const IdentifierWasAlreadyDefinedError("identifier '%s' was already defined in current scope.");
			self->RaiseError(IdentifierWasAlreadyDefinedError, &self->CurrentLexem.GetRawData()[0]);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// bool HLSLParserImpl::[Try]Expect[Next]Lexem([LexemType], [ID])
	//////////////////////////////////////////////////////////////////////////

	bool HLSLParserImpl::TryReadNextLexem()
	{
		if (self->Lexer.GetNextLexem(self->ShaderSourceInputStream, &self->CurrentLexem))
		{	// Reading succeded, we need to skip comments comments until read token.
			if (self->CurrentLexem.GetContentType() == GD_LEXEM_CONTENT_TYPE_COMMENT)
				return self->TryReadNextLexem();

			return true;
		}

		return false;
	}

	bool HLSLParserImpl::ExpectNextLexem()
	{
		if (!self->TryReadNextLexem())
		{	// Unexpected end of stream while reading lexem.
			static HRIShaderCrossCompilerErrorDesc const EndOfStreamInVariableDeclError("unexpected End-Of-Stream.");
			self->RaiseError(EndOfStreamInVariableDeclError);
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	bool HLSLParserImpl::ExpectLexem(LexemContentType ExpectedContentType)
	{
		if (self->CurrentLexem.GetContentType() != ExpectedContentType)
		{	// Unexpected lexem type.
			static HRIShaderCrossCompilerErrorDesc const UnexpectedLexemError("unexpected %s. Expected %s");
			self->RaiseError(UnexpectedLexemError, LexemContentTypeToString(self->CurrentLexem.GetContentType()), LexemContentTypeToString(ExpectedContentType));
			self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

			return false;
		}

		return true;
	}

	bool HLSLParserImpl::ExpectNextLexem(LexemContentType ExpectedContentType)
	{
		if (self->ExpectNextLexem())
			return self->ExpectLexem(ExpectedContentType);

		return false;
	}

	bool HLSLParserImpl::ExpectLexem(LexemContentType ExpectedContentType, StreamedLexerId const ID)
	{
		if (self->ExpectLexem(ExpectedContentType))
		{	// Next lexem exists and has expected content type.
			if (self->CurrentLexem.GetProcessedDataId() != ID)
			{	// Unexpected lexem value.
				static HRIShaderCrossCompilerErrorDesc const UnexpectedLexemValueError("unexpected '%s'.");
				self->RaiseError(UnexpectedLexemValueError, &self->CurrentLexem.GetRawData()[0]);
				self->RaiseExceptionWithCode(GD_HRI_SHADERCC_EXCEPTION_SYNTAX);

				return false;
			}

			return true;
		}

		return false;
	}

	bool HLSLParserImpl::ExpectNextLexem(LexemContentType ExpectedContentType, StreamedLexerId const ID)
	{
		if (self->ExpectNextLexem())
		{	// Next lexem exists and has expected content type.
			return self->ExpectLexem(ExpectedContentType, ID);
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// HLSL*** const* HLSLScope::Find***
	//////////////////////////////////////////////////////////////////////////

	HLSLDefinition const* HLSLScope::FindDefinition(String const& DefinitionName) const
	{	// Searching in specified scope.
		size_t const RequiredDefinitionIndex = self->InnerDefinitions.FindFirstElement([&DefinitionName](HLSLDefinition const* Definition) -> bool {
			return (Definition->Name == DefinitionName);
		});

		return ((RequiredDefinitionIndex != SIZE_MAX) ? self->InnerDefinitions[RequiredDefinitionIndex] : nullptr);
	}

	HLSLType const* HLSLScope::FindType(String const& TypeName) const
	{
		return HLSLDynamicCast<HLSLType const*>(self->FindDefinition(TypeName));
	}

	HLSLVariable const* HLSLScope::FindVariable(String const& VariableName) const
	{
		return HLSLDynamicCast<HLSLVariable const*>(self->FindDefinition(VariableName));
	}

	HLSLFunction const* HLSLScope::FindFunction(String const& FunctionName) const
	{
		return HLSLDynamicCast<HLSLFunction const*>(self->FindDefinition(FunctionName));
	}

	//////////////////////////////////////////////////////////////////////////
	// HLSL*** const* HLSLParserImpl::Find***[InScope]
	//////////////////////////////////////////////////////////////////////////

	HLSLDefinition const* HLSLParserImpl::FindDefinition(String const& DefinitionName) const
	{	// Searching in all stack of our declarations.
		for (auto const DefinitionScope : Reverse(self->GlobalScopesStack))
		{	// Definition was not found in current scope, searching in lower one.
			HLSLDefinition const* const FoundDefinition = DefinitionScope->FindDefinition(DefinitionName);
			if (FoundDefinition != nullptr) return FoundDefinition;
		}

		return nullptr;
	}

	HLSLType const* HLSLParserImpl::FindType(String const& TypeName) const
	{
		return HLSLDynamicCast<HLSLType const*>(self->FindDefinition(TypeName));
	}

	HLSLVariable const* HLSLParserImpl::FindVariable(String const& VariableName) const
	{
		return HLSLDynamicCast<HLSLVariable const*>(self->FindDefinition(VariableName));
	}

	HLSLFunction const* HLSLParserImpl::FindFunction(String const& FunctionName) const
	{
		return HLSLDynamicCast<HLSLFunction const*>(self->FindDefinition(FunctionName));
	}

	HLSLDefinition const* HLSLParserImpl::FindDefinitionInScope(String const& DefinitionName) const
	{	// Searching in top scope.
		HLSLScope const* const TopDefinitionScope = self->GlobalScopesStack.GetLastElement();
		return TopDefinitionScope->FindDefinition(DefinitionName);
	}

	HLSLType const* HLSLParserImpl::FindTypeInScope(String const& TypeName) const
	{
		HLSLScope const* const TopTypeScope = self->GlobalScopesStack.GetLastElement();
		return TopTypeScope->FindType(TypeName);
	}

	HLSLVariable const* HLSLParserImpl::FindVariableInScope(String const& VariableName) const
	{	
		HLSLScope const* const TopDefinitionScope = self->GlobalScopesStack.GetLastElement();
		return TopDefinitionScope->FindVariable(VariableName);
	}

	HLSLFunction const* HLSLParserImpl::FindFunctionInScope(String const& FunctionName) const
	{
		HLSLScope const* const TopFunctionScope = self->GlobalScopesStack.GetLastElement();
		return TopFunctionScope->FindFunction(FunctionName);
	}

	////////////////////////////////////////////////////////////////////////////
	/// HLSLParser class
	////////////////////////////////////////////////////////////////////////////

	HLSLScope const* HLSLParser::ParseShader(Stream* const InputStream)
	{
		return HLSLParserImpl(self->Toolchain, InputStream).ParseShader();
	}

GD_NAMESPACE_END
