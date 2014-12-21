//! ==========================================================================================
//! Dependency.cs - project's dependency class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace GoddamnEngine.BuildSystem
{
    //! Represents a single project's dependency.
    public class Dependency : TargetCollector
    {
        //! Collects list of directories that contain header files.
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            string StandartIncludePath = Path.Combine(GetLocation(), "include");
            if (Directory.Exists(StandartIncludePath)) {
                yield return StandartIncludePath;
            } else {
                throw new BuildSystemException("No include directories for dependencies {0} were found.", GetName());
            }
        }

        //! Collects list of files that should be copied to project build output directory.
        //! @param Platform One of the target platforms.
        public virtual IEnumerable<string> EnumerateCopyFiles(TargetPlatforms Platform)
        {
            Debug.Assert(Platform != TargetPlatforms.Unknown);
            if (Target.IsDesktopPlatform(Platform)) {
                foreach (string CopyFile in Directory.EnumerateFiles(GetLocation())) {
                    string CopyFileExtension = Path.GetExtension(CopyFile).ToLowerInvariant();
                    if (Target.IsWinAPIPlatform(Platform)) {
                        if (CopyFileExtension == ".dll") {
                            yield return CopyFile;
                        }
                    }
                }
            }
        }

        //! Collects list of libraries that should be linked with project build file.
        //! @param Platform One of the target platforms.
        public virtual IEnumerable<string> EnumerateLinkedLibraries(TargetPlatforms Platform)
        {
            Debug.Assert(Platform != TargetPlatforms.Unknown);
            foreach (string LibraryFile in Directory.EnumerateFiles(this.GetLocation())) {
                string LibraryFileExtension = Path.GetExtension(LibraryFile).ToLowerInvariant();
                if (Target.IsWebPlatform(Platform)) {
                    if (LibraryFileExtension == ".bc") {
                        yield return LibraryFile;
                    }
                } else if (Target.IsWinAPIPlatform(Platform)) {
                    if (LibraryFileExtension == ".lib") {
                        yield return LibraryFile;
                    }
                } else if (Target.IsPosixPlatform(Platform)) {
                    if ((LibraryFileExtension == ".a") || (LibraryFileExtension == ".so")) {
                        yield return LibraryFile;
                    }
                } else {
                    throw new NotImplementedException();
                }
            }

            if (Target.IsCocoaPlatform(Platform)) {
                foreach (string FrameworkPackage in Directory.EnumerateDirectories(this.GetLocation(), "*.framework")) {
                    yield return FrameworkPackage;
                }
            }
        }
    }   // class Dependency

    //! Represents a collection of cached data that was by dependency object.
    public sealed class DependencyCache : TargetCollectorCache
    {
        public readonly string[] m_CachedHeaderDirectories;
        public readonly Dictionary<TargetPlatforms, string[]> m_CachedCopyFiles;
        public readonly Dictionary<TargetPlatforms, string[]> m_CachedLinkedLibraries;

        //! Generates cache for specified dependency.
        public DependencyCache(Dependency DependencyObject)
            : base(DependencyObject)
        {
            if (m_IsSupported) {
                m_CachedHeaderDirectories = DependencyObject.EnumerateHeaderDirectories().ToArray();
                m_CachedCopyFiles = new Dictionary<TargetPlatforms, string[]>();
                m_CachedLinkedLibraries = new Dictionary<TargetPlatforms, string[]>();
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    m_CachedCopyFiles.Add(Platform, DependencyObject.EnumerateCopyFiles(Platform).ToArray());
                    m_CachedLinkedLibraries.Add(Platform, DependencyObject.EnumerateCopyFiles(Platform).ToArray());
                }
            }
        }
    }   // class DependencyCache

    //! Represents a factory of dependencies.
    public /*static*/ sealed class DependencyFactory : TargetCollectorFactory<Dependency, DependencyCache>
    {
    }   // class DependencyFactory
}   // namespace GoddamnEngine.BuildSystem

// GoddamnSDK specific dependencies code.
#if INCLUDE_GODDAMNSDK_SPECIFIC && FALSE
namespace GoddamnEngine.BuildSystem.GoddamnSpecific
{
    //! <summary>
    //! Represents a dependency of some GoddamnSDK library.
    //! </summary>
    public class GoddamnDependency : Dependency
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        private string m_ComponentName = null;

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Constructor.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a GoddamnSDK dependency. Location is set to default SDK location.
        internal GoddamnDependency(string ComponentName)
            : base(BuildSystem.GetSDKLocation())
        {
            m_ComponentName = ComponentName;
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Adds standard GoddamnSDK headers location into header paths.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        //! @note Result of this function is recommended to be cached.
        public sealed override List<string> GetHeaderPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> DependencyHeaderPaths = new List<string>();
            string DependencyCoreHeadersPath = Path.Combine(BuildSystem.GetSDKLocation(), "source");
            if (Directory.Exists(DependencyCoreHeadersPath)) {
                DependencyHeaderPaths.Add(DependencyCoreHeadersPath);
            } else {
                throw new BuildSystemException("Headers path for GoddamnSDK where not found. Was it installed correctly?");
            }

            return DependencyHeaderPaths;
        }

