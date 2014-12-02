//! ==========================================================================================
//! Dependency.cs - project's dependency class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 24.11.2014 - Created by James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Diagnostics;
using System.Collections.Specialized;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Represents a single project's dependency.
    /// </summary>
    public class Dependency
    {
        //! ------------------------------------------------------------------------------------------
        //! Fields.
        //! ------------------------------------------------------------------------------------------

        private string m_Location = null;

        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a new dependency.
        /// </summary>
        public Dependency() { }
        public Dependency(string Location) { m_Location = Location; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Returns the location of the dependency directory/source file.
        /// </summary>
        public string GetLocation()
        {
            Debug.Assert(m_Location != null);

            return m_Location;
        }

        /// <summary>
        /// If is false, then this dependency is not added to project's dependencies list.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual bool GetIsSkipped(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return false;
        }

        /// <summary>
        /// Returns list of paths to direcories that should be included into project's include paths list.
        /// By default, it is the subdirectory 'include' of dependency directory.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetHeaderPaths(ref StringCollection HeaderPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string StandartIncludePath = Path.Combine(this.GetLocation(), "include");
            if (Directory.Exists(StandartIncludePath)) {
                HeaderPaths.Add(StandartIncludePath);
            }
        }

        /// <summary>
        /// Returns list of files that should be copied to project build output directory.
        /// By default, this files are:
        /// <list type="bullet">
        ///     <item>
        ///         <description>.DLL files on Windows desktop platform.</description>
        ///     </item>
        /// </list>
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetCopyFilesPaths(ref StringCollection CopyFilesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            if (Target.IsDesktopPlatform(Platform)) {
                foreach (string DependencyFile in Directory.EnumerateFiles(this.GetLocation())) {
                    string DependencyFileExtension = Path.GetExtension(DependencyFile).ToLower();
                    switch (Platform) {
                        case TargetPlatform.Windows:
                            switch (DependencyFileExtension) {
                                case ".dll":
                                    CopyFilesPaths.Add(DependencyFile);
                                    break;
                            }
                            break;
                    }
                }
            } 
        }

        /// <summary>
        /// Returns list of paths to libraries that should be linked with project build file.
        /// By default, they are the files in dependency directory that match following pattern: "SomeName[.{Platform}[.{Configuration}]].{Extension}", where:
        /// <list type="bullet">
        ///     <item><term>Platform</term><description>Name of the platform, for which current library source is built. Should be equal to target one.</description></item>
        ///     <item><term>Configuration</term><description>Name of the configuration, for which current library source is built. Should be equal to target one.</description></item>
        ///     <item><term>Extension</term><description>Extension specific to target platform. E.g. ".lib" for Windows.</description></item>
        /// </list>
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetLibrariesPaths(ref StringCollection LibrariesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            foreach (string LibraryFile in Directory.EnumerateFiles(this.GetLocation())) {
                bool IsLibraryFile = false;
                string LibraryFileExtension = Path.GetExtension(LibraryFile).ToLower();
                if (Target.IsWebPlatform(Platform)) { 
                    switch (LibraryFileExtension) {
                        case ".bc":
                            IsLibraryFile = true;
                            break;
                    }
                } else if (Target.IsWinAPIPlatform(Platform)) {
                    switch (LibraryFileExtension) {
                        case ".lib":
                            IsLibraryFile = true;
                            break;
                    }
                } else if (Target.IsPosixPlatform(Platform)) {
                    switch (LibraryFileExtension) {
                        case ".a":
                        case ".so":
                            IsLibraryFile = true;
                            break;
                    }
                } else {
                    throw new NotImplementedException();
                }

                if (IsLibraryFile) {
                    string LibraryFileExtensionless = Path.GetFileNameWithoutExtension(LibraryFile);
                    string LibraryPlatform = Path.GetExtension(LibraryFileExtensionless);
                    if (!string.IsNullOrEmpty(LibraryPlatform)) {
                        // Checking if platform information is defined and it is of type TargetPlatform.
                        if (LibraryPlatform == string.Concat('.', Platform)) {
                            string LibraryConfiguration = Path.GetExtension(Path.GetFileNameWithoutExtension(LibraryFileExtensionless));
                            if (!string.IsNullOrEmpty(LibraryConfiguration)) {
                                // Checking if platform information is defined and it is of type TargetConfiguration.
                                IsLibraryFile = (LibraryConfiguration == string.Concat('.', Configuration));
                            }
                        } else {
                            // Library filename contains some platform information, but it not matches target platform.
                            IsLibraryFile = false;
                        }
                    }
                }

                if (IsLibraryFile) {
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
        }

        //! ------------------------------------------------------------------------------------------
        //! Class static API.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Creates a depenedency object for specified location.
        /// Dependency-configuration file should be located in the directory. It should be an .GDDEP.CS file: 
        /// empty or with Dependency-derived class.
        /// </summary>
        /// <param name="DependencyLocation">Path do directory in which dependencies would be searched.</param>
        internal static Dependency CreateDependencyForDirectory(string DependencyFile)
        {
            Dependency DependencyObject = CSharpCompiler.InstantiateSourceFile<Dependency>(DependencyFile);
            DependencyObject.m_Location = DependencyFile;

            return DependencyObject;
        }
    }   // class Dependency

// GoddamnSDK specific dependencies code.
#if INCLUDE_GODDAMNSDK_SPECIFIC

    /// <summary>
    /// Represents a dependency of some GoddamnSDK libary.
    /// </summary>
    public class GoddamnDependency : Dependency
    {
        //! ------------------------------------------------------------------------------------------
        //! Fields.
        //! ------------------------------------------------------------------------------------------

        private string m_ComponentName = null;

        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a GoddamnSDK dependency. Location is set to default SDK location.
        /// </summary>
        internal GoddamnDependency(string ComponentName) : base(BuildSystem.GetSDKLocation()) { m_ComponentName = ComponentName; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Adds standart GoddamnSDK headers location into header paths.
        /// </summary>
        /// <param name="HeaderPaths">Output for header paths.</param>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public sealed override void GetHeaderPaths(ref StringCollection HeaderPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string CoreHeadersPath = Path.Combine(BuildSystem.GetSDKLocation(), "source");
            if (Directory.Exists(CoreHeadersPath)) {
                HeaderPaths.Add(CoreHeadersPath);
            } else {
                throw new BuildSystemException("Headers path for GoddamnSDK where not found. Was it installed correctly?");
            }
        }

        /// <summary>
        /// Does nothing. All internally dependent file would be automatically installed during the build of SDK projects.
        /// </summary>
        /// <param name="CopyFilesPaths">Output for copied files.</param>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public sealed override void GetCopyFilesPaths(ref StringCollection CopyFilesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            // Yes, really nothing here.
        }

        /// <summary>
        /// Adds standart GoddamnSDK libaries (and frameworks) locations into additional libaries paths.
        /// </summary>
        /// <param name="LibrariesPaths">Output for libraries paths.</param>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public sealed override void GetLibrariesPaths(ref StringCollection LibrariesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

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
                        switch (SDKLibraryExtension) {
                            case ".bc":
                                LibrariesPaths.Add(SDKLibrary);
                                break;
                        }
                    } else if (Target.IsWinAPIPlatform(Platform)) {
                        switch (SDKLibraryExtension) {
                            case ".lib":
                                LibrariesPaths.Add(SDKLibrary);
                                break;
                        }
                    } else if (Target.IsPosixPlatform(Platform)) {
                        switch (SDKLibraryExtension) {
                            case ".a":
                            case ".so":
                                LibrariesPaths.Add(SDKLibrary);
                                break;
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
        }
    }   // class GoddamnDependency

    /// <summary>
    /// Represents a SDK dependency, which should be linked and cannot be build as plugin.
    /// This means, that this dependency would be never skipped on any platform.
    /// </summary>
    public class GoddamnNotPluginDependency : Dependency
    {
        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a GoddamnSDK dependency. Location is set to default SDK location.
        /// </summary>
        internal GoddamnNotPluginDependency(string ComponentName) : base(ComponentName) { }

        /// <summary>
        /// Returns false on all platforms.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public sealed override bool GetIsSkipped(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return false;
        }
    }   // class GoddamnNotPluginDependency

    /// <summary>
    /// Represents dependency on GoddamnCore library.
    /// </summary>
    public sealed class GoddamnCoreDependency : GoddamnNotPluginDependency
    {
        /// <summary>
        /// Instantiates a new GoddamnCore library dependency.
        /// </summary>
        public GoddamnCoreDependency() : base("GoddamnCore") { }
    }   // class GoddamnCoreDependency

    /// <summary>
    /// Represents dependency on GoddamnEngine library.
    /// </summary>
    public sealed class GoddamnEngineDependency : GoddamnNotPluginDependency
    {
        /// <summary>
        /// Instantiates a new GoddamnEngine library dependency.
        /// </summary>
        public GoddamnEngineDependency() : base("GoddamnEngine") { }
    }   // class EngineProjectDependency
#endif  // if INCLUDE_GODDAMNSDK_SPECIFIC

    // Some additional dependencies code.
#if INCLUDE_ADDITIONALS && FALSE

    /// <summary>
    /// Represents a dependency in NVidia PhysX (C) engine.
    /// </summary>
    public sealed class NVidiaPhysXDependency : Dependency
    {
        private const string s_X64DefaultPath = @"C:\Program Files (x86)\NVIDIA Corporation\PhysX";
    }   // class NVidiaPhysXDependency
#endif  // if INCLUDE_ADDITIONALS
}   // namespace GoddamnEngine.BuildSystem
