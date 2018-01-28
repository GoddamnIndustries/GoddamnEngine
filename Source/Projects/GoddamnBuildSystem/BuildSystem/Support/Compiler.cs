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
        static readonly object s_TemporaryAssemblyMutex = new object();

        /// <summary>
        /// Compiles C# source file into Assembly object.
        /// </summary>
        /// <param name="pathToSource">Path to the source file.</param>
        /// <returns>Compiled assembly object.</returns>
        internal static Assembly CompileSourceFile(string pathToSource)
        {
            lock (s_TemporaryAssemblyMutex)
            {
                var compilationParameters = new CompilerParameters()
                {
                    GenerateInMemory = false,
                    GenerateExecutable = false,
                    WarningLevel = 4,
                };
                compilationParameters.ReferencedAssemblies.Add("System.dll");
                compilationParameters.ReferencedAssemblies.Add(Assembly.GetExecutingAssembly().Location);

                var compilationResults = new CSharpCodeProvider().CompileAssemblyFromFile(compilationParameters, pathToSource);
                if (compilationResults.Errors.HasErrors)
                {
                    var compilingErrorsAndWarnings = new StringBuilder();
                    foreach (var compilingErrorOrWarning in compilationResults.Errors)
                    {
                        compilingErrorsAndWarnings.AppendLine(compilingErrorOrWarning.ToString());
                    }

                    var compilingErrorsAndWarningsString = compilingErrorsAndWarnings.ToString();
                    throw new BuildSystemException(compilingErrorsAndWarningsString);
                }

                var compiledAssembly = compilationResults.CompiledAssembly;
                return compiledAssembly;
            }
        }

    }   // class CSharpCompiler

}   // namespace GoddamnEngine.BuildSystem
