using System;
using System.IO;

namespace GoddamnEngine.BuildSystem
{
    public class Dependency
    {
        public readonly string Location;
        public Dependency(string Location) { this.Location = Location; }

        public virtual string HeaderLocations { get { return Path.Combine(this.Location, "include"); } }
        public virtual string ResolveDependency(TargetConfiguration Configuration)
        {
            string DependencyPathPCE = Path.Combine(this.Location, String.Format("{0}.{1}{2}", Target.Platform.ToString(), Configuration.ToString(), Target.GetPlatformStaticLibraryExtension()));
            if (!File.Exists(DependencyPathPCE))
            {   // Seams that library is build for all configurations into single library.
                string DependencyPathPE = Path.Combine(this.Location, String.Format("{0}{1}", Target.Platform.ToString(), Target.GetPlatformStaticLibraryExtension()));
                if (!File.Exists(DependencyPathPE))
                {   // Libary path on disk contains only extension
                    string DependencyPathE = Path.Combine(this.Location, Target.GetPlatformStaticLibraryExtension());
                    if (!File.Exists(DependencyPathE))
                    {   // Nothing found.
                        ConsoleOutput.WriteLine(Path.Combine(this.Location, ".gddep"), "warning: not static libraries was found this dependecy");
                        return "";
                    }

                    return DependencyPathE;
                }
                    
                return DependencyPathPE;
            }
         
            return DependencyPathPCE;
        }

    }   // class DependencyResolver

    public sealed class CoreProjectDependency : Dependency
    {
        private CoreProjectDependency() : base(Path.Combine(BuildSystem.SDKPath, "lib")) { }
        public static readonly CoreProjectDependency Instance = new CoreProjectDependency();
        public override string HeaderLocations { get { return ""; } }
        public override string ResolveDependency(TargetConfiguration Configuration)
        {
            string CoreProjectPath = Path.Combine(this.Location, String.Format("GoddamnCore{0}{1}", 
                ((Configuration == TargetConfiguration.Debug) ? ".Debug" : ""), 
                Target.GetPlatformStaticLibraryExtension())
                );

            return CoreProjectPath;
        }
    }   // class CoreProjectDependency
    public sealed class EngineProjectDependency : Dependency
    {
        private EngineProjectDependency() : base(Path.Combine(BuildSystem.SDKPath, "lib")) { }
        public static readonly EngineProjectDependency Instance = new EngineProjectDependency();
        public override string HeaderLocations { get { return ""; } }
        public override string ResolveDependency(TargetConfiguration Configuration)
        {
            string CoreProjectPath = Path.Combine(this.Location, String.Format("GoddamnEngine{0}{1}",
                ((Configuration == TargetConfiguration.Debug) ? ".Debug" : ""),
                Target.GetPlatformStaticLibraryExtension())
                );

            return CoreProjectPath;
        }
    }   // class CoreProjectDependency
}   // namespace GoddamnEngine.BuildSystem
