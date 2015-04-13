// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Toolchain.cs
//! Descriptions for toolchains (compilers, linkers, etc).

namespace GoddamnEngine.BuildSystem
{
    //! List of properties for C++ compiler.
    public static class TargetCppCompilerInformation
    {
        public const int  s_WarningLevel = 4;
        public const bool s_WarningsAreErrors = true;
    }   // class TargetCppCompilerInformation
}   // namespace GoddamnEngine.BuildSystem
