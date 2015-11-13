// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file TargetConfiguration.cs
//! Descriptions for platforms.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.Win32;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! List of supported target platforms.
    public enum TargetPlatform : byte
    {
        Unknown,
        Windows,
        WindowsUap,
        WindowsPhone,
        XboxOne,
        OsX,
        // ReSharper disable once InconsistentNaming
        iOS,
        PlayStation4,
        Android,
        Linux,
        Emscripten
    }   // enum TargetPlatforms

    // ------------------------------------------------------------------------------------------
    //! List of properties for specific platform.
    public class TargetPlatformInfo
    {
        private static Dictionary<TargetPlatform, TargetPlatformInfo> _cachedInformation;
        public readonly TargetPlatform Platform;
        public string HumanReadableName { get; protected set; }
        public bool IsSdkInstalled { get; protected set; }
        public string CompilerPath { get; protected set; }
        public string[] StandartHeadersPathes { get; protected set; }
        public string[] StandartMacros { get; protected set; }
        public string[] StandartLibraries { get; protected set; }
        public bool RequiresExceptions { get; protected set; }
        public bool RequiresRtti { get; protected set; }

        // ------------------------------------------------------------------------------------------
        //! Initializes a platform information. Should setup properties.
        protected TargetPlatformInfo(TargetPlatform platform)
        {
            Platform = platform;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of standard include paths.
        //! @param separator Separator string between include paths. ';' By default.
        //! @returns A strigified include paths.
        public string GenerateStandartIncludePaths(string separator = null)
        {
            if (StandartHeadersPathes != null)
            {
                if (separator == null)
                {
                    separator = Path.PathSeparator.ToString();
                }

                var standartIncludePathsBuilder = new StringBuilder();
                Array.ForEach(StandartHeadersPathes, standartHeadersPath =>
                    standartIncludePathsBuilder.Append(standartHeadersPath).Append(separator)
                );

                return standartIncludePathsBuilder.ToString();
            }

            return null;
        }

        //! Generates a strigified list of standard macros.
        //! @param separator Separator string between macros. ';' By default.
        //! @returns A strigified list of macros.
        public string GenerateStandartMacrosList(string separator = null)
        {
            if (StandartMacros != null)
            {
                if (separator == null)
                {
                    separator = Path.PathSeparator.ToString();
                }

                var standartMacrosListBuilder = new StringBuilder();
                Array.ForEach(StandartMacros, standartMacro =>
                    standartMacrosListBuilder.Append(standartMacro).Append(separator)
                );

                return standartMacrosListBuilder.ToString();
            }

            return null;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of standard linked libraries paths.
        //! @param separator Separator string between linked libraries paths. ';' By default.
        //! @returns A strigified list of linked libraries paths.
        public string GenerateStandartLinkedLibrariesPaths(string separator = null)
        {
            if (StandartLibraries != null)
            {
                if (separator == null)
                {
                    separator = Path.PathSeparator.ToString();
                }

                var standartLinkedLibraries = new StringBuilder();

                // Linking with default libraries on specified platform.
                Array.ForEach(StandartLibraries, standartLibrary =>
                    standartLinkedLibraries.Append(standartLibrary).Append(separator)
                );

                return standartLinkedLibraries.ToString();
            }

            return null;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns the information for specific platform.
        //! @param thePlatform Desired platform.
        //! @returns The information for specific platform.
        public static TargetPlatformInfo Get(TargetPlatform thePlatform)
        {
            if (_cachedInformation == null)
            {
                _cachedInformation = new Dictionary<TargetPlatform, TargetPlatformInfo>();
                foreach (var platform in Target.EnumerateAllPlatforms())
                {
                    var platformInfoType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(t => t.Name.EndsWith(platform + "PlatformInfo"));
                    if (platformInfoType != null)
                    {
                        var platformInfo = (TargetPlatformInfo)Activator.CreateInstance(platformInfoType);
                        _cachedInformation.Add(platform, platformInfo);
                    }
                    else
                    {
                        throw new BuildSystemException("Not platform information exists for platform {0}.", platform);
                    }
                }
            }

            return _cachedInformation[thePlatform];
        }
    }   // class TargetPlatformInfo

    // ------------------------------------------------------------------------------------------
    //! "Windows" platform information.
    public sealed class WindowsPlatformInfo : TargetPlatformInfo
    {
        public WindowsPlatformInfo() : base(TargetPlatform.Windows)
        {
            IsSdkInstalled = true;
            HumanReadableName = "Windows (x64)";
            StandartHeadersPathes = null;
            CompilerPath = "cl.exe";
            StandartLibraries = new[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            RequiresExceptions = true;
            RequiresRtti = false;
        }
    }   // class WindowsPlatformInfo

    // ------------------------------------------------------------------------------------------
    //! "Universal Windows Platform Application" platform information.
    public sealed class WindowsUapPlatformInfo : TargetPlatformInfo
    {
        public WindowsUapPlatformInfo() : base(TargetPlatform.WindowsUap)
        {
            IsSdkInstalled = true;
            HumanReadableName = "Windows (Universal Application)";
            StandartHeadersPathes = null;
            CompilerPath = "cl.exe";
            StandartLibraries = new[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            RequiresExceptions = true;
            RequiresRtti = true;
        }
    }   // class WindowsPlatformInfo

    // ------------------------------------------------------------------------------------------
    //! Android platform information.
    public sealed class AndroidPlatformInfo : TargetPlatformInfo
    {
        public AndroidPlatformInfo() : base(TargetPlatform.WindowsUap)
        {
            IsSdkInstalled = true;
            HumanReadableName = "Google Android";
            StandartHeadersPathes = null;
            CompilerPath = "cl.exe";
            StandartLibraries = new[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            RequiresExceptions = true;
            RequiresRtti = true;
        }
    }   // class AndroidPlatformInfo

    // ------------------------------------------------------------------------------------------
    //! "Emscripten" platform information.
    public sealed class EmscriptenPlatformInfo : TargetPlatformInfo
    {
        public EmscriptenPlatformInfo() : base(TargetPlatform.Emscripten)
        {
            IsSdkInstalled = false;
#if FALSE
            HumanReadableName = "Emscripten (HTML5)";
            RequiresExceptions = true;
            RequiresRtti = false;

            // Getting the installation directory of the Emscripten.
            string emscriptenSdkPath;
            var emscripten64Key = RegistryKey
                .OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry64)
                .OpenSubKey("Software\\Emscripten64");
            if (emscripten64Key != null)
            {
                var emscripten64SdkLocation = (string)emscripten64Key.GetValue("Install_Dir");
                var emscripten64LatestVersion = Directory.EnumerateDirectories(Path.Combine(emscripten64SdkLocation, "emscripten")).LastOrDefault();
                if (emscripten64LatestVersion != null)
                {
                    IsSdkInstalled = true;
                    emscriptenSdkPath = emscripten64LatestVersion;
                }
                else
                {
                    throw new BuildSystemException("Something is wrong in Emscripten SDK installation path. Try to reinstall Emscripten.");
                }
            }
            else
            {
                var emscriptenVar = Environment.GetEnvironmentVariable("EMSCRIPTEN");
                if (emscriptenVar != null)
                {
                    emscriptenSdkPath = emscriptenVar;
                }
                else
                {
                    IsSdkInstalled = false;
                    return;
                }
            }

            var emscriptenIncludePath = Path.Combine(emscriptenSdkPath, "system", "include");
            CompilerPath = string.Format("\"{0}\"", Path.Combine(emscriptenSdkPath, "emcc" + (Environment.OSVersion.Platform == PlatformID.Win32NT ? ".bat" : "")));
            StandartHeadersPathes = new[] { emscriptenIncludePath, Path.Combine(emscriptenIncludePath, "libc"), Path.Combine(emscriptenIncludePath, "libcxx") };
            StandartMacros = new[] { "__EMSCRIPTEN__", "__clang__" };
            StandartLibraries = null;
#endif  // if FALSE
        }
    }   // class EmscriptenPlatformInfo

    // Template for new platforms:
#if FALSE
    //! "" platform information.
    public sealed class PlatformInfo : TargetPlatformInfo
    {
        public PlatformInfo()
        {
        }
    }   // class PlatformInfo
#endif  // FALSE
}   // GoddamnEngine.BuildSystem
