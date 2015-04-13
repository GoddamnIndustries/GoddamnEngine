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
    //! @brief Represents a solution (set of projects).
    public class Solution : Collector
    {
        //! @brief Collects list of projects in solution.
        //! @note All unsupported projects should be filtered by this function.
        //! @returns Iterator for list of projects in solution.
        public virtual IEnumerable<ProjectCache> EnumerateProjects()
        {
            foreach (string ProjectFile in Directory.EnumerateFiles(Path.Combine(GetLocation(), "Projects"), "*.gdproj.cs", SearchOption.AllDirectories))
            {
                ProjectCache Project = ProjectFactory.Create(ProjectFile);
                if (Project.m_IsSupported)
                {
                    yield return Project;
                }
            }
        }

    }   // class Solution

    //! @brief Represents a collection of cached data that was by dependency object.
    public sealed class SolutionCache : CollectorCache
    {
        public string m_GeneratedSolutionPath;
        public readonly ProjectCache[] m_CachedProjects;

        //! @brief Generates cache for specified dependency.
        //! @param Solution Solution which dynamic properties would be cached.
        public SolutionCache(Solution Solution)
            : base(Solution)
        {
            if (m_IsSupported)
            {
                m_CachedProjects = Solution.EnumerateProjects().ToArray();
            }
        }
    }   // class DependencyCache

    //! @brief Represents a factory of dependencies.
    public /*static*/ sealed class SolutionFactory : CollectorFactory<Solution, SolutionCache>
    {
    }   // class SolutionFactory
}   // namespace GoddamnEngine.BuildSystem
