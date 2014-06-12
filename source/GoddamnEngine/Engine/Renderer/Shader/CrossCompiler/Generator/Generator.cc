//////////////////////////////////////////////////////////////////////////
/// Generator.cpp: shader cross compiler GLSL generator implementation.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/Generator/Generator.hh>

GD_NAMESPACE_BEGIN	

	static const Str GLSLInsertation = R"(
//////////////////////////////////////////////////////////////////////////
/// This file was automatically translated from HLSL by Goddamn Engine.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
//////////////////////////////////////////////////////////////////////////

#version 420
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_separate_shader_objects  : enable

// We do not have this keywords in GLSL...
#define static
#define inline
#define const

// Mathcing types...
#define bool2   bvec2
#define bool3   bvec3
#define bool4   bvec4
#define int2    ivec2
#define int3    ivec3
#define int4    ivec4
#define uint2   uvec2
#define uint3   uvec3
#define uint4   uvec4
#define float2   vec2
#define float3   vec3
#define float4   vec4
#define double2 dvec2
#define double3 dvec3
#define double4 dvec4)";

GD_NAMESPACE_END
