//////////////////////////////////////////////////////////////////////////
/// Framework.SDL2.inl - Framework SDL2 implementation.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <SDL2/SDL.h>

GD_NAMESPACE_BEGIN

	struct Window::WindowImpl final
	{
		SDL_Window* WindowHandle = nullptr;
	};	// struct WindowImpl

	Window::Window() : Impl(new Window::WindowImpl())
	{
		SDL_Init(SDL_INIT_VIDEO);
		self->Impl->WindowHandle = SDL_CreateWindow(&Window::DefaultWindowTitle[0],
			static_cast<int>(Window::DefaultWindowRectangle.Left),
			static_cast<int>(Window::DefaultWindowRectangle.Top),
			static_cast<int>(Window::DefaultWindowRectangle.Width),
			static_cast<int>(Window::DefaultWindowRectangle.Height),
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

		GD_ASSERT(self->Impl->WindowHandle != nullptr, "Failed to create SDL2 Window");
	}

	Window::~Window()
	{
		SDL_DestroyWindow(self->Impl->WindowHandle);
		delete self->Impl;
	}

	Rectangle Window::GetRectangle() const
	{
		Vector4t<int> SDLRectangle;
		SDL_GetWindowPosition(self->Impl->WindowHandle, &SDLRectangle.Left,  &SDLRectangle.Top   );
		SDL_GetWindowSize    (self->Impl->WindowHandle, &SDLRectangle.Width, &SDLRectangle.Height);

		return Rectangle(Float32(SDLRectangle.Left), Float32(SDLRectangle.Top), Float32(SDLRectangle.Width), Float32(SDLRectangle.Height));
	}

	void Window::SetRectangle(Rectangle const& NewRectangle)
	{
		SDL_SetWindowPosition(self->Impl->WindowHandle, static_cast<int>(NewRectangle.Left),  static_cast<int>(NewRectangle.Top)   );
		SDL_SetWindowPosition(self->Impl->WindowHandle, static_cast<int>(NewRectangle.Width), static_cast<int>(NewRectangle.Height));
	}

	bool Window::GetIsFullscreen() const
	{
		return ((SDL_GetWindowFlags(self->Impl->WindowHandle) & SDL_WINDOW_FULLSCREEN) != 0);
	}

	void Window::SetIsFullscreen(bool const IsFullscreen)
	{
		int const Result = SDL_SetWindowFullscreen(self->Impl->WindowHandle, IsFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		GD_ASSERT(Result == 0, "Failed to change fullscreen mode.");
	}

GD_NAMESPACE_END
