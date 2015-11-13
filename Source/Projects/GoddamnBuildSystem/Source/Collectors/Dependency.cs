// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! Dependency.cs
//! Project's dependency class.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! Represents a single project's dependency.
    public class Dependency : Collector
    {
        //! Collects list of directories that contain header files.
        //! @returns Iterator for list of header files.
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            var standartIncludePath = Path.Combine(GetLocation(), "include");
            if (Directory.Exists(standartIncludePath))
            {
                yield return standartIncludePath;
            }
            else
            {
                throw new BuildSystemException("No include directories for dependencies {0} were found.", GetName());
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of files that should be copied to project build output directory.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Iterator for list of files that should be copied to project build output directory.
        public virtual IEnumerable<string> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            if (Target.IsDesktopPlatform(platform))
            {
                foreach (var copyFile in Directory.EnumerateFiles(GetLocation()))
                {
                    var copyFileExtension = (Path.GetExtension(copyFile) ?? "").ToLowerInvariant();
                    if (Target.IsWinApiPlatform(platform))
                    {
                        if (copyFileExtension == ".dll")
                        {
                            yield return copyFile;
                        }
                    }
                }
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of libraries that should be linked with project build file.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Iterator for list of libraries that should be linked with project build file.
        public virtual IEnumerable<string> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            foreach (var libraryFile in Directory.EnumerateFiles(GetLocation()))
            {
                var libraryFileExtension = (Path.GetExtension(libraryFile) ?? "").ToLowerInvariant();
                if (Target.IsWebPlatform(platform))
                {
                    if (libraryFileExtension == ".bc")
                    {
                        if (MatchesPlatformConfiguration(libraryFile, platform, configuration))
                        {
                            yield return libraryFile;
                        }
                    }
                }
                else if (Target.IsWinApiPlatform(platform))
                {
                    if (libraryFileExtension == ".lib")
                    {
                        if (MatchesPlatformConfiguration(libraryFile, platform, configuration))
                        {
                            yield return libraryFile;
                        }
                    }
                }
                else if (Target.IsPosixPlatform(platform))
                {
                    if ((libraryFileExtension == ".a") || (libraryFileExtension == ".so"))
                    {
                        if (MatchesPlatformConfiguration(libraryFile, platform, configuration))
                        {
                            yield return libraryFile;
                        }
                    }
                }
                else
                {
                    throw new NotImplementedException();
                }
            }

            if (Target.IsCocoaPlatform(platform))
            {
                foreach (var frameworkPackage in Directory.EnumerateDirectories(GetLocation(), "*.framework"))
                {
                    if (MatchesPlatformConfiguration(frameworkPackage, platform, configuration))
                    {
                        yield return frameworkPackage;
                    }
                }
            }
        }
    }   // class Dependency

    // ------------------------------------------------------------------------------------------
    //! Adapter that converts projects to dependencies for other projects.
    public class ProjectDependency : Dependency
    {
        private ProjectCache _project;

        // ------------------------------------------------------------------------------------------
        //! Returns project caches that is used in this dependency.
        //! @returns Project caches that is used in this dependency.
        public virtual ProjectCache GetProject()
        {
            if (_project == null)
            {
                string projectSource = Path.Combine(GetLocation(), GetName()) + ".gdproj.cs";
                if (File.Exists(projectSource))
                {
                    _project = ProjectFactory.Create(projectSource);
                }
                else
                {
                    throw new BuildSystemException("Corresponding \"{0}\" project source file was not found.", projectSource);
                }
            }

            return _project;
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of directories that contain header files.
        //! @returns Iterator for list of header files.
        public sealed override IEnumerable<string> EnumerateHeaderDirectories()
        {
            yield return Path.Combine(GetProject().CachedLocation, "Source");
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of files that should be copied to project build output directory.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Iterator for list of files that should be copied to project build output directory.
        public sealed override IEnumerable<string> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.DynamicLibrary)
            {
                yield return GetProject().CachedOutputPaths[platform, configuration];
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of libraries that should be linked with project build file.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Iterator for list of libraries that should be linked with project build file.
        public sealed override IEnumerable<string> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (Target.IsWinApiPlatform(platform))
            {
                yield return GetProject().CachedImportLibraryOutputPaths[platform, configuration];
            }
            else
            {
                if ((GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.DynamicLibrary)
                    || (GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.StaticLibrary))
                {
                    yield return GetProject().CachedOutputPaths[platform, configuration];
                }
            }
        }

    }   // class ProjectDependency

    // ------------------------------------------------------------------------------------------
    //! Represents a collection of cached data that was by dependency object.
    public sealed class DependencyCache : CollectorCache
    {
        public readonly string[] CachedHeaderDirectories;
        public readonly CollectorContainer<string[]> CachedCopyFiles;
        public readonly CollectorContainer<string[]> CachedLinkedLibraries;

        // ------------------------------------------------------------------------------------------
        //! Generates cache for specified dependency.
        //! @param dependency Dependency which dynamic properties would be cached.
        public DependencyCache(Dependency dependency)
            : base(dependency)
        {
            if (IsSupported)
            {
                CachedHeaderDirectories = dependency.EnumerateHeaderDirectories().ToArray();
                CachedCopyFiles = new CollectorContainer<string[]>((p, c) => dependency.EnumerateCopyFiles(p, c).ToArray());
                CachedLinkedLibraries = new CollectorContainer<string[]>((p, c) => dependency.EnumerateLinkedLibraries(p, c).ToArray());
            }
        }

    }   // class DependencyCache

    // ------------------------------------------------------------------------------------------
    //! Represents a factory of dependencies.
    public /*static*/ sealed class DependencyFactory : CollectorFactory<Dependency, DependencyCache>
    {
#if INCLUDE_GODDAMNSDK_SPECIFIC
        private static DependencyCache _goddamnCoreDependency;
        private static DependencyCache _goddamnEngineDependency;

        // ------------------------------------------------------------------------------------------
        //! Returns cached dependency for GoddamnCore project.
        //! @returns Cached dependency for GoddamnCore project.
        public static DependencyCache GetGoddamnCoreDependency()
        {
            if (_goddamnCoreDependency == null)
            {
                var goddamnCoreDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), "Source", "Projects", "GoddamnCore", "GoddamnCore.gddep.cs");
                if (File.Exists(goddamnCoreDependencyPath))
                {
                    _goddamnCoreDependency = Create(goddamnCoreDependencyPath);
                    Debug.Assert(_goddamnCoreDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("GoddamnCore dependency was not found at \"{0}\".", goddamnCoreDependencyPath);
                }
            }

            return _goddamnCoreDependency;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns cached dependency for GoddamnEngine project.
        //! @returns Cached dependency for GoddamnEngine project.
        public static DependencyCache GetGoddamnEngineDependency()
        {
            if (_goddamnEngineDependency == null)
            {
                string goddamnEngineDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), "Source", "Projects", "GoddamnEngine", "GoddamnEngine.gddep.cs");
                if (File.Exists(goddamnEngineDependencyPath))
                {
                    _goddamnEngineDependency = Create(goddamnEngineDependencyPath);
                    Debug.Assert(_goddamnEngineDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("GoddamnEngine dependency was not found at \"{0}\".", goddamnEngineDependencyPath);
                }
            }

            return _goddamnEngineDependency;
        }

#if FALSE
        private static DependencyCache _incredibleEngineDependency;
        // ------------------------------------------------------------------------------------------
        //! Returns cached dependency for IncredibleEngine project.
        //! @returns Cached dependency for IncredibleEngine project.
        public static DependencyCache GetIncredibleEngineDependency()
        {
            if (_incredibleEngineDependency == null)
            {
                string incredibleEngineDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), "Source", "Projects", "IncredibleEngine", "IncredibleEngine.gddep.cs");
                if (File.Exists(incredibleEngineDependencyPath))
                {
                    _incredibleEngineDependency = Create(incredibleEngineDependencyPath);
                    Debug.Assert(_incredibleEngineDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("IncredibleEngine dependency was not found at \"{0}\".", incredibleEngineDependencyPath);
                }
            }

            return _incredibleEngineDependency;
        }
#endif  // FALSE

#endif  // INCLUDE_GODDAMNSDK_SPECIFIC
    }   // class DependencyFactory
}   // namespace GoddamnEngine.BuildSystem

// Some additional dependencies code.
#if INCLUDE_ADDITIONALS
namespace GoddamnEngine.BuildSystem.Additinals
{
    //! Represents a dependency in NVidia PhysX (C) engine.
    public sealed class NVidiaPhysXDependency : Dependency
    {
        private const string X64DefaultPath = @"C:\Program Files (x86)\NVIDIA Corporation\PhysX";
    }   // class NVidiaPhysXDependency
}   // namespace GoddamnEngine.BuildSystem.Additinals
#endif  // if INCLUDE_ADDITIONALS
