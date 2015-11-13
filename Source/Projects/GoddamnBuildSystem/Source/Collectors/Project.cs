// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Project.cs
//! Project class.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! Describes priority of the project.
    public enum ProjectPriority : byte
    {
        GameLevel,
        PluginLevel,
        EngineLevel,
        CoreLevel,
        BuildToolLevel
    }   // enum ProjectPriority

    // ------------------------------------------------------------------------------------------
    //! Types of file in project (sub)directory.
    public enum ProjectSourceFileType : byte
    {
        Unknown,
        SourceCode,             //!< .C, .CPP and other files with source code.
        HeaderFile,             //!< .H, .HPP and other files with source code.
        InlineImplementation,   //!< .INL files.
        ResourceScript,         //!< .RC files on Windows, etc.
        SupportFile = InlineImplementation //!< Some other file types.
    }   // enum SourceFileType

    // ------------------------------------------------------------------------------------------
    //! Represents build type of project.
    public enum ProjectBuildType : byte
    {
        Unknown,
        Application,            //!< Project represents an executable.
        StaticLibrary,          //!< Project represents a static library.
        DynamicLibrary          //!< Project represents a dynamic library (where is supported) and static library otherwise.
    }   // public enum ProjectBuildType

    // ------------------------------------------------------------------------------------------
    //! Represents a language in which project is written in.
    public enum ProjectLanguge : byte
    {
        D,
        Cs,
        Cpp
    }   // public enum ProjectLanguge

    // ------------------------------------------------------------------------------------------
    //! Represents a simple source file added to project.
    public sealed class ProjectSourceFile
    {
        // ------------------------------------------------------------------------------------------
        //! Returns true if this file is excluded on some platform.
        //! @param platform One of the target platforms.
        //! @returns True if this file is excluded on some platform.
        public delegate bool IsExcludedDelegate(TargetPlatform platform);

        private readonly static IsExcludedDelegate NotExcludedDelegate = platform => false;
        private readonly IsExcludedDelegate _isExcluded;
        public readonly string FileName;
        public readonly ProjectSourceFileType FileType;

        // ------------------------------------------------------------------------------------------
        //! Initializes new source file.
        //! @param fileName Path to source file.
        //! @param fileType Type of source file.
        //! @param isExcluded Platform-dependent delegate.
        public ProjectSourceFile(string fileName, ProjectSourceFileType fileType, IsExcludedDelegate isExcluded = null)
        {
            FileName = fileName;
            FileType = fileType;
            _isExcluded = isExcluded ?? NotExcludedDelegate;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns true if object on specified path has platform/configuration data and matches it.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        public bool ShouldBeExcluded(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (!_isExcluded(platform))
            {
                return !Collector.MatchesPlatformConfiguration(FileName, platform, configuration);
            }

            return true;
        }
    }   // class ProjectSourceFile

    // ------------------------------------------------------------------------------------------
    //! Represents a simple preprocessor definition.
    public sealed class ProjectMacro
    {
        private readonly string _name;
        private readonly string _value;

        // ------------------------------------------------------------------------------------------
        //! Initializes a new preprocessor definition.
        //! @param name Name of the macro.
        //! @param value Value of the macro.
        public ProjectMacro(string name, string value = null)
        {
            _name = name;
            _value = value;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns string version of this definition in format "NAME=VALUE".
        //! @returns string version of this definition in format "NAME=VALUE".
        public override string ToString()
        {
            return (_value != null) ? (_name + '=' + _value) : _name;
        }
    }   // class ProjectMacro

    // ------------------------------------------------------------------------------------------
    //! Representation of a normal C++ project.
    public class Project : Collector
    {
        // ------------------------------------------------------------------------------------------
        //! Returns a programming language, in which project is written in.
        //! @returns A programming language, in which project is written in.
        public virtual ProjectLanguge GetProgrammingLanguage()
        {
            return ProjectLanguge.Cpp;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns name of the filter of this project in generated solution.
        //! May return null if no filter is required.
        //! @returns Returns name of the filter of this project in generated solution.
        public virtual string GetFilter()
        {
            return null;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns priority of this project. Higher priority - earlier project is compiled.
        //! @returns Priority of this project. Higher priority - earlier project is compiled.
        public virtual ProjectPriority GetPriority()
        {
            return ProjectPriority.GameLevel;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns type of compiler/linker output.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Type of compiler/linker output.
        public virtual ProjectBuildType GetBuildType(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            return Target.IsDesktopPlatform(platform) ? ProjectBuildType.DynamicLibrary : ProjectBuildType.StaticLibrary;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path to which compilation result would be stored.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Path to which compilation result would be stored.
        public virtual string GetOutputPathDelegate(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            string outputDirectory = Path.Combine(BuildSystem.GetSdkLocation(), "bin", GetName());
            string outputExtension = null;
            if (Target.IsWebPlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        outputExtension = ".html";
                        break;
                    case ProjectBuildType.StaticLibrary:
                    case ProjectBuildType.DynamicLibrary:
                        outputExtension = ".bc";
                        break;
                }
            }
            else if (Target.IsWinApiPlatform(platform))
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
                    default:
                        throw new NotImplementedException();
                }
            }
            else if (Target.IsPosixPlatform(platform))
            {
                switch (GetBuildType(platform, configuration))
                {
                    case ProjectBuildType.Application:
                        if (Target.IsCocoaPlatform(platform))
                        {
                            // On OS X there is something like ".app" package, and some other shit...
                            throw new NotImplementedException();
                        }
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        outputExtension = ".so";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        outputExtension = ".a";
                        break;
                    default:
                        throw new NotImplementedException();
                }
            }
            else
            {
                throw new NotImplementedException();
            }

            return (configuration != TargetConfiguration.Release
                ? outputDirectory + "." + configuration + outputExtension
                : outputDirectory + outputExtension
            );
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path to which the import library would be stored (only actual for Windows DLLs), or null if no import library needs to be generated.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Path to which the import library would be stored (only actual for Windows DLLs), or null if no import library needs to be generated.
        public virtual string GetImportLibraryOutputPathDelegate(TargetPlatform platform, TargetConfiguration configuration)
        {
            Debug.Assert(platform != TargetPlatform.Unknown);
            if (Target.IsWinApiPlatform(platform) && (GetBuildType(platform, configuration) == ProjectBuildType.DynamicLibrary))
            {
                var importLibraryOutputDirectory = Path.Combine(BuildSystem.GetSdkLocation(), "lib", GetName());
                return (configuration != TargetConfiguration.Release
                    ? importLibraryOutputDirectory + "." + configuration + ".lib"
                    : importLibraryOutputDirectory + ".lib"
                );
            }

            return null;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns offset some path that should be treated as origin of source files (and filters of source files).
        //! @returns offset some path that should be treated as origin of source files (and filters of source files).
        public virtual string GetSourceFiltersOrigin()
        {
#if INCLUDE_GODDAMNSDK_SPECIFIC
            return Path.Combine(GetLocation(), "Source", "GoddamnEngine");
#else   // INCLUDE_GODDAMNSDK_SPECIFIC
            return GetLocation();
#endif  // INCLUDE_GODDAMNSDK_SPECIFIC
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of directories that contain header files.
        //! @returns Iterator for list of directories that contain header files.
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            var projectSourceFiles = Path.Combine(GetLocation(), "Source");
            if (!Directory.Exists(projectSourceFiles))
            {
                throw new BuildSystemException("No source directories for dependencies {0} were found.", GetName());
            }

            yield return projectSourceFiles;
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of files with source code, which would be added to generated project data.
        //! @returns Iterator for list of files with source code, which would be added to generated project data.
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
                switch (sourceFileExtension)
                {
                    case ".d":
                        yield return new ProjectSourceFile(sourceFile, GetProgrammingLanguage() == ProjectLanguge.D ? ProjectSourceFileType.HeaderFile : ProjectSourceFileType.SupportFile);
                        break;
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
                    case ".m":
                    case ".mm":
                    case ".swift":
                        yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.SourceCode, platform => !Target.IsCocoaPlatform(platform));
                        break;
                    case ".js":
                        yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.SourceCode, platform => !Target.IsWebPlatform(platform));
                        break;
                    case ".inl":
                        yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.InlineImplementation);
                        break;
                    case ".rc":
                        yield return new ProjectSourceFile(sourceFile, ProjectSourceFileType.ResourceScript, platform => !Target.IsWinApiPlatform(platform));
                        break;
                    case ".cs":
                        string projectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(sourceFile) ?? "").ToLowerInvariant();
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

        // ------------------------------------------------------------------------------------------
        //! Collects list of dependencies for this project.
        //! @returns Iterator for list of list of dependencies for this project.
        //! @note All unsupported projects should be filtered by this function.
        public virtual IEnumerable<DependencyCache> EnumerateDependencies()
        {
            string projectDependencyDirectory = Path.Combine(GetLocation(), "Dependencies");
            if (Directory.Exists(projectDependencyDirectory))
            {
                // Adding explicit dependencies.
                foreach (var dependency in Directory.EnumerateFiles(projectDependencyDirectory, "*.gddep.cs", SearchOption.AllDirectories)
                    .Select(DependencyFactory.Create).Where(dependency => dependency.IsSupported))
                {
                    yield return dependency;
                }
            }

#if INCLUDE_GODDAMNSDK_SPECIFIC
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
#if FALSE
                    if (priority < ProjectPriority.PluginLevel)
                    {
                        // IncredibleEngine is some kind of plug-in.
                        yield return DependencyFactory.GetIncredibleEngineDependency();
                    }
#endif  // FALSE
                }
            }
#endif  // INCLUDE_GODDAMNSDK_SPECIFIC
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of additional preprocessor definitions added to this project.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns Iterator for list of additional preprocessor definitions added to this project.
        public virtual IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform platform, TargetConfiguration configuration)
        {
            if (Target.IsWinApiPlatform(platform))
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

    // ------------------------------------------------------------------------------------------
    //! Representation of some build tool.
    public class BuildToolProject : Project
    {
        // ------------------------------------------------------------------------------------------
        //! Returns a programming language, in which project is written in.
        //! @returns A programming language, in which project is written in.
        public override ProjectLanguge GetProgrammingLanguage()
        {
            return ProjectLanguge.Cs;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path to project file of the build tool.
        //! By default, it set to ".csproj" file with same name, as project.
        //! @returns Path to project file of the build tool.
        public virtual string GetProjectFile()
        {
            var csProjFile = Path.Combine(GetLocation(), GetName() + ".csproj");
            if (File.Exists(csProjFile))
            {
                return csProjFile;
            }
            throw new BuildSystemException("No project files for build tool {0} where found.", GetName());
        }

        // ------------------------------------------------------------------------------------------
        //! Returns name of the filter of this project in generated solution.
        //! By default, returns "Build tools".
        //! @returns Name of the filter of this project in generated solution.
        public override string GetFilter()
        {
            return "Build tools";
        }

        // ------------------------------------------------------------------------------------------
        //! Returns priority of this project. Higher priority - earlier project is compiled.
        //! Obviously, build tool priority is constantly set to BuildToolLevel.
        //! @returns Returns priority of this project. Higher priority - earlier project is compiled.
        public sealed override ProjectPriority GetPriority()
        {
            return ProjectPriority.BuildToolLevel;
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override ProjectBuildType GetBuildType(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override string GetOutputPathDelegate(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override string GetImportLibraryOutputPathDelegate(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override string GetSourceFiltersOrigin()
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override IEnumerable<ProjectSourceFile> EnumerateSourceFiles()
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override IEnumerable<string> EnumerateHeaderDirectories()
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override IEnumerable<DependencyCache> EnumerateDependencies()
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform platform, TargetConfiguration configuration)
        {
            throw new NotSupportedException();
        }

    }   // class BuildToolProject

    // ------------------------------------------------------------------------------------------
    //! Represents a collection of cached data that was collected by project object.
    public class ProjectCache : CollectorCache
    {
        //public Dictionary<TargetPlatform, string> MakefilePathes;
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

        // ------------------------------------------------------------------------------------------
        //! Generates cache for specified project.
        //! @param project Project which dynamic properties would be cached.
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
                    //  MakefilePathes = new CollectorContainer<string>();
                    CachedBuildTypes = new CollectorContainer<ProjectBuildType>(project.GetBuildType);
                    CachedOutputPaths = new CollectorContainer<string>(project.GetOutputPathDelegate);
                    CachedImportLibraryOutputPaths = new CollectorContainer<string>(project.GetImportLibraryOutputPathDelegate);
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

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of include paths.
        //! @param separator Separator string between include paths. ';' By default.
        //! @returns A strigified include paths.
        public string GenerateIncludePaths(string separator = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            var includePathesBuilder = new StringBuilder();

            // Adding header paths from this projects.
            Array.ForEach(CachedHeaderDirectories, headerDirectory =>
                includePathesBuilder.Append(headerDirectory).Append(separator)
            );

            // Adding header paths from dependencies.
            Array.ForEach(CachedDependencies, dependency =>
                Array.ForEach(dependency.CachedHeaderDirectories, dependencyIncludePath =>
                    includePathesBuilder.Append(dependencyIncludePath).Append(separator)
                )
            );

            return includePathesBuilder.ToString();
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of macros.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @param separator Separator string between macros. ';' By default.
        //! @returns A strigified list of macros.
        public string GenerateMacros(TargetPlatform platform, TargetConfiguration configuration, string separator = null)
        {
            if (separator == null)
            {
                separator = Path.PathSeparator.ToString();
            }

            var macrosBuilder = new StringBuilder();
            Array.ForEach(CachedMacros[platform, configuration], macro =>
                macrosBuilder.Append(macro.ToString()).Append(separator)
            );

            return macrosBuilder.ToString();
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of linked libraries paths.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @param separator Separator string between linked libraries paths. ';' By default.
        //! @returns A strigified list of linked libraries paths.
        public string GenerateLinkedLibrariesPaths(TargetPlatform platform, TargetConfiguration configuration, string separator = null)
        {
            if (separator == null)
                separator = Path.PathSeparator.ToString();

            // Adding libraries from dependencies.
            var linkedLibraries = new StringBuilder();
            Array.ForEach(CachedDependencies, projectDependency =>
                Array.ForEach(projectDependency.CachedLinkedLibraries[platform, configuration], projectDependencyLibraryPath =>
                    linkedLibraries.Append(projectDependencyLibraryPath).Append(separator)
                )
            );

            return linkedLibraries.ToString();
        }
    }   // class ProjectCache

    // ------------------------------------------------------------------------------------------
    //! Represents a factory of projects.
    public class ProjectFactory : CollectorFactory<Project, ProjectCache>
    {
    }   // class ProjectFactory
}   // namespace GoddamnEngine.BuildSystem
