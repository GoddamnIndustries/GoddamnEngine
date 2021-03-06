﻿// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using GoddamnEngine.BuildSystem.Collectors;
using GoddamnEngine.BuildSystem.Support;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    /// <summary>
    /// Generator of the project/solution files for Visual Studio 2017.
    /// </summary>
    /// <inheritdoc />
    internal class VisualStudioProjectGenerator : ProjectGenerator
    {
        private const string c_MsBuild2003Namespace  = @"http://schemas.microsoft.com/developer/msbuild/2003";
        private const string c_SlnVcxProjProjectGuid = @"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
        private const string c_SlnCsProjProjectGuid  = @"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
        private const string c_SlnFilterProjectGuid  = @"{2150E333-8FDC-42A3-9474-1A3956D46DE8}";

        private const string c_ToolsVersion          = "15.0";
        private const string c_ToolsFiltersVersion   = "4.0";

        private static string CreateMsBuildGuid()
        {
            return $"{{{Guid.NewGuid()}}}".ToUpperInvariant();
        }

        private static string ConvertFileTypeToVcxProjElement(ProjectSourceFileType fileType)
        {
            switch (fileType)
            {
                case ProjectSourceFileType.SourceCode:
                    return "ClCompile";
                case ProjectSourceFileType.SourceCodeAssembler:
                    return "MASM";
                case ProjectSourceFileType.SourceCodeObjective:
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

        private static string ConvertPlatformToMsBuildPlatform(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.Windows:
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return "x64";

                default:
                    throw new NotImplementedException();
            }
        }

        private static string QueryGuidFromProject(string projectPath)
        {
            var proj = new XmlDocument();
            var projNamespaceMgr = new XmlNamespaceManager(proj.NameTable);
            projNamespaceMgr.AddNamespace("msb", c_MsBuild2003Namespace);
            proj.Load(projectPath);

            var projGuidNode = proj.SelectSingleNode("//msb:PropertyGroup/msb:ProjectGuid", projNamespaceMgr);
            return projGuidNode?.InnerText;
        }

        /// <summary>
        /// Checks if specified platform is natively supported by VS.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if platform is natively supported by VS, false otherwise.</returns>
        public override bool IsPlatformNativelySupported(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.Windows:
                case TargetPlatform.XboxOne:
                case TargetPlatform.PlayStation4:
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Generates project files for Visual Studio: '.vcxproj' and '.vcxproj.filter'.
        /// </summary>
        /// <param name="project">Parsed project object.</param>
        /// <returns>Path to Visual Studio '.vcxproj file'.</returns>
        public sealed override string GenerateProjectFiles(Project project)
        {
            var vcxProjPath = Path.Combine(base.GenerateProjectFiles(project), project.Name) + ".vcxproj";

            try
            {
                // For build tools the best was is to load a GUID from project files: this causes exceptions 
                // in Visual Studio when reloading of solution, and all opened tabs would be terminated.
                project.Misc.GUID = QueryGuidFromProject(vcxProjPath);
            }
            catch (Exception)
            {
                project.Misc.GUID = CreateMsBuildGuid();
            }

            // ==========================================================================================
            // Generating VCXPROJ files.
            // ==========================================================================================
            using (var vcxProj = new XmlTextWriter(vcxProjPath, null) { Formatting = Formatting.Indented, Indentation = 1, IndentChar = '\t' })
            {
                vcxProj.WriteStartDocument();
                vcxProj.WriteComment("Generated by GoddamnBuildSystem. Please, do not edit this file manually.");
                vcxProj./**/WriteStartElement("Project", c_MsBuild2003Namespace);
                vcxProj./**//**/WriteAttributeString("ToolsVersion", c_ToolsVersion);
                vcxProj./**//**/WriteAttributeString("DefaultTargets", "Build");

                // ------------------------------------------------------------------------------------------
                // Defining list of configurations.
                // ------------------------------------------------------------------------------------------
                vcxProj.WriteStartElement("ItemGroup");
                vcxProj./**/WriteAttributeString("Label", "ProjectConfigurations");
                foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                    {
                        var configurationName = string.Concat(configuration, platform);
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
                foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                    {
                        var configurationName = string.Concat(configuration, platform);
                        vcxProj.WriteStartElement("ItemGroup");
                        vcxProj.WriteAttributeStringFormat("Condition", @"'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                        foreach (var projectSource in project.Files[platform, configuration])
                        {
                            vcxProj.WriteStartElement(ConvertFileTypeToVcxProjElement(projectSource.FileType));
                            vcxProj.WriteAttributeString("Include", projectSource.FilePath);
                            vcxProj.WriteEndElement();
                        }
                        vcxProj.WriteEndElement();
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Overriding global project properties.
                // ------------------------------------------------------------------------------------------
                vcxProj.WriteStartElement("PropertyGroup");
                vcxProj./**/WriteElementString("ProjectGuid", project.Misc.GUID);
                vcxProj./**/WriteElementString("RootNamespace", project.Name);
                vcxProj./**/WriteElementString("MinimumVisualStudioVersion", "14.0");
                vcxProj.WriteEndElement();
                foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                    {
                        var configurationName = string.Concat(configuration, platform);
                        vcxProj.WriteStartElement("PropertyGroup");
                        vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                        vcxProj./**/WriteAttributeString("Label", "Globals");
                        vcxProj./**/WriteElementString("WindowsTargetPlatformVersion", "10.0.16299.0");
                        if (platform == TargetPlatform.XboxOne)
                        {
                            vcxProj./**/WriteElementString("AppContainerApplication", "true");
                            vcxProj./**/WriteElementString("ApplicationType", "Windows Store");
                            vcxProj./**/WriteElementString("WindowsTargetPlatformVersion", "10.0.16299.0");
                            vcxProj./**/WriteElementString("WindowsTargetPlatformMinVersion", "10.0.10586.0");
                            vcxProj./**/WriteElementString("ApplicationTypeRevision", "10");
                        }
                        vcxProj.WriteEndElement();
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Overriding main configuration properties.
                // ------------------------------------------------------------------------------------------
                foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                {
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);
                    var platformToolset = "v141";

                    foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                    {
                        var configurationName = string.Concat(configuration, platform);
                        vcxProj.WriteStartElement("PropertyGroup");
                        vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                        vcxProj./**/WriteAttributeString("Label", "Configuration");
                        vcxProj./**/WriteElementString("PlatformToolset", platformToolset);
                        if (IsPlatformNativelySupported(platform))
                        {
                            vcxProj./**/WriteElementString("ConfigurationType", project.BuildType[platform, configuration].ToString());
                            vcxProj./**/WriteElementString("CharacterSet", "Unicode");
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
                vcxProj.WriteStartElement("ImportGroup");
                vcxProj./**/WriteAttributeString("Label", "ExtensionSettings");
                vcxProj./**/WriteStartElement("Import");
                vcxProj./**//**/WriteAttributeString("Project", @"$(VCTargetsPath)\BuildCustomizations\masm.props");
                vcxProj./**/WriteEndElement();
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
                foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                {
                    var platformInfo = TargetPlatformInfo.Get(platform);
                    var platformString = ConvertPlatformToMsBuildPlatform(platform);

                    // Building list of header directories.
                    var includePathes = project.GenerateIncludePaths(platform, TargetConfiguration.Debug);
                    var standardLibrariesPaths = platformInfo.GenerateStandardIncludePaths();
                    if (string.IsNullOrEmpty(standardLibrariesPaths))
                    {
                        standardLibrariesPaths = "$(NMakeIncludeSearchPath)";
                    }

                    foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                    {
                        var configurationInfo = TargetConfigurationInfo.Get(configuration);

                        // Defining macros..
                        //var macros = project.GenerateMacros(platform, configuration);
                        //var standardMacros = platformInfo.GenerateStandardMacrosList();
                        //if (string.IsNullOrEmpty(standardMacros))
                        //{
                        //    standardMacros = "$(NMakePreprocessorDefinitions)";
                        //}

                        // Collecting linked libraries..
                        var linkedLibraries = project.GenerateLinkedLibrariesPaths(platform, configuration);
                        var standardLinkedLibraries = platformInfo.GenerateStandardLinkedLibrariesPaths();
                        if (string.IsNullOrEmpty(standardLinkedLibraries))
                        {
                            standardLinkedLibraries = "";
                        }
                        standardLinkedLibraries += "%(AdditionalDependencies)";

                        var configurationName = string.Concat(configuration, platform);
                        var outputPath = project.OutputPath[platform, configuration];

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
                            //vcxProj./**//**/WriteElementString("PreprocessorDefinitions", macros + "%(PreprocessorDefinition)");
                            vcxProj./**//**/WriteElementString("Optimization", configurationInfo.Optimize ? "Full" : "Disabled");
                            if (configurationInfo.IsDebug)
                            {
                                vcxProj.WriteElementString("RuntimeLibrary", "MultiThreadedDebugDLL");
                            }
                            if (!platformInfo.RequiresRtti) 
                            {
                                vcxProj.WriteElementString("RuntimeTypeInfo", "false");
                            }
                            if (!platformInfo.RequiresExceptions)
                            {
                                //vcxProj.WriteElementString("ExceptionHandling", "false");
                            }
                            vcxProj./**/WriteEndElement(); // </ VC++ COMPILER PROPERTIES

                            // Linker properties.
                            vcxProj./**/WriteStartElement("Link");
                            vcxProj./**//**/WriteElementString("AdditionalDependencies", linkedLibraries + standardLinkedLibraries);
                            vcxProj./**//**/WriteElementString("GenerateDebugInformation", configurationInfo.GenerateDebugInformation.ToString().ToLowerInvariant());
                            if (project.BuildType[platform, configuration] == ProjectBuildType.DynamicLibrary)
                            {
                                vcxProj./**//**/WriteElementString("ImportLibrary", project.ImportLibraryOutputPath[platform, configuration]);
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
                                $"\"{System.Reflection.Assembly.GetExecutingAssembly().Location}\" --compile-project \"{project.Source}\" {platform} {configuration} ";
                            vcxProj.WriteStartElement("PropertyGroup");
                            vcxProj./**/WriteAttributeStringFormat("Condition", "'$(Configuration)|$(Platform)'=='{0}|{1}'", configurationName, platformString);
                            vcxProj./**/WriteElementString("OutDir", Path.GetDirectoryName(outputPath) + Path.DirectorySeparatorChar);
                            vcxProj./**/WriteElementString("NMakeOutput", outputPath);
                            //vcxProj./**/WriteElementString("NMakePreprocessorDefinitions", macros + standardMacros + ";GD_PLATFORNOT_WINDOWS=1");
                            vcxProj./**/WriteElementString("NMakeIncludeSearchPath", includePathes + standardLibrariesPaths);
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
                vcxProj.WriteStartElement("ImportGroup");
                vcxProj./**/WriteAttributeString("Label", "ExtensionSettings");
                vcxProj./**/WriteStartElement("Import");
                vcxProj./**//**/WriteAttributeString("Project", @"$(VCTargetsPath)\BuildCustomizations\masm.targets");
                vcxProj./**/WriteEndElement();
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
                vcxProjFilters./**/WriteStartElement("Project", c_MsBuild2003Namespace);
                vcxProjFilters./**/WriteAttributeString("ToolsVersion", c_ToolsFiltersVersion);

                var projectFoldersCache = new HashSet<string>();
                foreach (var projectSourceFile in project.Files[TargetPlatform.Any, TargetConfiguration.Any])
                {
                    var projectSourceFileDirectory = Path.GetDirectoryName(projectSourceFile.FilePath);
                    string projectSourceFilter;

                    // Checking if this source can have a filter: it is located in project directory and it not in it's root.
                    // We also add "Source" to each filter to separate configuration and source files.  
                    if (projectSourceFileDirectory != null && projectSourceFileDirectory.StartsWith(project.SourcesFilterOrigin, StringComparison.InvariantCultureIgnoreCase) 
                                                           && projectSourceFileDirectory.Length > project.SourcesFilterOrigin.Length + 1)
                    {
                        projectSourceFilter = "Source\\" + projectSourceFileDirectory.Substring(project.SourcesFilterOrigin.Length + 1);
                        var projectSourceFilterIter = projectSourceFilter;
                        while (!projectFoldersCache.Contains(projectSourceFilterIter) && !string.IsNullOrEmpty(projectSourceFilterIter))
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
                    vcxProjFilters./**//**/WriteAttributeString("Include", projectSourceFile.FilePath);
                    vcxProjFilters./**//**/WriteElementString("Filter", projectSourceFilter);
                    vcxProjFilters./**/WriteEndElement();
                    vcxProjFilters.WriteEndElement();
                }

                vcxProjFilters./**/WriteEndElement();
                vcxProjFilters.WriteEndDocument();
            }

            return vcxProjPath;
        }

        /// <summary>
        /// Generates solution files for Visual Studio.
        /// </summary>
        /// <param name="solution">Parsed solution object.</param>
        /// <returns>Path to Visual Studio '.sln' file.</returns>
        public sealed override string GenerateSolutionFiles(Solution solution)
        {
            // ==========================================================================================
            // Generating SLN files..
            // ==========================================================================================
            var slnPath = Path.Combine(base.GenerateSolutionFiles(solution), solution.Name + ".sln");
            using (var sln = new StreamWriter(slnPath))
            {
                sln.WriteLine();
                sln.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
                sln.WriteLine("# Generated by GoddamnBuildSystem. Please, do not edit this file manually.");

                // ------------------------------------------------------------------------------------------
                // Caching GUIDs of projects and filters and defining filters.
                // ------------------------------------------------------------------------------------------
                var solutionFiltersGuidCache = new Dictionary<int, string>();
                foreach (var solutionProject in solution.Projects)
                {
                    // Loading the GUID of a build tool..
                    if (solutionProject.IsBuildTool)
                    {
                        // For build tools the best was is to load a GUID from project files: this causes exceptions 
                        // in Visual Studio when reloading of solution, and all opened tabs would be terminated..
                        try
                        {
                            solutionProject.Misc.GUID = QueryGuidFromProject(solutionProject.GeneratorOutputPath);
                        }
                        catch (Exception exception)
                        {
                            Console.Error.WriteLine("Unable to load a GUID of C# project \"{0}\". Check this out.", solution.GeneratedSolutionPath);
                            Console.Error.WriteLine(exception.ToString());

                            solutionProject.Misc.GUID = CreateMsBuildGuid();
                        }
                    }

                    // Looking for projects with filters which we have not already cached.
                    if (!string.IsNullOrEmpty(solutionProject.Filter))
                    {
                        var solutionProjectFilterHash = solutionProject.Filter.GetHashCode();
                        if (!solutionFiltersGuidCache.ContainsKey(solutionProjectFilterHash))
                        {
                            // Generating and caching filter GUIDS.
                            solutionProject.Misc.FilterGUID = CreateMsBuildGuid();
                            solutionFiltersGuidCache.Add(solutionProjectFilterHash, solutionProject.Misc.FilterGUID);

                            // E.g. 'Project({Filter-GUID}) = "Name", "Name", "Filter-GUID"'
                            sln.WriteLine("Project(\"{0}\") = \"{1}\", \"{1}\", \"{2}\"", c_SlnFilterProjectGuid, solutionProject.Filter, solutionProject.Misc.FilterGUID);
                            sln.WriteLine("EndProject");
                        }
                        else
                        {
                            solutionProject.Misc.FilterGUID = solutionFiltersGuidCache[solutionProjectFilterHash];
                        }
                    }
                }

                // ------------------------------------------------------------------------------------------
                // Defining Solution projects.
                // ------------------------------------------------------------------------------------------
                foreach (var solutionProject in solution.Projects)
                {
                    // E.g. 'Project({Type-GUID}) = "Name", "Path-ToProject-File", "Project-GUID"'
                    sln.WriteLine("Project(\"{0}\") = \"{1}\", \"{2}\", \"{3}\"",
                        /* 0 */ solutionProject.IsBuildTool ? c_SlnCsProjProjectGuid : c_SlnVcxProjProjectGuid,
                        /*1,2*/ solutionProject.Name, solutionProject.GeneratorOutputPath,
                        /* 3 */ solutionProject.Misc.GUID);
                    // Defining projects this one depends on.
                    sln.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                    var project = solutionProject;
                    foreach (var solutionDependentProject in solution.Projects)
                    {
                        if (solutionDependentProject.Priority > project.Priority)
                        {
                            // Writing projects with higher priority as ones we depend from.
                            sln.WriteLine("\t\t{0} = {0}", solutionDependentProject.Misc.GUID);
                        }
                    }
                    sln.WriteLine("\tEndProjectSection");
                    sln.WriteLine("EndProject");
                }

                sln.WriteLine("Global");

                // ------------------------------------------------------------------------------------------
                // Linking platforms & configurations.
                // ------------------------------------------------------------------------------------------
                sln.WriteLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
                foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                {
                    var platformInfo = TargetPlatformInfo.Get(platform);
                    foreach (var configurationInfo in TargetInfo.EnumerateAllConfigurations().Select(TargetConfigurationInfo.Get))
                    {
                        // E.g. \tConfiguration-Name|Platform-Name = Configuration-Name|Platform-Name
                        sln.WriteLine("\t\t{0}|{1} = {0}|{1}", configurationInfo.HumanReadableName, platformInfo.HumanReadableName);
                    }
                }
                sln.WriteLine("\tEndGlobalSection");

                sln.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
                foreach (var solutionProject in solution.Projects)
                {
                    foreach (var platform in TargetInfo.EnumerateAllPlatforms())
                    {
                        var platformInfo = TargetPlatformInfo.Get(platform);
                        foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                        {
                            var configurationInfo = TargetConfigurationInfo.Get(configuration);
                            var platfromConfig = solutionProject.IsBuildTool 
                                ? $"\t\t{solutionProject.Misc.GUID}.{configurationInfo.HumanReadableName}|{platformInfo.HumanReadableName}.* = {(configurationInfo.IsDebug ? "Debug" : "Release")}|Any CPU" 
                                : $"\t\t{solutionProject.Misc.GUID}.{configurationInfo.HumanReadableName}|{platformInfo.HumanReadableName}.* = {configuration}{platform}|{ConvertPlatformToMsBuildPlatform(platform)}";

                            sln.WriteLine(platfromConfig.Replace("*", "ActiveCfg"));
                            sln.WriteLine(platfromConfig.Replace("*", "Build.0"));
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
                foreach (var solutionProject in solution.Projects)
                {
                    if (!string.IsNullOrEmpty(solutionProject.Filter))
                    {
                        sln.WriteLine("\t\t{0} = {1}", solutionProject.Misc.GUID, solutionProject.Misc.FilterGUID);
                    }
                }
                sln.WriteLine("\tEndGlobalSection");
                sln.WriteLine("EndGlobal");
            }

            return slnPath;
        }

    }   // class VisualStudioProjectGenerator

}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
