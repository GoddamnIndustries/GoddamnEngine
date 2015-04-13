// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file VisualStudio.cs
//! Visual Studio IDE project files generator.

using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace GoddamnEngine.BuildSystem.ProjectGenerator.VisualStudio
{
    //! @brief Generator of the project/solution files for Visual Studio.
    internal class VisualStudioProjectGenerator : ProjectGenerator
    {
        private const string s_MSBuild2003Namespace = @"http://schemas.microsoft.com/developer/msbuild/2003";
        private const string s_SLNVCXProjProjectGUID = @"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
        private const string s_SLNCSProjProjectGUID = @"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
        private const string s_SLNDProjProjectGUID = @"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
        private const string s_SLNFilterProjectGUID = @"{2150E333-8FDC-42A3-9474-1A3956D46DE8}";

        private readonly string m_ToolsVersion = "14.0";
        private readonly string m_ToolsFiltersVersion = "4.0";

        //! @brief Returns formated GUID.
        //! @returns Formated GUID.
        internal static string CreateMSBuildGUID()
        {
            return string.Format("{{{0}}}", Guid.NewGuid().ToString()).ToUpperInvariant();
        }

        //! @brief Converts specified source file type into name of the node in MSBuild.
        //! @returns Name of the node in MSBuild.
        private static string ConvertFileTypeToVCXProjElement(ProjectSourceFileType FileType)
        {
            switch (FileType)
            {
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

        //! @brief Converts target platform into MSBuild compatible one.
        //! @param Platform Some platform.
        //! @returns MSBuild compatible one.
        private static string ConvertPlatformToMSBuildPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.Emscripten:
                //return "Win32";

                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsPhone:
                //return "ARM";

                case TargetPlatform.Windows:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.OSX:
                case TargetPlatform.Linux:
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return "x64";

                default:
                    throw new NotImplementedException();
            }
        }

        //! @brief Loads a GUID from an existing project file.
        //! @param ProjectPath Path to project file.
        //! @returns A loaded GUID on success or new generated GUID on failure.
        private static string QueryGUIDFromProject(string ProjectPath)
        {
            XmlDocument Proj = new XmlDocument();
            XmlNamespaceManager ProjNamespaceMgr = new XmlNamespaceManager(Proj.NameTable);
            ProjNamespaceMgr.AddNamespace("msb", s_MSBuild2003Namespace);
            Proj.Load(ProjectPath);

            XmlNode ProjGUIDNode = Proj.SelectSingleNode("//msb:PropertyGroup/msb:ProjectGuid", ProjNamespaceMgr);
            return ProjGUIDNode.InnerText;
        }

        //! @brief Checks if specified platform is natively supported by VS.
        //! @param Platform Some platform.
        //! @returns True if platform is natively supported by VS, false otherwise.
        public override bool IsPlatformNativelySupported(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsStore:
                case TargetPlatform.WindowsPhone:
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return true;
            }

            return false;
        }

        //! @brief Generates project files for Visual Studio: .vcxproj and .vcxproj.filter.
        //! @param Project Parsed project object.
        //! @param Configurations Array of target configurations.
        //! @returns Path to Visual Studio's .vcxproj file.
        public sealed override string GenerateProjectFiles(ProjectCache Project)
        {
            string VCXProjPath = Path.Combine(base.GenerateProjectFiles(Project), Project.m_CachedName) + ".vcxproj";

            // For build tools the best was is to load a GUID from project files: this causes exceptions 
            // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
            try
            {
                Project.m_AdditionalCache.m_GUID = QueryGUIDFromProject(VCXProjPath);
            }
            catch (Exception)
            {
                Project.m_AdditionalCache.m_GUID = CreateMSBuildGUID();
            }

            // ==========================================================================================
            // Generating VCXPROJ files.
            // ==========================================================================================
            using (XmlTextWriter VCXProj = new XmlTextWriter(VCXProjPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                VCXProj.WriteStartDocument();
                VCXProj./**/WriteStartElement("Project", s_MSBuild2003Namespace);
                VCXProj./**//**/WriteAttributeString("ToolsVersion", m_ToolsVersion);
                VCXProj./**//**/WriteAttributeString("DefaultTargets", "Build");

                // ------------------------------------------------------------------------------------------
                // Defining list of configurations.
                // ------------------------------------------------------------------------------------------
                VCXProj.WriteStartElement("ItemGroup");
                VCXProj./**/WriteAttributeString("Label", "ProjectConfigurations");
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations())
                    {
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
                foreach (var ProjectSource in Project.m_CachedSourceFiles)
                {
                    VCXProj./**/WriteStartElement(ConvertFileTypeToVCXProjElement(ProjectSource.m_FileType));
                    VCXProj./**//**/WriteAttributeString("Include", ProjectSource.m_FileName);
                    foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                    {
                        string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                        foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations())
                        {
                            if (ProjectSource.ShouldBeExcluded(Platform, Configuration))
                            {
                                string ConfigurationName = string.Concat(Configuration, Platform);
                                VCXProj./**//**/WriteStartElement("ExcludedFromBuild");
                                VCXProj./**//**//**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                                VCXProj./**//**//**/WriteString("true");
                                VCXProj./**//**/WriteEndElement();
                            }
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
                VCXProj./**/WriteElementString("ProjectGuid", Project.m_AdditionalCache.m_GUID);
                VCXProj./**/WriteElementString("RootNamespace", Project.m_CachedName);
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding main configuration properties.
                // ------------------------------------------------------------------------------------------
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                {
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);
                    string PlatformToolset = null;
                    switch (Platform)
                    {
                        case TargetPlatform.Windows:
                        case TargetPlatform.WindowsRT:
                        case TargetPlatform.WindowsStore:
                            PlatformToolset = "CTP_Nov2013";
                            break;

                        case TargetPlatform.WindowsPhone:
                            PlatformToolset = "v120_wp81";
                            break;

                        // These are not supported, setting up some generic toolset.
                        default:
                            PlatformToolset = "v120";
                            break;
                    }

                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations())
                    {
                        string ConfigurationName = string.Concat(Configuration, Platform);
                        VCXProj.WriteStartElement("PropertyGroup");
                        VCXProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                        VCXProj./**/WriteAttributeString("Label", "Configuration");
                        VCXProj./**/WriteElementString("PlatformToolset", PlatformToolset);
                        if (IsPlatformNativelySupported(Platform))
                        {
                            VCXProj./**/WriteElementString("ConfigurationType", Project.m_CachedBuildTypes[Platform, Configuration].ToString());
                            VCXProj./**/WriteElementString("CharacterSet", "MultiByte");
                            if (TargetPlatformInfo.Get(Platform).m_RequiresRTTI)
                            {
                                VCXProj./**/WriteElementString("CLRSupport", "true");
                            }
                        }
                        else
                        {
                            VCXProj./**/WriteElementString("ConfigurationType", "Makefile");
                        }
                        VCXProj.WriteEndElement();
                    }
                }

                VCXProj.WriteStartElement("Import");
                VCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                VCXProj.WriteEndElement();
                VCXProj.WriteStartElement("Import");
                VCXProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
                VCXProj.WriteEndElement();
                VCXProj.WriteStartElement("PropertyGroup");
                VCXProj./**/WriteAttributeString("Label", "UserMacros");
                VCXProj.WriteEndElement();
                VCXProj.WriteStartElement("PropertyGroup");
                VCXProj./**/WriteElementString("NMakeForcedIncludes", "$(NMakeForcedIncludes)");
                VCXProj./**/WriteElementString("NMakeAssemblySearchPath", "$(NMakeAssemblySearchPath)");
                VCXProj./**/WriteElementString("NMakeForcedUsingAssemblies", "$(NMakeForcedUsingAssemblies)");
                VCXProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding VC++ compiler and linker properties OR NMake commands..
                // ------------------------------------------------------------------------------------------
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                {
                    TargetPlatformInfo PlatformInfo = TargetPlatformInfo.Get(Platform);
                    string PlatformString = ConvertPlatformToMSBuildPlatform(Platform);

                    // Building list of header directories.
                    string IncludePathes = Project.GenerateIncludePaths();
                    string StandartLibrariesPaths = PlatformInfo.GenerateStandartIncludePaths();
                    if (string.IsNullOrEmpty(StandartLibrariesPaths))
                    {
                        StandartLibrariesPaths = "$(NMakeIncludeSearchPath)";
                    }

                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations())
                    {
                        TargetConfigurationInfo ConfigurationInfo = TargetConfigurationInfo.Get(Configuration);

                        // Defining macros..
                        string Macros = Project.GenerateMacros(Platform, Configuration);
                        string StandartMacros = PlatformInfo.GenerateStandartMacrosList();
                        if (string.IsNullOrEmpty(StandartMacros))
                        {
                            StandartMacros = "$(NMakePreprocessorDefinitions)";
                        }

                        // Collecting linked libraries..
                        string LinkedLibraries = Project.GenerateLinkedLibrariesPaths(Platform, Configuration);
                        string StandartLinkedLibraries = PlatformInfo.GenerateStandartLinkedLibrariesPaths();
                        if (string.IsNullOrEmpty(StandartLinkedLibraries))
                        {
                            StandartLinkedLibraries = "";
                        }
                        StandartLinkedLibraries += "%(AdditionalDependencies)";

                        string ConfigurationName = string.Concat(Configuration, Platform);
                        string OutputPath = Project.m_CachedOutputPaths[Platform, Configuration];

                        VCXProj.WriteStartElement("ImportGroup");
                        VCXProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                        VCXProj./**/WriteAttributeString("Label", "PropertySheets");
                        VCXProj./**/WriteStartElement("Import");
                        VCXProj./**//**/WriteAttributeString("Project", @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                        VCXProj./**//**/WriteAttributeString("Condition", @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                        VCXProj./**//**/WriteAttributeString("Label", "LocalAppDataPlatform");
                        VCXProj./**/WriteEndElement();
                        VCXProj.WriteEndElement();

                        if (IsPlatformNativelySupported(Platform))
                        {
                            // ------------------------------------------------------------------------------------------
                            // Defining VC++ compiler and linker properties.
                            // ------------------------------------------------------------------------------------------

                            // Include paths..
                            VCXProj.WriteStartElement("PropertyGroup");
                            VCXProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VCXProj./**/WriteElementString("IncludePath", IncludePathes + "$(IncludePath)");
                            VCXProj./**/WriteElementString("OutDir", Path.GetDirectoryName(OutputPath) + Path.DirectorySeparatorChar);
                            VCXProj./**/WriteElementString("TargetName", Path.GetFileNameWithoutExtension(OutputPath));
                            VCXProj.WriteEndElement();

                            // Compiler properties..
                            VCXProj.WriteStartElement("ItemDefinitionGroup");
                            VCXProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VCXProj./**/WriteStartElement("ClCompile");
                            VCXProj./**//**/WriteElementString("FavorSizeOrSpeed", "Speed");
                            VCXProj./**//**/WriteElementString("PrecompiledHeader", "NotUsing");
                            VCXProj./**//**/WriteElementString("CallingConvention", "FastCall");
                            VCXProj./**//**/WriteElementString("WarningLevel", "Level" + TargetCppCompilerInformation.s_WarningLevel);
                            VCXProj./**//**/WriteElementString("TreatWarningAsError", TargetCppCompilerInformation.s_WarningsAreErrors.ToString());
                            VCXProj./**//**/WriteElementString("PreprocessorDefinitions", Macros + "%(PreprocessorDefinition)");
                            VCXProj./**//**/WriteElementString("Optimization", ConfigurationInfo.m_Optimize ? "Full" : "Disabled");
                            if (ConfigurationInfo.m_IsDebug)
                            {
                                VCXProj.WriteElementString("RuntimeLibrary", "MultiThreadedDebugDLL");
                            }
                            if (!PlatformInfo.m_RequiresRTTI)
                            {
                                VCXProj.WriteElementString("RuntimeTypeInfo", "false");
                            }
                            if (!PlatformInfo.m_RequiresExceptions)
                            {
                                VCXProj.WriteElementString("ExceptionHandling", "false");
                            }
                            VCXProj./**/WriteEndElement(); // </ VC++ COMPILER PROPERTIES

                            // Linker properties.
                            VCXProj./**/WriteStartElement("Link");
                            VCXProj./**//**/WriteElementString("AdditionalDependencies", LinkedLibraries + StandartLinkedLibraries);
                            VCXProj./**//**/WriteElementString("GenerateDebugInformation", ConfigurationInfo.m_GenerateDebugInformation.ToString().ToLowerInvariant());
                            if (Project.m_CachedBuildTypes[Platform, Configuration] == ProjectBuildType.DynamicLibrary)
                            {
                                VCXProj./**//**/WriteElementString("ImportLibrary", Project.m_CachedImportLibraryOutputPaths[Platform, Configuration]);
                            }
                            VCXProj./**/WriteEndElement();
                            VCXProj.WriteEndElement();

                        }
                        else
                        {

                            // ------------------------------------------------------------------------------------------
                            // Defining NMake properties..
                            // ------------------------------------------------------------------------------------------

                            string NMakeCommand = string.Format("\"{0}\" --compile-project \"{1}\" {2} {3} ",
                                System.Reflection.Assembly.GetExecutingAssembly().Location,
                                Project.m_CachedSource,
                                Platform, Configuration
                            );

                            VCXProj.WriteStartElement("PropertyGroup");
                            VCXProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VCXProj./**/WriteElementString("OutDir", Path.GetDirectoryName(OutputPath) + Path.DirectorySeparatorChar);
                            VCXProj./**/WriteElementString("NMakeOutput", OutputPath);
                            VCXProj./**/WriteElementString("NMakePreprocessorDefinitions", Macros + StandartMacros + ";GD_PLATFORM_NOT_WINDOWS=1");
                            VCXProj./**/WriteElementString("NMakeIncludeSearchPath", IncludePathes + StandartLibrariesPaths);
                            VCXProj./**/WriteElementString("NMakeBuildCommandLine", NMakeCommand);
                            VCXProj./**/WriteElementString("NMakeReBuildCommandLine", NMakeCommand + "Rebuild");
                            VCXProj./**/WriteElementString("NMakeCleanCommandLine", NMakeCommand + "Clean");
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
            using (XmlTextWriter VCXProjFilters = new XmlTextWriter(VCXProjFiltersPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                VCXProjFilters.WriteStartDocument();
                VCXProjFilters./**/WriteStartElement("Project", s_MSBuild2003Namespace);
                VCXProjFilters./**/WriteAttributeString("ToolsVersion", m_ToolsFiltersVersion);

                HashSet<string> ProjectFoldersCache = new HashSet<string>();
                foreach (var ProjectSourceFile in Project.m_CachedSourceFiles)
                {
                    string ProjectSourceFileDirectory = Path.GetDirectoryName(ProjectSourceFile.m_FileName);
                    string ProjectSourceFilter;

                    // Checking if this source can have a filter: it is located in project directory and it not in it's root.
                    // We also add "Source" to each filter to separate config and source files.  
                    if (ProjectSourceFileDirectory.StartsWith(Project.m_CachedSourcesFilterOrigin, StringComparison.InvariantCultureIgnoreCase)
                     && (ProjectSourceFileDirectory.Length > (Project.m_CachedSourcesFilterOrigin.Length + 1)))
                    {
                        ProjectSourceFilter = "Source\\" + ProjectSourceFileDirectory.Substring(Project.m_CachedSourcesFilterOrigin.Length + 1);
                        string ProjectSourceFilterIter = ProjectSourceFilter;
                        while ((!ProjectFoldersCache.Contains(ProjectSourceFilterIter)) && (!string.IsNullOrEmpty(ProjectSourceFilterIter)))
                        {
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
                    }
                    else
                    {
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

        //! @brief Generates solution files for Visual Studio.
        //! @param Solution Parsed solution object.
        //! @returns Path to Visual Studio's .sln file.
        public sealed override string GenerateSolutionFiles(SolutionCache Solution)
        {
            // ==========================================================================================
            // Generating SLN files..
            // ==========================================================================================
            string SLNPath = Path.Combine(base.GenerateSolutionFiles(Solution), Solution.m_CachedName + ".sln");
            using (StreamWriter SLN = new StreamWriter(SLNPath))
            {
                SLN.WriteLine();
                SLN.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
                SLN.WriteLine("# Generated by the GoddamnBuildSystem.");

                // ------------------------------------------------------------------------------------------
                // Caching GUIDs of projects and filters and defining filters.
                // ------------------------------------------------------------------------------------------
                var SolutionFiltersGUIDCache = new Dictionary<int, string>();
                foreach (var SolutionProject in Solution.m_CachedProjects)
                {

                    // Loading the GUID of a build tool..
                    if (SolutionProject.m_IsBuildTool)
                    {
                        // For build tools the best was is to load a GUID from project files: this causes exceptions 
                        // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
                        try
                        {
                            SolutionProject.m_AdditionalCache.m_GUID = QueryGUIDFromProject(SolutionProject.m_GeneratorOutputPath);
                        }
                        catch (Exception Exception)
                        {
                            Console.Error.WriteLine("Unable to load a GUID of C# project \"{0}\". Check this out...", Solution.m_GeneratedSolutionPath);
                            Console.Error.WriteLine(Exception.ToString());

                            SolutionProject.m_AdditionalCache.m_GUID = CreateMSBuildGUID();
                        }
                    }

                    // Looking for projects with filters which we have not already cached.
                    if (!string.IsNullOrEmpty(SolutionProject.m_CachedFilter))
                    {
                        int SolutionProjectFilterHash = SolutionProject.m_CachedFilter.GetHashCode();
                        if (!SolutionFiltersGUIDCache.ContainsKey(SolutionProjectFilterHash))
                        {
                            // Generating and caching filter GUIDS.
                            SolutionProject.m_AdditionalCache.m_FilterGUID = CreateMSBuildGUID();
                            SolutionFiltersGUIDCache.Add(SolutionProjectFilterHash, SolutionProject.m_AdditionalCache.m_FilterGUID);

                            // E.g. 'Project({Filter-GUID}) = "Name", "Name", "Filter-GUID"'
                            SLN.WriteLine("Project(\"{0}\") = \"{1}\", \"{1}\", \"{2}\"", s_SLNFilterProjectGUID, SolutionProject.m_CachedFilter, SolutionProject.m_AdditionalCache.m_FilterGUID);
                            SLN.WriteLine("EndProject");
                        }
                        else
                        {
                            SolutionProject.m_AdditionalCache.m_FilterGUID = SolutionFiltersGUIDCache[SolutionProjectFilterHash];
                        }
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Defining Solution projects.
                // ------------------------------------------------------------------------------------------
                foreach (var SolutionProject in Solution.m_CachedProjects)
                {
                    // E.g. 'Project({Type-GUID}) = "Name", "Path-ToProject-File", "Project-GUID"'
                    SLN.WriteLine("Project(\"{0}\") = \"{1}\", \"{2}\", \"{3}\"",
                        /* 0 */ SolutionProject.m_IsBuildTool ? s_SLNCSProjProjectGUID : s_SLNVCXProjProjectGUID,
                        /*1,2*/ SolutionProject.m_CachedName, SolutionProject.m_GeneratorOutputPath,
                        /* 3 */ SolutionProject.m_AdditionalCache.m_GUID);
                    // Defining projects this one depends.
                    SLN.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                    foreach (var SolutionDependentProject in Solution.m_CachedProjects)
                    {
                        // Writing projects with higher priority as ones we depend from.
                        if (SolutionDependentProject.m_CachedPriority > SolutionProject.m_CachedPriority)
                        {
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
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                {
                    TargetPlatformInfo PlatformInfo = TargetPlatformInfo.Get(Platform);
                    foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations())
                    {
                        TargetConfigurationInfo ConfigurationInfo = TargetConfigurationInfo.Get(Configuration);
                        // E.g. \tConfiguration-Name|Platform-Name = Configuration-Name|Platform-Name
                        SLN.WriteLine("\t\t{0}|{1} = {0}|{1}", ConfigurationInfo.m_HumanReadableName, PlatformInfo.m_HumanReadableName);
                    }
                }
                SLN.WriteLine("\tEndGlobalSection");

                SLN.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
                foreach (var SolutionProject in Solution.m_CachedProjects)
                {
                    foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                    {
                        TargetPlatformInfo PlatformInfo = TargetPlatformInfo.Get(Platform);
                        foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations())
                        {
                            TargetConfigurationInfo ConfigurationInfo = TargetConfigurationInfo.Get(Configuration);
                            string PlatfromConfig = null;
                            if (SolutionProject.m_IsBuildTool)
                            {
                                // C# projects have only Debug/Release configurations. 
                                // If we specify others - projects would not be enabled in the configuration dispatcher.
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|Any CPU
                                PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {3}|Any CPU",
                                    /* 0 */ SolutionProject.m_AdditionalCache.m_GUID,              // GUID
                                    /*1,2*/ ConfigurationInfo.m_HumanReadableName, PlatformInfo.m_HumanReadableName,  // Human readable names
                                    /* 3 */ ConfigurationInfo.m_IsDebug ? "Debug" : "Release"        // Configuration.
                                );
                            }
                            else
                            {
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|MSBuild-Plaform
                                PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {3}{4}|{5}",
                                    /* 0 */ SolutionProject.m_AdditionalCache.m_GUID,              // GUID
                                    /*1,2*/ ConfigurationInfo.m_HumanReadableName, PlatformInfo.m_HumanReadableName,  // Human readable names
                                    /*3,4*/ Configuration, Platform,                // Internal names
                                    /* 5 */ ConvertPlatformToMSBuildPlatform(Platform)             // MSBuild platform
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
                foreach (var SolutionProject in Solution.m_CachedProjects)
                {
                    if (!string.IsNullOrEmpty(SolutionProject.m_CachedFilter))
                    {
                        SLN.WriteLine("\t\t{0} = {1}", SolutionProject.m_AdditionalCache.m_GUID, SolutionProject.m_AdditionalCache.m_FilterGUID);
                    }
                }
                SLN.WriteLine("\tEndGlobalSection");
                SLN.WriteLine("EndGlobal");
            }

            return SLNPath;
        }
    }   // class VisualStudioProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator.VisualStudio
