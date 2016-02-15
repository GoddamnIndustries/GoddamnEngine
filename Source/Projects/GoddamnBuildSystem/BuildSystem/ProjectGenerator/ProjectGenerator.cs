// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using GoddamnEngine.BuildSystem.Collectors;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    /// <summary>
    /// Represents a project generation algorithm interface.
    /// </summary>
    public abstract class ProjectGenerator
    {
        /// <summary>
        /// Checks if specified platform is natively supported by IDE.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if specified platform is natively supported by IDE.</returns>
        public virtual bool IsPlatformNativelySupported(TargetPlatform platform)
        {
            return false;
        }

        /// <summary>
        /// Generates project files for specified project.
        /// </summary>
        /// <param name="project">Parsed project object.</param>
        /// <returns>Path to main project file.</returns>
        public virtual string GenerateProjectFiles(ProjectCache project)
        {
            var projectDirectoryPath = Path.Combine(project.CachedLocation, "Build");
            Directory.CreateDirectory(projectDirectoryPath);

            return projectDirectoryPath;
        }

        /// <summary>
        /// Generates solution files for specified solution.
        /// </summary>
        /// <param name="solution">Parsed solution object.</param>
        /// <returns>Path to main solution file.</returns>
        public virtual string GenerateSolutionFiles(SolutionCache solution)
        {
            var solutionDirectoryPath = Path.Combine(solution.CachedLocation, "Build");
            Directory.CreateDirectory(solutionDirectoryPath);

            return solutionDirectoryPath;
        }

    }   // class ProjectGenerator

    /// <summary>
    /// Represents a factory of IDE project files generator.
    /// </summary>
    public static class ProjectGeneratorFactory
    {
        /// <summary>
        /// Constructs new IDE project files generator instance.
        /// </summary>
        /// <returns>A new IDE project files generator instance</returns>
        public static ProjectGenerator Create()
        {
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                return new VisualStudioProjectGenerator();
            }
            throw new BuildSystemException("Unable to determine IDE for this platform");
        }
    }   // class TargetProjectGeneratorFactory

    /// <summary>
    /// Represents an exception, thrown by ProjectGenerator code.
    /// </summary>
    [Serializable]
    public sealed class ProjectGeneratorException : BuildSystemException
    {
        /// <summary>
        /// Constructs the build system exception with a string.
        /// </summary>
        /// <param name="format">Format string.</param>
        /// <param name="arguments">Formatting arguments.</param>
        public ProjectGeneratorException(string format, params object[] arguments)
            : base(format, arguments)
        {
        }
    }   // class ProjectGeneratorException

    /// <summary>
    /// Project file generation module.
    /// </summary>
    [BuildSystemModule("--generate-project")]
    public sealed class ProjectGeneratorModule : BuildSystemModule
    {
        /// <summary>
        /// Generates project files.
        /// </summary>
        /// <param name="arguments">Command line arguments.</param>
        /// <returns></returns>
        public override int Execute(string[] arguments)
        {
            try
            {
                var projectGenerator = ProjectGeneratorFactory.Create();
                var solution = SolutionFactory.Create(arguments.Length > 0 ? arguments[0] : Path.Combine(BuildSystem.GetSdkLocation(), "source", "GoddamnEngine.gdsln.cs"));

                // Generating project files..
                Parallel.ForEach(solution.CachedProjects.Where(project => !project.IsBuildTool)
                    , project => project.GeneratorOutputPath = projectGenerator.GenerateProjectFiles(project));

                // Generating the solution..
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
    }   // class ProjectGeneratorModule

}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
