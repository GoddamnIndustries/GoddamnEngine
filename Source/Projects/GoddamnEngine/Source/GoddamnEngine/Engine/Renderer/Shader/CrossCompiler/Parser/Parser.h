/// ==========================================================================================
/// Parser.h - HLSL parser interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 14.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_PARSER
#define GD_ENGINE_RENDERER_SHADER_CROSS_COMPILER_PARSER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>
#include <GoddamnEngine/Core/Compilers/Toolchain/Toolchain.h>
#include <GoddamnEngine/Core/Compilers/StreamedLexer/StreamedLexer.h>

#if (!defined(GD_PLATFORM_DESKTOP))
#	error "This API can be used on desktop platfroms only."
#endif

GD_NAMESPACE_BEGIN

	struct HLSLType;
	struct HLSLVariable;
	struct HLSLFunction;

	/// ==========================================================================================
	/// High Level Shading Language infrastructure
	/// ==========================================================================================

	enum HLSLExpressionType : UInt8
	{
		GD_HLSL_EXPRESSION_UNKNOWN  = 0,
		GD_HLSL_EXPRESSION_DEFINITION  = GD_BIT(0),
		GD_HLSL_EXPRESSION_AFTERCOLON  = GD_BIT(1),
		GD_HLSL_EXPRESSION_FLOWCONTROL = GD_BIT(2),
		//GD_HLSL_EXPRESSION_ = GD_BIT(),
	};	// enum HLSLExpressionType

	enum HLSLDefinitionType : UInt8
	{
		GD_HLSL_DEFINITION_UNKNOWN  = 0,
		GD_HLSL_DEFINITION_VARIABLE    = GD_BIT(0),
		GD_HLSL_DEFINITION_FUNCTION    = GD_BIT(1),
		GD_HLSL_DEFINITION_ARGUMENT    = GD_BIT(2),
		GD_HLSL_DEFINITION_ATTRIBUTE   = GD_BIT(3),
		GD_HLSL_DEFINITION_CONSTANTBUFFER = GD_BIT(4),
		GD_HLSL_DEFINITION_TYPEDEF  = GD_BIT(5),
		GD_HLSL_DEFINITION_TYPE     = GD_BIT(6),
	};	// enum HLSLDefinitionType
	
	enum HLSLTypeClass : UInt8
	{
		GD_HLSL_TYPE_CLASS_UNKNOWN = 0,
		GD_HLSL_TYPE_CLASS_VOID,
		GD_HLSL_TYPE_CLASS_SCALAR,
		GD_HLSL_TYPE_CLASS_VECTOR,
		GD_HLSL_TYPE_CLASS_MATRIX,
		GD_HLSL_TYPE_CLASS_SAMPLER,
		GD_HLSL_TYPE_CLASS_TEXTURE2D,
		GD_HLSL_TYPE_CLASS_TEXTURECUBE,
		GD_HLSL_TYPE_CLASS_STRUCT,
	};	// enum HLSLTypeClass

	enum HLSLTypeDataType : UInt8
	{
		GD_HLSL_TYPE_DATA_TYPE_UNKNOWN = 0,
		GD_HLSL_TYPE_DATA_TYPE_bool,
		GD_HLSL_TYPE_DATA_TYPE_int,
		GD_HLSL_TYPE_DATA_TYPE_uint,
		GD_HLSL_TYPE_DATA_TYPE_dword,
	//	GD_HLSL_TYPE_DATA_TYPE_half,
		GD_HLSL_TYPE_DATA_TYPE_float,
	//	GD_HLSL_TYPE_DATA_TYPE_SNORM_FLOAT,
	//	GD_HLSL_TYPE_DATA_TYPE_UNORM_FLOAT,
		GD_HLSL_TYPE_DATA_TYPE_double,
	//	GD_HLSL_TYPE_DATA_TYPE_min16float,
	//	GD_HLSL_TYPE_DATA_TYPE_min10float,
	//	GD_HLSL_TYPE_DATA_TYPE_min16int,
	//	GD_HLSL_TYPE_DATA_TYPE_min12int,
	//	GD_HLSL_TYPE_DATA_TYPE_min16uint,
	};	// enum HLSLTypeDataType

	enum HLSLTypeModifier : UInt8
	{
		GD_HLSL_TYPE_MODIFIER_UNKNOWN   = 0,
		GD_HLSL_TYPE_MODIFIER_CONST  = GD_BIT(0),
		GD_HLSL_TYPE_MODIFIER_ROW_MAJOR = GD_BIT(1),
		GD_HLSL_TYPE_MODIFIER_COLUMN_MAJOR = GD_BIT(2),
	};	// enum HLSLTypeModifier

	enum HLSLExprColonType : UInt8
	{
		GD_HLSL_EXPRCOLON_TYPE_UNKNOWN = 0,
		GD_HLSL_EXPRCOLON_TYPE_SEMANTIC   = GD_BIT(0),
		GD_HLSL_EXPRCOLON_TYPE_REGISTER   = GD_BIT(1),
		GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET = GD_BIT(2),
	};	// enum HLSLExprColonType

	enum HLSLSemanticType : UInt8
	{
		GD_HLSL_SEMANTIC_UNKNOWN = 0,
		GD_HLSL_SEMANTIC_BINORMAL,
		GD_HLSL_SEMANTIC_BLENDINDICES,
		GD_HLSL_SEMANTIC_BLENDWEIGHT,
		GD_HLSL_SEMANTIC_COLOR,
		GD_HLSL_SEMANTIC_NORMAL,
		GD_HLSL_SEMANTIC_POSITION,
		GD_HLSL_SEMANTIC_POSITIONT,
		GD_HLSL_SEMANTIC_PSIZE,
		GD_HLSL_SEMANTIC_TANGENT,
		GD_HLSL_SEMANTIC_TEXCOORD,
		GD_HLSL_SEMANTIC_FOG,
		GD_HLSL_SEMANTIC_TESSFACTOR,
		GD_HLSL_SEMANTIC_SV_ClipDistance,
		GD_HLSL_SEMANTIC_SV_CullDistance,
		GD_HLSL_SEMANTIC_SV_Coverage,
		GD_HLSL_SEMANTIC_SV_Depth,
		GD_HLSL_SEMANTIC_SV_DispatchThreadID,
		GD_HLSL_SEMANTIC_SV_DomainLocation,
		GD_HLSL_SEMANTIC_SV_GroupID,
		GD_HLSL_SEMANTIC_SV_GroupIndex,
		GD_HLSL_SEMANTIC_SV_GroupThreadID,
		GD_HLSL_SEMANTIC_SV_GSInstanceID,
		GD_HLSL_SEMANTIC_SV_InsideTessFactor,
		GD_HLSL_SEMANTIC_SV_IsFrontFace,
		GD_HLSL_SEMANTIC_SV_OutputControlPointID,
		GD_HLSL_SEMANTIC_SV_Position,
		GD_HLSL_SEMANTIC_SV_RenderTargetArrayIndex,
		GD_HLSL_SEMANTIC_SV_SampleIndex,
		GD_HLSL_SEMANTIC_SV_Target,
		GD_HLSL_SEMANTIC_SV_TessFactor,
		GD_HLSL_SEMANTIC_SV_ViewportArrayIndex,
		GD_HLSL_SEMANTIC_SV_InstanceID,
		GD_HLSL_SEMANTIC_SV_PrimitiveID,
		GD_HLSL_SEMANTIC_SV_VertexID,
		GD_HLSL_SEMANTICS_COUNT,
	};	// enum HLSLSemantic 

	enum HLSLRegisterType : UInt8
	{
		GD_HLSL_REGISTER_UNKNOWN = 0,
		GD_HLSL_REGISTER_B,
		GD_HLSL_REGISTER_T,
		GD_HLSL_REGISTER_C,
		GD_HLSL_REGISTER_S,
	};	// enum HLSLRegisterType

	enum HLSLArgumentAccessType : UInt8
	{
		GD_HLSL_ARGUMENT_UNKNOWN =  0,
		GD_HLSL_ARGUMENT_IN   =  GD_BIT(0),
		GD_HLSL_ARGUMENT_OUT  =  GD_BIT(1),
		GD_HLSL_ARGUMENT_INOUT   = (GD_HLSL_ARGUMENT_IN | GD_HLSL_ARGUMENT_OUT),
	};	// enum HLSLArgumentAccessType

	struct HLSLExpression
	{
	private:
		GD_CLASS_UNASSIGNABLE(HLSLExpression);

	public:
		HLSLExpressionType const ExpressionType;
		GDINL virtual ~HLSLExpression(                 )          { }
		GDINL explicit HLSLExpression(HLSLExpressionType const ExpressionType = GD_HLSL_EXPRESSION_UNKNOWN) : ExpressionType(ExpressionType) { }
	};	// struct HLSLExpression

	struct HLSLDefinition : public HLSLExpression
	{
		HLSLDefinitionType const  DefinitionType;
		HLSLType     const* Type   = nullptr;
		HLSLTypeModifier    TypeModifier = GD_HLSL_TYPE_MODIFIER_UNKNOWN;
		String     Name;
		GDINL virtual ~HLSLDefinition() { }
		GDINL explicit HLSLDefinition(        HLSLDefinitionType const DefinitionType = GD_HLSL_DEFINITION_UNKNOWN) : HLSLExpression(GD_HLSL_EXPRESSION_DEFINITION), DefinitionType(DefinitionType) { }
		GDINL explicit HLSLDefinition(HLSLDefinition&& Definition, HLSLDefinitionType const DefinitionType = GD_HLSL_DEFINITION_UNKNOWN) : HLSLExpression(GD_HLSL_EXPRESSION_DEFINITION)
			, DefinitionType(DefinitionType)
			, Type(Definition.Type)
		{
			this->Name = Definition.Name;
		}
	};	// struct HLSLDefinition

	struct HLSLScope 
	{
	private:
		GD_CLASS_UNASSIGNABLE(HLSLScope);
		GD_CLASS_UNCOPIABLE  (HLSLScope);

	public:
		Vector<HLSLDefinition const*> InnerDefinitions;
		GDINL virtual ~HLSLScope() { for (auto const Definition : this->InnerDefinitions) delete Definition; }
		GDINL explicit HLSLScope() { }
		GDINT HLSLDefinition const* FindDefinition(String const& Name) const;
		GDINT HLSLVariable   const* FindVariable  (String const& Name) const;
		GDINT HLSLFunction   const* FindFunction  (String const& Name) const;
		GDINT HLSLType    const* FindType   (String const& Name) const;
	};	// struct HLSLScope

	struct HLSLExprColon : public HLSLExpression
	{
		HLSLExprColonType const Type;
		GDINL virtual ~HLSLExprColon(       )                { }
		GDINL explicit HLSLExprColon(HLSLExprColonType const Type) : HLSLExpression(GD_HLSL_EXPRESSION_AFTERCOLON), Type(Type) { }
	};	// struct HLSLExprColon

	struct HLSLSemantic final : public HLSLExprColon
	{
		HLSLSemanticType Semantic   = GD_HLSL_SEMANTIC_UNKNOWN;
		size_t     SemanticID = 0;
		GDINL virtual ~HLSLSemantic() { }
		GDINL explicit HLSLSemantic() : HLSLExprColon(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC) { }
	};	// struct HLSLSemantic

	struct HLSLRegister final : public HLSLExprColon
	{
		HLSLRegisterType Register   = GD_HLSL_REGISTER_UNKNOWN;
		size_t     RegisterID = 0;
		GDINL virtual ~HLSLRegister() { }
		GDINL explicit HLSLRegister() : HLSLExprColon(GD_HLSL_EXPRCOLON_TYPE_REGISTER) { }
	};	// struct HLSLRegister

	struct HLSLPackOffset final : public HLSLExprColon
	{
		GDINL virtual ~HLSLPackOffset() { }
		GDINL explicit HLSLPackOffset() : HLSLExprColon(GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET) { }
	};	// struct HLSLRegister

	struct HLSLType : public HLSLDefinition
	{
		HLSLTypeClass const Class;
		GDINL virtual ~HLSLType(       ) { }
		GDINL explicit HLSLType(HLSLTypeClass const Class) : HLSLDefinition(GD_HLSL_DEFINITION_TYPE), Class(Class) { this->Type = this; }
	};	// struct HLSLType

	struct HLSLScalarType : public HLSLType
	{
		HLSLTypeDataType DataType = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;
		GDINL virtual ~HLSLScalarType() { }
		GDINL explicit HLSLScalarType() : HLSLType(GD_HLSL_TYPE_CLASS_SCALAR) { }
	};	// struct HLSLScalarType

	struct HLSLVectorType : public HLSLType
	{
		HLSLTypeDataType DataType   = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;
		size_t     ComponentsNumber = 0;
		GDINL virtual ~HLSLVectorType() { }
		GDINL explicit HLSLVectorType() : HLSLType(GD_HLSL_TYPE_CLASS_VECTOR) { }
	};	// struct HLSLVectorType

	struct HLSLMatrixType : public HLSLType
	{
		HLSLTypeDataType DataType   = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;
		size_t     RowsNumber = 0;
		size_t     ColumnsNumber = 0;
		GDINL virtual ~HLSLMatrixType() { }
		GDINL explicit HLSLMatrixType() : HLSLType(GD_HLSL_TYPE_CLASS_MATRIX) { }
	};	// struct HLSLMatrixType

	struct HLSLStruct : public HLSLType, public HLSLScope
	{
		GDINL virtual ~HLSLStruct() { }
		GDINL explicit HLSLStruct() : HLSLType(GD_HLSL_TYPE_CLASS_STRUCT) { }
	};	// struct HLSLStruct

	struct HLSLCBuffer final : public HLSLDefinition, public HLSLScope
	{
		HLSLRegister const* Register = nullptr;
		GDINL virtual ~HLSLCBuffer() { delete this->Register; }
		GDINL explicit HLSLCBuffer() : HLSLDefinition(GD_HLSL_DEFINITION_CONSTANTBUFFER) { }
	};	// struct HLSLCBuffer

	struct HLSLVariable : public HLSLDefinition
	{
		HLSLExprColon  const* ExprColon = nullptr;
		HLSLExpression const* InitialValue = nullptr;
		size_t    ArrayIndex   = 0;
		GDINL virtual ~HLSLVariable(         ) { }
		GDINL explicit HLSLVariable(         ) : HLSLDefinition(   GD_HLSL_DEFINITION_VARIABLE) { }
		GDINL explicit HLSLVariable(HLSLDefinition&& Definition) : HLSLDefinition(Forward<HLSLDefinition>(Definition), GD_HLSL_DEFINITION_VARIABLE) { }
		GDINL explicit HLSLVariable(HLSLVariable  && Variable  ) : HLSLDefinition(Forward<HLSLVariable  >(Variable),   GD_HLSL_DEFINITION_ARGUMENT)
			, InitialValue(Variable.InitialValue)
			, ExprColon   (Variable.ExprColon)
			, ArrayIndex  (Variable.ArrayIndex)
		{
			Variable.ExprColon = nullptr;
			Variable.InitialValue = nullptr;
			Variable.ArrayIndex   = 0;
		}
	};	// struct HLSLVariable

	struct HLSLArgument final : public HLSLVariable
	{
		HLSLArgumentAccessType AccsessType = GD_HLSL_ARGUMENT_UNKNOWN;
		GDINL virtual ~HLSLArgument(         ) { }
		GDINL explicit HLSLArgument(         ) { }
		GDINL explicit HLSLArgument(HLSLVariable&& DrependingArgument) : HLSLVariable(Forward<HLSLVariable>(DrependingArgument)) { }
	};	// struct HLSLArgument

	struct HLSLFunction final : public HLSLDefinition
	{
		String					 Body;
		HLSLSemantic  const*  Semantic = nullptr;
		Vector<HLSLArgument const*> Arguments;
		GDINL virtual ~HLSLFunction(         ) { }
		GDINL explicit HLSLFunction(         ) : HLSLDefinition(          GD_HLSL_DEFINITION_FUNCTION) { }
		GDINL explicit HLSLFunction(HLSLDefinition&& Definition) : HLSLDefinition(Forward<HLSLDefinition>(Definition), GD_HLSL_DEFINITION_FUNCTION) { }
	};	// struct HLSLFunction

	/// Parses HLSL shader into abstract syntax tree.
	class HLSLParser final : public IToolchainTool
	{
	public /*Class API*/:
		GDINL  HLSLParser(IToolchain* const Toolchain) : IToolchainTool(Toolchain) { }
		GDINL ~HLSLParser(         )        { }

		/// Returns the Abstract syntax tree of specified shader
		GDAPI HLSLScope const* ParseShader(UniquePtr<InputStream>&& InputStream);
	};	// class HLSLParserImpl

	GDINT extern Str     HLSLSemanticToStr  (HLSLSemanticType const  Semantic);
	GDINT extern HLSLSemanticType HLSLSemanticFromString(String     const& SemanticString);
	GDINT extern HRISemantic   HLSLSemanticToHRI  (HLSLSemanticType const  Semantic);
	GDINT extern HLSLSemanticType HLSLSemanticFromHRI   (HRISemantic   const  Semantic);

	/// ==========================================================================================
	// ~~~ DYNAMIC CASTS ~~~
	/// ==========================================================================================

	// ~~~ BAD CAST ~~~

	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(chandle const InvalidPointer)
	{
		GD_NOT_USED(InvalidPointer);
		return nullptr;
	}

	// ~~~ EXPR COLON ~~~
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLExprColon   * const ExprColon);
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLExprColon const* const ExprColon);

	template<>
	GDINL HLSLSemantic* HLSLDynamicCast<HLSLSemantic*>(HLSLExprColon* const ExprColon)
	{
		return ((ExprColon != nullptr) ? ((ExprColon->Type == GD_HLSL_EXPRCOLON_TYPE_SEMANTIC) ? static_cast<HLSLSemantic*>(ExprColon) : nullptr): nullptr);
	}
	template<>
	GDINL HLSLSemantic const* HLSLDynamicCast<HLSLSemantic const*>(HLSLExprColon const* const ExprColon)
	{
		return HLSLDynamicCast<HLSLSemantic*>(const_cast<HLSLExprColon*>(ExprColon));
	}

	template<>
	GDINL HLSLRegister* HLSLDynamicCast<HLSLRegister*>(HLSLExprColon* const ExprColon)
	{
		return ((ExprColon != nullptr) ? ((ExprColon->Type == GD_HLSL_EXPRCOLON_TYPE_REGISTER) ? static_cast<HLSLRegister*>(ExprColon) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLRegister const* HLSLDynamicCast<HLSLRegister const*>(HLSLExprColon const* const ExprColon)
	{
		return HLSLDynamicCast<HLSLRegister*>(const_cast<HLSLExprColon*>(ExprColon));
	}

	template<>
	GDINL HLSLPackOffset* HLSLDynamicCast<HLSLPackOffset*>(HLSLExprColon* const ExprColon)
	{
		return ((ExprColon != nullptr) ? ((ExprColon->Type == GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET) ? static_cast<HLSLPackOffset*>(ExprColon) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLPackOffset const* HLSLDynamicCast<HLSLPackOffset const*>(HLSLExprColon const* const ExprColon)
	{
		return HLSLDynamicCast<HLSLPackOffset*>(const_cast<HLSLExprColon*>(ExprColon));
	}

	// ~~~ TYPES ~~~
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLType   * const Type);
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLType const* const Type);

	template<>
	GDINL HLSLScalarType* HLSLDynamicCast<HLSLScalarType*>(HLSLType* const Type)
	{
		return ((Type != nullptr) ? ((Type->Class == GD_HLSL_TYPE_CLASS_SCALAR) ? static_cast<HLSLScalarType*>(Type) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLScalarType const* HLSLDynamicCast<HLSLScalarType const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLScalarType*>(const_cast<HLSLType*>(Type));
	}

	template<>
	GDINL HLSLVectorType* HLSLDynamicCast<HLSLVectorType*>(HLSLType* const Type)
	{
		return ((Type != nullptr) ? ((Type->Class == GD_HLSL_TYPE_CLASS_VECTOR) ? static_cast<HLSLVectorType*>(Type) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLVectorType const* HLSLDynamicCast<HLSLVectorType const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLVectorType*>(const_cast<HLSLType*>(Type));
	}

	template<>
	GDINL HLSLMatrixType* HLSLDynamicCast<HLSLMatrixType*>(HLSLType* const Type)
	{
		return ((Type != nullptr) ? ((Type->Class == GD_HLSL_TYPE_CLASS_MATRIX) ? static_cast<HLSLMatrixType*>(Type) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLMatrixType const* HLSLDynamicCast<HLSLMatrixType const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLMatrixType*>(const_cast<HLSLType*>(Type));
	}

	template<>
	GDINL HLSLStruct* HLSLDynamicCast<HLSLStruct*>(HLSLType* const Type)
	{
		return ((Type != nullptr) ? ((Type->Class == GD_HLSL_TYPE_CLASS_STRUCT) ? static_cast<HLSLStruct*>(Type) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLStruct const* HLSLDynamicCast<HLSLStruct const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLStruct*>(const_cast<HLSLType*>(Type));
	}

	// ~~~ DEFINITION ~~~

	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLDefinition   * const Definition);
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLDefinition const* const Definition);

	template<>
	GDINL HLSLType* HLSLDynamicCast<HLSLType*>(HLSLDefinition* const Definition)
	{
		return ((Definition != nullptr) ? ((Definition->DefinitionType == GD_HLSL_DEFINITION_TYPE) ? static_cast<HLSLType*>(Definition) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLType const* HLSLDynamicCast<HLSLType const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLType*>(const_cast<HLSLDefinition*>(Definition));
	}

	template<>
	GDINL HLSLVariable* HLSLDynamicCast<HLSLVariable*>(HLSLDefinition* const Definition)
	{
		return ((Definition != nullptr) ? ((Definition->DefinitionType == GD_HLSL_DEFINITION_VARIABLE) ? static_cast<HLSLVariable*>(Definition) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLVariable const* HLSLDynamicCast<HLSLVariable const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLVariable*>(const_cast<HLSLDefinition*>(Definition));
	}

	template<>
	GDINL HLSLFunction* HLSLDynamicCast<HLSLFunction*>(HLSLDefinition* const Definition)
	{
		return ((Definition != nullptr) ? ((Definition->DefinitionType == GD_HLSL_DEFINITION_FUNCTION) ? static_cast<HLSLFunction*>(Definition) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLFunction const* HLSLDynamicCast<HLSLFunction const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLFunction*>(const_cast<HLSLDefinition*>(Definition));
	}

	template<>
	GDINL HLSLCBuffer* HLSLDynamicCast<HLSLCBuffer*>(HLSLDefinition* const Definition)
	{
		return ((Definition != nullptr) ? ((Definition->DefinitionType == GD_HLSL_DEFINITION_CONSTANTBUFFER) ? static_cast<HLSLCBuffer*>(Definition) : nullptr) : nullptr);
	}
	template<>
	GDINL HLSLCBuffer const* HLSLDynamicCast<HLSLCBuffer const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLCBuffer*>(const_cast<HLSLDefinition*>(Definition));
	}

GD_NAMESPACE_END

#endif
