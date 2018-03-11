// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Priority of the project.
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
        SourceCodeObjective,
        HeaderFile,
        InlineImplementation,
        ResourceScript,
        SupportFile = InlineImplementation
    }   // enum SourceFileType

    /// <summary>
    /// Build type of project.
    /// </summary>
    public enum ProjectBuildType : byte
    {
        Application,
        StaticLibrary,
        DynamicLibrary
    }   // public enum ProjectBuildType

    /// <summary>
    /// Language in which project is written in.
    /// </summary>
    public enum ProjectLanguge : byte
    {
        Cs,
        Cpp,
    }   // public enum ProjectLanguge

    /// <summary>
    /// Project file.
    /// </summary>
    public sealed class ProjectFile
    {
        /// <summary>
        /// Path to source file.
        /// </summary>
        public readonly string FilePath;

        /// <summary>
        /// Type of source file.
        /// </summary>
        public readonly ProjectSourceFileType FileType;

        /// <summary>
        /// Empty object that can be used internally.
        /// </summary>
        internal readonly dynamic FileMisc = new ExpandoObject();

        /// <summary>
        /// Initializes new source file.
        /// </summary>
        /// <param name="filePath">Path to source file.</param>
        /// <param name="fileType">Type of source file.</param>
        public ProjectFile(string filePath, ProjectSourceFileType fileType)
        {
            FilePath = filePath;
            FileType = fileType;
        }
    }   // class ProjectSourceFile

    /// <summary>
    /// Preprocessor definition.
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
    /// Project enumerator.
    /// </summary>
    /// <inheritdoc />
    public class ProjectEnumerator : CollectorEnumerator
    {
        /// <summary>
        /// Returns a programming language, in which project is written in.
        /// </summary>
        public virtual ProjectLanguge GetProgrammingLanguage()
        {
            return ProjectLanguge.Cpp;
        }

        /// <summary>
        /// Returns name of the filter of this project in generated solution. May return null if no filter is required.
        /// </summary>
        public virtual string GetFilter()
        {
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
        /// Returns directory of the compilation result.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual string GetOutputFileName(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            string outputExtension = null, outputPrefix = null;
            if (TargetInfo.IsMicrosoftPlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        outputPrefix = "";
                        outputExtension = ".exe";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        outputPrefix = "";
                        outputExtension = ".dll";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        outputPrefix = "";
                        outputExtension = ".lib";
                        break;
                }
            }
            else if (TargetInfo.IsApplePlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        outputPrefix = "";
                        outputExtension = ".app";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        outputPrefix = "";
                        outputExtension = ".dylib";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        outputPrefix = "lib";
                        outputExtension = ".a";
                        break;
                }
            }
            else if (TargetInfo.IsPosixPlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        outputPrefix = "";
                        outputExtension = "";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        outputPrefix = "lib";
                        outputExtension = ".so";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        outputPrefix = "lib";
                        outputExtension = ".a";
                        break;
                }
            }
            else
            {
                throw new NotSupportedException("Unknown target platform for project output path.");
            }
            return outputPrefix + GetName() + outputExtension;
        }

        /// <summary>
        /// Returns directory of the compilation result.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual string GetOutputDir(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            var outputDirectory = Path.Combine(BuildSystem.GetSdkLocation(), "Bin");
            if (configuration == TargetConfiguration.Release)
            {
                return outputDirectory;
            }
            return Path.Combine(outputDirectory, configuration.ToString());
        }

        /// <summary>
        /// Returns path to the import library (only actual for Windows DLLs), or null if no import library needs to be generated.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual string GetImportLibraryOutputPath(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            if (TargetInfo.IsMicrosoftPlatform(platform) && GetBuildType(platform, configuration) == ProjectBuildType.DynamicLibrary)
            {
                var importLibraryOutputDirectory = Path.Combine(BuildSystem.GetSdkLocation(), "lib", GetName());
                return configuration != TargetConfiguration.Release
                    ? importLibraryOutputDirectory + "." + configuration + ".lib"
                    : importLibraryOutputDirectory + ".lib";
            }
            return null;
        }

        /// <summary>
        /// Enumerates list of directories that contain header files.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual IEnumerable<string> EnumerateHeaderDirectories(TargetPlatform platform, TargetConfiguration configuration)
        {
            var projectSourceFiles = Path.Combine(GetLocation(), "Source");
            if (Directory.Exists(projectSourceFiles))
            {
                yield return projectSourceFiles;
            }
        }

        /// <summary>
        /// Enumerates list of files with source code, which would be added to generated project data.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual IEnumerable<ProjectFile> EnumerateSourceFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            // Adding collected project files.
            var projectSourceDirectory = Path.Combine(GetLocation(), "Source");
            if (Directory.Exists(projectSourceDirectory))
            {
                foreach (var sourceFile in Directory.EnumerateFiles(projectSourceDirectory, "*.*", SearchOption.AllDirectories))
                {
                    var sourceFileExtension = (Path.GetExtension(sourceFile) ?? "").ToLowerInvariant();
                    switch (sourceFileExtension)
                    {
                        case ".h":
                        case ".hh":
                        case ".hpp":
                        case ".hxx":
                            yield return new ProjectFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.HeaderFile : ProjectSourceFileType.SupportFile);
                            break;
                        case ".c":
                        case ".cc":
                        case ".cpp":
                        case ".cxx":
                            yield return new ProjectFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.SourceCode : ProjectSourceFileType.SupportFile);
                            break;
                        case ".s":
                        case ".asm":
                            yield return new ProjectFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.SourceCodeAssembler : ProjectSourceFileType.SupportFile);
                            break;
                        case ".m":
                        case ".mm":
                            yield return new ProjectFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.SourceCodeObjective : ProjectSourceFileType.SupportFile);
                            break;
                        case ".inl":
                            yield return new ProjectFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.InlineImplementation : ProjectSourceFileType.SupportFile);
                            break;
                        case ".rc":
                            if (TargetInfo.IsMicrosoftPlatform(platform))
                            {
                                yield return new ProjectFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.Cpp ? ProjectSourceFileType.ResourceScript : ProjectSourceFileType.SupportFile);
                            }
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
                                        yield return new ProjectFile(sourceFile, ProjectSourceFileType.SupportFile);
                                        break;
                                }
                            }
                            break;
                    }
                }
                // Adding project file as support file.
                yield return new ProjectFile(GetSource(), ProjectSourceFileType.SupportFile);
            }
        }

        /// <summary>
        /// Enumerates list of dependencies for this project.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual IEnumerable<Dependency> EnumerateDependencies(TargetPlatform platform, TargetConfiguration configuration)
        {
            var projectDependencyDirectory = Path.Combine(GetLocation(), "Dependencies");
            if (Directory.Exists(projectDependencyDirectory))
            {
                // Adding explicit dependencies.
                foreach (var dependencySourcePath in Directory.EnumerateFiles(projectDependencyDirectory, "*.gddep.cs", SearchOption.AllDirectories))
                {
                    Dependency dependency;
                    try
                    {
                        dependency = DependencyFactory.Create(dependencySourcePath);
                    }
                    catch (Exception exception)
                    {
                        Console.WriteLine($"Failed to load dependency {dependencySourcePath} with exception {exception}");
                        continue;
                    }
                    if (dependency.IsSupported[platform, configuration])
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
                yield return ProjectDependencyFactory.GetGoddamnCoreDependency();
                if (priority < ProjectPriority.EngineLevel)
                {
                    // Resolving Engine library dependency.
                    yield return ProjectDependencyFactory.GetGoddamnEngineDependency();
                }
            }
        }

        /// <summary>
        /// Enumerates list of additional preprocessor definitions added to this project.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
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
    }   // public class Project
    
    /// <summary>
    /// Represents a collection of cached data that was collected by project object.
    /// </summary>
    /// <inheritdoc />
    public class Project : Collector
    {
        public readonly bool IsBuildTool;

        /// <summary>
        /// Project filter.
        /// </summary>
        public readonly string Filter;

        /// <summary>
        /// Sources filter origin.
        /// </summary>
        public readonly string SourcesFilterOrigin;

        /// <summary>
        /// Project priority.
        /// </summary>
        public readonly ProjectPriority Priority;

        /// <summary>
        /// Build type.
        /// </summary>
        public readonly CollectorContainer<ProjectBuildType> BuildType;

        /// <summary>
        /// Output path.
        /// </summary>
        public readonly CollectorContainer<string> OutputPath;

        /// <summary>
        /// Import library output path.
        /// </summary>
        public readonly CollectorContainer<string> ImportLibraryOutputPath;

        /// <summary>
        /// List of project files.
        /// </summary>
        public readonly CollectorContainer<ProjectFile[]> Files;

        /// <summary>
        /// List of header directories.
        /// </summary>
        public readonly CollectorContainer<string[]> HeaderDirectories;

        /// <summary>
        /// List of project dependencies.
        /// </summary>
        public readonly CollectorContainer<Dependency[]> Dependencies;

        /// <summary>
        /// List of project predefined macros.
        /// </summary>
        public readonly CollectorContainer<ProjectMacro[]> Macros;

        /// <summary>
        /// Path the the generated project file.
        /// </summary>
        public string GeneratorOutputPath;

        /// <inheritdoc />
        public Project(ProjectEnumerator projectEnumerator)
            : base(projectEnumerator)
        {
            Filter = projectEnumerator.GetFilter();
            Priority = projectEnumerator.GetPriority();
            IsBuildTool = projectEnumerator is BuildToolProjectEnumerator;
            if (!IsBuildTool)
            {
                SourcesFilterOrigin = projectEnumerator.GetSourceFiltersOrigin();
                BuildType 
                    = new CollectorContainer<ProjectBuildType>(projectEnumerator.GetBuildType);
                OutputPath 
                    = new CollectorContainer<string>((p, c) => Path.Combine(projectEnumerator.GetOutputDir(p, c), projectEnumerator.GetOutputFileName(p, c)));
                ImportLibraryOutputPath 
                    = new CollectorContainer<string>(projectEnumerator.GetImportLibraryOutputPath);
                Files 
                    = new CollectorContainer<ProjectFile[]>((p, c) => projectEnumerator.EnumerateSourceFiles(p, c).ToArray());
                HeaderDirectories 
                    = new CollectorContainer<string[]>((p, c) => projectEnumerator.EnumerateHeaderDirectories(p, c).ToArray());
                Dependencies 
                    = new CollectorContainer<Dependency[]>((p, c) => projectEnumerator.EnumerateDependencies(p, c).ToArray());
                Macros 
                    = new CollectorContainer<ProjectMacro[]>((p, c) => projectEnumerator.EnumerateMacros(p, c).ToArray());
            }
            else
            {
                GeneratorOutputPath = ((BuildToolProjectEnumerator)projectEnumerator).GetProjectFile();
            }
        }

        /// <summary>
        /// Generates a strigified list of include paths.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <param name="separator">Separator string between include paths. ';' By default.</param>
        /// <returns>A strigified include paths.</returns>
        public string GenerateIncludePaths(TargetPlatform platform, TargetConfiguration configuration, string separator = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            // Adding header paths from this projects.
            var includePathesBuilder = new StringBuilder();
            foreach (var headerDirectory in HeaderDirectories[platform, configuration])
            {
                includePathesBuilder.Append(headerDirectory).Append(separator);
            }
            // Adding header paths from dependencies.
            foreach (var dependency in Dependencies[platform, configuration])
            {
                foreach (var dependencyIncludePath in dependency.HeaderDirectories[platform, configuration])
                {
                    includePathesBuilder.Append(dependencyIncludePath).Append(separator);

                }
            }
            return includePathesBuilder.ToString();
        }

        /// <summary>
        /// Generates a strigified list of linked libraries paths.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <param name="separator">Separator string between linked libraries paths. ';' By default.</param>
        /// <param name="filter">Linked library path filter.</param>
        /// <returns>A strigified list of linked libraries paths.</returns>
        public string GenerateLinkedLibrariesPaths(TargetPlatform platform, TargetConfiguration configuration, string separator = null, Func<string, string> filter = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }
            if (filter == null)
            {
                filter = s => s;
            }

            // Adding libraries from dependencies.
            var linkedLibraries = new StringBuilder();
            foreach (var projectDependency in Dependencies[platform, configuration])
            {
                foreach (var projectDependencyLibraryPath in projectDependency.LinkedLibraries[platform, configuration])
                {
                    linkedLibraries.Append(filter(projectDependencyLibraryPath.FilePath)).Append(separator);
                }
            }
            return linkedLibraries.ToString();
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
            foreach (var macro in Macros[platform, configuration])
            {
                macrosBuilder.Append(macro).Append(separator);
            }
            return macrosBuilder.ToString();
        }
    }   // class Project

    /// <summary>
    /// Represents a factory of projects.
    /// </summary>
    public static class ProjectFactory
    {
        /// <summary>
        /// Constructs new project instance and it's cached data.
        /// </summary>
        /// <param name="projectSourcePath">Path so source file of the project.</param>
        /// <returns>Created instance of cached project data.</returns>
        public static Project Create(string projectSourcePath)
        {
            return CollectorFactory<ProjectEnumerator, Project>.Create(projectSourcePath);
        }
    }   // class ProjectFactory
}   // namespace GoddamnEngine.BuildSystem
