//! ==========================================================================================
//! Dependency.cs - project's dependency class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    //! Represents a single project's dependency.
    public class Dependency
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        private string m_Location = null;

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Constructors.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a new dependency.
        public Dependency() { }
        public Dependency(string Location)
        {
            m_Location = Location;
        }

        //! @}

        //! Returns the location of the dependency directory/source file.
        public string GetLocation()
        {
            Debug.Assert(m_Location != null);

            return m_Location;
        }

        //! ------------------------------------------------------------------------------------------
        //! @name Properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! If is false, then this dependency is not added to project's dependencies list.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual bool GetIsSkipped(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return false;
        }

        //! Collects list of paths to directories that should be included into project's include paths list.
        //! By default, it is the subdirectory 'include' of dependency directory.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual List<string> GetHeaderPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> HeaderPaths = new List<string>();

            string StandartIncludePath = Path.Combine(this.GetLocation(), "include");
            if (Directory.Exists(StandartIncludePath)) {
                HeaderPaths.Add(StandartIncludePath);
            }

            return HeaderPaths;
        }

        //! Collects list of files that should be copied to project build output directory.
        //! By default, this files are:
        //! @li .DLL files on Windows desktop platform.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual List<string> GetCopyFilesPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> CopyFilesPaths = new List<string>();

            if (Target.IsDesktopPlatform(Platform)) {
                foreach (string DependencyFile in Directory.EnumerateFiles(this.GetLocation())) {
                    string DependencyFileExtension = Path.GetExtension(DependencyFile).ToLower();
                    switch (Platform) {
                        case TargetPlatform.Windows:
                            if (DependencyFileExtension == ".dll") {
                                CopyFilesPaths.Add(DependencyFile);
                            }
                            break;
                    }
                }
            }

            return CopyFilesPaths;
        }

        //! Collects list of paths to libraries that should be linked with project build file.
        //! By default, they are the files in dependency directory that match following pattern: "SomeName[.{Platform}[.{Configuration}]].{Extension}", where:
        //! @li Platform - Name of the platform, for which current library source is built. Should be equal to target one. 
        //! @li Configuration - Name of the configuration, for which current library source is built. Should be equal to target one. 
        //! @li Extension - Extension specific to target platform. E.g. ".lib" for Windows. 
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual List<string> GetLibrariesPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> LibrariesPaths = new List<string>();

            foreach (string LibraryFile in Directory.EnumerateFiles(this.GetLocation())) {
                bool IsLibraryFile = false;
                string LibraryFileExtension = Path.GetExtension(LibraryFile).ToLower();
                if (Target.IsWebPlatform(Platform)) {
                    IsLibraryFile = (LibraryFileExtension == ".bc");
                } else if (Target.IsWinAPIPlatform(Platform)) {
                    IsLibraryFile = (LibraryFileExtension == ".lib");
                } else if (Target.IsPosixPlatform(Platform)) {
                    IsLibraryFile = (LibraryFileExtension == ".a") || (LibraryFileExtension == ".so");
                } else {
                    throw new NotImplementedException();
                }

                if (IsLibraryFile && Target.MatchesPlatformConfiguration(LibraryFile, Platform, Configuration)) {
                    LibrariesPaths.Add(LibraryFile);
                }
            }

            // On Apple platforms we also have '.framework' directories that are treated as libraries.
            if (Target.IsCocoaPlatform(Platform)) {
                foreach (string FrameworkFile in Directory.EnumerateDirectories(this.GetLocation())) {
                    if (Path.GetExtension(FrameworkFile).ToLower() == ".framework") {
                        //  LibrariesPaths.Add(FrameworkFile);
                        throw new NotImplementedException("Using frameworks is not implemented.");
                    }
                }
            }

            return LibrariesPaths;
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Class static API.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Creates a dependency object for specified location.
        //! Dependency-configuration file should be located in the directory. It should be an .GDDEP.CS file: 
        //! empty or with Dependency-derived class.
        //! @param DependencyLocation Path do directory in which dependencies would be searched.
        internal static Dependency CreateDependencyForSource(string DependencyFile)
        {
            Dependency DependencyObject = CSharpCompiler.InstantiateSourceFile<Dependency>(DependencyFile);
            DependencyObject.m_Location = DependencyFile;

            return DependencyObject;
        }

        //! @}
    }   // class Dependency
}   // namespace GoddamnEngine.BuildSystem

// GoddamnSDK specific dependencies code.
#if INCLUDE_GODDAMNSDK_SPECIFIC
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
        public sealed override List<string> GetHeaderPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> HeaderPaths = new List<string>();
            string CoreHeadersPath = Path.Combine(BuildSystem.GetSDKLocation(), "source");
            if (Directory.Exists(CoreHeadersPath)) {
                HeaderPaths.Add(CoreHeadersPath);
            } else {
                throw new BuildSystemException("Headers path for GoddamnSDK where not found. Was it installed correctly?");
            }

            return HeaderPaths;
        }

        //! Does nothing. All internally dependent file would be automatically installed during the build of SDK projects.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
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
        public sealed override List<string> GetLibrariesPaths(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<string> LibrariesPaths = new List<string>();

            string SDKLibrariesLocation = Path.Combine(BuildSystem.GetSDKLocation(), "lib");
            string SDKLibraryPattern = m_ComponentName.ToLower();
            if (Configuration != TargetConfiguration.Release) {
                SDKLibraryPattern += string.Concat('.', Configuration).ToLower();
            }

            // Installing "file"-library dependencies.
            foreach (string SDKLibrary in Directory.EnumerateFiles(SDKLibrariesLocation)) {
                if (Path.GetFileName(SDKLibrary.ToLower()) == SDKLibraryPattern) {
                    string SDKLibraryExtension = Path.GetExtension(SDKLibrary).ToLower();
                    if (Target.IsWebPlatform(Platform)) {
                        if (SDKLibraryExtension == ".bc") {
                            LibrariesPaths.Add(SDKLibrary);
                        }
                    } else if (Target.IsWinAPIPlatform(Platform)) {
                        if (SDKLibraryExtension == ".lib") {
                            LibrariesPaths.Add(SDKLibrary);
                        }
                    } else if (Target.IsPosixPlatform(Platform)) {
                        if ((SDKLibraryExtension == ".a") || (SDKLibraryExtension == ".so")) {
                            LibrariesPaths.Add(SDKLibrary);
                        }
                    } else {
                        throw new NotImplementedException();
                    }
                }
            }

            // On Apple platforms we also have '.framework' directories that are treated as libraries.
            if (Target.IsCocoaPlatform(Platform)) {
                foreach (string SDKFramework in Directory.EnumerateDirectories(SDKLibrariesLocation)) {
                    if (Path.GetFileName(SDKFramework.ToLower()) == SDKLibraryPattern) {
                        if (Path.GetExtension(SDKFramework).ToLower() == ".framework") {
                            //  LibrariesPaths.Add(FrameworkFile);
                            throw new NotImplementedException("Using frameworks is not implemented.");
                        }
                    }
                }
            }

            return LibrariesPaths;
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
