#ifndef GD_HRI_SHADER_SIMPLEST
#define GD_HRI_SHADER_SIMPLEST

struct VertexToFragment
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD;
	float3 normal	: TEXCOORD1;
};

#endif
