/// ==========================================================================================
/// Compiler.cs - Inline dynamic C# compiler.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2014 - Created by James Jhuighuy
///     * 25.11.2014 - Refactored.
/// ==========================================================================================

using System.Text;
using System.Reflection;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Inline dynamic C# compiler.
    /// </summary>
    internal static class CSharpCompiler
    {
        //! ------------------------------------------------------------------------------------------
        //! Static fields.
        //! ------------------------------------------------------------------------------------------

        private readonly static object s_TemporaryAssemblyMutex = new object();
        private static CompilerParameters s_Parameters;

        //! ------------------------------------------------------------------------------------------
        //! Class API.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Compiles C# source file into Assembly object.
        /// </summary>
        public static Assembly CompileSourceFile(string PathToSource)
        {
            lock (s_TemporaryAssemblyMutex) {
                if (s_Parameters == null) { 
                    s_Parameters = new CompilerParameters(new string[] { 
                        "System.dll",                               // Only symbols from System.dll are avaliable in config files.
                        Assembly.GetExecutingAssembly().Location    // Adding current assembly.
                    }) {
                        TreatWarningsAsErrors = true,
                        GenerateInMemory      = true,
                        GenerateExecutable    = false,
                        WarningLevel          = 4
                    };
                }

                CompilerResults CompilingResults = new CSharpCodeProvider().CompileAssemblyFromFile(s_Parameters, PathToSource);
                if (CompilingResults.Errors.HasErrors || CompilingResults.Errors.HasWarnings) {   
                    // Seams we have compiling errors/warnings (assume using /wx compiling options) here
                    StringBuilder CompilingErrorsAndWarnings = new StringBuilder();
                    foreach (var CompilingErrorOrWarning in CompilingResults.Errors) { 
                        CompilingErrorsAndWarnings.AppendLine(CompilingErrorOrWarning.ToString());
                    }

                    throw new BuildSystemException(CompilingErrorsAndWarnings.ToString());
                }

                return CompilingResults.CompiledAssembly;
            }
        }
    }   // class CSharpCompiler
}   // namespace GoddamnEngine.BuildSystem
