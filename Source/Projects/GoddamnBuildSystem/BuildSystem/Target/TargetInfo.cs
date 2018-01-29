// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem.Target
{
    /// <summary>
    /// Contains helper methods for targets.
    /// </summary>
    public static class TargetInfo
    {
        /// <summary>
        /// Enumerates all supported and implemented target platforms.
        /// </summary>
        /// <returns>All supported and implemented target platforms.</returns>
        public static IEnumerable<TargetPlatform> EnumerateAllPlatforms()
        {
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                yield return TargetPlatform.Windows;
                //yield return TargetPlatform.XboxOne;
                //yield return TargetPlatform.PlayStation4;   // Not sure here..
            }
            else
            {
                yield return TargetPlatform.MacOS;
            }
        }

        /// <summary>
        /// Enumerates all supported and implemented target configurations.
        /// </summary>
        /// <returns>All supported and implemented target configurations.</returns>
        public static IEnumerable<TargetConfiguration> EnumerateAllConfigurations()
        {
            yield return TargetConfiguration.Debug;
            //yield return TargetConfiguration.Development;
            yield return TargetConfiguration.Release;
        }

        /// <summary>
        /// Returns true if target platform is desktop.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if target platform is desktop.</returns>
        public static bool IsDesktopPlatform(TargetPlatform platform)
        {
            return platform == TargetPlatform.Windows;
        }

        /// <summary>
        /// Returns true if target platform is mobile.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if target platform is mobile.</returns>
        public static bool IsMobilePlatform(TargetPlatform platform)
        {
            return false;
        }

        /// <summary>
        /// Returns true if target platform is console.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if target platform is console.</returns>
        public static bool IsConsolePlatform(TargetPlatform platform)
        {
            return platform == TargetPlatform.XboxOne || platform == TargetPlatform.PlayStation4;
        }

        /// <summary>
        /// Returns true if target platform`s native API is WinAPI.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if target platform`s native API is WinAPI.</returns>
        public static bool IsMicrosoftPlatform(TargetPlatform platform)
        {
            return platform == TargetPlatform.XboxOne || platform == TargetPlatform.Windows;
        }

        /// <summary>
        /// Returns true if target platform`s native API is Cocoa.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if target platform`s native API is Cocoa.</returns>
        public static bool IsApplePlatform(TargetPlatform platform)
        {
            return platform == TargetPlatform.MacOS;
        }

        /// <summary>
        /// Returns true if target platform is POSIX compatible.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if target platform is POSIX compatible.</returns>
        public static bool IsPosixPlatform(TargetPlatform platform)
        {
            return platform == TargetPlatform.MacOS || platform == TargetPlatform.Windows;
        }

    }   // class TargetInfo

}   // namespace GoddamnEngine.BuildSystem
