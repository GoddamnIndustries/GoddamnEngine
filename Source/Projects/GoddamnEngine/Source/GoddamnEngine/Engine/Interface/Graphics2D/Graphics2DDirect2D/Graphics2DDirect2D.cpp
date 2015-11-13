// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#define GD_IGRAPHICS2D_DIRECT2D_IMPL 1

/*!
 * @file GoddamnEngine/Engine/Interface/Graphics/GraphicsDirect2D/GraphicsDirect2D.h
 * File contains Implementation for Direct2D Implementation of the graphics interface.
 */
#include <GoddamnEngine/Engine/Interface/Graphics2D/Graphics2DDirect2D/Graphics2DDirect2D.h>
#include <GoddamnEngine/Engine/Interface/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX2D device (Direct2D)"

#include <d2d1helper.h>
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "d2d1.lib")

GD_NAMESPACE_BEGIN

	interface IGraphics2DDirect2D::WriteFontFileEnumerator final : public IDWriteFontFileEnumerator
	{
	private:
		SizeTp Index = 0;
	public:
		// IUnknown interface..
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override { return S_OK; }
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override final { return 1; }
		virtual ULONG STDMETHODCALLTYPE Release(void) override final { return 1; }

		// IDWriteFontFileEnumerator interface..
		STDMETHOD(MoveNext)(_Out_ BOOL* hasCurrentFile) override final { return -1; }
		STDMETHOD(GetCurrentFontFile)(_COM_Outptr_ IDWriteFontFile** fontFile) override final { return -1; }
	};	// interface IGraphics2DDirect2DFontFileEnumerator

	GD_IMPLEMENT_CLASS_INFORMATION(IGraphics2DDirect2D);
	GDEXP extern IGraphics2D* CreateIGraphics2DInstance()
	{
		return GD_NEW(IGraphics2DDirect2D);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Runtime..
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphics2DDirect2D::OnRuntimePostInitialize()
	{
		HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

		// Initialing the Direct2D interfaces - factory, render target and the default brush..
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_PPV_ARGS(&D2DFactory.p)));

		auto const d2dRenderTargetProperties     = D2D1::RenderTargetProperties();
		auto const d2dHwndRenderTargetProperties = D2D1::HwndRenderTargetProperties(static_cast<IGraphicsWindows*>(Graphics.GetPointer())->hwndMain
			, D2D1::Size(Graphics->Canvas_GetResolution()->Width, Graphics->Canvas_GetResolution()->Height));
		ThrowIfFailed(D2DFactory->CreateHwndRenderTarget(d2dRenderTargetProperties, d2dHwndRenderTargetProperties, &D2DRenderTarget.p));
		ThrowIfFailed(D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &D2DDefaultBrush));

		// Next, we need WIC to load the bitmaps into our Direct2D..
		ThrowIfFailed(CoInitialize(nullptr));
		ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&D2DWICImagingFactory.p)));

		// And finally, we need to load the DirectWrite interface to load TrueType fonts.
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(D2DDWFactory), reinterpret_cast<IUnknown**>(&D2DDWFactory.p)));
		ThrowIfFailed(D2DDWFactory->GetSystemFontCollection(&D2DDWFontCollectionDefault.p));

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame, before all other runtime
	//! interfaces are updated.
	GDAPI void IGraphics2DDirect2D::OnRuntimePreUpdate()
	{
		D2DRenderTarget->BeginDraw();
		D2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		D2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame, after all other runtime 
	//! interfaces are deinitialized.
	GDAPI void IGraphics2DDirect2D::OnRuntimePostUpdate()
	{
		D2DRenderTarget->EndDraw();
	}

GD_NAMESPACE_END
