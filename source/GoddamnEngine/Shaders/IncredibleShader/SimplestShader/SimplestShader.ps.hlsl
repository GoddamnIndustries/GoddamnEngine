#include <GoddamnShader/SimplestShader/SimplestShader.h.hlsl>

void Main(in  const VertexToFragment	IN_V2F,
		  out float4					OUT_COLOR : SV_Target0)
{
	OUT_COLOR = float4(normalize(abs(float3(IN_V2F.normal))), 1.0f);
}
