// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using GoddamnEngine.BuildSystem.Collectors;

/// <summary>
/// The 'GoddamnCore' project file.
/// </summary>
public sealed class GoddamnCoreProject : Project
{
    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.CoreLevel;
    }
}   // class GoddamnCoreProject
