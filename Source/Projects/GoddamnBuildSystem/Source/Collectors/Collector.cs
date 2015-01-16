//! ==========================================================================================
//! Collector.cs - Collector class definition.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.IO;

namespace GoddamnEngine.BuildSystem
{
    //! @brief Represents an abstract data collector.
    public abstract class Collector
    {
        internal string m_Source = null;
        private string m_Location;
        private string m_Name;

        //! @brief Returns path to source file of this collector.
        //! @returns Path to source file of this collector.
        public string GetSource()
        {
            Debug.Assert(m_Source != null);
            return m_Source;
        }

        //! @brief Returns path directory of this collector.
        //! @returns Path directory of this collector.
        public string GetLocation()
        {
            if (m_Location == null) {
                m_Location = Path.GetDirectoryName(GetSource());
            }

            return m_Location;
        }

        //! @brief Returns name of this collector.
        //! @returns Name of this collector.
        public string GetName()
        {
            if (m_Name == null) {
                m_Name = GetSource();
                while (!string.IsNullOrEmpty(Path.GetExtension(m_Name))) {
                    m_Name = Path.GetFileNameWithoutExtension(m_Name);
                }
            }

            return m_Name;
        }

        //! @brief Returns false is this object should be skipped.
        //! @returns False is this object should be skipped.
        public virtual bool GetIsSupported()
        {
            return true;
        }

        //! @brief Returns true if object on specified path has platform/configuration data and matches it.
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns True if object on specified path has platform/configuration data and matches it.
        public static bool MatchesPlatformConfiguration(string ObjectPath, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            string ObjectPathExtensionless = Path.GetFileNameWithoutExtension(ObjectPath);
            string ObjectPlatform = Path.GetExtension(ObjectPathExtensionless);
            if (!string.IsNullOrEmpty(ObjectPlatform)) {
                // Checking if platform information is defined and it is of type TargetPlatform.
                if (ObjectPlatform == string.Concat('.', Platform)) {
                    string LibraryConfiguration = Path.GetExtension(Path.GetFileNameWithoutExtension(ObjectPathExtensionless));
                    if (!string.IsNullOrEmpty(LibraryConfiguration)) {
                        // Checking if platform information is defined and it is of type TargetConfiguration.
                        return (LibraryConfiguration == string.Concat('.', Configuration));
                    }
                } else {
                    // Library filename contains some platform information, but it not matches target platform.
                    return false;
                }
            }

            return true;
        }
    }   // class TargetCollector

    //! @brief Represents a data structure, that contains pre-cached from collector for different platforms/configurations.
    //! @tparam T Type of data stored in the container.
    public sealed class CollectorPlatformConfigurationData<T>
    {
        private Dictionary<UInt16, T> m_Container;

        //! @brief Packs target platform and configuration into single value.
        //! @param Platform Platform to be packed.
        //! @param Configuration Configuration to be packed.
        //! @returns Packed target platform and configuration as a single value.
        private static UInt16 CompressPlatformConfiguration(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            UInt16 PlatformValue = (byte)Convert.ChangeType(Platform, Platform.GetTypeCode());
            UInt16 ConfigurationValue = (byte)Convert.ChangeType(Configuration, Platform.GetTypeCode());

            return (UInt16)(PlatformValue | (ConfigurationValue << 8));
        }

        //! @brief Initializes the container with all values.
        //! @param Accessor Function that returns a value depending on platform/configuration.
        public CollectorPlatformConfigurationData(Func<TargetPlatform, TargetConfiguration, T> Accessor)
        {
            m_Container = new Dictionary<UInt16, T>();
            foreach (TargetPlatform Platform in Target.EnumerateAllPlatforms()) {
                foreach (TargetConfiguration Configuration in Target.EnumerateAllConfigurations()) {
                    m_Container.Add(CompressPlatformConfiguration(Platform, Configuration), Accessor(Platform, Configuration));
                }
            }
        }

        //! @brief Accesses the platform/configuration specific value of the container.
        //! @param Platform Some platform.
        //! @param Configuration Some configuration.
        //! @return Platform/configuration specific value of the container.
        public T this[TargetPlatform Platform, TargetConfiguration Configuration]
        {
            get { return m_Container[CompressPlatformConfiguration(Platform, Configuration)]; }
            set { m_Container[CompressPlatformConfiguration(Platform, Configuration)] = value; }
        }
    }   // class CollectorPlatformConfigurationData<T>

    //! @brief Contains cached data of some abstract collector.
    //!        Caching properties rules:
    //!        @li Returns target/configuration independent result - just store.
    //!        @li Returns target/configuration dependent result - avoid this.
    //!        @li Returns target dependent result: store as platform-value dictionary.
    //!        @li Returns configuration dependent result: store as delegate of configuration.
    public class CollectorCache
    {
        public dynamic m_AdditionalCache;
        public readonly bool m_IsSupported;
        public readonly string m_CachedSource;
        public readonly string m_CachedName;
        public readonly string m_CachedLocation;

        //! @brief Initializes a new generic collector cache.
        //! @param Collector Collector which dynamic properties would be cached.
        public CollectorCache(Collector Collector)
        {
            m_IsSupported = Collector.GetIsSupported();
            if (m_IsSupported) {
                m_AdditionalCache = new ExpandoObject();
                m_CachedSource = Collector.GetSource();
                m_CachedName = Collector.GetName();
                m_CachedLocation = Collector.GetLocation();
            }
        }
    }   // class TargetCollectorCache

    //! @brief Collects data and generates cache.
    //! @tparam TCollector Type of collector.
    //! @tparam TCache Type of cache of collector.
    public /*static*/ class CollectorFactory<TCollector, TCache>
        where TCollector : Collector
        where TCache : CollectorCache
    {
        //! @brief Constructs new collector instance and cached it data.
        //! @param CollectorSourcePath Path so source file of the collector.
        //! @returns Created instance of cached collector data.
        public static TCache Create(string CollectorSourcePath)
        {
            TCollector Collector = null;
            if (CollectorSourcePath != null) {
                foreach (Type InternalType in CSharpCompiler.CompileSourceFile(CollectorSourcePath).GetTypes()) {
                    if (InternalType.IsSubclassOf(typeof(TCollector))) {
                        Collector = (TCollector)Activator.CreateInstance(InternalType);
                        break;
                    }
                }
            }

            if (Collector == null) {
                Collector = Activator.CreateInstance<TCollector>();
            }

            Collector.m_Source = CollectorSourcePath;

            return (TCache)Activator.CreateInstance(typeof(TCache), Collector);
        }
    }   // class TargetCollectorFactory<TCollector, TCache>
}   // namespace GoddamnEngine.BuildSystem
