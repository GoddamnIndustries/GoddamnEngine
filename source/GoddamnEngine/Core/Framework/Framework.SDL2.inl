//////////////////////////////////////////////////////////////////////////
/// Framework.SDL2.inl - Framework SDL2 implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

GD_NAMESPACE_BEGIN

	struct Window::WindowImpl final { SDL_Window* WindowHandle = nullptr; };

	Window::Window() : Impl(new Window::WindowImpl())
	{
		for (auto& EventListener : self->EventListeners)
			EventListener = ([](chandle const Param) {});

		int const Result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
		GD_ASSERT(Result == 0, "Failed to initialize SDL2 library: \"%s\" error.", SDL_GetError());
		self->Impl->WindowHandle = SDL_CreateWindow(
			&Window::DefaultWindowTitle[0],
			static_cast<int>(Window::DefaultWindowRectangle.Left ), static_cast<int>(Window::DefaultWindowRectangle.Top),
			static_cast<int>(Window::DefaultWindowRectangle.Width),	static_cast<int>(Window::DefaultWindowRectangle.Height),
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

		GD_ASSERT(self->Impl->WindowHandle != nullptr, "Failed to create SDL2 Window: \"%s\" error.", SDL_GetError());
	}

	Window::~Window()
	{
		SDL_DestroyWindow(self->Impl->WindowHandle);
		SDL_Quit();
		delete self->Impl;
	}

	void Window::WindowUpdate() const
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event) != 0)
			switch (Event.type)
		{
			case SDL_QUIT: 
				self->EventListeners[GD_WINDOW_EVENT_ON_WINDOW_CLOSED](nullptr);
				break;
		}
	}

	handle Window::GetWindowNativeHandle() const
	{
		SDL_SysWMinfo WindowInformation;
		SDL_VERSION(&WindowInformation.version);
		SDL_bool const Result = SDL_GetWindowWMInfo(self->Impl->WindowHandle, &WindowInformation);
		GD_ASSERT(Result == SDL_TRUE, "Failed to initialize SDL2 library: \"%s\" error.", SDL_GetError());

		return &WindowInformation.info;
	}

	Rectangle Window::GetWindowRectangle() const
	{
		SDL_Rect SDLRectangle;
		SDL_GetWindowPosition(self->Impl->WindowHandle, &SDLRectangle.x, &SDLRectangle.w);
		SDL_GetWindowSize    (self->Impl->WindowHandle, &SDLRectangle.y, &SDLRectangle.h);

		return Rectangle(Float32(SDLRectangle.x), Float32(SDLRectangle.y), Float32(SDLRectangle.w), Float32(SDLRectangle.h));
	}

	void Window::SetWindowRectangle(Rectangle const& NewRectangle)
	{
		SDL_SetWindowPosition(self->Impl->WindowHandle, static_cast<int>(NewRectangle.Left),  static_cast<int>(NewRectangle.Top)   );
		SDL_SetWindowPosition(self->Impl->WindowHandle, static_cast<int>(NewRectangle.Width), static_cast<int>(NewRectangle.Height));
	}

	bool Window::GetWindowIsFullscreen() const
	{
		return ((SDL_GetWindowFlags(self->Impl->WindowHandle) & SDL_WINDOW_FULLSCREEN) != 0);
	}

	void Window::SetWindowIsFullscreen(bool const IsFullscreen)
	{
		int const Result = SDL_SetWindowFullscreen(self->Impl->WindowHandle, IsFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		GD_ASSERT(Result == 0, "Failed to change fullscreen mode: \"%s\" error.", SDL_GetError());
	}

	String Window::GetWindowTitle() const
	{
		return SDL_GetWindowTitle(self->Impl->WindowHandle);
	}

	void Window::SetWindowTitle(String const& NewTitle)
	{
		SDL_SetWindowTitle(self->Impl->WindowHandle, &NewTitle[0]);
	}

GD_NAMESPACE_END
