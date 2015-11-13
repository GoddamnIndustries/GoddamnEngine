// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file ProjectGenerator.cs
//! Project generator interface.

using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    // ------------------------------------------------------------------------------------------
    //! List of supported target make systems.
    public enum ProjectGenerators : byte
    {
        Unknown,
        VisualStudio,
        QtCreator,
        XCode,
    }   //enum TargetBuildSystem

    // ------------------------------------------------------------------------------------------
    //! Represents a project generation algorithm interface.
    public abstract class ProjectGenerator
    {
        // ------------------------------------------------------------------------------------------
        //! Checks if specified platform is natively supported by IDE.
        //! @param Platform Some platform.
        public virtual bool IsPlatformNativelySupported(TargetPlatform platform)
        {
            return false;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates project files for specified project.
        //! @param Project Parsed project object.
        //! @returns Path to main project file.
        public virtual string GenerateProjectFiles(ProjectCache project)
        {
            var projectDirectoryPath = Path.Combine(project.CachedLocation, "Build");
            Directory.CreateDirectory(projectDirectoryPath);

            return projectDirectoryPath;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates solution files for specified solution.
        //! @param Solution Parsed solution object.
        //! @returns Path to main project file.
        public virtual string GenerateSolutionFiles(SolutionCache solution)
        {
            var solutionDirectoryPath = Path.Combine(solution.CachedLocation, "Build");
            Directory.CreateDirectory(solutionDirectoryPath);

            return solutionDirectoryPath;
        }
    }   // class TargetProjectGenerator

    // ------------------------------------------------------------------------------------------
    //! Represents a factory of IDE project files generator.
    public static class ProjectGeneratorFactory
    {
        // ------------------------------------------------------------------------------------------
        //! Constructs new IDE project files generator instance.
        public static ProjectGenerator Create()
        {
            ProjectGenerators projectGenerator;
            switch (Environment.OSVersion.Platform)
            {
                case PlatformID.Win32NT:
                    projectGenerator = ProjectGenerators.VisualStudio;
                    break;
                case PlatformID.MacOSX:
                    projectGenerator = ProjectGenerators.XCode;
                    break;
                case PlatformID.Unix:
                    projectGenerator = ProjectGenerators.QtCreator;
                    break;
                default:
                    throw new BuildSystemException("Unable to determine IDE for this platform");
            }

            var projectGeneratorType = Assembly.GetExecutingAssembly().GetTypes()
                .FirstOrDefault(T => T.Name.EndsWith(projectGenerator + "ProjectGenerator"));
            if (projectGeneratorType == null)
            {
                throw new BuildSystemException("No generator for {0} IDE exists.", projectGenerator);
            }

            return (ProjectGenerator)Activator.CreateInstance(projectGeneratorType);
        }
    }   // class TargetProjectGeneratorFactory

    // ------------------------------------------------------------------------------------------
    //! Represents an exception, thrown by ProjectGenerator code.
    [Serializable]
    public sealed class ProjectGeneratorException : BuildSystemException
    {
        // ------------------------------------------------------------------------------------------
        //! Constructs the build system exception with a string.
        //! @param Format the format string.
        //! @param arguments Formatting arguments.
        public ProjectGeneratorException(string format, params object[] arguments)
            : base(format, arguments)
        {
        }
    }   // class ProjectGeneratorException

    // ------------------------------------------------------------------------------------------
    //! Project file generation module.
    [BuildSystemModule("--generate-project")]
    public sealed class ProjectGeneratorModule : BuildSystemModule
    {
        // ------------------------------------------------------------------------------------------
        //! Generates project files.
        public override int Execute(string[] arguments)
        {
            try
            {
                var projectGenerator = ProjectGeneratorFactory.Create();
                var solution = SolutionFactory.Create(arguments.Length > 0
                    ? arguments[0]
                    : Path.Combine(BuildSystem.GetSdkLocation(), "source", "GoddamnEngine.gdsln.cs")
                );

                foreach (var project in solution.CachedProjects.Where(project => !project.IsBuildTool))
                {
                    project.GeneratorOutputPath = projectGenerator.GenerateProjectFiles(project);
                }

                solution.GeneratedSolutionPath = projectGenerator.GenerateSolutionFiles(solution);
                return 0;
            }
            catch (ProjectGeneratorException exception)
            {
                Console.Error.WriteLine("Unhanded error was caught while running the Project Generator Module:");
                Console.Error.WriteLine(exception.Message);
                return 1;
            }
        }
    }
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
