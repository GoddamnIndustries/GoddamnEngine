// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsUnitTests.ñpp
 * File contains unit test for the graphics subsystem.
 */
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

GD_NAMESPACE_BEGIN

	//! @todo Write examples and tests here.

#if 0	// Simple example that runs the IGraphics3D.

typedef float float4[4];
typedef float float4x4[4 * 4];

static const float4 Positions[] =
{
	{ -1.0f, -1.0f, 0.0f, 1.0f },
	{ -1.0f,  1.0f, 0.0f, 1.0f },
	{  1.0f,  1.0f, 0.0f, 1.0f },
	{  1.0f, -1.0f, 0.0f, 1.0f },
};
static const float4 Colors[] =
{
	{ 0.0f, 0.0f, 1.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f, 1.0f },
};

static const unsigned Indices[] =
{
	0, 1, 2, 0, 2, 3
};

//static const float4x4 Matrix =
//{
//	0.1f, 0.0f, 0.0f, 0.0f,
//	0.0f, 0.5f, 0.0f, 0.0f,
//	0.0f, 0.0f, 1.1f, 0.0f,
//	0.0f, 0.0f, 0.0f, 1.0f,
//};


static const float4 Color =
{
	0.1f, 0.1f, 1.0f, 1.0f
};

static const Byte Texture[] = {
#if !__INTELLISENSE__
#include "demon_image.h"
#else	// if !__INTELISENSE__
	0
#endif	// if !__INTELISENSE__
};

char static const VertexShaderCode[] = R"(
	layout(location = 0) in vec4 in_VertexPosition;
	layout(location = 1) in vec4 in_VertexColor;
	layout(location = 0) out vec4 inout_VertexPosition;
	layout(location = 1) out vec4 inout_VertexColor;
	
	layout(binding = 0, std140, row_major) 
	uniform uniformIn_MatrixBuffers
	{
		mat4x4 uniformIn_MVPMatrix;
	};

	void main()
	{
		inout_VertexPosition = in_VertexPosition * uniformIn_MVPMatrix;
		inout_VertexColor = in_VertexColor;
		gl_Position = inout_VertexPosition;
	}
)";

char static const PixelShaderCode[] = R"(
	layout(location = 0) in vec4 inout_VertexPosition;
	layout(location = 1) in vec4 inout_VertexColor;
	out vec4 out_FragmentColor;

	layout(binding = 14, std140, row_major) 
	uniform uniformIn_ColorBuffers
	{
		vec4 uniformIn_ColorTint;
	};

	layout(binding = 8) 
	uniform sampler2D uniformIn_Texture;

	void main()
	{
	//	out_FragmentColor = uniformIn_ColorTint * inout_VertexColor;
		out_FragmentColor = (texture(uniformIn_Texture, vec2(0.5) * (inout_VertexPosition.xy + vec2(1.0, 1.0))));
	}
)";

char static const PixelShaderCodeHLSL[] = R"(
struct PixelInputType
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
};
Texture2D shaderTexture : register(t0);
SamplerState shaderTextureSampler : register(s0);

void TexturePixelShader(PixelInputType _input, out float4 _output : SV_Target)
{
#define texture(a, b) a.Sample(a##Sampler, b)
    float4 textureColor;
    _output = texture(shaderTexture, _input.tex);
}
)";

