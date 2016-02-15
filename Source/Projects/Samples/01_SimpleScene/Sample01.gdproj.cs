//! ==========================================================================================
//! Sample01.gdproj.cs - Sample #01 project file.
//! Copyright (C) Goddamn Industries 2016. All Rights Reserved.
//! ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem.Target;
using GoddamnEngine.BuildSystem.Collectors;

public sealed class Sample01Project : Project
{
    public override string GetFilter()
    {
        return "Samples";
    }

    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.GameLevel;
    }

    public sealed override ProjectBuildType GetBuildType(TargetPlatform Platform, TargetConfiguration Configuration)
    {
        return ProjectBuildType.Application;
    }
}   // class GoddamnCoreProject
