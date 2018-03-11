// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.IO;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Representation of some build tool project.
    /// </summary>
    public class BuildToolProjectEnumerator : ProjectEnumerator
    {
        /// <inheritdoc />
        public override ProjectLanguge GetProgrammingLanguage()
        {
            return ProjectLanguge.Cs;
        }

        /// <summary>
        /// Returns path to project file of the build tool.
        /// By default, it set to ".csproj" file with same name, as project.
        /// </summary>
        /// <returns>Path to project file of the build tool.</returns>
        public virtual string GetProjectFile()
        {
            var csProjFile = Path.Combine(GetLocation(), GetName() + ".csproj");
            if (File.Exists(csProjFile))
            {
                return csProjFile;
            }
            throw new BuildSystemException("No project files for build tool {0} where found.", GetName());
        }

        /// <inheritdoc />
        public override string GetFilter()
        {
            return "Build tools";
        }

        /// <inheritdoc />
        public sealed override ProjectPriority GetPriority()
        {
            return ProjectPriority.BuildToolLevel;
        }

        /// <inheritdoc />
        public sealed override ProjectBuildType GetBuildType(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override string GetOutputFileName(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override string GetOutputDir(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override string GetImportLibraryOutputPath(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override string GetSourceFiltersOrigin()
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override IEnumerable<ProjectFile> EnumerateSourceFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override IEnumerable<string> EnumerateHeaderDirectories(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override IEnumerable<Dependency> EnumerateDependencies(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <inheritdoc />
        public sealed override IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }
    }   // public class BuildToolProjectEnumerator
}   // namespace GoddamnEngine.BuildSystem.Collectors
