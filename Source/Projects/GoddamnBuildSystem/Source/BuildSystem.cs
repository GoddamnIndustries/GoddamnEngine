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
    //! @brief Represents an exception, thrown by BuildSystem code.
    [Serializable]
    public class BuildSystemException : Exception
    {
        //! @brief Constructs the build system exception with a String.
        //! @param Format Format String.
        //! @param Arguments Formatting arguments.
        public BuildSystemException(String Format, params object[] Arguments)
            : base(String.Format(Format, Arguments))
        {
        }
    }   // class ProjectException

    //! @brief Represents a attribute of a build system module.
    [AttributeUsage(AttributeTargets.Class)]
    internal sealed class BuildSystemModuleAttribute : Attribute
    {
        public readonly String m_CommandLineName;
        public BuildSystemModuleAttribute(String m_CommandLineName)
        {
            this.m_CommandLineName = m_CommandLineName;
        }
    }   // class BuildSystemModuleAttribute

    //! @brief Represents a module of a build system.
    public abstract class BuildSystemModule
    {
        //! @brief Entry point of a module.
        public virtual int Execute(String[] Arguments)
        {
            return 0;
        }
    }   // class BuildSystemModule

    //! @brief Main class of build system.
    public static class BuildSystem
    {
        private static String s_SDKPath = null;

        //! @brief Returns path to GoddamnSDK installation location.
        //! @returns Path to GoddamnSDK installation location.
        public static String GetSDKLocation()
        {
            if (s_SDKPath == null)
            {
                String ExecutableLocation = Environment.CurrentDirectory;
                String ExecutableShouldBe = Path.Combine("Bin", "ThirdParty");
                if (ExecutableLocation.EndsWith(ExecutableShouldBe, StringComparison.InvariantCultureIgnoreCase))
                {
                    s_SDKPath = ExecutableLocation.Substring(0, ExecutableLocation.Length - ExecutableShouldBe.Length - 1);
                }
                else
                {
                    s_SDKPath = @"D:\GoddamnEngine";
                    //throw new BuildSystemException("Unable to determine where build system is located relatively to Goddamn SDK root.");
                }
            }

            return s_SDKPath;
        }

        //! @brief Application entry point.
        private static void Main(String[] Arguments)
        {
            try
            {
                BuildSystemModule ExecutingModule = null;
                String[] ExecutingModuleArguments = null;
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

                Environment.Exit(ExecutingModule.Execute(ExecutingModuleArguments));
            }
            catch (BuildSystemException Exception)
            {
                Console.Error.WriteLine("Internal unhanded error was caught while running the Build System:");
                Console.Error.WriteLine(Exception.Message);
            }
            catch (Exception Exception)
            {
                Console.Error.WriteLine("Unhanded error was caught while running the Build System:");
                Console.Error.WriteLine(Exception.ToString());
            }

            Environment.Exit(1);
        }

    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
