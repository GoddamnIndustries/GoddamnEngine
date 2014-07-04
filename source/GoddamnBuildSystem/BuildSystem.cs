using System;
using System.IO;

namespace GoddamnEngine.BuildSystem
{
    public sealed class BuildSystem
    {
        private static string _SDKPath = null;
        public static string SDKPath 
        { 
            get 
            {
                if (BuildSystem._SDKPath == null)
                {
                    BuildSystem._SDKPath = Environment.GetEnvironmentVariable("GODDAMN_SDK");
                    if (BuildSystem._SDKPath == null)
                        BuildSystem._SDKPath = Path.Combine(Path.Combine(Environment.CurrentDirectory, ".."), "..") + Path.DirectorySeparatorChar;
                }

                return BuildSystem._SDKPath;
            } 
        }

        static void Main(string[] args)
        {
        //  CommandLineArguments.PrintHelp();
            string SolutionLocation = @"../../";
            Project.ProcessProjectsInDirectory(SolutionLocation);
        }
    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
