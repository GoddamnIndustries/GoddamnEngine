// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem.Collectors;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    /// <summary>
    /// Generator of the project/solution files for CMake.
    /// </summary>
    /// <inheritdoc />
    class CMakeProjectGenerator : ProjectGenerator
    {

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
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Generates project files for Visual Studio: '.vcxproj' and '.vcxproj.filter'.
        /// </summary>
        /// <param name="project">Parsed project object.</param>
        /// <returns>Path to Visual Studio's '.vcxproj file'.</returns>
        public sealed override string GenerateProjectFiles(ProjectCache project)
        {
            // ==========================================================================================
            // Generating CMakeLists.txt files.
            // ==========================================================================================
            var cmakeListsPath = Path.Combine(project.CachedLocation, "CMakeLists.txt");
            using (var cmakeLists = new StreamWriter(cmakeListsPath))
            {
                cmakeLists.WriteLine("cmake_minimum_required (VERSION 3.0)");
                cmakeLists.WriteLine("project({0})", project.CachedName);
                cmakeLists.WriteLine("include_directories({0})", project.GenerateIncludePaths("\n\t").Replace('\\', '/'));
                cmakeLists.WriteLine("set(CMAKE_CXX_FLAGS -std=c++14)");
                switch (project.CachedBuildTypes[TargetPlatform.Windows, TargetConfiguration.Debug])
                {
                    case ProjectBuildType.Application:
                        cmakeLists.WriteLine("add_executable({0}", project.CachedName);
                        break;
                    case ProjectBuildType.StaticLibrary:
                        cmakeLists.WriteLine("add_library({0}", project.CachedName);
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        cmakeLists.WriteLine("add_library({0} SHARED", project.CachedName);
                        break;
                }
                foreach (var projectSource in project.CachedSourceFiles)
                {
                    if (projectSource.FileType != ProjectSourceFileType.SupportFile)
                        cmakeLists.WriteLine("\t{0}", projectSource.FileName.Replace('\\', '/'));
                }
                cmakeLists.WriteLine("\t)");
            }

            return cmakeListsPath;
        }

        /// <summary>
        /// Generates solution files for Visual Studio.
        /// </summary>
        /// <param name="solution">Parsed solution object.</param>
        /// <returns>Path to Visual Studio's .'sln' file.</returns>
        public sealed override string GenerateSolutionFiles(SolutionCache solution)
        {
            var cmakeListsPath = Path.Combine(solution.CachedLocation, "CMakeLists.txt");
            using (var cmakeLists = new StreamWriter(cmakeListsPath))
            {
                cmakeLists.WriteLine("cmake_minimum_required (VERSION 2.8)");
                cmakeLists.WriteLine("project({0})", solution.CachedName);
                foreach (var solutionProject in solution.CachedProjects)
                {
                    if (solutionProject.IsBuildTool) continue;
                    cmakeLists.WriteLine("add_subdirectory({0})", solutionProject.CachedLocation.Replace('\\', '/'));
                }

            }

            return cmakeListsPath;
        }
    }   // class CMakeProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
