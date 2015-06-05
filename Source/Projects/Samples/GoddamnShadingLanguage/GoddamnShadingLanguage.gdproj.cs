//! ==========================================================================================
//! GoddamnEngine.gdproj.cs - GoddamnEngine project file.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnShadingLanguage : Project
{
    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.CoreLevel;
    }

    public sealed override string GetSourceFiltersOrigin()
    {
        return Path.Combine(base.GetSourceFiltersOrigin(), "Engine");
    }
}   // class GoddamnEngineProject

