//! ==========================================================================================
//! Project.cs - project class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    //! Base class for both (Objective-)C(++) and C# projects.
    public class BaseProject
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        // Made 'protected' to be accessible to 'CreateProject' method.
        protected string m_Location;

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Constructors.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a new project.
        protected BaseProject() { }
        protected BaseProject(string Location)
        {
            m_Location = Location;
        }

        //! @}

        //! Returns the location of the project.
        public string GetLocation()
        {
            Debug.Assert(m_Location != null);

            return m_Location;
        }

        //! ------------------------------------------------------------------------------------------
        //! @name Properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! If is false, then this project is not added to solution project's list.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual bool GetIsSkipped(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return false;
        }

        //! Returns name of this project.
        //! By default, it is a name of directory, where project is located or name of the project file.
        //! @param Platform Platform for which resolving is done.
        public virtual string GetName(TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            string ProjectName = this.GetLocation();
            while (!string.IsNullOrEmpty(Path.GetExtension(ProjectName))) {
                ProjectName = Path.GetFileNameWithoutExtension(ProjectName);
            }

            return ProjectName;
        }
    }   // class BaseProject

    //! Types of file in project (sub)directory.
    public enum ProjectSourceFileType : byte
    {
        Unknown,
        SourceCode,                         //!< .C, .CPP and other files with source code.
        HeaderFile,                         //!< .H, .HPP and other files with source code.
        InlineImplementation,               //!< .INL files.
        ResourceScript,                     //!< .RC files on Windows, etc.
        SupportFile = InlineImplementation, //!< Some other file types.
    }   // enum SourceFileType

    //! Represents build type of project.
    public enum ProjectBuildType : byte
    {
        Unknown,
        Application,                        //!< Project represents an executable.
        StaticLibrary,                      //!< Project represents a static library.
        DynamicLibrary                      //!< Project represents a dynamic library (where is supported) and static library otherwise.
    }   // public enum ProjectBuildType

    //! Represents a simple source file added to project.
    public sealed class ProjectSourceFile
    {
        private string m_FileName;
        private ProjectSourceFileType m_FileType;
        private bool m_IsExcluded;

        //! Instantiates new source file.
        //! @param FileName Name of file without directory.
        //! @param FileType Type of file.
        //! @param IsExcluded True if this file would be excluded from build.
        public ProjectSourceFile(string FileName, ProjectSourceFileType FileType, bool IsExcluded)
        {
            m_FileName = FileName;
            m_FileType = FileType;
            m_IsExcluded = IsExcluded;
        }

        //! Returns name of this file.
        public string GetFileName()
        {
            return m_FileName;
        }

        //! Returns type of this file.
        public ProjectSourceFileType GetFileType()
        {
            return m_FileType;
        }

        //! Returns true if this source would be excluded from project build files.
        public bool GetIsExcluded()
        {
            return m_IsExcluded;
        }
    }   // class ProjectSourceFile

    //! Represents a simple preprocessor definition.
    public sealed class ProjectPreprocessorDefinition
    {
        private string m_Name = null;
        private string m_Value = null;

        //! Instantiates a new preprocessor definition.
        //! @param Name Name of the definition.
        //! @param Value Value of the definition.
        public ProjectPreprocessorDefinition(string Name, string Value = null)
        {
            m_Name = Name;
            m_Value = Value;
        }

        //! Returns name of this definition.
        public string GetName()
        {
            return m_Name;
        }

        //! Returns a value of this definition.
        public string GetValue()
        {
            return m_Value;
        }

        //! Returns stringified version of this definition on format "NAME=VALUE".
        public override string ToString()
        {
            return (m_Value != null) ? string.Concat(m_Name, '=', m_Value) : m_Name;
        }
    }   // class ProjectPreprocessorDefinition


    //! Represents some block of (Objective-)C(++) code that requires separate build.
    public class Project : BaseProject
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Constructors.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a new project.
        public Project() { }
        public Project(string Location)
            : base(Location)
        {
        }

        //! ------------------------------------------------------------------------------------------
        //! @name Properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Returns type of compiler/linker output.
        //! By default, it is dynamically-linked library where they are supported or static libraries.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual ProjectBuildType GetBuildType(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return Target.IsDesktopPlatform(Platform) ? ProjectBuildType.DynamicLibrary : ProjectBuildType.StaticLibrary;
        }

        //! Returns path to which compilation result would be stored.
        //! By default, it is 'bin' subdirectory of GoddamnSDK installation location
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual string GetOutputPath(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            var OutputPath = new StringBuilder(Path.Combine(BuildSystem.GetSDKLocation(), "bin", this.GetName(Platform)));
            if (Configuration != TargetConfiguration.Release) {
                OutputPath.Append('.').Append(Configuration);
            }

            if (Target.IsWebPlatform(Platform)) {
                OutputPath.Append(".js");
            } else if (Target.IsWinAPIPlatform(Platform)) {
                switch (this.GetBuildType(Platform, Configuration)) {
                    case ProjectBuildType.Application:
                        OutputPath.Append(".exe");
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        OutputPath.Append(".dll");
                        break;
                    case ProjectBuildType.StaticLibrary:
                        OutputPath.Append(".lib");
                        break;
                    default:
                        throw new NotImplementedException();
                }
            } else if (Target.IsPosixPlatform(Platform)) {
                switch (this.GetBuildType(Platform, Configuration)) {
                    case ProjectBuildType.Application:
                        switch (Platform) {
                            case TargetPlatform.iOS:
                            case TargetPlatform.OSX:
                                // On OS X there is something like ".app" directory-file, and some other shit...
                                throw new NotImplementedException();

                            default:
                                // No extension for executables.
                                break;
                        }
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        OutputPath.Append(".so");
                        break;
                    case ProjectBuildType.StaticLibrary:
                        OutputPath.Append(".a");
                        break;
                    default:
                        throw new NotImplementedException();
                }
            } else {
                throw new NotImplementedException();
            }

            return OutputPath.ToString();
        }

        //! Returns path to which the import library would be stored. Actual for Windows DLLs.
        //! By default, it is 'lib' subdirectory of GoddamnSDK installation location
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual string GetImportLibraryOutputPath(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            if (Target.IsWinAPIPlatform(Platform) && (this.GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary)) {
                StringBuilder ImportLibraryOutputPath = new StringBuilder(Path.Combine(BuildSystem.GetSDKLocation(), "lib", this.GetName(Platform)));
                if (Configuration != TargetConfiguration.Release) {
                    ImportLibraryOutputPath.Append('.').Append(Configuration);
                }

                return ImportLibraryOutputPath.Append(".lib").ToString();
            }

            return null;
        }

        //! Collects list of dependencies for this project.
        //! By default, dependencies are recursively listed in "_Dependencies" project subdirectories.
        //! @param Platform Platform for which resolving is done.
        public virtual List<Dependency> GetDependencies(TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            List<Dependency> ProjectDependencies = new List<Dependency>();

            string ProjectLocation = Path.GetFullPath(Path.GetDirectoryName(this.GetLocation()));
            foreach (string ProjectDependency in Directory.EnumerateFiles(ProjectLocation, "*.gddep.cs", SearchOption.AllDirectories)) {
                ProjectDependencies.Add(Dependency.CreateDependencyForSource(ProjectDependency));
            }

            return ProjectDependencies;
        }

        //! Collects list of additional preprocessor definitions added to this project.
        //! @param Platform Platform for which resolving is done.
        //! @param Configuration Configuration for which resolving is done.
        public virtual List<ProjectPreprocessorDefinition> GetPreprocessorDefinitions(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            List<ProjectPreprocessorDefinition> ProjectPreprocessorDefinitions = new List<ProjectPreprocessorDefinition>();

            ProjectPreprocessorDefinitions.Add(new ProjectPreprocessorDefinition(Configuration == TargetConfiguration.Debug ? "_DEBUG" : "NDEBUG"));
            if (Target.IsWinAPIPlatform(Platform)) {
                if (this.GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary) {
                    ProjectPreprocessorDefinitions.Add(new ProjectPreprocessorDefinition("_WINDLL"));
                }

                // Something else here..
            }

            return ProjectPreprocessorDefinitions;
        }

        //! Collects list of files with source code, which would be added to generated project data.
        //! By default, it scans all subdirectories on files with specific extensions:
        //! @li .H, .HH, .HPP, .HXX - C/C++ header files. Added as 'HeaderFile'. 
        //! @li .C, .CC, .CPP, .CXX - C/C++ source files. Added as 'SourceCode'. 
        //! @li .M, .MM, .SWIFT - Objective-C(++)/Swift source files. On Cocoa platforms added as 'SourceCode', 'SupportFile' on others. 
        //! @li .JS - Javascript source files. On Web platforms added as 'SourceCode', 'SupportFile' on others. 
        //! @li .INL - C/C++ inlined source code. Added as 'InlineImplementation'. 
        //! @li .RC - C/C++ resource scripts. Added as 'ResourceScript' on WinAPI platforms, 'SupportFile' on others. 
        //! @li .GD*.CS - BuildSystem project files. Added as 'SupportFile'. 
        //! @param Platform Platform for which resolving is done.
        public virtual List<ProjectSourceFile> GetSourceFiles(TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            List<ProjectSourceFile> ProjectSourceFiles = new List<ProjectSourceFile>();

            string ProjectLocation = Path.GetFullPath(Path.GetDirectoryName(this.GetLocation()));
            foreach (string ProjectSubdirectory in Directory.GetDirectories(ProjectLocation, "*", SearchOption.AllDirectories).Concat(new string[] { ProjectLocation })) {
                if (ProjectSubdirectory.Substring(ProjectLocation.Length).IndexOf('_') != -1) {
                    continue;
                }

                foreach (string ProjectSourceFile in Directory.EnumerateFiles(ProjectSubdirectory, "*.*")) {
                    string ProjectSourceFileExtension = Path.GetExtension(ProjectSourceFile).ToLower();
                    string ProjectSourceFileName = Path.GetFileName(ProjectSourceFile);

                    // Determining a type of a file.
                    ProjectSourceFileType ProjectFileType = ProjectSourceFileType.Unknown;
                    switch (ProjectSourceFileExtension) {
                        case ".h":
                        case ".hh":
                        case ".hpp":
                        case ".hxx":
                            ProjectFileType = ProjectSourceFileType.HeaderFile;
                            break;

                        case ".c":
                        case ".cc":
                        case ".cpp":
                        case ".cxx":
                            ProjectFileType = ProjectSourceFileType.SourceCode;
                            break;

                        case ".m":
                        case ".mm":
                        case ".swift":
                            ProjectFileType = Target.IsCocoaPlatform(Platform) ? ProjectSourceFileType.SourceCode : ProjectSourceFileType.SupportFile;
                            break;

                        case ".js":
                            ProjectFileType = Target.IsWebPlatform(Platform) ? ProjectSourceFileType.SourceCode : ProjectSourceFileType.SupportFile;
                            break;

                        case ".inl":
                            ProjectFileType = ProjectSourceFileType.InlineImplementation;
                            break;

                        case ".rc":
                            ProjectFileType = Target.IsWinAPIPlatform(Platform) ? ProjectSourceFileType.ResourceScript : ProjectSourceFileType.SupportFile;
                            break;

                        case ".cs":
                            string ProjectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(ProjectSourceFile)).ToLower();
                            if (!string.IsNullOrEmpty(ProjectFileSecondExtension)) {
                                switch (ProjectFileSecondExtension) {
                                    case ".gdproj":
                                    case ".gddep":
                                    case ".gdsln":
                                        ProjectFileType = ProjectSourceFileType.SupportFile;
                                        break;
                                }
                            }

                            break;
                    }

                    if (ProjectFileType != ProjectSourceFileType.Unknown) {
                        // Trying to determine, if file is excluded by platform/configuration filters and adding it to project.
                        ProjectSourceFiles.Add(new ProjectSourceFile(ProjectSourceFile, ProjectFileType, 
                            (ProjectFileType == ProjectSourceFileType.SourceCode) && (!Target.MatchesPlatformConfiguration(ProjectSourceFile, Platform, TargetConfiguration.Unknown))));
                    }
                }
            }

            return ProjectSourceFiles;
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Class static API.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Creates a Project object for specified configuration file.
        //! @param ProjectFile Name of the project file, or path to directory in which project would be located. SCANNED NOT RECURSIVELY!
        internal static Project CreateProjectForSource(string ProjectFile)
        {
            Project ProjectObject = CSharpCompiler.InstantiateSourceFile<Project>(ProjectFile);
            ProjectObject.m_Location = ProjectFile;

            return ProjectObject;
        }

        //! @}
    }   // class Project

    //! Represents C# project that would be also added to solution on Visual Studio platforms.
    internal sealed class CSharpProject : BaseProject
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Constructor.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a new C# project.
        //! @param Location Location, where the project files exist.
        internal CSharpProject(string Location = null) : base() { }

        //! @}
    }   // class CSharpProject
}   // namespace GoddamnEngine.BuildSystem
