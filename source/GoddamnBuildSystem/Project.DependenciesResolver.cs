using System.IO;

namespace GoddamnEngine.BuildSystem
{
    partial class Project
    {
        private static class DependenciesResolver
        {
            public static void ProcessDirectory(Project TheProject, string DependenciesDirectory)
            {
                foreach (var DependencyDirectory in Directory.EnumerateDirectories(DependenciesDirectory))
                {
                    TheProject.Dependencies.Add(new Dependency(DependencyDirectory));
                }
            }
        }   // private class Dependencies
    }   // class Project
}   // namespace GoddamnEngine.BuildSystem
