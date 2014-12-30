//! ==========================================================================================
//! TargetConfiguration.cs - descriptions for configurations.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace GoddamnEngine.BuildSystem
{
    //! List of supported target configurations.
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Release,
        Development,
        Shipping,
    }   // enum TargetConfigurations

    //! List of properties for specific configuration.
    public class TargetConfigurationInfo
    {
        private static Dictionary<TargetConfiguration, TargetConfigurationInfo> s_CachedInformation;
        public readonly TargetConfiguration m_Configuration;
        public string m_HumanReadableName { get; protected set; }
        public bool m_IsDebug { get; protected set; }
        public bool m_GenerateDebugInformation { get; protected set; }
        public bool m_Optimize { get; protected set; }

        //! Instantiates a configuration information. Should setup properties.
        protected TargetConfigurationInfo()
        {
        }

        //! Returns the information for specific platform.
        //! @param TheConfiguration Desired configuration.
        public static TargetConfigurationInfo Get(TargetConfiguration TheConfiguration)
        {
            if (s_CachedInformation == null) {
                s_CachedInformation = new Dictionary<TargetConfiguration, TargetConfigurationInfo>();
                foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                    Type ConfigurationInfoType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(Configuration + "ConfigurationInfo"));
                    if (ConfigurationInfoType != null) {
                        TargetConfigurationInfo ConfigurationInfo = (TargetConfigurationInfo)Activator.CreateInstance(ConfigurationInfoType);
                        s_CachedInformation.Add(Configuration, ConfigurationInfo);
                    } else {
                        throw new BuildSystemException("Not сonfiguration information exists for сonfiguration {0}.", Configuration);
                    }
                }
            }

            return s_CachedInformation[TheConfiguration];
        }
    }   // class TargetConfigurationInfo

    //! "Debug" configuration information.
    public sealed class DebugConfigurationInfo : TargetConfigurationInfo
    {
        public DebugConfigurationInfo()
        {
            m_HumanReadableName = "Debug";
            m_IsDebug = true;
            m_GenerateDebugInformation = true;
            m_Optimize = false;
        }
    }   // class DebugConfigurationInfo

    //! "Release" configuration information.
    public sealed class ReleaseConfigurationInfo : TargetConfigurationInfo
    {
        public ReleaseConfigurationInfo()
        {
            m_HumanReadableName = "Release";
            m_IsDebug = false;
            m_GenerateDebugInformation = true;
            m_Optimize = true;
        }
    }   // class ReleaseConfigurationInfo

    //! "Development" configuration information.
    public sealed class DevelopmentConfigurationInfo : TargetConfigurationInfo
    {
        public DevelopmentConfigurationInfo()
        {
            m_HumanReadableName = "Development";
            m_IsDebug = false;
            m_GenerateDebugInformation = true;
            m_Optimize = true;
        }
    }   // class DevelopmentConfigurationInfo

    //! "Shipping" configuration information.
    public sealed class ShippingConfigurationInfo : TargetConfigurationInfo
    {
        public ShippingConfigurationInfo()
        {
            m_HumanReadableName = "Shipping";
            m_IsDebug = false;
            m_GenerateDebugInformation = true;
            m_Optimize = true;
        }
    }   // class ShippingConfigurationInfo

    // Template for new configurations:
#if FALSE
 //! "" configuration information.
 public sealed class ConfigurationInfo : TargetConfigurationInfo
 {
  public ConfigurationInfo()
  {
  }
 }   // class ConfigurationInfo
#endif  // FALSE
}   // namespace GoddamnEngine.BuildSystem
