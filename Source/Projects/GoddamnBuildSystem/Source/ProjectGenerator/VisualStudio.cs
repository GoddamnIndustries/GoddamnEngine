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
using System.Linq;
using System.Xml;

namespace GoddamnEngine.BuildSystem.Source.ProjectGenerator
{
    //[
    //! Generator of the project/solution files for Visual Studio.
    // ReSharper disable once UnusedMember.Global
    internal class VisualStudioProjectGenerator : GoddamnEngine.BuildSystem.ProjectGenerator.ProjectGenerator
    {
        private const string MsBuild2003Namespace  = @"http://schemas.microsoft.com/developer/msbuild/2003";
        private const string SlnVcxProjProjectGuid = @"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
        private const string SlnCsProjProjectGuid  = @"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
/*
        private const string SlnDprojProjectGuid   = @"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
*/
        private const string SlnFilterProjectGuid  = @"{2150E333-8FDC-42A3-9474-1A3956D46DE8}";

        private const string ToolsVersion          = "14.0";
        private const string ToolsFiltersVersion   = "4.0";

        //[
        //! Returns formatted GUID.
        //! @returns Formatted GUID.
        private static string CreateMsBuildGuid()
        {
            return string.Format("{{{0}}}", Guid.NewGuid().ToString()).ToUpperInvariant();
        }

        //[
        //! Converts specified source file type into name of the node in MSBuild.
        //! @returns Name of the node in MSBuild.
        private static string ConvertFileTypeToVcxProjElement(ProjectSourceFileType FileType)
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

        //[
        //! Converts target platform into MSBuild compatible one.
        //! @param Platform Some platform.
        //! @returns MSBuild compatible one.
        private static string ConvertPlatformToMsBuildPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.iOS:
                case TargetPlatform.Android:
                case TargetPlatform.Emscripten:
                    //! @todo Fix correct toolset selection.
                    //return "Win32";

                case TargetPlatform.WindowsRT:
                case TargetPlatform.WindowsPhone:
                    //! @todo Fix correct toolset selection.
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

        //[
        //! Loads a GUID from an existing project file.
        //! @param ProjectPath Path to project file.
        //! @returns A loaded GUID on success or new generated GUID on failure.
        private static string QueryGuidFromProject(string ProjectPath)
        {
            XmlDocument Proj = new XmlDocument();
            XmlNamespaceManager ProjNamespaceMgr = new XmlNamespaceManager(Proj.NameTable);
            ProjNamespaceMgr.AddNamespace("msb", MsBuild2003Namespace);
            Proj.Load(ProjectPath);

            var ProjGuidNode = Proj.SelectSingleNode("//msb:PropertyGroup/msb:ProjectGuid", ProjNamespaceMgr);
            return ProjGuidNode != null ? ProjGuidNode.InnerText : null;
        }

        //[
        //! Checks if specified platform is natively supported by VS.
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

