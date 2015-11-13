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
    // ------------------------------------------------------------------------------------------
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

        // ------------------------------------------------------------------------------------------
        //! Returns formatted GUID.
        //! @returns Formatted GUID.
        private static string CreateMsBuildGuid()
        {
            return $"{{{Guid.NewGuid().ToString()}}}".ToUpperInvariant();
        }

        // ------------------------------------------------------------------------------------------
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

        // ------------------------------------------------------------------------------------------
        //! Converts target platform into MSBuild compatible one.
        //! @param platform Some platform.
        //! @returns MSBuild compatible one.
        private static string ConvertPlatformToMsBuildPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.Android:
                    return "ARM";

                case TargetPlatform.iOS:
                case TargetPlatform.Emscripten:
                    //! @todo Fix correct toolset selection.
                    //return "Win32";

                case TargetPlatform.WindowsUap:
                case TargetPlatform.WindowsPhone:
                    //! @todo Fix correct toolset selection.
                    //return "ARM";

                case TargetPlatform.Windows:
                case TargetPlatform.OsX:
                case TargetPlatform.Linux:
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return "x64";

                default:
                    throw new NotImplementedException();
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Loads a GUID from an existing project file.
        //! @param projectPath Path to project file.
        //! @returns A loaded GUID on success or new generated GUID on failure.
        private static string QueryGuidFromProject(string projectPath)
        {
            var proj = new XmlDocument();
            var projNamespaceMgr = new XmlNamespaceManager(proj.NameTable);
            projNamespaceMgr.AddNamespace("msb", MsBuild2003Namespace);
            proj.Load(projectPath);

            var projGuidNode = proj.SelectSingleNode("//msb:PropertyGroup/msb:ProjectGuid", projNamespaceMgr);
            return projGuidNode?.InnerText;
        }

        // ------------------------------------------------------------------------------------------
        //! Checks if specified platform is natively supported by VS.
        //! @param platform Some platform.
        //! @returns True if platform is natively supported by VS, false otherwise.
        public override bool IsPlatformNativelySupported(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.Windows:
                case TargetPlatform.WindowsUap:
                case TargetPlatform.WindowsPhone:
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                case TargetPlatform.Android:
                    return true;
            }

            return false;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates project files for Visual Studio: .vcxproj and .vcxproj.filter.
        //! @param project Parsed project object.
        //! @returns Path to Visual Studio's .vcxproj file.
        public sealed override string GenerateProjectFiles(ProjectCache project)
        {
            var vcxProjPath = Path.Combine(base.GenerateProjectFiles(project), project.CachedName) + ".vcxproj";

            // For build tools the best was is to load a GUID from project files: this causes exceptions 
            // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
            try
            {
                project.AdditionalCache.GUID = QueryGuidFromProject(vcxProjPath);
            }
            catch (Exception)
            {
                project.AdditionalCache.GUID = CreateMsBuildGuid();
            }

            // ==========================================================================================
            // Generating VCXPROJ files.
            // ==========================================================================================
            using (var vcxProj = new XmlTextWriter(vcxProjPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                vcxProj.WriteStartDocument();
                vcxProj.WriteComment("Generated by GoddamnBuildSystem. Please, do not edit this file manually.");
                vcxProj./**/WriteStartElement("Project", MsBuild2003Namespace);
                vcxProj./**//**/WriteAttributeString("ToolsVersion", ToolsVersion);
                vcxProj./**//**/WriteAttributeString("DefaultTargets", "Build");

                // ------------------------------------------------------------------------------------------
                // Defining list of configurations.
                // ------------------------------------------------------------------------------------------
                vcxProj.WriteStartElement("ItemGroup");
                vcxProj./**/WriteAttributeString("Label", "ProjectConfigurations");
                foreach (var platform in Target.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    var platform1 = platform;   // Access to foreach variable in closure shit..
                    foreach (var configurationName in Target.EnumerateAllConfigurations().Select(configuration => string.Concat(configuration, platform1)))
                    {
                        vcxProj./**/WriteStartElement("ProjectConfiguration");
                        vcxProj./**//**/WriteAttributeString("Include", configurationName + '|' + platformString);
                        vcxProj./**//**/WriteElementString("Configuration", configurationName);
                        vcxProj./**//**/WriteElementString("Platform", platformString);
                        vcxProj./**/WriteEndElement();
                    }
                }
                vcxProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Defining list of source files.
                // ------------------------------------------------------------------------------------------
                vcxProj.WriteStartElement("ItemGroup");
                foreach (var projectSource in project.CachedSourceFiles)
                {
                    vcxProj./**/WriteStartElement(ConvertFileTypeToVcxProjElement(projectSource.FileType));
                    vcxProj./**//**/WriteAttributeString("Include", projectSource.FileName);
                    foreach (var platform in Target.EnumerateAllPlatforms())
                    {
                        var platformString = ConvertPlatformToMsBuildPlatform(platform);
                        foreach (var configuration in Target.EnumerateAllConfigurations())
                        {
                            if (projectSource.ShouldBeExcluded(platform, configuration))
                            {
                                var configurationName = string.Concat(configuration, platform);
                                vcxProj./**//**/WriteStartElement("ExcludedFromBuild");
                                vcxProj./**//**//**/WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                                vcxProj./**//**//**/WriteString("true");
                                vcxProj./**//**/WriteEndElement();
                            }
                        }
                    }
                    vcxProj./**/WriteEndElement();
                }
                vcxProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding global project properties.
                // ------------------------------------------------------------------------------------------
                vcxProj.WriteStartElement("PropertyGroup");
                vcxProj./**/WriteElementString("ProjectGuid", project.AdditionalCache.GUID);
                vcxProj./**/WriteElementString("RootNamespace", project.CachedName);
                vcxProj./**/WriteElementString("MinimumVisualStudioVersion", "14.0");
                vcxProj.WriteEndElement();
                foreach (var platform in Target.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    foreach (var configuration in Target.EnumerateAllConfigurations())
                    {
                        var configurationName = string.Concat(configuration, platform);
                        vcxProj.WriteStartElement("PropertyGroup");
                        vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                        vcxProj./**/WriteAttributeString("Label", "Globals");
                        if (Target.IsWinApiPlatform(platform))
                        {   
                            vcxProj./**/WriteElementString("WindowsTargetPlatformVersion", "10.0.10240.0");
                            vcxProj./**/WriteElementString("WindowsTargetPlatformMinVersion", "10.0.10240.0");
                            if (platform == TargetPlatform.WindowsUap || platform == TargetPlatform.WindowsPhone)
                            {
                                vcxProj./**/WriteElementString("ApplicationType", "Windows Store");
                                vcxProj./**/WriteElementString("ApplicationTypeRevision", "10");
                            }
                        }
                        else if (platform == TargetPlatform.Android)
                        {
                            vcxProj./**/WriteElementString("Keyword", "Android");
                            vcxProj./**/WriteElementString("ApplicationType", "Android");
                            vcxProj./**/WriteElementString("ApplicationTypeRevision", "1.0");
                        }
                        vcxProj.WriteEndElement();
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Overriding main configuration properties.
                // ------------------------------------------------------------------------------------------
                foreach (var platform in Target.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    string platformToolset;
                    switch (platform)
                    {
                        case TargetPlatform.Windows:
                        case TargetPlatform.WindowsUap:
                        //  platformToolset = "CTP_Nov2013";
                            platformToolset = "v140";
                            break;

                         case TargetPlatform.Android:
                            platformToolset = "Clang_3_6";
                            break;
                        case TargetPlatform.WindowsPhone:
                            platformToolset = "v120_wp81";
                            break;

                        // These are not supported, setting up some generic toolset.
                        default:
                            platformToolset = "v140";
                            break;
                    }

                    foreach (var configuration in Target.EnumerateAllConfigurations())
                    {
                        var configurationName = string.Concat(configuration, platform);
                        vcxProj.WriteStartElement("PropertyGroup");
                        vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                        vcxProj./**/WriteAttributeString("Label", "Configuration");
                        vcxProj./**/WriteElementString("PlatformToolset", platformToolset);
                       
                        if (IsPlatformNativelySupported(platform))
                        {
                            vcxProj./**/WriteElementString("ConfigurationType", project.CachedBuildTypes[platform, configuration].ToString());
                            vcxProj./**/WriteElementString("CharacterSet", "Unicode");  // We need Unicode for the Windows store applications.
                            //if (TargetPlatformInfo.Get(platform).RequiresRtti)
                            //{
                            //    vcxProj./**/WriteElementString("CLRSupport", "true");
                            //}
                        }
                        else
                        {
                            vcxProj./**/WriteElementString("ConfigurationType", "Makefile");
                        }
                        vcxProj.WriteEndElement();
                    }
                }

                vcxProj.WriteStartElement("Import");
                vcxProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
                vcxProj.WriteEndElement();
                vcxProj.WriteStartElement("Import");
                vcxProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
                vcxProj.WriteEndElement();
                vcxProj.WriteStartElement("PropertyGroup");
                vcxProj./**/WriteAttributeString("Label", "UserMacros");
                vcxProj.WriteEndElement();
                vcxProj.WriteStartElement("PropertyGroup");
                vcxProj./**/WriteElementString("NMakeForcedIncludes", "$(NMakeForcedIncludes)");
                vcxProj./**/WriteElementString("NMakeAssemblySearchPath", "$(NMakeAssemblySearchPath)");
                vcxProj./**/WriteElementString("NMakeForcedUsingAssemblies", "$(NMakeForcedUsingAssemblies)");
                vcxProj.WriteEndElement();

                // ------------------------------------------------------------------------------------------
                // Overriding VC++ compiler and linker properties OR NMake commands..
                // ------------------------------------------------------------------------------------------
                foreach (var platform in Target.EnumerateAllPlatforms())
                {
                    var platformInfo = TargetPlatformInfo.Get(platform);
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);

                    // Building list of header directories.
                    var includePathes = project.GenerateIncludePaths();
                    var standartLibrariesPaths = platformInfo.GenerateStandartIncludePaths();
                    if (string.IsNullOrEmpty(standartLibrariesPaths))
                    {
                        standartLibrariesPaths = "$(NMakeIncludeSearchPath)";
                    }

                    foreach (var configuration in Target.EnumerateAllConfigurations())
                    {
                        var configurationInfo = TargetConfigurationInfo.Get(configuration);

                        // Defining macros..
                        var macros = project.GenerateMacros(platform, configuration);
                        var standartMacros = platformInfo.GenerateStandartMacrosList();
                        if (string.IsNullOrEmpty(standartMacros))
                        {
                            standartMacros = "$(NMakePreprocessorDefinitions)";
                        }

                        // Collecting linked libraries..
                        var linkedLibraries = project.GenerateLinkedLibrariesPaths(platform, configuration);
                        var standartLinkedLibraries = platformInfo.GenerateStandartLinkedLibrariesPaths();
                        if (string.IsNullOrEmpty(standartLinkedLibraries))
                        {
                            standartLinkedLibraries = "";
                        }
                        standartLinkedLibraries += "%(AdditionalDependencies)";

                        var configurationName = string.Concat(configuration, platform);
                        var outputPath = project.CachedOutputPaths[platform, configuration];

                        vcxProj.WriteStartElement("ImportGroup");
                        vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                        vcxProj./**/WriteAttributeString("Label", "PropertySheets");
                        vcxProj./**/WriteStartElement("Import");
                        vcxProj./**//**/WriteAttributeString("Project", @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                        vcxProj./**//**/WriteAttributeString("Condition", @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                        vcxProj./**//**/WriteAttributeString("Label", "LocalAppDataPlatform");
                        vcxProj./**/WriteEndElement();
                        vcxProj.WriteEndElement();

                        if (IsPlatformNativelySupported(platform))
                        {
                            // ------------------------------------------------------------------------------------------
                            // Defining VC++ compiler and linker properties.
                            // ------------------------------------------------------------------------------------------

                            // Include paths..
                            vcxProj.WriteStartElement("PropertyGroup");
                            vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                            vcxProj./**/WriteElementString("IncludePath", includePathes + "$(IncludePath)");
                            vcxProj./**/WriteElementString("OutDir", Path.GetDirectoryName(outputPath) + Path.DirectorySeparatorChar);
                            vcxProj./**/WriteElementString("TargetName", Path.GetFileNameWithoutExtension(outputPath));
                            vcxProj.WriteEndElement();

                            // Compiler properties..
                            vcxProj.WriteStartElement("ItemDefinitionGroup");
                            vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                            vcxProj./**/WriteStartElement("ClCompile");
                            vcxProj./**//**/WriteElementString("FavorSizeOrSpeed", "Speed");
                            vcxProj./**//**/WriteElementString("PrecompiledHeader", "NotUsing");
                            vcxProj./**//**/WriteElementString("CallingConvention", "FastCall");
                            vcxProj./**//**/WriteElementString("WarningLevel", "Level" + TargetCppCompilerInformation.WarningLevel);
                            vcxProj./**//**/WriteElementString("TreatWarningAsError", TargetCppCompilerInformation.WarningsAreErrors.ToString());
                            vcxProj./**//**/WriteElementString("PreprocessorDefinitions", macros + "%(PreprocessorDefinition)");
                            vcxProj./**//**/WriteElementString("Optimization", configurationInfo.Optimize ? "Full" : "Disabled");
                            if (configurationInfo.IsDebug)
                            {
                                vcxProj.WriteElementString("RuntimeLibrary", "MultiThreadedDebugDLL");
                            }
                            if (platform != TargetPlatform.WindowsUap && (platform != TargetPlatform.WindowsPhone))
                            {
                                // We do need to generate this parameter for UAP applications. Compiler throws
                                // 'inconsistent parameters "/clr" and "/Gr"'
                                if (!platformInfo.RequiresRtti) 
                                {
                                    vcxProj.WriteElementString("RuntimeTypeInfo", "false");
                                }
                                if (!platformInfo.RequiresExceptions)
                                {
                                    vcxProj.WriteElementString("ExceptionHandling", "false");
                                }
                            }
                            vcxProj./**/WriteEndElement(); // </ VC++ COMPILER PROPERTIES

                            // Linker properties.
                            vcxProj./**/WriteStartElement("Link");
                            vcxProj./**//**/WriteElementString("AdditionalDependencies", linkedLibraries + standartLinkedLibraries);
                            vcxProj./**//**/WriteElementString("GenerateDebugInformation", configurationInfo.GenerateDebugInformation.ToString().ToLowerInvariant());
                            if (project.CachedBuildTypes[platform, configuration] == ProjectBuildType.DynamicLibrary)
                            {
                                vcxProj./**//**/WriteElementString("ImportLibrary", project.CachedImportLibraryOutputPaths[platform, configuration]);
                            }
                            vcxProj./**/WriteEndElement();
                            vcxProj.WriteEndElement();

                        }
                        else
                        {

                            // ------------------------------------------------------------------------------------------
                            // Defining NMake properties..
                            // ------------------------------------------------------------------------------------------

                            var nmakeCommand =
                                $"\"{System.Reflection.Assembly.GetExecutingAssembly().Location}\" --compile-project \"{project.CachedSource}\" {platform} {configuration} ";
                            vcxProj.WriteStartElement("PropertyGroup");
                            vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                            vcxProj./**/WriteElementString("OutDir", Path.GetDirectoryName(outputPath) + Path.DirectorySeparatorChar);
                            vcxProj./**/WriteElementString("NMakeOutput", outputPath);
                            vcxProj./**/WriteElementString("NMakePreprocessorDefinitions", macros + standartMacros + ";GD_PLATFORNOT_WINDOWS=1");
                            vcxProj./**/WriteElementString("NMakeIncludeSearchPath", includePathes + standartLibrariesPaths);
                            vcxProj./**/WriteElementString("NMakeBuildCommandLine", nmakeCommand);
                            vcxProj./**/WriteElementString("NMakeReBuildCommandLine", nmakeCommand + "Rebuild");
                            vcxProj./**/WriteElementString("NMakeCleanCommandLine", nmakeCommand + "Clean");
                            vcxProj.WriteEndElement();
                        }
                    }
                }

                vcxProj.WriteStartElement("Import");
                vcxProj./**/WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets");
                vcxProj.WriteEndElement();

                vcxProj./**/WriteEndElement();
                vcxProj.WriteEndDocument();
            }

            // ==========================================================================================
            // Generating VCPROJ.FILTERS files.
            // ==========================================================================================
            var vcxProjFiltersPath = vcxProjPath + ".filters";
            using (var vcxProjFilters = new XmlTextWriter(vcxProjFiltersPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                vcxProjFilters.WriteStartDocument();
                vcxProjFilters./**/WriteStartElement("Project", MsBuild2003Namespace);
                vcxProjFilters./**/WriteAttributeString("ToolsVersion", ToolsFiltersVersion);

                var projectFoldersCache = new HashSet<string>();
                foreach (var projectSourceFile in project.CachedSourceFiles)
                {
                    var projectSourceFileDirectory = Path.GetDirectoryName(projectSourceFile.FileName);
                    string projectSourceFilter;

                    // Checking if this source can have a filter: it is located in project directory and it not in it's m_RootNode.
                    // We also add "Source" to each filter to separate config and source files.  
                    if (projectSourceFileDirectory != null && (projectSourceFileDirectory.StartsWith(project.CachedSourcesFilterOrigin, StringComparison.InvariantCultureIgnoreCase)
                                                           && (projectSourceFileDirectory.Length > (project.CachedSourcesFilterOrigin.Length + 1))))
                    {
                        projectSourceFilter = "Source\\" + projectSourceFileDirectory.Substring(project.CachedSourcesFilterOrigin.Length + 1);
                        var projectSourceFilterIter = projectSourceFilter;
                        while ((!projectFoldersCache.Contains(projectSourceFilterIter)) && (!string.IsNullOrEmpty(projectSourceFilterIter)))
                        {
                            // ------------------------------------------------------------------------------------------
                            // Defining group of filters.
                            // ------------------------------------------------------------------------------------------
                            vcxProjFilters.WriteStartElement("ItemGroup");
                            vcxProjFilters./**/WriteStartElement("Filter");
                            vcxProjFilters./**//**/WriteAttributeString("Include", projectSourceFilterIter);
                            vcxProjFilters./**//**/WriteElementString("UniqueIdentifier", CreateMsBuildGuid());
                            vcxProjFilters./**/WriteEndElement();
                            vcxProjFilters.WriteEndElement();

                            projectFoldersCache.Add(projectSourceFilterIter);
                            projectSourceFilterIter = Path.GetDirectoryName(projectSourceFilterIter);
                        }
                    }
                    else
                    {
                        projectSourceFilter = "Source";
                    }

                    // ------------------------------------------------------------------------------------------
                    // Defining source's filters.
                    // ------------------------------------------------------------------------------------------
                    vcxProjFilters.WriteStartElement("ItemGroup");
                    vcxProjFilters./**/WriteStartElement(ConvertFileTypeToVcxProjElement(projectSourceFile.FileType));
                    vcxProjFilters./**//**/WriteAttributeString("Include", projectSourceFile.FileName);
                    vcxProjFilters./**//**/WriteElementString("Filter", projectSourceFilter);
                    vcxProjFilters./**/WriteEndElement();
                    vcxProjFilters.WriteEndElement();
                }

                vcxProjFilters./**/WriteEndElement();
                vcxProjFilters.WriteEndDocument();
            }

            return vcxProjPath;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates solution files for Visual Studio.
        //! @param solution Parsed solution object.
        //! @returns Path to Visual Studio's .sln file.
        public sealed override string GenerateSolutionFiles(SolutionCache solution)
        {
            // ==========================================================================================
            // Generating SLN files..
            // ==========================================================================================
            var slnPath = Path.Combine(base.GenerateSolutionFiles(solution), solution.CachedName + ".sln");
            using (var sln = new StreamWriter(slnPath))
            {
                sln.WriteLine();
                sln.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
                sln.WriteLine("# Generated by GoddamnBuildSystem. Please, do not edit this file manually.");

                // ------------------------------------------------------------------------------------------
                // Caching GUIDs of projects and filters and defining filters.
                // ------------------------------------------------------------------------------------------
                var solutionFiltersGuidCache = new Dictionary<int, string>();
                foreach (var solutionProject in solution.CachedProjects)
                {
                    // Loading the GUID of a build tool..
                    if (solutionProject.IsBuildTool)
                    {
                        // For build tools the best was is to load a GUID from project files: this causes exceptions 
                        // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
                        try
                        {
                            solutionProject.AdditionalCache.GUID = QueryGuidFromProject(solutionProject.GeneratorOutputPath);
                        }
                        catch (Exception exception)
                        {
                            Console.Error.WriteLine("Unable to load a GUID of C# project \"{0}\". Check this out...", solution.GeneratedSolutionPath);
                            Console.Error.WriteLine(exception.ToString());

                            solutionProject.AdditionalCache.GUID = CreateMsBuildGuid();
                        }
                    }

                    // Looking for projects with filters which we have not already cached.
                    if (!string.IsNullOrEmpty(solutionProject.CachedFilter))
                    {
                        var solutionProjectFilterHash = solutionProject.CachedFilter.GetHashCode();
                        if (!solutionFiltersGuidCache.ContainsKey(solutionProjectFilterHash))
                        {
                            // Generating and caching filter GUIDS.
                            solutionProject.AdditionalCache.FilterGUID = CreateMsBuildGuid();
                            solutionFiltersGuidCache.Add(solutionProjectFilterHash, solutionProject.AdditionalCache.FilterGUID);

                            // E.g. 'Project({Filter-GUID}) = "Name", "Name", "Filter-GUID"'
                            sln.WriteLine("Project(\"{0}\") = \"{1}\", \"{1}\", \"{2}\"", SlnFilterProjectGuid, solutionProject.CachedFilter, solutionProject.AdditionalCache.FilterGUID);
                            sln.WriteLine("EndProject");
                        }
                        else
                        {
                            solutionProject.AdditionalCache.FilterGUID = solutionFiltersGuidCache[solutionProjectFilterHash];
                        }
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Defining Solution projects.
                // ------------------------------------------------------------------------------------------
                foreach (var solutionProject in solution.CachedProjects)
                {
                    // E.g. 'Project({Type-GUID}) = "Name", "Path-ToProject-File", "Project-GUID"'
                    sln.WriteLine("Project(\"{0}\") = \"{1}\", \"{2}\", \"{3}\"",
                        /* 0 */ solutionProject.IsBuildTool ? SlnCsProjProjectGuid : SlnVcxProjProjectGuid,
                        /*1,2*/ solutionProject.CachedName, solutionProject.GeneratorOutputPath,
                        /* 3 */ solutionProject.AdditionalCache.GUID);
                    // Defining projects this one depends.
                    sln.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                    var project = solutionProject;
                    foreach (var solutionDependentProject in solution.CachedProjects
                        .Where(solutionDependentProject => solutionDependentProject.CachedPriority > project.CachedPriority))
                    {
                        // Writing projects with higher priority as ones we depend from.
                        sln.WriteLine("\t\t{0} = {0}", solutionDependentProject.AdditionalCache.GUID);
                    }
                    sln.WriteLine("\tEndProjectSection");
                    sln.WriteLine("EndProject");
                }

                sln.WriteLine("Global");

                // ------------------------------------------------------------------------------------------
                // Linking platforms & configurations.
                // ------------------------------------------------------------------------------------------
                sln.WriteLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
                foreach (var platform in Target.EnumerateAllPlatforms())
                {
                    var platformInfo = TargetPlatformInfo.Get(platform);
                    foreach (var configurationInfo in Target.EnumerateAllConfigurations().Select(TargetConfigurationInfo.Get))
                    {
                        // E.g. \tConfiguration-Name|Platform-Name = Configuration-Name|Platform-Name
                        sln.WriteLine("\t\t{0}|{1} = {0}|{1}", configurationInfo.HumanReadableName, platformInfo.HumanReadableName);
                    }
                }
                sln.WriteLine("\tEndGlobalSection");

                sln.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
                foreach (var solutionProject in solution.CachedProjects)
                {
                    foreach (var platform in Target.EnumerateAllPlatforms())
                    {
                        var platformInfo = TargetPlatformInfo.Get(platform);
                        foreach (var configuration in Target.EnumerateAllConfigurations())
                        {
                            string platfromConfig;
                            var configurationInfo = TargetConfigurationInfo.Get(configuration);
                            if (solutionProject.IsBuildTool)
                            {
                                // C# projects have only Debug/Release configurations. 
                                // If we specify others - projects would not be enabled in the configuration dispatcher.
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|Any CPU
                                platfromConfig = $"\t\t{solutionProject.AdditionalCache.GUID}.{configurationInfo.HumanReadableName}|{platformInfo.HumanReadableName}.* = {(configurationInfo.IsDebug ? "Debug" : "Release")}|Any CPU";
                            }
                            else
                            {
                                // E.g. \t{Project-GUID}.Configuration-Name|Platform-Name.* = Configuration|MSBuild-Platform
                                platfromConfig = $"\t\t{solutionProject.AdditionalCache.GUID}.{configurationInfo.HumanReadableName}|{platformInfo.HumanReadableName}.* = {configuration}{platform}|{ConvertPlatformToMsBuildPlatform(platform)}";
                            }

                            sln.WriteLine(platfromConfig.Replace("*", "ActiveCfg"));
                            sln.WriteLine(platfromConfig.Replace("*", "Build.0"));
                            //SLN.WriteLine(PlatfromConfig.Replace("*", "Deploy.0"));   // @todo Do something with deployment..
                        }
                    }
                }
                sln.WriteLine("\tEndGlobalSection");

                sln.WriteLine("\tGlobalSection(SolutionProperties) = postSolution");
                sln.WriteLine("\t\tHideSolutionNode = FALSE");
                sln.WriteLine("\tEndGlobalSection");

                // ------------------------------------------------------------------------------------------
                // Matching projects and filters hierarchy.
                // ------------------------------------------------------------------------------------------
                sln.WriteLine("\tGlobalSection(NestedProjects) = preSolution");
                foreach (var solutionProject in solution.CachedProjects
                    .Where(solutionProject => !string.IsNullOrEmpty(solutionProject.CachedFilter)))
                {
                    sln.WriteLine("\t\t{0} = {1}", solutionProject.AdditionalCache.GUID, solutionProject.AdditionalCache.FilterGUID);
                }
                sln.WriteLine("\tEndGlobalSection");
                sln.WriteLine("EndGlobal");
            }

            return slnPath;
        }
    }   // class VisualStudioProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator.VisualStudio
