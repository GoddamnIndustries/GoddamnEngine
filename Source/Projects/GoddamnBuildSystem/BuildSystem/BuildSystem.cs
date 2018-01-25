// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.IO;
using GoddamnEngine.BuildSystem.Support;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Represents an exception thrown by BuildSystem code.
    /// </summary>
    [Serializable]
    public class BuildSystemException : Exception
    {
        /// <summary>
        /// Constructs the build system exception with a string.
        /// </summary>
        /// <param name="format">The format string.</param>
        /// <param name="arguments">Formatting arguments.</param>
        public BuildSystemException(string format, params object[] arguments) 
            : base(string.Format(format, arguments))
        {
        }
    }   // class ProjectException

    /// <summary>
    /// Represents a attribute of a build system module.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class)]
    internal sealed class BuildSystemModuleAttribute : Attribute
    {
        public readonly string CommandLineName;
        public BuildSystemModuleAttribute(string commandLineName)
        {
            CommandLineName = commandLineName;
        }
    }   // class BuildSystemModuleAttribute

    /// <summary>
    /// Represents a module of a build system.
    /// </summary>
    public abstract class BuildSystemModule
    {
        /// <summary>
        /// Entry point of a module.
        /// </summary>
        /// <param name="arguments">Command line arguments.</param>
        /// <returns></returns>
        public virtual int Execute(string[] arguments)
        {
            return 0;
        }
    }   // class BuildSystemModule

    /// <summary>
    /// Main class of build system.
    /// </summary>
    public static class BuildSystem
    {
        private static string s_SdkPath;

        /// <summary>
        /// Returns path to GoddamnSDK installation location.
        /// </summary>
        public static string GetSdkLocation()
        {
            if (s_SdkPath != null)
            {
                return s_SdkPath;
            }

            var executableLocation = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            var executableShouldBe = Path.Combine("Bin", "Tools");
            if (executableLocation == null || !executableLocation.EndsWith(executableShouldBe, StringComparison.InvariantCultureIgnoreCase))
            {
                throw new BuildSystemException("Invalid GoddamnBuildSystemExecutable location {0}", executableLocation);
            }
            return s_SdkPath = Path.GetFullPath(Path.Combine(executableLocation, "..", ".."));
        }

        /// <summary>
        /// Application entry point.
        /// </summary>
        /// <param name="arguments">Command line arguments.</param>
        private static void Main(string[] arguments)
        {
            try
            {
                BuildSystemModule executingModule;
                string[] executingModuleArguments;
                if (arguments.Length > 0)
                {
                    executingModuleArguments = arguments.SubArray(1);
                    switch (arguments[0])
                    {
                        case "--generate-project-files":
                            executingModule = new ProjectGenerator.ProjectGeneratorModule();
                            break;

                        default:
                            executingModuleArguments = arguments;
                            goto case "--generate-project-files";
                    }
                }
                else
                {
                    executingModule = new ProjectGenerator.ProjectGeneratorModule();
                    executingModuleArguments = arguments;
                }

                Environment.Exit(executingModule.Execute(executingModuleArguments));
            }
            catch (BuildSystemException exception)
            {
                Console.Error.WriteLine("Internal unhanded error was caught while running the Build System:");
                Console.Error.WriteLine(exception.Message);
            }
#if !DEBUG
            // We need Visual Studio to debug break when the exception is thrown, not just silently shut down.
            catch (Exception Exception)
            {
                Console.Error.WriteLine("Unhanded error was caught while running the Build System:");
                Console.Error.WriteLine(Exception.ToString());
            }
#endif  // if !DEBUG
            Environment.Exit(1);
        }

    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