        //! Does nothing. All internally dependent file would be automatically installed during the build of SDK projects.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        //! @note Result of this function is recommended to be cached.
        public sealed override List<string> GetCopyFilesPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return new List<string>();
        }

        //! Adds standard GoddamnSDK libraries (and frameworks) locations into additional libraries paths.
        //! @param LibrariesPaths Output for libraries paths.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        //! @note Result of this function is recommended to be cached.
        public sealed override List<string> GetLibrariesPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> DependencyLibrariesPaths = new List<string>();

            string DependencySDKLibrariesLocation = Path.Combine(BuildSystem.GetSDKLocation(), "lib");
            string DependencySDKLibraryPattern = m_ComponentName.ToLowerInvariant();
            if (Configuration != TargetConfiguration.Release) {
                DependencySDKLibraryPattern += string.Concat('.', Configuration).ToLowerInvariant();
            }

            // Installing "file"-library dependencies.
            foreach (string DependencySDKLibrary in Directory.EnumerateFiles(DependencySDKLibrariesLocation)) {
                if (Path.GetFileName(DependencySDKLibrary.ToLowerInvariant()) == DependencySDKLibraryPattern) {
                    string SDKLibraryExtension = Path.GetExtension(DependencySDKLibrary).ToLowerInvariant();
                    if (Target.IsWebPlatform(Platform)) {
                        if (SDKLibraryExtension == ".bc") {
                            DependencyLibrariesPaths.Add(DependencySDKLibrary);
                        }
                    } else if (Target.IsWinAPIPlatform(Platform)) {
                        if (SDKLibraryExtension == ".lib") {
                            DependencyLibrariesPaths.Add(DependencySDKLibrary);
                        }
                    } else if (Target.IsPosixPlatform(Platform)) {
                        if ((SDKLibraryExtension == ".a") || (SDKLibraryExtension == ".so")) {
                            DependencyLibrariesPaths.Add(DependencySDKLibrary);
                        }
                    } else {
                        throw new NotImplementedException();
                    }
                }
            }

            // On Apple platforms we also have '.framework' directories that are treated as libraries.
            if (Target.IsCocoaPlatform(Platform)) {
                foreach (string DependencySDKFramework in Directory.EnumerateDirectories(DependencySDKLibrariesLocation)) {
                    if (Path.GetFileName(DependencySDKFramework.ToLowerInvariant()) == DependencySDKLibraryPattern) {
                        if (Path.GetExtension(DependencySDKFramework).ToLowerInvariant() == ".framework") {
                            //  LibrariesPaths.Add(FrameworkFile);
                            throw new NotImplementedException("Using frameworks is not implemented.");
                        }
                    }
                }
            }

            return DependencyLibrariesPaths;
        }
        //! @}
    }   // class GoddamnDependency

    //! Represents a SDK dependency, which should be linked and cannot be build as plugin.
    //! This means, that this dependency would be never skipped on any platform.
    public class GoddamnNotPluginDependency : Dependency
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Constructor.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a GoddamnSDK dependency. Location is set to default SDK location.
        internal GoddamnNotPluginDependency(string ComponentName) : base(ComponentName) { }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Returns false on all platforms.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public sealed override bool GetIsSkipped(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return false;
        }

        //! @}
    }   // class GoddamnNotPluginDependency

    //! Represents dependency on GoddamnCore library.
    public sealed class GoddamnCoreDependency : GoddamnNotPluginDependency
    {
        //! Instantiates a new GoddamnCore library dependency.
        public GoddamnCoreDependency() : base("GoddamnCore") { }
    }   // class GoddamnCoreDependency

    //! Represents dependency on GoddamnEngine library.
    public sealed class GoddamnEngineDependency : GoddamnNotPluginDependency
    {
        //! Instantiates a new GoddamnEngine library dependency.
        public GoddamnEngineDependency() : base("GoddamnEngine") { }
    }   // class EngineProjectDependency
}   // namespace GoddamnEngine.BuildSystem.GoddamnSpecific
#endif  // INCLUDE_GODDAMNSDK_SPECIFIC

// Some additional dependencies code.
#if INCLUDE_ADDITIONALS
namespace GoddamnEngine.BuildSystem.Additinals
{
    //! Represents a dependency in NVidia PhysX (C) engine.
    public sealed class NVidiaPhysXDependency : Dependency
    {
        private const string s_X64DefaultPath = @"C:\Program Files (x86)\NVIDIA Corporation\PhysX";
    }   // class NVidiaPhysXDependency
}   // namespace GoddamnEngine.BuildSystem.Additinals
#endif  // if INCLUDE_ADDITIONALS
