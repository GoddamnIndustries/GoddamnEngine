//! ==========================================================================================
//! Solution.cs - represents a solution (set of projects).
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! ==========================================================================================

using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GoddamnEngine.BuildSystem
{
    //! Represents a solution (set of projects).
    public class Solution : Collector
    {
        //! Collects list of projects in solution.
        //! @note All unsupported projects should be filtered by this function.
        public virtual IEnumerable<ProjectCache> EnumerateProjects()
        {
            foreach (string ProjectFile in Directory.EnumerateFiles(Path.Combine(GetLocation(), "Projects"), "*.gdproj.cs", SearchOption.AllDirectories)) {
                ProjectCache Project = ProjectFactory.Create(ProjectFile);
                if (Project.m_IsSupported) {
                    yield return Project;
                }
            }
        }

    }   // class Solution

    //! Represents a collection of cached data that was by dependency object.
    public sealed class SolutionCache : CollectorCache
    {
        public string m_GeneratedSolutionPath;
        public readonly ProjectCache[] m_CachedProjects;

        //! Generates cache for specified dependency.
        public SolutionCache(Solution SolutionObject)
            : base(SolutionObject)
        {
            if (m_IsSupported) {
                m_CachedProjects = SolutionObject.EnumerateProjects().ToArray();
            }
        }
    }   // class DependencyCache

    //! Represents a factory of dependencies.
    public /*static*/ sealed class SolutionFactory : CollectorFactory<Solution, SolutionCache>
    {
    }   // class SolutionFactory
}   // namespace GoddamnEngine.BuildSystem
