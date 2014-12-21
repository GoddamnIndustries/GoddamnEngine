//! ==========================================================================================
//! ConfigurationInformation.cs - description for platforms.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

namespace GoddamnEngine.BuildSystem
{
    //! "Windows" platform information.
    public sealed class WindowsPlatformInformation : TargetPlatformInformation
    {
        public WindowsPlatformInformation()
        {
            m_HumanReadableName = "Windows (x64)";
            m_StandartHeadersPathes = null;
            m_DefaultLibraries = new string[] { "winmm.lib", "imm32.lib", "version.lib" };
            m_RequiresExceptions = false;
            m_RequiresRTTI = false;
        }
    }   // class WindowsPlatformInformation

    //public sealed class PlatformInformation : TargetPlatformInformation
    //{
    //    public PlatformInformation()
    //    {
    //    }
    //}   // class PlatformInformation
}   // GoddamnEngine.BuildSystem
