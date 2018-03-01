// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Application implementation.
 */
#include <GoddamnEngine/Core/Platform/PlatformApplication.h>
#include <GoddamnEngine/Core/Platform/PlatformAtomics.h>
#if GD_PLATFORM_WINDOWS_UWP

using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Native UWP application instance.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	ref class GD_PLATFORM_KERNEL WindowsUWPFrameworkView sealed : public IFrameworkView
	{
	private:
		AtomicBool m_IsRunning;
		IPlatformApplicationDelegate& m_AppDelegate;

	internal:
		GDINL WindowsUWPFrameworkView(IPlatformApplicationDelegate& appDelegate)
			: m_AppDelegate(appDelegate)
		{
		}

	private:

		// ------------------------------------------------------------------------------------------
		// Helper functions.
		// ------------------------------------------------------------------------------------------

		GDINT IPlatformKeyCode ConvertKeyCode(Windows::System::VirtualKey const key)
		{
			return (IPlatformKeyCode)key;
		}

		/*!
		 * Finds a window by it's system handle.
		 * 
		 * @param windowSystem Window system handle.
		 * @returns Found window or null pointer.
		 */
		GDINT SharedPtr<IPlatformWindow> FindWindowBySystemWindowHandle(CoreWindow^ windowSystem)
		{
			return nullptr;
		}

	public:

		// ------------------------------------------------------------------------------------------
		// Native event loop.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Used to initialize application instance.
		 * @param appView Application view instance.
		 */
		GDINT virtual void Initialize(CoreApplicationView^ appView) sealed
		{
			appView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &WindowsUWPFrameworkView::OnActivated);
			CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &WindowsUWPFrameworkView::OnSuspending);
			CoreApplication::Resuming += ref new EventHandler<Object^>(this, &WindowsUWPFrameworkView::OnResuming);
		}

		/*!
		 * Used to load or activate any external resource.
		 * @param entryPoint Name of the entry point for the activated type.
		 */
		GDINT virtual void Load(Platform::String^ entryPoint) sealed
		{
		}

		/*!
		 * Used to run application instance.
		 */
		GDINT virtual void Run()
		{
			m_IsRunning = true;
			while (m_IsRunning)
			{
				CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
				m_AppDelegate.OnAppUpdate();
			}
		}

		/*!
		 * Used to set current window for the app object's view.
		 * @param window The window to set.
		 */
		GDINT virtual void SetWindow(CoreWindow^ window)
		{
			window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &WindowsUWPFrameworkView::OnWindowClosed);

			// Mouse/Touch Screen/Tablet/.. events.
			window->PointerPressed 
				+= ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WindowsUWPFrameworkView::OnWindowPointerPressed);
			window->PointerReleased 
				+= ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WindowsUWPFrameworkView::OnWindowPointerReleased);

			// Keyboard events.
			window->KeyDown 
				+= ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &WindowsUWPFrameworkView::OnWindowKeyDown);
			window->KeyUp 
				+= ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &WindowsUWPFrameworkView::OnWindowKeyUp);
		}

		/*!
		 * Used to uninitialize application instance.
		 */
		GDINT virtual void Uninitialize()
		{
		}

		// ------------------------------------------------------------------------------------------
		// Native input events.
		// ------------------------------------------------------------------------------------------

		GDINT void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args)
		{
			CoreWindow::GetForCurrentThread()->Activate();
		}

		GDINT void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args)
		{
			printf_s("");
		}
		GDINT void OnResuming(Platform::Object^ sender, Platform::Object^ args)
		{
			printf_s("");
		}

		// ------------------------------------------------------------------------------------------
		// Native window events.
		// ------------------------------------------------------------------------------------------

		GDINT void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
		{
			auto const window = FindWindowBySystemWindowHandle(sender);
			m_AppDelegate.OnAppWindowClosed(window);
		}

		// ------------------------------------------------------------------------------------------
		// Native input events.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Event sent when pointer is pressed.
		 * 
		 * @param sender Current active window.
		 * @param args Key event description.
		 */
		GDINT void OnWindowPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
		{
			printf_s("");
		}

		/*!
		 * Event sent when pointer is released after beening pressed.
		 * 
		 * @param sender Current active window.
		 * @param args Key event description.
		 */
		GDINT void OnWindowPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
		{
			printf_s("");
		}

		/*!
		 * Event sent when key is pressed.
		 * 
		 * @param sender Current active window.
		 * @param args Key event description.
		 */
		GDINT void OnWindowKeyDown(CoreWindow^ sender, KeyEventArgs^ args)
		{
			auto const windoKeyCode = ConvertKeyCode(args->VirtualKey);
			auto const window = FindWindowBySystemWindowHandle(sender);
		}

		/*!
		 * Event sent when key is released after beening pressed.
		 * 
		 * @param sender Current active window.
		 * @param args Key event description.
		 */
		GDINT void OnWindowKeyUp(CoreWindow^ sender, KeyEventArgs^ args)
		{
			auto const windoKeyCode = ConvertKeyCode(args->VirtualKey);
			auto const window = FindWindowBySystemWindowHandle(sender);
		}

	};	// ref class WindowsUWPFrameworkView

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Creates a native UWP application instance.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	ref class GD_PLATFORM_KERNEL WindowsUWPFrameworkViewSource sealed : public IFrameworkViewSource
	{
	private:
		IPlatformApplicationDelegate& m_AppDelegate;

	internal:
		GDINL WindowsUWPFrameworkViewSource(IPlatformApplicationDelegate& appDelegate)
			: m_AppDelegate(appDelegate)
		{
		}

	public:
		GDINT virtual IFrameworkView^ CreateView() sealed
		{
			return ref new WindowsUWPFrameworkView(m_AppDelegate);
		}
	};	// ref class WindowsUWPFrameworkViewSource

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Application on UWP platform.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL WindowsUWPPlatformApplication final : public IPlatformApplication
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Main event loop.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Starts the main event loop.
		 * 
		 * @param appDelegate Application event delegate.
		 * @returns True if the main loop was successfully ran.
		 */
		GDINT virtual bool Run(IPlatformApplicationDelegate& appDelegate) override final
		{
			CoreApplication::Run(ref new WindowsUWPFrameworkViewSource(appDelegate));
			return true;
		}

		/*!
		 * Stop the main event loop.
		 */
		GDINT virtual void Stop() override final
		{
		}

	};	// class WindowsUWPPlatformApplication

	GD_IMPLEMENT_SINGLETON(IPlatformApplication, WindowsUWPPlatformApplication);

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_WINDOWS
