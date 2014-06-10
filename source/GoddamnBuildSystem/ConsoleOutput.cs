using System;
using System.Diagnostics;

namespace GoddamnEngine.BuildSystem
{
    internal static class ConsoleOutput
    {
        public static void WriteLine(string File, string Format, params object[] VaArgs)
        {
            switch(Environment.OSVersion.Platform)
            {
                case PlatformID.Win32NT:
                case PlatformID.Xbox:   // Formatting to visual studio output.
                    string Message = String.Format("{0}(0,0): {1}", File, String.Format(Format, VaArgs));
                    if (Debugger.IsAttached)
                        Debug.WriteLine("\n\n" + Message);
                    else
                        Console.WriteLine(Message);
                    break;

                default:
                    throw new NotImplementedException();
            }
        }
    }   // class ConsoleOutput
}   // namespace GoddamnEngine.BuildSystem
