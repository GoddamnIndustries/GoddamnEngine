//! ==========================================================================================
//! VisualStudioTarget.cs - Visual Studio Project generator main class.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

namespace GoddamnEngine.BuildSystem.VisualStudio
{
    internal abstract partial class VisualStudioTarget : Target
    {
        //! ------------------------------------------------------------------------------------------
        //! @name Fields.
        //! @{
        //! ------------------------------------------------------------------------------------------

        private string m_ToolsVersion = null;
        private string m_ToolsFiltersVersion = null;
        private string m_PlatformToolsSet = null;

        //! @}

        //! ------------------------------------------------------------------------------------------
        //! @name Constructor.
        //! @{
        //! ------------------------------------------------------------------------------------------

        protected VisualStudioTarget(string ToolsVersion, string ToolsFiltersVersion, string PlatformToolsSet)
        {
            m_ToolsVersion = ToolsVersion;
            m_ToolsFiltersVersion = ToolsFiltersVersion;
            m_PlatformToolsSet = PlatformToolsSet;
        }

        //! @}
    }   // class VisualStudioTarget

    internal sealed class VisualStudio2013Target : VisualStudioTarget
    {
        public VisualStudio2013Target()
            : base("14.0", "4.0", "CTP_Nov2013")
        {
        }
    }   // class VisualStudio2013Target

    //internal sealed class VisualStudio2015Target : VisualStudioTarget
    //{
    //}   // class VisualStudio2013Target
}   // namespace GoddamnEngine.BuildSystem.VisualStudio
