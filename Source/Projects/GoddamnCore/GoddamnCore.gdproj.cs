//! ==========================================================================================
//! GoddamnCore.gdproj.cs - GoddamnCore project file.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

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
        return Path.Combine(base.GetSourceFiltersOrigin(), "Core");
    }
}   // class GoddamnCoreProject
