// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace GoddamnEngine.BuildSystem.Target
{
    /// <summary>
    /// List of supported target platforms.
    /// </summary>
    public enum TargetPlatform : byte
    {
        Unknown,
        Windows,
        XboxOne,
        PlayStation4,
    }   // enum TargetPlatforms

    /// <summary>
    /// List of properties for specific platform.
    /// </summary>
    public class TargetPlatformInfo
    {
        private static Dictionary<TargetPlatform, TargetPlatformInfo> s_CachedInformation;

        public string HumanReadableName { get; protected set; }
        //public bool IsSdkInstalled { get; protected set; }
        //public string CompilerPath { get; protected set; }
        public string[] StandardHeadersPathes { get; protected set; }
        public string[] StandardMacros { get; protected set; }
        public string[] StandardLibraries { get; protected set; }
        public bool RequiresExceptions { get; protected set; }
        public bool RequiresRtti { get; protected set; }

        /// <summary>
        /// Initializes a platform information. Should setup properties.
        /// </summary>
        protected TargetPlatformInfo()
        {
        }

        /// <summary>
        /// Generates a strigified list of standard include paths.
        /// </summary>
        /// <param name="separator">Separator string between include paths. ';' By default.</param>
        /// <returns>A strigified include paths.</returns>
        public string GenerateStandardIncludePaths(string separator = null)
        {
            if (StandardHeadersPathes == null)
            {
                return null;
            }

            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }
            var standardIncludePathsBuilder = new StringBuilder();
            Array.ForEach(StandardHeadersPathes, standardHeadersPath => standardIncludePathsBuilder.Append(standardHeadersPath).Append(separator));
            return standardIncludePathsBuilder.ToString();
        }

        /// <summary>
        /// Generates a strigified list of standard macros.
        /// </summary>
        /// <param name="separator">Separator string between macros. ';' By default.</param>
        /// <returns>A strigified list of macros.</returns>
        public string GenerateStandardMacrosList(string separator = null)
        {
            if (StandardMacros == null)
            {
                return null;
            }

            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            var standardMacrosListBuilder = new StringBuilder();
            Array.ForEach(StandardMacros, standardMacro => standardMacrosListBuilder.Append(standardMacro).Append(separator));
            return standardMacrosListBuilder.ToString();
        }

        /// <summary>
        /// Generates a strigified list of standard linked libraries paths.
        /// </summary>
        /// <param name="separator">Separator string between linked libraries paths. ';' By default.</param>
        /// <returns>A strigified list of linked libraries paths.</returns>
        public string GenerateStandardLinkedLibrariesPaths(string separator = null)
        {
            if (StandardLibraries == null)
            {
                return null;
            }

            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            var standardLinkedLibraries = new StringBuilder();
            Array.ForEach(StandardLibraries, standardLibrary => standardLinkedLibraries.Append(standardLibrary).Append(separator));
            return standardLinkedLibraries.ToString();
        }

        /// <summary>
        /// Returns the information for specific platform.
        /// </summary>
        /// <param name="thePlatform">Desired platform.</param>
        /// <returns>The information for specific platform.</returns>
        public static TargetPlatformInfo Get(TargetPlatform thePlatform)
        {
            if (s_CachedInformation == null)
            {
                s_CachedInformation = new Dictionary<TargetPlatform, TargetPlatformInfo>
                {
                    [TargetPlatform.Unknown] = null,
                    [TargetPlatform.Windows] = new WindowsPlatformInfo(),
                    [TargetPlatform.XboxOne] = new XboxOnePlatformInfo(),
                    [TargetPlatform.PlayStation4] = new PlayStation4PlatformInfo()
                };
            }
            return s_CachedInformation[thePlatform];
        }
    }   // class TargetPlatformInfo

    /// <summary>
    /// "Windows" platform information.
    /// </summary>
    internal sealed class WindowsPlatformInfo : TargetPlatformInfo
    {
        public WindowsPlatformInfo()
        {
            //IsSdkInstalled = true;
            HumanReadableName = "Windows (x64)";
            //StandardHeadersPathes = null;
            //CompilerPath = "cl.exe";
            //StandardLibraries = new[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            RequiresExceptions = true;
            RequiresRtti = false;
        }
    }   // class WindowsPlatformInfo

    /// <summary>
    /// "XBox One" platform information.
    /// </summary>
    internal sealed class XboxOnePlatformInfo : TargetPlatformInfo
    {
        public XboxOnePlatformInfo()
        {
            //IsSdkInstalled = true;
            HumanReadableName = "XBox One";
            //StandardHeadersPathes = null;
            //CompilerPath = "cl.exe";
            //StandardLibraries = new[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            //RequiresExceptions = true;
            RequiresRtti = false;
        }
    }   // class XboxOnePlatformInfo

    /// <summary>
    /// "PlayStation 4" platform information.
    /// </summary>
    internal sealed class PlayStation4PlatformInfo : TargetPlatformInfo
    {
        public PlayStation4PlatformInfo()
        {
            //IsSdkInstalled = true;
            HumanReadableName = "Sony PlayStation 4";
            //StandardHeadersPathes = null;
            //CompilerPath = "cl.exe";
            //StandardLibraries = new[] { "winmm.lib", "imm32.lib", "version.lib", "Dwmapi.lib" };
            //RequiresExceptions = true;
            RequiresRtti = false;
        }
    }   // class PlayStation4PlatformInfo

    // Template for new platforms:
#if FALSE
    /// <summary>
    /// "" platform information.
    /// </summary>
    public sealed class PlatformInfo : TargetPlatformInfo
    {
        public PlatformInfo()
        {
        }
    }   // class PlatformInfo
#endif  // FALSE

}   // GoddamnEngine.BuildSystem
