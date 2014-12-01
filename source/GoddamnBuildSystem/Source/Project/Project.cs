//! ==========================================================================================
//! Project.cs - project class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 24.11.2014 - Created by James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using System.Collections.Specialized;

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
    /// Class represents some block of code that requires separate build.
    /// </summary>
    public partial class Project
    {
        //! ------------------------------------------------------------------------------------------
        //! Fields.
        //! ------------------------------------------------------------------------------------------

        private string m_Name = null;
        private string m_Location;

        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a new dependency.
        /// </summary>
        /// <param name="Location">Location, where the dependency is located.</param>
        protected Project(string Location = null) { m_Location = Location; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

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

            if (m_Name == null) {
                m_Name = Path.GetFileNameWithoutExtension(m_Location);
            }

            return m_Name;
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

            return ProjectBuildType.DynamicLibrary;
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

            List<Dependency> ProjectDependenciesRef = new List<Dependency>();
            Action<string> ProcessProjectDirectory = null;
            ProcessProjectDirectory = new Action<string>(ProjectDirectory => {
                foreach (string ProjectSubdirectory in Directory.EnumerateFiles(ProjectDirectory)) {
                    string ProjectSubdirectoryName = Path.GetFileName(ProjectSubdirectory);
                    if (ProjectSubdirectoryName.ToLower() == "_dependencies") {
                        Dependency.CreateDependenciesForDirectory(ref ProjectDependenciesRef, ProjectSubdirectory);
                    }
                }
            });

            ProcessProjectDirectory(Path.GetDirectoryName(m_Location));
            ProjectDependencies.AddRange(ProjectDependenciesRef);
        }

        /// <summary>
        /// Returns list of addtional preprocessor definitions added to this project.
        /// By default, dependencies are recursively listed in "_Dependencies" project subdirectories.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetPreprocessorDefinitions(ref List<ProjectPreprocessorDefinition> ProjectPreprocessorDefinitions, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

        }

        /// <summary>
        /// Returns list of files with source code, which would be added to generated project data.
        /// By default, it scans all subdirectories on files with specifiec extensions:
        /// <list>
        ///     <item>.H, .HH, .HPP, .HXX - C/C++ header files. Added as 'HeaderFile'.</item>
        ///     <item>.C, .CC, .CPP, .CXX - C/C++ source files. Added as 'SourceCode'.</item>
        ///     <item>.M, .MM, .SWIFT - Objective-C(++)/Swift source files. On Cocoa platforms added as 'SourceCode', 'SupportFile' on others.</item>
        ///     <item>.JS - Javascript source files. On Web platforms added as 'SourceCode', 'SupportFile' on others.</item>
        ///     <item>.INL - C/C++ inlined source code. Added as 'InlineImplementation'.</item>
        ///     <item>.RC - C/C++ resource scripts. Added as 'ResourceScript' on WinAPI platforms, 'SupportFile' on others.</item>
        ///     <item>.GD*.CS - BuildSystem project files. Added as 'SupportFile'.</item>
        /// </list>
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        /// <param name="Configuration">Configuration for which resolving is done.</param>
        public virtual void GetSourceFiles(ref MultiTree<ProjectSourceFile> SourceFiles, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);
            Debug.Assert(TargetConfiguration.Unknown != Configuration);

            Stack<MultiTree<ProjectSourceFile>.Node> ProjectSources = new Stack<MultiTree<ProjectSourceFile>.Node>();
            ProjectSources.Push(SourceFiles.GetRootNode());

            Action<string> ProcessProjectDirectory = null;
            ProcessProjectDirectory = new Action<string>(ProjectDirectory => {
                // Processing all source files in current directory.
                foreach (string ProjectFile in Directory.EnumerateFiles(ProjectDirectory)) {
                    string ProjectFileExtension = Path.GetExtension(ProjectFile).ToLower();
                    string ProjectFileName = Path.GetFileName(ProjectFile);
                    
                    // Determining a type of a file.
                    SourceFileType ProjectFileType = SourceFileType.Unknown;
                    switch (ProjectFileExtension) {
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
                            string ProjectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(ProjectFile)).ToLower();
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

                    // Trying to determine, if file is excluded by platform/configuration filters.
                    bool ProjectFileIsExcluded = false;
                    if ((ProjectFileType != SourceFileType.Unknown) && (ProjectFileType != SourceFileType.SupportFile)) {
                        string ProjectFileExtensionless = Path.GetFileNameWithoutExtension(ProjectFile);
                        string ProjectPlatform = Path.GetExtension(ProjectFileExtensionless);
                        if (!string.IsNullOrEmpty(ProjectPlatform)) {
                            ProjectFileIsExcluded = (ProjectPlatform != string.Concat('.', Platform));
                        }
                    }

                    // Finally, adding file to project.
                    if (ProjectFileType != SourceFileType.Unknown) {
                        ProjectSources.Peek().GetElements().Add(new ProjectSourceFile(ProjectFileName, ProjectFileType, ProjectFileIsExcluded));
                    }
                }

                // Processing all subdirectories in this directory.
                foreach (string ProjectSubdirectory in Directory.EnumerateFiles(ProjectDirectory)) {
                    string ProjectSubdirectoryName = Path.GetFileName(ProjectSubdirectory);

                    // Checking is this subdir is service subdir.
                    if (ProjectSubdirectoryName[0] == '_') { 
                        switch (ProjectSubdirectoryName.ToLower()) {
                            case "_build":
                            case "_dependencies":
                            case "_dllresources":
                                break;

                            default:
                                continue;
                        }
                    }

                    ProjectSources.Push(ProjectSources.Peek().CreateChildNode(ProjectSubdirectoryName));
                    ProcessProjectDirectory(ProjectSubdirectory);
                    ProjectSources.Pop();
                }
            });

            ProcessProjectDirectory(Path.GetDirectoryName(m_Location));
        }

        //! ------------------------------------------------------------------------------------------
        //! Some other shit.
        //! ------------------------------------------------------------------------------------------

        private int _Checksumm = 0;

        private string _Location = null;
        private StringCollection _Folders = new StringCollection();
        private List<ProjectSourceFile> _SourceFiles = new List<ProjectSourceFile>();
        private List<Dependency> _Dependencies = new List<Dependency>();
        private StringCollection _PreprocessorDefinitions = new StringCollection();

        public int Checksumm { get { return this._Checksumm; } }
        public string Location { get { return this._Location; } }
        public StringCollection Folders { get { return this._Folders; } }
        public List<ProjectSourceFile> SourceFiles { get { return this._SourceFiles; } }
        public List<Dependency> Dependencies { get { return this._Dependencies; } }
        public StringCollection PreprocessorDefinitions { get { return this._PreprocessorDefinitions; } }

        //public abstract string Name { get; }
        //public abstract bool IsPlugin { get; }
        //public abstract ProjectBuildType BuildType { get; }

        public virtual string RelativeOutputPath { get { return ""; } }

        protected virtual void InitializeSelf()
        {
        }

        internal static Project CreateProject(string ProjectFile)
        {
            throw new NotImplementedException();
        }
    }   // public class TheProject
}   // namespace GoddamnEngine.BuildSystem
