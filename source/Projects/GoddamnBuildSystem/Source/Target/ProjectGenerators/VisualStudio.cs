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
        private static string ConvertPlatformToMSBuildPlatform(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.iOS:
                case TargetPlatforms.Android:
                case TargetPlatforms.Emscripten:
                    return "Win32";

                case TargetPlatforms.Windows:
                case TargetPlatforms.WindowsStore:
                case TargetPlatforms.OSX:
                case TargetPlatforms.Linux:
                case TargetPlatforms.XBoxOne:
                case TargetPlatforms.PlayStation4:
                    return "x64";

                case TargetPlatforms.WindowsRT:
                case TargetPlatforms.WindowsPhone:
                    return "ARM";

                default:
                    throw new NotImplementedException();
            }
        }

        //! Checks if specified platform is natively supported by VS.
        //! @param Platform Some platform.
        public override bool IsPlatformNativelySupported(TargetPlatforms Platform)
        {
            switch (Platform) {
                case TargetPlatforms.Windows:
                case TargetPlatforms.WindowsStore:
                case TargetPlatforms.WindowsRT:
                case TargetPlatforms.WindowsPhone:
                case TargetPlatforms.XBoxOne:
                case TargetPlatforms.PlayStation4:
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
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        VCXProj./**/WriteStartElement("ProjectConfiguration");
                        VCXProj./**//**/WriteAttributeString("Include", ConfigurationName + '|' + PlatformString);
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
                    foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                        string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                        foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                            if (Target.MatchesPlatformConfiguration(ProjectSource.m_FileName, Platform, Configuration)
                                && (!ProjectSource.m_IsExcludedDelegate(Platform))) {
                                continue;
                            }

                            string ConfigurationName = string.Concat(Configuration, Platform);
                            VCXProj./**//**/WriteStartElement("ExcludedFromBuild");
                            VCXProj./**//**//**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
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
                Project.m_AdditionalCache.m_GUID = CreateMSBuildGUID();
                VCXProj.WriteStartElement("PropertyGroup");
                VCXProj./**/WriteAttributeString("Label", "Globals");
                VCXProj./**/WriteElementString("ProjectGuid", Project.m_AdditionalCache.m_GUID);
                VCXProj./**/WriteElementString("RootNamespace", Project.m_CachedName);
                VCXProj.WriteEndElement();

                VCXProj.WriteStartElement("Import");
                VCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding main configuration properties.
                // ------------------------------------------------------------------------------------------
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    string PlatformToolset = null;
                    switch (Platform) {
                        case TargetPlatforms.Windows:
                        case TargetPlatforms.WindowsRT:
                        case TargetPlatforms.WindowsStore:
                            PlatformToolset = "CTP_Nov2013";
                            break;
                        case TargetPlatforms.WindowsPhone:
                            PlatformToolset = "v120_wp81";
                            break;
                        // These are not supported, setting up some generic toolset.
                        default:
                            PlatformToolset = "v120";
                            break;
                    }

                    foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        VCXProj.WriteStartElement("PropertyGroup");
                        VCXProj./**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                        VCXProj./**/WriteAttributeString("Label", "Configuration");
                        VCXProj./**/WriteElementString("PlatformToolset", PlatformToolset);
                        if (IsPlatformNativelySupported(Platform)) {
                            VCXProj./**/WriteElementString("ConfigurationType", Project.m_CachedBuildTypes[Platform].ToString());
                            VCXProj./**/WriteElementString("CharacterSet", "MultiByte");
                            if (TargetPlatformInformation.Get(Platform).m_RequiresRTTI) {
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

                // Building list of header directories.
                StringBuilder ProjectIncludePathesBuilder = new StringBuilder();
                ProjectIncludePathesBuilder.Append(Path.Combine(Project.m_CachedLocation, "Source")).Append(Path.PathSeparator);
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
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        string ProjectOutputPath = Project.m_CachedOutputPathDelegates[Platform](Configuration);
                        VCXProj.WriteStartElement("PropertyGroup");
                        VCXProj./**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
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
                // Overriding VC++ compiler and linker properties OR NMake commands.
                // ------------------------------------------------------------------------------------------
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    TargetPlatformInformation PlatformInformation = TargetPlatformInformation.Get(Platform);
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);

                    // Defining macros.
                    StringBuilder MacrosBuilder = new StringBuilder();
                    Array.ForEach(Project.m_CachedMacros[Platform], Macro => MacrosBuilder.Append(Macro.ToString()).Append(Path.PathSeparator));

                    // Overriding default paths.
                    string StandartLibrariesPaths;
                    if (IsPlatformNativelySupported(Platform) && (PlatformInformation.m_StandartHeadersPathes != null)) {
                        StringBuilder StandartLibrariesPathsBuilder = new StringBuilder();
                        Array.ForEach(PlatformInformation.m_StandartHeadersPathes, StandartHeadersPath => StandartLibrariesPathsBuilder.Append(StandartHeadersPath).Append(Path.PathSeparator));
                        StandartLibrariesPaths = StandartLibrariesPathsBuilder.ToString();
                    } else {
                        StandartLibrariesPaths = "$(NMakeIncludeSearchPath)";
                    }

                    foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                        TargetConfigurationInformation ConfigurationInformation = TargetConfigurationInformation.Get(Configuration);

                        string ConfigurationName = string.Concat(Configuration, Platform);
                        string Macros = MacrosBuilder.ToString();
                        if (ConfigurationInformation.m_IsDebug) {
                            Macros += "_DEBUG";
                        } else {
                            Macros += "NDEBUG";
                        }

                        if (IsPlatformNativelySupported(Platform)) {
                            // ------------------------------------------------------------------------------------------
                            // Defining VC++ compiler properties.
                            // ------------------------------------------------------------------------------------------
                            VCXProj.WriteStartElement("ItemDefinitionGroup");
                            VCXProj./**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VCXProj./**/WriteStartElement("ClCompile");
                            VCXProj./**//**/WriteElementString("FavorSizeOrSpeed", "Speed");
                            VCXProj./**//**/WriteElementString("PrecompiledHeader", "NotUsing");
                            VCXProj./**//**/WriteElementString("CallingConvention", "FastCall");
                            VCXProj./**//**/WriteElementString("WarningLevel", "Level" + TargetCppCompilerInformation.s_WarningLevel);
                            VCXProj./**//**/WriteElementString("TreatWarningAsError", TargetCppCompilerInformation.s_WarningsAreErrors.ToString());
                            VCXProj./**//**/WriteElementString("PreprocessorDefinitions", Macros + "%(PreprocessorDefinition)");
                            VCXProj./**//**/WriteElementString("Optimization", ConfigurationInformation.m_Optimize ? "Full" : "Disabled");
                            VCXProj./**//**/WriteElementString("RuntimeTypeInfo", PlatformInformation.m_RequiresRTTI.ToString());
                            VCXProj./**//**/WriteElementString("ExceptionHandling", PlatformInformation.m_RequiresExceptions.ToString());
                            VCXProj./**/WriteEndElement();    // </ VC++ COMPILER PROPERTIES

                            // ------------------------------------------------------------------------------------------
                            // Defining VC++ linker properties.
                            // ------------------------------------------------------------------------------------------

                            // Collecting linked libraries..
                            StringBuilder ProjectLinkedLibraries = new StringBuilder();
                            Array.ForEach(PlatformInformation.m_DefaultLibraries, Library => ProjectLinkedLibraries.Append(Library).Append(Path.PathSeparator));
                            Array.ForEach(Project.m_CachedDependencies, ProjectDependency =>
                                Array.ForEach(ProjectDependency.m_CachedLinkedLibraries[Platform], ProjectDependencyLibraryPath =>
                                    ProjectLinkedLibraries.Append(ProjectDependencyLibraryPath).Append(Path.PathSeparator)
                                )
                            );

                            VCXProj./**/WriteStartElement("Link");
                            VCXProj./**//**/WriteElementString("AdditionalDependencies", ProjectLinkedLibraries.Append("%(AdditionalDependencies)").ToString());
                            VCXProj./**//**/WriteElementString("GenerateDebugInformation", ConfigurationInformation.m_GenerateDebugInformation.ToString());
                            if (Project.m_CachedBuildTypes[Platform] == ProjectBuildType.DynamicLibrary) {
                                VCXProj./**//**/WriteElementString("ImportLibrary", Project.m_CachedImportLibraryOutputPathDelegates[Platform](Configuration));
                            }
                            VCXProj./**/WriteEndElement();
                            VCXProj.WriteEndElement();
                        } else {
                            string NMakeCommand = string.Format("nmake -f {0}", Project.m_MakefilePathes[Platform]);

                            // ------------------------------------------------------------------------------------------
                            // Defining NMake properties.
                            // ------------------------------------------------------------------------------------------
                            VCXProj.WriteStartElement("PropertyGroup");
                            VCXProj./**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VCXProj./**/WriteElementString("NMakePreprocessorDefinitions", Macros + "$(NMakePreprocessorDefinitions)");
                            VCXProj./**/WriteElementString("NMakeIncludeSearchPath", ProjectIncludePathes + StandartLibrariesPaths);
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
                    string ProjectSourceFilter;

                    // Checking if this source can have a filter: it is located in project directory and it not in it's root.
                    // We also add "Source" to each filter to separate config and source files.  
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
                    // Looking for cached project's GUID.
                    if (string.IsNullOrEmpty(SolutionProject.m_AdditionalCache.m_GUID)) {
                        SolutionProject.m_AdditionalCache.m_GUID = CreateMSBuildGUID();
                    }
                    // Looking for projects with filters which we have not already cached.
                    if (!string.IsNullOrEmpty(SolutionProject.m_CachedFilter)) {
                        int SolutionProjectFilterHash = SolutionProject.m_CachedFilter.GetHashCode();
                        if (!SolutionFiltersGUIDCache.ContainsKey(SolutionProjectFilterHash)) {
                            // Generating and caching filter GUIDS.
                            SolutionProject.m_AdditionalCache.m_FilterGUID = CreateMSBuildGUID();
                            SolutionFiltersGUIDCache.Add(SolutionProjectFilterHash, SolutionProject.m_AdditionalCache.m_FilterGUID);

                            // E.g. 'Project({Filter-GUID}) = "Name", "Name", "Filter-GUID"'
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
                    // E.g. 'Project({Type-GUID}) = "Name", "Path-ToProject-File", "Project-GUID"'
                    SLN.WriteLine("Project(\"{0}\") = \"{1}\", \"{2}\", \"{3}\"",
                        SolutionProject.m_IsBuildTool ? s_SLNCSProjProjectGUID : s_SLNVCProjProjectGUID,
                        SolutionProject.m_CachedName, SolutionProject.m_GeneratorOutputPath,
                        SolutionProject.m_AdditionalCache.m_GUID);
                    // Defining projects this one depends.
                    SLN.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                    foreach (var SolutionDependentProject in Solution.m_CachedProjects) {
                        // Writing projects with higher priority as ones we depend from.
                        if (SolutionDependentProject.m_CachedPriority > SolutionProject.m_CachedPriority) {
                            SLN.WriteLine("\t\t{0} = {0}", SolutionDependentProject.m_AdditionalCache.m_GUID);
                        }
                    }
                    SLN.WriteLine("\tEndProjectSection");
                    SLN.WriteLine("EndProject");
                }

                SLN.WriteLine("Global");

                // ------------------------------------------------------------------------------------------
                // Linking platforms & configurations.
                // ------------------------------------------------------------------------------------------
                SLN.WriteLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
                foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                    TargetPlatformInformation PlatformInformation = TargetPlatformInformation.Get(Platform);
                    foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                        TargetConfigurationInformation ConfigurationInformation = TargetConfigurationInformation.Get(Configuration);
                        // E.g. \tConfiguration-Name|Platform-Name = Configuration-Name|Platform-Name
                        SLN.WriteLine("\t\t{0}|{1} = {0}|{1}", ConfigurationInformation.m_HumanReadableName, PlatformInformation.m_HumanReadableName);
                    }
                }
                SLN.WriteLine("\tEndGlobalSection");

                SLN.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
                foreach (var SolutionProject in Solution.m_CachedProjects) {
                    foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                        TargetPlatformInformation PlatformInformation = TargetPlatformInformation.Get(Platform);
                        foreach (TargetConfigurations Configuration in Target.EnumerateAllConfigurations()) {
                            TargetConfigurationInformation ConfigurationInformation = TargetConfigurationInformation.Get(Configuration);
                            string PlatfromConfig = null;
                            if (SolutionProject.m_IsBuildTool) {
                                // C# projects have only Debug/Release configurations. 
                                // If we specify others - projects would not be enabled in the configuration dispatcher.
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|Any CPU
                                PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {3}|Any CPU",
                                    /* 0 */ SolutionProject.m_AdditionalCache.m_GUID,                                               // GUID
                                    /*1,2*/ ConfigurationInformation.m_HumanReadableName, PlatformInformation.m_HumanReadableName,  // Human readable names
                                    /* 3 */ ConfigurationInformation.m_IsDebug ? "Debug" : "Release"                                // Configuration.
                                );
                            } else {
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|MSBuild-Plaform
                                PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {3}{4}|{5}",
                                    /* 0 */ SolutionProject.m_AdditionalCache.m_GUID,                                               // GUID
                                    /*1,2*/ ConfigurationInformation.m_HumanReadableName, PlatformInformation.m_HumanReadableName,  // Human readable names
                                    /*3,4*/ Configuration, Platform,                                                                // Internal names
                                    /* 5 */ ConvertPlatformToMSBuildPlatform(Platform)                                              // MSBuild platform
                                );
                            }

                            SLN.WriteLine(PlatfromConfig.Replace("*", "ActiveCfg"));
                            SLN.WriteLine(PlatfromConfig.Replace("*", "Build.0"));
                            //SLN.WriteLine(PlatfromConfig.Replace("*", "Deploy.0"));   // @todo Do something with deployment..
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
