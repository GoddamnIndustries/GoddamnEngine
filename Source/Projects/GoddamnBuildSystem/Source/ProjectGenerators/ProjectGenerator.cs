﻿//! ==========================================================================================
//! ProjectGenerator.cs - Project generator interface.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! ==========================================================================================

using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    //! List of supported target make systems.
    public enum ProjectGenerators : byte
    {
        Unknown,
        VisualStudio,
        QtCreator,
        XCode,
    }   //enum TargetBuildSystem

    //! @brief Represents a project generation algorithm interface.
    public abstract class ProjectGenerator
    {
        //! @brief Checks if specified platform is natively supported by IDE.
        //! @param Platform Some platform.
        public virtual bool IsPlatformNativelySupported(TargetPlatform Platform)
        {
            return false;
        }

        //! @brief Generates project files for specified project.
        //! @param Project Parsed project object.
        //! @returns Path to main project file.
        public virtual string GenerateProjectFiles(ProjectCache Project)
        {
            string ProjectDirectoryPath = Path.Combine(Project.m_CachedLocation, "Build");
            Directory.CreateDirectory(ProjectDirectoryPath);

            return ProjectDirectoryPath;
        }

        //! @brief Generates solution files for specified solution.
        //! @param Solution Parsed solution object.
        //! @returns Path to main project file.
        public virtual string GenerateSolutionFiles(SolutionCache Solution)
        {
            string SolutionDirectoryPath = Path.Combine(Solution.m_CachedLocation, "Build");
            Directory.CreateDirectory(SolutionDirectoryPath);

            return SolutionDirectoryPath;
        }
    }   // class TargetProjectGenerator

    //! @brief Represents a factory of IDE project files generator.
    public static class ProjectGeneratorFactory
    {
        //! Constructs new IDE project files generator instance.
        public static ProjectGenerator Create()
        {
            ProjectGenerators ProjectGenerator;
            switch (Environment.OSVersion.Platform) {
                case PlatformID.Win32NT:
                    ProjectGenerator = ProjectGenerators.VisualStudio;
                    break;
                case PlatformID.MacOSX:
                    ProjectGenerator = ProjectGenerators.XCode;
                    break;
                case PlatformID.Unix:
                    ProjectGenerator = ProjectGenerators.QtCreator;
                    break;
                default:
                    throw new BuildSystemException("Unable to determine IDE for this platform");
            }

            Type ProjectGeneratorType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(ProjectGenerator + "ProjectGenerator"));
            if (ProjectGeneratorType == null) {
                throw new BuildSystemException("No generator for {0} IDE exists.", ProjectGenerator);
            }

            return (ProjectGenerator)Activator.CreateInstance(ProjectGeneratorType);
        }
    }   // class TargetProjectGeneratorFactory

    //! Represents an exception, thrown by ProjectGenerator code.
    [Serializable]
    public sealed class ProjectGeneratorException : BuildSystemException
    {
        //! Constructs the build system exception with a string.
        //! @param Format Format string.
        //! @param Arguments Formatting arguments.
        public ProjectGeneratorException(string Format, params object[] Arguments)
            : base(Format, Arguments)
        {
        }
    }   // class ProjectGeneratorException

    //! Project file generation module.
    [BuildSystemModule("--generate-project")]
    public sealed class ProjectGeneratorModule : BuildSystemModule
    {
        //! Generates project files.
        public sealed override int Execute(string[] Arguments)
        {
            try {
                ProjectGenerator ProjectGenerator = ProjectGeneratorFactory.Create();
                SolutionCache Solution = SolutionFactory.Create(Arguments.Length > 0
                    ? Arguments[0]
                    : Path.Combine(BuildSystem.GetSDKLocation(), "source", "GoddamnEngine.gdsln.cs")
                );

                foreach (ProjectCache Project in Solution.m_CachedProjects) {
                    if (!Project.m_IsBuildTool) {
                        Project.m_GeneratorOutputPath = ProjectGenerator.GenerateProjectFiles(Project);
                    }
                }

                Solution.m_GeneratedSolutionPath = ProjectGenerator.GenerateSolutionFiles(Solution);
                return 0;
            } catch (ProjectGeneratorException Exception) {
                Console.Error.WriteLine("Unhanded error was caught while running the Project Generator Module:");
                Console.Error.WriteLine(Exception.Message);
                return 1;
            }
        }
    }
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator