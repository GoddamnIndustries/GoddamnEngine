//! ==========================================================================================
//! Toolchain.cs - descriptions for toolchains (compilers, linkers, etc).
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

namespace GoddamnEngine.BuildSystem
{
    //! List of properties for C++ compiler.
    public static class TargetCppCompilerInformation
    {
        public const int s_WarningLevel = 4;
        public const bool s_WarningsAreErrors = true;
    }   // class TargetCppCompilerInformation
}   // namespace GoddamnEngine.BuildSystem