int main()
{
	auto I = GD_NEW(AImage);
	I->OnImportFromPath("C:\\Users\\Oleg\\Pictures\\IMG_1045.JPG");

	StringBuilder b;
	auto s = GD_NEW(StringInputStream, PixelShaderCodeHLSL, GD_ARRAY_LENGTH(PixelShaderCodeHLSL));
	DHLSLCrossCompiler().CrossCompileShader(s
		, GD_NEW(StringOutputStream, b), IGRAPHICS_SHADER_TYPE_PIXEL, DHLSL_CC_TARGET_HLSL50, "TexturePixelShader");

	GetPipelineManager()->InitializeAllPipelines();
//	DEngineCoreIterface* D = CreateDEngineCoreIterface();
	ConsoleDevice->ShowConsole(true);

	auto c = ConsoleDevice->GetTypeInfo();

	// Setting up the vertex buffers:
	IGraphicsBufferCreationInfo VertexPositionBufferInfo{ IGRAPHICS_BUFFER_TYPE_VERTEX, sizeof(Positions) };
	IGraphicsBuffer* VertexPositionBuffer = nullptr;
	Graphics->GfxImm_BufferCreate(&VertexPositionBuffer, &VertexPositionBufferInfo, Positions);
	IGraphicsBufferCreationInfo VertexColorBufferInfo{ IGRAPHICS_BUFFER_TYPE_VERTEX, sizeof(Colors) };
	IGraphicsBuffer* VertexColorBuffer = nullptr;
	Graphics->GfxImm_BufferCreate(&VertexColorBuffer, &VertexColorBufferInfo, Colors);

	// Setting up the m_Index buffer:
	IGraphicsBufferCreationInfo IndexBufferInfo{ IGRAPHICS_BUFFER_TYPE_INDEX, sizeof(Indices) };
	IGraphicsBuffer* IndexBuffer = nullptr;
	Graphics->GfxImm_BufferCreate(&IndexBuffer, &IndexBufferInfo, Indices);

	// Setting up the input layout.
	IGraphicsVertexArrayLayoutSlot VertexArrayLayoutSlots[] = {
		{ IGRAPHICS_FORMAT_R32G32B32A32_FLOAT, IGRAPHICS_SEMANTIC_POSITION, 0, IGRAPHICS_INPUT_SLOT_CLASS_PER_VERTEX },
		{ IGRAPHICS_FORMAT_R32G32B32A32_FLOAT, IGRAPHICS_SEMANTIC_COLOR, 0, IGRAPHICS_INPUT_SLOT_CLASS_PER_VERTEX }
	};
	IGraphicsVertexArrayLayout VertexArrayLayout{ VertexArrayLayoutSlots, GD_ARRAY_LENGTH(VertexArrayLayoutSlots) };

	// Setting up the vertex array.
	IGraphicsBuffer const* VertexArrayBuffers[] = { VertexPositionBuffer, VertexColorBuffer };
	IGraphicsVertexArrayCreationInfo VertexArrayInfo{ &VertexArrayLayout, VertexArrayBuffers, IndexBuffer };
	IGraphicsVertexArray* VertexArray = nullptr;
	Graphics->GfxImm_VertexArrayCreate(&VertexArray, &VertexArrayInfo);

	// Setting up the texture.
	IGraphicsSamplerStateCreationInfo SamplerStateInfo{};
	IGraphicsSamplerState* SamplerState = nullptr;
	SamplerStateInfo.SamplerMaxAnisotropy = 10;
	Graphics->GfxImm_SamplerStateCreate(&SamplerState, &SamplerStateInfo);
	IGraphicsTexture2DCreationInfo Texture2DInfo{ 128, 128, IGRAPHICS_FORMAT_R8G8B8_UNORM };
	IGraphicsTexture2DData Texture2DData{ Texture };
	IGraphicsTexture2D* Texture2D = nullptr;
	Graphics->GfxImm_Texture2DCreate(&Texture2D, &Texture2DInfo, &Texture2DData);
	IGraphicsShaderResourceView const* Texture2DResource = Texture2D->CPU_GetShaderResourceView();
	Graphics->GfxCmd_PixelShaderBindResources(nullptr, &Texture2DResource, 1);

	// Setting up the shaders.
	IGraphicsVertexShader* VertexShader;
	IGraphicsShaderCreationInfo VertexShaderInfo{ VertexShaderCode, sizeof(VertexShaderCode) };
	Graphics->GfxImm_VertexShaderCreate(&VertexShader, &VertexShaderInfo, &VertexArrayLayout);
	IGraphicsBuffer* UniformVertexBuffer;
	IGraphicsBufferCreationInfo UniformVertexBufferInfo{ IGRAPHICS_BUFFER_TYPE_UNIFORM, sizeof(Matrix4x4), IGRAPHICS_BUFFER_FLAGS_DYNAMIC_WRITE };
	Graphics->GfxImm_BufferCreate(&UniformVertexBuffer, &UniformVertexBufferInfo, nullptr);
	Graphics->GfxCmd_VertexShaderBindUniformBuffers(nullptr, &UniformVertexBuffer, 1);

	IGraphicsPixelShader* PixelShader;
	IGraphicsShaderCreationInfo PixelShaderInfo{ PixelShaderCode, sizeof(PixelShaderCode) };
	Graphics->GfxImm_PixelShaderCreate(&PixelShader, &PixelShaderInfo);
	IGraphicsBuffer* UniformPixelBuffer;
	IGraphicsBufferCreationInfo UniformPixelBufferInfo{ IGRAPHICS_BUFFER_TYPE_UNIFORM, sizeof(Color) };
	Graphics->GfxImm_BufferCreate(&UniformPixelBuffer, &UniformPixelBufferInfo, Color);
	Graphics->GfxCmd_PixelShaderBindUniformBuffers(nullptr, &UniformPixelBuffer, 1);
	Graphics->GfxCmd_PixelShaderBindSamplers(nullptr, &SamplerState, 1);

	// Setting up our pipeline.
	IGraphicsPipelineState* Pipeline;
	IGraphicsPipelineStateCreationInfo PipelineInfo{ VertexShader, PixelShader };
	Graphics->GfxImm_PipelineCreate(&Pipeline, &PipelineInfo);

	while (true)
	{
		Graphics->OnRuntimePreUpdate();
		Graphics->OnRuntimeUpdate();
		Input->OnRuntimeUpdate();
		DateTime->OnRuntimeUpdate();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0f);
		glClearColor(0.3f, 0.275f, 1.0f, 1.0f);
		/*Graphics->gfx_LinkagePointRender(LinkagePoint, nullptr)*/;
		glDrawElements(GL_TRIANGLES, GD_ARRAY_LENGTH(Indices), GL_UNSIGNED_INT, nullptr);

		Matrix4x4* Matrix;
		UniformVertexBuffer->Imm_MapForWriting((Handle*)&Matrix);
		*Matrix = Matrix4x4::TranslateRotateScale(Vector3(0.1f), Quaternion::FromEulerRadians(Vector3(0.0f, 0.0f, (float)M_PI / 2.0f)), Vector3(-1.0f));
		UniformVertexBuffer->Imm_Unmap();

		if (Input->Mouse_GetButtonState(IINPUT_BUTTON_MOUSE_MIDDLE) == IINPUT_BUTTONSTATE_UP)
			Input->Gamepad_Vibrate(0.1, 0.5f, 0.0f);

		DateTime->OnRuntimePostUpdate();
		Graphics->OnRuntimePostUpdate();
		DateTime->OnRuntimePostUpdate();
	}

	return 0;
}

#endif	// if 0

GD_NAMESPACE_END
