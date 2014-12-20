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
        //! ------------------------------------------------------------------------------------------
        //! @name Internal types.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Interface to description of a single command-line property.
        //! All command-line properties would be stored in 's_CommandLineArguments' variable.
        private abstract class ICommandLinePropertyProxy
        {
            //! Returns name of command-line parameter.
            public abstract string GetName();

            //! Returns description of command-line parameter.
            public abstract string GetDescription();

            //! Returns value of command-line parameter.
            public abstract object GetValue();

            //! Assigns new value to command-line parameter.
            public abstract void SetValue(object Value);

            //! Returns type of command-line parameter.
            public new Type GetType()
            {
                return this.GetValue().GetType();
            }
        }   // interface ICommandLinePropertyProxy

        //! Proxy for a command-line parameter.
        //! @tparam T Real type of parameter
        private sealed class CommandLinePropertyProxy<T> : ICommandLinePropertyProxy
        {
            private T m_Value;
            private string m_Name;
            private string m_Description;

            //! Constructs a proxy variable for command-line parameter.
            //! @param Value 
            //! @param Description 
            public CommandLinePropertyProxy(T Value, String Name, String Description)
            {
                Debug.Assert((typeof(T) == typeof(bool)) || (typeof(T) == typeof(string)) || typeof(T).IsEnum);
                Debug.Assert(!string.IsNullOrWhiteSpace(Name));
                Debug.Assert(!string.IsNullOrWhiteSpace(Description));

                m_Value = Value;
                m_Name = Name;
                m_Description = Description;
                BuildSystem.s_CommandLineProperties.Add(this);
            }

            //! Returns name of command-line parameter.
            public sealed override string GetName()
            {
                return m_Name;
            }

            //! Returns description of command-line parameter.
            public sealed override string GetDescription()
            {
                return m_Description;
            }

            //! Returns value of command-line parameter.
            public sealed override object GetValue()
            {
                return m_Value;
            }

            //! Assigns new value to command-line parameter.
            public sealed override void SetValue(object Value)
            {
                m_Value = (T)Value;
            }

            //! Implicit cast to real type.
            public static implicit operator T(CommandLinePropertyProxy<T> ValueProxy)
            {
                return ValueProxy.m_Value;
            }
        }   // ValueProxy<T>

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Static fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        private static string s_SDKPath = null;
        private static List<ICommandLinePropertyProxy> s_CommandLineProperties = new List<ICommandLinePropertyProxy>();
        private static CommandLinePropertyProxy<bool> s_DoJustPrintHelp = new CommandLinePropertyProxy<bool>(false, "--help", "Prints this help.");
#if INCLUDE_GODDAMNSDK_SPECIFIC
        private static CommandLinePropertyProxy<bool> s_PrintSkippersFromBuildScript = new CommandLinePropertyProxy<bool>(false, "---*", 
            "All arguments, that start from \"---\" are skipped (are referred as setup script parameters).");
