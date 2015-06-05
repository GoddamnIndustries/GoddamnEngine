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
        BuildToolLevel,
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
        SupportFile = InlineImplementation, //!< Some other file types.
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
        CS,
        CPP
    }   // public enum ProjectLanguge

    // ------------------------------------------------------------------------------------------
    //! Represents a simple source file added to project.
    public sealed class ProjectSourceFile
    {
        // ------------------------------------------------------------------------------------------
        //! Returns true if this file is excluded on some platform.
        //! @returns True if this file is excluded on some platform.
        public delegate bool IsExcludedDelegate(TargetPlatform Platform);

        private readonly static IsExcludedDelegate NotExcludedDelegate = Platform => false;
        private readonly IsExcludedDelegate IsExcluded;
        public readonly string FileName;
        public readonly ProjectSourceFileType FileType;

        // ------------------------------------------------------------------------------------------
        //! Initializes new source file.
        //! @param FileName Path to source file.
        //! @param FileType Type of source file.
        //! @param IsExcluded Platform-dependent delegate.
        public ProjectSourceFile(string FileName, ProjectSourceFileType FileType, IsExcludedDelegate IsExcluded = null)
        {
            this.FileName = FileName;
            this.FileType = FileType;
            this.IsExcluded = IsExcluded ?? NotExcludedDelegate;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns true if object on specified path has platform/configuration data and matches it.
        public bool ShouldBeExcluded(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            if (!IsExcluded(Platform))
            {
                return !Collector.MatchesPlatformConfiguration(FileName, Platform, Configuration);
            }

            return true;
        }
    }   // class ProjectSourceFile

    // ------------------------------------------------------------------------------------------
    //! Represents a simple preprocessor definition.
    public sealed class ProjectMacro
    {
        private readonly string Name;
        private readonly string Value;

        // ------------------------------------------------------------------------------------------
        //! Initializes a new preprocessor definition.
        //! @param Name Name of the macro.
        //! @param Value Value of the macro.
        public ProjectMacro(string Name, string Value = null)
        {
            this.Name = Name;
            this.Value = Value;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns string version of this definition in format "NAME=VALUE".
        //! @returns string version of this definition in format "NAME=VALUE".
        public override string ToString()
        {
            return (Value != null) ? (Name + '=' + Value) : Name;
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
            return ProjectLanguge.CPP;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns name of the filter of this project in generated solution.
        //!        May return null if no filter is required.
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
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Type of compiler/linker output.
        public virtual ProjectBuildType GetBuildType(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            return Target.IsDesktopPlatform(Platform) ? ProjectBuildType.DynamicLibrary : ProjectBuildType.StaticLibrary;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path to which compilation result would be stored.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Path to which compilation result would be stored.
        public virtual string GetOutputPathDelegate(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            string OutputDirectory = Path.Combine(BuildSystem.GetSDKLocation(), "bin", this.GetName());
            string OutputExtension = null;
            if (Target.IsWebPlatform(Platform))
            {
                switch (GetBuildType(Platform, Configuration))
                {
                    case ProjectBuildType.Application:
                        OutputExtension = ".html";
                        break;
                    case ProjectBuildType.StaticLibrary:
                    case ProjectBuildType.DynamicLibrary:
                        OutputExtension = ".bc";
                        break;
                }
            }
            else if (Target.IsWinAPIPlatform(Platform))
            {
                switch (GetBuildType(Platform, Configuration))
                {
                    case ProjectBuildType.Application:
                        OutputExtension = ".exe";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        OutputExtension = ".dll";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        OutputExtension = ".lib";
                        break;
                    default:
                        throw new NotImplementedException();
                }
            }
            else if (Target.IsPosixPlatform(Platform))
            {
                switch (GetBuildType(Platform, Configuration))
                {
                    case ProjectBuildType.Application:
                        if (Target.IsCocoaPlatform(Platform))
                        {
                            // On OS X there is something like ".app" package, and some other shit...
                            throw new NotImplementedException();
                        }
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        OutputExtension = ".so";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        OutputExtension = ".a";
                        break;
                    default:
                        throw new NotImplementedException();
                }
            }
            else
            {
                throw new NotImplementedException();
            }

            return (Configuration != TargetConfiguration.Release
                ? OutputDirectory + "." + Configuration.ToString() + OutputExtension
                : OutputDirectory + OutputExtension
            );
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path to which the import library would be stored (only actual for Windows DLLs), or null if no import library needs to be generated.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Path to which the import library would be stored (only actual for Windows DLLs), or null if no import library needs to be generated.
        public virtual string GetImportLibraryOutputPathDelegate(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            if (Target.IsWinAPIPlatform(Platform) && (GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary))
            {
                string ImportLibraryOutputDirectory = Path.Combine(BuildSystem.GetSDKLocation(), "lib", this.GetName());
                return (Configuration != TargetConfiguration.Release
                    ? ImportLibraryOutputDirectory + "." + Configuration.ToString() + ".lib"
                    : ImportLibraryOutputDirectory + ".lib"
                );
            }

            return null;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns offset some path that should be treated as origin of source files (and filters of source files).
        //! @returns Offset some path that should be treated as origin of source files (and filters of source files).
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
            string ProjectSourceFiles = Path.Combine(GetLocation(), "Source");
            if (!Directory.Exists(ProjectSourceFiles))
            {
                throw new BuildSystemException("No source directories for dependencies {0} were found.", GetName());
            }

            yield return ProjectSourceFiles;
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of files with source code, which would be added to generated project data.
        //! @returns Iterator for list of files with source code, which would be added to generated project data.
        public virtual IEnumerable<ProjectSourceFile> EnumerateSourceFiles()
        {
            // Adding project file as support file.
            yield return new ProjectSourceFile(GetSource(), ProjectSourceFileType.SupportFile);

            // Adding collected project files.
            string ProjectSourceDirectory = Path.Combine(GetLocation(), "Source");
            if (!Directory.Exists(ProjectSourceDirectory))
            {
                throw new BuildSystemException("Project {0} does not contain \"Source\" directory.", GetName());
            }

            foreach (string SourceFile in Directory.EnumerateFiles(ProjectSourceDirectory, "*.*", SearchOption.AllDirectories))
            {
                string SourceFileName = Path.GetFileName(SourceFile);
                string SourceFileExtension = Path.GetExtension(SourceFile).ToLowerInvariant();
                switch (SourceFileExtension)
                {
                    case ".d":
                        yield return new ProjectSourceFile(SourceFile, GetProgrammingLanguage() == ProjectLanguge.D ? ProjectSourceFileType.HeaderFile : ProjectSourceFileType.SupportFile);
                        break;
                    case ".h":
                    case ".hh":
                    case ".hpp":
                    case ".hxx":
                        yield return new ProjectSourceFile(SourceFile, GetProgrammingLanguage() == ProjectLanguge.CPP ? ProjectSourceFileType.HeaderFile : ProjectSourceFileType.SupportFile);
                        break;
                    case ".c":
                    case ".cc":
                    case ".cpp":
                    case ".cxx":
                        yield return new ProjectSourceFile(SourceFile, GetProgrammingLanguage() == ProjectLanguge.CPP ? ProjectSourceFileType.SourceCode : ProjectSourceFileType.SupportFile);
                        break;
                    case ".m":
                    case ".mm":
                    case ".swift":
                        yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.SourceCode, Platform => !Target.IsCocoaPlatform(Platform));
                        break;
                    case ".js":
                        yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.SourceCode, Platform => !Target.IsWebPlatform(Platform));
                        break;
                    case ".inl":
                        yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.InlineImplementation);
                        break;
                    case ".rc":
                        yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.ResourceScript, Platform => !Target.IsWinAPIPlatform(Platform));
                        break;
                    case ".cs":
                        string ProjectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(SourceFile)).ToLowerInvariant();
                        if (!string.IsNullOrEmpty(ProjectFileSecondExtension))
                        {
                            switch (ProjectFileSecondExtension)
                            {
                                case ".gdproj":
                                case ".gddep":
                                case ".gdsln":
                                    yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.SupportFile);
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
            string ProjectDependencyDirectory = Path.Combine(GetLocation(), "Dependencies");
            if (Directory.Exists(ProjectDependencyDirectory))
            {
                // Adding explicit dependencies.
                foreach (string ProjectDependency in Directory.EnumerateFiles(ProjectDependencyDirectory, "*.gddep.cs", SearchOption.AllDirectories))
                {
                    DependencyCache Dependency = DependencyFactory.Create(ProjectDependency);
                    if (Dependency.IsSupported)
                    {
                        yield return Dependency;
                    }
                }
            }

#if INCLUDE_GODDAMNSDK_SPECIFIC
            // Adding implicit priority-related dependencies.
            ProjectPriority Priority = GetPriority();
            if (Priority < ProjectPriority.CoreLevel)
            {
                // Resolving Core library dependency.
                yield return DependencyFactory.GetGoddamnCoreDependency();

                if (Priority < ProjectPriority.EngineLevel)
                {
                    // Resolving Engine library dependency.
                    yield return DependencyFactory.GetGoddamnEngineDependency();
                }
            }
#endif  // INCLUDE_GODDAMNSDK_SPECIFIC
        }

        // ------------------------------------------------------------------------------------------
        //! Collects list of additional preprocessor definitions added to this project.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns Iterator for list of additional preprocessor definitions added to this project.
        public virtual IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            if (Target.IsWinAPIPlatform(Platform))
            {
                if (TargetConfigurationInfo.Get(Configuration).IsDebug)
                {
                    yield return new ProjectMacro("_DEBUG", "1");
                }
                else
                {
                    yield return new ProjectMacro("NDEBUG", "1");
                }

                if (GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary)
                {
                    yield return new ProjectMacro("_WINDLL");
                }
            }

            // Something else here..
        }

    }   // class Project

    // ------------------------------------------------------------------------------------------
    //! Representation of some build tool.
    public abstract class BuildToolProject : Project
    {
        // ------------------------------------------------------------------------------------------
        //! Returns a programming language, in which project is written in.
        //! @returns A programming language, in which project is written in.
        public override ProjectLanguge GetProgrammingLanguage()
        {
            return ProjectLanguge.CS;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path to project file of the build tool.
        //!        By default, it set to ".csproj" file with same name, as project.
        //! @returns Path to project file of the build tool.
        public virtual string GetProjectFile()
        {
            string CSProjFile = Path.Combine(GetLocation(), GetName() + ".csproj");
            if (File.Exists(CSProjFile))
            {
                return CSProjFile;
            }
            else
            {
                throw new BuildSystemException("No project files for build tool {0} where found.", GetName());
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Returns name of the filter of this project in generated solution.
        //!        By default, returns "Build tools".
        //! @returns Name of the filter of this project in generated solution.
        public override string GetFilter()
        {
            return "Build tools";
        }

        // ------------------------------------------------------------------------------------------
        //! Returns priority of this project. Higher priority - earlier project is compiled.
        //!        Obviously, build tool priority is constantly set to BuildToolLevel.
        //! @returns Returns priority of this project. Higher priority - earlier project is compiled.
        public sealed override ProjectPriority GetPriority()
        {
            return ProjectPriority.BuildToolLevel;
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override ProjectBuildType GetBuildType(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override string GetOutputPathDelegate(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            throw new NotSupportedException();
        }

        // ------------------------------------------------------------------------------------------
        //! @warning Method is not supported for a build tool.
        public sealed override string GetImportLibraryOutputPathDelegate(TargetPlatform Platform, TargetConfiguration Configuration)
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
        public sealed override IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            throw new NotSupportedException();
        }

    }   // class BuildToolProject

    // ------------------------------------------------------------------------------------------
    //! Represents a collection of cached data that was collected by project object.
    public abstract class ProjectCache : CollectorCache
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
        //! @param Project Project which dynamic properties would be cached.
        protected ProjectCache(Project Project)
            : base(Project)
        {
            if (IsSupported)
            {
                CachedFilter = Project.GetFilter();
                CachedPriority = Project.GetPriority();
                IsBuildTool = Project is BuildToolProject;
                if (!IsBuildTool)
                {
                    //  MakefilePathes = new CollectorContainer<string>();
                    CachedBuildTypes = new CollectorContainer<ProjectBuildType>(Project.GetBuildType);
                    CachedOutputPaths = new CollectorContainer<string>(Project.GetOutputPathDelegate);
                    CachedImportLibraryOutputPaths = new CollectorContainer<string>(Project.GetImportLibraryOutputPathDelegate);
                    CachedSourcesFilterOrigin = Project.GetSourceFiltersOrigin();
                    CachedSourceFiles = Project.EnumerateSourceFiles().ToArray();
                    CachedHeaderDirectories = Project.EnumerateHeaderDirectories().ToArray();
                    CachedDependencies = Project.EnumerateDependencies().ToArray();
                    CachedMacros = new CollectorContainer<ProjectMacro[]>((TargetPlatform P, TargetConfiguration C) => Project.EnumerateMacros(P, C).ToArray());
                }
                else
                {
                    GeneratorOutputPath = ((BuildToolProject)Project).GetProjectFile();
                }
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of include paths.
        //! @param Separator Separator string between include paths. ';' By default.
        //! @returns A strigified include paths.
        public string GenerateIncludePaths(string Separator = null)
        {
            if (Separator == null)
            {
                Separator = Path.PathSeparator.ToString();
            }

            StringBuilder IncludePathesBuilder = new StringBuilder(Separator);

            // Adding header paths from this projects.
            Array.ForEach(CachedHeaderDirectories, HeaderDirectory =>
                IncludePathesBuilder.Append(Separator).Append(HeaderDirectory)
            );

            // Adding header paths from dependencies.
            Array.ForEach(CachedDependencies, Dependency =>
                Array.ForEach(Dependency.CachedHeaderDirectories, DependencyIncludePath =>
                    IncludePathesBuilder.Append(Separator).Append(DependencyIncludePath)
                )
            );

            return IncludePathesBuilder.ToString();
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of macros.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @param Separator Separator string between macros. ';' By default.
        //! @returns A strigified list of macros.
        public string GenerateMacros(TargetPlatform Platform, TargetConfiguration Configuration, string Separator = null)
        {
            if (Separator == null)
            {
                Separator = Path.PathSeparator.ToString();
            }

            StringBuilder MacrosBuilder = new StringBuilder();
            Array.ForEach(CachedMacros[Platform, Configuration], Macro =>
                MacrosBuilder.Append(Macro.ToString()).Append(Separator)
            );

            return MacrosBuilder.ToString();
        }

        // ------------------------------------------------------------------------------------------
        //! Generates a strigified list of linked libraries paths.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @param Separator Separator string between linked libraries paths. ';' By default.
        //! @returns A strigified list of linked libraries paths.
        public string GenerateLinkedLibrariesPaths(TargetPlatform Platform, TargetConfiguration Configuration, string Separator = null)
        {
            if (Separator == null)
                Separator = Path.PathSeparator.ToString();

            // Adding libraries from dependencies.
            StringBuilder linkedLibraries = new StringBuilder();
            Array.ForEach(CachedDependencies, ProjectDependency =>
                Array.ForEach(ProjectDependency.CachedLinkedLibraries[Platform, Configuration], ProjectDependencyLibraryPath =>
                    linkedLibraries.Append(ProjectDependencyLibraryPath).Append(Separator)
                )
            );

            return linkedLibraries.ToString();
        }
    }   // class ProjectCache

    // ------------------------------------------------------------------------------------------
    //! Represents a factory of projects.
    public abstract class ProjectFactory : CollectorFactory<Project, ProjectCache>
    {
    }   // class ProjectFactory
}   // namespace GoddamnEngine.BuildSystem
