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
                case TargetPlatform.MacOS:
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Generates project files for CMake: 'CMakeLists.txt'.
        /// </summary>
        /// <param name="project">Parsed project object.</param>
        /// <returns>Path to CMake's 'CMakeLists.txt' file.</returns>
        public sealed override string GenerateProjectFiles(Project project)
        {
            var cmakeListsPath = Path.Combine(base.GenerateProjectFiles(project), "CMakeLists.txt");

            // ==========================================================================================
            // Generating CMakeLists.txt files.
            // ==========================================================================================
            using (var cmakeLists = new StreamWriter(cmakeListsPath))
            {
				var platform = TargetPlatform.Linux;
				var configuration = TargetConfiguration.Debug;

                cmakeLists.WriteLine("cmake_minimum_required (VERSION 3.0)");
                cmakeLists.WriteLine("project({0})", project.Name);
				cmakeLists.WriteLine("include_directories({0})", project.GenerateIncludePaths(platform, configuration, "\n\t").Replace('\\', '/'));
				cmakeLists.WriteLine("link_libraries({0})", project.GenerateLinkedLibrariesPaths(platform, configuration, "\n\t").Replace('\\', '/'));
				cmakeLists.WriteLine("set(CMAKE_CXX_FLAGS \"-std=c++14 -fpermissive\")");
				switch (project.BuildType[platform, configuration])
                {
                    case ProjectBuildType.Application:
						cmakeLists.WriteLine("set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \"{0}\")", Path.GetDirectoryName(project.OutputPath[platform, configuration]));
                        cmakeLists.WriteLine("add_executable({0}", project.Name);
                        break;
                    case ProjectBuildType.StaticLibrary:
						cmakeLists.WriteLine("set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY \"{0}\")", Path.GetDirectoryName(project.OutputPath[platform, configuration]));
                        cmakeLists.WriteLine("add_library({0}", project.Name);
                        break;
                    case ProjectBuildType.DynamicLibrary:
						cmakeLists.WriteLine("set(CMAKE_LIBRARY_OUTPUT_DIRECTORY \"{0}\")", Path.GetDirectoryName(project.OutputPath[platform, configuration]));
                        cmakeLists.WriteLine("add_library({0} SHARED", project.Name);
                        break;
                }
				foreach (var projectSource in project.Files[platform, configuration])
                {
					switch (projectSource.FileType)
					{
						case ProjectSourceFileType.SourceCode:
						case ProjectSourceFileType.HeaderFile:
						case ProjectSourceFileType.InlineImplementation:
							cmakeLists.WriteLine ("\t{0}", projectSource.FilePath.Replace ('\\', '/'));
							break;
					}
                }
                cmakeLists.WriteLine("\t)");
            }

            return cmakeListsPath;
        }

        /// <summary>
        /// Generates solution files for CMake.
        /// </summary>
        /// <param name="solution">Parsed solution object.</param>
        /// <returns>Path to CMake's 'CMakeLists.txt' file.</returns>
        public sealed override string GenerateSolutionFiles(Solution solution)
        {
            var cmakeListsPath = Path.Combine(solution.Location, "CMakeLists.txt");
            using (var cmakeLists = new StreamWriter(cmakeListsPath))
            {
                cmakeLists.WriteLine("cmake_minimum_required (VERSION 2.8)");
                cmakeLists.WriteLine("project({0})", solution.Name);
                foreach (var solutionProject in solution.Projects)
                {
                    if (solutionProject.IsBuildTool) continue;
                    cmakeLists.WriteLine("add_subdirectory({0})", 
                                         Path.Combine(solutionProject.Location, "Build").Replace('\\', '/'));
                }
            }
            return cmakeListsPath;
        }
    }   // class CMakeProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
