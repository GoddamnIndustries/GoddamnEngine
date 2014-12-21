//! ==========================================================================================
//! VisualStudio.cs - Visual Studio IDE project files generator.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;

namespace GoddamnEngine.BuildSystem.Emscripten
{
    //! Generator of the makefiles for Emscripten.
    internal class EmscriptenMakefileGenerator : TargetMakefileGenerator
    {
        //! Generates makefile for specified project.
        //! @param Project Parsed project object.
        //! @returns Path to generated makefile.
        public override string GenerateMakefile(ProjectCache Project)
        {
            // ==========================================================================================
            // Generating makefile..
            // ==========================================================================================
            string MakefilePath = string.Concat(base.GenerateMakefile(Project), TargetPlatforms.Emscripten);
            using (StreamWriter Makefile = new StreamWriter(MakefilePath)) {
                Makefile.WriteLine("CC=\"C:\\Program Files\\Emscripten\\emscripten\\1.27.0\\emcc.bat\"");
                Makefile.WriteLine("CFLAGS=-std=c++11 -fno-rtti -O3 -Wno-warn-absolute-paths");
                Makefile.WriteLine("TARGET= {0}", Project.m_CachedOutputPathDelegates[TargetPlatforms.Emscripten](TargetConfigurations.Debug));
                Makefile.WriteLine("SOURCES= \\");
                foreach (var ProjectSource in Project.m_CachedSourceFiles) {
                    if (ProjectSource.m_FileType == ProjectSourceFileType.SourceCode) {
                        Makefile.WriteLine("\t{0} \\", ProjectSource.m_FileName);
                    }
                }
                Makefile.WriteLine();

                Makefile.WriteLine("HEADERS = \\");
                Makefile.WriteLine("\t{0} \\", Path.Combine(Project.m_CachedLocation, "Source"));
                Array.ForEach(Project.m_CachedDependencies, ProjectDependency =>
                    Array.ForEach(ProjectDependency.m_CachedHeaderDirectories, ProjectDependencyIncludePath =>
                        Makefile.WriteLine("\t-I{0} \\", ProjectDependencyIncludePath)
                    )
                );
                Makefile.WriteLine();

                Makefile.WriteLine("all: $(TARGET)");
                Makefile.WriteLine();
                Makefile.WriteLine("$(TARGET):$(SOURCES)");
                Makefile.WriteLine("\t$(CC) $(CFLAGS) -o $(TARGET) -I$(HEADERS) $(SOURCES) 2>&1");
            }

            return MakefilePath;
        }
    }   // class EmscriptenMakefileGenerator
}   // namespace GoddamnEngine.BuildSystem.Emscripten
