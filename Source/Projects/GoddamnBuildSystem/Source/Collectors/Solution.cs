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
    public abstract class Solution : Collector
    {
        // ------------------------------------------------------------------------------------------
        //! Collects list of projects in solution.
        //! @note All unsupported projects should be filtered by this function.
        //! @returns Iterator for list of projects in solution.
        public virtual IEnumerable<ProjectCache> EnumerateProjects()
        {
            return Directory.EnumerateFiles(Path.Combine(GetLocation(), "Projects"), "*.gdproj.cs", SearchOption.AllDirectories)
                .Select(ProjectFactory.Create).Where(Project => Project.IsSupported);
        }
    }   // class Solution

    // ------------------------------------------------------------------------------------------
    //! Represents a collection of cached data that was by dependency object.
    public abstract class SolutionCache : CollectorCache
    {
        public string GeneratedSolutionPath;
        public readonly ProjectCache[] CachedProjects;

        // ------------------------------------------------------------------------------------------
        //! Generates cache for specified dependency.
        //! @param Solution Solution which dynamic properties would be cached.
        protected SolutionCache(Solution Solution)
            : base(Solution)
        {
            if (IsSupported)
                CachedProjects = Solution.EnumerateProjects().ToArray();
        }
    }   // class DependencyCache

    //! Represents a factory of dependencies.
    public abstract class SolutionFactory : CollectorFactory<Solution, SolutionCache>
    {
    }   // class SolutionFactory
}   // namespace GoddamnEngine.BuildSystem
