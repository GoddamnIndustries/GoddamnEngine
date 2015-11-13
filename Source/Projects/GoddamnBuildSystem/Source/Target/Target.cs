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
                    //yield return TargetPlatform.WindowsUap;
                    //yield return TargetPlatform.WindowsPhone;
                    //yield return TargetPlatform.XboxOne;
                    //yield return TargetPlatform.PlayStation4;   // Not sure here..
                    break;
                case PlatformID.MacOSX:
                    //yield return TargetPlatform.OsX;
                    //yield return TargetPlatform.iOS;
                    break;
                case PlatformID.Unix:
                    //yield return TargetPlatform.Linux;
                    break;
            }

            yield return TargetPlatform.Android;
            //yield return TargetPlatform.Emscripten;
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
        public static bool IsDesktopPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.OsX:
                case TargetPlatform.Linux:
                case TargetPlatform.Windows:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is mobile.
        public static bool IsMobilePlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.WindowsUap:
                case TargetPlatform.WindowsPhone:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is console.
        public static bool IsConsolePlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform is Web
        public static bool IsWebPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.Emscripten:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform`s native API is WinAPI.
        public static bool IsWinApiPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.XboxOne:
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsUap:
                case TargetPlatform.WindowsPhone:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is POSIX compatible.
        public static bool IsPosixPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.OsX:
                case TargetPlatform.Linux:
                case TargetPlatform.Android:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform`s native API is Cocoa.
        public static bool IsCocoaPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.OsX:
                    return true;
            }

            return false;
        }
    }   // class Target
}   // namespace GoddamnEngine.BuildSystem
