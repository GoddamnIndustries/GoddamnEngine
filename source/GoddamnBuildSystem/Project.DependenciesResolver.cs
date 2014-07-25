using System;
using System.IO;
using System.Reflection;
using System.Diagnostics;

namespace GoddamnEngine.BuildSystem
{
    partial class Project
    {
        private static class DependenciesResolver
        {
            public static void ProcessDirectory(Project TheProject, string DependenciesDirectory)
            {
                foreach (var DependencyDirectory in Directory.EnumerateDirectories(DependenciesDirectory)) {
                    string[] CustomDependencyResolvers = Directory.GetFiles(DependencyDirectory, "*.gddep.cs");
                    if (CustomDependencyResolvers.Length != 0) {   // Directory locates custom setup file for dependency.
                        if (CustomDependencyResolvers.Length != 1) {
                            throw new ProjectException(string.Format("Dependency directory \'{0}\' contains multiple '*.gddep.cs' files.", DependenciesDirectory));
                        }
                        Assembly CustomDependencyResolversAssembly = Compiler.CompileSourceFile(CustomDependencyResolvers[0]);
                        foreach (var CustomDependencyResolversAssemblyTypeInfo in CustomDependencyResolversAssembly.DefinedTypes) {
                            if (CustomDependencyResolversAssemblyTypeInfo.IsSubclassOf(typeof(Dependency))) {
                                TheProject.Dependencies.Add(Activator.CreateInstance(CustomDependencyResolversAssemblyTypeInfo, DependencyDirectory) as Dependency);
                                CustomDependencyResolversAssembly = null;
                            }
                        }

                        if (CustomDependencyResolversAssembly != null) {
                            throw new ProjectException(string.Format("Custom dependency resolver '{0}' does not any Dependency-derived type.", CustomDependencyResolvers[0]));
                        }
                    } else {   // No custom dependency. Resolving it using exsiting files.
                        TheProject.Dependencies.Add(new Dependency(DependencyDirectory));
                    }
                }
            }
        }   // class Dependencies
    }   // class Project
}   // namespace GoddamnEngine.BuildSystem
