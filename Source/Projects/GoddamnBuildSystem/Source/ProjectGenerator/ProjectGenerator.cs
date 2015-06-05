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
        public virtual bool IsPlatformNativelySupported(TargetPlatform Platform)
        {
            return false;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates project files for specified project.
        //! @param Project Parsed project object.
        //! @returns Path to main project file.
        public virtual string GenerateProjectFiles(ProjectCache Project)
        {
            var ProjectDirectoryPath = Path.Combine(Project.CachedLocation, "Build");
            Directory.CreateDirectory(ProjectDirectoryPath);

            return ProjectDirectoryPath;
        }

        // ------------------------------------------------------------------------------------------
        //! Generates solution files for specified solution.
        //! @param Solution Parsed solution object.
        //! @returns Path to main project file.
        public virtual string GenerateSolutionFiles(SolutionCache Solution)
        {
            var SolutionDirectoryPath = Path.Combine(Solution.CachedLocation, "Build");
            Directory.CreateDirectory(SolutionDirectoryPath);

            return SolutionDirectoryPath;
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
            ProjectGenerators ProjectGenerator;
            switch (Environment.OSVersion.Platform)
            {
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

            var ProjectGeneratorType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(ProjectGenerator + "ProjectGenerator"));
            if (ProjectGeneratorType == null)
            {
                throw new BuildSystemException("No generator for {0} IDE exists.", ProjectGenerator);
            }

            return (ProjectGenerator)Activator.CreateInstance(ProjectGeneratorType);
        }
    }   // class TargetProjectGeneratorFactory

    // ------------------------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------------------------
    //! Project file generation module.
    [BuildSystemModule("--generate-project")]
    public sealed class ProjectGeneratorModule : BuildSystemModule
    {
        // ------------------------------------------------------------------------------------------
        //! Generates project files.
        public override int Execute(string[] Arguments)
        {
            try
            {
                var ProjectGenerator = ProjectGeneratorFactory.Create();
                var Solution = SolutionFactory.Create(Arguments.Length > 0
                    ? Arguments[0]
                    : Path.Combine(BuildSystem.GetSDKLocation(), "source", "GoddamnEngine.gdsln.cs")
                );

                foreach (var Project in Solution.CachedProjects.Where(Project => !Project.IsBuildTool))
                {
                    Project.GeneratorOutputPath = ProjectGenerator.GenerateProjectFiles(Project);
                }

                Solution.GeneratedSolutionPath = ProjectGenerator.GenerateSolutionFiles(Solution);
                return 0;
            }
            catch (ProjectGeneratorException Exception)
            {
                Console.Error.WriteLine("Unhanded error was caught while running the Project Generator Module:");
                Console.Error.WriteLine(Exception.Message);
                return 1;
            }
        }
    }
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
