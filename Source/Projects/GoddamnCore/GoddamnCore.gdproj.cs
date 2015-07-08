// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnCoreProject : Project
{
    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.CoreLevel;
    }

    public sealed override string GetSourceFiltersOrigin()
    {
        return Path.Combine(base.GetSourceFiltersOrigin());
    }
}   // class GoddamnCoreProject