#endif  // INCLUDE_GODDAMNSDK_SPECIFIC

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Configuration properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

        // ------------------------------------------------------------------------------------------
        // IsVerboseOutput
        // ------------------------------------------------------------------------------------------

        private static CommandLinePropertyProxy<bool> s_IsVerboseOutput = new CommandLinePropertyProxy<bool>(
            true, "--verbose",
            "If is specified, then all BuildSystem's logs, warnings and errors would be printed to output. " +
            "Disabled by default.");

        //! Is the log output of the build system verbose.
        public static bool GetIsVerboseOutput()
        {
            return s_IsVerboseOutput;
        }

        // ------------------------------------------------------------------------------------------
        // IsMonolithicBuild
        // ------------------------------------------------------------------------------------------

        private static CommandLinePropertyProxy<bool> s_IsMonolithicBuild = new CommandLinePropertyProxy<bool>(
            false, "--monolithic",
            "If is specified, then the whole solution would be compiled into single executable/static library. " +
            "Disabled by default.");

        //! Is the engine built into single executable/static library.
        public static bool GetIsMonolithicBuild()
        {
            return s_IsVerboseOutput;
        }

        // ------------------------------------------------------------------------------------------
        // SolutionPath
        // ------------------------------------------------------------------------------------------
        private static CommandLinePropertyProxy<string> s_SolutionPath = new CommandLinePropertyProxy<string>(
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

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Command-line interface.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Prints a line into specified output in specified color.
        private static void PrintLineWithColor(ConsoleColor Color, TextWriter Output, string Message, params object[] Args)
        {
            ConsoleColor OldColor = Console.ForegroundColor;
            Console.ForegroundColor = Color;
            if (s_IsVerboseOutput) {
                Output.WriteLine(Message, Args);
            }

            Console.ForegroundColor = OldColor;
        }

        //! Prints a message string into stdout.
        public static void PrintMessage(string Message, params object[] Args)
        {
            BuildSystem.PrintLineWithColor(Console.ForegroundColor, Console.Out, Message, Args);
        }

        //! Prints a warning string into stdout.
        public static void PrintWarning(string Message, params object[] Args)
        {
            BuildSystem.PrintLineWithColor(ConsoleColor.Yellow, Console.Out, Message, Args);
        }

        //! Prints a error string into stderr.
        public static void PrintError(string Message, params object[] Args)
        {
            BuildSystem.PrintLineWithColor(ConsoleColor.Red, Console.Error, Message, Args);
        }

        //! Prints help to command-line interface of the BuildSystem.
        private static void PrintHelp()
        {
            string BuildSystemExecutableName = Path.GetFileName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);

            // Printing copyright.
            Console.WriteLine("GoddamnBuildSystem project/solution files generation system.");
            Console.WriteLine("Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.");
            Console.WriteLine();

            // Printing properties.
            Console.WriteLine("Usage: {0} [properties...]", BuildSystemExecutableName);
            Console.WriteLine(" Properties:");
            foreach (var CommandLineProperty in s_CommandLineProperties) {
                // Creating padding to each property.
                var CommandLinePropertyKey = string.Format("  {0,-12} -", CommandLineProperty.GetName());
                var CommandLinePropertyPadding = new string(' ', CommandLinePropertyKey.Length - 1);
                Console.Write(CommandLinePropertyKey);

                // Generating full description for enum elements.
                string CommandLinePropertyDescription = null;
                if (CommandLineProperty.GetType().IsEnum) {
                    CommandLinePropertyDescription = new StringBuilder()
                        .Append(CommandLineProperty.GetDescription())
                        .Append(" Supported values are: ")
                        .Append(string.Join(", ", CommandLineProperty.GetType().GetEnumNames().SubArray(1)))
                        .Append('.').ToString();
                } else {
                    CommandLinePropertyDescription = CommandLineProperty.GetDescription();
                }

                // Splitting all description to match padding.
                int CommandLinePropertyPartLengthInit = Console.WindowWidth - CommandLinePropertyPadding.Length - 1;
                if (CommandLinePropertyDescription.Length >= CommandLinePropertyPartLengthInit) {
                    while (CommandLinePropertyDescription.Length > CommandLinePropertyPartLengthInit) {
                        int CommandLinePropertyDescPartLength = CommandLinePropertyPartLengthInit;
                        do {
                            CommandLinePropertyDescPartLength -= 1;
                        } while (" ,.:!?\t\n\r".IndexOf(CommandLinePropertyDescription[CommandLinePropertyDescPartLength]) == -1);

                        Console.Write("{0}\n{1}", CommandLinePropertyDescription.Substring(0, CommandLinePropertyDescPartLength), CommandLinePropertyPadding);
                        CommandLinePropertyDescription = CommandLinePropertyDescription.Substring(CommandLinePropertyDescPartLength + 1).TrimStart();
                    }
                }

                Console.WriteLine(CommandLinePropertyDescription);
            }
            Console.WriteLine();

            // Printing examples.
            Console.WriteLine("Examples:");
            Console.WriteLine(" {0} --platform=Windows --ide=QtCreator Game.gdsln.cs", BuildSystemExecutableName);
            Console.WriteLine(" {0} --monolithic --platform=iOS Game.gdsln.cs", BuildSystemExecutableName);
        }

        //! Parses command-line arguments.
        private static void ParseCommandLineArguments(string[] Arguments)
        {
            foreach (string CommandLineProperty in Arguments) {
#if INCLUDE_GODDAMNSDK_SPECIFIC
                if (CommandLineProperty.StartsWith("---", StringComparison.InvariantCultureIgnoreCase)) {
                    continue;
                }
#endif  // if INCLUDE_GODDAMNSDK_SPECIFIC

                ICommandLinePropertyProxy CommandLinePropertyProxy = s_CommandLineProperties.Find(
                    P => (P.GetName() == CommandLineProperty) || ((P.GetType() != typeof(bool)) && CommandLineProperty.StartsWith(P.GetName(), StringComparison.InvariantCultureIgnoreCase)));
                if (CommandLinePropertyProxy != null) {
                    // If we are just supposed to print help there is no need in other arguments parsing.
                    if (CommandLinePropertyProxy == s_DoJustPrintHelp) {
                        BuildSystem.PrintHelp();
                        Environment.Exit(0);
                    }

                    if (CommandLinePropertyProxy.GetType() == typeof(bool)) {
                        CommandLinePropertyProxy.SetValue(true);
                    } else {
                        int CommandLinePropertyValueSeparatorIndex = CommandLineProperty.IndexOf('=');
                        if (CommandLinePropertyValueSeparatorIndex == -1) {
                            throw new BuildSystemException("Property {0} requires a value of type {1}.", CommandLineProperty, CommandLinePropertyProxy.GetType().Name);
                        }

                        string CommandLinePropertyValue = CommandLineProperty.Substring(CommandLinePropertyValueSeparatorIndex + 1);
                        if (CommandLinePropertyProxy.GetType() == typeof(string)) {
                            if (CommandLinePropertyValue[0] == '"') {
                                CommandLinePropertyValue = CommandLinePropertyValue.Substring(1);
                            }
                            if (CommandLinePropertyValue[CommandLinePropertyValue.Length - 1] == '"') {
                                CommandLinePropertyValue = CommandLinePropertyValue.Substring(0, CommandLinePropertyValue.Length - 1);
                            }

                            CommandLinePropertyProxy.SetValue(CommandLinePropertyValue);
                        } else if (CommandLinePropertyProxy.GetType().IsEnum) {
                            try {
                                Enum CommandLinePropertyValueEnum = (Enum)Enum.Parse(CommandLinePropertyProxy.GetType(), CommandLinePropertyValue, true);
                                int CommandLinePropertyValueInt = (byte)Convert.ChangeType(CommandLinePropertyValueEnum, CommandLinePropertyValueEnum.GetTypeCode());
                                if (CommandLinePropertyValueInt == 0) {
                                    throw new ArgumentException("'Unknown' value specified to type.");
                                }

                                CommandLinePropertyProxy.SetValue(CommandLinePropertyValueEnum);
                            } catch (ArgumentException) {
                                throw new BuildSystemException("Property {0} has an invalid value specified {1}.", CommandLineProperty, CommandLinePropertyValue);
                            }
                        } else {
                            throw new NotImplementedException();
                        }
                    }
                } else {
                    throw new BuildSystemException("Unknown property {0} specified.", CommandLineProperty);
                }
            }
        }

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Environmental properties.
        //! @{
        //! ------------------------------------------------------------------------------------------

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

        //! ------------------------------------------------------------------------------------------
        //! @name Entry point.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Application entry point.
        private static void Main(string[] Arguments)
        {
            try {
                BuildSystem.ParseCommandLineArguments(Arguments);

                TargetMakefileGenerator MakefileGenerator = TargetMakefileGeneratorFactory.Create(TargetPlatform.Emscripten);
                ProjectCache Project = ProjectFactory.Create(@"D:\GoddamnEngine\source\Projects\GoddamnCore\GoddamnCore.gdproj.cs");
                MakefileGenerator.GenerateMakefile(Project);
                //TargetIDEGenerator TargetObject = TargetIDEGeneratorFactory.Create();
                //SolutionCache Solution = SolutionFactory.Create(@"D:\GoddamnEngine\source\GoddamnEngine.gdsln.cs");
                //foreach (ProjectCache Project in Solution.m_CachedProjects) {
                //    if (!Project.m_IsBuildTool) {
                //        foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                //            if (TargetObject.IsPlatformNativelySupported(Platform)) {
                //                continue;
                //            }
                //        }

                //        Project.m_GeneratorOutputPath = TargetObject.GenerateProjectFiles(Project);
                //    }
                //}
                //Solution.m_GeneratedSolutionPath = TargetObject.GenerateSolutionFiles(Solution);

                Environment.Exit(0);
            } catch (BuildSystemException Exception) {
                // Internal error in code.
                Console.Error.WriteLine(Exception.Message);
#if !DEBUG
            } catch (Exception Exception) {
                Console.Error.WriteLine("Unhanded error caught while generation project files. Sorry :(");
                Console.Error.WriteLine(Exception.Message);
#endif  // !DEBUG
            }

            Environment.Exit(1);
        }

        //! @}
    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
