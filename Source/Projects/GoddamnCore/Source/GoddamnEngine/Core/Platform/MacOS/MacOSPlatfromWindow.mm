// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Platform window.
 */
#include <GoddamnEngine/Core/Platform/PlatformWindow.h>
#include <GoddamnEngine/Core/Platform/PlatformApplication.h>
#if GD_PLATFORM_OS_X

#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#import <Cocoa/Cocoa.h>

// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
//! Cocoa window on MacOS platform.
// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
@interface GD_PLATFORM_KERNEL GoddamnMacOSCocoaWindow : NSWindow<NSWindowDelegate>
{
    GD::IWindowDelegate* m_WindowDelegate;
}
@end

@implementation GoddamnMacOSCocoaWindow

/*!
 * Initializes a MacOS window.
 *
 * @param contentRect Window client rectangle.
 * @param styleMask Window style mask.
 * @param windowDelegate Window delegate.
 */
-(id)initWithContentRectAndDelegate:(NSRect)contentRect styleMask:(NSUInteger)styleMask windowDelegate:(GD::IWindowDelegate*)windowDelegate
{
    id const newSelf = [super initWithContentRect:contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];
    if (newSelf != nullptr)
    {
        m_WindowDelegate = windowDelegate;
    }
    return newSelf;
}

-(BOOL)canBecomeMainWindow
{
    return YES;
}
-(BOOL)canBecomeKeyWindow
{
    return YES;
}

/*!
 * Handles key
 */
-(void)keyDown:(NSEvent*)event
{
    printf("kek");
}

@end

GD_NAMESPACE_BEGIN

    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    //! System window on MacOS platform.
    // **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
    class GD_PLATFORM_KERNEL MacOSPlatformWindow : public IPlatformWindow
    {
    private:
        GoddamnMacOSCocoaWindow* m_CocoaWindow;
        
    public:
        GDINT explicit MacOSPlatformWindow(IWindowDelegate& windowDelegate)
        {
            m_CocoaWindow = [[GoddamnMacOSCocoaWindow alloc] initWithContentRectAndDelegate:NSMakeRect(100.0f, 100.0f, 400.0f, 400.0f) styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable windowDelegate:&windowDelegate];
            NSWindowController* windowController = [[NSWindowController alloc] initWithWindow:m_CocoaWindow];
            [m_CocoaWindow orderFrontRegardless];
            [m_CocoaWindow setAcceptsMouseMovedEvents:YES];
            [m_CocoaWindow setLevel:NSNormalWindowLevel];
            [m_CocoaWindow setDelegate:m_CocoaWindow];
            [m_CocoaWindow setOpaque:NO];
        }
        
        /*!
         * Returns system handle of this window.
         */
        GDINT virtual CHandle GetSystemHandle() const override final
        {
            return (__bridge CHandle)m_CocoaWindow;
        }
        
    public:
        
        // ------------------------------------------------------------------------------------------
        // Window sizing.
        // ------------------------------------------------------------------------------------------
        
        /*!
         * Sets size of the window client area.
         *
         * @param windowSize New window size.
         * @returns True if operation succeeded.
         */
        GDINT virtual bool SetWindowSize(WindowSize const& windowSize) override final
        {
            return false;
        }
        
        /*!
         * Sets type of the window.
         *
         * @param windowType New window type.
         * @returns True if operation succeeded.
         */
        GDINT virtual bool SetWindowType(WindowType const windowType) override final
        {
            return false;
        }
        
        /*!
         * Sets flags of the window.
         *
         * @param windowFlags New window flags.
         * @returns True if operation succeeded.
         */
        GDINT virtual bool SetWindowFlags(WindowFlags const windowFlags) override final
        {
            return false;
        }
    };    // class MacOSPlatformWindow

    GDINT SharedPtr<IPlatformWindow> IPlatformApplication::CreateApplicationWindowBase(IWindowDelegate& windowDelegate)
    {
        return gd_new MacOSPlatformWindow(windowDelegate);
    }

GD_NAMESPACE_END

#endif    // if GD_PLATFORM_MacOS
