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
using System.Collections.Generic;
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
        /// <param name="Location">Location, where the dependency is located.</param>
        protected Dependency(string Location = null) { m_Location = Location; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Returns the location of the dependency directory/source file.
        /// </summary>
        public string GetLocation()
        {
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

            string StandartIncludePath = Path.Combine(m_Location, "include");
            if (Directory.Exists(StandartIncludePath)) {
                HeaderPaths.Add(StandartIncludePath);
            }
        }

        /// <summary>
        /// Returns list of files that should be copied to project build output directory.
        /// By default, this files are:
        /// <list>
        ///     <item>.DLL files on Windows desktop platform.</item>
        /// </list>
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetCopyFilesPaths(ref StringCollection CopyFilesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            if (Target.IsDesktopPlatform(Platform)) {
                foreach (string DependencyFile in Directory.EnumerateFiles(m_Location)) {
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
        /// <list>
        ///     <item>Platform - Name of the platform, for which current library source is built. Should be equal to target one.</item>
        ///     <item>Configuration - Name of the configuration, for which current library source is built. Should be equal to target one.</item>
        ///     <item>Extension - Extension specific to target platform. E.g. ".lib" for Windows.</item>
        /// </list>
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetLibrariesPaths(ref StringCollection LibrariesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            foreach (string LibraryFile in Directory.EnumerateFiles(m_Location)) {
                bool IsLibraryFile = false;
                string LibraryFileExtension = Path.GetExtension(LibraryFile).ToLower();
                switch (Platform) {
                    case TargetPlatform.Emscripten:
                        switch (LibraryFileExtension) {
                            case ".bc":
                                IsLibraryFile = true;
                                break;
                        }
                        break;

                    case TargetPlatform.XBoxOne:
                    case TargetPlatform.Windows:
                    case TargetPlatform.WindowsPhone8:
                    case TargetPlatform.WindowsStore:
                        switch (LibraryFileExtension) {
                            case ".lib":
                                IsLibraryFile = true;
                                break;
                        }
                        break;

                    case TargetPlatform.iOS:
                    case TargetPlatform.OSX:
                    case TargetPlatform.Linux:
                    case TargetPlatform.Android:
                    case TargetPlatform.PlayStation4:
                        switch (LibraryFileExtension) {
                            case ".a":
                            case ".so":
                                IsLibraryFile = true;
                                break;
                        }
                        break;

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
            if ((Platform == TargetPlatform.iOS) || (Platform == TargetPlatform.OSX)) { 
                foreach (string FrameworkFile in Directory.EnumerateDirectories(m_Location)) {
                    if (Path.GetExtension(FrameworkFile).ToLower() == ".framework") {
                    //  m_CopyFilesPaths.Add(FrameworkFile);
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
        /// If any dependency-configuration files are located in directory, they are parsed.
        /// Otherwise, the whole directory is parsed as single depenedency.
        /// </summary>
        /// <param name="Dependencies">Output list of dependencies.</param>
        /// <param name="DependencyLocation">Path do directory in which dependencies would be searched. NOT RECURSIVELY!</param>
        internal static void CreateDependenciesForDirectory(ref List<Dependency> Dependencies, string DependencyLocation)
        {
            bool WereNoDependeciesResolved = true;
            foreach (string DependencyFile in Directory.EnumerateFiles(DependencyLocation)) {
                if (Path.GetExtension(DependencyFile).ToLower() == ".cs") {
                    if (Path.GetExtension(Path.GetFileNameWithoutExtension(DependencyFile)).ToLower() == ".gddep") {
                        foreach (Type DependencyType in CSharpCompiler.CompileSourceFile(DependencyFile).GetInstancableTypesDerivedFrom<Dependency>()) {
                            Dependency DependencyObject = (Dependency)Activator.CreateInstance(DependencyType);
                            DependencyObject.m_Location = DependencyFile;
                            Dependencies.Add(DependencyObject);

                            WereNoDependeciesResolved = false;
                        }
                    }
                }
            }

            if (WereNoDependeciesResolved) {
                Dependencies.Add(new Dependency(DependencyLocation));
            }
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
        protected GoddamnDependency(string ComponentName) : base(BuildSystem.SDKPath) { m_ComponentName = ComponentName; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Adds standart GoddamnSDK headers location into header paths.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public sealed override void GetHeaderPaths(ref StringCollection HeaderPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string CoreHeadersPath = Path.Combine(BuildSystem.SDKPath, "source");
            if (Directory.Exists(CoreHeadersPath)) {
                HeaderPaths.Add(CoreHeadersPath);
            } else {
                throw new BuildSystemException("Headers path for GoddamnSDK where not found. Was it installed correctly?");
            }
        }

        /// <summary>
        /// Does nothing. All internally dependent file would be automatically installed dureing the build of SDK projects.
        /// </summary>
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
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public sealed override void GetLibrariesPaths(ref StringCollection LibrariesPaths, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string SDKLibrariesLocation = Path.Combine(BuildSystem.SDKPath, "lib");
            string SDKLibraryPattern = m_ComponentName.ToLower();
            if (Configuration != TargetConfiguration.Release) {
                SDKLibraryPattern += string.Concat('.', Configuration).ToLower();
            }

            // Installing "file"-library dependencies.
            foreach (string SDKLibrary in Directory.EnumerateFiles(SDKLibrariesLocation)) {
                if (Path.GetFileName(SDKLibrary.ToLower()) == SDKLibraryPattern) {
                    string SDKLibraryExtension = Path.GetExtension(SDKLibrary).ToLower();
                    switch (Platform) {
                        case TargetPlatform.Emscripten:
                            switch (SDKLibraryExtension) {
                                case ".bc":
                                    LibrariesPaths.Add(SDKLibrary);
                                    break;
                            }
                            break;

                        case TargetPlatform.XBoxOne:
                        case TargetPlatform.Windows:
                        case TargetPlatform.WindowsStore:
                        case TargetPlatform.WindowsRT:
                        case TargetPlatform.WindowsPhone8:
                            switch (SDKLibraryExtension) {
                                case ".lib":
                                    LibrariesPaths.Add(SDKLibrary);
                                    break;
                            }
                            break;

                        case TargetPlatform.iOS:
                        case TargetPlatform.OSX:
                        case TargetPlatform.Linux:
                        case TargetPlatform.Android:
                        case TargetPlatform.PlayStation4:
                            switch (SDKLibraryExtension) {
                                case ".a":
                                case ".so":
                                    LibrariesPaths.Add(SDKLibrary);
                                    break;
                            }
                            break;

                        default:
                            throw new NotImplementedException();
                        }
                }
            }

            // On Apple platforms we also have '.framework' directories that are treated as libraries.
            if ((Platform == TargetPlatform.iOS) || (Platform == TargetPlatform.OSX)) { 
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
        protected GoddamnNotPluginDependency(string ComponentName) : base(ComponentName) { }

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
        public GoddamnCoreDependency() : base("GoddamnCore") { }
    }   // class GoddamnCoreDependency

    /// <summary>
    /// Represents dependency on GoddamnEngine library.
    /// </summary>
    public sealed class GoddamnEngineDependency : GoddamnNotPluginDependency
    {
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
