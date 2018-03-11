// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Adapter that converts projects to dependencies enumerator for other projects.
    /// </summary>
    /// <inheritdoc />
    public class ProjectDependencyEnumerator : DependencyEnumerator
    {
        private Project m_Project;

        /// <summary>
        /// Returns project caches that is used in this dependency.
        /// </summary>
        protected virtual Project GetProject()
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

        /// <inheritdoc />
        public sealed override IEnumerable<string> EnumerateHeaderDirectories(TargetPlatform platform, TargetConfiguration configuration)
        {
            yield return Path.Combine(GetProject().Location, "Source");
        }

        /// <inheritdoc />
        public sealed override IEnumerable<DependencyFile> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (GetProject().BuildType[platform, configuration] == ProjectBuildType.DynamicLibrary)
            {
                yield return new DependencyFile(GetProject().OutputPath[platform, configuration], DependencyFileType.DynamicLibrary);
            }
        }

        /// <inheritdoc />
        public sealed override IEnumerable<DependencyFile> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (TargetInfo.IsMicrosoftPlatform(platform))
            {
                yield return new DependencyFile(GetProject().ImportLibraryOutputPath[platform, configuration], DependencyFileType.StaticLibrary);
            }
            else
            {
                switch (GetProject().BuildType[platform, configuration])
                {
                    case ProjectBuildType.StaticLibrary:
                        yield return new DependencyFile(GetProject().OutputPath[platform, configuration], DependencyFileType.StaticLibrary);
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        yield return new DependencyFile(GetProject().OutputPath[platform, configuration], DependencyFileType.DynamicLibrary);
                        break;
                }
            }
        }
    }   // class ProjectDependencyEnumerator

    public static class ProjectDependencyFactory
    {
        private static Dependency s_GoddamnCoreDependency;
        private static Dependency s_GoddamnEngineDependency;

        /// <summary>
        /// Returns cached dependency for GoddamnCore project.
        /// </summary>
        /// <returns>Cached dependency for GoddamnCore project.</returns>
        public static Dependency GetGoddamnCoreDependency()
        {
            if (s_GoddamnCoreDependency == null)
            {
                var goddamnCoreDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), 
                                                             "Source", "Projects", "GoddamnCore", "GoddamnCore.gddep.cs");
                if (File.Exists(goddamnCoreDependencyPath))
                {
                    s_GoddamnCoreDependency = DependencyFactory.Create(goddamnCoreDependencyPath);
                    Debug.Assert(s_GoddamnCoreDependency.Enumerator is ProjectDependencyEnumerator);
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
        public static Dependency GetGoddamnEngineDependency()
        {
            if (s_GoddamnEngineDependency == null)
            {
                var goddamnEngineDependencyPath = Path.Combine(BuildSystem.GetSdkLocation(), "Source", "Projects", "GoddamnEngine", "GoddamnEngine.gddep.cs");
                if (File.Exists(goddamnEngineDependencyPath))
                {
                    s_GoddamnEngineDependency = DependencyFactory.Create(goddamnEngineDependencyPath);
                    Debug.Assert(s_GoddamnEngineDependency.Enumerator is ProjectDependencyEnumerator);
                }
                else
                {
                    throw new BuildSystemException("GoddamnEngine dependency was not found at \"{0}\".", goddamnEngineDependencyPath);
                }
            }
            return s_GoddamnEngineDependency;
        }
    }   // class DependencyFactory
}   // namespace GoddamnEngine.BuildSystem.Collectors