        //[
        //! Generates project files for Visual Studio: .vcxproj and .vcxproj.filter.
        //! @param Project Parsed project object.
        //! @param Configurations Array of target configurations.
        //! @returns Path to Visual Studio's .vcxproj file.
        public sealed override string GenerateProjectFiles(ProjectCache Project)
        {
            var VcxProjPath = Path.Combine(base.GenerateProjectFiles(Project), Project.CachedName) + ".vcxproj";

            // For build tools the best was is to load a GUID from project files: this causes exceptions 
            // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
            try
            {
                Project.AdditionalCache.GUID = QueryGuidFromProject(VcxProjPath);
            }
            catch (Exception)
            {
                Project.AdditionalCache.GUID = CreateMsBuildGuid();
            }

            // ==========================================================================================
            // Generating VCXPROJ files.
            // ==========================================================================================
            using (var VcxProj = new XmlTextWriter(VcxProjPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                VcxProj.WriteStartDocument();
                VcxProj./**/WriteStartElement("Project", MsBuild2003Namespace);
                VcxProj./**//**/WriteAttributeString("ToolsVersion", ToolsVersion);
                VcxProj./**//**/WriteAttributeString("DefaultTargets", "Build");

                //[
                // Defining list of configurations.
                //[
                VcxProj.WriteStartElement("ItemGroup");
                VcxProj./**/WriteAttributeString("Label", "ProjectConfigurations");
                foreach (var Platform in Target.EnumerateAllPlatforms())
                {
                    var PlatformString = ConvertPlatformToMsBuildPlatform(Platform);
                    var Platform1 = Platform;   // Access to foreach variable in closure shit..
                    foreach (var ConfigurationName in Target.EnumerateAllConfigurations().Select(Configuration => string.Concat(Configuration, Platform1)))
                    {
                        VcxProj./**/WriteStartElement("ProjectConfiguration");
                        VcxProj./**//**/WriteAttributeString("Include", ConfigurationName + '|' + PlatformString);
                        VcxProj./**//**/WriteElementString("Configuration", ConfigurationName);
                        VcxProj./**//**/WriteElementString("Platform", PlatformString);
                        VcxProj./**/WriteEndElement();
                    }
                }
                VcxProj.WriteEndElement();

                //[
                // Defining list of source files.
                //[
                VcxProj.WriteStartElement("ItemGroup");
                foreach (var ProjectSource in Project.CachedSourceFiles)
                {
                    VcxProj./**/WriteStartElement(ConvertFileTypeToVcxProjElement(ProjectSource.FileType));
                    VcxProj./**//**/WriteAttributeString("Include", ProjectSource.FileName);
                    foreach (var Platform in Target.EnumerateAllPlatforms())
                    {
                        var PlatformString = ConvertPlatformToMsBuildPlatform(Platform);
                        foreach (var Configuration in Target.EnumerateAllConfigurations())
                        {
                            if (ProjectSource.ShouldBeExcluded(Platform, Configuration))
                            {
                                var ConfigurationName = string.Concat(Configuration, Platform);
                                VcxProj./**//**/WriteStartElement("ExcludedFromBuild");
                                VcxProj./**//**//**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                                VcxProj./**//**//**/WriteString("true");
                                VcxProj./**//**/WriteEndElement();
                            }
                        }
                    }
                    VcxProj./**/WriteEndElement();
                }
                VcxProj.WriteEndElement();

                //[
                // Overriding global project properties.
                //[
                VcxProj.WriteStartElement("PropertyGroup");
                VcxProj./**/WriteAttributeString("Label", "Globals");
                VcxProj./**/WriteElementString("ProjectGuid", Project.AdditionalCache.GUID);
                VcxProj./**/WriteElementString("RootNamespace", Project.CachedName);
                VcxProj.WriteEndElement();

                //[
                // Overriding main configuration properties.
                //[
                foreach (var Platform in Target.EnumerateAllPlatforms())
                {
                    var PlatformString = ConvertPlatformToMsBuildPlatform(Platform);
                    string PlatformToolset;
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
                        VcxProj.WriteStartElement("PropertyGroup");
                        VcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                        VcxProj./**/WriteAttributeString("Label", "Configuration");
                        VcxProj./**/WriteElementString("PlatformToolset", PlatformToolset);
                        if (IsPlatformNativelySupported(Platform))
                        {
                            VcxProj./**/WriteElementString("ConfigurationType", Project.CachedBuildTypes[Platform, Configuration].ToString());
                            VcxProj./**/WriteElementString("CharacterSet", "MultiByte");
                            if (TargetPlatformInfo.Get(Platform).RequiresRTTI)
                            {
                                VcxProj./**/WriteElementString("CLRSupport", "true");
                            }
                        }
                        else
                        {
                            VcxProj./**/WriteElementString("ConfigurationType", "Makefile");
                        }
                        VcxProj.WriteEndElement();
                    }
                }

                VcxProj.WriteStartElement("Import");
                VcxProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                VcxProj.WriteEndElement();
                VcxProj.WriteStartElement("Import");
                VcxProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
                VcxProj.WriteEndElement();
                VcxProj.WriteStartElement("PropertyGroup");
                VcxProj./**/WriteAttributeString("Label", "UserMacros");
                VcxProj.WriteEndElement();
                VcxProj.WriteStartElement("PropertyGroup");
                VcxProj./**/WriteElementString("NMakeForcedIncludes", "$(NMakeForcedIncludes)");
                VcxProj./**/WriteElementString("NMakeAssemblySearchPath", "$(NMakeAssemblySearchPath)");
                VcxProj./**/WriteElementString("NMakeForcedUsingAssemblies", "$(NMakeForcedUsingAssemblies)");
                VcxProj.WriteEndElement();

                //[
                // Overriding VC++ compiler and linker properties OR NMake commands..
                //[
                foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms())
                {
                    TargetPlatformInfo PlatformInfo = TargetPlatformInfo.Get(Platform);
                    string PlatformString = ConvertPlatformToMsBuildPlatform(Platform);

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
                        string OutputPath = Project.CachedOutputPaths[Platform, Configuration];

                        VcxProj.WriteStartElement("ImportGroup");
                        VcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                        VcxProj./**/WriteAttributeString("Label", "PropertySheets");
                        VcxProj./**/WriteStartElement("Import");
                        VcxProj./**//**/WriteAttributeString("Project", @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                        VcxProj./**//**/WriteAttributeString("Condition", @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                        VcxProj./**//**/WriteAttributeString("Label", "LocalAppDataPlatform");
                        VcxProj./**/WriteEndElement();
                        VcxProj.WriteEndElement();

                        if (IsPlatformNativelySupported(Platform))
                        {
                            //[
                            // Defining VC++ compiler and linker properties.
                            //[

                            // Include paths..
                            VcxProj.WriteStartElement("PropertyGroup");
                            VcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VcxProj./**/WriteElementString("IncludePath", IncludePathes + "$(IncludePath)");
                            VcxProj./**/WriteElementString("OutDir", Path.GetDirectoryName(OutputPath) + Path.DirectorySeparatorChar);
                            VcxProj./**/WriteElementString("TargetName", Path.GetFileNameWithoutExtension(OutputPath));
                            VcxProj.WriteEndElement();

                            // Compiler properties..
                            VcxProj.WriteStartElement("ItemDefinitionGroup");
                            VcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VcxProj./**/WriteStartElement("ClCompile");
                            VcxProj./**//**/WriteElementString("FavorSizeOrSpeed", "Speed");
                            VcxProj./**//**/WriteElementString("PrecompiledHeader", "NotUsing");
                            VcxProj./**//**/WriteElementString("CallingConvention", "FastCall");
                            VcxProj./**//**/WriteElementString("WarningLevel", "Level" + TargetCppCompilerInformation.WarningLevel);
                            VcxProj./**//**/WriteElementString("TreatWarningAsError", TargetCppCompilerInformation.WarningsAreErrors.ToString());
                            VcxProj./**//**/WriteElementString("PreprocessorDefinitions", Macros + "%(PreprocessorDefinition)");
                            VcxProj./**//**/WriteElementString("Optimization", ConfigurationInfo.Optimize ? "Full" : "Disabled");
                            if (ConfigurationInfo.IsDebug)
                            {
                                VcxProj.WriteElementString("RuntimeLibrary", "MultiThreadedDebugDLL");
                            }
                            if (!PlatformInfo.RequiresRTTI)
                            {
                                VcxProj.WriteElementString("RuntimeTypeInfo", "false");
                            }
                            if (!PlatformInfo.RequiresExceptions)
                            {
                                VcxProj.WriteElementString("ExceptionHandling", "false");
                            }
                            VcxProj./**/WriteEndElement(); // </ VC++ COMPILER PROPERTIES

                            // Linker properties.
                            VcxProj./**/WriteStartElement("Link");
                            VcxProj./**//**/WriteElementString("AdditionalDependencies", LinkedLibraries + StandartLinkedLibraries);
                            VcxProj./**//**/WriteElementString("GenerateDebugInformation", ConfigurationInfo.GenerateDebugInformation.ToString().ToLowerInvariant());
                            if (Project.CachedBuildTypes[Platform, Configuration] == ProjectBuildType.DynamicLibrary)
                            {
                                VcxProj./**//**/WriteElementString("ImportLibrary", Project.CachedImportLibraryOutputPaths[Platform, Configuration]);
                            }
                            VcxProj./**/WriteEndElement();
                            VcxProj.WriteEndElement();

                        }
                        else
                        {

                            //[
                            // Defining NMake properties..
                            //[

                            string NMakeCommand = string.Format("\"{0}\" --compile-project \"{1}\" {2} {3} ",
                                System.Reflection.Assembly.GetExecutingAssembly().Location,
                                Project.CachedSource,
                                Platform, Configuration
                            );

                            VcxProj.WriteStartElement("PropertyGroup");
                            VcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", ConfigurationName, PlatformString);
                            VcxProj./**/WriteElementString("OutDir", Path.GetDirectoryName(OutputPath) + Path.DirectorySeparatorChar);
                            VcxProj./**/WriteElementString("NMakeOutput", OutputPath);
                            VcxProj./**/WriteElementString("NMakePreprocessorDefinitions", Macros + StandartMacros + ";GD_PLATFORNOT_WINDOWS=1");
                            VcxProj./**/WriteElementString("NMakeIncludeSearchPath", IncludePathes + StandartLibrariesPaths);
                            VcxProj./**/WriteElementString("NMakeBuildCommandLine", NMakeCommand);
                            VcxProj./**/WriteElementString("NMakeReBuildCommandLine", NMakeCommand + "Rebuild");
                            VcxProj./**/WriteElementString("NMakeCleanCommandLine", NMakeCommand + "Clean");
                            VcxProj.WriteEndElement();
                        }
                    }
                }

                VcxProj.WriteStartElement("Import");
                VcxProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets");
                VcxProj.WriteEndElement();

                VcxProj./**/WriteEndElement();
                VcxProj.WriteEndDocument();
            }

            // ==========================================================================================
            // Generating VCPROJ.FILTER files.
            // ==========================================================================================
            var VcxProjFiltersPath = VcxProjPath + ".filters";
            using (var VcxProjFilters = new XmlTextWriter(VcxProjFiltersPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                VcxProjFilters.WriteStartDocument();
                VcxProjFilters./**/WriteStartElement("Project", MsBuild2003Namespace);
                VcxProjFilters./**/WriteAttributeString("ToolsVersion", ToolsFiltersVersion);

                HashSet<string> ProjectFoldersCache = new HashSet<string>();
                foreach (var ProjectSourceFile in Project.CachedSourceFiles)
                {
                    string ProjectSourceFileDirectory = Path.GetDirectoryName(ProjectSourceFile.FileName);
                    string ProjectSourceFilter;

                    // Checking if this source can have a filter: it is located in project directory and it not in it's root.
                    // We also add "Source" to each filter to separate config and source files.  
                    if (ProjectSourceFileDirectory != null && (ProjectSourceFileDirectory.StartsWith(Project.CachedSourcesFilterOrigin, StringComparison.InvariantCultureIgnoreCase)
                                                           && (ProjectSourceFileDirectory.Length > (Project.CachedSourcesFilterOrigin.Length + 1))))
                    {
                        ProjectSourceFilter = "Source\\" + ProjectSourceFileDirectory.Substring(Project.CachedSourcesFilterOrigin.Length + 1);
                        string ProjectSourceFilterIter = ProjectSourceFilter;
                        while ((!ProjectFoldersCache.Contains(ProjectSourceFilterIter)) && (!string.IsNullOrEmpty(ProjectSourceFilterIter)))
                        {
                            //[
                            // Defining group of filters.
                            //[
                            VcxProjFilters.WriteStartElement("ItemGroup");
                            VcxProjFilters./**/WriteStartElement("Filter");
                            VcxProjFilters./**//**/WriteAttributeString("Include", ProjectSourceFilterIter);
                            VcxProjFilters./**//**/WriteElementString("UniqueIdentifier", CreateMsBuildGuid());
                            VcxProjFilters./**/WriteEndElement();
                            VcxProjFilters.WriteEndElement();

                            ProjectFoldersCache.Add(ProjectSourceFilterIter);
                            ProjectSourceFilterIter = Path.GetDirectoryName(ProjectSourceFilterIter);
                        }
                    }
                    else
                    {
                        ProjectSourceFilter = "Source";
                    }

                    //[
                    // Defining source's filters.
                    //[
                    VcxProjFilters.WriteStartElement("ItemGroup");
                    VcxProjFilters./**/WriteStartElement(ConvertFileTypeToVcxProjElement(ProjectSourceFile.FileType));
                    VcxProjFilters./**//**/WriteAttributeString("Include", ProjectSourceFile.FileName);
                    VcxProjFilters./**//**/WriteElementString("Filter", ProjectSourceFilter);
                    VcxProjFilters./**/WriteEndElement();
                    VcxProjFilters.WriteEndElement();
                }

                VcxProjFilters./**/WriteEndElement();
                VcxProjFilters.WriteEndDocument();
            }

            return VcxProjPath;
        }

        //[
        //! Generates solution files for Visual Studio.
        //! @param Solution Parsed solution object.
        //! @returns Path to Visual Studio's .sln file.
        public sealed override string GenerateSolutionFiles(SolutionCache Solution)
        {
            // ==========================================================================================
            // Generating SLN files..
            // ==========================================================================================
            var SlnPath = Path.Combine(base.GenerateSolutionFiles(Solution), Solution.CachedName + ".sln");
            using (var Sln = new StreamWriter(SlnPath))
            {
                Sln.WriteLine();
                Sln.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
                Sln.WriteLine("# Generated by the GoddamnBuildSystem.");

                //[
                // Caching GUIDs of projects and filters and defining filters.
                //[
                var SolutionFiltersGuidCache = new Dictionary<int, string>();
                foreach (var SolutionProject in Solution.CachedProjects)
                {

                    // Loading the GUID of a build tool..
                    if (SolutionProject.IsBuildTool)
                    {
                        // For build tools the best was is to load a GUID from project files: this causes exceptions 
                        // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
                        try
                        {
                            SolutionProject.AdditionalCache.GUID = QueryGuidFromProject(SolutionProject.GeneratorOutputPath);
                        }
                        catch (Exception Exception)
                        {
                            Console.Error.WriteLine("Unable to load a GUID of C# project \"{0}\". Check this out...", Solution.GeneratedSolutionPath);
                            Console.Error.WriteLine(Exception.ToString());

                            SolutionProject.AdditionalCache.GUID = CreateMsBuildGuid();
                        }
                    }

                    // Looking for projects with filters which we have not already cached.
                    if (!string.IsNullOrEmpty(SolutionProject.CachedFilter))
                    {
                        int SolutionProjectFilterHash = SolutionProject.CachedFilter.GetHashCode();
                        if (!SolutionFiltersGuidCache.ContainsKey(SolutionProjectFilterHash))
                        {
                            // Generating and caching filter GUIDS.
                            SolutionProject.AdditionalCache.FilterGUID = CreateMsBuildGuid();
                            SolutionFiltersGuidCache.Add(SolutionProjectFilterHash, SolutionProject.AdditionalCache.FilterGUID);

                            // E.g. 'Project({Filter-GUID}) = "Name", "Name", "Filter-GUID"'
                            Sln.WriteLine("Project(\"{0}\") = \"{1}\", \"{1}\", \"{2}\"", SlnFilterProjectGuid, SolutionProject.CachedFilter, SolutionProject.AdditionalCache.FilterGUID);
                            Sln.WriteLine("EndProject");
                        }
                        else
                        {
                            SolutionProject.AdditionalCache.FilterGUID = SolutionFiltersGuidCache[SolutionProjectFilterHash];
                        }
                    }
                }

                //[
                // Defining Solution projects.
                //[
                foreach (var SolutionProject in Solution.CachedProjects)
                {
                    // E.g. 'Project({Type-GUID}) = "Name", "Path-ToProject-File", "Project-GUID"'
                    Sln.WriteLine("Project(\"{0}\") = \"{1}\", \"{2}\", \"{3}\"",
                        /* 0 */ SolutionProject.IsBuildTool ? SlnCsProjProjectGuid : SlnVcxProjProjectGuid,
                        /*1,2*/ SolutionProject.CachedName, SolutionProject.GeneratorOutputPath,
                        /* 3 */ SolutionProject.AdditionalCache.GUID);
                    // Defining projects this one depends.
                    Sln.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                    var Project = SolutionProject;
                    foreach (var SolutionDependentProject in Solution.CachedProjects.Where(SolutionDependentProject => SolutionDependentProject.CachedPriority > Project.CachedPriority))
                    {
                        // Writing projects with higher priority as ones we depend from.
                        Sln.WriteLine("\t\t{0} = {0}", SolutionDependentProject.AdditionalCache.GUID);
                    }
                    Sln.WriteLine("\tEndProjectSection");
                    Sln.WriteLine("EndProject");
                }

                Sln.WriteLine("Global");

                //[
                // Linking platforms & configurations.
                //[
                Sln.WriteLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
                foreach (var Platform in Target.EnumerateAllPlatforms())
                {
                    var PlatformInfo = TargetPlatformInfo.Get(Platform);
                    foreach (var ConfigurationInfo in Target.EnumerateAllConfigurations().Select(TargetConfigurationInfo.Get))
                    {
                        // E.g. \tConfiguration-Name|Platform-Name = Configuration-Name|Platform-Name
                        Sln.WriteLine("\t\t{0}|{1} = {0}|{1}", ConfigurationInfo.HumanReadableName, PlatformInfo.HumanReadableName);
                    }
                }
                Sln.WriteLine("\tEndGlobalSection");

                Sln.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
                foreach (var SolutionProject in Solution.CachedProjects)
                {
                    foreach (var Platform in Target.EnumerateAllPlatforms())
                    {
                        var PlatformInfo = TargetPlatformInfo.Get(Platform);
                        foreach (var Configuration in Target.EnumerateAllConfigurations())
                        {
                            string PlatfromConfig;
                            var ConfigurationInfo = TargetConfigurationInfo.Get(Configuration);
                            if (SolutionProject.IsBuildTool)
                            {
                                // C# projects have only Debug/Release configurations. 
                                // If we specify others - projects would not be enabled in the configuration dispatcher.
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|Any CPU
                                PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {3}|Any CPU",
                                    /* 0 */ SolutionProject.AdditionalCache.GUID,                                   // GUID
                                    /*1,2*/ ConfigurationInfo.HumanReadableName, PlatformInfo.HumanReadableName,    // Human readable names
                                    /* 3 */ ConfigurationInfo.IsDebug ? "Debug" : "Release"                         // Configuration.
                                );
                            }
                            else
                            {
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|MSBuild-Plaform
                                PlatfromConfig = string.Format("\t\t{0}.{1}|{2}.* = {3}{4}|{5}",
                                    /* 0 */ SolutionProject.AdditionalCache.GUID,                                   // GUID
                                    /*1,2*/ ConfigurationInfo.HumanReadableName, PlatformInfo.HumanReadableName,    // Human readable names
                                    /*3,4*/ Configuration, Platform,                                                // Internal names
                                    /* 5 */ ConvertPlatformToMsBuildPlatform(Platform)                              // MSBuild platform
                                );
                            }

                            Sln.WriteLine(PlatfromConfig.Replace("*", "ActiveCfg"));
                            Sln.WriteLine(PlatfromConfig.Replace("*", "Build.0"));
                            //SLN.WriteLine(PlatfromConfig.Replace("*", "Deploy.0"));   // @todo Do something with deployment..
                        }
                    }
                }
                Sln.WriteLine("\tEndGlobalSection");

                Sln.WriteLine("\tGlobalSection(SolutionProperties) = postSolution");
                Sln.WriteLine("\t\tHideSolutionNode = FALSE");
                Sln.WriteLine("\tEndGlobalSection");

                //[
                // Matching projects and filters hierarchy.
                //[
                Sln.WriteLine("\tGlobalSection(NestedProjects) = preSolution");
                foreach (var SolutionProject in Solution.CachedProjects.Where(SolutionProject => !string.IsNullOrEmpty(SolutionProject.CachedFilter)))
                {
                    Sln.WriteLine("\t\t{0} = {1}", SolutionProject.AdditionalCache.GUID, SolutionProject.AdditionalCache.FilterGUID);
                }
                Sln.WriteLine("\tEndGlobalSection");
                Sln.WriteLine("EndGlobal");
            }

            return SlnPath;
        }
    }   // class VisualStudioProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator.VisualStudio
