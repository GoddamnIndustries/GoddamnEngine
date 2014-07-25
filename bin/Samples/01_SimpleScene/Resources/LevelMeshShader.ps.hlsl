struct VertexToPixel
{
	float4 FragCoord	: SV_Position;
	float3 Position		: TEXCOORD0;
	float3 Normal		: TEXCOORD2;
	float2 TexCoord		: TEXCOORD3;
};

//Texture2D TestTexture : register(t0);
//SamplerState TestTextureSampler : register(s0);

void Main(
	in  const	VertexToPixel	IN_V2P,
	out float4	OUT_COLOR : SV_Target0,
	out float4	OUT_POSITION : SV_Target1,
	out float4	OUT_NORMAL : SV_Target2
)
{
	OUT_POSITION = float4(IN_V2P.Position, IN_V2P.FragCoord.a);
	OUT_NORMAL	 = float4(IN_V2P.Normal, 1.0f);
	OUT_COLOR    = float4(normalize(abs(IN_V2P.TexCoord)), 1.0, 1.0f);// TestTexture.Sample(TestTextureSampler, IN_V2P.TexCoord);
}
