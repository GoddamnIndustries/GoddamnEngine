using System;
using System.Text;
using System.Reflection;
using System.Diagnostics;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

namespace GoddamnEngine.BuildSystem
{
    internal static class Compiler
    {
        private readonly static object TemporaryAssemblyMutex = new object();
        private readonly static CompilerParameters Parameters = new CompilerParameters(new string[] { 
            "System.dll", // Only symbols from System.dll are avaliable in config files.
            Assembly.GetExecutingAssembly().Location 
        })
        {
            TreatWarningsAsErrors = true,
            GenerateInMemory = true,
            GenerateExecutable = false,
            WarningLevel = 4
        };

        public static Assembly CompileSourceFile(string PathToSource)
        {
            lock (Compiler.TemporaryAssemblyMutex)
            {
                CompilerResults CompilingResults = new CSharpCodeProvider().CompileAssemblyFromFile(Compiler.Parameters, PathToSource);
                if (CompilingResults.Errors.HasErrors || CompilingResults.Errors.HasWarnings)
                {   // Seams we have compiling errors/warnings (assume using /wx compiling options) here
                    StringBuilder CompilingErrorsAndWarnings = new StringBuilder();
                    foreach (var CompilingErrorOrWarning in CompilingResults.Errors)
                        CompilingErrorsAndWarnings.AppendLine(CompilingErrorOrWarning.ToString());

                    string CompilingErrorsAndWarningsString = CompilingErrorsAndWarnings.ToString();
                    Debug.WriteLine(CompilingErrorsAndWarningsString);
                    throw new InvalidOperationException(CompilingErrorsAndWarningsString);
                }

                return CompilingResults.CompiledAssembly;
            }
        }
    }   // class Compiler
}   // namespace GoddamnEngine.BuildSystem
