// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem.Target;
using GoddamnEngine.BuildSystem.Collectors;

public sealed class GoddamnUIProject : ProjectEnumerator
{
    public override bool GetIsSupported(TargetPlatform platform, TargetConfiguration configuration)
    {
        return platform == TargetPlatform.Windows;
    }

    public override string GetFilter()
    {
        return "Core";
    }

    public sealed override string GetSourceFiltersOrigin()
    {
        return Path.Combine(base.GetSourceFiltersOrigin(), "UI");
    }

    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.GameLevel;
    }

    public sealed override ProjectBuildType GetBuildType(TargetPlatform Platform, TargetConfiguration Configuration)
    {
        return ProjectBuildType.Application;
    }
}   // class GoddamnUIProject
