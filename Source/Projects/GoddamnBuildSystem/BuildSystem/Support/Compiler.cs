// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.CodeDom.Compiler;
using System.Reflection;
using System.Text;
using Microsoft.CSharp;

namespace GoddamnEngine.BuildSystem.Support
{
    /// <summary>
    /// Inline dynamic C# compiler.
    /// </summary>
    internal static class CSharpCompiler
    {
        private static readonly object s_TemporaryAssemblyMutex = new object();
        private static CompilerParameters s_Parameters;

        /// <summary>
        /// Compiles C# source file into Assembly object.
        /// </summary>
        /// <param name="pathToSource">Path to the source file.</param>
        /// <returns>Compiled assembly object.</returns>
        internal static Assembly CompileSourceFile(string pathToSource)
        {
            lock (s_TemporaryAssemblyMutex)
            {
                if (s_Parameters == null)
                {
                    s_Parameters = new CompilerParameters(new[] { 
                        "System.dll",                            // Only symbols from System.dll are available in configuration files.
                        Assembly.GetExecutingAssembly().Location // Adding current assembly.
                    })
                    {
                        TreatWarningsAsErrors = true,
                        GenerateInMemory = true,
                        GenerateExecutable = false,
                        WarningLevel = 4
                    };
                }

                var compilingResults = new CSharpCodeProvider().CompileAssemblyFromFile(s_Parameters, pathToSource);
                if (compilingResults.Errors.HasErrors || compilingResults.Errors.HasWarnings)
                {
                    // Seams we have compiling errors/warnings (assume compiling using /wx options) here..
                    var compilingErrorsAndWarnings = new StringBuilder();
                    foreach (var compilingErrorOrWarning in compilingResults.Errors)
                    {
                        compilingErrorsAndWarnings.AppendLine(compilingErrorOrWarning.ToString());
                    }

                    throw new BuildSystemException(compilingErrorsAndWarnings.ToString());
                }

                return compilingResults.CompiledAssembly;
            }
        }

    }   // class CSharpCompiler

}   // namespace GoddamnEngine.BuildSystem
