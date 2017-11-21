#define GD_DLL_IMPORT 1

#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Object/Enum.h>
#include <GoddamnEngine/Core/Object/ObjectVisitor.h>
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>
#include <GoddamnEngine/Core/IO/FileStream.h>
#include <GoddamnEngine/Core/Containers/List.h>

#include <GoddamnEngine/Engine/Entity/Scene.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>
#include <GoddamnEngine/Engine/Resources/Mesh.h>
#include "GoddamnEngine/Core/Math/Vector.h"
#include "../../../GoddamnGraphics/GoddamnGraphicsOpenGL/Dependencies/glew/include/GL/glew.h"

using namespace GD_NAMESPACE;
#pragma comment(lib, "opengl32.lib")

#if 0
GD_DECLARE_ENUM_CLASS(MyEnum, int
	, FirstEnumValue
	, SecondEnumValue = 300
	, ThirdEnumValue = 3 + 0
	);

class MyObject : public SerializableObject
{
	GD_DECLARE_OBJECT(MyObject, SerializableObject, "");

public:

	/*!
		* First property. 
		*/
	GD_PROPERTY(public, Int64, m_FirstProperty, PFNone) = -13;

	/*!
		* Second property. 
		*/
	GD_PROPERTY(public, String, m_SecondProperty, PFNone) = "hui";

	/*!
		* Third property.
 		*/
	GD_PROPERTY(public, Vector<String>, m_ThirdProperty, PFNone) = { "aaa", "bb", "-", "c" };

public:
};	// class MyObject

GD_IMPLEMENT_OBJECT(GDINT, MyObject);

MyObject::~MyObject()
{
}

int main()
{

	/*MyEnum e;
	e = MyEnum::ThirdEnumValue;
	auto const myEnumInfo = EnumClass<MyEnum>::ToString(e);
*/
	auto i = SharedPtr<InputStream>(gd_new FileInputStream(L"./test.json"));
	auto l = SharedPtr<OutputStream>(gd_new FileOutputStream(L"out.json"));

	auto const t0 = clock();
	for (int t = 0; t < 1000; ++t)
	{
		i->Seek(0, CStdio::FseekOrigin::SeekSet);
		auto const o = Object::FindOrCreateObject(EmptyGUID, MyObject::GetStaticClass());
		auto const s = static_cast<SerializableObject*>(o.Get());
		s->DeserializeSync(i);
	}
	
	auto const t1 = clock() - t0;
	printf_s("\n%d", t1);

	/*auto const package = PackageManager::FindOrCreatePackage(L"hui");
	
	package->Save();*/

	_getc_nolock(stdin);
}
#endif	// if 0

