// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/ShaderCrossCompiler/HLSLParser.h
 * File HLSL parser interface for the shader cross compiler.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#if GD_PLATFORM_DESKTOP

#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/System/IO/Stream.h>

GD_NAMESPACE_BEGIN

	struct HLSLScope;
	struct HLSLType;
	enum   HLSLTypeModifier : UInt8;
	struct HLSLVariable;
	struct HLSLFunction;

	//! @todo Erase this enum.
	enum HRISemantic : UInt8
	{
		GD_HRI_SEMANTIC_POSITION,
		GD_HRI_SEMANTIC_NORMAL,
		GD_HRI_SEMANTIC_TANGENT,
		GD_HRI_SEMANTIC_BINORMAL,
		GD_HRI_SEMANTIC_TEXCOORD,
		GD_HRI_SEMANTIC_BLENDINDICES,
		GD_HRI_SEMANTIC_BLENDWEIGHT,
		GD_HRI_SEMANTIC_COLOR,
		GD_HRI_SEMANTIC_UNKNOWN,
		GD_HRI_SEMANTIC_FIRST = GD_HRI_SEMANTIC_POSITION,
		GD_HRI_SEMANTICS_COUNT = GD_HRI_SEMANTIC_UNKNOWN
	};	// enum HRISemantic

	// ==========================================================================================
	// HLSLParser class.
	// ==========================================================================================
	
	// ------------------------------------------------------------------------------------------
	//! Parses HLSL shader code into abstract syntax tree.
	class HLSLParser final
	{
	public:
		
		// ------------------------------------------------------------------------------------------
		//! Parses HLSL shader code into abstract syntax tree.
		GDAPI HLSLScope const* ParseShader(class InputStream* inputStream);
	
	};	// class HLSLParserImpl

	// ==========================================================================================
	// High Level Shading Language infrastructure.
	// ==========================================================================================
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// HLSL Expressions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents types of the expressions that HLSL supports.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509663(v=vs.85).aspx
	enum HLSLExpressionType : UInt8
	{
		GD_HLSL_EXPRESSION_UNKNOWN,
		GD_HLSL_EXPRESSION_BINDING,
		GD_HLSL_EXPRESSION_DEFINITION,
	};	// enum HLSLExpressionType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL generic expression.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509663(v=vs.85).aspx
	struct HLSLExpression : public IVirtuallyDestructible
	{
	public:
		HLSLExpressionType const ExpressionType;

	public:
		GDINL virtual ~HLSLExpression() { }
		GDINL explicit HLSLExpression(HLSLExpressionType const ExpressionType = GD_HLSL_EXPRESSION_UNKNOWN) : ExpressionType(ExpressionType) { }
	};	// struct HLSLExpression

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~ HLSL Binding expressions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents types of the bindings, that HLSL definitions can contains.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509706(v=vs.85).aspx
	enum HLSLBindingType : UInt8
	{
		GD_HLSL_EXPRCOLON_TYPE_UNKNOWN     = 0,
		GD_HLSL_EXPRCOLON_TYPE_SEMANTIC    = GD_BIT(0),
		GD_HLSL_EXPRCOLON_TYPE_REGISTER    = GD_BIT(1),
		GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET  = GD_BIT(2),
	};	// enum HLSLBindingType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL expression after ':' declaration.
	struct HLSLBinding : public HLSLExpression
	{
	public:
		HLSLBindingType const Type;

	public:
		GDINL virtual ~HLSLBinding() { }
		GDINL explicit HLSLBinding(HLSLBindingType const Type) : HLSLExpression(GD_HLSL_EXPRESSION_BINDING), Type(Type) { }
	};	// struct HLSLBinding

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~ HLSL Semantics Binding expressions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents supported HLSL semantics.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509647(v=vs.85).aspx
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

	GDINT extern CStr HLSLSemanticToStr(HLSLSemanticType const  Semantic);
	GDINT extern HLSLSemanticType HLSLSemanticFromString(String const& SemanticString);
	GDINT extern HRISemantic HLSLSemanticToHRI(HLSLSemanticType const Semantic);

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL semantic declaration.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509647(v=vs.85).aspx
	struct HLSLSemantic final : public HLSLBinding
	{
	public:
		HLSLSemanticType Semantic = GD_HLSL_SEMANTIC_UNKNOWN;
		SizeTp           SemanticID = 0;
	
	public:
		GDINL virtual ~HLSLSemantic() { }
		GDINL explicit HLSLSemantic() : HLSLBinding(GD_HLSL_EXPRCOLON_TYPE_SEMANTIC) { }
	};	// struct HLSLSemantic

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~ HLSL Registers Binding expressions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents the list of the supported HLSL register types.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/dd607359(v=vs.85).aspx
	enum HLSLRegisterType : UInt8
	{
		GD_HLSL_REGISTER_UNKNOWN = 0,
		GD_HLSL_REGISTER_B,
		GD_HLSL_REGISTER_T,
		GD_HLSL_REGISTER_C,
		GD_HLSL_REGISTER_S,
	};	// enum HLSLRegisterType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL 'register' declaration.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/dd607359(v=vs.85).aspx
	struct HLSLRegister final : public HLSLBinding
	{
	public:
		HLSLRegisterType Register = GD_HLSL_REGISTER_UNKNOWN;
		SizeTp           RegisterID = 0;

	public:
		GDINL virtual ~HLSLRegister() { }
		GDINL explicit HLSLRegister() : HLSLBinding(GD_HLSL_EXPRCOLON_TYPE_REGISTER) { }
	};	// struct HLSLRegister

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~ HLSL 'PackOffset's Binding expressions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL 'packoffset' declaration.
	struct HLSLPackOffset final : public HLSLBinding
	{
	public:
		GDINL virtual ~HLSLPackOffset() { }
		GDINL explicit HLSLPackOffset() : HLSLBinding(GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET) { }
	};	// struct HLSLRegister

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~ HLSL Base definitions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents base HLSL definition class.
	enum HLSLDefinitionType : UInt8
	{
		GD_HLSL_DEFINITION_UNKNOWN         = 0,
		GD_HLSL_DEFINITION_VARIABLE        = GD_BIT(0),
		GD_HLSL_DEFINITION_FUNCTION        = GD_BIT(1),
		GD_HLSL_DEFINITION_ARGUMENT        = GD_BIT(2),
		GD_HLSL_DEFINITION_ATTRIBUTE       = GD_BIT(3),
		GD_HLSL_DEFINITION_CONSTANTBUFFER  = GD_BIT(4),
		GD_HLSL_DEFINITION_TYPEDEF         = GD_BIT(5),
		GD_HLSL_DEFINITION_TYPE            = GD_BIT(6),
	};	// enum HLSLDefinitionType
	
	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL generic declaration.
	struct HLSLDefinition : public HLSLExpression
	{
	public:
		HLSLDefinitionType const  DefinitionType;
		HLSLType           const* Type         = nullptr;
		HLSLTypeModifier          TypeModifier = HLSLTypeModifier(0);
		String                    Name;

	public:
		GDINL virtual ~HLSLDefinition() { }
		GDINL explicit HLSLDefinition(HLSLDefinitionType const DefinitionType = GD_HLSL_DEFINITION_UNKNOWN) 
			: HLSLExpression(GD_HLSL_EXPRESSION_DEFINITION), DefinitionType(DefinitionType) { }
		GDINL explicit HLSLDefinition(HLSLDefinition&& Definition, HLSLDefinitionType const DefinitionType = GD_HLSL_DEFINITION_UNKNOWN) 
			: HLSLExpression(GD_HLSL_EXPRESSION_DEFINITION), DefinitionType(DefinitionType), Type(Definition.Type), Name(Definition.Name) {}
	};	// struct HLSLDefinition

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL scope ({}) declaration.
	struct HLSLScope : public TNonCopyable
	{
	public:
		Vector<UniquePtr<HLSLDefinition const>> InnerDefinitions;

	public:
		GDINL virtual ~HLSLScope() { }
		GDINL explicit HLSLScope() { }

	public:
		GDINT HLSLDefinition const* FindDefinition(String const& Name) const;
		GDINT HLSLVariable   const* FindVariable(String const& Name) const;
		GDINT HLSLFunction   const* FindFunction(String const& Name) const;
		GDINT HLSLType       const* FindType (String const& Name) const;
	};	// struct HLSLScope

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~ HLSL Type definitions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents base HLSL type class.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509587(v=vs.85).aspx
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
		GD_HLSL_TYPE_CLASS_TEXTURE3D,
		GD_HLSL_TYPE_CLASS_STRUCT,
	};	// enum HLSLTypeClass

	// ------------------------------------------------------------------------------------------
	//! Represents base scalar HLSL types.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509646(v=vs.85).aspx
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

	// ------------------------------------------------------------------------------------------
	//! Represents HLSL type modifiers.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509706(v=vs.85).aspx
	enum HLSLTypeModifier : UInt8
	{
		GD_HLSL_TYPE_MODIFGDR_UNKNOWN      = 0,
		GD_HLSL_TYPE_MODIFGDR_CONST        = GD_BIT(0),
		GD_HLSL_TYPE_MODIFGDR_ROW_MAJOR    = GD_BIT(1),
		GD_HLSL_TYPE_MODIFGDR_COLUMN_MAJOR = GD_BIT(2),
	};	// enum HLSLTypeModifier

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL type.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509587(v=vs.85).aspx
	struct HLSLType : public HLSLDefinition
	{
	public:
		HLSLTypeClass const Class;

	public:
		GDINL virtual ~HLSLType() { }
		GDINL explicit HLSLType(HLSLTypeClass const Class) : HLSLDefinition(GD_HLSL_DEFINITION_TYPE), Class(Class) { Type = this; }
	};	// struct HLSLType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL scalar type.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509646(v=vs.85).aspx
	struct HLSLScalarType : public HLSLType
	{
	public:
		HLSLTypeDataType DataType = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;

	public:
		GDINL virtual ~HLSLScalarType() { }
		GDINL explicit HLSLScalarType() : HLSLType(GD_HLSL_TYPE_CLASS_SCALAR) { }
	};	// struct HLSLScalarType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL vector type.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509707(v=vs.85).aspx
	struct HLSLVectorType : public HLSLType
	{
	public:
		HLSLTypeDataType DataType = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;
		SizeTp           ComponentsNumber = 0;

	public:
		GDINL virtual ~HLSLVectorType() { }
		GDINL explicit HLSLVectorType() : HLSLType(GD_HLSL_TYPE_CLASS_VECTOR) { }
	};	// struct HLSLVectorType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL matrix type.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509623(v=vs.85).aspx
	struct HLSLMatrixType : public HLSLType
	{
	public:
		HLSLTypeDataType DataType = GD_HLSL_TYPE_DATA_TYPE_UNKNOWN;
		SizeTp           RowsNumber = 0;
		SizeTp           ColumnsNumber = 0;

	public:
		GDINL virtual ~HLSLMatrixType() { }
		GDINL explicit HLSLMatrixType() : HLSLType(GD_HLSL_TYPE_CLASS_MATRIX) { }
	};	// struct HLSLMatrixType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL structure.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509668(v=vs.85).aspx
	struct HLSLStruct : public HLSLType, public HLSLScope
	{
	public:
		GDINL virtual ~HLSLStruct() { }
		GDINL explicit HLSLStruct() : HLSLType(GD_HLSL_TYPE_CLASS_STRUCT) { }
	};	// struct HLSLStruct

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL constant buffer.
	//! https://msdn.microsoft.com/ru-RU/library/windows/desktop/bb509581(v=vs.85).aspx (cbuffers)
	struct HLSLCBuffer final : public HLSLDefinition, public HLSLScope
	{
	public:
		HLSLRegister const* Register = nullptr;

	public:
		GDINL virtual ~HLSLCBuffer() { GD_DELETE(Register); }
		GDINL explicit HLSLCBuffer() : HLSLDefinition(GD_HLSL_DEFINITION_CONSTANTBUFFER) { }
	};	// struct HLSLCBuffer

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~ HLSL Variables definitions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL variable.
	struct HLSLVariable : public HLSLDefinition
	{
	public:
		HLSLBinding    const* Binding      = nullptr;
		HLSLExpression const* InitialValue = nullptr;
		SizeTp                ArrayRank   = 0;

	public:
		GDINL virtual ~HLSLVariable() { }
		GDINL explicit HLSLVariable() : HLSLDefinition(GD_HLSL_DEFINITION_VARIABLE) { }
		GDINL explicit HLSLVariable(HLSLDefinition&& Definition) : HLSLDefinition(Forward<HLSLDefinition>(Definition), GD_HLSL_DEFINITION_VARIABLE) { }
		GDINL explicit HLSLVariable(HLSLVariable  && Variable  ) : HLSLDefinition(Forward<HLSLVariable  >(Variable),   GD_HLSL_DEFINITION_ARGUMENT)
			, Binding(Variable.Binding), InitialValue(Variable.InitialValue), ArrayRank(Variable.ArrayRank)
		{
			Variable.Binding    = nullptr;
			Variable.InitialValue = nullptr;
			Variable.ArrayRank   = 0;
		}
	};	// struct HLSLVariable

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~ HLSL Functions definitions.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Attributes of the HLSL function arguments.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509606(v=vs.85).aspx
	enum HLSLArgumentAccessType : UInt8
	{
		GD_HLSL_ARGUMENT_UNKNOWN           = 0,
		GD_HLSL_ARGUMENT_IN                = GD_BIT(0),
		GD_HLSL_ARGUMENT_OUT               = GD_BIT(1),
		GD_HLSL_ARGUMENT_INOUT             = GD_HLSL_ARGUMENT_IN | GD_HLSL_ARGUMENT_OUT,
	};	// enum HLSLArgumentAccessType

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL function argument.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509606(v=vs.85).aspx
	struct HLSLArgument final : public HLSLVariable
	{
	public:
		HLSLArgumentAccessType AccsessType = GD_HLSL_ARGUMENT_UNKNOWN;

	public:
		GDINL virtual ~HLSLArgument() { }
		GDINL explicit HLSLArgument() { }
		GDINL explicit HLSLArgument(HLSLVariable&& DrependingArgument) : HLSLVariable(Forward<HLSLVariable>(DrependingArgument)) { }
	};	// struct HLSLArgument

	// ------------------------------------------------------------------------------------------
	//! Represents the HLSL function.
	//! https://msdn.microsoft.com/en-us/library/windows/desktop/bb509605(v=vs.85).aspx
	struct HLSLFunction final : public HLSLDefinition
	{
	public:
		String					    Body;
		HLSLSemantic const*         Semantic = nullptr;
		Vector<HLSLArgument const*> Arguments;

	public:
		GDINL virtual ~HLSLFunction() { }
		GDINL explicit HLSLFunction() : HLSLDefinition(GD_HLSL_DEFINITION_FUNCTION) { }
		GDINL explicit HLSLFunction(HLSLDefinition&& Definition) : HLSLDefinition(Forward<HLSLDefinition>(Definition), GD_HLSL_DEFINITION_FUNCTION) { }
	};	// struct HLSLFunction

	// ==========================================================================================
	// Dynamic casting functions.
	// ==========================================================================================

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~ BAD CAST ~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !GD_DOCUMENTATION
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(CHandle const InvalidPointer)
	{
		GD_NOT_USED(InvalidPointer);
		return nullptr;
	}
