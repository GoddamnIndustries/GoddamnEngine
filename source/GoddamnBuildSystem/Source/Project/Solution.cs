/// ==========================================================================================
/// Solution.cs - represents a solution (set of projects).
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2014 - Created by James Jhuighuy
///     * 24.11.2014 - Refactored.
/// ==========================================================================================

using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Represents a solution (set of projects)
    /// </summary>
    public abstract class Solution
    {
        //! ------------------------------------------------------------------------------------------
        //! Fields.
        //! ------------------------------------------------------------------------------------------

        private string m_Location = null;
        private MultiTree<Project> m_Projects = null;
        private List<TargetConfiguration> m_Configurations = null;

        //! ------------------------------------------------------------------------------------------
        //! Constructor.
        //! ------------------------------------------------------------------------------------------

        protected Solution() { }

        //! ------------------------------------------------------------------------------------------
        //! Virtual getters.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Returns name of the solution.
        /// </summary>
        public abstract string GetName();

        /// <summary>
        /// Returns path to solution file.
        /// </summary>
        public string GetLocation()
        {
            return m_Location;
        }

        /// <summary>
        /// Returns list of projects in solution.
        /// </summary>
        public virtual MultiTree<Project> GetProjects()
        {
            if (m_Projects == null) {
                m_Projects = new Tree<Project>();

                Stack<TreeNode<Project>> ProjectNodes = new Stack<TreeNode<Project>>();
                ProjectNodes.Push(m_Projects);

                Action<string> ProcessSolutionDirectory = null;
                ProcessSolutionDirectory = new Action<string>(CurrentSourceFilesDirectory => {
                    TreeNode<Project> Node = new TreeNode<Project>() { m_Label = Path.GetFileName(CurrentSourceFilesDirectory.Substring(0, CurrentSourceFilesDirectory.Length - 1)) };
                    ProjectNodes.Peek().m_Branches.Add(Node);
                    ProjectNodes.Push(Node);

                    foreach (string SolutionProjectFile in Directory.EnumerateFiles(CurrentSourceFilesDirectory)) {
                        switch (Path.GetExtension(SolutionProjectFile).ToLower()) {
                            case ".cs":
                                break;

                            case ".csproj":
                                if ((BuildSystem.GetProjectCompiler() == TargetProjectCompiler.VisualStudio2013) || (BuildSystem.GetProjectCompiler() == TargetProjectCompiler.VisualStudio2014)) {
                                    throw new NotFiniteNumberException();
                                }
                                break;
                        }

                        Node.m_Elements.Add(Project.CreateProject(ProjectFile));
                    }

                    foreach (string SolutionSubdirectory in Directory.GetDirectories(CurrentSourceFilesDirectory)) {
                        ProcessSolutionDirectory(SolutionSubdirectory);
                    }

                    ProjectNodes.Pop();
                });

                ProcessSolutionDirectory(m_Location);
            }

            return m_Projects;
        }

        /// <summary>
        /// Returns list of configurations in solution.
        /// </summary>
        public virtual List<TargetConfiguration> GetConfigurations()
        {
            if (m_Configurations == null) {
                m_Configurations = Enum.GetValues(typeof(TargetConfiguration)).Cast<TargetConfiguration>().ToList();
            }

            return m_Configurations;
        }

        //! ------------------------------------------------------------------------------------------
        //! Class static API.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Creates a solution object for specified location.
        /// </summary>
        internal static Solution CreateSolutionForSource(string SolutionLocation)
        {
            Assembly SolutionAssembly = CSharpCompiler.CompileSourceFile(SolutionLocation);
            Type[] SolutionAssembleTypes = (from T in SolutionAssembly.GetTypes() where (!T.IsAbstract) && (T.BaseType == typeof(Solution)) select T).ToArray();
            if (SolutionAssembleTypes.Length == 0) {
                throw new BuildSystemException("No appropriate type where found in {0}", SolutionAssembly);
            } else if (SolutionAssembleTypes.Length != 1) {
                throw new BuildSystemException("Multiple solution types where found in {0}", SolutionAssembly);
            }

            Solution CreatedSolution = (Solution)Activator.CreateInstance(SolutionAssembleTypes[0]);
            CreatedSolution.m_Location = SolutionLocation;
            return CreatedSolution;
        }
    }   // GoddamnEngine.BuildSystem
}   // GoddamnEngine.BuildSystem
