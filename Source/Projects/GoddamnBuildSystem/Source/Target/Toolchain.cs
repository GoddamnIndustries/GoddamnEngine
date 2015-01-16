//! ==========================================================================================
//! Toolchain.cs - descriptions for toolchains (compilers, linkers, etc).
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
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
