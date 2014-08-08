struct VertexToPixel
{
	float4 FragCoord	: SV_Position;
	float3 Position		: TEXCOORD0;
	float3 Normal		: TEXCOORD2;
	float2 TexCoord		: TEXCOORD3;
};	// struct VertexToPixel

cbuffer VertexMatrices : register(b0)
{
	float4x4 VertexMvpMatrix;
	float4x4 VertexModelMatrix;
	float3x3 VertexNormalMatrix;
}	// cbuffer VertexMatrices

struct VertexInput
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
	float2 TexCoord	: TEXCOORD0;
};	// struct VertexInput

void Main(in  VertexInput	IN_VERTEX_INPUT,
		  out VertexToPixel	OUT_V2P)
{
//	OUT_V2P.FragCoord = float4(IN_VERTEX_INPUT.Position, 1.0);
	OUT_V2P.FragCoord = mul(float4(IN_VERTEX_INPUT.Position, 1.0), VertexMvpMatrix);
	OUT_V2P.Position  = float3(mul(float4(IN_VERTEX_INPUT.Position, 1.0), VertexModelMatrix).xyz);
	OUT_V2P.Normal	  = /*mul(_VertexNormalMatrix, */IN_VERTEX_INPUT.Normal/*)*/;
	OUT_V2P.TexCoord  = IN_VERTEX_INPUT.TexCoord;
}

