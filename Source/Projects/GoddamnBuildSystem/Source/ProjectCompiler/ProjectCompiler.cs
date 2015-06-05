// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file ProjectCompiler.cs
//! Project compiler interface.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;

namespace GoddamnEngine.BuildSystem.ProjectCompiler
{
    //! Represents list of compiler commands.
    public enum ProjectCompilerCommand : byte
    {
        Unknown,
        Build,
        Rebuild,
        Clean,
    }   // enum ProjectCompilerCommand

    //! Represents an exception, thrown by ProjectCompiler code.
    [Serializable]
    public sealed class ProjectCompilerException : BuildSystemException
    {
        //! Constructs the build system exception with a string.
        //! @param Format Format string.
        //! @param Arguments Formatting arguments.
        public ProjectCompilerException(string Format, params object[] Arguments)
            : base(Format, Arguments)
        {
        }
    }   // class ProjectCompilerException

    //! Project file generation module.
    [BuildSystemModule("--generate-project")]
    public sealed class ProjectCompilerModule : BuildSystemModule
    {
        //! Handles input of some Clang/GCC-like compiler and converts it into VS format.
        //! @param Sender Unused.
        //! @param ReceivedOutputData Object with output data.
        private static void CompileOutputReceivedDataEventHandler(object Sender, DataReceivedEventArgs ReceivedOutputData)
        {
            string ReceivedOutput = ReceivedOutputData.Data;
            if (!string.IsNullOrEmpty(ReceivedOutput))
            {
                ReceivedOutput = ReceivedOutput.Replace("\\", "/");

                string MatchDescription = Regex.Match(ReceivedOutput, @"(\serror:\s|\swarning:\s|\snote:\s).*").Value;
                string MatchLineNumber = Regex.Match(ReceivedOutput, @"\:\d+\:\d+\:").Value;
                string MatchFilePath = Regex.Match(ReceivedOutput, @"^([\/A-Za-z0-9_\-\.]*)+\.(c|cc|cpp|cxx|h|hh|hpp|hxx|m|mm|inl)").Value;
                if (string.IsNullOrEmpty(MatchFilePath))
                {
                    MatchFilePath = Regex.Match(ReceivedOutput, @"^([A-Z]:[\/A-Za-z0-9_\-\.]*)+\.(c|cc|cpp|cxx|h|hh|hpp|hxx|m|mm|inl)").Value;
                }

                // If any of the above matches failed, do nothing
                if (!(string.IsNullOrEmpty(MatchDescription) || string.IsNullOrEmpty(MatchLineNumber) || string.IsNullOrEmpty(MatchFilePath)))
                {
                    // Stripping path & extracting line & column number..
                    string ConvertedFilePath = Regex.Match(MatchFilePath, @"(Source\/|[A-Za-z0-9_\-\.]*\/).*").Value;
                    string ConvertedLineNumber = Regex.Match(MatchLineNumber, @"\d+").Value;
                    string ConvertedColumnNumber = Regex.Match(MatchLineNumber, @"(?<=:\d+:)\d+").Value;

                    // Writing output..
                    Console.Out.WriteLine("\t{0}({1},{2}):{3}.", ConvertedFilePath, ConvertedLineNumber, ConvertedColumnNumber, MatchDescription);
                }
                else
                {
                    Console.Out.WriteLine(ReceivedOutput);
                }
            }
        }

