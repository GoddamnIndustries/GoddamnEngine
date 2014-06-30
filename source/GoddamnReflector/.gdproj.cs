using System.IO;
using System.Collections.Generic;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnReflector : Project
{
    public override string Name { get { return "GoddamnReflector"; } }
    public override bool IsPlugin { get { return false; } }
    public override ProjectBuildType BuildType { get { return ProjectBuildType.Application; } }
//  public override string RelativeOutputPath { get { return "ThirdParty" + Path.DirectorySeparatorChar; } }
    protected override void InitializeSelf()
    {
        base.InitializeSelf();
        this.Dependencies.Add(CoreProjectDependency.Instance);
    }
}
