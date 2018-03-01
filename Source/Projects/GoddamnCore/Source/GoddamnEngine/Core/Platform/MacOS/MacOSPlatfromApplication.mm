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
#include <GoddamnEngine/Core/Containers/Vector.h>
#if GD_PLATFORM_OS_X

#import <Cocoa/Cocoa.h>

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! Application on MacOS platform.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class GD_PLATFORM_KERNEL MacOSPlatformApplication final : public IPlatformApplication
    {
    private:
        Vector<SharedPtr<IPlatformWindow>> m_Windows;
        AtomicBool m_IsRunning;
        
    public:
        GDINT virtual SharedPtr<IPlatformWindow> GetMainWindow() override final
        {
            return m_Windows[0];
        }
        
        GDINT virtual SharedPtr<IPlatformWindow> CreateApplicationWindow(IWindowDelegate& windowDelegate) override final
        {
            return CreateApplicationWindowBase(windowDelegate);
        }
        
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
            m_Windows.InsertLast(CreateApplicationWindow(appDelegate));
            [NSApplication sharedApplication];
            [NSApp run];
            return true;
        }
        
        /*!
         * Stop the main event loop.
         */
        GDINT virtual void Stop() override final
        {
            m_IsRunning = false;
        }
        
    };    // class MacOSPlatformApplication

    GD_IMPLEMENT_SINGLETON(IPlatformApplication, MacOSPlatformApplication);

GD_NAMESPACE_END

#endif    // if GD_PLATFORM_OS_X
