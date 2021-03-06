// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem.Collectors;

//! <summary>
//! The 'GoddamnCore' project file.
//! </summary>
public sealed class GoddamnCoreProject : ProjectEnumerator
{
    public override string GetFilter()
    {
        return "Core";
    }

    public sealed override ProjectPriority GetPriority()
    {
        return ProjectPriority.CoreLevel;
    }

    public sealed override string GetSourceFiltersOrigin()
    {
        return Path.Combine(base.GetSourceFiltersOrigin(), "Core");
    }
}   // class GoddamnCoreProject
