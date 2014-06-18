//////////////////////////////////////////////////////////////////////////
/// This file was automatically translated from HLSL by Goddamn Engine.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////

#version 430
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_separate_shader_objects   : enable
struct VertexToPixel
{
	vec4  FragCoord;
	vec3  Position;
	vec3  Normal;
	vec2  TexCoord;
};

layout(std140, row_major, binding = 0) uniform VertexMatrices
{
	mat4x4  VertexMvpMatrix;
	mat4x4  VertexModelMatrix;
	mat3x3  VertexNormalMatrix;
};

struct VertexInput
{
	vec3  Position;
	vec3  Normal;
	vec2  TexCoord;
};

 void Main(in VertexInput IN_VERTEX_INPUT, out VertexToPixel OUT_V2P)
{
	OUT_V2P.FragCoord =  (VertexMvpMatrix * vec4 (IN_VERTEX_INPUT.Position, 1.0)) ;
	OUT_V2P.Position =  vec3 ( (VertexModelMatrix * vec4 (IN_VERTEX_INPUT.Position, 1.0)) .xyz);
	OUT_V2P.Normal = IN_VERTEX_INPUT.Normal ;
	OUT_V2P.TexCoord = IN_VERTEX_INPUT.TexCoord;
}

layout(location = 0) in  vec3  VaryingIN_VERTEX_INPUTPosition;
layout(location = 1) in  vec3  VaryingIN_VERTEX_INPUTNormal;
layout(location = 2) in  vec2  VaryingIN_VERTEX_INPUTTexCoord;
layout(location = 0) out  vec4  VaryingOUT_V2PFragCoord;
layout(location = 1) out  vec3  VaryingOUT_V2PPosition;
layout(location = 2) out  vec3  VaryingOUT_V2PNormal;
layout(location = 3) out  vec2  VaryingOUT_V2PTexCoord;

void main()
{
	VertexToPixel OUT_V2P;

	Main(VertexInput(VaryingIN_VERTEX_INPUTPosition, VaryingIN_VERTEX_INPUTNormal, VaryingIN_VERTEX_INPUTTexCoord), OUT_V2P);

	VaryingOUT_V2PFragCoord = OUT_V2P.FragCoord;
	VaryingOUT_V2PPosition = OUT_V2P.Position;
	VaryingOUT_V2PNormal = OUT_V2P.Normal;
	VaryingOUT_V2PTexCoord = OUT_V2P.TexCoord;
}
