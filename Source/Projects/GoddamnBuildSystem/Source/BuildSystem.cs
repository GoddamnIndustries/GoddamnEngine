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
    //[
    //! Represents an exception, thrown by BuildSystem code.
    [Serializable]
    public class BuildSystemException : Exception
    {
        //[
        //! Constructs the build system exception with a string.
        //! @param Format Format string.
        //! @param Arguments Formatting arguments.
        public BuildSystemException(string Format, params object[] Arguments)
            : base(string.Format(Format, Arguments))
        {
        }
    }   // class ProjectException

    //[
    //! Represents a attribute of a build system module.
    [AttributeUsage(AttributeTargets.Class)]
    internal sealed class BuildSystemModuleAttribute : Attribute
    {
        public readonly string CommandLineName;
        public BuildSystemModuleAttribute(string CommandLineName)
        {
            this.CommandLineName = CommandLineName;
        }
    }   // class BuildSystemModuleAttribute

    //[
    //! Represents a module of a build system.
    public abstract class BuildSystemModule
    {
        //[
        //! Entry point of a module.
        public virtual int Execute(string[] Arguments)
        {
            return 0;
        }
    }   // class BuildSystemModule

    //[
    //! Main class of build system.
    public static class BuildSystem
    {
        private static string SDKPath = null;

        //[
        //! Returns path to GoddamnSDK installation location.
        //! @returns Path to GoddamnSDK installation location.
        public static string GetSDKLocation()
        {
            if (SDKPath == null)
            {
                var ExecutableLocation = Environment.CurrentDirectory;
                var ExecutableShouldBe = Path.Combine("Bin", "ThirdParty");
                SDKPath = ExecutableLocation.EndsWith(ExecutableShouldBe, StringComparison.InvariantCultureIgnoreCase) 
                    ? ExecutableLocation.Substring(0, ExecutableLocation.Length - ExecutableShouldBe.Length - 1) : @"D:\GoddamnEngine";
            }
            return SDKPath;
        }

        //[
        //! Application entry point.
        //! @param Arguments Command line arguments.
        private static void Main(string[] Arguments)
        {
            try
            {
                BuildSystemModule ExecutingModule = null;
                string[] ExecutingModuleArguments = null;
                if (Arguments.Length > 0)
                {
                    ExecutingModuleArguments = Arguments.SubArray(1);
                    switch (Arguments[0])
                    {
                        case "--generate-project-files":
                            ExecutingModule = new ProjectGenerator.ProjectGeneratorModule();
                            break;

                        case "--compile-project":
                            ExecutingModule = new ProjectCompiler.ProjectCompilerModule();
                            break;

                        case "--copyright-verify":
                            break;

                        default:
                            ExecutingModuleArguments = Arguments;
                            goto case "--generate-project-files";
                    }
                }
                else
                {
                    ExecutingModule = new ProjectGenerator.ProjectGeneratorModule();
                    ExecutingModuleArguments = Arguments;
                }

                if (ExecutingModule != null) 
                    Environment.Exit(ExecutingModule.Execute(ExecutingModuleArguments));
            }
            catch (BuildSystemException Exception)
            {
                Console.Error.WriteLine("Internal unhanded error was caught while running the Build System:");
                Console.Error.WriteLine(Exception.Message);
            }
            //catch (Exception Exception)
            //{
            //    Console.Error.WriteLine("Unhanded error was caught while running the Build System:");
            //    Console.Error.WriteLine(Exception.ToString());
            //}

            Environment.Exit(1);
        }

    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
