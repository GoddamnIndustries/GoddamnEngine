// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Represents a single project's dependency.
    /// </summary>
    public class Dependency : Collector
    {
        /// <summary>
        /// Collects list of directories that contain header files.
        /// </summary>
        /// <returns>Iterator for list of header files.</returns>
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            var standardIncludePath = Path.Combine(GetLocation(), "include");
            if (Directory.Exists(standardIncludePath))
            {
                yield return standardIncludePath;
            }
            else
            {
                throw new BuildSystemException("No include directories for dependencies {0} were found.", GetName());
            }
        }

        /// <summary>
        /// Collects list of files that should be copied to project build output directory.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of files that should be copied to project build output directory.</returns>
        public virtual IEnumerable<string> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            if (TargetInfo.IsDesktopPlatform(platform))
            {
                foreach (var copyFile in Directory.EnumerateFiles(GetLocation()))
                {
                    var copyFileExtension = (Path.GetExtension(copyFile) ?? "").ToLowerInvariant();
                    if (TargetInfo.IsMicrosoftPlatform(platform))
                    {
                        if (copyFileExtension == ".dll")
                        {
                            yield return copyFile;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Collects list of libraries that should be linked with project build file.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of libraries that should be linked with project build file.</returns>
        public virtual IEnumerable<string> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            foreach (var libraryFile in Directory.EnumerateFiles(GetLocation()))
            {
                var libraryFileExtension = (Path.GetExtension(libraryFile) ?? "").ToLowerInvariant();
                if (TargetInfo.IsMicrosoftPlatform(platform))
                {
                    if (libraryFileExtension == ".lib")
                    {
                        if (MatchesPlatformConfiguration(libraryFile, platform, configuration))
                        {
                            yield return libraryFile;
                        }
                    }
                }
                else if (TargetInfo.IsPosixPlatform(platform))
                {
                    if (libraryFileExtension == ".a" || libraryFileExtension == ".so")
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
        }
    }   // class Dependency

    /// <summary>
    /// Adapter that converts projects to dependencies for other projects.
    /// </summary>
    public class ProjectDependency : Dependency
    {
        private ProjectCache m_Project;

        /// <summary>
        /// Returns project caches that is used in this dependency.
        /// </summary>
        protected virtual ProjectCache GetProject()
        {
            if (m_Project == null)
            {
                var projectSource = Path.Combine(GetLocation(), GetName()) + ".gdproj.cs";
                if (File.Exists(projectSource))
                {
                    m_Project = ProjectFactory.Create(projectSource);
                }
                else
                {
                    throw new BuildSystemException("Corresponding \"{0}\" project source file was not found.", projectSource);
                }
            }
            return m_Project;
        }

        /// <summary>
        /// Collects list of directories that contain header files.
        /// </summary>
        /// <returns>Iterator for list of header files.</returns>
        public sealed override IEnumerable<string> EnumerateHeaderDirectories()
        {
            yield return Path.Combine(GetProject().CachedLocation, "Source");
        }

        /// <summary>
        /// Collects list of files that should be copied to project build output directory.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of files that should be copied to project build output directory.</returns>
        public sealed override IEnumerable<string> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.DynamicLibrary)
            {
                yield return GetProject().CachedOutputPaths[platform, configuration];
            }
        }

        /// <summary>
        /// Collects list of libraries that should be linked with project build file.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of libraries that should be linked with project build file.</returns>
        public sealed override IEnumerable<string> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (TargetInfo.IsMicrosoftPlatform(platform))
            {
                yield return GetProject().CachedImportLibraryOutputPaths[platform, configuration];
            }
            else
            {
                if (GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.DynamicLibrary
                    || GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.StaticLibrary)
                {
                    yield return GetProject().CachedOutputPaths[platform, configuration];
                }
            }
        }

    }   // class ProjectDependency

    /// <summary>
    /// Represents a collection of cached data that was by dependency object.
    /// </summary>
    public sealed class DependencyCache : CollectorCache
    {
        public readonly string[] CachedHeaderDirectories;
        public readonly CollectorContainer<string[]> CachedCopyFiles;
        public readonly CollectorContainer<string[]> CachedLinkedLibraries;

        /// <summary>
        /// Generates cache for specified dependency.
        /// </summary>
        /// <param name="dependency">Dependency which dynamic properties would be cached.</param>
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

    /// <summary>
    /// Represents a factory of dependencies.
    /// </summary>
    public sealed class DependencyFactory : CollectorFactory<Dependency, DependencyCache>
    {
        private static DependencyCache s_GoddamnCoreDependency;
        private static DependencyCache s_GoddamnEngineDependency;

        /// <summary>
        /// Returns cached dependency for GoddamnCore project.
        /// </summary>
        /// <returns>Cached dependency for GoddamnCore project.</returns>
        public static DependencyCache GetGoddamnCoreDependency()
        {
            if (s_GoddamnCoreDependency == null)
            {
                var goddamnCoreDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), "Source", "Projects", "GoddamnCore", "GoddamnCore.gddep.cs");
                if (File.Exists(goddamnCoreDependencyPath))
                {
                    s_GoddamnCoreDependency = Create(goddamnCoreDependencyPath);
                    Debug.Assert(s_GoddamnCoreDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("GoddamnCore dependency was not found at \"{0}\".", goddamnCoreDependencyPath);
                }
            }

            return s_GoddamnCoreDependency;
        }

        /// <summary>
        /// Returns cached dependency for GoddamnEngine project.
        /// </summary>
        /// <returns>Cached dependency for GoddamnEngine project.</returns>
        public static DependencyCache GetGoddamnEngineDependency()
        {
            if (s_GoddamnEngineDependency == null)
            {
                var goddamnEngineDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), "Source", "Projects", "GoddamnEngine", "GoddamnEngine.gddep.cs");
                if (File.Exists(goddamnEngineDependencyPath))
                {
                    s_GoddamnEngineDependency = Create(goddamnEngineDependencyPath);
                    Debug.Assert(s_GoddamnEngineDependency.Collector is ProjectDependency);
                }
                else
                {
                    throw new BuildSystemException("GoddamnEngine dependency was not found at \"{0}\".", goddamnEngineDependencyPath);
                }
            }

            return s_GoddamnEngineDependency;
        }

    }   // class DependencyFactory

}   // namespace GoddamnEngine.BuildSystem
