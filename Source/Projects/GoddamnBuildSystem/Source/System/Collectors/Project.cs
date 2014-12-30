//! ==========================================================================================
//! Project.cs - project class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
    //! Describes priority of the project.
    public enum ProjectPriority : byte
    {
        GameLevel,
        PluginLevel,
        EngineLevel,
        CoreLevel,
        BuildToolLevel,
    }   // enum ProjectPriority

    //! Types of file in project (sub)directory.
    public enum ProjectSourceFileType : byte
    {
        Unknown,
        SourceCode,       //!< .C, .CPP and other files with source code.
        HeaderFile,       //!< .H, .HPP and other files with source code.
        InlineImplementation,      //!< .INL files.
        ResourceScript,      //!< .RC files on Windows, etc.
        SupportFile = InlineImplementation, //!< Some other file types.
    }   // enum SourceFileType

    //! Represents build type of project.
    public enum ProjectBuildType : byte
    {
        Unknown,
        Application,      //!< Project represents an executable.
        StaticLibrary,       //!< Project represents a static library.
        DynamicLibrary       //!< Project represents a dynamic library (where is supported) and static library otherwise.
    }   // public enum ProjectBuildType

    //! Represents a simple source file added to project.
    public sealed class ProjectSourceFile
    {
        //! Returns true if this file is excluded on some platform.
        public delegate bool IsExcludedDelegate(TargetPlatform Platform);
        private readonly IsExcludedDelegate s_NotExcludedDelegate = (Platform => false);
        public readonly string m_FileName;
        public readonly ProjectSourceFileType m_FileType;
        public readonly IsExcludedDelegate m_IsExcludedDelegate;

        //! Instantiates new source file.
        public ProjectSourceFile(string FileName, ProjectSourceFileType FileType, IsExcludedDelegate IsExcluded = null)
        {
            m_FileName = FileName;
            m_FileType = FileType;
            m_IsExcludedDelegate = IsExcluded != null ? IsExcluded : s_NotExcludedDelegate;
        }

        //! Returns true if object on specified path has platform/configuration data and matches it.
        public bool ShouldBeExcluded(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            if (!m_IsExcludedDelegate(Platform)) {
                string ObjectPathExtensionless = Path.GetFileNameWithoutExtension(m_FileName);
                string ObjectPlatform = Path.GetExtension(ObjectPathExtensionless);
                if (!string.IsNullOrEmpty(ObjectPlatform)) {
                    // Checking if platform information is defined and it is of type TargetPlatform.
                    if (ObjectPlatform == string.Concat('.', Platform)) {
                        string LibraryConfiguration = Path.GetExtension(Path.GetFileNameWithoutExtension(ObjectPathExtensionless));
                        if (!string.IsNullOrEmpty(LibraryConfiguration)) {
                            // Checking if platform information is defined and it is of type TargetConfiguration.
                            return (LibraryConfiguration != string.Concat('.', Configuration));
                        }
                    } else {
                        // Library filename contains some platform information, but it not matches target platform.
                        return true;
                    }
                }

                return false;
            }

            return true;
        }
    }   // class ProjectSourceFile

    //! Represents a simple preprocessor definition.
    public sealed class ProjectMacro
    {
        public readonly string m_Name;
        public readonly string m_Value;

        //! Instantiates a new preprocessor definition.
        public ProjectMacro(string Name, string Value = null)
        {
            m_Name = Name;
            m_Value = Value;
        }

        //! Returns string version of this definition in format "NAME=VALUE".
        public override string ToString()
        {
            return (m_Value != null) ? (m_Name + '=' + m_Value) : m_Name;
        }
    }   // class ProjectMacro

    //! Representation of a normal C++ project. 
    public class Project : Collector
    {
        //! Returns path for specified configuration.
        public delegate string PathDelegate(TargetConfiguration Configuration);

        //! Returns name of the filter of this project in generated solution.
        //! May return null if no filter is required.
        public virtual string GetFilter()
        {
            return null;
        }

        //! Returns priority of this project. Higher priority - earlier project is compiled.
        public virtual ProjectPriority GetPriority()
        {
            return ProjectPriority.GameLevel;
        }

        //! Returns type of compiler/linker output.
        //! @param Platform One of the target platforms.
        public virtual ProjectBuildType GetBuildType(TargetPlatform Platform)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            return Target.IsDesktopPlatform(Platform) ? ProjectBuildType.DynamicLibrary : ProjectBuildType.StaticLibrary;
        }

        //! Returns path to which compilation result would be stored.
        //! @param Platform One of the target platforms.
        public virtual PathDelegate GetOutputPathDelegate(TargetPlatform Platform)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            string OutputDirectory = Path.Combine(BuildSystem.GetSDKLocation(), "bin", this.GetName());
            string OutputExtension = null;
            if (Target.IsWebPlatform(Platform)) {
                switch (GetBuildType(Platform)) {
                    case ProjectBuildType.Application:
                        OutputExtension = ".html";
                        break;
                    case ProjectBuildType.StaticLibrary:
                    case ProjectBuildType.DynamicLibrary:
                        OutputExtension = ".bc";
                        break;
                }
            } else if (Target.IsWinAPIPlatform(Platform)) {
                switch (GetBuildType(Platform)) {
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
            } else if (Target.IsPosixPlatform(Platform)) {
                switch (GetBuildType(Platform)) {
                    case ProjectBuildType.Application:
                        if (Target.IsCocoaPlatform(Platform)) {
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
            } else {
                throw new NotImplementedException();
            }

            return (Configuration => Configuration != TargetConfiguration.Release
             ? OutputDirectory + Configuration.ToString() + OutputExtension
             : OutputDirectory + OutputExtension
            );
        }

        //! Returns path to which the import library would be stored (only actual for Windows DLLs), or null if no import library needs to be generated.
        //! @param Platform One of the target platforms.
        public virtual PathDelegate GetImportLibraryOutputPathDelegate(TargetPlatform Platform)
        {
            Debug.Assert(Platform != TargetPlatform.Unknown);
            if (Target.IsWinAPIPlatform(Platform) && (GetBuildType(Platform) == ProjectBuildType.DynamicLibrary)) {
                string ImportLibraryOutputDirectory = Path.Combine(BuildSystem.GetSDKLocation(), "lib", this.GetName());
                return (Configuration => Configuration != TargetConfiguration.Release
                 ? ImportLibraryOutputDirectory + Configuration.ToString() + ".lib"
                 : ImportLibraryOutputDirectory + ".lib"
                );
            }

            return null;
        }

        //! Returns offset some path that should be treated as origin of source files (and filters of source files).
        public virtual string GetSourceFiltersOrigin()
        {
#if INCLUDE_GODDAMNSDK_SPECIFIC
            return Path.Combine(GetLocation(), "Source", "GoddamnEngine");
#else   // INCLUDE_GODDAMNSDK_SPECIFIC
   return GetLocation();
#endif  // INCLUDE_GODDAMNSDK_SPECIFIC
        }

        //! Collects list of directories that contain header files.
        public virtual IEnumerable<string> EnumerateHeaderDirectories()
        {
            string ProjectSourceFiles = Path.Combine(GetLocation(), "Source");
            if (Directory.Exists(ProjectSourceFiles)) {
                yield return ProjectSourceFiles;
            } else {
                throw new BuildSystemException("No source directories for dependencies {0} were found.", GetName());
            }
        }

        //! Collects list of files with source code, which would be added to generated project data.
        public virtual IEnumerable<ProjectSourceFile> EnumerateSourceFiles()
        {
            // Adding project file as support file.
            yield return new ProjectSourceFile(GetSource(), ProjectSourceFileType.SupportFile);

            // Adding collected project files.
            string ProjectSourceDirectory = Path.Combine(GetLocation(), "Source");
            if (!Directory.Exists(ProjectSourceDirectory)) {
                throw new BuildSystemException("Project {0} does not contain \"Source\" directory.", GetName());
            }

            foreach (string Subdirectory in Directory.GetDirectories(ProjectSourceDirectory, "*", SearchOption.AllDirectories)) {
                foreach (string SourceFile in Directory.EnumerateFiles(Subdirectory, "*.*")) {
                    string SourceFileName = Path.GetFileName(SourceFile);
                    string SourceFileExtension = Path.GetExtension(SourceFile).ToLowerInvariant();
                    switch (SourceFileExtension) {
                        case ".h":
                        case ".hh":
                        case ".hpp":
                        case ".hxx":
                            yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.HeaderFile);
                            break;
                        case ".c":
                        case ".cc":
                        case ".cpp":
                        case ".cxx":
                            yield return new ProjectSourceFile(SourceFile, ProjectSourceFileType.SourceCode);
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
                            if (!string.IsNullOrEmpty(ProjectFileSecondExtension)) {
                                switch (ProjectFileSecondExtension) {
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
        }

        //! Collects list of dependencies for this project.
        //! @note All unsupported projects should be filtered by this function.
        public virtual IEnumerable<DependencyCache> EnumerateDependencies()
        {
            string ProjectDependencyDirectory = Path.Combine(GetLocation(), "Dependencies");
            if (!Directory.Exists(ProjectDependencyDirectory)) {
                yield break;
            }

            foreach (string ProjectDependency in Directory.EnumerateFiles(ProjectDependencyDirectory, "*.gddep.cs", SearchOption.AllDirectories)) {
                DependencyCache Dependency = DependencyFactory.Create(ProjectDependency);
                if (Dependency.m_IsSupported) {
                    yield return Dependency;
                }
            }
        }

        //! Collects list of additional preprocessor definitions added to this project.
        //! @param Platform One of the target platforms.
        public virtual IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform Platform)
        {
            if (Target.IsWinAPIPlatform(Platform)) {
                if (GetBuildType(Platform) == ProjectBuildType.DynamicLibrary) {
                    yield return new ProjectMacro("_WINDLL");
                }
            }

            // Something else here..
        }

    }   // class Project

    //! Representation of some build tool. 
    public class BuildToolProject : Project
    {
        //! Returns path to project file of the build tool.
        //! By default, it set to ".csproj" file with same name, as project.
        public virtual string GetProjectFile()
        {
            string CSProjFile = Path.Combine(GetLocation(), GetName() + ".csproj");
            if (File.Exists(CSProjFile)) {
                return CSProjFile;
            } else {
                throw new BuildSystemException("No project files for build tool {0} where found.", GetName());
            }
        }

        //! Returns name of the filter of this project in generated solution.
        //! By default, returns "Build tools".
        public override string GetFilter()
        {
            return "Build tools";
        }

        //! Returns priority of this project. Higher priority - earlier project is compiled.
        //! Obviously, build tool priority is constantly set to BuildToolLevel.
        public sealed override ProjectPriority GetPriority()
        {
            return ProjectPriority.BuildToolLevel;
        }

        /// @warning Method is not supported for a build tool.
        public sealed override ProjectBuildType GetBuildType(TargetPlatform Platform)
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override PathDelegate GetOutputPathDelegate(TargetPlatform Platform)
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override PathDelegate GetImportLibraryOutputPathDelegate(TargetPlatform Platform)
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override string GetSourceFiltersOrigin()
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override IEnumerable<ProjectSourceFile> EnumerateSourceFiles()
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override IEnumerable<string> EnumerateHeaderDirectories()
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override IEnumerable<DependencyCache> EnumerateDependencies()
        {
            throw new NotSupportedException();
        }

        /// @warning Method is not supported for a build tool.
        public sealed override IEnumerable<ProjectMacro> EnumerateMacros(TargetPlatform Platform)
        {
            throw new NotSupportedException();
        }

    }   // class BuildToolProject

    //! Represents a collection of cached data that was collected by project object.
    public sealed class ProjectCache : CollectorCache
    {
        public Dictionary<TargetPlatform, string> m_MakefilePathes;
        public string m_GeneratorOutputPath;
        public readonly bool m_IsBuildTool;
        public readonly string m_CachedFilter;
        public readonly ProjectPriority m_CachedPriority;
        public readonly Dictionary<TargetPlatform, ProjectBuildType> m_CachedBuildTypes;
        public readonly Dictionary<TargetPlatform, Project.PathDelegate> m_CachedOutputPathDelegates;
        public readonly Dictionary<TargetPlatform, Project.PathDelegate> m_CachedImportLibraryOutputPathDelegates;
        public readonly string m_CachedSourcesFilterOrigin;
        public readonly ProjectSourceFile[] m_CachedSourceFiles;
        public readonly string[] m_CachedHeaderDirectories;
        public readonly DependencyCache[] m_CachedDependencies;
        public readonly Dictionary<TargetPlatform, ProjectMacro[]> m_CachedMacros;

        //! Generates cache for specified project.
        public ProjectCache(Project Project)
            : base(Project)
        {
            if (m_IsSupported) {
                m_CachedFilter = Project.GetFilter();
                m_CachedPriority = Project.GetPriority();
                m_IsBuildTool = Project is BuildToolProject;
                if (!m_IsBuildTool) {
                    m_MakefilePathes = new Dictionary<TargetPlatform, string>();
                    m_CachedBuildTypes = new Dictionary<TargetPlatform, ProjectBuildType>();
                    m_CachedOutputPathDelegates = new Dictionary<TargetPlatform, Project.PathDelegate>();
                    m_CachedImportLibraryOutputPathDelegates = new Dictionary<TargetPlatform, Project.PathDelegate>();
                    m_CachedSourcesFilterOrigin = Project.GetSourceFiltersOrigin();
                    m_CachedSourceFiles = Project.EnumerateSourceFiles().ToArray();
                    m_CachedHeaderDirectories = Project.EnumerateHeaderDirectories().ToArray();
                    m_CachedDependencies = Project.EnumerateDependencies().ToArray();
                    m_CachedMacros = new Dictionary<TargetPlatform, ProjectMacro[]>();//ProjectObject.EnumerateMacros().ToArray();

                    foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                        m_CachedBuildTypes.Add(Platform, Project.GetBuildType(Platform));
                        m_CachedOutputPathDelegates.Add(Platform, Project.GetOutputPathDelegate(Platform));
                        m_CachedImportLibraryOutputPathDelegates.Add(Platform, Project.GetImportLibraryOutputPathDelegate(Platform));
                        m_CachedMacros.Add(Platform, Project.EnumerateMacros(Platform).ToArray());
                    }
                } else {
                    m_GeneratorOutputPath = ((BuildToolProject)Project).GetProjectFile();
                }
            }
        }

        //! Generates a strigified list of include paths.
        public string GenerateIncludePaths(string Separator = null)
        {
            if (Separator == null) {
                Separator = Path.PathSeparator.ToString();
            }

            StringBuilder IncludePathesBuilder = new StringBuilder();

            // Adding header paths from this projects.
            Array.ForEach(m_CachedHeaderDirectories, HeaderDirectory =>
             IncludePathesBuilder.Append(HeaderDirectory).Append(Separator)
            );

            // Adding header paths from dependencies.
            Array.ForEach(m_CachedDependencies, Dependency =>
             Array.ForEach(Dependency.m_CachedHeaderDirectories, DependencyIncludePath =>
              IncludePathesBuilder.Append(DependencyIncludePath).Append(Separator)
             )
            );

            return IncludePathesBuilder.ToString();
        }

        //! Generates a strigified list of macros.
        //! @param Platform Some platform.
        public string GenerateMacros(TargetPlatform Platform, string Separator = null)
        {
            if (Separator == null) {
                Separator = Path.PathSeparator.ToString();
            }

            StringBuilder MacrosBuilder = new StringBuilder();
            Array.ForEach(m_CachedMacros[Platform], Macro =>
             MacrosBuilder.Append(Macro.ToString()).Append(Separator)
            );

            return MacrosBuilder.ToString();
        }

        //! Generates a strigified list of linked libraries paths.
        //! @param Platform Some platform.
        public string GenerateLinkedLibrariesPaths(TargetPlatform Platform, string Separator = null)
        {
            if (Separator == null) {
                Separator = Path.PathSeparator.ToString();
            }

            StringBuilder LinkedLibraries = new StringBuilder();

            // Adding libraries from dependencies.
            Array.ForEach(m_CachedDependencies, ProjectDependency =>
             Array.ForEach(ProjectDependency.m_CachedLinkedLibraries[Platform], ProjectDependencyLibraryPath =>
              LinkedLibraries.Append(ProjectDependencyLibraryPath).Append(Separator)
             )
            );

            return LinkedLibraries.ToString();
        }
    }   // class ProjectCache

    //! Represents a factory of projects.
    public /*static*/ sealed class ProjectFactory : CollectorFactory<Project, ProjectCache>
    {
    }   // class ProjectFactory
}   // namespace GoddamnEngine.BuildSystem
