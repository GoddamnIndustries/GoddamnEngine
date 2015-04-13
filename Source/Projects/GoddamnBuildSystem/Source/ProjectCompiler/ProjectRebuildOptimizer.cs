// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file ProjectRebuildOptimizer.cs
//! Resolves dependencies in header files and determines, whether source is needed to be rebuilt.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GoddamnEngine.BuildSystem.ProjectCompiler
{
    //[Serializable]
    public class ProjectBuildableSourceFile
    {
        public ProjectSourceFile m_Source;
        public List<ProjectBuildableSourceFile> m_IncludedFiles;
        public int m_LastHash;

        public ProjectBuildableSourceFile(ProjectSourceFile Source)
        {
            m_Source = Source;
            m_IncludedFiles = new List<ProjectBuildableSourceFile>();
            m_LastHash = 0;
        }

        public void Parse(List<ProjectBuildableSourceFile> AllSourceFiles)
        {
            foreach (var ReadLine in File.ReadAllLines(m_Source.m_FileName))
            {
                string Line = ReadLine.Trim();
                if (Line.StartsWith("#"))
                {
                    Line = Line.Substring("#".Length).Trim();
                    if (Line.StartsWith("include"))
                    {
                        Line = Line.Substring("include".Length).Trim();

                        string IncludedFilePath = null;
                        if (Line.StartsWith("<"))
                        {
                            IncludedFilePath = Line.Substring("<".Length, Line.IndexOf(">") - "<".Length).Trim();
                        }
                        else if (Line.StartsWith("\""))
                        {
                            IncludedFilePath = Line.Substring("\"".Length, Line.IndexOf("\"") - "\"".Length).Trim();
                        }
                        else if (Line.StartsWith("GD_PLATFORM_INCLUDE("))
                        {
                            throw new NotImplementedException();
                            // IncludedFilePath = Line.Substring("GD_PLATFORM_INCLUDE(".Length, Line.IndexOf(",") - "GD_PLATFORM_INCLUDE(".Length).Trim();
                        }


                    }
                }
            }
        }

    }   // public class ProjectBuildableSourceFile

    public class ProjectRebuildOptimizer
    {

    }   // public class ProjectRebuildOptimizer
}   // namespace GoddamnEngine.BuildSystem.ProjectCompiler
