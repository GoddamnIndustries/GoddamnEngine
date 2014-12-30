//! ==========================================================================================
//! CommandLineUI.cs - Command line user interface class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace GoddamnEngine.BuildSystem
{
 //! Interface to description of a single command-line property.
 //! All command-line properties would be stored in 's_CommandLineArguments' variable.
 public abstract class ICommandLinePropertyProxy
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
 public sealed class CommandLinePropertyProxy<T> : ICommandLinePropertyProxy
 {
  private T m_Value;
  private readonly string m_Name;
  private readonly string m_Description;

  //! Constructs a proxy variable for command-line parameter.
  public CommandLinePropertyProxy(T Value, String Name, String Description)
  {
   Debug.Assert((typeof(T) == typeof(bool)) || (typeof(T) == typeof(string)) || typeof(T).IsEnum);
   Debug.Assert(!string.IsNullOrWhiteSpace(Name));
   Debug.Assert(!string.IsNullOrWhiteSpace(Description));

   m_Value = Value;
   m_Name = Name;
   m_Description = Description;
   CommandLineUI.s_CommandLineProperties.Add(this);
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
 }   // CommandLinePropertyProxy<T>

 //! Represents access to command line user interface.
 public static class CommandLineUI
 {
  internal static List<ICommandLinePropertyProxy> s_CommandLineProperties = new List<ICommandLinePropertyProxy>();

  //! Prints a line into specified output in specified color.
  private static void PrintLineWithColor(ConsoleColor Color, TextWriter Output, string Message, params object[] Args)
  {
   ConsoleColor OldColor = Console.ForegroundColor;
   Console.ForegroundColor = Color;
   Output.WriteLine(Message, Args);
   Console.ForegroundColor = OldColor;
  }

  //! Prints a message string into stdout.
  public static void PrintMessage(string Message, params object[] Args)
  {
   PrintLineWithColor(Console.ForegroundColor, Console.Out, Message, Args);
  }

  //! Prints a warning string into stdout.
  public static void PrintWarning(string Message, params object[] Args)
  {
   PrintLineWithColor(ConsoleColor.Yellow, Console.Out, Message, Args);
  }

  //! Prints a error string into stderr.
  public static void PrintError(string Message, params object[] Args)
  {
   PrintLineWithColor(ConsoleColor.Red, Console.Error, Message, Args);
  }

  //! Prints help to command-line interface of the BuildSystem.
  public static void PrintHelp()
  {
   string BuildSystemExecutableName = Path.GetFileName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);

   // Printing copyright.
   Console.Out.WriteLine("GoddamnBuildSystem project/solution files generation system.");
   Console.Out.WriteLine("Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.");
   Console.Out.WriteLine();

   // Printing properties.
   Console.Out.WriteLine("Usage: {0} [properties...]", BuildSystemExecutableName);
   Console.Out.WriteLine(" Properties:");
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

      Console.Out.Write("{0}\n{1}", CommandLinePropertyDescription.Substring(0, CommandLinePropertyDescPartLength), CommandLinePropertyPadding);
      CommandLinePropertyDescription = CommandLinePropertyDescription.Substring(CommandLinePropertyDescPartLength + 1).TrimStart();
     }
    }

    Console.Out.WriteLine(CommandLinePropertyDescription);
   }
   Console.Out.WriteLine();

   // Printing examples. These ones are deprecated.
  //  Console.Out.WriteLine("Examples:");
  //  Console.Out.WriteLine(" {0} --platform=Windows --ide=QtCreator Game.gdsln.cs", BuildSystemExecutableName);
  //  Console.Out.WriteLine(" {0} --monolithic --platform=iOS Game.gdsln.cs", BuildSystemExecutableName);
  }

  //! Parses command-line arguments.
  public static void ParseCommandLineArguments(string[] Arguments)
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
     //if (CommandLinePropertyProxy == s_DoJustPrintHelp) {
     // PrintHelp();
     // Environment.Exit(0);
     //}

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

 }   // class CommandLineUI
}   // namespace GoddamnEngine.BuildSystem
