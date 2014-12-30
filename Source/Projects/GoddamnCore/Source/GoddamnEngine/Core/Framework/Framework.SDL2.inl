/// ==========================================================================================
/// Framework.SDL2.inl - Framework SDL2 implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define GD_SAFE_INVOKE(Event, Param) \
	do { \
		if (this->EventListeners[static_cast<size_t>(Event)] != nullptr) { \
			this->EventListeners[static_cast<size_t>(Event)](Param); \
		} \
	} while (false)

GD_NAMESPACE_BEGIN

	struct Window::WindowImpl final { SDL_Window* WindowHandle = nullptr; };

	class SDL2WindowException : public WindowException
	{
	public /*Public API*/:
		GDINL explicit SDL2WindowException(Str const ErrorMessage, ...) : WindowException(ErrorMessage) { }
		GDINL virtual ~SDL2WindowException() { }
	};	// class WindowException

	Window::Window() : Impl(new Window::WindowImpl())
	{
		for (auto& EventListener : this->EventListeners) {
			EventListener = ([](chandle const Param) {});
		}

		int const Result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
		if (Result != 0) {
			throw SDL2WindowException("Failed to initialize SDL2 library: \"%s\" error.", SDL_GetError());
		}

		this->Impl->WindowHandle = SDL_CreateWindow(
			Window::DefaultWindowTitle,
			static_cast<int>(Window::DefaultWindowRectangle.Left ), static_cast<int>(Window::DefaultWindowRectangle.Top),
			static_cast<int>(Window::DefaultWindowRectangle.Width),	static_cast<int>(Window::DefaultWindowRectangle.Height),
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
		if (this->Impl->WindowHandle == nullptr) {
			throw SDL2WindowException("Failed to create SDL2 Window: \"%s\" error.", SDL_GetError());
		}
	}

	Window::~Window()
	{
		SDL_DestroyWindow(this->Impl->WindowHandle);
		SDL_Quit();
		delete this->Impl;
	}

	void Window::WindowUpdate() const
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event) != 0) {
			switch (Event.type)	{
				case SDL_QUIT: {
					GD_SAFE_INVOKE(WindowEvent::OnWindowClosed, nullptr);
				} break;
				default: {
				//	GD_NOT_IMPLEMENTED();
				} break;
			}
		}
	}

	handle Window::GetWindowNativeHandle() const
	{
		SDL_SysWMinfo WindowInformation;
		SDL_VERSION(&WindowInformation.version);
		SDL_bool const Result = SDL_GetWindowWMInfo(this->Impl->WindowHandle, &WindowInformation);
		if (Result != SDL_TRUE) {
			throw SDL2WindowException("Failed to initialize SDL2 library: \"%s\" error.", SDL_GetError());
		} else {
			return static_cast<handle>(&WindowInformation.info);
		}
	}

	Rectangle Window::GetWindowRectangle() const
	{
		SDL_Rect SDLRectangle;
		SDL_GetWindowPosition(this->Impl->WindowHandle, &SDLRectangle.x, &SDLRectangle.w);
		SDL_GetWindowSize (this->Impl->WindowHandle, &SDLRectangle.y, &SDLRectangle.h);

		return Rectangle(Float32(SDLRectangle.x), Float32(SDLRectangle.y), Float32(SDLRectangle.w), Float32(SDLRectangle.h));
	}

	void Window::SetWindowRectangle(Rectangle const& NewRectangle)
	{
		SDL_SetWindowPosition(this->Impl->WindowHandle, static_cast<int>(NewRectangle.Left),  static_cast<int>(NewRectangle.Top)   );
		SDL_SetWindowPosition(this->Impl->WindowHandle, static_cast<int>(NewRectangle.Width), static_cast<int>(NewRectangle.Height));
	}

	bool Window::GetWindowIsFullscreen() const
	{
		return ((SDL_GetWindowFlags(this->Impl->WindowHandle) & SDL_WINDOW_FULLSCREEN) != 0);
	}

	void Window::SetWindowIsFullscreen(bool const IsFullscreen)
	{
		int const Result = SDL_SetWindowFullscreen(this->Impl->WindowHandle, IsFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		GD_ASSERT(Result == 0, "Failed to change fullscreen mode: \"%s\" error.", SDL_GetError());
	}

	String Window::GetWindowTitle() const
	{
		return SDL_GetWindowTitle(this->Impl->WindowHandle);
	}

	void Window::SetWindowTitle(String const& NewTitle)
	{
		SDL_SetWindowTitle(this->Impl->WindowHandle, &NewTitle[0]);
	}

GD_NAMESPACE_END
