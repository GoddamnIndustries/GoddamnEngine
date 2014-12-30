//! ==========================================================================================
//! TargetConfiguration.cs - descriptions for platforms.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

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
        XBoxOne,
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
        private static Dictionary<TargetPlatform, TargetPlatformInfo> s_CachedInformation;
        public readonly TargetPlatform m_Platform;
        public string m_HumanReadableName { get; protected set; }
        public bool m_IsSDKInstalled { get; protected set; }
        public string m_CompilerPath { get; protected set; }
        public string[] m_StandartHeadersPathes { get; protected set; }
        public string[] m_StandartMacros { get; protected set; }
        public string[] m_StandartLibraries { get; protected set; }
        public bool m_RequiresExceptions { get; protected set; }
        public bool m_RequiresRTTI { get; protected set; }

        //! Instantiates a platform information. Should setup properties.
        protected TargetPlatformInfo()
        {
        }

        //! Generates a strigified list of standart include paths.
        public string GenerateStandartIncludePaths()
        {
            if (m_StandartHeadersPathes != null) {
                StringBuilder StandartIncludePathsBuilder = new StringBuilder();
                Array.ForEach(m_StandartHeadersPathes, StandartHeadersPath =>
                 StandartIncludePathsBuilder.Append(StandartHeadersPath).Append(Path.PathSeparator)
                );

                return StandartIncludePathsBuilder.ToString();
            }

            return null;
        }

        //! Generates a strigified list of standart include paths.
        public string GenerateStandartMacrosList()
        {
            if (m_StandartMacros != null) {
                StringBuilder StandartMacrosListBuilder = new StringBuilder();
                Array.ForEach(m_StandartMacros, StandartMacro =>
                 StandartMacrosListBuilder.Append(StandartMacro).Append(Path.PathSeparator)
                );

                return StandartMacrosListBuilder.ToString();
            }

            return null;
        }

        //! Generates a strigified list of standart linked libraries paths.
        public string GenerateStandartLinkedLibrariesPaths()
        {
            if (m_StandartLibraries != null) {
                StringBuilder StandartLinkedLibraries = new StringBuilder();

                // Linking with default libraries on specified platform.
                Array.ForEach(m_StandartLibraries, StandartLibrary =>
                 StandartLinkedLibraries.Append(StandartLibrary).Append(Path.PathSeparator)
                );

                return StandartLinkedLibraries.ToString();
            }

            return null;
        }

        //! Returns the information for specific platform.
        //! @param ThePlatform Desired platform.
        public static TargetPlatformInfo Get(TargetPlatform ThePlatform)
        {
            if (s_CachedInformation == null) {
                s_CachedInformation = new Dictionary<TargetPlatform, TargetPlatformInfo>();
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                    Type PlatformInfoType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(Platform + "PlatformInfo"));
                    if (PlatformInfoType != null) {
                        TargetPlatformInfo PlatformInfo = (TargetPlatformInfo)Activator.CreateInstance(PlatformInfoType);
                        s_CachedInformation.Add(Platform, PlatformInfo);
                    } else {
                        throw new BuildSystemException("Not platform information exists for platform {0}.", Platform);
                    }
                }
            }

            return s_CachedInformation[ThePlatform];
        }
    }   // class TargetPlatformInfo

    //! "Windows" platform information.
    public sealed class WindowsPlatformInfo : TargetPlatformInfo
    {
        public WindowsPlatformInfo()
        {
            m_IsSDKInstalled = true;
            m_HumanReadableName = "Windows (x64)";
            m_StandartHeadersPathes = null;
            m_CompilerPath = "cl.exe";
            m_StandartLibraries = new string[] { "winmm.lib", "imm32.lib", "version.lib" };
            m_RequiresExceptions = true;
            m_RequiresRTTI = false;
        }
    }   // class WindowsPlatformInfo

    //! "Emscripten" platform information.
    public sealed class EmscriptenPlatformInfo : TargetPlatformInfo
    {
        public EmscriptenPlatformInfo()
        {
            m_HumanReadableName = "Emscripten (HTML5)";
            m_RequiresExceptions = true;
            m_RequiresRTTI = false;

            // Getting the installation directory of the Emscripten.
            string EmscriptenSDKPath = null;
            var Emscripten64Key = Microsoft.Win32.RegistryKey
             .OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, Microsoft.Win32.RegistryView.Registry64)
             .OpenSubKey("Software\\Emscripten64");
            if (Emscripten64Key != null) {
                string Emscripten64SDKLocation = (string)Emscripten64Key.GetValue("Install_Dir");
                string Emscripten64LatestVersion = Directory.EnumerateDirectories(Path.Combine(Emscripten64SDKLocation, "emscripten")).LastOrDefault();
                if (Emscripten64LatestVersion != null) {
                    m_IsSDKInstalled = true;
                    EmscriptenSDKPath = Emscripten64LatestVersion;
                } else {
                    throw new BuildSystemException("Something is wrong in Emscripten SDK installation path. Try to reinstall Emscripten.");
                }
            } else {
                string EmscriptenVar = Environment.GetEnvironmentVariable("EMSCRIPTEN");
                if (EmscriptenVar != null) {
                    EmscriptenSDKPath = EmscriptenVar;
                } else {
                    m_IsSDKInstalled = false;
                }
            }

            string EmscriptenIncludePath = Path.Combine(EmscriptenSDKPath, "system", "include");
            m_CompilerPath = string.Format("\"{0}\"", Path.Combine(EmscriptenSDKPath, "emcc" + (Environment.OSVersion.Platform == PlatformID.Win32NT ? ".bat" : "")));
            m_StandartHeadersPathes = new string[] { EmscriptenIncludePath, Path.Combine(EmscriptenIncludePath, "libc"), Path.Combine(EmscriptenIncludePath, "libcxx") };
            m_StandartMacros = new string[] { "__EMSCRIPTEN__", "__clang__" };
            m_StandartLibraries = null;
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
