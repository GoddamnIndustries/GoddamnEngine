// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.IO;
using System.Linq;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Dependency file type.
    /// </summary>
    public enum DependencyFileType
    {
        Other,
        StaticLibrary,
        DynamicLibrary,
    }   // enum DependencyFileType

    /// <summary>
    /// Dependency file.
    /// </summary>
    public class DependencyFile
    {
        public readonly string FilePath;
        public readonly DependencyFileType FileType;
        public readonly dynamic FileMisc = new ExpandoObject();

        /// <summary>
        /// Initializes a dependency file.
        /// </summary>
        /// <param name="filePath">File path.</param>
        /// <param name="fileType">File type.</param>
        public DependencyFile(string filePath, DependencyFileType fileType)
        {
            FilePath = filePath;
            FileType = fileType;
        }
    }   // class DependencyFile

    /// <summary>
    /// Project dependency interface.
    /// </summary>
    public interface IDependency : ICollector
    {
        /// <summary>
        /// Collects list of directories that contain header files.
        /// </summary>
        /// <returns>Iterator for list of header files.</returns>
        IEnumerable<string> EnumerateHeaderDirectories();

        /// <summary>
        /// Collects list of files that should be copied to project build output directory.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of files that should be copied to project build output directory.</returns>
        IEnumerable<DependencyFile> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration);

        /// <summary>
        /// Collects list of libraries that should be linked with project build file.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of libraries that should be linked with project build file.</returns>
        IEnumerable<DependencyFile> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration);
    }   // public interface IDependency

    /// <summary>
    /// Project dependency.
    /// </summary>
    public class Dependency : Collector, IDependency
    {
        /// <summary>
        /// Enumerates list of directories that contain header files.
        /// </summary>
        /// <returns>Iterator for list of header files .</returns>
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            // TODO: '.framework' bundles on Apple platforms.
            var standardIncludePath = Path.Combine(GetLocation(), "include");
            if (Directory.Exists(standardIncludePath))
            {
                yield return standardIncludePath;
            }
        }

        /// <summary>
        /// Enumerates list of files that should be copied to project build output directory.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of files that should be copied to project build output directory.</returns>
        public virtual IEnumerable<DependencyFile> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            Debug.Assert(configuration != TargetConfiguration.Unknown);
            if (TargetInfo.IsDesktopPlatform(platform))
            {
                foreach (var copyFilePath in Directory.EnumerateFiles(GetLocation()))
                {
                    if (!MatchesPlatformConfiguration(copyFilePath, platform, configuration))
                    {
                        continue;
                    }

                    var copyFileExtension = (Path.GetExtension(copyFilePath) ?? "").ToLowerInvariant();
                    if (TargetInfo.IsMicrosoftPlatform(platform))
                    {
                        switch (copyFileExtension)
                        {
                            case ".dll":
                                yield return new DependencyFile(copyFilePath, DependencyFileType.DynamicLibrary);
                                break;
                        }
                    }
                    else if (TargetInfo.IsApplePlatform(platform))
                    {
                        switch (copyFileExtension)
                        {
                            case ".dylib":
                                yield return new DependencyFile(copyFilePath, DependencyFileType.DynamicLibrary);
                                break;
                        }
                    }
                    else
                    {
                        throw new NotSupportedException();
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
        public virtual IEnumerable<DependencyFile> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            Debug.Assert(configuration != TargetConfiguration.Unknown);
            foreach (var libraryFile in Directory.EnumerateFiles(GetLocation()))
            {
                if (!MatchesPlatformConfiguration(libraryFile, platform, configuration))
                {
                    continue;
                }

                var libraryFileExtension = (Path.GetExtension(libraryFile) ?? "").ToLowerInvariant();
                if (TargetInfo.IsMicrosoftPlatform(platform))
                {
                    switch (libraryFileExtension)
                    {
                        case ".lib":
                            yield return new DependencyFile(libraryFile, DependencyFileType.StaticLibrary);
                            break;
                    }
                }
                else if (TargetInfo.IsApplePlatform(platform))
                {
                    // TODO: '.framework' bundles on Apple platforms.
                    switch (libraryFileExtension)
                    {
                        case ".a":
                            yield return new DependencyFile(libraryFile, DependencyFileType.StaticLibrary);
                            break;
                        case ".dylib":
                            yield return new DependencyFile(libraryFile, DependencyFileType.DynamicLibrary);
                            break;
                    }
                }
                else if (TargetInfo.IsPosixPlatform(platform))
                {
                    switch (libraryFileExtension)
                    {
                        case ".a":
                            yield return new DependencyFile(libraryFile, DependencyFileType.StaticLibrary);
                            break;
                        case ".so":
                            yield return new DependencyFile(libraryFile, DependencyFileType.DynamicLibrary);
                            break;
                    }
                }
                else
                {
                    throw new NotSupportedException();
                }
            }
        }
    }   // class Dependency

    /// <summary>
    /// Adapter that converts projects to dependencies for other projects.
    /// </summary>
    public class ProjectDependency : Dependency
    {
        ProjectCache m_Project;

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
        /// Enumerates list of files that should be copied to project build output directory.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of files that should be copied to project build output directory.</returns>
        public sealed override IEnumerable<DependencyFile> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (GetProject().CachedBuildTypes[platform, configuration] == ProjectBuildType.DynamicLibrary)
            {
                yield return new DependencyFile(GetProject().CachedOutputPaths[platform, configuration], DependencyFileType.DynamicLibrary);
            }
        }

        /// <summary>
        /// Collects list of libraries that should be linked with project build file.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of libraries that should be linked with project build file.</returns>
        public sealed override IEnumerable<DependencyFile> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (TargetInfo.IsMicrosoftPlatform(platform))
            {
                yield return new DependencyFile(GetProject().CachedImportLibraryOutputPaths[platform, configuration], DependencyFileType.StaticLibrary);
            }
            else
            {
                switch (GetProject().CachedBuildTypes[platform, configuration])
                {
                    case ProjectBuildType.StaticLibrary:
                        yield return new DependencyFile(GetProject().CachedOutputPaths[platform, configuration], DependencyFileType.StaticLibrary);
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        yield return new DependencyFile(GetProject().CachedOutputPaths[platform, configuration], DependencyFileType.DynamicLibrary);
                        break;
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
        public readonly CollectorContainer<DependencyFile[]> CachedCopyFiles;
        public readonly CollectorContainer<DependencyFile[]> CachedLinkedLibraries;

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
                CachedCopyFiles = new CollectorContainer<DependencyFile[]>((p, c) => dependency.EnumerateCopyFiles(p, c).ToArray());
                CachedLinkedLibraries = new CollectorContainer<DependencyFile[]>((p, c) => dependency.EnumerateLinkedLibraries(p, c).ToArray());
            }
        }

    }   // class DependencyCache

    /// <summary>
    /// Represents a factory of dependencies.
    /// </summary>
    public static class DependencyFactory
    {
        static DependencyCache s_GoddamnCoreDependency;
        static DependencyCache s_GoddamnEngineDependency;

        /// <summary>
        /// Constructs new dependency instance and it's cached data.
        /// </summary>
        /// <param name="dependencySourcePath">Path so source file of the dependency.</param>
        /// <returns>Created instance of cached dependency data.</returns>
        public static DependencyCache Create(string dependencySourcePath)
        {
            return CollectorFactory<Dependency, DependencyCache>.Create(dependencySourcePath);
        }

        /// <summary>
        /// Returns cached dependency for GoddamnCore project.
        /// </summary>
        /// <returns>Cached dependency for GoddamnCore project.</returns>
        public static DependencyCache GetGoddamnCoreDependency()
        {
            if (s_GoddamnCoreDependency == null)
            {
                var goddamnCoreDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), 
                                                             "Source", "Projects", "GoddamnCore", "GoddamnCore.gddep.cs");
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
