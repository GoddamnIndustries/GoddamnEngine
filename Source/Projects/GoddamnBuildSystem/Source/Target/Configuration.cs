// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file TargetConfiguration.cs
//! Descriptions for configurations.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! List of supported target configurations.
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Release,
        Development,
        Shipping,
    }   // enum TargetConfigurations

    // ------------------------------------------------------------------------------------------
    //! List of properties for specific configuration.
    public class TargetConfigurationInfo
    {
        private static Dictionary<TargetConfiguration, TargetConfigurationInfo> _cachedInformation;
        public readonly TargetConfiguration Configuration;
        public string HumanReadableName { get; protected set; }
        public bool IsDebug { get; protected set; }
        public bool GenerateDebugInformation { get; protected set; }
        public bool Optimize { get; protected set; }

        // ------------------------------------------------------------------------------------------
        //! Initializes a configuration information. Should setup properties.
        //! @param theConfiguration Desired configuration.
        protected TargetConfigurationInfo(TargetConfiguration configuration)
        {
            Configuration = configuration;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns the information for specific platform.
        //! @param theConfiguration Desired configuration.
        //! @returns The information for specific platform.
        public static TargetConfigurationInfo Get(TargetConfiguration theConfiguration)
        {
            if (_cachedInformation != null) return _cachedInformation[theConfiguration];
            _cachedInformation = new Dictionary<TargetConfiguration, TargetConfigurationInfo>();
            foreach (var configuration in Target.EnumerateAllConfigurations())
            {
                var configurationInfoType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(configuration + "ConfigurationInfo"));
                if (configurationInfoType != null)
                {
                    var configurationInfo = (TargetConfigurationInfo)Activator.CreateInstance(configurationInfoType);
                    _cachedInformation.Add(configuration, configurationInfo);
                }
                else
                {
                    throw new BuildSystemException("Not configuration information exists for configuration {0}.", configuration);
                }
            }

            return _cachedInformation[theConfiguration];
        }
    }   // class TargetConfigurationInfo

    // ------------------------------------------------------------------------------------------
    //! "Debug" configuration information.
    public sealed class DebugConfigurationInfo : TargetConfigurationInfo
    {
        public DebugConfigurationInfo() : base(TargetConfiguration.Debug)
        {
            HumanReadableName = "Debug";
            IsDebug = true;
            GenerateDebugInformation = true;
            Optimize = false;
        }
    }   // class DebugConfigurationInfo

    // ------------------------------------------------------------------------------------------
    //! "Release" configuration information.
    public sealed class ReleaseConfigurationInfo : TargetConfigurationInfo
    {
        public ReleaseConfigurationInfo() : base(TargetConfiguration.Release)
        {
            HumanReadableName = "Release";
            IsDebug = false;
            GenerateDebugInformation = true;
            Optimize = true;
        }
    }   // class ReleaseConfigurationInfo

    // ------------------------------------------------------------------------------------------
    //! "Development" configuration information.
    public sealed class DevelopmentConfigurationInfo : TargetConfigurationInfo
    {
        public DevelopmentConfigurationInfo() : base(TargetConfiguration.Development)
        {
            HumanReadableName = "Development";
            IsDebug = false;
            GenerateDebugInformation = true;
            Optimize = true;
        }
    }   // class DevelopmentConfigurationInfo

    // ------------------------------------------------------------------------------------------
    //! "Shipping" configuration information.
    public sealed class ShippingConfigurationInfo : TargetConfigurationInfo
    {
        public ShippingConfigurationInfo() : base(TargetConfiguration.Shipping)
        {
            HumanReadableName = "Shipping";
            IsDebug = false;
            GenerateDebugInformation = true;
            Optimize = true;
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
