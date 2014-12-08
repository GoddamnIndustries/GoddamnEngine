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
        public virtual void GetProjects(ref List<Project> SolutionProjects, TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            // Processing all project files in current directory
            string SolutionLocation = Path.GetFullPath(Path.GetDirectoryName(this.GetLocation()));
            foreach (string SolutionProjectFile in Directory.EnumerateFiles(SolutionLocation, "*", SearchOption.AllDirectories)) {
                string SolutionProjectFileExtension = Path.GetExtension(SolutionProjectFile).ToLower();
                switch (SolutionProjectFileExtension) {
                    case ".csproj":
                        SolutionProjects.Add(new CSharpProject(SolutionProjectFile));
                        return;

                    case ".cs":
                        string SolutionProjectFileSecondExtension = Path.GetExtension(Path.GetFileNameWithoutExtension(SolutionProjectFile)).ToLower();
                        switch (SolutionProjectFileSecondExtension) {
                            case ".gdproj":
                                SolutionProjects.Add(Project.CreateProjectForSource(SolutionProjectFile));
                                return;
                        }
                        break;
                }
            }

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
            Solution SolutionObject = CSharpCompiler.InstantiateSourceFile<Solution>(SolutionFile);
            SolutionObject.m_Location = SolutionFile;

            return SolutionObject;
        }
    }   // GoddamnEngine.BuildSystem
}   // GoddamnEngine.BuildSystem
