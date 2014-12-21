//! ==========================================================================================
//! Target.cs - description for target platforms, confutations, etc.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace GoddamnEngine.BuildSystem
{
    //! List of supported target platforms.
    public enum TargetPlatforms : byte
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
    public class TargetPlatformInformation
    {
        private static Dictionary<TargetPlatforms, TargetPlatformInformation> s_CachedInformation;
        public readonly TargetPlatforms m_Platform;
        public string m_HumanReadableName { get; protected set; }
        public string[] m_StandartHeadersPathes { get; protected set; }
        public string[] m_DefaultLibraries { get; protected set; }
        public bool m_RequiresExceptions { get; protected set; }
        public bool m_RequiresRTTI { get; protected set; }

        //! Instantiates a platform information. Should setup properties.
        protected TargetPlatformInformation()
        {
        }

        //! Returns the information for specific platform.
        //! @param ThePlatform Desired platform.
        public static TargetPlatformInformation Get(TargetPlatforms ThePlatform)
        {
            if (s_CachedInformation == null) {
                s_CachedInformation = new Dictionary<TargetPlatforms, TargetPlatformInformation>();
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    Type PlatformInformationType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(Platform + "PlatformInformation"));
                    if (PlatformInformationType == null) {
                        throw new BuildSystemException("Not platform information exists for platform {0}.", Platform);
                    }

                    TargetPlatformInformation PlatformInformation = (TargetPlatformInformation)Activator.CreateInstance(PlatformInformationType);
                    s_CachedInformation.Add(Platform, PlatformInformation);
                }
            }

            return s_CachedInformation[ThePlatform]; 
        }
    }   // class TargetPlatformInformation

    //! List of supported target configurations.
    public enum TargetConfigurations : byte
    {
        Unknown,
        Debug,
        Release,
        Development,
        Shipping,
    }   // enum TargetConfigurations

    //! List of properties for specific configuration.
    public class TargetConfigurationInformation
    {
        private static Dictionary<TargetConfigurations, TargetConfigurationInformation> s_CachedInformation;
        public readonly TargetConfigurations m_Configuration;
        public string m_HumanReadableName { get; protected set; }
        public bool m_IsDebug { get; protected set; }
        public bool m_GenerateDebugInformation { get; protected set; }
        public bool m_Optimize { get; protected set; }

        //! Instantiates a configuration information. Should setup properties.
        protected TargetConfigurationInformation()
        {
        }

        //! Returns the information for specific platform.
        //! @param TheConfiguration Desired configuration.
        public static TargetConfigurationInformation Get(TargetConfigurations TheConfiguration)
        {
            if (s_CachedInformation == null) {
                s_CachedInformation = new Dictionary<TargetConfigurations, TargetConfigurationInformation>();
                foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                    Type ConfigurationInformationType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(Configuration + "ConfigurationInformation"));
                    if (ConfigurationInformationType == null) {
                        throw new BuildSystemException("Not сonfiguration information exists for сonfiguration {0}.", Configuration);
                    }

                    TargetConfigurationInformation ConfigurationInformation = (TargetConfigurationInformation)Activator.CreateInstance(ConfigurationInformationType);
                    s_CachedInformation.Add(Configuration, ConfigurationInformation);
                }
            }

            return s_CachedInformation[TheConfiguration];
        }
    }   // class TargetConfigurationInformation

    //! List of properties for C++ compiler.
    public static class TargetCppCompilerInformation
    {
        public const int s_WarningLevel = 4;
        public const bool s_WarningsAreErrors = true;
    }   // class TargetCppCompilerInformation

    //! Represents an abstract data collector.
    public abstract class TargetCollector
    {
        internal string m_Source = null;
        private string m_Location;
        private string m_Name;

        //! Returns path to source file of this collector.
        public string GetSource()
        {
            Debug.Assert(m_Source != null);
            return m_Source;
        }

        //! Returns path directory of this collector.
        public string GetLocation()
        {
            if (m_Location == null) {
                m_Location = Path.GetDirectoryName(GetSource());
            }

            return m_Location;
        }

        //! Returns name of this collector.
        public string GetName()
        {
            if (m_Name == null) {
                m_Name = GetSource();
                while (!string.IsNullOrEmpty(Path.GetExtension(m_Name))) {
                    m_Name = Path.GetFileNameWithoutExtension(m_Name);
                }
            }

            return m_Name;
        }

        public virtual bool GetIsSupported()
        {
            return true;
        }
    }   // class TargetCollector

    //! Contains cached data of some abstract collector.
    //! Caching properties rules:
    //! @li Returns target/configuration independent result - just store.
    //! @li Returns target/configuration dependent result - avoid this.
    //! @li Returns target dependent result: store as platform-value dictionary.
    //! @li Returns configuration dependent result: store as delegate of configuration.
    public class TargetCollectorCache
    {
        public dynamic m_AdditionalCache;
        public readonly bool m_IsSupported;
        public readonly string m_CachedName;
        public readonly string m_CachedLocation;

        public TargetCollectorCache(TargetCollector BuildTargetCollectorObject)
        {
            m_IsSupported = BuildTargetCollectorObject.GetIsSupported();
            if (m_IsSupported) {
                m_AdditionalCache = new ExpandoObject();
                m_CachedName = BuildTargetCollectorObject.GetName();
                m_CachedLocation = BuildTargetCollectorObject.GetLocation();
            }
        }
    }   // class TargetCollectorCache

    //! Collects data and generates cache.
    //! @tparam TCollector Type of collector.
    //! @tparam TCache Type of cache of collector.
    public /*static*/ class TargetCollectorFactory<TCollector, TCache>
        where TCollector : TargetCollector
        where TCache : TargetCollectorCache
    {
        //! Constructs new collector instance and cached it data.
        //! @param CollectorSourcePath Path so source file of the collector.
        public static TCache Create(string CollectorSourcePath)
        {
            TCollector Collector = null;
            if (CollectorSourcePath != null) {
                foreach (Type InternalType in CSharpCompiler.CompileSourceFile(CollectorSourcePath).GetTypes()) {
                    if (InternalType.IsSubclassOf(typeof(TCollector))) {
                        Collector = (TCollector)Activator.CreateInstance(InternalType);
                        break;
                    }
                }
            }

            if (Collector == null) {
                Collector = Activator.CreateInstance<TCollector>();
            }

            Collector.m_Source = CollectorSourcePath;

            return (TCache)Activator.CreateInstance(typeof(TCache), Collector);
        }
    }   // class TargetCollectorFactory<TCollector, TCache>

    //! List of supported target make systems.
    public enum TargetProjectGenerators : byte
    {
        Unknown,
        VisualStudio,
        QtCreator,
        XCode,
    }   //enum TargetBuildSystem

    //! Represents a project generation algorithm interface.
    public abstract class TargetProjectGenerator
    {
        //! Checks if specified platform is natively supported by IDE.
        //! @param Platform Some platform.
        public virtual bool IsPlatformNativelySupported(TargetPlatforms Platform)
        {
            return false;
        }

        //! Generates project files for specified project.
        //! @param Project Parsed project object.
        //! @returns Path to main project file.
        public virtual string GenerateProjectFiles(ProjectCache Project)
        {
            string ProjectDirectoryPath = Path.Combine(Project.m_CachedLocation, "Build");
            Directory.CreateDirectory(ProjectDirectoryPath);

            return ProjectDirectoryPath;
        }

        //! Generates solution files for specified solution.
        //! @param Solution Parsed solution object.
        //! @returns Path to main project file.
        public virtual string GenerateSolutionFiles(SolutionCache Solution)
        {
            string SolutionDirectoryPath = Path.Combine(Solution.m_CachedLocation, "Build");
            Directory.CreateDirectory(SolutionDirectoryPath);

            return SolutionDirectoryPath;
        }
    }   // class TargetProjectGenerator

    //! Represents a factory of IDE project files generator.
    public static class TargetProjectGeneratorFactory
    {
        //! Constructs new IDE project files generator instance.
        public static TargetProjectGenerator Create()
        {
            TargetProjectGenerators ProjectGenerator;
            switch (Environment.OSVersion.Platform) {
                case PlatformID.Win32NT:
                    ProjectGenerator = TargetProjectGenerators.VisualStudio;
                    break;
                case PlatformID.MacOSX:
                    ProjectGenerator = TargetProjectGenerators.XCode;
                    break;
                case PlatformID.Unix:
                    ProjectGenerator = TargetProjectGenerators.QtCreator;
                    break;
                default:
                    throw new BuildSystemException("Unable to determine IDE for this platform");
            }

            Type ProjectGeneratorType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(ProjectGenerator + "ProjectGenerator"));
            if (ProjectGeneratorType == null) {
                throw new BuildSystemException("No generator for {0} IDE exists.", ProjectGenerator);
            }

            return (TargetProjectGenerator)Activator.CreateInstance(ProjectGeneratorType);
        }
    }   // class TargetProjectGeneratorFactory

    //! Represents a makefile generation algorithm interface.
    public abstract class TargetMakefileGenerator
    {
        //! Generates makefile for specified project.
        //! @param Project Parsed project object.
        //! @returns Path to generated makefile.
        public virtual string GenerateMakefile(ProjectCache Project)
        {
            string MakefilePath = Path.Combine(Project.m_CachedLocation, "Build", "Makefile-");
            return MakefilePath;
        }
    }   // class TargetMakefileGenerator

    //! Represents a factory of makefiles generator.
    public static class TargetMakefileGeneratorFactory
    {
        //! Constructs new makefile generator instance.
        public static TargetMakefileGenerator Create(TargetPlatforms Platform)
        {
            Type MakefileGeneratorType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(Tp => Tp.Name.EndsWith(Platform + "MakefileGenerator"));
            if (MakefileGeneratorType == null) {
                throw new BuildSystemException("No generator for {0}'s makefiles exists.", Platform);
            }

            return (TargetMakefileGenerator)Activator.CreateInstance(MakefileGeneratorType);
        }
    }   // class TargetMakefileGeneratorFactory

    //! Contains helper methods for targets.
    public static class Target
    {
        //! Enumerates all supported and implemented target platforms.
        public static IEnumerable<TargetPlatforms> EnumerateAllPlatforms()
        {
            switch (Environment.OSVersion.Platform) {
                case PlatformID.Win32NT:
                    yield return TargetPlatforms.Windows;
                    //yield return TargetPlatforms.WindowsRT;
                    //yield return TargetPlatforms.WindowsStore;
                    //yield return TargetPlatforms.WindowsPhone;
                    //yield return TargetPlatforms.XBoxOne;
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
            //yield return TargetPlatforms.Emscripten;
        }

        //! Enumerates all supported and implemented target configurations.
        public static IEnumerable<TargetConfigurations> EnumerateAllConfigurations()
        {
            yield return TargetConfigurations.Debug;
            yield return TargetConfigurations.Release;
            yield return TargetConfigurations.Development;
            yield return TargetConfigurations.Shipping;
        }

        //! Returns true if target platform is desktop.
        public static bool IsDesktopPlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.OSX:
                case TargetPlatforms.Linux:
                case TargetPlatforms.Windows:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is mobile.
        public static bool IsMobilePlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.iOS:
                case TargetPlatforms.Android:
                case TargetPlatforms.WindowsRT:
                case TargetPlatforms.WindowsStore:
                case TargetPlatforms.WindowsPhone:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is console.
        public static bool IsConsolePlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.XBoxOne:
                case TargetPlatforms.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if specified platform is Web
        public static bool IsWebPlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.Emscripten:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform`s native API is WinAPI.
        public static bool IsWinAPIPlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.XBoxOne:
                case TargetPlatforms.Windows:
                case TargetPlatforms.WindowsRT:
                case TargetPlatforms.WindowsStore:
                case TargetPlatforms.WindowsPhone:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform is POSIX compatible.
        public static bool IsPosixPlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.iOS:
                case TargetPlatforms.OSX:
                case TargetPlatforms.Linux:
                case TargetPlatforms.Android:
                case TargetPlatforms.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Returns true if target platform`s native API is Cocoa.
        public static bool IsCocoaPlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.iOS:
                case TargetPlatforms.OSX:
                    return true;
            }

            return false;
        }

        //! Returns true if object on specified path has platform/configuration data and matches it.
        public static bool MatchesPlatformConfiguration(string ObjectPath, TargetPlatforms Platform, TargetConfigurations Configuration)
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
    }   // class Target
}   // namespace GoddamnEngine.BuildSystem
