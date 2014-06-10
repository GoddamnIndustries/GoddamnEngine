﻿//////////////////////////////////////////////////////////////////////////
/// VisualStudioTarget.cs - *.vcxproj Generator.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

using System;
using System.IO;
using System.Xml;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
    internal sealed class VisualStudioTarget : Target
    {
        private readonly static TargetConfiguration[] Configurations = (TargetConfiguration[])Enum.GetValues(typeof(TargetConfiguration));
        private const string MSBuild2003Namespace = @"http://schemas.microsoft.com/developer/msbuild/2003";
        private const string Copyright = @"
##########################################################################
# *.vcxproj/sln: Some Visual Studio project/solution file part.
# Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
#
# This file was automatically generated by GoddamnBuildSystem. 
# Please, do not edit it manually.
##########################################################################
";

        private static void CreateElementBySourceFileType(XmlWriter Writer, SourceFileType FileType)
        {
            switch (FileType)
            {
                case SourceFileType.SourceCode:
                    Writer.WriteStartElement("ClCompile");
                    break;
                case SourceFileType.HeaderFile:
                    Writer.WriteStartElement("ClInclude");
                    break;
                case SourceFileType.InlineImpl:
                    Writer.WriteStartElement("None");
                    break;
                case SourceFileType.ResourceScript:
                    Writer.WriteStartElement("ResourceCompile");
                    break;
                default:
                    throw new NotImplementedException();
            }
        }

        private static string GetPlatformString()
        {
            return "x64";
        }

        private static string GenerateCondition(TargetConfiguration Configuration)
        {
            string Platform = GetPlatformString();
            return string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", Configuration.ToString(), Platform);
        }

        private static void GeneratesFiltersFile(Project TheProject)
        {
            using (XmlTextWriter FiltersConfig = new XmlTextWriter(string.Format("{0}/_Build/{1}.vcxproj.filters", TheProject.Location, TheProject.Name), null))
            {
                FiltersConfig.Formatting = Formatting.Indented;
                FiltersConfig.Indentation = 4;
                FiltersConfig.WriteStartDocument();
                FiltersConfig.WriteComment(Copyright);
                FiltersConfig.WriteStartElement("Project", MSBuild2003Namespace);
                FiltersConfig.WriteAttributeString("ToolsVersion", "4.0");

                FiltersConfig.WriteStartElement("ItemGroup");
                foreach (var Folder in TheProject.Folders)
                {   // Generating new filers
                    FiltersConfig.WriteStartElement("Filter");
                    FiltersConfig.WriteAttributeString("Include", Folder.Replace('/', '\\'));
                    FiltersConfig.WriteElementString("UniqueIdentifier", string.Format("{{{0}}}", Guid.NewGuid().ToString()));
                    FiltersConfig.WriteEndElement();
                }
                FiltersConfig.WriteEndElement();

                FiltersConfig.WriteStartElement("ItemGroup");
                foreach (var SourceFile in TheProject.SourceFiles)
                {   // Generating new filers
                    string FilterName = SourceFile.RelativeFileFolder;
                    CreateElementBySourceFileType(FiltersConfig, SourceFile.FileType);
                    FiltersConfig.WriteAttributeString("Include", SourceFile.AbsoluteFilePath.Replace('/', '\\'));
                    if (!string.IsNullOrEmpty(FilterName))
                        FiltersConfig.WriteElementString("Filter", FilterName.Replace('/', '\\'));
                    FiltersConfig.WriteEndElement();
                }
                FiltersConfig.WriteEndElement();

                FiltersConfig.WriteEndElement();
                FiltersConfig.WriteEndDocument();
            }
        }

        private static void GeneratesProjectFile(Project TheProject)
        {
            string Platform = GetPlatformString();
            using (XmlTextWriter ProjectConfig = new XmlTextWriter(string.Format("{0}/_Build/{1}.vcxproj", TheProject.Location, TheProject.Name), null))
            {
                ProjectConfig.Formatting = Formatting.Indented;
                ProjectConfig.Indentation = 4;
                ProjectConfig.WriteStartDocument();
                ProjectConfig.WriteComment(Copyright);
                ProjectConfig.WriteStartElement("Project", MSBuild2003Namespace);
                ProjectConfig.WriteAttributeString("ToolsVersion", "14.0");
                ProjectConfig.WriteAttributeString("DefaultTargets", "Build");

                ProjectConfig.WriteStartElement("ItemGroup");
                ProjectConfig.WriteAttributeString("Label", "ProjectConfigurations");
                foreach (var Configuration in Configurations)
                {
                    ProjectConfig.WriteStartElement("ProjectConfiguration");
                    ProjectConfig.WriteAttributeString("Include", new StringBuilder(Configuration.ToString()).Append('|').Append(Platform).ToString());
                    ProjectConfig.WriteElementString("Configuration", Configuration.ToString());
                    ProjectConfig.WriteElementString("Platform", Platform);
                    ProjectConfig.WriteEndElement();
                }
                ProjectConfig.WriteEndElement();

                ProjectConfig.WriteStartElement("ItemGroup");
                foreach (var SourceFile in TheProject.SourceFiles)
                {
                    CreateElementBySourceFileType(ProjectConfig, SourceFile.FileType);
                    ProjectConfig.WriteAttributeString("Include", SourceFile.AbsoluteFilePath.Replace('/', '\\'));
                    ProjectConfig.WriteEndElement();
                }
                ProjectConfig.WriteEndElement();

                ProjectConfig.WriteStartElement("PropertyGroup");
                ProjectConfig.WriteAttributeString("Label", "Globals");
                ProjectConfig.WriteElementString("ProjectGuid", string.Format("{{{0}}}", Guid.NewGuid().ToString().ToUpper()));
                ProjectConfig.WriteElementString("RootNamespace", TheProject.Name);
                ProjectConfig.WriteEndElement();

                ProjectConfig.WriteStartElement("Import");
                ProjectConfig.WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                ProjectConfig.WriteEndElement();

                // Global TheProject properties:
                foreach (var Configuration in Configurations)
                {
                    ProjectConfig.WriteStartElement("PropertyGroup");
                    ProjectConfig.WriteAttributeString("Condition", GenerateCondition(Configuration));
                    ProjectConfig.WriteAttributeString("Label", "Configuration");
                    ProjectConfig.WriteElementString("ConfigurationType", TheProject.BuildType.ToString());
                    ProjectConfig.WriteElementString("PlatformToolset", "v140");
                    ProjectConfig.WriteElementString("CharacterSet", "MultiByte");
                    ProjectConfig.WriteEndElement();
                }

                ProjectConfig.WriteStartElement("Import");
                ProjectConfig.WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
                ProjectConfig.WriteEndElement();

                // Default property values.
                foreach (var Configuration in Configurations)
                {
                    ProjectConfig.WriteStartElement("ImportGroup");
                    ProjectConfig.WriteAttributeString("Condition", GenerateCondition(Configuration));
                    ProjectConfig.WriteAttributeString("Label", "PropertySheets");
                    ProjectConfig.WriteStartElement("Import");
                    ProjectConfig.WriteAttributeString("Project", @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                    ProjectConfig.WriteAttributeString("Condition", @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                    ProjectConfig.WriteAttributeString("Label",  "LocalAppDataPlatform");
                    ProjectConfig.WriteEndElement();
                    ProjectConfig.WriteEndElement();
                }

                // Configurations
                // Strange "UserMacros" node.
                ProjectConfig.WriteStartElement("PropertyGroup");
                ProjectConfig.WriteAttributeString("Label", "UserMacros");
                ProjectConfig.WriteEndElement();

                // Visual studio additional paths.
                foreach (var Configuration in Configurations)
                {
                    ProjectConfig.WriteStartElement("PropertyGroup");
                    ProjectConfig.WriteAttributeString("Condition", GenerateCondition(Configuration));
                    ProjectConfig.WriteElementString("TargetName", TheProject.Name + ((Configuration == TargetConfiguration.Debug) ? ".Debug" : ""));
                    ProjectConfig.WriteElementString("OutDir", TheProject.BuildType == ProjectBuildType.StaticLibrary ? @"$(GODDAMN_SDK)lib\" : @"$(GODDAMN_SDK)bin\");

                    StringBuilder IncludePathes = new StringBuilder();
                    foreach (var Dependency in TheProject.Dependencies)
                        IncludePathes.Append(Dependency.HeaderLocations).Append(Path.PathSeparator);
                    IncludePathes.Append(Path.Combine(BuildSystem.SDKPath, "source")).Append(Path.PathSeparator);
                    IncludePathes.Append(@"$(IncludePath)");
                    ProjectConfig.WriteElementString("IncludePath", IncludePathes.ToString());
                    ProjectConfig.WriteEndElement();
                }

                // C++ Compiler/Linker properties.
                foreach (var Configuration in Configurations)
                {
                    bool IsDebugConfiguration = (Configuration == TargetConfiguration.Debug);
                    ProjectConfig.WriteStartElement("ItemDefinitionGroup");
                    ProjectConfig.WriteAttributeString("Condition", GenerateCondition(Configuration));
                    ProjectConfig.WriteStartElement("ClCompile");
                    StringBuilder PreprocessorDefinitions = new StringBuilder();
                    foreach (var PreprocessorDefinition in TheProject.PreprocessorDefinitions)
                        PreprocessorDefinitions.Append(PreprocessorDefinition).Append(Path.PathSeparator);
                    PreprocessorDefinitions.Append(IsDebugConfiguration ? "_DEBUG" : "NDEBUG").Append(Path.PathSeparator);

                    if (TheProject.BuildType == ProjectBuildType.DynamicLibrary)
                        PreprocessorDefinitions.Append("_WINDLL").Append(Path.PathSeparator);

                    PreprocessorDefinitions.Append(@"%(PreprocessorDefinition)");
                    ProjectConfig.WriteElementString("PreprocessorDefinitions", PreprocessorDefinitions.ToString());

                    ProjectConfig.WriteElementString("Optimization", IsDebugConfiguration ? "Disabled" : "Full");
                    ProjectConfig.WriteElementString("WarningLevel", "Level4");
                    ProjectConfig.WriteElementString("TreatWarningAsError", "true");
                    ProjectConfig.WriteElementString("PrecompiledHeader", "NotUsing");
                    ProjectConfig.WriteElementString("CallingConvention", "FastCall");
                //  ProjectConfig.WriteElementString("ExceptionHandling", "false");
                    ProjectConfig.WriteElementString("RuntimeTypeInfo", "false");
                    ProjectConfig.WriteElementString("FavorSizeOrSpeed", "Speed");
                    ProjectConfig.WriteElementString("DisableSpecificWarnings", "4100;4127;4189;4201;4458;4456;4505;4714"); 
                    ProjectConfig.WriteEndElement();

                    ProjectConfig.WriteStartElement("Link");
                    StringBuilder AdditionalDependencies = new StringBuilder();
                    foreach (var AdditionalDependency in TheProject.Dependencies)
                        AdditionalDependencies.Append(AdditionalDependency.ResolveDependency(Configuration)).Append(Path.PathSeparator);
                    AdditionalDependencies.Append(@"%(AdditionalDependencies)");
                    ProjectConfig.WriteElementString("AdditionalDependencies", AdditionalDependencies.ToString());
                    ProjectConfig.WriteElementString("GenerateDebugInformation", IsDebugConfiguration.ToString());

                    if (TheProject.IsPlugin)
                        ProjectConfig.WriteElementString("AdditionalOptions", @"/EXPORT:EnginePluginEntry %(AdditionalOptions)");
                    if (TheProject.BuildType == ProjectBuildType.DynamicLibrary)
                        ProjectConfig.WriteElementString("ImportLibrary", @"$(GODDAMN_SDK)lib\$(TargetName).lib");
                    ProjectConfig.WriteEndElement();
                    ProjectConfig.WriteEndElement();
                }

                ProjectConfig.WriteStartElement("Import");
                ProjectConfig.WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets");
                ProjectConfig.WriteEndElement();

                ProjectConfig.WriteEndElement();
                ProjectConfig.WriteEndDocument();
            }
        }

        public override void GenerateProject(Project TheProject)
        {
            GeneratesFiltersFile(TheProject);
            GeneratesProjectFile(TheProject);
        }
    }   // class VisualStudioTarget
}   // namespace GoddamnEngine.BuildSystem