        //! Compiles project files.
        public sealed override int Execute(string[] Arguments)
        {
            try
            {
                string ProjectFilePath;
                TargetPlatform Platform;
                TargetConfiguration Configuration;
                ProjectCompilerCommand Command;

                try
                {
                    ProjectFilePath = Arguments[0];
                    Platform = (TargetPlatform)Enum.Parse(typeof(TargetPlatform), Arguments[1]);
                    Configuration = (TargetConfiguration)Enum.Parse(typeof(TargetConfiguration), Arguments[2]);
                    Command = Arguments.Length > 3 ? (ProjectCompilerCommand)Enum.Parse(typeof(ProjectCompilerCommand), Arguments[3]) : ProjectCompilerCommand.Build;
                }
                catch (Exception)
                {
                    throw new ProjectCompilerException("Invalid arguments passed to project compiler module.");
                }

                TargetPlatformInfo PlatformInfo = TargetPlatformInfo.Get(Platform);
                TargetConfigurationInfo ConfigurationInfo = TargetConfigurationInfo.Get(Configuration);
                ProjectCache Project = ProjectFactory.Create(ProjectFilePath);

                List<ProjectBuildableSourceFile> AllSourceFiles = new List<ProjectBuildableSourceFile>();
                List<ProjectBuildableSourceFile> BuildableSourceFiles = new List<ProjectBuildableSourceFile>();
                foreach (var ProjectSource in Project.CachedSourceFiles)
                {
                    AllSourceFiles.Add(new ProjectBuildableSourceFile(ProjectSource));
                    if ((ProjectSource.FileType == ProjectSourceFileType.SourceCode) && (!ProjectSource.ShouldBeExcluded(Platform, Configuration)))
                    {
                        BuildableSourceFiles.Add(new ProjectBuildableSourceFile(ProjectSource));
                    }
                }

                Console.Out.WriteLine("==========================================================================================");
                Console.Out.WriteLine("Compiling:\n\tProject: {0},\n\tPlatform: {1},\n\tConfiguration: {2}...", ProjectFilePath, Platform, Configuration);
                Console.Out.WriteLine("==========================================================================================");
                Console.Out.WriteLine();

                Console.Out.WriteLine("Compiling project files..");

                Console.Out.WriteLine("Compiling source files..");

                // Building command line arguments of the compiler.
                StringBuilder CompilerArguments = new StringBuilder();
                CompilerArguments.Append(" -Wno-warn-absolute-paths");
                CompilerArguments.Append(" -std=c++11");
                if (ConfigurationInfo.Optimize)
                {
                    CompilerArguments.Append(" -O3");
                }
                if (!PlatformInfo.RequiresRTTI)
                {
                    CompilerArguments.Append(" -fno-rtti");
                }

                // Adding header files..
                CompilerArguments.Append(" -I").Append(Project.GenerateIncludePaths(" -I"));

                // Specifing output paths..
                CompilerArguments.Append(" -o ").Append(Project.CachedOutputPaths[Platform, Configuration]);

                // Specifing dependencies paths..
                foreach (var ProjectDependency in Project.CachedDependencies)
                {
                    foreach (var ProjectDependencyPath in ProjectDependency.CachedLinkedLibraries[Platform, Configuration])
                    {
                        CompilerArguments.Append(" \"").Append(ProjectDependencyPath).Append('\"');
                        //CompilerArguments.Append(@" D:/GoddamnEngine/bin/GoddamnCore.Debug.bc");
                        //goto Outer;
                    }
                }
                //Outer:

                bool CompilationSuccessful = true;


                Console.Out.WriteLine("==========================================================================================");
                Console.Out.WriteLine("./" + CompilerArguments.ToString());
                Console.Out.WriteLine("==========================================================================================");

                ProcessStartInfo CompilerProcessInfo = new ProcessStartInfo();
                CompilerProcessInfo.WindowStyle = ProcessWindowStyle.Hidden;
                CompilerProcessInfo.UseShellExecute = false;
                CompilerProcessInfo.RedirectStandardError = true;
                CompilerProcessInfo.RedirectStandardOutput = true;
                CompilerProcessInfo.FileName = PlatformInfo.CompilerPath;
                CompilerProcessInfo.WorkingDirectory = Environment.CurrentDirectory;
                CompilerProcessInfo.Arguments = CompilerArguments.ToString();

                Process CompilerProcess = new Process();
                CompilerProcess.EnableRaisingEvents = true;
                CompilerProcess.StartInfo = CompilerProcessInfo;
                CompilerProcess.OutputDataReceived += new DataReceivedEventHandler(CompileOutputReceivedDataEventHandler);
                CompilerProcess.ErrorDataReceived += new DataReceivedEventHandler(CompileOutputReceivedDataEventHandler);
                CompilerProcess.Start();
                CompilerProcess.BeginOutputReadLine();
                CompilerProcess.BeginErrorReadLine();
                CompilerProcess.WaitForExit();

                CompilationSuccessful = CompilationSuccessful && (CompilerProcess.ExitCode == 0);
                Console.Out.WriteLine();

                return CompilationSuccessful ? 0 : 1;
            }
            catch (ProjectCompilerException Exception)
            {
                Console.Error.WriteLine("Unhanded error was caught while running the Project Compiler Module:");
                Console.Error.WriteLine(Exception.ToString());

                return 1;
            }
        }
    }   // class ProjectCompilerModule
}   // namespace GoddamnEngine.BuildSystem.ProjectCompiler
