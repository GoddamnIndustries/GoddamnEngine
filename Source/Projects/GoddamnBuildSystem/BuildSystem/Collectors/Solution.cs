// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Represents a solution (set of projects).
    /// </summary>
    public class Solution : Collector
    {
        /// <summary>
        /// Collects list of projects in solution.
        /// All unsupported projects should be filtered by this function.
        /// </summary>
        /// <returns>Iterator for list of projects in solution.</returns>
        public virtual IEnumerable<ProjectCache> EnumerateProjects()
        {
            foreach (var solutionSourcePath in Directory.EnumerateFiles(
                Path.Combine(GetLocation(), "Projects"), "*.gdproj.cs", SearchOption.AllDirectories))
            {
                var solutionCache = ProjectFactory.Create(solutionSourcePath);
                if (solutionCache.IsSupported)
                {
                    yield return solutionCache;
                }
            }
        }

    }   // class Solution

    /// <summary>
    /// Represents a collection of cached data that was by dependency object.
    /// </summary>
    public class SolutionCache : CollectorCache
    {
        public string GeneratedSolutionPath;
        public readonly ProjectCache[] CachedProjects;

        /// <summary>
        /// Generates cache for specified dependency.
        /// </summary>
        /// <param name="solution">Solution which dynamic properties would be cached.</param>
        public SolutionCache(Solution solution) : base(solution)
        {
            if (IsSupported)
            {
                CachedProjects = solution.EnumerateProjects().ToArray();
            }
        }
    }   // class SolutionCache

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
        public static SolutionCache Create(string solutionSourcePath)
        {
            return CollectorFactory<Solution, SolutionCache>.Create(solutionSourcePath);
        }
    }   // class SolutionFactory

}   // namespace GoddamnEngine.BuildSystem.Collectors
