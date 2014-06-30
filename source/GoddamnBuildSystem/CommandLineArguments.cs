using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
    public static class CommandLineArguments
    {
        private struct CommandLineArgument
        {
            public string Description;
            public object Value;
        }   // struct CommandLineArgument

        private static int MaxCommandLineArgumentNameLength = 12;
        private static Dictionary<string, CommandLineArgument> _Arguments = new Dictionary<string, CommandLineArgument>() 
        {
            { "help",       new CommandLineArgument { Description = "Prints this help.", Value = false }},
            { "target",     new CommandLineArgument { Description = "Target platform to which project files would be generated.", Value = TargetPlatform.Windows } },
            { "monolithic", new CommandLineArgument { Description = "Generate single static library for entire engine. All subprojects would be linked into it.", Value = false  } },
        };

        public static object GetCommandLineArgument(string CommandLineArgumentName)
        {
            if(CommandLineArguments._Arguments.ContainsKey(CommandLineArgumentName))
                return CommandLineArguments._Arguments[CommandLineArgumentName];

            return null;
        }

        public static void PrintHelp()
        {
            Console.Write("\nGoddamnBuildSystem");
            Console.Write("\n==================");
            Console.Write("\n");
            Console.Write("\nOptions");
            Console.Write("\n-------");
            Console.Write("\nCommand line options that may be passed to GoddamnBuildSystem:");
            foreach (var Argument in CommandLineArguments._Arguments)
            {
                Debug.Assert(Argument.Key.Length < MaxCommandLineArgumentNameLength);
                string ArgumentKey     = string.Format(" --{0}{1}: ", Argument.Key, new string(' ', MaxCommandLineArgumentNameLength - Argument.Key.Length));
                string ArgumentPadding = new string(' ', ArgumentKey.Length);
                
                Console.Write("\n");
                Console.Write(ArgumentKey);

                string ArgumentDesc = null;
                if (Argument.Value.Value.GetType().IsEnum)
                {
                    StringBuilder ArgumentDescBuilder = new StringBuilder(Argument.Value.Description);
                    ArgumentDescBuilder.Append(" Supported value are: ");
                    ArgumentDescBuilder.Append(string.Join(", ", Argument.Value.Value.GetType().GetEnumNames()));
                    ArgumentDescBuilder.Append('.');
                    ArgumentDesc = ArgumentDescBuilder.ToString();
                }
                else
                    ArgumentDesc = Argument.Value.Description;

                int ArgumentDescPartLengthInit = Console.WindowWidth - ArgumentKey.Length;
                if (ArgumentDesc.Length >= ArgumentDescPartLengthInit)
                {
                    while(true)
                    {
                        if (ArgumentDesc.Length > ArgumentDescPartLengthInit)
                        {   // Finding last word end.
                            int ArgumentDescPartLength = ArgumentDescPartLengthInit;
                            do { --ArgumentDescPartLength; } while (" \n\r\t.,".IndexOf(ArgumentDesc[ArgumentDescPartLength]) == -1);
                            Console.Write("{0}\n{1}", ArgumentDesc.Substring(0, ArgumentDescPartLength), ArgumentPadding);
                            ArgumentDesc = ArgumentDesc.Substring(ArgumentDescPartLength + 1);
                        }
                        else
                        {
                            Console.Write(ArgumentDesc);
                            break;
                        }
                    }
                }
                else
                    Console.Write(Argument.Value.Description);
            }
        }
    }   // class CommandLineArguments
}   // namespace GoddamnEngine.BuildSystem
