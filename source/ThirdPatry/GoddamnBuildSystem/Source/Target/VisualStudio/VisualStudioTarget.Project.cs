//! ==========================================================================================
//! VisualStudioTarget.Project.cs - *.vcxproj files generator.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Xml;
using System.Text;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem.VisualStudio
{
    //! Generator of the project/solution files for Visual Studio.
    //! Implements ".GenerateProjectFiles()" method.
    partial class VisualStudioTarget
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        private const string s_MSBuild2003Namespace = @"http://schemas.microsoft.com/developer/msbuild/2003";

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Helper methods.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Converts specified source file type into name of the node in MSBuild.
        private static string ConvertFileTypeToElement(ProjectSourceFileType FileType)
        {
            switch (FileType) {
                case ProjectSourceFileType.SourceCode:
                    return "ClCompile";

                case ProjectSourceFileType.HeaderFile:
                    return "ClInclude";

                case ProjectSourceFileType.InlineImplementation:
                    return "None";

                case ProjectSourceFileType.ResourceScript:
                    return "ResourceCompile";

                default:
                    throw new NotImplementedException();
            }
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Generation interface.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Generates project files for Visual Studio 2013. 
        //! Implantation is weird: it simulates behavior of the Visual Studio's project generator.
        //! @note I do not really understand what parts of this code are really useful. And I ask masters to fix it.
        //! @param ProjectObject Parsed project object.
        //! @param ProjectPlatform Target platform of the object
        //! @param ProjectConfigurations Array of target configurations.
        //! @returns Path to main project file.
        internal sealed override string GenerateProjectFiles(Project ProjectObject, TargetPlatform Platform, TargetConfiguration[] Configurations)
        {
            string ProjectName = ProjectObject.GetName(Platform);
            string ProjectDirectoryPath = Path.Combine(base.GenerateProjectFiles(ProjectObject, Platform, Configurations), ProjectName);
         
            List<ProjectSourceFile> ProjectSourceFiles = ProjectObject.GetSourceFiles(Platform);
            List<Dependency> ProjectDependencies = ProjectObject.GetDependencies(Platform);

            string ProjectPlatformString = null;
            bool ProjectRequiresCLI = false;
            bool ProjectDoesNotRequiresRTTI = ProjectRequiresCLI;
            bool ProjectDoesNotRequiresExceptions = true;

            switch (Platform) {
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsStore:
                    ProjectPlatformString = "x64";
                    break;
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsPhone8:
                    ProjectPlatformString = "ARM";
                    break;
                default:
                    throw new NotImplementedException();
            }

            // ==========================================================================================
            // GENERATING VCPROJ FILES.
            string ProjectVCXProjPath = ProjectDirectoryPath + ".vcxproj";
            using (XmlTextWriter ProjectVCXProj = new XmlTextWriter(ProjectVCXProjPath, null)) {
                ProjectVCXProj.Formatting = Formatting.Indented;
                ProjectVCXProj.Indentation = 4;
                ProjectVCXProj.WriteStartDocument();
                ProjectVCXProj./**/WriteStartElement("Project", s_MSBuild2003Namespace);
                ProjectVCXProj./**//**/WriteAttributeString("ToolsVersion", m_ToolsVersion);
                ProjectVCXProj./**//**/WriteAttributeString("DefaultTargets", "Build");

                // ------------------------------------------------------------------------------------------
                // Defining list of CONFIGURATIONS.
                ProjectVCXProj.WriteStartElement("ItemGroup");
                ProjectVCXProj./**/WriteAttributeString("Label", "ProjectConfigurations");
                foreach (var ProjectConfiguration in Configurations) {
                    ProjectVCXProj.WriteStartElement("ProjectConfiguration");
                    ProjectVCXProj./**/WriteAttributeString("Include", string.Concat(ProjectConfiguration, '|', ProjectPlatformString));
                    ProjectVCXProj./**/WriteElementString("Configuration", ProjectConfiguration.ToString());
                    ProjectVCXProj./**/WriteElementString("Platform", ProjectPlatformString);
                    ProjectVCXProj.WriteEndElement();
                }
                ProjectVCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining list of SOURCE FILES.
                ProjectVCXProj.WriteStartElement("ItemGroup");
                foreach (var ProjectSourceFile in ProjectSourceFiles) {
                    ProjectVCXProj.WriteStartElement(VisualStudioTarget.ConvertFileTypeToElement(ProjectSourceFile.GetFileType()));
                    ProjectVCXProj.WriteAttributeString("Include", ProjectSourceFile.GetFileName());
                    if (ProjectSourceFile.GetIsExcluded()) {
                        ProjectVCXProj.WriteStartElement("ExcludedFromBuild");
                        ProjectVCXProj./**/WriteAttributeString("Condition", string.Format("'$(Platform)'=='{0}'", ProjectPlatformString));
                        ProjectVCXProj./**/WriteString("true");
                        ProjectVCXProj.WriteEndElement();
                    }
                    ProjectVCXProj.WriteEndElement();
                }
                ProjectVCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining GLOBAL PROJECT PROPERTIES.
                ProjectVCXProj.WriteStartElement("PropertyGroup");
                ProjectVCXProj./**/WriteAttributeString("Label", "Globals");
                ProjectVCXProj./**/WriteElementString("ProjectGuid", string.Format("{{{0}}}", Guid.NewGuid().ToString().ToUpper()));
                ProjectVCXProj./**/WriteElementString("RootNamespace", ProjectName);
                ProjectVCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Importing DEFAULT PROPERTIES for C++.
                ProjectVCXProj.WriteStartElement("Import");
                ProjectVCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                ProjectVCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining MAIN CONFIGURATION PROPERTIES.
                foreach (var Configuration in Configurations) {
                    ProjectVCXProj.WriteStartElement("PropertyGroup");
                    ProjectVCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", Configuration, ProjectPlatformString));
                    ProjectVCXProj./**/WriteAttributeString("Label", "Configuration");
                    ProjectVCXProj./**/WriteElementString("ConfigurationType", ProjectObject.GetBuildType(Platform, Configuration).ToString());
                    ProjectVCXProj./**/WriteElementString("PlatformToolset", m_PlatformToolsSet);
                    ProjectVCXProj./**/WriteElementString("CharacterSet", "MultiByte");
                    if (ProjectRequiresCLI) {
                        ProjectVCXProj./**/WriteElementString("CLRSupport", "true");
                    }
                    ProjectVCXProj.WriteEndElement();
                }

                // ------------------------------------------------------------------------------------------
                // Importing DEFAULT PROPERTIES for C++. I do not understand why are the default properties separated. 
                // According to comment in this file (it checks whether the platform is supported in this VS edition), this includes are not required.
                ProjectVCXProj.WriteStartElement("Import");
                ProjectVCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
                ProjectVCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Importing USER-DEFINED PROPERTIES for C++.
                foreach (var Configuration in Configurations) {
                    ProjectVCXProj.WriteStartElement("ImportGroup");
                    ProjectVCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", Configuration, ProjectPlatformString));
                    ProjectVCXProj./**/WriteAttributeString("Label", "PropertySheets");
                    ProjectVCXProj./**/WriteStartElement("Import");
                    ProjectVCXProj./**//**/WriteAttributeString("Project", @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                    ProjectVCXProj./**//**/WriteAttributeString("Condition", @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                    ProjectVCXProj./**//**/WriteAttributeString("Label", "LocalAppDataPlatform");
                    ProjectVCXProj./**/WriteEndElement();
                    ProjectVCXProj.WriteEndElement();
                }

                // ------------------------------------------------------------------------------------------
                // Importing USER MACROS. Since this node is empty, we just do not write it.
                ProjectVCXProj.WriteStartElement("PropertyGroup");
                ProjectVCXProj./**/WriteAttributeString("Label", "UserMacros");
                ProjectVCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining VC++ DIRECTORIES.
                foreach (var Configuration in Configurations) {
                    string ProjectOutputPath = ProjectObject.GetOutputPath(Platform, Configuration);

                    StringBuilder ProjectIncludePathes = new StringBuilder()
                        .Append("$(IncludePath)").Append(Path.PathSeparator)
                        .Append(Path.Combine(BuildSystem.GetSDKLocation(), "source")).Append(Path.PathSeparator);
                    ProjectDependencies.ForEach(ProjectDependency =>
                        ProjectDependency.GetHeaderPaths(Platform, Configuration).ForEach(ProjectDependencyIncludePath =>
                            ProjectIncludePathes.Append(ProjectDependencyIncludePath).Append(Path.PathSeparator)
                        )
                    );

                    ProjectVCXProj.WriteStartElement("PropertyGroup");
                    ProjectVCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", Configuration, ProjectPlatformString));
                    ProjectVCXProj./**/WriteElementString("TargetName", Path.GetFileNameWithoutExtension(ProjectOutputPath));
                    ProjectVCXProj./**/WriteElementString("OutDir", Path.GetDirectoryName(ProjectOutputPath) + Path.DirectorySeparatorChar);
                    ProjectVCXProj./**/WriteElementString("IncludePath", ProjectIncludePathes.ToString());
                    ProjectVCXProj.WriteEndElement();    // </ VC++ DIRECTORIES
                }

                // ------------------------------------------------------------------------------------------
                // Defining VC++ COMPILER and LINKER PROPERTIES.
                foreach (var Configuration in Configurations) {
                    bool ProjectIsDebugConfiguration = (Configuration == TargetConfiguration.Debug);

                    StringBuilder PreprocessorDefinitions = new StringBuilder()
                        .Append("%(PreprocessorDefinition)").Append(Path.PathSeparator);
                    if (ProjectDoesNotRequiresExceptions) {
                        PreprocessorDefinitions.Append("_HAS_EXCEPTIONS=0").Append(Path.PathSeparator);
                    }
                    ProjectObject.GetPreprocessorDefinitions(Platform, Configuration).ForEach(ProjectPreprocessorDefinition =>
                        PreprocessorDefinitions.Append(ProjectPreprocessorDefinition.ToString()).Append(Path.PathSeparator)
                    );

                    StringBuilder ProjectLibrariesPathes = new StringBuilder()
                        .Append("%(AdditionalDependencies)").Append(Path.PathSeparator)
                        .Append("winmm.lib").Append(Path.PathSeparator)
                        .Append("imm32.lib").Append(Path.PathSeparator)
                        .Append("version.lib").Append(Path.PathSeparator);
                    ProjectDependencies.ForEach(ProjectDependency =>
                        ProjectDependency.GetHeaderPaths(Platform, Configuration).ForEach(ProjectDependencyLibraryPath =>
                            ProjectLibrariesPathes.Append(ProjectDependencyLibraryPath).Append(Path.PathSeparator)
                        )
                    );

                    ProjectVCXProj.WriteStartElement("ItemDefinitionGroup");
                    ProjectVCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", Configuration, ProjectPlatformString));

                    // ------------------------------------------------------------------------------------------
                    // Defining VC++ COMPILER PROPERTIES.
                    ProjectVCXProj./**/WriteStartElement("ClCompile");
                    ProjectVCXProj./**//**/WriteElementString("PreprocessorDefinitions", PreprocessorDefinitions.ToString());
                    ProjectVCXProj./**//**/WriteElementString("Optimization", ProjectIsDebugConfiguration ? "Disabled" : "Full");
                    ProjectVCXProj./**//**/WriteElementString("WarningLevel", "Level4");
                    ProjectVCXProj./**//**/WriteElementString("TreatWarningAsError", "true");
                    ProjectVCXProj./**//**/WriteElementString("PrecompiledHeader", "NotUsing");
                    ProjectVCXProj./**//**/WriteElementString("CallingConvention", "FastCall");
                    ProjectVCXProj./**//**/WriteElementString("FavorSizeOrSpeed", "Speed");
                    if (ProjectDoesNotRequiresRTTI) {
                        ProjectVCXProj./**//**/WriteElementString("RuntimeTypeInfo", "false");
                    }
                    if (ProjectDoesNotRequiresExceptions) {
                        ProjectVCXProj./**//**/WriteElementString("ExceptionHandling", "false");
                    }
                    ProjectVCXProj./**/WriteEndElement();    // </ VC++ COMPILER PROPERTIES

                    // ------------------------------------------------------------------------------------------
                    // Defining VC++ LINKER PROPERTIES.
                    ProjectVCXProj./**/WriteStartElement("Link");
                    ProjectVCXProj./**//**/WriteElementString("AdditionalDependencies", ProjectLibrariesPathes.ToString());
                    ProjectVCXProj./**//**/WriteElementString("GenerateDebugInformation", ProjectIsDebugConfiguration.ToString());
                    // Import library output path.
                    if (ProjectObject.GetBuildType(Platform, Configuration) == ProjectBuildType.DynamicLibrary) {
                        ProjectVCXProj./**//**/WriteElementString("ImportLibrary", ProjectObject.GetImportLibraryOutputPath(Platform, Configuration));
                    }

                    ProjectVCXProj./**/WriteEndElement();    // </ VC++ LINKER PROPERTIES.
                    ProjectVCXProj.WriteEndElement();    // </ VC++ COMPILER and LINKER PROPERTIES.
                }

                // ------------------------------------------------------------------------------------------
                // Importing DEFAULT TARGETS for build. 
                ProjectVCXProj.WriteStartElement("Import");
                ProjectVCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets");
                ProjectVCXProj.WriteEndElement();

                ProjectVCXProj./**/WriteEndElement();
                ProjectVCXProj.WriteEndDocument();
            }

            // ==========================================================================================
            // GENERATING VCPROJ.FILTER FILES.
            string ProjectVCXProjFiltersPath = ProjectVCXProjPath + ".filters";
            using (XmlTextWriter ProjectVCXProjFilters = new XmlTextWriter(ProjectVCXProjFiltersPath, null)) {
                ProjectVCXProjFilters.Formatting = Formatting.Indented;
                ProjectVCXProjFilters.Indentation = 4;
                ProjectVCXProjFilters.WriteStartDocument();
                ProjectVCXProjFilters./**/WriteStartElement("Project", s_MSBuild2003Namespace);
                ProjectVCXProjFilters./**/WriteAttributeString("ToolsVersion", m_ToolsFiltersVersion);

                // ------------------------------------------------------------------------------------------
                // Defining TREE FILTER STRUCTURE.
                ProjectVCXProjFilters.WriteStartElement("ItemGroup");
                List<string> ProjectFolders = new List<string>();
                string ProjectBaseDirectory = Path.GetDirectoryName(ProjectObject.GetLocation());
                foreach (var ProjectSourceFile in ProjectSourceFiles) {
                    try {
                        string ProjectSourceRelativePath = Path.GetDirectoryName(ProjectSourceFile.GetFileName()).Substring(ProjectBaseDirectory.Length + 1);
                        while ((!string.IsNullOrEmpty(ProjectSourceRelativePath)) && (ProjectFolders.IndexOf(ProjectSourceRelativePath) == -1)) {
                            ProjectVCXProjFilters.WriteStartElement("Filter");
                            ProjectVCXProjFilters./**/WriteAttributeString("Include", ProjectSourceRelativePath);
                            ProjectVCXProjFilters./**/WriteElementString("UniqueIdentifier", string.Format("{{{0}}}", Guid.NewGuid().ToString()));
                            ProjectVCXProjFilters.WriteEndElement();

                            ProjectFolders.Add(ProjectSourceRelativePath);
                            ProjectSourceRelativePath = Path.GetDirectoryName(ProjectSourceRelativePath);
                        }
                    } catch (ArgumentOutOfRangeException) {
                    }
                }
                ProjectVCXProjFilters.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining SOURCES' FILTERS.
                ProjectVCXProjFilters.WriteStartElement("ItemGroup");
                foreach (var ProjectSourceFile in ProjectSourceFiles) {
                    string ProjectSourceFileFilterName;
                    try {
                        ProjectSourceFileFilterName = Path.GetDirectoryName(ProjectSourceFile.GetFileName()).Substring(ProjectBaseDirectory.Length + 1);
                    } catch (ArgumentOutOfRangeException) {
                        ProjectSourceFileFilterName = null;
                    }

                    ProjectVCXProjFilters.WriteStartElement(VisualStudioTarget.ConvertFileTypeToElement(ProjectSourceFile.GetFileType()));
                    ProjectVCXProjFilters./**/WriteAttributeString("Include", ProjectSourceFile.GetFileName());
                    if (!string.IsNullOrEmpty(ProjectSourceFileFilterName)) {
                        ProjectVCXProjFilters./**/WriteElementString("Filter", ProjectSourceFileFilterName);
                    }
                    ProjectVCXProjFilters.WriteEndElement();
                }
                ProjectVCXProjFilters.WriteEndElement();

                ProjectVCXProjFilters./**/WriteEndElement();
                ProjectVCXProjFilters.WriteEndDocument();
            }

            return ProjectVCXProjPath;
        }

        //! @}
    }   // class VisualStudio2013Target
}   // namespace GoddamnEngine.BuildSystem.VisualStudio
