using System.Collections.Generic;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnCoreProject : Project
{
    public override string Name { get { return "GoddamnCore"; } }
    public override bool IsPlugin { get { return false; } }
    public override ProjectBuildType BuildType 
    { 
        get 
        {
            if (Target.IsDesktopPlatform() && !Target.IsMonolithicBuild())
                return ProjectBuildType.DynamicLibrary;
            return ProjectBuildType.StaticLibrary; 
        } 
    }
}
