//! ==========================================================================================
//! GoddamnEngine.gdproj.cs - GoddamnEngine project file.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnEngineProject : Project
{
    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.EngineLevel;
    }

    public sealed override string GetSourceFiltersOrigin()
    {
        return Path.Combine(base.GetSourceFiltersOrigin(), "Engine");
    }
}   // class GoddamnEngineProject

