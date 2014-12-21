//! ==========================================================================================
//! GoddamnEngine.gdproj.cs - GoddamnEngine project file.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnEngineProject : Project
{
    public sealed override string GetSourceFiltersOrigin()
    {
        return Path.Combine(base.GetSourceFiltersOrigin(), "Engine");
    }
}   // class GoddamnEngineProject
    