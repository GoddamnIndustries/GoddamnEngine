// ==========================================================================================
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

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Solution enumerator.
    /// </summary>
    public class SolutionEnumerator : CollectorEnumerator
    {
        /// <summary>
        /// Collects list of projects in solution.
        /// </summary>
        public virtual IEnumerable<Project> EnumerateProjects()
        {
            foreach (var solutionSourcePath in Directory.EnumerateFiles(Path.Combine(GetLocation(), "Projects"), "*.gdproj.cs", SearchOption.AllDirectories))
            {
                Project project;
                try
                {
                    project = ProjectFactory.Create(solutionSourcePath);
                }
                catch (Exception exception)
                {
                    Console.WriteLine($"Failed to load project {solutionSourcePath} with exception {exception}");
                    continue;
                }
                //if (solutionCache.IsSupported)
                {
                    yield return project;
                }
            }
        }
    }   // public class SolutionEnumerator

    /// <summary>
    /// Solution object.
    /// </summary>
    public class Solution : Collector
    {
        /// <summary>
        /// List of projects.
        /// </summary>
        public readonly Project[] Projects;

        /// <summary>
        /// Path the the generated solution file.
        /// </summary>
        public string GeneratedSolutionPath;

        /// <inheritdoc />
        public Solution(SolutionEnumerator solution) : base(solution)
        {
            Projects = solution.EnumerateProjects().ToArray();
        }
    }   // class Solution

    /// <summary>
    /// Represents a factory of dependencies.
    /// </summary>
    public static class SolutionFactory
    {
        /// <summary>
        /// Constructs new solution instance and it's cached data.
        /// </summary>
        /// <param name="solutionSourcePath">Path to the source file of the solution.</param>
        /// <returns>Created instance of cached solution data.</returns>
        public static Solution Create(string solutionSourcePath)
        {
            return CollectorFactory<SolutionEnumerator, Solution>.Create(solutionSourcePath);
        }
    }   // class SolutionFactory

}   // namespace GoddamnEngine.BuildSystem.Collectors
