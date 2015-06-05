// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Platform/Windows/WindowsSplashScreen.cpp
//! Windows Splash screen definitions.

#include <GoddamnEngine/Core/Platform/SplashScreen.h>
#if GD_PLATFORM_WINDOWS

#include <GoddamnEngine/Core/Platform/Application.h>

#include <StrSafe.h>
#include <WinCodec.h>
#pragma comment(lib, "windowscodecs.lib")

GD_NAMESPACE_BEGIN

	//! Name of a Splash screen window class.
#if defined(GD_NAMESPACE)
	CHAR const WindowsSplashScreen::SplashScreenClassName[] = GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "_WindowsSplashScreen");
#else	// if defined(GD_NAMESPACE)
	CHAR const WindowsSplashScreen::SplashScreenClassName[] = "Goddamn_WindowsSplashScreen";
#endif	// if defined(GD_NAMESPACE)

	//! Initializes a splash screen.
	//! @note This method should be invoked in child class.
	GDAPI void WindowsSplashScreen::InitializeObject_Inst()
	{
		GD_DEBUG_ASSERT(this->SplashScreenThread == nullptr, "Error: 'WindowsSplashScreen' class was already initialized.");

		IGenericSplashScreen::InitializeObject_Inst();

		this->SplashScreenGuard = ::CreateWindowExA(0, "STATIC", "Goddamn_WindowsSplashScreenGuard", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetApplicationHInstance(), nullptr);
		GD_ASSERT(this->SplashScreenGuard != nullptr, "Failed to create a Splash screen guard window.");

		this->SplashScreenThread = ::CreateThread(nullptr, 128 * 1024, &WindowsSplashScreen::SplashScreenThreadProc, this, STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr);
		GD_ASSERT(this->SplashScreenThread != nullptr, "Failed to create a Splash screen UI thread.");
	}

	//! Destroys a splash screen.
	GDAPI void WindowsSplashScreen::DestroyObject_Inst()
	{
		GD_DEBUG_ASSERT(this->SplashScreenThread != nullptr, "Error: 'WindowsSplashScreen' class was not initialized.");

		PostMessageA(this->SplashScreenWindow, WM_DESTROY, 0, 0);

		WaitForSingleObject(this->SplashScreenThread, INFINITE);
		CloseHandle(this->SplashScreenThread);
		this->SplashScreenThread = nullptr;

		DestroyWindow(this->SplashScreenWindow);
		this->SplashScreenWindow = nullptr;

		DestroyWindow(this->SplashScreenGuard);
		this->SplashScreenGuard = nullptr;
	}

	//! Shows or hides a splash screen.
	//! @param SplashDoShow Whether to show or hide splash screen.
	GDAPI void WindowsSplashScreen::ShowSplash_Inst(bool const SplashDoShow)
	{
		GD_DEBUG_ASSERT(this->SplashScreenThread != nullptr, "Error: 'WindowsSplashScreen' class was not initialized.");

		if (SplashDoShow)
		{
			if (!this->SplashScreenIsShown)
			{
				this->SplashScreenIsShown = true;
				ShowWindow(this->SplashScreenWindow, SW_SHOW);
			}
		}
		else
		{
			if (this->SplashScreenIsShown)
			{
				this->SplashScreenIsShown = false;
				ShowWindow(this->SplashScreenWindow, SW_HIDE);
			}
		}
	}

	//! Returns true, if splash is shown.
	//! @returns True, if splash is shown.
	GDAPI bool WindowsSplashScreen::IsSplashShown_Inst() const
	{
		GD_DEBUG_ASSERT(this->SplashScreenThread != nullptr, "Error: 'WindowsSplashScreen' class was not initialized.");
		return this->SplashScreenIsShown;
	}

	//! Sets text that is displayed in some region of a splash screen.
	//! @param SplashTextRegion Region to which set new text.
	//! @param SplashText Text to set.
	GDAPI void WindowsSplashScreen::SetSplashText_Inst(SplashScreenTextRegionTp const SplashTextRegion, String const& SplashText)
	{
		GD_DEBUG_ASSERT(this->SplashScreenThread != nullptr, "Error: 'WindowsSplashScreen' class was not initialized.");
		GD_DEBUG_ASSERT(SplashTextRegion <= SplashScreenTextRegion::RegionsCount, "Error: 'WindowsSplashScreen' invalid region specified.");

		bool SplashScreenTextWasUpdated = false;
		{
			ScopedLock SplashScreenLock(&this->SplashScreenSyncObject);
			if (SplashText != this->SplashScreenRegionTexts[SplashTextRegion])
			{
				SplashScreenTextWasUpdated = true;
				this->SplashScreenRegionTexts[SplashTextRegion] = SplashText;
			}
		}
		if (SplashScreenTextWasUpdated)
		{
			InvalidateRect(this->SplashScreenWindow, &this->SplashScreenRegionRects[SplashTextRegion], false);
		}
	}

	//! Splash screen thread routine.
	GDINT DWORD WINAPI WindowsSplashScreen::SplashScreenThreadProc(LPVOID const SplashScreenThreadParameter)
	{
		auto SplashScreen = reinterpret_cast<WindowsSplashScreen*>(SplashScreenThreadParameter);
		HRESULT SplashScreenResult;

		// Initializing the splash screen resources and window.
		{
			ScopedLock SplashScreenLock(&SplashScreen->SplashScreenSyncObject);

			// Registering Splash Screen window class..
			{
				WNDCLASSA SplashScreenClass		= { 0 };
				SplashScreenClass.style			= CS_HREDRAW | CS_VREDRAW;
				SplashScreenClass.cbClsExtra	= 0;
				SplashScreenClass.cbWndExtra	= 0;
				SplashScreenClass.hInstance		= GetApplicationHInstance();
				SplashScreenClass.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
				SplashScreenClass.hCursor		= LoadCursor(nullptr, IDC_ARROW);
				SplashScreenClass.hbrBackground	= reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
				SplashScreenClass.lpszMenuName	= nullptr;
				SplashScreenClass.lpszClassName	= WindowsSplashScreen::SplashScreenClassName;
				SplashScreenClass.lpfnWndProc	= &WindowsSplashScreen::SplashScreenWindowProc;

				ATOM const Result = RegisterClassA(&SplashScreenClass);
				GD_DEBUG_ASSERT(Result != 0, "Error: 'WindowsSplashScreen' failed to register Splash screen window class.");
			}

			// Loading Splash Screen bitmap..
			{
				SplashScreenResult = CoInitialize(nullptr);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': 'CoInitialize' failed.");

				IWICImagingFactory* SplashScreenImagingFactory = nullptr;
				CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&SplashScreenImagingFactory));
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to create 'SplashScreenImagingFactory'.");

				IWICBitmapDecoder* SplashScreenBitmapDecoder = nullptr;
				SplashScreenResult = SplashScreenImagingFactory->CreateDecoderFromFilename(SplashScreen->SplashScreenFilename.CStr(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &SplashScreenBitmapDecoder);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to create 'SplashScreenBitmapDecoder'.");

				IWICFormatConverter* SplashScreenBitmapConverter = nullptr;
				SplashScreenResult = SplashScreenImagingFactory->CreateFormatConverter(&SplashScreenBitmapConverter);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to create 'SplashScreenBitmapConverter'.");

				IWICBitmapFrameDecode* SplashScreenBitmapFrame = nullptr;
				SplashScreenResult = SplashScreenBitmapDecoder->GetFrame(0, &SplashScreenBitmapFrame);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to retrieve 'SplashScreenBitmapFrame'.");

				IWICBitmapSource* SplashScreenOriginalBitmapSource = nullptr;
				SplashScreenResult = SplashScreenBitmapFrame->QueryInterface(IID_IWICBitmapSource, reinterpret_cast<void**>(&SplashScreenOriginalBitmapSource));
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to retrieve 'SplashScreenOriginalBitmapSource'.");

				SplashScreenResult = SplashScreenBitmapConverter->Initialize(SplashScreenBitmapFrame, GUID_WICPixelFormat32bppBGR, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to initialize 'SplashScreenBitmapConverter'.");

				IWICBitmapSource* SplashScreenRenderedBitmapSource = nullptr;
				SplashScreenResult = SplashScreenBitmapConverter->QueryInterface(IID_PPV_ARGS(&SplashScreenRenderedBitmapSource));
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to retrieve 'SplashScreenRenderedBitmapSource'.");

				WICPixelFormatGUID SplashScreenPixelFormat = { 0 };
				SplashScreenResult = SplashScreenRenderedBitmapSource->GetPixelFormat(&SplashScreenPixelFormat);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to retrieve 'SplashScreenPixelFormat'.");
				GD_ASSERT(SplashScreenPixelFormat == GUID_WICPixelFormat32bppBGR, "Error: 'WindowsSplashScreen': Retrieved 'SplashScreenPixelFormat' is invalid.");

				UINT SplashScreenWidth = 0;
				UINT SplashScreenHeight = 0;
				SplashScreenResult = SplashScreenRenderedBitmapSource->GetSize(&SplashScreenWidth, &SplashScreenHeight);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': Failed to retrieve 'SplashScreenWidth' or 'SplashScreenHeight'.");

				HDC const SplashScreenDC = GetDC(nullptr);
				GD_ASSERT(SplashScreenDC != nullptr, "Error: 'WindowsSplashScreen': Failed to retrieve 'SplashScreenDC'.");

				LPVOID SplashScreenData = nullptr;
				BITMAPINFO SplashScreenBitmapInfo				= { 0 };
				SplashScreenBitmapInfo.bmiHeader.biSize			= sizeof(SplashScreenBitmapInfo.bmiHeader);
				SplashScreenBitmapInfo.bmiHeader.biWidth		= static_cast<LONG>(SplashScreenWidth);
				SplashScreenBitmapInfo.bmiHeader.biHeight		= static_cast<LONG>(SplashScreenHeight) * -1;
				SplashScreenBitmapInfo.bmiHeader.biPlanes		= 1;
				SplashScreenBitmapInfo.bmiHeader.biBitCount		= 32;
				SplashScreenBitmapInfo.bmiHeader.biCompression	= BI_RGB;
				SplashScreen->SplashScreenBitmap = CreateDIBSection(SplashScreenDC, &SplashScreenBitmapInfo, DIB_RGB_COLORS, &SplashScreenData, nullptr, 0);
				ReleaseDC(nullptr, SplashScreenDC);
				GD_ASSERT(SplashScreen->SplashScreenBitmap != nullptr, "Error: 'WindowsSplashScreen': Failed to create 'SplashScreen->SplashScreenBitmap'.");

				UINT SplashScreenPixelStride = 0;
				SplashScreenResult = UIntMult(SplashScreenWidth, sizeof(DWORD), &SplashScreenPixelStride);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': multiplication result is out of UINT bounds.");

				UINT SplashScreenImageSize = 0;
				SplashScreenResult = UIntMult(SplashScreenHeight, SplashScreenPixelStride, &SplashScreenImageSize);
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': multiplication result is out of UINT bounds.");

				SplashScreenResult = SplashScreenRenderedBitmapSource->CopyPixels(nullptr, SplashScreenPixelStride, SplashScreenImageSize, reinterpret_cast<BYTE*>(SplashScreenData));
				GD_ASSERT(SUCCEEDED(SplashScreenResult), "Error: 'WindowsSplashScreen': 'SplashScreenRenderedBitmapSource->CopyPixels' failed.");

				DeleteObject(SplashScreenRenderedBitmapSource);
				DeleteObject(SplashScreenOriginalBitmapSource);
				DeleteObject(SplashScreenBitmapFrame);
				DeleteObject(SplashScreenBitmapConverter);
				DeleteObject(SplashScreenBitmapDecoder);
				DeleteObject(SplashScreenImagingFactory);
			}

			BITMAP SplashScreenBitmapInfo = { 0 };
			GetObject(SplashScreen->SplashScreenBitmap, sizeof(SplashScreenBitmapInfo), &SplashScreenBitmapInfo);

			// Loading Splash Screen fonts..
			{
				auto const SplashScreenDefaultFont = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
				LOGFONT SplashScreenSomeFont = { 0 };
				GetObjectA(SplashScreenDefaultFont, sizeof(SplashScreenSomeFont), &SplashScreenSomeFont);
				StringCchCopyA(SplashScreenSomeFont.lfFaceName, LF_FACESIZE, "Consolas");
				SplashScreenSomeFont.lfQuality = ANTIALIASED_QUALITY;
				{
					SplashScreenSomeFont.lfHeight = 12;
					SplashScreen->SplashScreenSmallTextFont = CreateFontIndirect(&SplashScreenSomeFont);
					if (SplashScreen->SplashScreenSmallTextFont == nullptr)
					{
						SplashScreen->SplashScreenSmallTextFont = SplashScreenDefaultFont;
					}
				}
				{
					SplashScreenSomeFont.lfHeight = 16;
					SplashScreen->SplashScreenNormalTextFont = CreateFontIndirect(&SplashScreenSomeFont);
					if (SplashScreen->SplashScreenNormalTextFont == nullptr)
					{
						SplashScreen->SplashScreenNormalTextFont = SplashScreenDefaultFont;
					}
				}
				{
					SplashScreenSomeFont.lfHeight = 18;
					SplashScreenSomeFont.lfWeight = FW_BOLD;
					SplashScreen->SplashScreenLargeTextFont = CreateFontIndirect(&SplashScreenSomeFont);
					if (SplashScreen->SplashScreenLargeTextFont == nullptr)
					{
						SplashScreen->SplashScreenLargeTextFont = SplashScreenDefaultFont;
					}
				}
			}

			// Setting up rectangles and etc..
			{
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionApplicationName].left	= 3;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionApplicationName].top	= SplashScreenBitmapInfo.bmHeight - 50;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionApplicationName].right	= SplashScreenBitmapInfo.bmWidth  - 3;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionApplicationName].bottom	= SplashScreenBitmapInfo.bmHeight - 35;

				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionStartupProgress].left	= 3;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionStartupProgress].top	= SplashScreenBitmapInfo.bmHeight - 30 - 2;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionStartupProgress].right	= SplashScreenBitmapInfo.bmWidth  - 3;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionStartupProgress].bottom	= SplashScreenBitmapInfo.bmHeight - 15;

				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionCopyrightInfo].left		= 3;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionCopyrightInfo].top		= SplashScreenBitmapInfo.bmHeight - 15 - 2;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionCopyrightInfo].right	= SplashScreenBitmapInfo.bmWidth  - 3;
				SplashScreen->SplashScreenRegionRects[SplashScreenTextRegion::RegionCopyrightInfo].bottom	= SplashScreenBitmapInfo.bmHeight - 2;
			}

			// Creating Splash screen window.
			{
				LONG const SplashScreenWindowBorderWidth	= GetSystemMetrics(SM_CXBORDER);
				LONG const SplashScreenWindowBorderHeight	= GetSystemMetrics(SM_CYBORDER);
				LONG const SplashScreenWindowWidth			= SplashScreenBitmapInfo.bmWidth + SplashScreenWindowBorderWidth;
				LONG const SplashScreenWindowHeight			= SplashScreenBitmapInfo.bmHeight + SplashScreenWindowBorderHeight;
				LONG const SplashScreenLeft					= (GetSystemMetrics(SM_CXSCREEN) - SplashScreenWindowWidth) / 2;
				LONG const SplashScreenTop					= (GetSystemMetrics(SM_CYSCREEN) - SplashScreenWindowHeight) / 2;

				SplashScreen->SplashScreenWindow = CreateWindowExA(WS_EX_TOOLWINDOW
					, WindowsSplashScreen::SplashScreenClassName, "GoddamnEnging Splash Screen"
					, WS_BORDER | WS_POPUP
					, SplashScreenLeft, SplashScreenTop, SplashScreenWindowWidth, SplashScreenWindowHeight
					, nullptr, nullptr, GetApplicationHInstance()
					, SplashScreen
					);
				GD_ASSERT(SplashScreen->SplashScreenWindow != nullptr, "Error: 'WindowsSplashScreen': Failed to create 'SplashScreenWindow'.");

				SetWindowTextA(SplashScreen->SplashScreenWindow, SplashScreen->SplashScreenRegionTexts[SplashScreenTextRegion::RegionApplicationName].CStr());
				ShowWindow(SplashScreen->SplashScreenWindow, SW_SHOW);
			}
		}

		// Splash screen window main loop..
		{
			MSG SplashScreenOSMessage = { 0 };
			while (true)
			{
				if (PeekMessageA(&SplashScreenOSMessage, SplashScreen->SplashScreenWindow, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&SplashScreenOSMessage);
					DispatchMessage(&SplashScreenOSMessage);
					if (SplashScreenOSMessage.message == WM_QUIT)
					{
						break;
					}

					Sleep(1000 / 2);
				}
			}
		}

		DeleteObject(SplashScreen->SplashScreenBitmap);
		UnregisterClassA(WindowsSplashScreen::SplashScreenClassName, GetApplicationHInstance());
		return 0;
	}

	//! Splash screen thread routine.
	GDINT LRESULT WINAPI WindowsSplashScreen::SplashScreenWindowProc(HWND const SplashScreenWindowHandle, UINT const SplashScreenMessage, WPARAM const SplashScreenWParam, LPARAM const SplashScreenLParam)
	{
		static WindowsSplashScreen* SplashScreen = nullptr;
		switch (SplashScreenMessage)
		{
			case WM_CREATE:
			{
				GD_DEBUG_ASSERT(SplashScreen == nullptr, "Error: 'SplashScreenWindowProc' was already initialized.");
				CREATESTRUCTA const* const SplashScreenWindowData = reinterpret_cast<CREATESTRUCTA const*>(SplashScreenLParam);
				SplashScreen = reinterpret_cast<WindowsSplashScreen*>(SplashScreenWindowData->lpCreateParams);
			} return 0;

			case WM_DESTROY:
			{
				SplashScreen = nullptr;
				PostQuitMessage(0);
			} return 0;

			case WM_PAINT:
			{
				GD_DEBUG_ASSERT(SplashScreen != nullptr, "Error: 'SplashScreenWindowProc' was not initialized.");
				
				PAINTSTRUCT SplashScreenPaintStruct = { 0 };
				auto const SplashScreenDC = BeginPaint(SplashScreenWindowHandle, &SplashScreenPaintStruct);
				DrawStateA(SplashScreenDC, DSS_NORMAL, nullptr, reinterpret_cast<LPARAM>(SplashScreen->SplashScreenBitmap), 0, 0, 0, 0, 0, DST_BITMAP);
				{
					ScopedLock SplashScreenLock(&SplashScreen->SplashScreenSyncObject);
					for (SizeTp SplashRegionCnt = 0; SplashRegionCnt < SplashScreenTextRegion::RegionsCount; ++SplashRegionCnt)
					{
						SplashScreenTextRegionTp const SplashRegion = static_cast<SplashScreenTextRegionTp>(SplashRegionCnt);
						String const& SplashRegionText = SplashScreen->SplashScreenRegionTexts[SplashRegionCnt];
						RECT   const& SplashRegionRect = SplashScreen->SplashScreenRegionRects[SplashRegionCnt];
						if (!SplashRegionText.IsEmpty())
						{
							switch (SplashRegion)
							{
								case SplashScreenTextRegion::RegionApplicationName:
									SelectObject(SplashScreenDC, SplashScreen->SplashScreenLargeTextFont);
									SetTextColor(SplashScreenDC, RGB(175, 175, 175));
									break;
								case SplashScreenTextRegion::RegionStartupProgress:
									SelectObject(SplashScreenDC, SplashScreen->SplashScreenNormalTextFont);
									SetTextColor(SplashScreenDC, RGB(150, 150, 150));
									break;
								default:
								case SplashScreenTextRegion::RegionCopyrightInfo:
									SelectObject(SplashScreenDC, SplashScreen->SplashScreenSmallTextFont);
									SetTextColor(SplashScreenDC, RGB(113, 113, 113));
									break;
							}

							SetTextAlign(SplashScreenDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);
							SetBkColor(SplashScreenDC, 0);
							SetBkMode(SplashScreenDC, TRANSPARENT);
							TextOutA(SplashScreenDC, SplashRegionRect.left, SplashRegionRect.top, SplashRegionText.CStr(), static_cast<int>(SplashRegionText.GetLength()));
						}
					}
				}

				EndPaint(SplashScreenWindowHandle, &SplashScreenPaintStruct);
			} return 0;
		}
		return DefWindowProcA(SplashScreenWindowHandle, SplashScreenMessage, SplashScreenWParam, SplashScreenLParam);
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
