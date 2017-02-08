// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLWindows.cpp
 * File contains Implementations for Windows-specific code of the OpenGL 4.3 Implementation 
 * of the graphics interface.
 */
#include <GoddamnEngine/GraphicsOpenGL/GraphicsOpenGLWindows.h>
#if GD_PLATFORM_WINDOWS

//#include <GoddamnEngine/Core/OutputDevice/OutputDevice.h>
#define GD_DLOG_CAT "GFX device (Windows@OpenGL)"

//! @todo Move this as a dependency.
#pragma comment (lib, "opengl32.lib")
#include <Windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step, before all other interfaces
	//! are initialized.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWindows::OnRuntimePreInitialize()
	{
		IResult const _BaseResult = IGraphicsPlatform::OnRuntimePreInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;

		// Retrieving information about the list of supported screen resolutions.
		{
			DEVMODE HGLRCTestCanvasMode;
			HGLRCTestCanvasMode.dmSize = sizeof(HGLRCTestCanvasMode);
			for (DWORD _Cnt = 0; EnumDisplaySettings(nullptr, _Cnt, &HGLRCTestCanvasMode) != GD_FALSE; ++_Cnt)
			{
				// Testing current resolution..
				if (ChangeDisplaySettings(&HGLRCTestCanvasMode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
				{
					m_GfxResolutionsList.InsertAt(0, { static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsWidth), static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsHeight), 30, 1 });
					m_GfxResolutionsList.InsertAt(0, { static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsWidth), static_cast<UInt32>(HGLRCTestCanvasMode.dmPelsHeight), 60, 1 });
				}
			}
		}

		//! @todo Load default parameters.
		m_GfxCanvasMode = IGRAPHICS_OUTPUT_MODE_WINDOWED;	// Easier for debugging purposes.
		m_GfxResolutionSelected = &m_GfxResolutionsList.GetData()[8];

		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global initialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWindows::OnRuntimeInitialize()
	{
	//	_CheckNotInitialized();
	//	ConsoleDevice->Log(GD_DLOG_CAT ": going to initialize graphics devices...");

		IResult const _BaseResult = IGraphicsPlatform::OnRuntimeInitialize();
		if (IFailed(_BaseResult))
			return _BaseResult;

		// Loading the device context of the window..
		HGLRCDeviceContext = GetDC(m_HwndMain);
		GD_VERIFY(HGLRCDeviceContext != nullptr
			, "'GetDC' error: failed to retrieve the device context of the main window.");

		// Choosing the pixel format..
		PIXELFORMATDESCRIPTOR glPixelFormatDescriptor = {};
		glPixelFormatDescriptor.nSize      = sizeof(glPixelFormatDescriptor);
		glPixelFormatDescriptor.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		glPixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
		glPixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
		glPixelFormatDescriptor.cColorBits = 32;
		glPixelFormatDescriptor.cDepthBits = 32;
		int const _OglPixelFormat = ChoosePixelFormat(HGLRCDeviceContext, &glPixelFormatDescriptor);
		SetPixelFormat(HGLRCDeviceContext, _OglPixelFormat, &glPixelFormatDescriptor);
		GD_VERIFY(_OglPixelFormat != 0
			, "'ChoosePixelFormat' error: failed to select correct pixel format.");

		// Creating temporary OpenGL 2.1 context..
		// We need it to load a function, that helps us to create an advanced context with
		// the support of the OpenGL 3.0 and above.
		HGLRC const HGLRCContextTemp = wglCreateContext(HGLRCDeviceContext);
		wglMakeCurrent(HGLRCDeviceContext, HGLRCContextTemp);
		auto const wglCreateContextAttribsARB = reinterpret_cast<HGLRC(*)(HDC hDC, HGLRC hGLRC
			, int const* Attributes)>(wglGetProcAddress("wglCreateContextAttribsARB"));
		GD_VERIFY(wglCreateContextAttribsARB != nullptr
			, "'wglGetProcAddress' error: failed to retrieve the address of the 'wglCreateContextAttribsARB' routine.");
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(HGLRCContextTemp);

		// Creating the full-featured OpenGL 4.3++ context..
		int static const HGLRCContextVersionAttributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
#if GD_DEBUG
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
#endif	// if GD_DEBUG
			0
		};
		HGLRCContext = wglCreateContextAttribsARB(HGLRCDeviceContext, nullptr, HGLRCContextVersionAttributes);
		wglMakeCurrent(HGLRCDeviceContext, HGLRCContext);
		
		if (WGLEW_EXT_swap_control)
			wglSwapIntervalEXT(0);

	//	ConsoleDevice->Log(GD_DLOG_CAT ": ... initialized.");
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called on the global deinitialization step.
	//! @returns Non-negative value if the operation succeeded.
	GDAPI IResult IGraphicsOpenGLWindows::OnRuntimeDeinitialize()
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(HGLRCContext);
		ReleaseDC(m_HwndMain, HGLRCDeviceContext);

		return IGraphicsPlatform::OnRuntimeDeinitialize();
	}

	// ------------------------------------------------------------------------------------------
	//! Function would be called once per frame, after all other runtime 
	//! interfaces are deinitialized.
	GDAPI void IGraphicsOpenGLWindows::OnRuntimePostUpdate()
	{
		//! @todo Uncomment this.
		/*IGraphicsPlatform::OnRuntimePostUpdate();
		SwapBuffers(HGLRCDeviceContext);*/
	}

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
