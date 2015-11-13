// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Compiler.cs
//! Inline dynamic C# compiler.

using System.CodeDom.Compiler;
using System.Reflection;
using System.Text;
using Microsoft.CSharp;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! Inline dynamic C# compiler.
    internal static class CSharpCompiler
    {
        private readonly static object TemporaryAssemblyMutex = new object();
        private static CompilerParameters _parameters;

        // ------------------------------------------------------------------------------------------
        //! Compiles C# source file into Assembly object.
        //! @param pathToSource Path to the source file.
        //! @returns Compiled assembly object.
        internal static Assembly CompileSourceFile(string pathToSource)
        {
            lock (TemporaryAssemblyMutex)
            {
                if (_parameters == null)
                {
                    _parameters = new CompilerParameters(new[] { 
                        "System.dll",                            // Only symbols from System.dll are available in config files.
                        Assembly.GetExecutingAssembly().Location // Adding current assembly.
                    })
                    {
                        TreatWarningsAsErrors = true,
                        GenerateInMemory = true,
                        GenerateExecutable = false,
                        WarningLevel = 4
                    };
                }

                var compilingResults = new CSharpCodeProvider().CompileAssemblyFromFile(_parameters, pathToSource);
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
