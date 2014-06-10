#include <GoddamnShader/SimplestShader/SimplestShader.h.hlsl>

struct VertexInput
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD0;
};

void Main(in  const VertexInput		IN_VERTEX_INPUT,
		  out VertexToFragment		OUT_V2F)
{
	OUT_V2F.position = float4(IN_VERTEX_INPUT.position, 1.0);
	OUT_V2F.normal = IN_VERTEX_INPUT.normal;
	OUT_V2F.uv = IN_VERTEX_INPUT.uv;
}
