/// ==========================================================================================
/// Target.cs - Build target class.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2014 - Created by James Jhuighuy
///     * 26.11.2014 - Refactored.
/// ==========================================================================================

using System;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// List of supported target platforms.
    /// </summary>
    public enum TargetPlatform : byte
    {
        Unknown,
        Windows,
        WindowsPhone8,
        WindowsRT,
        WindowsStore,
        XBoxOne,
        OSX,
        iOS,
        PlayStation4,
        Android,
        Linux,
        Emscripten,
    }   // enum TargetPlatform

    /// <summary>
    /// List of supported target make systems.
    /// </summary>
    public enum TargetProjectCompiler : byte
    {
        Unknown,
        VisualStudio2014,
        VisualStudio2013,
        XCode,
        QtCreator,
        Make,
    }   //enum TargetBuildSystem

    /// <summary>
    /// List of supported target configurations.
    /// </summary>
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Release
    }   // enum Configuration

    /// <summary>
    /// Represents 
    /// </summary>
    public abstract class Target
    {
        //! ------------------------------------------------------------------------------------------
        //! Static methods.
        //! ------------------------------------------------------------------------------------------

        /// <summary> 
        /// Returns true if specified platform is desktop.
        /// </summary>
        public static bool IsDesktopPlatform(TargetPlatform Platform)
        {
            switch(Platform) {
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                case TargetPlatform.Windows: 
                    return true;
            }
        
            return false;
        }

        /// <summary> 
        /// Returns true if specified platform is mobile.
        /// </summary>
        public static bool IsMobilePlatform(TargetPlatform Platform)
        {
            switch(Platform) {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsPhone8:
                    return true;
            }
        
            return false;
        }

        /// <summary> 
        /// Returns true if specified platform is console.
        /// </summary>
        public static bool IsConsolePlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.XBoxOne:
                case TargetPlatform.PlayStation4: 
                    return true;
            }

            return false;
        }

        /// <summary> 
        /// Returns true if specified platform is Web
        /// </summary>
        public static bool IsWebPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.Emscripten:
                    return true;
            }

            return false;
        }

        /// <summary> 
        /// Returns true if specified platform`s native API is WinAPI.
        /// </summary>
        public static bool IsWinAPIPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.XBoxOne:
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsPhone8:
                    return true;
            }

            return false;
        }

        /// <summary> 
        /// Returns true if specified platform`s native API is Cocoa.
        /// </summary>
        public static bool IsCocoaPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.iOS: 
                case TargetPlatform.OSX: 
                    return true;
            }

            return false;
        }

        public abstract void GenerateProject(Project TheProject);
    }
}
