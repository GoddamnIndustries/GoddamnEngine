// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file BuildSystem.cs
//! GoddamnBuildSystem main source module.

using System;
using System.IO;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! Represents an exception, thrown by BuildSystem code.
    [Serializable]
    public class BuildSystemException : Exception
    {
        // ------------------------------------------------------------------------------------------
        //! Constructs the build system exception with a string.
        //! @param Format the format string.
        //! @param arguments Formatting arguments.
        public BuildSystemException(string format, params object[] arguments)
            : base(string.Format(format, arguments))
        {
        }
    }   // class ProjectException

    // ------------------------------------------------------------------------------------------
    //! Represents a attribute of a build system module.
    [AttributeUsage(AttributeTargets.Class)]
    internal sealed class BuildSystemModuleAttribute : Attribute
    {
        public readonly string CommandLineName;
        public BuildSystemModuleAttribute(string commandLineName)
        {
            CommandLineName = commandLineName;
        }
    }   // class BuildSystemModuleAttribute

    // ------------------------------------------------------------------------------------------
    //! Represents a module of a build system.
    public abstract class BuildSystemModule
    {
        // ------------------------------------------------------------------------------------------
        //! Entry point of a module.
        public virtual int Execute(string[] arguments)
        {
            return 0;
        }
    }   // class BuildSystemModule

    // ------------------------------------------------------------------------------------------
    //! Main class of build system.
    public static class BuildSystem
    {
        private static string _sdkPath;

        // ------------------------------------------------------------------------------------------
        //! Returns path to GoddamnSDK installation location.
        //! @returns Path to GoddamnSDK installation location.
        public static string GetSdkLocation()
        {
            if (_sdkPath != null) return _sdkPath;
            var executableLocation = Environment.CurrentDirectory;
            var executableShouldBe = Path.Combine("Bin", "ThirdParty");
            _sdkPath = executableLocation.EndsWith(executableShouldBe, StringComparison.InvariantCultureIgnoreCase) 
                ? executableLocation.Substring(0, executableLocation.Length - executableShouldBe.Length - 1) : @"D:\GoddamnEngine";
            return _sdkPath;
        }

        // ------------------------------------------------------------------------------------------
        //! Application entry point.
        //! @param arguments Command line arguments.
        private static void Main(string[] arguments)
        {
            try
            {
                BuildSystemModule executingModule = null;
                string[] executingModuleArguments;
                if (arguments.Length > 0)
                {
                    executingModuleArguments = arguments.SubArray(1);
                    switch (arguments[0])
                    {
                        case "--generate-project-files":
                            executingModule = new ProjectGenerator.ProjectGeneratorModule();
                            break;

                        case "--compile-project":
                            executingModule = new ProjectCompiler.ProjectCompilerModule();
                            break;

                        case "--copyright-verify":
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

                if (executingModule != null) 
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