#endif	// if !GD_DOCUMENTATION

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~ EXPR COLON ~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Performs a dynamic cast for the expressions after ':'.
	//! @param Type Object to cast to new type.
	//! @returns Casted pointer if the type match or null pointer.
	//! @{
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLBinding* const Binding)
	{
		GD_NOT_USED(Binding);
		return nullptr;
	}
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLBinding const* const Binding)
	{
		GD_NOT_USED(Binding);
		return nullptr;
	}
	//! @}

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLSemantic* HLSLDynamicCast<HLSLSemantic*>(HLSLBinding* const Binding)
	{
		return Binding != nullptr 
			? (Binding->Type == GD_HLSL_EXPRCOLON_TYPE_SEMANTIC ? static_cast<HLSLSemantic*>(Binding) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLSemantic const* HLSLDynamicCast<HLSLSemantic const*>(HLSLBinding const* const Binding)
	{
		return HLSLDynamicCast<HLSLSemantic*>(const_cast<HLSLBinding*>(Binding));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLRegister* HLSLDynamicCast<HLSLRegister*>(HLSLBinding* const Binding)
	{
		return Binding != nullptr 
			? (Binding->Type == GD_HLSL_EXPRCOLON_TYPE_REGISTER ? static_cast<HLSLRegister*>(Binding) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLRegister const* HLSLDynamicCast<HLSLRegister const*>(HLSLBinding const* const Binding)
	{
		return HLSLDynamicCast<HLSLRegister*>(const_cast<HLSLBinding*>(Binding));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLPackOffset* HLSLDynamicCast<HLSLPackOffset*>(HLSLBinding* const Binding)
	{
		return Binding != nullptr 
			? (Binding->Type == GD_HLSL_EXPRCOLON_TYPE_PACKOFFSET ? static_cast<HLSLPackOffset*>(Binding) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLPackOffset const* HLSLDynamicCast<HLSLPackOffset const*>(HLSLBinding const* const Binding)
	{
		return HLSLDynamicCast<HLSLPackOffset*>(const_cast<HLSLBinding*>(Binding));
	}
#endif	// if !GD_DOCUMENTATION

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~ TYPES ~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Performs a dynamic cast for the types.
	//! @param Type Object to cast to new type.
	//! @returns Casted pointer if the type match or null pointer.
	//! @{
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLType* const Type);
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLType const* const Type);
	//! @}

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLScalarType* HLSLDynamicCast<HLSLScalarType*>(HLSLType* const Type)
	{
		return Type != nullptr 
			? (Type->Class == GD_HLSL_TYPE_CLASS_SCALAR ? static_cast<HLSLScalarType*>(Type) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLScalarType const* HLSLDynamicCast<HLSLScalarType const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLScalarType*>(const_cast<HLSLType*>(Type));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLVectorType* HLSLDynamicCast<HLSLVectorType*>(HLSLType* const Type)
	{
		return Type != nullptr 
			? (Type->Class == GD_HLSL_TYPE_CLASS_VECTOR ? static_cast<HLSLVectorType*>(Type) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLVectorType const* HLSLDynamicCast<HLSLVectorType const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLVectorType*>(const_cast<HLSLType*>(Type));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLMatrixType* HLSLDynamicCast<HLSLMatrixType*>(HLSLType* const Type)
	{
		return Type != nullptr 
			? (Type->Class == GD_HLSL_TYPE_CLASS_MATRIX ? static_cast<HLSLMatrixType*>(Type) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLMatrixType const* HLSLDynamicCast<HLSLMatrixType const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLMatrixType*>(const_cast<HLSLType*>(Type));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLStruct* HLSLDynamicCast<HLSLStruct*>(HLSLType* const Type)
	{
		return Type != nullptr 
			? (Type->Class == GD_HLSL_TYPE_CLASS_STRUCT ? static_cast<HLSLStruct*>(Type) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLStruct const* HLSLDynamicCast<HLSLStruct const*>(HLSLType const* const Type)
	{
		return HLSLDynamicCast<HLSLStruct*>(const_cast<HLSLType*>(Type));
	}
#endif	// if !GD_DOCUMENTATION

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~ DEFINITION ~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Performs a dynamic cast for the definitions.
	//! @param Definition Object to cast to new type.
	//! @returns Casted pointer if the type match or null pointer.
	//! @{
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLDefinition* const Definition);
	template<typename CastToType>
	GDINL CastToType HLSLDynamicCast(HLSLDefinition const* const Definition);
	//! @}

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLType* HLSLDynamicCast<HLSLType*>(HLSLDefinition* const Definition)
	{
		return Definition != nullptr 
			? (Definition->DefinitionType == GD_HLSL_DEFINITION_TYPE ? static_cast<HLSLType*>(Definition) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLType const* HLSLDynamicCast<HLSLType const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLType*>(const_cast<HLSLDefinition*>(Definition));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLVariable* HLSLDynamicCast<HLSLVariable*>(HLSLDefinition* const Definition)
	{
		return Definition != nullptr 
			? (Definition->DefinitionType == GD_HLSL_DEFINITION_VARIABLE ? static_cast<HLSLVariable*>(Definition) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLVariable const* HLSLDynamicCast<HLSLVariable const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLVariable*>(const_cast<HLSLDefinition*>(Definition));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLFunction* HLSLDynamicCast<HLSLFunction*>(HLSLDefinition* const Definition)
	{
		return Definition != nullptr 
			? (Definition->DefinitionType == GD_HLSL_DEFINITION_FUNCTION ? static_cast<HLSLFunction*>(Definition) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLFunction const* HLSLDynamicCast<HLSLFunction const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLFunction*>(const_cast<HLSLDefinition*>(Definition));
	}
#endif	// if !GD_DOCUMENTATION

#if !GD_DOCUMENTATION
	template<>
	GDINL HLSLCBuffer* HLSLDynamicCast<HLSLCBuffer*>(HLSLDefinition* const Definition)
	{
		return Definition != nullptr 
			? (Definition->DefinitionType == GD_HLSL_DEFINITION_CONSTANTBUFFER ? static_cast<HLSLCBuffer*>(Definition) : nullptr) : nullptr;
	}
	template<>
	GDINL HLSLCBuffer const* HLSLDynamicCast<HLSLCBuffer const*>(HLSLDefinition const* const Definition)
	{
		return HLSLDynamicCast<HLSLCBuffer*>(const_cast<HLSLDefinition*>(Definition));
	}
#endif	// if !GD_DOCUMENTATION

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_DESKTOP
