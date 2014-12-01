/// ==========================================================================================
/// BuildSystem.cs - GoddamnBuildSystem main source module.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2014 - Created by James Jhuighuy
///     * 09.11.2014 - Refactored.
/// ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
    /// <summary> Provides some useful methods for parsing command line arguments. </summary>
    public static class CommandLineArguments
    {
        private class _CommandLineArgument
        {
            public string Description;
            public object Value;
        }   // struct CommandLineArgument
        private static Dictionary<string, _CommandLineArgument> _ArgumentsDefinitions = new Dictionary<string, _CommandLineArgument>() 
        {
            { "--help",       new _CommandLineArgument { Description = "Prints this help.", Value = false }},
            { "--target",     new _CommandLineArgument { Description = "Target platform to which project files would be generated.", Value = TargetPlatform.Windows } },
            { "--monolithic", new _CommandLineArgument { Description = "Generate single static library for entire engine. All subprojects would be linked into it.", Value = false  } },
        };

        public static object GetCommandLineArgument(string CommandLineArgumentName)
        {
            if (_ArgumentsDefinitions.ContainsKey(CommandLineArgumentName)) {
                return _ArgumentsDefinitions[CommandLineArgumentName].Value;
            }

            return null;
        }

        /// <summary> Parses command line arguments </summary>
        /// <param name="Arguments"> Array of arguments. </param>
        public static void Parse(string[] Arguments)
        {
            for (int i  = -1; ++i < Arguments.Length; ) {
                if (_ArgumentsDefinitions.ContainsKey(Arguments[i])) {
                    var ArgumentDefinition = _ArgumentsDefinitions[Arguments[i]];
                    if (ArgumentDefinition.Value.GetType() != typeof(bool)) {
                        if (++i < Arguments.Length) {
                            if (ArgumentDefinition.Value.GetType().IsEnum) {
                                ArgumentDefinition.Value = Enum.Parse(ArgumentDefinition.Value.GetType(), Arguments[i], true);
                            } else {
                                throw new NotImplementedException();
                            }
                        } else {
                            throw new ArgumentException("\nArgument \"{0}\" specified without value.", Arguments[i]);
                        }
                    } else {
                        ArgumentDefinition.Value = true;
                    }
                } else {
                    throw new ArgumentException("\nInvalid argument \"{0}\" specified.", Arguments[i]);
                }
            }
        }

        /// <summary> Prints help for the nGoddamnBuildSystem </summary>
        public static void PrintHelp()
        {
            Console.Write("\nGoddamnBuildSystem - takes care about compiling projects.");
            Console.Write("\nCopyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.");
            Console.Write("\nCommand line options that may be passed to GoddamnBuildSystem:");

            foreach (var Argument in _ArgumentsDefinitions) {
                string ArgumentPadding = null; {
                    string ArgumentKey = string.Format("\n {0,-12}: ", Argument.Key);
                    ArgumentPadding = new string(' ', ArgumentKey.Length - 1);
                    Console.Write(ArgumentKey);
                }
                
                string ArgumentDesc = null;
                if (Argument.Value.Value.GetType().IsEnum) {
                    ArgumentDesc = new StringBuilder()
                    //  .Append('[').Append(Argument.Value.Value.GetType().Name).Append("] ")
                        .Append(Argument.Value.Description)
                        .Append(string.Join(", ", Argument.Value.Value.GetType().GetEnumNames()))
                        .Append('.').ToString();
                } else {
                    ArgumentDesc = Argument.Value.Description;
                }

                int ArgumentDescPartLengthInit = Console.WindowWidth - ArgumentPadding.Length;
                if (ArgumentDesc.Length >= ArgumentDescPartLengthInit) {
                    Debug.Assert(Argument.Value.Description.EndsWith("."));
                    while(true) {
                        if (ArgumentDesc.Length > ArgumentDescPartLengthInit) {   // Finding last word end.
                            int ArgumentDescPartLength = ArgumentDescPartLengthInit;
                            do { /// @todo Pretty shitty code here. Rewrite it.
                                --ArgumentDescPartLength; 
                            } while (" \n\r\t.,".IndexOf(ArgumentDesc[ArgumentDescPartLength]) == -1);
                            
                            Console.Write("{0}\n{1}", ArgumentDesc.Substring(0, ArgumentDescPartLength), ArgumentPadding);
                            ArgumentDesc = ArgumentDesc.Substring(ArgumentDescPartLength + 1);
                        } else {
                            Console.Write(ArgumentDesc);
                            break;
                        }
                    }
                } else {
                    Console.Write(Argument.Value.Description);
                }
            }
        }
    }   // class CommandLineArguments
}   // namespace GoddamnEngine.BuildSystem
