//! ==========================================================================================
//! ConfigurationInformation.cs - description for configurations.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author James Jhuighuy
//! ==========================================================================================

namespace GoddamnEngine.BuildSystem
{
    //! "Debug" configuration information.
    public sealed class DebugConfigurationInformation : TargetConfigurationInformation
    {
        public DebugConfigurationInformation()
        {
            m_HumanReadableName = "Debug";
            m_IsDebug = true;
            m_GenerateDebugInformation = true;
            m_Optimize = false;
        }
    }   // class DebugConfigurationInformation

    //! "Release" configuration information.
    public sealed class ReleaseConfigurationInformation : TargetConfigurationInformation
    {
        public ReleaseConfigurationInformation()
        {
            m_HumanReadableName = "Release";
            m_IsDebug = false;
            m_GenerateDebugInformation = true;
            m_Optimize = true;
        }
    }   // class ReleaseConfigurationInformation

    //! "Development" configuration information.
    public sealed class DevelopmentConfigurationInformation : TargetConfigurationInformation
    {
        public DevelopmentConfigurationInformation()
        {
            m_HumanReadableName = "Development";
            m_IsDebug = false;
            m_GenerateDebugInformation = true;
            m_Optimize = true;
        }
    }   // class DevelopmentConfigurationInformation

    //! "Shipping" configuration information.
    public sealed class ShippingConfigurationInformation : TargetConfigurationInformation
    {
        public ShippingConfigurationInformation()
        {
            m_HumanReadableName = "Shipping";
            m_IsDebug = false;
            m_GenerateDebugInformation = true;
            m_Optimize = true;
        }
    }   // class ShippingConfigurationInformation
}   // namespace GoddamnEngine.BuildSystem
