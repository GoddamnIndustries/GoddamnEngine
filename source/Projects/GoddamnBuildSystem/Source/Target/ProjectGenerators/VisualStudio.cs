//! ==========================================================================================
//! VisualStudio.cs - Visual Studio IDE project files generator.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace GoddamnEngine.BuildSystem.VisualStudio
{
    internal enum VisualStudioVersion
    {
        VS2013,
        VS2015
    }   // enum VisualStudioVersion

    //! Generator of the project/solution files for Visual Studio.
    internal class VisualStudioProjectGenerator : TargetProjectGenerator
    {
        private const string s_MSBuild2003Namespace = @"http://schemas.microsoft.com/developer/msbuild/2003";
        private const string s_SLNVCProjProjectGUID = @"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
        private const string s_SLNCSProjProjectGUID = @"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
        private const string s_SLNFilterProjectGUID = @"{2150E333-8FDC-42A3-9474-1A3956D46DE8}";

        private readonly string m_ToolsVersion = "14.0";
        private readonly string m_ToolsFiltersVersion = "4.0";

        //! Returns formated GUID.
        internal static string CreateMSBuildGUID()
        {
            return string.Format("{{{0}}}", Guid.NewGuid().ToString()).ToUpperInvariant();
        }

        //! Converts specified source file type into name of the node in MSBuild.
        private static string ConvertFileTypeToVCXProjElement(ProjectSourceFileType FileType)
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

        //! Converts target platform into MSBuild compatible one.
        //! @param Platform Some platform.
        private static string ConvertPlatformToMSBuildPlatform(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.Emscripten:
                    return "Win32";

                case TargetPlatform.Windows:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                    return "x64";

                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsPhone:
                    return "ARM";

                default:
                    throw new NotImplementedException();
            }
        }

        //! Checks if specified platform is natively supported by VS.
        //! @param Platform Some platform.
        public override bool IsPlatformNativelySupported(TargetPlatform Platform)
        {
            switch (Platform) {
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsPhone:
                case TargetPlatform.XBoxOne:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! Generates project files for Visual Studio: .vcxproj and .vcxproj.filter. 
        //! @param Project Parsed project object.
        //! @param Configurations Array of target configurations.
        //! @returns Path to Visual Studio's .vcxproj file.
        public sealed override string GenerateProjectFiles(ProjectCache Project)
        {
            string ProjectDirectoryPath = Path.Combine(base.GenerateProjectFiles(Project), Project.m_CachedName);
            bool PlatformRequiresCLI = false;
            bool PlatformDoesNotRequiresRTTI = PlatformRequiresCLI;
            bool PlatformDoesNotRequiresExceptions = false;

            // ==========================================================================================
            // Generating VCXPROJ files.
            // ==========================================================================================
            string VCXProjPath = ProjectDirectoryPath + ".vcxproj";
            using (XmlTextWriter VCXProj = new XmlTextWriter(VCXProjPath, null)) {
                VCXProj.Formatting = Formatting.Indented;
                VCXProj.IndentChar = '\t';
                VCXProj.Indentation = 1;

                VCXProj.WriteStartDocument();
                VCXProj./**/WriteStartElement("Project", s_MSBuild2003Namespace);
                VCXProj./**//**/WriteAttributeString("ToolsVersion", m_ToolsVersion);
                VCXProj./**//**/WriteAttributeString("DefaultTargets", "Build");

                // ------------------------------------------------------------------------------------------
                // Defining list of configurations.
                // ------------------------------------------------------------------------------------------
                VCXProj.WriteStartElement("ItemGroup");
                VCXProj./**/WriteAttributeString("Label", "ProjectConfigurations");
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        VCXProj./**/WriteStartElement("ProjectConfiguration");
                        VCXProj./**//**/WriteAttributeString("Include", string.Concat(ConfigurationName, '|', PlatformString));
                        VCXProj./**//**/WriteElementString("Configuration", ConfigurationName);
                        VCXProj./**//**/WriteElementString("Platform", PlatformString);
                        VCXProj./**/WriteEndElement();
                    }
                }
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining list of source files.
                // ------------------------------------------------------------------------------------------
                VCXProj.WriteStartElement("ItemGroup");
                foreach (var ProjectSource in Project.m_CachedSourceFiles) {
                    VCXProj./**/WriteStartElement(ConvertFileTypeToVCXProjElement(ProjectSource.m_FileType));
                    VCXProj./**//**/WriteAttributeString("Include", ProjectSource.m_FileName);
                    foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                        string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                        foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                            if (Target.MatchesPlatformConfiguration(ProjectSource.m_FileName, Platform, Configuration)
                                && (!ProjectSource.m_IsExcludedDelegate(Platform))) {
                                continue;
                            }

                            string ConfigurationName = string.Concat(Configuration, Platform);
                            VCXProj./**//**/WriteStartElement("ExcludedFromBuild");
                            VCXProj./**//**//**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString));
                            VCXProj./**//**//**/WriteString("true");
                            VCXProj./**//**/WriteEndElement();
                        }
                    }
                    VCXProj./**/WriteEndElement();
                }
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding global project properties.
                // ------------------------------------------------------------------------------------------
                VCXProj.WriteStartElement("PropertyGroup");
                VCXProj./**/WriteAttributeString("Label", "Globals");
                VCXProj./**/WriteElementString("ProjectGuid", CreateMSBuildGUID());
                VCXProj./**/WriteElementString("RootNamespace", Project.m_CachedName);
                VCXProj.WriteEndElement();

                VCXProj.WriteStartElement("Import");
                VCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding main configuration properties.
                // ------------------------------------------------------------------------------------------
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    string PlatformToolset = null;
                    switch (Platform) {
                        case TargetPlatform.Windows:
                        case TargetPlatform.WindowsRT:
                        case TargetPlatform.WindowsStore:
                            PlatformToolset = "CTP_Nov2013";
                            break;
                        case TargetPlatform.WindowsPhone:
                            PlatformToolset = "v120_wp81";
                            break;
                            // These are not supported, setting up some generic toolset.
                        case TargetPlatform.Android:
                        case TargetPlatform.Emscripten:
                            PlatformToolset = "v120";
                            break;
                        default:
                            throw new NotImplementedException();
                    }

                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        VCXProj.WriteStartElement("PropertyGroup");
                        VCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString));
                        VCXProj./**/WriteAttributeString("Label", "Configuration");
                        VCXProj./**/WriteElementString("PlatformToolset", PlatformToolset);
                        if (IsPlatformNativelySupported(Platform)) {
                            VCXProj./**/WriteElementString("ConfigurationType", Project.m_CachedBuildType[Platform].ToString());
                            VCXProj./**/WriteElementString("CharacterSet", "MultiByte");
                            if (PlatformRequiresCLI) {
                                VCXProj./**/WriteElementString("CLRSupport", "true");
                            }
                        } else {
                            VCXProj./**/WriteElementString("ConfigurationType", "Makefile");
                        }
                        VCXProj.WriteEndElement();
                    }
                }

                VCXProj.WriteStartElement("Import");
                VCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
                VCXProj.WriteEndElement();
                VCXProj.WriteStartElement("ImportGroup");
                VCXProj./**/WriteAttributeString("Label", "PropertySheets");
                VCXProj./**/WriteStartElement("Import");
                VCXProj./**//**/WriteAttributeString("Project", @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                VCXProj./**//**/WriteAttributeString("Condition", @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                VCXProj./**//**/WriteAttributeString("Label", @"LocalAppDataPlatform");
                VCXProj./**/WriteEndElement();
                VCXProj.WriteEndElement();
                VCXProj.WriteStartElement("PropertyGroup");
                VCXProj./**/WriteAttributeString("Label", "UserMacros");
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding VC++ directories (and sometimes NMake commands).
                // ------------------------------------------------------------------------------------------
                StringBuilder ProjectIncludePathesBuilder = new StringBuilder()
                    .Append(Path.Combine(Project.m_CachedLocation, "Source")).Append(Path.PathSeparator);
                Array.ForEach(Project.m_CachedDependencies, ProjectDependency =>
                    Array.ForEach(ProjectDependency.m_CachedHeaderDirectories, ProjectDependencyIncludePath =>
                        ProjectIncludePathesBuilder.Append(ProjectDependencyIncludePath).Append(Path.PathSeparator)
                    )
                );
                string ProjectIncludePathes = ProjectIncludePathesBuilder.ToString();

                VCXProj.WriteStartElement("PropertyGroup");
                VCXProj./**/WriteElementString("NMakeForcedIncludes", "$(NMakeForcedIncludes)");
                VCXProj./**/WriteElementString("NMakeAssemblySearchPath", "$(NMakeAssemblySearchPath)");
                VCXProj./**/WriteElementString("NMakeForcedUsingAssemblies", "$(NMakeForcedUsingAssemblies)");
                VCXProj.WriteEndElement();
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        string ProjectOutputPath = Project.m_CachedOutputPathDelegate[Platform](Configuration);
                        VCXProj.WriteStartElement("PropertyGroup");
                        VCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString));
                        VCXProj./**/WriteElementString("OutDir", Path.GetDirectoryName(ProjectOutputPath) + Path.DirectorySeparatorChar);
                        if (IsPlatformNativelySupported(Platform)) {
                            VCXProj./**/WriteElementString("TargetName", Path.GetFileNameWithoutExtension(ProjectOutputPath));
                            VCXProj./**/WriteElementString("IncludePath", ProjectIncludePathes + "$(IncludePath)");
                        } else {
                            VCXProj./**/WriteElementString("NMakeOutput", ProjectOutputPath);
                        }
                        VCXProj.WriteEndElement();    // </ VC++ DIRECTORIES
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Overriding VC++ compiler and linker properties.
                // ------------------------------------------------------------------------------------------
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);

                    StringBuilder MacrosBuilder = new StringBuilder();
                    Array.ForEach(Project.m_CachedMacros[Platform], ProjectPreprocessorDefinition =>
                        MacrosBuilder.Append(ProjectPreprocessorDefinition.ToString()).Append(Path.PathSeparator)
                    );

                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                        bool IsDebugConfiguration = (Configuration == TargetConfiguration.Debug);
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        string Macros = MacrosBuilder.ToString();
                        if (IsDebugConfiguration) {
                            Macros += "_DEBUG";
                        } else {
                            Macros += "NDEBUG";
                        }

                        if (IsPlatformNativelySupported(Platform)) {
                            // ------------------------------------------------------------------------------------------
                            // Defining VC++ compiler properties.
                            // ------------------------------------------------------------------------------------------
                            VCXProj.WriteStartElement("ItemDefinitionGroup");
                            VCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString));
                            VCXProj./**/WriteStartElement("ClCompile");
                            VCXProj./**//**/WriteElementString("PreprocessorDefinitions", Macros + "%(PreprocessorDefinition)");
                            VCXProj./**//**/WriteElementString("Optimization", IsDebugConfiguration ? "Disabled" : "Full");
                            VCXProj./**//**/WriteElementString("WarningLevel", "Level4");
                            VCXProj./**//**/WriteElementString("TreatWarningAsError", "true");
                            VCXProj./**//**/WriteElementString("PrecompiledHeader", "NotUsing");
                            VCXProj./**//**/WriteElementString("CallingConvention", "FastCall");
                            VCXProj./**//**/WriteElementString("FavorSizeOrSpeed", "Speed");
                            if (PlatformDoesNotRequiresRTTI) {
                                VCXProj./**//**/WriteElementString("RuntimeTypeInfo", "false");
                            }
                            if (PlatformDoesNotRequiresExceptions) {
                                VCXProj./**//**/WriteElementString("ExceptionHandling", "false");
                            }
                            VCXProj./**/WriteEndElement();    // </ VC++ COMPILER PROPERTIES

                            // ------------------------------------------------------------------------------------------
                            // Defining VC++ linker properties.
                            // ------------------------------------------------------------------------------------------
                            StringBuilder ProjectLinkedLibraries = new StringBuilder()
                                .Append("%(AdditionalDependencies)").Append(Path.PathSeparator)
                                .Append("winmm.lib").Append(Path.PathSeparator)
                                .Append("imm32.lib").Append(Path.PathSeparator)
                                .Append("version.lib").Append(Path.PathSeparator);
                            Array.ForEach(Project.m_CachedDependencies, ProjectDependency =>
                                Array.ForEach(ProjectDependency.m_CachedLinkedLibraries[Platform], ProjectDependencyLibraryPath =>
                                    ProjectLinkedLibraries.Append(ProjectDependencyLibraryPath).Append(Path.PathSeparator)
                                )
                            );

                            VCXProj./**/WriteStartElement("Link");
                            VCXProj./**//**/WriteElementString("AdditionalDependencies", ProjectLinkedLibraries.ToString());
                            VCXProj./**//**/WriteElementString("GenerateDebugInformation", IsDebugConfiguration.ToString());
                            if (Project.m_CachedBuildType[Platform] == ProjectBuildType.DynamicLibrary) {
                                VCXProj./**//**/WriteElementString("ImportLibrary", Project.m_CachedImportLibraryOutputPathDelegate[Platform](Configuration));
                            }
                            VCXProj./**/WriteEndElement();
                            VCXProj.WriteEndElement();
                        } else {
                            string NMakeCommand = string.Format("nmake -f {0}", Project.m_MakefilePath[Platform]);

                            VCXProj.WriteStartElement("PropertyGroup");
                            VCXProj./**/WriteAttributeString("Condition", string.Format(@"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString));
                            VCXProj./**/WriteElementString("NMakePreprocessorDefinitions", Macros + "$(NMakePreprocessorDefinitions)".ToString());
                            VCXProj./**/WriteElementString("NMakeIncludeSearchPath", ProjectIncludePathesBuilder + "$(NMakeIncludeSearchPath)");
                            VCXProj./**/WriteElementString("NMakeBuildCommandLine", NMakeCommand);
                            VCXProj./**/WriteElementString("NMakeReBuildCommandLine", NMakeCommand + " --rebuild");
                            VCXProj./**/WriteElementString("NMakeCleanCommandLine", NMakeCommand + " --clean");
                            VCXProj.WriteEndElement();
                        }
                    }
                }

                VCXProj.WriteStartElement("Import");
                VCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets");
                VCXProj.WriteEndElement();

                VCXProj./**/WriteEndElement();
                VCXProj.WriteEndDocument();
            }

            // ==========================================================================================
            // Generating VCPROJ.FILTER files.
            // ==========================================================================================
            string VCXProjFiltersPath = VCXProjPath + ".filters";
            using (XmlTextWriter VCXProjFilters = new XmlTextWriter(VCXProjFiltersPath, null)) {
                VCXProjFilters.Formatting = Formatting.Indented;
                VCXProjFilters.IndentChar = '\t';
                VCXProjFilters.Indentation = 1;

                VCXProjFilters.WriteStartDocument();
                VCXProjFilters./**/WriteStartElement("Project", s_MSBuild2003Namespace);
                VCXProjFilters./**/WriteAttributeString("ToolsVersion", m_ToolsFiltersVersion);

                HashSet<string> ProjectFoldersCache = new HashSet<string>();
                foreach (var ProjectSourceFile in Project.m_CachedSourceFiles) {
                    string ProjectSourceFileDirectory = Path.GetDirectoryName(ProjectSourceFile.m_FileName);

                    // Checking if this source can have a filter: it is located in project directory and it not in it's root.
                    string ProjectSourceFilter;
                    if (ProjectSourceFileDirectory.StartsWith(Project.m_CachedSourcesFilterOrigin, StringComparison.InvariantCultureIgnoreCase)
                        && (ProjectSourceFileDirectory.Length > (Project.m_CachedSourcesFilterOrigin.Length + 1))) {
                        ProjectSourceFilter = "Source\\" + ProjectSourceFileDirectory.Substring(Project.m_CachedSourcesFilterOrigin.Length + 1);
                        string ProjectSourceFilterIter = ProjectSourceFilter;
                        while ((!ProjectFoldersCache.Contains(ProjectSourceFilterIter)) && (!string.IsNullOrEmpty(ProjectSourceFilterIter))) {
                            // ------------------------------------------------------------------------------------------
                            // Defining group of filters.
                            // ------------------------------------------------------------------------------------------
                            VCXProjFilters.WriteStartElement("ItemGroup");
                            VCXProjFilters./**/WriteStartElement("Filter");
                            VCXProjFilters./**//**/WriteAttributeString("Include", ProjectSourceFilterIter);
                            VCXProjFilters./**//**/WriteElementString("UniqueIdentifier", CreateMSBuildGUID());
                            VCXProjFilters./**/WriteEndElement();
                            VCXProjFilters.WriteEndElement();

                            ProjectFoldersCache.Add(ProjectSourceFilterIter);
                            ProjectSourceFilterIter = Path.GetDirectoryName(ProjectSourceFilterIter);
                        }
                    } else {
                        ProjectSourceFilter = "Source";
                    }

                    // ------------------------------------------------------------------------------------------
                    // Defining source's filters.
                    // ------------------------------------------------------------------------------------------
                    VCXProjFilters.WriteStartElement("ItemGroup");
                    VCXProjFilters./**/WriteStartElement(ConvertFileTypeToVCXProjElement(ProjectSourceFile.m_FileType));
                    VCXProjFilters./**//**/WriteAttributeString("Include", ProjectSourceFile.m_FileName);
                    VCXProjFilters./**//**/WriteElementString("Filter", ProjectSourceFilter);
                    VCXProjFilters./**/WriteEndElement();
                    VCXProjFilters.WriteEndElement();
                }

                VCXProjFilters./**/WriteEndElement();
                VCXProjFilters.WriteEndDocument();
            }

            return VCXProjPath;
        }

        //! Generates solution files for Visual Studio.
        //! This is not a full-featured generator. It generates some minimum of data, and VS 
        //! itself generates the rest. In some time, this may break the whole system.
        //! @param Solution Parsed solution object.
        //! @returns Path to Visual Studio's .sln file.
        public sealed override string GenerateSolutionFiles(SolutionCache Solution)
        {
            // ==========================================================================================
            // Generating SLN files..
            // ==========================================================================================
            string SLNPath = Path.Combine(base.GenerateSolutionFiles(Solution), Solution.m_CachedName + ".sln");
            using (StreamWriter SLN = new StreamWriter(SLNPath)) {
                SLN.WriteLine();
                SLN.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
                SLN.WriteLine("# Generated by the GoddamnBuildSystem.");

                // ------------------------------------------------------------------------------------------
                // Caching GUIDs of projects and filters and defining filters.
                // ------------------------------------------------------------------------------------------
                var SolutionFiltersGUIDCache = new Dictionary<int, string>();
                foreach (var SolutionProject in Solution.m_CachedProjects) {
                    SolutionProject.m_AdditionalCache.m_GUID = CreateMSBuildGUID();
                    if (!string.IsNullOrEmpty(SolutionProject.m_CachedFilter)) {
                        int SolutionProjectFilterHash = SolutionProject.m_CachedFilter.GetHashCode();
                        if (!SolutionFiltersGUIDCache.ContainsKey(SolutionProjectFilterHash)) {
                            SolutionProject.m_AdditionalCache.m_FilterGUID = CreateMSBuildGUID();
                            SolutionFiltersGUIDCache.Add(SolutionProjectFilterHash, SolutionProject.m_AdditionalCache.m_FilterGUID);

                            SLN.WriteLine("Project(\"{0}\") = \"{1}\", \"{1}\", \"{2}\"", s_SLNFilterProjectGUID, SolutionProject.m_CachedFilter, SolutionProject.m_AdditionalCache.m_FilterGUID);
                            SLN.WriteLine("EndProject");
                        } else {
                            SolutionProject.m_AdditionalCache.m_FilterGUID = SolutionFiltersGUIDCache[SolutionProject.m_AdditionalCache.m_FilterGUID];
                        }
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Defining Solution projects.
                // ------------------------------------------------------------------------------------------
                foreach (var SolutionProject in Solution.m_CachedProjects) {
                    SLN.WriteLine("Project(\"{0}\") = \"{1}\", \"{2}\", \"{3}\"",
                        SolutionProject.m_IsBuildTool ? s_SLNCSProjProjectGUID : s_SLNVCProjProjectGUID,
                        SolutionProject.m_CachedName, SolutionProject.m_GeneratorOutputPath,
                        SolutionProject.m_AdditionalCache.m_GUID);
                    SLN.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                    foreach (var SolutionDependentProject in Solution.m_CachedProjects) {
                        if (SolutionDependentProject.m_CachedPriority <= SolutionProject.m_CachedPriority) {
                            continue;
                        }

                        SLN.WriteLine("\t\t{0} = {0}", SolutionDependentProject.m_AdditionalCache.m_GUID);
                    }
                    SLN.WriteLine("\tEndProjectSection");
                    SLN.WriteLine("EndProject");
                }

                // ------------------------------------------------------------------------------------------
                // Defining platforms/configurations.
                // ------------------------------------------------------------------------------------------
                SLN.WriteLine("Global");

                SLN.WriteLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                        SLN.WriteLine("\t\t{0}|{1} = {0}|{1}", Configuration, Platform);
                    }
                }
                SLN.WriteLine("\tEndGlobalSection");

                SLN.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
                foreach (var SolutionProject in Solution.m_CachedProjects) {
                    foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                        foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                            string PlatformString = SolutionProject.m_IsBuildTool ? "Any CPU" : ConvertPlatformToMSBuildPlatform(Platform);
                            string PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {1}{2}|{3}", SolutionProject.m_AdditionalCache.m_GUID, Configuration, Platform, PlatformString);
                            
                            SLN.WriteLine(PlatfromConfig.Replace("*", "ActiveCfg"));
                            SLN.WriteLine(PlatfromConfig.Replace("*", "Build.0"));
                        }
                    }
                }
                SLN.WriteLine("\tEndGlobalSection");

                SLN.WriteLine("\tGlobalSection(SolutionProperties) = postSolution");
                SLN.WriteLine("\t\tHideSolutionNode = FALSE");
                SLN.WriteLine("\tEndGlobalSection");

                // ------------------------------------------------------------------------------------------
                // Matching projects and filters hierarchy.
                // ------------------------------------------------------------------------------------------
                SLN.WriteLine("\tGlobalSection(NestedProjects) = preSolution");
                foreach (var SolutionProject in Solution.m_CachedProjects) {
                    if (!string.IsNullOrEmpty(SolutionProject.m_CachedFilter)) {
                        SLN.WriteLine("\t\t{0} = {1}", SolutionProject.m_AdditionalCache.m_GUID, SolutionProject.m_AdditionalCache.m_FilterGUID);
                    }
                }
                SLN.WriteLine("\tEndGlobalSection");
                SLN.WriteLine("EndGlobal");
            }

            return SLNPath;
        }
    }   // class VisualStudioProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.VisualStudio