int main()
{
	Graphics->OnRuntimePreInitialize();
	Graphics->OnRuntimeInitialize();
	Graphics->OnRuntimePostInitialize();

	Vector3 const positions[3] = {
		Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)
	};
	Vector3 const colors[3] = {
		Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)
	};

	IGraphicsBuffer* vertexBuffers[2] = {};
	IGraphicsVertexArrayLayoutSlot layoutSlots[2] = {};

	IGraphicsBuffer*& positionBuffer = vertexBuffers[0];
	IGraphicsBufferCreationInfo positionBufferCI = {};
	positionBufferCI.gfxBufferType = IGRAPHICS_BUFFER_TYPE_VERTEX;
	positionBufferCI.gfxBufferSize = sizeof(positions);
	Graphics->GfxImm_BufferCreate(&positionBuffer, &positionBufferCI, &positions);
	IGraphicsVertexArrayLayoutSlot& positionSlot = layoutSlots[0];
	positionSlot.SlotFormat = IGRAPHICS_FORMAT_R32G32B32_FLOAT;
	positionSlot.SlotSemantic = IGRAPHICS_SEMANTIC_POSITION;
	positionSlot.SlotSemanticIndex = 0;
	positionSlot.SlotClass = IGRAPHICS_INPUT_SLOT_CLASS_PER_VERTEX;

	IGraphicsBuffer*& colorBuffer = vertexBuffers[1];
	IGraphicsBufferCreationInfo colorBufferCI = {};
	colorBufferCI.gfxBufferType = IGRAPHICS_BUFFER_TYPE_VERTEX;
	colorBufferCI.gfxBufferSize = sizeof(colors);
	Graphics->GfxImm_BufferCreate(&colorBuffer, &colorBufferCI, &colors);
	IGraphicsVertexArrayLayoutSlot& colorSlot = layoutSlots[1];
	colorSlot.SlotFormat = IGRAPHICS_FORMAT_R32G32B32_FLOAT;
	colorSlot.SlotSemantic = IGRAPHICS_SEMANTIC_COLOR;
	colorSlot.SlotSemanticIndex = 0;
	colorSlot.SlotClass = IGRAPHICS_INPUT_SLOT_CLASS_PER_VERTEX;

	IGraphicsVertexArrayLayout layout = {};
	layout.LayoutSlots = layoutSlots;
	layout.LayoutSlotsCount = 2;

	IGraphicsVertexArray* vertexArray = nullptr;
	IGraphicsVertexArrayCreationInfo vertexArrayCI;
	vertexArrayCI.VertexArrayLayout = &layout;
	vertexArrayCI.ArrayIndexBuffer = nullptr;
	vertexArrayCI.ArrayVertexBuffers = vertexBuffers;
	Graphics->GfxImm_VertexArrayCreate(&vertexArray, &vertexArrayCI);

	IGraphicsVertexShader* vertexShader;
	IGraphicsShaderCreationInfo vertexShaderCI = {};
	vertexShaderCI.ShaderType = IGRAPHICS_SHADER_TYPE_VERTEX;
	vertexShaderCI.ShaderByteCode = R"(
layout(location = 0) in vec4 in_VertexPosition;
layout(location = 1) in vec4 in_VertexColor;
out vec4 out_VertexColor;

void main()
{
	out_VertexColor = in_VertexColor;
	gl_Position = in_VertexPosition - vec4(0.5, 0.0, 0.0, 0.0);
}
	)";
	Graphics->GfxImm_VertexShaderCreate(&vertexShader, &vertexShaderCI, &layout);

	IGraphicsPixelShader* pixelShader;
	IGraphicsShaderCreationInfo pixelShaderCI = {};
	pixelShaderCI.ShaderType = IGRAPHICS_SHADER_TYPE_PIXEL;
	pixelShaderCI.ShaderByteCode = R"(
in vec4 in_VertexColor;
out vec4 out_FragmentColor;

void main()
{
	out_FragmentColor = in_VertexColor;
	out_FragmentColor.a = 0.5f;
}
	)";
	Graphics->GfxImm_PixelShaderCreate(&pixelShader, &pixelShaderCI);

	IGraphicsPipelineState* pipelineState = nullptr;
	IGraphicsPipelineStateCreationInfo pipelineStateCI = {};
	pipelineStateCI.PipelineVertexShader = vertexShader;
	pipelineStateCI.PipelinePixelShader = pixelShader;
	Graphics->GfxImm_PipelineCreate(&pipelineState, &pipelineStateCI);

	auto const scene = Object::CreateOrFindClassRelatedObjectByGUID<Scene>(EmptyGUID);
	
	{	/* And runnting the whole shit. */
		scene->OnInit();
		scene->OnStart();
		while (true)
		{
			Graphics->OnRuntimePreUpdate();
			Graphics->GfxCmd_RenderTargetClearDepth(nullptr);
			Graphics->GfxCmd_RenderTargetClearColor(nullptr, { 1, 0, 1, 0.5f });
			scene->OnPreUpdate();

			Graphics->OnRuntimeUpdate();
			scene->OnUpdate();
			scene->OnRender();

			Graphics->GfxCmd_VertexArrayBind(nullptr, vertexArray);
			Graphics->GfxCmd_PipelineBind(nullptr, pipelineState);
			Graphics->GfxCmd_RenderTargetRender(nullptr, IGRAPHICS_SHADER_TOPOLOGY_TRIANGLES, 3);

			Graphics->OnRuntimePostUpdate();
			scene->OnPostUpdate();

		}
	}
}
