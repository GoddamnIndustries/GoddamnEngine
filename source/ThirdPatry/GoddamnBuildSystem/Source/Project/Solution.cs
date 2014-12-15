//! ==========================================================================================
//! Solution.cs - represents a solution (set of projects).
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Linq;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    //! Represents a solution (set of projects).
    public class Solution
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        private string m_Location = null;

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Constructors.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Instantiates a new solution.
        //! @param Location Location, where the solution files exist.
        public Solution() { }
        public Solution(string Location)
        {
            m_Location = Location;
        }

        //! @}

        //! Returns path to solution file.
        public string GetLocation()
        {
            return m_Location;
        }

        //! ------------------------------------------------------------------------------------------
        //! @name Properties.
        //! @{
        //! ------------------------------------------------------------------------------------------


        //! Returns name of this solution.
        //! By default, it is a name of directory, where solution is located or name of the solution file.
        //! @param Platform Platform for which resolving is done.
        public virtual string GetName(TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            string SolutionName = m_Location;
            while (!string.IsNullOrEmpty(Path.GetExtension(SolutionName))) {
                SolutionName = Path.GetFileNameWithoutExtension(SolutionName);
            }

            return SolutionName;
        }

        //! Collects list of projects in solution.
        //! By default, it scans all solution sub-directories for ones with .GDPROJ.CS and .CSPROJ files. 
        //! @param SolutionProjects Output for collected projects.
        //! @param Platform Platform for which resolving is done.
        public virtual void GetProjects(ref List<BaseProject> SolutionProjects, TargetPlatform Platform)
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

        //! Returns list of configurations in solution.
        //! @param Platform Platform for which resolving is done.
        public virtual void GetConfigurations(ref List<TargetConfiguration> SolutionConfigurations, TargetPlatform Platform)
        {
            Debug.Assert(TargetPlatform.Unknown != Platform);

            SolutionConfigurations.AddRange(Enum.GetValues(typeof(TargetConfiguration)).Cast<TargetConfiguration>().ToList());
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Class static API.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Creates a solution object for specified location.
        //! @param SolutionFile Name of the solution file, or path to directory in which project would be located.
        internal static Solution CreateSolutionForSource(string SolutionFile)
        {
            Solution SolutionObject = CSharpCompiler.InstantiateSourceFile<Solution>(SolutionFile);
            SolutionObject.m_Location = SolutionFile;

            return SolutionObject;
        }

        //! @}
    }   // class Solution
}   // namespace GoddamnEngine.BuildSystem
