// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Target.cs
//! Description for target platforms, confutations, etc.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
    //! Contains helper methods for targets.
    public static class Target
    {
        //! Enumerates all supported and implemented target platforms.
        public static IEnumerable<TargetPlatform> EnumerateAllPlatforms()
        {
            switch (Environment.OSVersion.Platform)
            {
                case PlatformID.Win32NT:
                    yield return TargetPlatform.Windows;
                    //yield return TargetPlatforms.WindowsRT;
                    //yield return TargetPlatforms.WindowsStore;
                    //yield return TargetPlatforms.WindowsPhone;
                    //yield return TargetPlatforms.XboxOne;
                    //yield return TargetPlatforms.PlayStation4;   // Not sure here..
                    break;
                case PlatformID.MacOSX:
                    //yield return TargetPlatforms.OSX;
                    //yield return TargetPlatforms.iOS;
                    break;
                case PlatformID.Unix:
                    //yield return TargetPlatforms.Linux;
                    break;
            }

            //yield return TargetPlatforms.Android;
            yield return TargetPlatform.Emscripten;
        }

        //! Enumerates all supported and implemented target configurations.
        public static IEnumerable<TargetConfiguration> EnumerateAllConfigurations()
        {
            yield return TargetConfiguration.Debug;
            yield return TargetConfiguration.Release;
            yield return TargetConfiguration.Development;
            yield return TargetConfiguration.Shipping;
        }

        //! Returns true if target platform is desktop.
        public static bool IsDesktopPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                case TargetPlatform.Windows:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is mobile.
        public static bool IsMobilePlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsPhone:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is console.
        public static bool IsConsolePlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform is Web
        public static bool IsWebPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.Emscripten:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform`s native API is WinAPI.
        public static bool IsWinAPIPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.XboxOne:
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsPhone:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is POSIX compatible.
        public static bool IsPosixPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                case TargetPlatform.Android:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform`s native API is Cocoa.
        public static bool IsCocoaPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.OSX:
                    return true;
            }

            return false;
        }
    }   // class Target
}   // namespace GoddamnEngine.BuildSystem
