using System.Collections.Generic;
using GoddamnEngine.BuildSystem;

public sealed class RuntimeProject : Project
{
    public override string Name { get { return "01_SimpleScene"; } }
    public override bool IsPlugin { get { return false; } }
    public override ProjectBuildType BuildType { get { return ProjectBuildType.Application; } }
    public override string RelativeOutputPath { get { return "Samples/01_SimpleScene/"; } }
    protected override void InitializeSelf()
    {
        base.InitializeSelf();
        this.Dependencies.Add(CoreProjectDependency.Instance);
        this.Dependencies.Add(EngineProjectDependency.Instance);
    }
}
