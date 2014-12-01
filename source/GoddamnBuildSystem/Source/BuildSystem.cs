/// ==========================================================================================
/// BuildSystem.cs - GoddamnBuildSystem main source module.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2014 - Created by James Jhuighuy
///     * 24.11.2014 - Refactored.
/// ==========================================================================================

using System;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Represents an exception, thrown by BuildSystem code.
    /// </summary>
    public sealed class BuildSystemException : Exception
    {
        public BuildSystemException(string Format, params object[] Arguments) : base(string.Format(Format, Arguments)) { }
    }   // class ProjectException

    /// <summary>
    /// Main class of build system.
    /// </summary>
    public static class BuildSystem
    {
        //! ------------------------------------------------------------------------------------------
        //! Internal types.
        //! ------------------------------------------------------------------------------------------
        #region

        /// <summary>
        /// Interface to description of a single command-line property.
        /// All command-line properties would be stored in 's_CommandLineArguments' variable.
        /// </summary>
        private abstract class ICommandLinePropertyProxy
        {
            /// <summary>
            /// Returns name of command-line parameter.
            /// </summary>
            public abstract string GetName();

            /// <summary>
            /// Returns description of command-line parameter.
            /// </summary>
            public abstract string GetDescription();

            /// <summary>
            /// Returns value of command-line parameter.
            /// </summary>
            public abstract object GetValue();

            /// <summary>
            /// Assigns new value to command-line parameter.
            /// </summary>
            public abstract void SetValue(object Value);

            /// <summary>
            /// Returns type of command-line parameter.
            /// </summary>
            public new Type GetType()
            {
                return this.GetValue().GetType();
            }
        }   // interface ICommandLinePropertyProxy

        /// <summary>
        /// Proxy for a command-line parameter.
        /// </summary>
        /// <typeparam name="T">Real type of parameter</typeparam>
        private sealed class CommandLinePropertyProxy<T> : ICommandLinePropertyProxy
        {
            private string m_Name;
            private string m_Description;
            private T m_Value;

            /// <summary>
            /// Constructs a proxy variable for command-line parameter.
            /// </summary>
            /// <param name="Value"></param>
            /// <param name="Description"></param>
            public CommandLinePropertyProxy(T Value, String Name, String Description)
            {
                Debug.Assert((typeof(T) == typeof(bool)) || (typeof(T) == typeof(string)) || typeof(T).IsEnum);
                Debug.Assert(!string.IsNullOrWhiteSpace(Name));
                Debug.Assert(!string.IsNullOrWhiteSpace(Description));

                m_Name = Name;
                m_Description = Description;
                m_Value = Value;
                BuildSystem.s_CommandLineProperties.Add(this);
            }

            /// <summary>
            /// Returns name of command-line parameter.
            /// </summary>
            public sealed override string GetName()
            {
                return m_Name;
            }

            /// <summary>
            /// Returns description of command-line parameter.
            /// </summary>
            public sealed override string GetDescription()
            {
                return m_Description;
            }

            /// <summary>
            /// Returns value of command-line parameter.
            /// </summary>
            public sealed override object GetValue()
            {
                return m_Value;
            }

            /// <summary>
            /// Assigns new value to command-line parameter.
            /// </summary>
            public sealed override void SetValue(object Value)
            {
                m_Value = (T)Value;
            }

            /// <summary>
            /// Implicit cast to real type.
            /// </summary>
            public static implicit operator T(CommandLinePropertyProxy<T> ValueProxy)
            {
                return ValueProxy.m_Value;
            }
        }   // ValueProxy<T>

        #endregion

        //! ------------------------------------------------------------------------------------------
        //! Static fields.
        //! ------------------------------------------------------------------------------------------
        #region

        private static string s_SDKPath = null;
        private static List<ICommandLinePropertyProxy> s_CommandLineProperties = new List<ICommandLinePropertyProxy>();

        #endregion

        //! ------------------------------------------------------------------------------------------
        //! Command-line parameters.
        //! ------------------------------------------------------------------------------------------
        #region

        private static CommandLinePropertyProxy<bool> s_DoJustPrintHelp = new CommandLinePropertyProxy<bool>(
            false, "--help",
            "Prints this help.");

        private static CommandLinePropertyProxy<bool> s_IsVerboseOutput = new CommandLinePropertyProxy<bool>(
#if DEBUG
            // Force verbose output for debug builds.
            true,
#else   // if DEBUG
            false, 
#endif  // if DEBUG
            "--verbose",
            "If is specified, then all BuildSystem's logs, warnings and errors would be printed to output. " +
            "Disabled by default.");
        
        private static CommandLinePropertyProxy<bool> s_IsMonolithicBuild = new CommandLinePropertyProxy<bool>(
            false, "--monolithic",
            "If is specified, then the whole solution would be compiled into sinlge executable/static library. " +
            "Disabled by default.");
        
        private static CommandLinePropertyProxy<TargetPlatform> s_Platform = new CommandLinePropertyProxy<TargetPlatform>(
            TargetPlatform.Unknown, "--platform",
            "Target platform for which project file would be generated. " + 
            "By default, it is determined by host OS.");
        
        private static CommandLinePropertyProxy<TargetProjectCompiler> s_ProjectCompiler = new CommandLinePropertyProxy<TargetProjectCompiler>(
            TargetProjectCompiler.Unknown, "--ide",
            "Target compiler/IDE for which project files would be generated. " + 
            "By default, it is determined by the target platform/IDE installation.");

        private static CommandLinePropertyProxy<string> s_SolutionPath = new CommandLinePropertyProxy<string>(
            null, "--solution",
            "Path to solution file for which generation is provided. " +
            "By default, it is determined by the GoddamnSDK installation location.");

        #endregion

        //! ------------------------------------------------------------------------------------------
        //! Configuration properties.
        //! ------------------------------------------------------------------------------------------
        #region

        /// <summary>
        /// Is the log output of the build system verbose.
        /// </summary>
        public static bool GetIsVerboseOutput()
        {
            return s_IsVerboseOutput;
        }

        /// <summary>
        /// Is the engine built into single executable/static library.
        /// </summary>
        public static bool GetIsMonolithicBuild()
        {
            return s_IsVerboseOutput;
        }

        /// <summary>
        /// Target platform of the build.
        /// </summary>
        public static TargetPlatform GetPlatform()
        {
            if (s_Platform == TargetPlatform.Unknown) {
                switch (Environment.OSVersion.Platform) {
                    case PlatformID.Win32NT:
                        s_Platform.SetValue(TargetPlatform.Windows);
                        break;
                    case PlatformID.MacOSX:
                        s_Platform.SetValue(TargetPlatform.OSX);
                        break;
                    case PlatformID.Unix:
                        s_Platform.SetValue(TargetPlatform.Linux);
                        break;
                    default:
                        throw new BuildSystemException("Unable to determine target platform.");
                }

                BuildSystem.PrintWarning("Target platform is not specified. Guessed {0}.", s_Platform.GetValue());
            }

            return s_Platform;
        }

        /// <summary>
        /// Target build system for which project/solution files are generated.
        /// </summary>
        public static TargetProjectCompiler GetProjectCompiler()
        {
            if (s_ProjectCompiler == TargetProjectCompiler.Unknown) {
                switch (BuildSystem.GetPlatform()) {
                    case TargetPlatform.XBoxOne:
                    case TargetPlatform.Windows:
                    case TargetPlatform.WindowsPhone8:
                    case TargetPlatform.WindowsRT:
                    case TargetPlatform.WindowsStore:
                        if (!string.IsNullOrEmpty(Environment.GetEnvironmentVariable("VS140COMNTOOLS"))) {
                            s_ProjectCompiler.SetValue(TargetProjectCompiler.VisualStudio2014);
                        } else if (!string.IsNullOrEmpty(Environment.GetEnvironmentVariable("VS120COMNTOOLS"))) {
                            s_ProjectCompiler.SetValue(TargetProjectCompiler.VisualStudio2013);
                            if (s_Platform == TargetPlatform.WindowsPhone8) {
                                throw new BuildSystemException("Windows Phone builds are not supported in VS2013.");
                            }
                        } else {
                            s_ProjectCompiler.SetValue(TargetProjectCompiler.VisualStudio2013);
                            BuildSystem.PrintError("Failed to locate VisualStudio installation. Using {0}.", s_ProjectCompiler.GetValue());
                        }
                        break;

                    case TargetPlatform.iOS:
                    case TargetPlatform.OSX:
                        /// @todo Check XCode installation.
                        s_ProjectCompiler.SetValue(TargetProjectCompiler.XCode);
                        break;

                    case TargetPlatform.Linux:
                    case TargetPlatform.Android:
                        /// @todo Check QtCreator installation.
                        s_ProjectCompiler.SetValue(TargetProjectCompiler.QtCreator);
                        break;

                    case TargetPlatform.Emscripten:
                        /// @todo Check Emscripten installation.
                        s_ProjectCompiler.SetValue(TargetProjectCompiler.Make);
                        break;

                    default:
                        throw new BuildSystemException("Unable to determine target build system.");
                }

                BuildSystem.PrintWarning("Target build system is not specified. Guessed {0}.", s_ProjectCompiler.GetValue());
            }

            return s_ProjectCompiler;
        }

        /// <summary>
        /// Path to solution file for which generation is provided.
        /// </summary>
        public static string GetSolutionPath()
        {
            if (s_SolutionPath.GetValue() == null) {
                s_SolutionPath.SetValue(Path.Combine(SDKPath, "source", "GoddamnEngine.gdsln.cs"));
            }

            return s_SolutionPath;
        }

        #endregion

        //! ------------------------------------------------------------------------------------------
        //! Command-line interface.
        //! ------------------------------------------------------------------------------------------
        #region

        /// <summary>
        /// Prints a line into specified output in specified color.
        /// </summary>
        private static void PrintLineWithColor(ConsoleColor Color, TextWriter Output, string Message, params object[] Args)
        {
            ConsoleColor OldColor = Console.ForegroundColor;
            Console.ForegroundColor = Color;
            if (s_IsVerboseOutput) {
                Output.WriteLine(Message, Args);
            }

            Console.ForegroundColor = OldColor;
        }

        /// <summary>
        /// Prints a message string into stdout.
        /// </summary>
        public static void PrintMessage(string Message, params object[] Args) 
        {
            BuildSystem.PrintLineWithColor(Console.ForegroundColor, Console.Out, Message, Args);
        }

        /// <summary>
        /// Prints a warning string into stdout.
        /// </summary>
        public static void PrintWarning(string Message, params object[] Args) 
        {
            BuildSystem.PrintLineWithColor(ConsoleColor.Yellow, Console.Out, Message, Args);
        }

        /// <summary>
        /// Prints a error string into stderr.
        /// </summary>
        public static void PrintError(string Message, params object[] Args) 
        {
            BuildSystem.PrintLineWithColor(ConsoleColor.Red, Console.Error, Message, Args);
        }

        /// <summary>
        /// Prints copyright message.
        /// </summary>
        private static void PrintCopyright()
        {
            Console.WriteLine("GoddamnBuildSystem project/solution files generation system.");
            Console.WriteLine("Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.");
            Console.WriteLine();
        }

        /// <summary>
        /// Prints help to command-line interface of the BuildSystem.
        /// </summary>
        private static void PrintHelp()
        {
            string BuildSystemExecutableName = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
            BuildSystemExecutableName = BuildSystemExecutableName.Substring(BuildSystemExecutableName.LastIndexOf(Path.DirectorySeparatorChar) + 1);

            // Printing copyright.
            BuildSystem.PrintCopyright();

            // Printing properties.
            Console.WriteLine("Usage: {0} [properties...]", BuildSystemExecutableName);
            Console.WriteLine(" Properties:");
            foreach (var CommandLineProperty in s_CommandLineProperties) {
                // Creating padding to each property.
                string CommandLinePropertyKey = string.Format("  {0,-12} -", CommandLineProperty.GetName());
                string CommandLinePropertyPadding = new string(' ', CommandLinePropertyKey.Length - 1);
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
                    while(CommandLinePropertyDescription.Length > CommandLinePropertyPartLengthInit) {
                        int CommandLinePropertyDescPartLength = CommandLinePropertyPartLengthInit;
                        do {
                            CommandLinePropertyDescPartLength -= 1;
                        } while (" :,.!?\n\r\t".IndexOf(CommandLinePropertyDescription[CommandLinePropertyDescPartLength]) == -1);

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

        /// <summary>
        /// Parses command-line arguments.
        /// </summary>
        private static void ParseCommandLineArguments(string[] Arguments)
        {
            foreach (string CommandLineProperty in Arguments) {
                ICommandLinePropertyProxy CommandLinePropertyProxy = s_CommandLineProperties.Find(
                    P => (P.GetName() == CommandLineProperty) || ((P.GetType() != typeof(bool)) && CommandLineProperty.StartsWith(P.GetName())));

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

        #endregion

        //! ------------------------------------------------------------------------------------------
        //! Environmental properties.
        //! ------------------------------------------------------------------------------------------
        #region

        /// <summary>
        /// Path to GoddamnSDK installation location.
        /// </summary>
        public static string SDKPath 
        { 
            get {
                if (s_SDKPath == null) {
                    s_SDKPath = Environment.GetEnvironmentVariable("GODDAMN_SDK");
                    if (s_SDKPath == null) {
                        throw new BuildSystemException("Environmental variable 'GODDAMN_SDK' was not set. Is the SDK installed correctly?");
                    }
                }

                return s_SDKPath;
            } 
        }

        #endregion

        //! ------------------------------------------------------------------------------------------
        //! Entry point.
        //! ------------------------------------------------------------------------------------------
        #region

        /// <summary>
        /// Application entry point.
        /// </summary>
        private static void Main(string[] Arguments)
        {
            try {
                BuildSystem.ParseCommandLineArguments(Arguments);
                if (s_IsVerboseOutput) {
                    BuildSystem.PrintCopyright();
                }

                Environment.Exit(0);
            } catch (BuildSystemException Exception) {
                // Internal error in code.
                Console.Error.WriteLine(Exception.Message);
            } catch (Exception Exception) {
                Console.Error.WriteLine("Unhandled error cathed while generation project files. Sorry :(");
                Console.Error.WriteLine(Exception.Message);
            }

            Environment.Exit(1);
        }

        #endregion
    }   // class BuildSystem
}   // namespace GoddamnEngine.BuildSystem
