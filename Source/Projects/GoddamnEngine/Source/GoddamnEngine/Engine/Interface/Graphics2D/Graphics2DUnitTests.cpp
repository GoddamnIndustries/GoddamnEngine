// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics2D/Graphics2DUnitTests.ñpp
 * File contains unit test for the 2D graphics subsystem.
 */
#include <GoddamnEngine/Engine/Interface/Graphics2D/Graphics2D.h>

GD_NAMESPACE_BEGIN

	//! @todo Write examples and tests here.
#if 0	// Simple example that runs the IGraphics2D.
	int main()
{
	GetPipelineManager()->InitializeAllPipelines();
	//	DEngineCoreIterface* D = CreateDEngineCoreIterface();
	ConsoleDevice->ShowConsole(true);

	IGraphics2DBitmapCreationInfo BitmapCreationInfo{ 128, 128, IGRAPHICS_FORMAT_R8G8B8_UNORM, Texture, sizeof(Texture) };
	IGraphics2DBitmap* Bitmap;
	Graphics2D->GfxImm_BitmapCreate(&Bitmap, &BitmapCreationInfo);
	IGraphics2DRectRendererCreationInfo RectRendererCreationInfo{ {}, Bitmap };
	IGraphics2DRectRenderer* RectRenderer;
	Graphics2D->GfxImm_RectRendererCreate(&RectRenderer, &RectRendererCreationInfo);
	RectRenderer->Imm_SetTransform({ 100, 100, 1024, 512 }, 45.0f);
	RectRenderer->Imm_SetColor({ 0.0f, 0.0f, 1.0f, 1.0f });

	auto const A = Bitmap->GetClassInformation();

	IGraphics2DFontCreationInfo FontCreationInfo{ L"H:\\Wild Gold\\Assets\\UI\\OldTownRegular.ttf", IGRPAHICS2D_FONT_LOCATION_FILE };
	IGraphics2DFont* Font = nullptr;
	Graphics2D->GfxImm_FontCreate(&Font, &FontCreationInfo);
	IGraphics2DTextRendererCreationInfo StringRendererCreationInfo = { GD_IGRAPHICS2D_TEXT_RENDERER_DATA(L"HUI"), Font, 32.0f };
	IGraphics2DTextRenderer* StringRenderer = nullptr;
	Graphics2D->GfxImm_TextRendererCreate(&StringRenderer, &StringRendererCreationInfo);

	while (true)
	{
		Graphics->OnRuntimePreUpdate();
		Graphics->OnRuntimeUpdate();
		Graphics2D->OnRuntimePreUpdate();
		Graphics2D->OnRuntimeUpdate();
		Input->OnRuntimeUpdate();
		DateTime->OnRuntimeUpdate();

		StringRenderer->Imm_Render();
		RectRenderer->Imm_Render();

		DateTime->OnRuntimePostUpdate();
		Graphics2D->OnRuntimePostUpdate();
		//	Graphics->OnRuntimePostUpdate();
		DateTime->OnRuntimePostUpdate();
	}

	return 0;
}

#endif

GD_NAMESPACE_END
