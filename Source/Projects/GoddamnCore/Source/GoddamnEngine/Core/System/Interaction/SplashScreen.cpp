// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/System/Interaction/SplashScreen.cpp
 * Cross-platform splash screens.
 */
#include <GoddamnEngine/Core/System/Interaction/SplashScreen.h>
#include <GoddamnEngine/Core/System/IO/Paths.h>

#include <gl/GL.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Initializes a new splash screen.
	 */
	GDAPI SplashScreenGeneric::SplashScreenGeneric()
		: Window(WindowMode::Windowed, WindowFlags::Borderless | WindowFlags::Centered | WindowFlags::WithOpenGL, 512, 256)
	{
		m_SplashScreenPath = Paths::Combine(Paths::GetEngineContentDirectory(), L"SplashScreen.bmp");
		m_SplashTexts[static_cast<SizeTp>(SplashScreenText::ApplicationName)] = String::Format("Goddamn Engine Runtime %d.%d.%d", GD_ENGINE_VERSION_MAJOR, GD_ENGINE_VERSION_MINOR, GD_ENGINE_VERSION_PATCH);
		m_SplashTexts[static_cast<SizeTp>(SplashScreenText::StartupProgress)] = "Initializing...";
		m_SplashTexts[static_cast<SizeTp>(SplashScreenText::CopyrightInfo)] = "Copyright (C) Goddamn Industries 2016. All Rights Reserved.";

		glEnable(GL_TEXTURE_2D);

		glLoadIdentity();
		glTranslatef(-1.0f, 1.0f, 0.0f);
		glScalef(1.0f * m_WindowWidth, -1.0f * m_WindowHeight, 1.0f);
	}

	/*!
	 * Callback being called to update the splash screen window.
	 */
	GDAPI void SplashScreenGeneric::OnUpdate()
	{
		Window::OnUpdate();

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2i(  0,   0);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(512,   0);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(512, 256);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(  0, 256);
		glEnd();
	}

GD_NAMESPACE_END
