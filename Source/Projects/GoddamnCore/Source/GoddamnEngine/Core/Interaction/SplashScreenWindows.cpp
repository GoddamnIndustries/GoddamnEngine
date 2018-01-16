// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Interaction/SplashScreenWindows.cpp
 * Windows splash screens.
 */
#include <GoddamnEngine/Core/Interaction/SplashScreen.h>
#if GD_PLATFORM_WINDOWS

#include <atlbase.h>
#include <StrSafe.h>
#include <WinCodec.h>
#pragma comment(lib, "windowscodecs.lib")

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	// SplashScreenWindows class.
	// ------------------------------------------------------------------------------------------

	GDAPI SplashScreenWindows::SplashScreenWindows()
		: m_SplashBitmap(nullptr), m_SplashWindow(nullptr), m_SplashGuard(nullptr)
		, m_SplashIsShown(false), m_SplashSmallTextFont(nullptr), m_SplashNormalTextFont(nullptr), m_SplashLargeTextFont(nullptr)
	{
	}

	GDAPI SplashScreenWindows::~SplashScreenWindows()
	{
		DeleteObject(m_SplashBitmap);
		CloseHandle(m_SplashSmallTextFont);
		CloseHandle(m_SplashNormalTextFont);
		CloseHandle(m_SplashLargeTextFont);
		DestroyWindow(m_SplashWindow);
		DestroyWindow(m_SplashGuard);
		
	}

	/*! 
	 * Shows or hides a splash screen.
	 * @param doShow Whether to show or hide splash screen.
	 */
	GDAPI void SplashScreenWindows::ShowSplash(bool const doShow)
	{
		if (doShow)
		{
			if (!m_SplashIsShown)
			{
				m_SplashIsShown = true;
				::ShowWindow(m_SplashWindow, SW_SHOW);
			}
		}
		else
		{
			if (m_SplashIsShown)
			{
				m_SplashIsShown = false;
				::ShowWindow(m_SplashWindow, SW_HIDE);
			}
		}
	}

	/*! 
	 * Returns true if splash is shown.
	 */
	GDAPI bool SplashScreenWindows::IsSplashShown() const
	{
		return m_SplashIsShown;
	}		

	/*! 
	 * Sets text that is displayed in some region of a splash screen.
	 * @param splashTextRegion Region to which set new text.
	 * @param splashText Text to set.
	 */
	GDAPI void SplashScreenWindows::SetSplashText(SplashScreenText const splashTextRegion, String const& splashText)
	{
		auto splashTextWasUpdated = false;
		{
			ScopedCriticalSection splashLock(m_SplashLock);
			if (splashText != m_SplashTexts[static_cast<SizeTp>(splashTextRegion)])
			{
				splashTextWasUpdated = true;
				m_SplashTexts[static_cast<SizeTp>(splashTextRegion)] = splashText;
			}
		}
		if (splashTextWasUpdated)
		{
			InvalidateRect(m_SplashWindow, &m_SplashRects[static_cast<SizeTp>(splashTextRegion)], false);
		}
	}

#if defined(GD_NAMESPACE)
	CHAR const g_SplashScreenClassName[] = GD_GLUE(GD_STRIGIFY(GD_NAMESPACE), "_SplashScreenWindows");
#else	// if defined(GD_NAMESPACE)
	CHAR const g_SplashScreenClassName[] = "Goddamn_SplashScreenWindows";
