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

namespace GoddamnEngine.BuildSystem
{
    //! List of supported target platforms.
    public enum TargetPlatform : byte
    {
        Unknown,
        Windows,
        WindowsRT,
        WindowsStore,
        WindowsPhone,
        XboxOne,
        OSX,
        iOS,
        PlayStation4,
        Android,
        Linux,
        Emscripten,
    }   // enum TargetPlatforms

    //! List of properties for specific platform.
    public class TargetPlatformInfo
    {
        private static Dictionary<TargetPlatform, TargetPlatformInfo> CachedInformation;
        public readonly TargetPlatform Platform;
        public string HumanReadableName { get; protected set; }
        public bool IsSDKInstalled { get; protected set; }
        public string CompilerPath { get; protected set; }
        public string[] StandartHeadersPathes { get; protected set; }
        public string[] StandartMacros { get; protected set; }
        public string[] StandartLibraries { get; protected set; }
        public bool RequiresExceptions { get; protected set; }
        public bool RequiresRTTI { get; protected set; }

        //! Initializes a platform information. Should setup properties.
        protected TargetPlatformInfo()
        {
        }

        //! Generates a strigified list of standart include paths.
        //! @param Separator Separator string between include paths. ';' By default.
        //! @returns A strigified include paths.
        public string GenerateStandartIncludePaths(string Separator = null)
        {
            if (StandartHeadersPathes != null)
            {
                if (Separator == null)
                {
                    Separator = Path.PathSeparator.ToString();
                }

                StringBuilder StandartIncludePathsBuilder = new StringBuilder();
                Array.ForEach(StandartHeadersPathes, StandartHeadersPath =>
                    StandartIncludePathsBuilder.Append(StandartHeadersPath).Append(Separator)
                );

                return StandartIncludePathsBuilder.ToString();
            }

            return null;
        }

        //! Generates a strigified list of standart macros.
        //! @param Separator Separator string between macros. ';' By default.
        //! @returns A strigified list of macros.
        public string GenerateStandartMacrosList(string Separator = null)
        {
            if (StandartMacros != null)
            {
                if (Separator == null)
                {
                    Separator = Path.PathSeparator.ToString();
                }

                StringBuilder StandartMacrosListBuilder = new StringBuilder();
                Array.ForEach(StandartMacros, StandartMacro =>
                 StandartMacrosListBuilder.Append(StandartMacro).Append(Separator)
                );

                return StandartMacrosListBuilder.ToString();
            }

            return null;
        }

        //! Generates a strigified list of standart linked libraries paths.
        //! @param Separator Separator string between linked libraries paths. ';' By default.
        //! @returns A strigified list of linked libraries paths.
        public string GenerateStandartLinkedLibrariesPaths(string Separator = null)
        {
            if (StandartLibraries != null)
            {
                if (Separator == null)
                {
                    Separator = Path.PathSeparator.ToString();
                }

                StringBuilder StandartLinkedLibraries = new StringBuilder();

                // Linking with default libraries on specified platform.
                Array.ForEach(StandartLibraries, StandartLibrary =>
                 StandartLinkedLibraries.Append(StandartLibrary).Append(Path.PathSeparator)
                );

                return StandartLinkedLibraries.ToString();
            }

            return null;
        }

        //! Returns the information for specific platform.
        //! @param ThePlatform Desired platform.
        //! @returns The information for specific platform.
        public static TargetPlatformInfo Get(TargetPlatform ThePlatform)
        {
            if (CachedInformation == null)
            {
                CachedInformation = new Dictionary<TargetPlatform, TargetPlatformInfo>();
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                {
                    Type PlatformInfoType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(Platform + "PlatformInfo"));
                    if (PlatformInfoType != null)
                    {
                        TargetPlatformInfo PlatformInfo = (TargetPlatformInfo)Activator.CreateInstance(PlatformInfoType);
                        CachedInformation.Add(Platform, PlatformInfo);
                    }
                    else
                    {
                        throw new BuildSystemException("Not platform information exists for platform {0}.", Platform);
                    }
                }
            }

            return CachedInformation[ThePlatform];
        }
    }   // class TargetPlatformInfo

    //! "Windows" platform information.
    public sealed class WindowsPlatformInfo : TargetPlatformInfo
    {
        public WindowsPlatformInfo()
        {
            IsSDKInstalled = true;
            HumanReadableName = "Windows (x64)";
            StandartHeadersPathes = null;
            CompilerPath = "cl.exe";
            StandartLibraries = new string[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            RequiresExceptions = true;
            RequiresRTTI = false;
        }
    }   // class WindowsPlatformInfo

    //! "Emscripten" platform information.
    public sealed class EmscriptenPlatformInfo : TargetPlatformInfo
    {
        public EmscriptenPlatformInfo()
        {
            HumanReadableName = "Emscripten (HTML5)";
            RequiresExceptions = true;
            RequiresRTTI = false;

            // Getting the installation directory of the Emscripten.
            string EmscriptenSDKPath = null;
            var Emscripten64Key = Microsoft.Win32.RegistryKey
                .OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, Microsoft.Win32.RegistryView.Registry64)
                .OpenSubKey("Software\\Emscripten64");
            if (Emscripten64Key != null)
            {
                string Emscripten64SDKLocation = (string)Emscripten64Key.GetValue("Install_Dir");
                string Emscripten64LatestVersion = Directory.EnumerateDirectories(Path.Combine(Emscripten64SDKLocation, "emscripten")).LastOrDefault();
                if (Emscripten64LatestVersion != null)
                {
                    IsSDKInstalled = true;
                    EmscriptenSDKPath = Emscripten64LatestVersion;
                }
                else
                {
                    throw new BuildSystemException("Something is wrong in Emscripten SDK installation path. Try to reinstall Emscripten.");
                }
            }
            else
            {
                string EmscriptenVar = Environment.GetEnvironmentVariable("EMSCRIPTEN");
                if (EmscriptenVar != null)
                {
                    EmscriptenSDKPath = EmscriptenVar;
                }
                else
                {
                    IsSDKInstalled = false;
                }
            }

            string EmscriptenIncludePath = Path.Combine(EmscriptenSDKPath, "system", "include");
            CompilerPath = string.Format("\"{0}\"", Path.Combine(EmscriptenSDKPath, "emcc" + (Environment.OSVersion.Platform == PlatformID.Win32NT ? ".bat" : "")));
            StandartHeadersPathes = new string[] { EmscriptenIncludePath, Path.Combine(EmscriptenIncludePath, "libc"), Path.Combine(EmscriptenIncludePath, "libcxx") };
            StandartMacros = new string[] { "__EMSCRIPTEN__", "__clang__" };
            StandartLibraries = null;
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
