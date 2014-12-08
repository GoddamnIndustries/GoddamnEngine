//! ==========================================================================================
//! Project.cs - project class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 24.11.2014 - Created by James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Types of file in project (sub)directory.
    /// </summary>
    public enum SourceFileType : byte
    {
        Unknown,
        SourceCode, 
        HeaderFile,
        InlineImplementation,
        SupportFile = InlineImplementation,
        ResourceScript
    }   // enum SourceFileType

    /// <summary>
    /// Represents build type of project.
    /// </summary>
    public enum ProjectBuildType : byte
    {
        Unknown,
        Application,
        StaticLibrary,
        DynamicLibrary
    }   // public enum ProjectBuildType

    /// <summary>
    /// Represents a simple source file added to project.
    /// </summary>
    public sealed class ProjectSourceFile
    {
        private string m_FileName;
        private SourceFileType m_FileType;
        private bool m_IsExcluded;

        /// <summary>
        /// Instantiates new source file.
        /// </summary>
        /// <param name="FileName">Name of file without directory.</param>
        /// <param name="FileType">Type of file.</param>
        /// <param name="IsExcluded">True if this file would be excluded from build.</param>
        public ProjectSourceFile(string FileName, SourceFileType FileType, bool IsExcluded)
        {
            m_FileName = FileName;
            m_FileType = FileType;
            m_IsExcluded = IsExcluded;
        }

        /// <summary>
        /// Returns name of this file.
        /// </summary>
        public string GetFileName()
        {
            return m_FileName;
        }

        /// <summary>
        /// Returns type of this file.
        /// </summary>
        public SourceFileType GetFileType()
        {
            return m_FileType;
        }

        /// <summary>
        /// Returns true if this source would be excluded from project build files.
        /// </summary>
        public bool GetIsExcluded()
        {
            return m_IsExcluded;
        }
    }   // class ProjectSourceFile

    /// <summary>
    /// Represents a simple preprocessor definition.
    /// </summary>
    public sealed class ProjectPreprocessorDefinition
    {
        private string m_Name = null;
        private string m_Value = null;

        /// <summary>
        /// Instantiates a new preprocessor definition.
        /// </summary>
        /// <param name="Name">Name of the definition.</param>
        /// <param name="Value">Value of the definition.</param>
        public ProjectPreprocessorDefinition(string Name, string Value = null)
        {
            m_Name = Name;
            m_Value = Value;
        }

        /// <summary>
        /// Returns name of this definition.
        /// </summary>
        public string GetName()
        {
            return m_Name;
        }

        /// <summary>
        /// Returns a value of this definition.
        /// </summary>
        public string GetValue()
        {
            return m_Value;
        }

        /// <summary>
        /// Returns stringified version of this difinition on format "NAME=VALUE".
        /// </summary>
        public override string ToString()
        {
            return (m_Value != null) ? string.Concat(m_Name, '=', m_Value) : m_Name;
        }
    }   // class ProjectPreprocessorDefinition

    /// <summary>
    /// Represents some block of code that requires separate build.
    /// </summary>
    public class Project
    {
        //! ------------------------------------------------------------------------------------------
        //! Fields.
        //! ------------------------------------------------------------------------------------------

        private string m_Location;

        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a new project.
        /// </summary>
        public Project() { }
        public Project(string Location) { m_Location = Location; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Returns the location of the project.
        /// </summary>
        public string GetLocation()
        {
            Debug.Assert(m_Location != null);

            return m_Location;
        }

        /// <summary>
        /// If is false, then this project is not added to solution project's list.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual bool GetIsSkipped(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return false;
        }

        /// <summary>
        /// Returns name of this project.
        /// By default, it is a name of directory, where project is located or name of the project file.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual string GetName(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string ProjectName = this.GetLocation();
            while (!string.IsNullOrEmpty(Path.GetExtension(ProjectName))) {
                ProjectName = Path.GetFileNameWithoutExtension(ProjectName);
            }

            return ProjectName;
        }

        /// <summary>
        /// Returns type of compiler/linker output.
        /// By default, it is dynamically-linked library.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual ProjectBuildType GetBuildType(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            return Target.IsDesktopPlatform(Platform) ? ProjectBuildType.DynamicLibrary : ProjectBuildType.StaticLibrary;
        }

        /// <summary>
        /// Returns path to which compilation result would be stored.
        /// By default, it is 'bin' subdirectory of GoddamnSDK installation location
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual string GetOutputPath(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            StringBuilder OutputPath = new StringBuilder(Path.Combine(BuildSystem.GetSDKLocation(), "bin", this.GetName(Platform, Configuration)));
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

        /// <summary>
        /// Returns path to which the import libary would be stored. Actual for Windows DLLs.
        /// By default, it is 'lib' subdirectory of GoddamnSDK installation location
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual string GetImportLibraryOutputPath(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            if (Target.IsWinAPIPlatform(Platform) && (this.GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary)) { 
                StringBuilder ImportLibraryOutputPath = new StringBuilder(Path.Combine(BuildSystem.GetSDKLocation(), "lib", this.GetName(Platform, Configuration)));
                if (Configuration != TargetConfiguration.Release) {
                    ImportLibraryOutputPath.Append('.').Append(Configuration);
                }

                return ImportLibraryOutputPath.Append(".lib").ToString();
            }

            return null;
        }

        /// <summary>
        /// Returns list of dependencies for this project.
        /// By default, dependencies are recursively listed in "_Dependencies" project subdirectories.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetDependencies(ref List<Dependency> ProjectDependencies, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string ProjectLocation = Path.GetFullPath(Path.GetDirectoryName(this.GetLocation()));
            foreach (string ProjectDependency in Directory.EnumerateFiles(ProjectLocation, "*.gddep.cs", SearchOption.AllDirectories)) {
                ProjectDependencies.Add(Dependency.CreateDependencyForSource(ProjectDependency));
            }
        }

        /// <summary>
        /// Returns list of addtional preprocessor definitions added to this project.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetPreprocessorDefinitions(ref List<ProjectPreprocessorDefinition> ProjectPreprocessorDefinitions, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            if (Target.IsWinAPIPlatform(Platform)) {
                if (this.GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary) {
                    ProjectPreprocessorDefinitions.Add(new ProjectPreprocessorDefinition("_WINDLL"));
                }

                // Something else here..
            }
        }

        /// <summary>
        /// Returns list of files with source code, which would be added to generated project data.
        /// By default, it scans all subdirectories on files with specifiec extensions:
        /// <list type="bullet">
        ///     <item><term>.H, .HH, .HPP, .HXX</term><description>C/C++ header files. Added as 'HeaderFile'.</description></item>
        ///     <item><term>.C, .CC, .CPP, .CXX</term><description>C/C++ source files. Added as 'SourceCode'.</description></item>
        ///     <item><term>.M, .MM, .SWIFT</term><description>Objective-C(++)/Swift source files. On Cocoa platforms added as 'SourceCode', 'SupportFile' on others.</description></item>
        ///     <item><term>.JS</term><description>Javascript source files. On Web platforms added as 'SourceCode', 'SupportFile' on others.</description></item>
        ///     <item><term>.INL</term><description>C/C++ inlined source code. Added as 'InlineImplementation'.</description></item>
        ///     <item><term>.RC</term><description>C/C++ resource scripts. Added as 'ResourceScript' on WinAPI platforms, 'SupportFile' on others.</description></item>
        ///     <item><term>.GD*.CS</term><description>BuildSystem project files. Added as 'SupportFile'.</description></item>
        /// </list>
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetSourceFiles(ref List<ProjectSourceFile> ProjectSourceFiles, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            string ProjectLocation = Path.GetFullPath(Path.GetDirectoryName(this.GetLocation()));
            foreach (string ProjectSubdirectory in Directory.EnumerateDirectories(ProjectLocation, "*", SearchOption.AllDirectories)) {
                if (ProjectSubdirectory.Substring(ProjectLocation.Length).IndexOf('_') != -1) {
                    continue;
                }

                foreach (string ProjectSourceFile in Directory.EnumerateFiles(ProjectSubdirectory)) {
                    string ProjectSourceFileExtension = Path.GetExtension(ProjectSourceFile).ToLower();
                    string ProjectSourceFileName = Path.GetFileName(ProjectSourceFile);
                    
                    // Determining a type of a file.
                    SourceFileType ProjectFileType = SourceFileType.Unknown;
                    switch (ProjectSourceFileExtension) {
                        case ".h":
                        case ".hh":
                        case ".hpp":
                        case ".hxx":
                            ProjectFileType = SourceFileType.HeaderFile;
                            break;

                        case ".c":
                        case ".cc":
                        case ".cpp":
                        case ".cxx":
                            ProjectFileType = SourceFileType.SourceCode;
                            break;

                        case ".m":
                        case ".mm":
                        case ".swift":
                            ProjectFileType = Target.IsCocoaPlatform(Platform) ? SourceFileType.SourceCode : SourceFileType.SupportFile;
                            break;

                        case ".js":
                            ProjectFileType = Target.IsWebPlatform(Platform) ? SourceFileType.SourceCode : SourceFileType.SupportFile;
                            break;

                        case ".inl":
                            ProjectFileType = SourceFileType.InlineImplementation;
                            break;

                        case ".rc":
                            ProjectFileType = Target.IsWinAPIPlatform(Platform) ? SourceFileType.ResourceScript : SourceFileType.SupportFile;
                            break;

                        case ".cs":
                            string ProjectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(ProjectSourceFile)).ToLower();
                            if (!string.IsNullOrEmpty(ProjectFileSecondExtension)) {
                                switch (ProjectFileSecondExtension) {
                                    case ".gdproj":
                                    case ".gddep":
                                    case ".gdsln":
                                        ProjectFileType = SourceFileType.SupportFile;
                                        break;
                                }
                            }

                            break;
                    }

                    if (ProjectFileType != SourceFileType.Unknown) {
                        // Trying to determine, if file is excluded by platform/configuration filters.
                        bool ProjectFileIsExcluded = false;
                        if (ProjectFileType != SourceFileType.SupportFile) {
                            string ProjectFileExtensionless = Path.GetFileNameWithoutExtension(ProjectSourceFile);
                            string ProjectPlatform = Path.GetExtension(ProjectFileExtensionless);
                            if (!string.IsNullOrEmpty(ProjectPlatform)) {
                                ProjectFileIsExcluded = (ProjectPlatform != string.Concat('.', Platform));
                            }
                        }

                        // Finally, adding file to project.
                        if (ProjectFileType != SourceFileType.Unknown) {
                            ProjectSourceFiles.Add(new ProjectSourceFile(ProjectSourceFile, ProjectFileType, ProjectFileIsExcluded));
                        }
                    }
                }   
            }
        }

        //! ------------------------------------------------------------------------------------------
        //! Class static API.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Creates a Project object for specified configuration file.
        /// </summary>
        /// <param name="ProjectFile">Name of the project file, or path to directory in which project would be located. SCANNED NOT RECURSIVELY!</param>
        internal static Project CreateProjectForSource(string ProjectFile)
        {
            Project ProjectObject = CSharpCompiler.InstantiateSourceFile<Project>(ProjectFile);
            ProjectObject.m_Location = ProjectFile;

            return ProjectObject;
        }
    }   // class Project

    /// <summary>
    /// Reprents C# project that would be also added to solution on Visual Studio platforms.
    /// </summary>
    internal sealed class CSharpProject : Project
    {
        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a new C# project.
        /// </summary>
        /// <param name="Location">Location, where the project files exist.</param>
        internal CSharpProject(string Location = null) : base() { }
    }   // class CSharpProject
}   // namespace GoddamnEngine.BuildSystem
