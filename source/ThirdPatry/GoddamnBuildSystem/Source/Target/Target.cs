//! ==========================================================================================
//! Target.cs - Build target class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Diagnostics;

namespace GoddamnEngine.BuildSystem
{
    //! List of target actions that build system can do.
    public enum TargetAction : byte
    {
        Unknown,
        GenerateSolutionFiles,
        AcrhiveDependencies
    }   // enum TargetAction

    //! List of supported target platforms.
    public enum TargetPlatform : byte
    {
        Unknown,
        Windows,
        WindowsRT,
        WindowsStore,
        WindowsPhone8,
        XBoxOne,
        OSX,
        iOS,
        PlayStation4,
        Android,
        Linux,
        Emscripten,
    }   // enum TargetPlatform

    //! List of supported target make systems.
    public enum TargetProjectCompiler : byte
    {
        Unknown,
        Archieve,
        VisualStudio2015,
        VisualStudio2013,
        XCode,
        QtCreator,
        Make,
    }   //enum TargetBuildSystem

    //! List of supported target configurations.
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Release
    }   // enum Configuration

    //! Represents a project generation (or some other action) algorithm interface.
    public abstract class Target
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Helper methods for target-describing enumerations.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Returns true if specified platform is desktop.
        public static bool IsDesktopPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                case TargetPlatform.Windows:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform is mobile.
        public static bool IsMobilePlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsPhone8:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform is console.
        public static bool IsConsolePlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.XBoxOne:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform is Web
        public static bool IsWebPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.Emscripten:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform`s native API is WinAPI.
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

        //! Returns true if specified platform is Posix compatible.
        public static bool IsPosixPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.iOS:
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                case TargetPlatform.Android:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform`s native API is Cocoa.
        public static bool IsCocoaPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.iOS:
                case TargetPlatform.OSX:
                    return true;
            }

            return false;
        }

        //! Returns true if object on specified path has platform/configuration data and matches it.
        public static bool MatchesPlatformConfiguration(string ObjectPath, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            string ObjectPathExtensionless = Path.GetFileNameWithoutExtension(ObjectPath);
            string ObjectPlatform = Path.GetExtension(ObjectPathExtensionless);
            if (!string.IsNullOrEmpty(ObjectPlatform)) {
                // Checking if platform information is defined and it is of type TargetPlatform.
                if (ObjectPlatform == string.Concat('.', Platform)) {
                    string LibraryConfiguration = Path.GetExtension(Path.GetFileNameWithoutExtension(ObjectPathExtensionless));
                    if (!string.IsNullOrEmpty(LibraryConfiguration)) {
                        // Checking if platform information is defined and it is of type TargetConfiguration.
                        return (LibraryConfiguration == string.Concat('.', Configuration));
                    }
                } else {
                    // Library filename contains some platform information, but it not matches target platform.
                    return false;
                }
            }

            return true;
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Generation interface.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Generates project files for specified project.
        //! @param ProjectObject Parsed project object.
        //! @param ProjectPlatform Target platform of the object
        //! @param ProjectConfigurations Array of target configurations.
        //! @returns Path to main project file. E.g. .VCXPROJ for Visual Studio.
        internal virtual string GenerateProjectFiles(Project ProjectObject, TargetPlatform ProjectPlatform, TargetConfiguration[] ProjectConfigurations)
        {
            string ProjectDirectoryPath = Path.Combine(Path.GetDirectoryName(ProjectObject.GetLocation()), "_Build");
            Directory.CreateDirectory(ProjectDirectoryPath);

            return ProjectDirectoryPath;
        }

        //! @}
    }   // class Target
}   // namespace GoddamnEngine.BuildSystem
