using System.Collections.Generic;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnEngineProject : Project
{
    public override string Name { get { return "GoddamnEngine"; } }
    public override bool IsPlugin { get { return false; } }
    public override ProjectBuildType BuildType { get { return ProjectBuildType.DynamicLibrary; } }
    protected override void InitializeSelf()
    {
        base.InitializeSelf();
        this.Dependencies.Add(CoreProjectDependency.Instance);
    }
}
