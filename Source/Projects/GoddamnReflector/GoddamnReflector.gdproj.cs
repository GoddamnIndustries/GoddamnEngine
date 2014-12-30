//! ==========================================================================================
//! GoddamnReflector.gdproj.cs - GoddamnReflector project file.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! ==========================================================================================

using System.IO;
using GoddamnEngine.BuildSystem;

public sealed class GoddamnReflectorProject : Project
{
 public override string GetFilter()
 {
  return "Build tools";
 }

 public sealed override ProjectPriority GetPriority()
 {
  return ProjectPriority.EngineLevel;
 }


 public sealed override string GetSourceFiltersOrigin()
 {
  return Path.Combine("GoddamnReflector");
 }
}   // class GoddamnCoreProject
