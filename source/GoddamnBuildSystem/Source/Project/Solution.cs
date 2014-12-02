//! ==========================================================================================
//! Solution.cs - represents a solution (set of projects).
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* --.06.2014 - Created by James Jhuighuy
//!     * 24.11.2014 - Refactored.
//! ==========================================================================================

using System;
using System.IO;
using System.Linq;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Represents a solution (set of projects).
    /// </summary>
    public class Solution
    {
        //! ------------------------------------------------------------------------------------------
        //! Fields.
        //! ------------------------------------------------------------------------------------------

        private string m_Location = null;

        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Instantiates a new solution.
        /// </summary>
        /// <param name="Location">Location, where the solution files exist.</param>
        public Solution() { }
        public Solution(string Location) { m_Location = Location; }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Returns path to solution file.
        /// </summary>
        public string GetLocation()
        {
            return m_Location;
        }

        /// <summary>
        /// Returns name of this solution.
        /// By default, it is a name of directory, where solution is located or name of the solution file.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        public virtual string GetName(TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            string SolutionName = m_Location;
            while (!string.IsNullOrEmpty(Path.GetExtension(SolutionName))) {
                SolutionName = Path.GetFileNameWithoutExtension(SolutionName);
            }

            return SolutionName;
        }

        /// <summary>
        /// Returns list of projects in solution.
        /// By default, it scans all solution sub-directories for ones with .GDPROJ.CS and .CSPROJ files. 
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        public virtual void GetProjects(ref MultiTree<Project> SolutionProjects, TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            Stack<MultiTree<Project>.Node> SolutionProjectsStack = new Stack<MultiTree<Project>.Node>();
            SolutionProjectsStack.Push(SolutionProjects.GetRootNode());

            Action<string> ProcessSolutionDirectory = null;
            ProcessSolutionDirectory = new Action<string>(SolutionDirectory => { 
                // Processing all project files in current directory
                foreach (string SolutionProjectFile in Directory.EnumerateFiles(SolutionDirectory)) {
                    string SolutionProjectFileExtension = Path.GetExtension(SolutionProjectFile).ToLower();
                    switch (SolutionProjectFileExtension) {
                        case ".csproj":
                            SolutionProjectsStack.Peek().GetElements().Add(new CSharpProject(SolutionProjectFile));
                            return;

                        case ".cs":
                            string SolutionProjectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(SolutionProjectFile)).ToLower();
                            switch (SolutionProjectFileSecondExtension) {
                                case ".gdproj":
                                    SolutionProjectsStack.Peek().GetElements().Add(Project.CreateProjectForDirectory(SolutionProjectFile));
                                    return;
                            }

                            break;
                    }
                }

                // Processing all subdirectories in this directory.
                foreach (string SolutionSubdirectory in Directory.EnumerateFiles(SolutionDirectory)) {
                    string SolutionSubdirectoryName = Path.GetFileName(SolutionSubdirectory);

                    // Processing this subdirectory if is not service one.
                    if (SolutionSubdirectoryName[0] != '_') {
                        SolutionProjectsStack.Push(SolutionProjectsStack.Peek().CreateChildNode(SolutionSubdirectoryName));
                        ProcessSolutionDirectory(SolutionSubdirectory);
                        SolutionProjectsStack.Pop();
                    }
                }
            });

            ProcessSolutionDirectory(Path.GetDirectoryName(this.GetLocation()));
        }

        /// <summary>
        /// Returns list of configurations in solution.
        /// </summary>
        /// <param name="Platform">Platform for which resolving is done.</param>
        public virtual void GetConfigurations(List<TargetConfiguration> SolutionConfigurations, TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            SolutionConfigurations.AddRange(Enum.GetValues(typeof(TargetConfiguration)).Cast<TargetConfiguration>().ToList());
        }

        //! ------------------------------------------------------------------------------------------
        //! Class static API.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Creates a solution object for specified location.
        /// </summary>
        /// <param name="SolutionFile">Name of the solution file, or path to directory in which project would be located.</param>
        internal static Solution CreateSolutionForSource(string SolutionFile)
        {
            throw new NotImplementedException();
        }
    }   // GoddamnEngine.BuildSystem
}   // GoddamnEngine.BuildSystem
