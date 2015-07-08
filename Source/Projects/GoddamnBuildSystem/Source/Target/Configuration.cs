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
    //[
    //! List of supported target configurations.
    public enum TargetConfiguration : byte
    {
        Unknown,
        Debug,
        Release,
        Development,
        Shipping,
    }   // enum TargetConfigurations

    //[
    //! List of properties for specific configuration.
    public class TargetConfigurationInfo
    {
        private static Dictionary<TargetConfiguration, TargetConfigurationInfo> CachedInformation;
        public readonly TargetConfiguration Configuration;
        public string HumanReadableName { get; protected set; }
        public bool IsDebug { get; protected set; }
        public bool GenerateDebugInformation { get; protected set; }
        public bool Optimize { get; protected set; }

        //[
        //! Initializes a configuration information. Should setup properties.
        protected TargetConfigurationInfo()
        {
        }

        //[
        //! Returns the information for specific platform.
        //! @param TheConfiguration Desired configuration.
        //! @returns The information for specific platform.
        public static TargetConfigurationInfo Get(TargetConfiguration TheConfiguration)
        {
            if (CachedInformation != null) return CachedInformation[TheConfiguration];
            CachedInformation = new Dictionary<TargetConfiguration, TargetConfigurationInfo>();
            foreach (var Configuration in Target.EnumerateAllConfigurations())
            {
                var ConfigurationInfoType = Assembly.GetExecutingAssembly().GetTypes().FirstOrDefault(T => T.Name.EndsWith(Configuration + "ConfigurationInfo"));
                if (ConfigurationInfoType != null)
                {
                    var ConfigurationInfo = (TargetConfigurationInfo)Activator.CreateInstance(ConfigurationInfoType);
                    CachedInformation.Add(Configuration, ConfigurationInfo);
                }
                else
                {
                    throw new BuildSystemException("Not configuration information exists for configuration {0}.", Configuration);
                }
            }

            return CachedInformation[TheConfiguration];
        }
    }   // class TargetConfigurationInfo

    //[
    //! "Debug" configuration information.
    // ReSharper disable once UnusedMember.Global
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

    //[
    //! "Release" configuration information.
    // ReSharper disable once UnusedMember.Global
    public sealed class ReleaseConfigurationInfo : TargetConfigurationInfo
    {
        public ReleaseConfigurationInfo()
        {
            HumanReadableName = "Release";
            IsDebug = false;
            GenerateDebugInformation = true;
            Optimize = true;
        }
    }   // class ReleaseConfigurationInfo

    //[
    //! "Development" configuration information.
    // ReSharper disable once UnusedMember.Global
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

    //[
    //! "Shipping" configuration information.
    // ReSharper disable once UnusedMember.Global
    public sealed class ShippingConfigurationInfo : TargetConfigurationInfo
    {
        public ShippingConfigurationInfo()
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
