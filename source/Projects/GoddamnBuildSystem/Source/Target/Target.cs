//! ==========================================================================================
//! Target.cs - Build target class.
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
    }   // enum TargetPlatform

    //! List of supported target make systems.
    public enum TargetIDE : byte
    {
        Unknown,
        VisualStudio,
        QtCreator,
        XCode,
    }   //enum TargetBuildSystem

    //! List of supported target configurations.
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Release,
        Development,
        Shipping,
    }   // enum Configuration

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

    //! Represents a project generation algorithm interface.
    public abstract class TargetProjectGenerator
    {
        //! Checks if specified platform is natively supported by IDE.
        //! @param Platform Some platform.
        public virtual bool IsPlatformNativelySupported(TargetPlatform Platform)
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
            TargetIDE IDEGeneratorType;
            switch (Environment.OSVersion.Platform) {
                case PlatformID.Win32NT:
                    IDEGeneratorType = TargetIDE.VisualStudio;
                    break;
                case PlatformID.MacOSX:
                    IDEGeneratorType = TargetIDE.XCode;
                    break;
                case PlatformID.Unix:
                    IDEGeneratorType = TargetIDE.QtCreator;
                    break;
                default:
                    throw new BuildSystemException("Unable to determine IDE for this platform");
            }

            Type IDEType = Assembly.GetExecutingAssembly().GetTypes().Where(Tp => Tp.Name.EndsWith(string.Concat(IDEGeneratorType, "ProjectGenerator"))).FirstOrDefault();
            if (IDEType != null) {
                return (TargetProjectGenerator)Activator.CreateInstance(IDEType);
            } else {
                throw new BuildSystemException("No generator for {0} IDE exists.", IDEGeneratorType);
            }
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
        public static TargetMakefileGenerator Create(TargetPlatform Platform)
        {
            Type MakefileGeneratorType = Assembly.GetExecutingAssembly().GetTypes().Where(Tp => Tp.Name.EndsWith(string.Concat(Platform, "MakefileGenerator"))).FirstOrDefault();
            if (MakefileGeneratorType != null) {
                return (TargetMakefileGenerator)Activator.CreateInstance(MakefileGeneratorType);
            } else {
                throw new BuildSystemException("No generator for {0}'s makefiles exists.", Platform);
            }
        }
    }   // class TargetMakefileGeneratorFactory

    //! Contains helper methods for targets.
    public static class Target
    {
        //! Enumerates all supported and implemented target platforms.
        public static IEnumerable<TargetPlatform> EnumerateAllPlatforms()
        {
            yield return TargetPlatform.Windows;
            //yield return TargetPlatform.WindowsRT;
            //yield return TargetPlatform.WindowsStore;
            //yield return TargetPlatform.WindowsPhone;
            //yield return TargetPlatform.XBoxOne;
            //yield return TargetPlatform.PlayStation4;
            //yield return TargetPlatform.OSX;
            //yield return TargetPlatform.iOS;
            //yield return TargetPlatform.Linux;
            //yield return TargetPlatform.Android;
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
            switch (Platform) {
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
            switch (Platform) {
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

        //! Returns true if target platform`s native API is WinAPI.
        public static bool IsWinAPIPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.XBoxOne:
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

        //! Returns true if target platform`s native API is Cocoa.
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
    }   // class Target
}   // namespace GoddamnEngine.BuildSystem