#endif	// if defined(GD_NAMESPACE)

	/*!
	 * Initializes the splash screen.
	 * Would called from separate thread.
	 */
	GDAPI void SplashScreenWindows::OnInitialize()
	{
		ScopedCriticalSection splashLock(m_SplashLock);

		{	// Registering splash window class..
			WNDCLASSA splashClass		= { 0 };
			splashClass.style			= CS_HREDRAW | CS_VREDRAW;
			splashClass.cbClsExtra		= 0;
			splashClass.cbWndExtra		= 0;
			splashClass.hInstance		= GetModuleHandle(nullptr);
			splashClass.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
			splashClass.hCursor			= LoadCursor(nullptr, IDC_ARROW);
			splashClass.hbrBackground	= reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
			splashClass.lpszMenuName	= nullptr;
			splashClass.lpszClassName	= g_SplashScreenClassName;
			splashClass.lpfnWndProc		= DefWindowProc;

			auto const result = RegisterClass(&splashClass);
			GD_VERIFY(result != 0, "'RegisterClass' function has failed.");
		}

		{	// Loading splash bitmap..
			auto result = CoInitialize(nullptr);

			// Creating the imaging factory.
			CComPtr<IWICImagingFactory> splashImagingFactory = nullptr;
			if (SUCCEEDED(result))
			{
				result = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&splashImagingFactory));
			}

			// Loading the image.
			CComPtr<IWICBitmapDecoder> splashBitmapDecoder = nullptr;
			if (SUCCEEDED(result))
			{
				result = splashImagingFactory->CreateDecoderFromFilename(m_SplashScreenPath.CStr(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &splashBitmapDecoder);
			}

			CComPtr<IWICBitmapFrameDecode> splashBitmapFrame = nullptr;
			if (SUCCEEDED(result))
			{
				result = splashBitmapDecoder->GetFrame(0, &splashBitmapFrame);
			}

			// Converting to the required pixel format.
			CComPtr<IWICBitmapSource> splashOriginalBitmapSource = nullptr;
			if (SUCCEEDED(result))
			{
				result = splashBitmapFrame->QueryInterface(IID_IWICBitmapSource, reinterpret_cast<void**>(&splashOriginalBitmapSource));
			}

			CComPtr<IWICBitmapSource> splashRenderedBitmapSource = nullptr;
			if (SUCCEEDED(result))
			{
				CComPtr<IWICFormatConverter> splashBitmapConverter = nullptr;
				result = splashImagingFactory->CreateFormatConverter(&splashBitmapConverter);

				if (SUCCEEDED(result))
				{
					result = splashBitmapConverter->Initialize(splashBitmapFrame, GUID_WICPixelFormat32bppBGR, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
					if (SUCCEEDED(result))
					{
						result = splashBitmapConverter->QueryInterface(IID_PPV_ARGS(&splashRenderedBitmapSource));
					}
				}
			}

			// Loading the bitmap to Windows API.
			if (SUCCEEDED(result))
			{
				WICPixelFormatGUID splashPixelFormat = { 0 };
				result = splashRenderedBitmapSource->GetPixelFormat(&splashPixelFormat);
				if (SUCCEEDED(result))
				{
					result = splashPixelFormat == GUID_WICPixelFormat32bppBGR ? S_OK : E_FAIL;
				}

				UINT splashWidth = 0;
				UINT splashHeight = 0;
				if (SUCCEEDED(result))
				{
					result = splashRenderedBitmapSource->GetSize(&splashWidth, &splashHeight);
				}

				LPVOID splashData = nullptr;
				if (SUCCEEDED(result))
				{
					auto const splashDC = GetDC(nullptr);
					result = splashDC != nullptr ? S_OK : E_FAIL;

					if (SUCCEEDED(result))
					{
						BITMAPINFO splashBitmapInfo = {};
						splashBitmapInfo.bmiHeader.biSize = sizeof splashBitmapInfo.bmiHeader;
						splashBitmapInfo.bmiHeader.biWidth = static_cast<LONG>(splashWidth);
						splashBitmapInfo.bmiHeader.biHeight = static_cast<LONG>(splashHeight) * -1;
						splashBitmapInfo.bmiHeader.biPlanes = 1;
						splashBitmapInfo.bmiHeader.biBitCount = 32;
						splashBitmapInfo.bmiHeader.biCompression = BI_RGB;
						m_SplashBitmap = CreateDIBSection(splashDC, &splashBitmapInfo, DIB_RGB_COLORS, &splashData, nullptr, 0);
						ReleaseDC(nullptr, splashDC);
						result = m_SplashBitmap != nullptr ? S_OK : E_FAIL;
					}
				}

				UINT splashPixelStride = 0;
				if (SUCCEEDED(result))
				{
					result = UIntMult(splashWidth, sizeof(DWORD), &splashPixelStride);
				}

				UINT splashImageSize = 0;
				if (SUCCEEDED(result))
				{
					result = UIntMult(splashHeight, splashPixelStride, &splashImageSize);
				}

				if (SUCCEEDED(result) && splashRenderedBitmapSource != nullptr)
				{
					result = splashRenderedBitmapSource->CopyPixels(nullptr, splashPixelStride, splashImageSize, reinterpret_cast<BYTE*>(splashData));
				}
			}

			if (FAILED(result))
			{
				DeleteObject(m_SplashBitmap);
				m_SplashBitmap = nullptr;
			}
		}

		// Retrieving information for the loaded bitmap.
		BITMAP splashBitmapInfo = {};
		if (m_SplashBitmap != nullptr)
		{
			GetObjectA(m_SplashBitmap, sizeof splashBitmapInfo, &splashBitmapInfo);
		}
		else
		{
			splashBitmapInfo.bmWidth = 512;
			splashBitmapInfo.bmHeight = 256;
		}

		{	// Loading fonts..
			auto const splashDefaultFont = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
			GD_VERIFY(splashDefaultFont != nullptr, "'GetStockObject(DEFAULT_GUI_FONT)' function has failed.");

			LOGFONT splashSomeFont = { 0 };
			GetObjectA(splashDefaultFont, sizeof splashSomeFont, &splashSomeFont);
			if (SUCCEEDED(StringCchCopyA(splashSomeFont.lfFaceName, LF_FACESIZE, "Consolas")))
			{
				splashSomeFont.lfQuality = ANTIALIASED_QUALITY;
				{
					splashSomeFont.lfHeight = 12;
					m_SplashSmallTextFont = CreateFontIndirectA(&splashSomeFont);
					if (m_SplashSmallTextFont == nullptr)
					{
						m_SplashSmallTextFont = splashDefaultFont;
					}
				}
				{
					splashSomeFont.lfHeight = 16;
					m_SplashNormalTextFont = CreateFontIndirectA(&splashSomeFont);
					if (m_SplashNormalTextFont == nullptr)
					{
						m_SplashNormalTextFont = splashDefaultFont;
					}
				}
				{
					splashSomeFont.lfHeight = 18;
					splashSomeFont.lfWeight = FW_BLACK;
					m_SplashLargeTextFont = CreateFontIndirectA(&splashSomeFont);
					if (m_SplashLargeTextFont == nullptr)
					{
						m_SplashLargeTextFont = splashDefaultFont;
					}
				}
			}
			else
			{
				m_SplashSmallTextFont = splashDefaultFont;
				m_SplashNormalTextFont = splashDefaultFont;
				m_SplashLargeTextFont = splashDefaultFont;
			}
		}

		{	// Setting up rectangles and etc..
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::ApplicationName)].left	 = 3;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::ApplicationName)].top	 = splashBitmapInfo.bmHeight - 50;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::ApplicationName)].right	 = splashBitmapInfo.bmWidth  - 3;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::ApplicationName)].bottom = splashBitmapInfo.bmHeight - 35;

			m_SplashRects[static_cast<SizeTp>(SplashScreenText::StartupProgress)].left	 = 3;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::StartupProgress)].top	 = splashBitmapInfo.bmHeight - 30 - 4;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::StartupProgress)].right	 = splashBitmapInfo.bmWidth  - 3;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::StartupProgress)].bottom = splashBitmapInfo.bmHeight - 15;

			m_SplashRects[static_cast<SizeTp>(SplashScreenText::CopyrightInfo)].left	 = 3;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::CopyrightInfo)].top		 = splashBitmapInfo.bmHeight - 15 - 2;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::CopyrightInfo)].right	 = splashBitmapInfo.bmWidth  - 3;
			m_SplashRects[static_cast<SizeTp>(SplashScreenText::CopyrightInfo)].bottom	 = splashBitmapInfo.bmHeight - 2;
		}
			
		{	// Creating window..
			LONG const splashWindowBorderWidth	= GetSystemMetrics(SM_CXBORDER);
			LONG const splashWindowBorderHeight	= GetSystemMetrics(SM_CYBORDER);
			auto const splashWindowWidth		= splashBitmapInfo.bmWidth + splashWindowBorderWidth;
			auto const splashWindowHeight		= splashBitmapInfo.bmHeight + splashWindowBorderHeight;
			auto const splashLeft				= (GetSystemMetrics(SM_CXSCREEN) - splashWindowWidth) / 2;
			auto const splashTop				= (GetSystemMetrics(SM_CYSCREEN) - splashWindowHeight) / 2;

			m_SplashGuard = CreateWindowExA(0
				, "STATIC", "GoddamnEnging Splash Screen Guard"
				, 0
				, 0, 0, 0, 0
				, HWND_MESSAGE, nullptr, GetModuleHandle(nullptr)
				, nullptr
				);
			GD_VERIFY(m_SplashGuard != nullptr, "'CreateWindowEx' function has failed.");

			m_SplashWindow = CreateWindowExA(WS_EX_TOOLWINDOW | WS_EX_TOPMOST
				, g_SplashScreenClassName, "GoddamnEnging Splash Screen"
				, WS_BORDER | WS_POPUP
				, splashLeft, splashTop, splashWindowWidth, splashWindowHeight
				, nullptr, nullptr, GetModuleHandleA(nullptr)
				, this
				);
			GD_VERIFY(m_SplashWindow != nullptr, "'CreateWindowEx' function has failed.");

			SetWindowTextA(m_SplashWindow, m_SplashTexts[static_cast<SizeTp>(SplashScreenText::ApplicationName)].CStr());
			::ShowWindow(m_SplashWindow, SW_SHOW);
		}
	}

	/*!
	 * Updates and renders the splash screen.
	 * Would called from separate thread.
	 */
	// ReSharper disable once CppMemberFunctionMayBeConst
	GDAPI void SplashScreenWindows::OnUpdate()
	{
		MSG splashWindowMessage = { };
		while (PeekMessageA(&splashWindowMessage, m_SplashWindow, 0, 0, PM_REMOVE))
		{
			if (splashWindowMessage.message == WM_PAINT)
			{
				PAINTSTRUCT splashPaintStruct = { };
				auto const splashDC = BeginPaint(m_SplashWindow, &splashPaintStruct);
				
				// Rendering image..
				// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
				DrawStateA(splashDC, DSS_NORMAL, nullptr, reinterpret_cast<LPARAM>(m_SplashBitmap), 0, 0, 0, 0, 0, DST_BITMAP);
				// ReSharper restore CppZeroConstantCanBeReplacedWithNullptr

				{	// Rendering texts..
					ScopedCriticalSection splashLock(m_SplashLock);
					for (SizeTp splashRegion = 0; splashRegion < SplashScreenTextsCount; ++splashRegion)
					{
						auto const& splashText = m_SplashTexts[splashRegion];
						if (!splashText.IsEmpty())
						{
							auto const& splashRect = m_SplashRects[splashRegion];
							switch (static_cast<SplashScreenText>(splashRegion))
							{
								case SplashScreenText::ApplicationName:
									SelectObject(splashDC, m_SplashLargeTextFont);
									SetTextColor(splashDC, RGB(175, 175, 175));
									break;

								case SplashScreenText::StartupProgress:
									SelectObject(splashDC, m_SplashNormalTextFont);
									SetTextColor(splashDC, RGB(150, 150, 150));
									break;

								default:
									SelectObject(splashDC, m_SplashSmallTextFont);
									SetTextColor(splashDC, RGB(113, 113, 113));
									break;
							}

							SetTextAlign(splashDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);
							SetBkColor(splashDC, 0);
							SetBkMode(splashDC, TRANSPARENT);
							TextOut(splashDC, splashRect.left, splashRect.top, splashText.CStr(), static_cast<int>(splashText.GetLength()));
						}
					}
				}

				EndPaint(m_SplashWindow, &splashPaintStruct);
			}
			TranslateMessage(&splashWindowMessage);
			DispatchMessageA(&splashWindowMessage);
		}
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
