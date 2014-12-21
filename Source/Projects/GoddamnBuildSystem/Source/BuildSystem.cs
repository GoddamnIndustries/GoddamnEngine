//! ==========================================================================================
//! BuildSystem.cs - GoddamnBuildSystem main source module.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    //! Represents an exception, thrown by BuildSystem code.
    [Serializable]
    public sealed class BuildSystemException : Exception
    {
        //! Constructs the build system exception with a string.
        //! @param Format Format string.
        //! @param Arguments Formatting arguments.
        public BuildSystemException(string Format, params object[] Arguments)
            : base(string.Format(Format, Arguments)) { }
    }   // class ProjectException

    //! Main class of build system.
    public static class BuildSystem
    {
        private static string s_SDKPath = null;

        private readonly static CommandLinePropertyProxy<bool> s_IsVerboseOutput = new CommandLinePropertyProxy<bool>(
            true, "--verbose",
            "If is specified, then all BuildSystem's logs, warnings and errors would be printed to output. " +
            "Disabled by default.");

        //! Is the log output of the build system verbose.
        public static bool GetIsVerboseOutput()
        {
            return s_IsVerboseOutput;
        }

        private readonly static CommandLinePropertyProxy<bool> s_IsMonolithicBuild = new CommandLinePropertyProxy<bool>(
            false, "--monolithic",
            "If is specified, then the whole solution would be compiled into single executable/static library. " +
            "Disabled by default.");

        //! Is the engine built into single executable/static library.
        public readonly static bool GetIsMonolithicBuild()
        {
            if (s_IsMonolithicBuild) {
                throw new NotImplementedException();
            }

            return s_IsMonolithicBuild;
        }

        private readonly static CommandLinePropertyProxy<string> s_SolutionPath = new CommandLinePropertyProxy<string>(
            string.Empty, "--solution",
            "Path to solution file for which generation is provided. " +
            "By default, it is determined by the GoddamnSDK installation location.");

        //! Returns path to solution file for which generation is provided.
        public static string GetSolutionPath()
        {
            if (s_SolutionPath.GetValue() == null) {
                s_SolutionPath.SetValue(Path.Combine(BuildSystem.GetSolutionPath(), "source", "GoddamnEngine.gdsln.cs"));
            }

            return s_SolutionPath;
        }

        //! Path to GoddamnSDK installation location.
        public static string GetSDKLocation()
        {
            if (s_SDKPath == null) {
                s_SDKPath = @"D:\GoddamnEngine";
                //s_SDKPath = Path.GetDirectoryName(Environment.GetCommandLineArgs()[0]);
                //string BuildSystemLocationInsideSDK = Path.Combine("GoddamnEngine", "bin", "ThirdParty");
                //if (!s_SDKPath.EndsWith(BuildSystemLocationInsideSDK)) {
                //    throw new BuildSystemException("Environmental variable 'GODDAMN_SDK' was not set. Is the SDK installed correctly?");
                //}

                //s_SDKPath = s_SDKPath.Substring(0, s_SDKPath.Length - BuildSystemLocationInsideSDK.Length);
            }

            return s_SDKPath;
        }

        //! @}

        //! Application entry point.
        private static void Main(string[] Arguments)
        {
            try {
                CommandLineUI.ParseCommandLineArguments(Arguments);

                //TargetMakefileGenerator MakefileGenerator = TargetMakefileGeneratorFactory.Create(TargetPlatform.Emscripten);
                //ProjectCache Project = ProjectFactory.Create(@"D:\GoddamnEngine\source\Projects\GoddamnCore\GoddamnCore.gdproj.cs");
                //MakefileGenerator.GenerateMakefile(Project);

                TargetProjectGenerator ProjectGenerator = TargetProjectGeneratorFactory.Create();
                SolutionCache Solution = SolutionFactory.Create(@"D:\GoddamnEngine\source\GoddamnEngine.gdsln.cs");
                foreach (ProjectCache Project in Solution.m_CachedProjects) {
                    if (!Project.m_IsBuildTool) {
                        foreach (TargetPlatforms Platform in Target.EnumerateAllPlatforms()) {
                            if (ProjectGenerator.IsPlatformNativelySupported(Platform)) {
                                continue;
                            }

                            // Generate makefiles here..
                            Project.m_MakefilePathes.Add(Platform, null);
                        }

                        Project.m_GeneratorOutputPath = ProjectGenerator.GenerateProjectFiles(Project);
                    }
                }
                Solution.m_GeneratedSolutionPath = ProjectGenerator.GenerateSolutionFiles(Solution);

                Environment.Exit(0);
            } catch (BuildSystemException Exception) {
                // Internal error in code.
                Console.Error.WriteLine(Exception.Message);
#if RELEASE  // We want fatal exception been caught in release modes only..
            } catch (Exception Exception) {
                Console.Error.WriteLine("Unhanded error caught while generation project files. Sorry :(");
                Console.Error.WriteLine(Exception.Message);
#endif  // RELEASE
            }

            Environment.Exit(1);
        }

        //! @}
    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
