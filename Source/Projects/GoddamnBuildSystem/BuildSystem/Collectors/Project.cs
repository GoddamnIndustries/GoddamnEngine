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
using System.IO;
using System.Linq;
using System.Text;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Describes priority of the project.
    /// </summary>
    public enum ProjectPriority : byte
    {
        GameLevel,
        PluginLevel,
        PluginCommonLevel,
        EngineLevel,
        CoreLevel,
        BuildToolLevel
    }   // enum ProjectPriority

    /// <summary>
    /// Types of file in project (sub)directory.
    /// </summary>
    public enum ProjectSourceFileType : byte
    {
        SourceCode,
        SourceCodeAssembler,
        HeaderFile,
        InlineImplementation,
        ResourceScript,
        SupportFile = InlineImplementation
    }   // enum SourceFileType

    /// <summary>
    /// Represents build type of project.
    /// </summary>
    public enum ProjectBuildType : byte
    {
        Application,
        StaticLibrary,
        DynamicLibrary
    }   // public enum ProjectBuildType

    /// <summary>
    /// Represents a language in which project is written in.
    /// </summary>
    public enum ProjectLanguge : byte
    {
        Cs,
        Cpp,
    }   // public enum ProjectLanguge

    /// <summary>
    /// Represents a simple source file added to project.
    /// </summary>
    public sealed class ProjectSourceFile
    {
        /// <summary>
        /// Returns true if this file is excluded on some platform.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if this file is excluded on some platform.</returns>
        public delegate bool IsExcludedDelegate(TargetPlatform platform);

        private static readonly IsExcludedDelegate s_NotExcludedDelegate = platform => false;
        private readonly IsExcludedDelegate m_IsExcluded;
        public readonly string FileName;
        public readonly ProjectSourceFileType FileType;

        /// <summary>
        /// Initializes new source file.
        /// </summary>
        /// <param name="fileName">Path to source file.</param>
        /// <param name="fileType">Type of source file.</param>
        /// <param name="isExcluded">Platform-dependent delegate.</param>
        public ProjectSourceFile(string fileName, ProjectSourceFileType fileType, IsExcludedDelegate isExcluded = null)
        {
            FileName = fileName;
            FileType = fileType;
            m_IsExcluded = isExcluded ?? s_NotExcludedDelegate;
        }

        /// <summary>
        /// Returns true if object on specified path has platform/configuration data and matches it.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns></returns>
        public bool ShouldBeExcluded(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (!m_IsExcluded(platform))
            {
                return !Collector.MatchesPlatformConfiguration(FileName, platform, configuration);
            }
            return true;
        }

    }   // class ProjectSourceFile

    /// <summary>
    /// Represents a simple preprocessor definition.
    /// </summary>
    public sealed class ProjectMacro
    {
        private readonly string m_Name;
        private readonly string m_Value;

        /// <summary>
        /// Initializes a new preprocessor definition.
        /// </summary>
        /// <param name="name">Name of the macro.</param>
        /// <param name="value">Value of the macro.</param>
        public ProjectMacro(string name, string value = null)
        {
            m_Name = name;
            m_Value = value;
        }

        /// <summary>
        /// Returns string version of this definition in format "NAME=VALUE".
        /// </summary>
        public override string ToString()
        {
            return m_Value != null ? m_Name + '=' + m_Value : m_Name;
        }

    }   // class ProjectMacro

    /// <summary>
    /// Representation of a normal C++ project.
    /// </summary>
    public class Project : Collector
    {
        /// <summary>
        /// Returns a programming language, in which project is written in.
        /// </summary>
        protected virtual ProjectLanguge GetProgrammingLanguage()
        {
            return ProjectLanguge.Cpp;
        }

        /// <summary>
        /// Returns name of the filter of this project in generated solution.
        /// May return null if no filter is required.
        /// </summary>
        public virtual string GetFilter()
        {
            return null;
        }

        /// <summary>
        /// Returns priority of this project. Higher priority - earlier project is compiled.
        /// </summary>
        public virtual ProjectPriority GetPriority()
        {
            return ProjectPriority.GameLevel;
        }

        /// <summary>
        /// Returns type of compiler/linker output.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual ProjectBuildType GetBuildType(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            return TargetInfo.IsDesktopPlatform(platform) ? ProjectBuildType.DynamicLibrary : ProjectBuildType.StaticLibrary;
        }

        /// <summary>
        /// Returns path to the compilation result.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual string GetOutputPath(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            var outputDirectory = Path.Combine(BuildSystem.GetSdkLocation(), "bin", GetName());
            string outputExtension = null;
            if (TargetInfo.IsMicrosoftPlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        outputExtension = ".exe";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        outputExtension = ".dll";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        outputExtension = ".lib";
                        break;
                }
            }
            else if (TargetInfo.IsPosixPlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        outputExtension = "";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        outputExtension = ".so";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        outputExtension = ".a";
                        break;
                }
            }
            else
            {
                throw new NotImplementedException();
            }

            return configuration != TargetConfiguration.Release
                ? outputDirectory + "." + configuration + outputExtension
                : outputDirectory + outputExtension;
        }

        /// <summary>
        /// Returns path to the import library (only actual for Windows DLLs), or null if no import library needs to be generated.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual string GetImportLibraryOutputPath(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            if (TargetInfo.IsMicrosoftPlatform(platform) && (GetBuildType(platform, configuration) == ProjectBuildType.DynamicLibrary))
            {
                var importLibraryOutputDirectory = Path.Combine(BuildSystem.GetSdkLocation(), "lib", GetName());
                return configuration != TargetConfiguration.Release
                    ? importLibraryOutputDirectory + "." + configuration + ".lib"
                    : importLibraryOutputDirectory + ".lib";
            }

            return null;
        }

        /// <summary>
        /// Returns offset some path that should be treated as origin of source files (and filters of source files).
        /// </summary>
        public virtual string GetSourceFiltersOrigin()
        {
            return Path.Combine(GetLocation(), "Source", "GoddamnEngine");
        }

        /// <summary>
        /// Collects list of directories that contain header files.
        /// </summary>
        /// <returns>Iterator for list of directories that contain header files.</returns>
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            var projectSourceFiles = Path.Combine(GetLocation(), "Source");
            if (!Directory.Exists(projectSourceFiles))
            {
                throw new BuildSystemException("No source directories for dependencies {0} were found.", GetName());
            }

            yield return projectSourceFiles;
        }

        /// <summary>
        /// Collects list of files with source code, which would be added to generated project data.
        /// </summary>
        /// <returns>Iterator for list of files with source code, which would be added to generated project data.</returns>
        public virtual IEnumerable<ProjectSourceFile> EnumerateSourceFiles()
        {
            // Adding project file as support file.
            yield return new ProjectSourceFile(GetSource(), ProjectSourceFileType.SupportFile);

            // Adding collected project files.
            var projectSourceDirectory = Path.Combine(GetLocation(), "Source");
            if (!Directory.Exists(projectSourceDirectory))
            {
                throw new BuildSystemException("Project {0} does not contain \"Source\" directory.", GetName());
            }

            foreach (var sourceFile in Directory.EnumerateFiles(projectSourceDirectory, "*.*", SearchOption.AllDirectories))
            {
                var sourceFileExtension = (Path.GetExtension(sourceFile) ?? "").ToLowerInvariant();
                // ReSharper disable once SwitchStatementMissingSomeCases
                switch (sourceFileExtension)
                {
                    case ".h":
                    case ".hh":
                    case ".hpp":
                    case ".hxx":
                        yield return new ProjectSourceFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.HeaderFile : ProjectSourceFileType.SupportFile);
                        break;

                    case ".c":
                    case ".cc":
                    case ".cpp":
                    case ".cxx":
                        yield return new ProjectSourceFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.SourceCode : ProjectSourceFileType.SupportFile);
                        break;
                    case ".s":
                    case ".asm":
                        yield return new ProjectSourceFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.SourceCodeAssembler : ProjectSourceFileType.SupportFile);
                        break;

                    case ".inl":
                        yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.InlineImplementation);
                        break;

                    case ".rc":
                        yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.ResourceScript, platform => !TargetInfo.IsMicrosoftPlatform(platform));
                        break;

                    case ".cs":
                        var projectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(sourceFile) ?? "").ToLowerInvariant();
                        if (!string.IsNullOrEmpty(projectFileSecondExtension))
                        {
                            switch (projectFileSecondExtension)
                            {
                                case ".gdproj":
                                case ".gddep":
                                case ".gdsln":
                                    yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.SupportFile);
                                    break;
                            }
                        }
                        break;
                }
            }
        }

        /// <summary>
        /// Collects list of dependencies for this project.
        /// All unsupported projects should be filtered by this function.
        /// </summary>
        /// <returns>Iterator for list of list of dependencies for this project.</returns>
        public virtual IEnumerable<DependencyCache> EnumerateDependencies()
        {
            string projectDependencyDirectory = Path.Combine(GetLocation(), "Dependencies");
            if (Directory.Exists(projectDependencyDirectory))
            {
                // Adding explicit dependencies.
                foreach (var dependency in Directory.EnumerateFiles(projectDependencyDirectory, "*.gddep.cs", SearchOption.AllDirectories).Select(DependencyFactory.Create))
                {
                    if (dependency.IsSupported)
                    {
                        yield return dependency;
                    }
                }
            }

            // Adding implicit priority-related dependencies.
            var priority = GetPriority();
            if (priority < ProjectPriority.CoreLevel)
            {
                // Resolving Core library dependency.
                yield return DependencyFactory.GetGoddamnCoreDependency();
                if (priority < ProjectPriority.EngineLevel)
                {
                    // Resolving Engine library dependency.
                    yield return DependencyFactory.GetGoddamnEngineDependency();
                }
            }
        }

        /// <summary>
        /// Collects list of additional preprocessor definitions added to this project.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Iterator for list of additional preprocessor definitions added to this project.</returns>
        public virtual IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (TargetInfo.IsMicrosoftPlatform(platform))
            {
                if (TargetConfigurationInfo.Get(configuration).IsDebug)
                {
                    yield return new ProjectMacro("_DEBUG", "1");
                }
                else
                {
                    yield return new ProjectMacro("NDEBUG", "1");
                }

                if (GetBuildType(platform, configuration) == ProjectBuildType.DynamicLibrary)
                {
                    yield return new ProjectMacro("_WINDLL");
                }
            }

            // Something else here..
        }

    }   // class Project

    /// <summary>
    /// Representation of some build tool project.
    /// </summary>
    public class BuildToolProject : Project
    {
        /// <summary>
        /// Returns a programming language, in which project is written in.
        /// C# typically.
        /// </summary>
        protected override ProjectLanguge GetProgrammingLanguage()
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

        /// <summary>
        /// Returns name of the filter of this project in generated solution.
        /// By default, returns "Build tools".
        /// </summary>
        /// <returns>Name of the filter of this project in generated solution.</returns>
        public override string GetFilter()
        {
            return "Build tools";
        }

        /// <summary>
        /// Returns priority of this project. Higher priority - earlier project is compiled.
        /// Obviously, build tool priority is constantly set to BuildToolLevel.
        /// </summary>
        /// <returns>Returns priority of this project.</returns>
        public sealed override ProjectPriority GetPriority()
        {
            return ProjectPriority.BuildToolLevel;
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override ProjectBuildType GetBuildType(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override string GetOutputPath(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override string GetImportLibraryOutputPath(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override string GetSourceFiltersOrigin()
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override IEnumerable<ProjectSourceFile> EnumerateSourceFiles()
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override IEnumerable<string> EnumerateHeaderDirectories()
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override IEnumerable<DependencyCache> EnumerateDependencies()
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// Method is not supported for a build tool.
        /// </summary>
        public sealed override IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

    }   // class BuildToolProject

    /// <inheritdoc />
    /// <summary>
    /// Represents a collection of cached data that was collected by project object.
    /// </summary>
    public class ProjectCache : CollectorCache
    {
        public string GeneratorOutputPath;
        public readonly bool IsBuildTool;
        public readonly string CachedFilter;
        public readonly ProjectPriority CachedPriority;
        public readonly CollectorContainer<ProjectBuildType> CachedBuildTypes;
        public readonly CollectorContainer<string> CachedOutputPaths;
        public readonly CollectorContainer<string> CachedImportLibraryOutputPaths;
        public readonly string CachedSourcesFilterOrigin;
        public readonly ProjectSourceFile[] CachedSourceFiles;
        public readonly string[] CachedHeaderDirectories;
        public readonly DependencyCache[] CachedDependencies;
        public readonly CollectorContainer<ProjectMacro[]> CachedMacros;

        /// <inheritdoc />
        /// <summary>
        /// Generates cache for specified project.
        /// </summary>
        /// <param name="project">Project which dynamic properties would be cached.</param>
        public ProjectCache(Project project)
            : base(project)
        {
            if (IsSupported)
            {
                CachedFilter = project.GetFilter();
                CachedPriority = project.GetPriority();
                IsBuildTool = project is BuildToolProject;
                if (!IsBuildTool)
                {
                    CachedBuildTypes = new CollectorContainer<ProjectBuildType>(project.GetBuildType);
                    CachedOutputPaths = new CollectorContainer<string>(project.GetOutputPath);
                    CachedImportLibraryOutputPaths = new CollectorContainer<string>(project.GetImportLibraryOutputPath);
                    CachedSourcesFilterOrigin = project.GetSourceFiltersOrigin();
                    CachedSourceFiles = project.EnumerateSourceFiles().ToArray();
                    CachedHeaderDirectories = project.EnumerateHeaderDirectories().ToArray();
                    CachedDependencies = project.EnumerateDependencies().ToArray();
                    CachedMacros = new CollectorContainer<ProjectMacro[]>((p, c) => project.EnumerateMacros(p, c).ToArray());
                }
                else
                {
                    GeneratorOutputPath = ((BuildToolProject)project).GetProjectFile();
                }
            }
        }

        /// <summary>
        /// Generates a strigified list of include paths.
        /// </summary>
        /// <param name="separator">Separator string between include paths. ';' By default.</param>
        /// <returns>A strigified include paths.</returns>
        public string GenerateIncludePaths(string separator = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            var includePathesBuilder = new StringBuilder();

            // Adding header paths from this projects.
            foreach (var headerDirectory in CachedHeaderDirectories)
            {
                includePathesBuilder.Append(headerDirectory).Append(separator);
            }

            // Adding header paths from dependencies.
            foreach (var dependency in CachedDependencies)
            {
                foreach (var dependencyIncludePath in dependency.CachedHeaderDirectories)
                {
                    includePathesBuilder.Append(dependencyIncludePath).Append(separator);

                }
            }

            return includePathesBuilder.ToString();
        }

        /// <summary>
        /// Generates a strigified list of macros.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <param name="separator">Separator string between macros. ';' By default.</param>
        /// <returns>A strigified list of macros.</returns>
        public string GenerateMacros(TargetPlatform platform, TargetConfiguration configuration, string separator = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            var macrosBuilder = new StringBuilder();
            foreach (var macro in CachedMacros[platform, configuration])
            {
                macrosBuilder.Append(macro).Append(separator);
            }
            return macrosBuilder.ToString();
        }

        /// <summary>
        /// Generates a strigified list of linked libraries paths.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <param name="separator">Separator string between linked libraries paths. ';' By default.</param>
        /// <returns>A strigified list of linked libraries paths.</returns>
        public string GenerateLinkedLibrariesPaths(TargetPlatform platform, TargetConfiguration configuration, string separator = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            // Adding libraries from dependencies.
            var linkedLibraries = new StringBuilder();
            foreach (var projectDependency in CachedDependencies)
            {
                foreach (var projectDependencyLibraryPath in projectDependency.CachedLinkedLibraries[platform, configuration])
                {
                    linkedLibraries.Append(projectDependencyLibraryPath).Append(separator);
                }
            }
            return linkedLibraries.ToString();
        }
    }   // class ProjectCache

    /// <summary>
    /// Represents a factory of projects.
    /// </summary>
    public class ProjectFactory : CollectorFactory<Project, ProjectCache>
    {
    }   // class ProjectFactory

}   // namespace GoddamnEngine.BuildSystem
