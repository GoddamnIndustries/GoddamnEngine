// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem.Target
{
    /// <summary>
    /// List of supported target configurations.
    /// </summary>
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Development,
        Release,
        Any
    }   // enum TargetConfigurations

    /// <summary>
    /// List of properties for specific configuration.
    /// </summary>
    public class TargetConfigurationInfo
    {
        static Dictionary<TargetConfiguration, TargetConfigurationInfo> s_CachedInformation;

        public string HumanReadableName { get; protected set; }
        public bool IsDebug { get; protected set; }
        public bool GenerateDebugInformation { get; protected set; }
        public bool Optimize { get; protected set; }

        /// <summary>
        /// Initializes the configuration information. Should setup properties.
        /// </summary>
        protected TargetConfigurationInfo()
        {
        }

        /// <summary>
        /// Returns the information for specific platform.
        /// </summary>
        /// <param name="theConfiguration">Desired configuration.</param>
        public static TargetConfigurationInfo Get(TargetConfiguration theConfiguration)
        {
            if (s_CachedInformation == null)
            {
                s_CachedInformation = new Dictionary<TargetConfiguration, TargetConfigurationInfo>
                {
                    [TargetConfiguration.Unknown] = null,
                    [TargetConfiguration.Debug] = new DebugConfigurationInfo(),
                    [TargetConfiguration.Release] = new ReleaseConfigurationInfo(),
                    [TargetConfiguration.Development] = new DevelopmentConfigurationInfo(),
                };
            }
            return s_CachedInformation[theConfiguration];
        }
    }   // class TargetConfigurationInfo

    /// <summary>
    /// "Debug" configuration information.
    /// </summary>
    public sealed class DebugConfigurationInfo : TargetConfigurationInfo
    {
        public DebugConfigurationInfo() 
        {
            HumanReadableName = "Debug";
            IsDebug = true;
            GenerateDebugInformation = true;
            Optimize = false;
        }
    }   // class DebugConfigurationInfo

    /// <summary>
    /// "Development" configuration information.
    /// </summary>
    public sealed class DevelopmentConfigurationInfo : TargetConfigurationInfo
    {
        public DevelopmentConfigurationInfo()
        {
            HumanReadableName = "Development";
            IsDebug = false;
            GenerateDebugInformation = true;
            Optimize = true;
        }
    }   // class DevelopmentConfigurationInfo

    /// <summary>
    /// "Release" configuration information.
    /// </summary>
    public sealed class ReleaseConfigurationInfo : TargetConfigurationInfo
    {
        public ReleaseConfigurationInfo() 
        {
            HumanReadableName = "Release";
            IsDebug = false;
            GenerateDebugInformation = false;
            Optimize = true;
        }
    }   // class ReleaseConfigurationInfo
    
    // Template for new configurations:
#if FALSE
    /// <summary>
    /// "" configuration information.
    /// </summary>
    public sealed class ConfigurationInfo : TargetConfigurationInfo
    {
        public ConfigurationInfo()
        {
        }
    }   // class ConfigurationInfo
#endif  // FALSE

}   // namespace GoddamnEngine.BuildSystem
