#include <GoddamnEngine/DInterface/DGFX/DGFX.h>
#include <GoddamnEngine/DInterface/DGFX/_DGFXDX11/_DGFXDX11.h>
#include <GoddamnEngine/DInterface/DEngineCore.h>
using namespace GD_NAMESPACE;

typedef float float4[4];
struct VertexType
{
	float4 position;
	float4 color;
};

static const VertexType triangle [] = 
{
	{ { -1.0f, -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.0f, 1.0f, 0.0f, 0.0f },   { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 1.0f, -1.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 1.0f, 1.0f } }
};
UINT indices [] = { 0, 1, 2 };

int main()
{
	DEngineCoreIterface* D = CreateDEngineCoreIterface();
	_DGFXDX11* G = GD_NEW(_DGFXDX11);

	G->Runtime_PreInitialize();
	G->Runtime_Initialize();
	G->Runtime_PostInitialize();

	DGFXHandle VertexBuffer, IndexBuffer;
	G->Gfx_BufferCreate(&VertexBuffer, DGFX_BUFFER_TYPE_VERTEX, sizeof(triangle), triangle);
	G->Gfx_BufferCreate(&IndexBuffer, DGFX_BUFFER_TYPE_INDEX, sizeof(indices), indices);

	DGFXHandle IndexedShape;
	G->Gfx_IndexedShapeCreate(&IndexedShape, DGFX_TOPOLOGY_TYPE_TRAINGLELIST);
	G->Gfx_IndexedShapeAttachVertexBuffer(IndexedShape, VertexBuffer);
	G->Gfx_IndexedShapeAttachIndexBuffer(IndexedShape, IndexBuffer);

	/*DGFXHandle VertexShader, PixelShader;
	G->Gfx_ShaderCreate(&VertexShader, DGFX_SHADER_TYPE_VERTEX, L"xvs.hlsl");
	G->Gfx_ShaderCreate(&PixelShader, DGFX_SHADER_TYPE_PIXEL, L"xps.hlsl");*/

	/*DGFXHandle LinkagePoint;
	G->Gfx_LinkagePointCreate(&LinkagePoint);
	G->Gfx_LinkagePointSetIndexedShape(LinkagePoint, IndexedShape);
	G->Gfx_LinkagePointSetShader(LinkagePoint, DGFX_SHADER_TYPE_VERTEX, VertexShader);
	G->Gfx_LinkagePointSetShader(LinkagePoint, DGFX_SHADER_TYPE_PIXEL, PixelShader);*/

	while (true)
	{
		G->Runtime_PreUpdate();
		G->Runtime_Update();
		/*G->Gfx_LinkagePointRender(LinkagePoint);*/
		G->Runtime_PostUpdate();
	}

	return 0;
}
