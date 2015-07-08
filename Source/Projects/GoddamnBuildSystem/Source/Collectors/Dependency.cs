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
    //[
    //! Represents a single project's dependency.
    public class Dependency : Collector
    {
        //! Collects list of directories that contain header files.
        //! @returns Iterator for list of header files.
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            string StandartIncludePath = Path.Combine(GetLocation(), "include");
            if (Directory.Exists(StandartIncludePath))
            {
                yield return StandartIncludePath;
            }
            else
            {
                throw new BuildSystemException("No include directories for dependencies {0} were found.", GetName());
            }
        }

        //[
        //! Collects list of files that should be copied to project build output directory.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Iterator for list of files that should be copied to project build output directory.
        public virtual IEnumerable<string> EnumerateCopyFiles(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            if (Target.IsDesktopPlatform(Platform))
            {
                foreach (string CopyFile in Directory.EnumerateFiles(GetLocation()))
                {
                    string CopyFileExtension = Path.GetExtension(CopyFile).ToLowerInvariant();
                    if (Target.IsWinAPIPlatform(Platform))
                    {
                        if (CopyFileExtension == ".dll")
                        {
                            yield return CopyFile;
                        }
                    }
                }
            }
        }

        //[
        //! Collects list of libraries that should be linked with project build file.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Iterator for list of libraries that should be linked with project build file.
        public virtual IEnumerable<string> EnumerateLinkedLibraries(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            foreach (string LibraryFile in Directory.EnumerateFiles(this.GetLocation()))
            {
                string LibraryFileExtension = Path.GetExtension(LibraryFile).ToLowerInvariant();
                if (Target.IsWebPlatform(Platform))
                {
                    if (LibraryFileExtension == ".bc")
                    {
                        if (Collector.MatchesPlatformConfiguration(LibraryFile, Platform, Configuration))
                        {
                            yield return LibraryFile;
                        }
                    }
                }
                else if (Target.IsWinAPIPlatform(Platform))
                {
                    if (LibraryFileExtension == ".lib")
                    {
                        if (Collector.MatchesPlatformConfiguration(LibraryFile, Platform, Configuration))
                        {
                            yield return LibraryFile;
                        }
                    }
                }
                else if (Target.IsPosixPlatform(Platform))
                {
                    if ((LibraryFileExtension == ".a") || (LibraryFileExtension == ".so"))
                    {
                        if (Collector.MatchesPlatformConfiguration(LibraryFile, Platform, Configuration))
                        {
                            yield return LibraryFile;
                        }
                    }
                }
                else
                {
                    throw new NotImplementedException();
                }
            }

            if (Target.IsCocoaPlatform(Platform))
            {
                foreach (string FrameworkPackage in Directory.EnumerateDirectories(this.GetLocation(), "*.framework"))
                {
                    if (Collector.MatchesPlatformConfiguration(FrameworkPackage, Platform, Configuration))
                    {
                        yield return FrameworkPackage;
                    }
                }
            }
        }
    }   // class Dependency

    //[
    //! Adapter that converts projects to dependencies for other projects.
    public class ProjectDependency : Dependency
    {
        private ProjectCache Project = null;

        //[
        //! Returns project caches that is used in this dependency.
        //! @returns Project caches that is used in this dependency.
        public virtual ProjectCache GetProject()
        {
            if (Project == null)
            {
                string ProjectSource = Path.Combine(GetLocation(), GetName()) + ".gdproj.cs";
                if (File.Exists(ProjectSource))
                {
                    Project = ProjectFactory.Create(ProjectSource);
                }
                else
                {
                    throw new BuildSystemException("Corresponding \"{0}\" project source file was not found.", ProjectSource);
                }
            }

            return Project;
        }

        //[
        //! Collects list of directories that contain header files.
        //! @returns Iterator for list of header files.
        public sealed override IEnumerable<string> EnumerateHeaderDirectories()
        {
            yield return Path.Combine(GetProject().CachedLocation, "Source");
        }

        //[
        //! Collects list of files that should be copied to project build output directory.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Iterator for list of files that should be copied to project build output directory.
        public sealed override IEnumerable<string> EnumerateCopyFiles(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            if (GetProject().CachedBuildTypes[Platform, Configuration] == ProjectBuildType.DynamicLibrary)
            {
                yield return GetProject().CachedOutputPaths[Platform, Configuration];
            }
        }

        //[
        //! Collects list of libraries that should be linked with project build file.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Iterator for list of libraries that should be linked with project build file.
        public sealed override IEnumerable<string> EnumerateLinkedLibraries(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            if (Target.IsWinAPIPlatform(Platform))
            {
                yield return GetProject().CachedImportLibraryOutputPaths[Platform, Configuration];
            }
            else
            {
                if ((GetProject().CachedBuildTypes[Platform, Configuration] == ProjectBuildType.DynamicLibrary)
                    || (GetProject().CachedBuildTypes[Platform, Configuration] == ProjectBuildType.StaticLibrary))
                {
                    yield return GetProject().CachedOutputPaths[Platform, Configuration];
                }
            }
        }

    }   // class ProjectDependency

    //[
    //! Represents a collection of cached data that was by dependency object.
    public sealed class DependencyCache : CollectorCache
    {
        public readonly string[] CachedHeaderDirectories;
        public readonly CollectorContainer<string[]> CachedCopyFiles;
        public readonly CollectorContainer<string[]> CachedLinkedLibraries;

        //[
        //! Generates cache for specified dependency.
        //! @param Dependency Dependency which dynamic properties would be cached.
        public DependencyCache(Dependency Dependency)
            : base(Dependency)
        {
            if (IsSupported)
            {
                CachedHeaderDirectories = Dependency.EnumerateHeaderDirectories().ToArray();
                CachedCopyFiles = new CollectorContainer<string[]>((TargetPlatform P, TargetConfiguration C) => Dependency.EnumerateCopyFiles(P, C).ToArray());
                CachedLinkedLibraries = new CollectorContainer<string[]>((TargetPlatform P, TargetConfiguration C) => Dependency.EnumerateLinkedLibraries(P, C).ToArray());
            }
        }

    }   // class DependencyCache

    //[
    //! Represents a factory of dependencies.
    public /*static*/ sealed class DependencyFactory : CollectorFactory<Dependency, DependencyCache>
    {
#if INCLUDE_GODDAMNSDK_SPECIFIC
        private static DependencyCache GoddamnCoreDependency = null;
        private static DependencyCache GoddamnEngineDependency = null;

        //[
        //! Returns cached dependency for GoddamnCore project.
        //! @returns Cached dependency for GoddamnCore project.
        public static DependencyCache GetGoddamnCoreDependency()
        {
            if (GoddamnCoreDependency == null)
            {
                string GoddamnCoreDependencyPath = Path.Combine(BuildSystem.GetSDKLocation(), "Source", "Projects", "GoddamnCore", "GoddamnCore.gddep.cs");
                if (File.Exists(GoddamnCoreDependencyPath))
                {
                    GoddamnCoreDependency = Create(GoddamnCoreDependencyPath);
                    Debug.Assert(GoddamnCoreDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("GoddamnCore dependency was not found at \"{0}\".", GoddamnCoreDependencyPath);
                }
            }

            return GoddamnCoreDependency;
        }

        //[
        //! Returns cached dependency for GoddamnEngine project.
        //! @returns Cached dependency for GoddamnEngine project.
        public static DependencyCache GetGoddamnEngineDependency()
        {
            if (GoddamnEngineDependency == null)
            {
                string GoddamnEngineDependencyPath = Path.Combine(BuildSystem.GetSDKLocation(), "Source", "Projects", "GoddamnEngine", "GoddamnEngine.gddep.cs");
                if (File.Exists(GoddamnEngineDependencyPath))
                {
                    GoddamnEngineDependency = Create(GoddamnEngineDependencyPath);
                    Debug.Assert(GoddamnEngineDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("GoddamnEngine dependency was not found at \"{0}\".", GoddamnEngineDependencyPath);
                }
            }

            return GoddamnEngineDependency;
        }

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
