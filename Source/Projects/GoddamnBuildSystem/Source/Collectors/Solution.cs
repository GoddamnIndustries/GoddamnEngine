// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Solution.cs 
//! Represents a solution (set of projects).

using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GoddamnEngine.BuildSystem
{
	// ------------------------------------------------------------------------------------------
    //! Represents a solution (set of projects).
    public class Solution : Collector
    {
        // ------------------------------------------------------------------------------------------
        //! Collects list of projects in solution.
        //! @note All unsupported projects should be filtered by this function.
        //! @returns Iterator for list of projects in solution.
        public virtual IEnumerable<ProjectCache> EnumerateProjects()
        {
            return Directory.EnumerateFiles(Path.Combine(GetLocation(), "Projects"), "*.gdproj.cs", SearchOption.AllDirectories)
                .Select(ProjectFactory.Create).Where(project => project.IsSupported);
        }
    }   // class Solution

    // ------------------------------------------------------------------------------------------
    //! Represents a collection of cached data that was by dependency object.
    public class SolutionCache : CollectorCache
    {
        public string GeneratedSolutionPath;
        public readonly ProjectCache[] CachedProjects;

        // ------------------------------------------------------------------------------------------
        //! Generates cache for specified dependency.
        //! @param solution Solution which dynamic properties would be cached.
        public SolutionCache(Solution solution)
            : base(solution)
        {
            if (IsSupported)
                CachedProjects = solution.EnumerateProjects().ToArray();
        }
    }   // class DependencyCache

    //! Represents a factory of dependencies.
    public class SolutionFactory : CollectorFactory<Solution, SolutionCache>
    {
    }   // class SolutionFactory
}   // namespace GoddamnEngine.BuildSystem
