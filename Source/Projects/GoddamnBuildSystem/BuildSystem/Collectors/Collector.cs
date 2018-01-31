// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.IO;
using System.Dynamic;
using System.Diagnostics;
using System.Collections.Generic;

using GoddamnEngine.BuildSystem.Target;
using GoddamnEngine.BuildSystem.Support;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Abstract data collector interface.
    /// </summary>
    public interface ICollector
    {
        /// <summary>
        /// Returns path to source file of this collector.
        /// </summary>
        string GetSource();

        /// <summary>
        /// Returns path directory of this collector.
        /// </summary>
        string GetLocation();

        /// <summary>
        /// Returns name of this collector.
        /// </summary>
        string GetName();

        /// <summary>
        /// Returns false is this object should be skipped.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        bool GetIsSupported(TargetPlatform platform, TargetConfiguration configuration);
    }   // public interface ICollector

    /// <summary>
    /// Abstract data collector.
    /// </summary>
    public class Collector : ICollector
    {
        internal string Source;
        string m_Location;
        string m_Name;

        /// <summary>
        /// Returns path to source file of this collector.
        /// </summary>
        public string GetSource()
        {
            Debug.Assert(Source != null);
            return Source;
        }

        /// <summary>
        /// Returns path directory of this collector.
        /// </summary>
        public string GetLocation()
        {
            return m_Location ?? (m_Location = Path.GetDirectoryName(GetSource()));
        }

        /// <summary>
        /// Returns name of this collector.
        /// </summary>
        public string GetName()
        {
            if (m_Name == null)
            {
                m_Name = GetSource();
                while (!string.IsNullOrEmpty(Path.GetExtension(m_Name)))
                {
                    m_Name = Path.GetFileNameWithoutExtension(m_Name);
                }
            }
            return m_Name;
        }

        /// <summary>
        /// Returns false is this object should be skipped.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual bool GetIsSupported(TargetPlatform platform, TargetConfiguration configuration)
        {
            return true;
        }

        /// <summary>
        /// Returns true if object on specified path has platform/configuration data and matches it.
        /// </summary>
        /// <param name="objectPath">Path to check.</param>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public static bool MatchesPlatformConfiguration(string objectPath, TargetPlatform platform, TargetConfiguration configuration)
        {
            var objectPathExtensionless = Path.GetFileNameWithoutExtension(objectPath);
            var objectPlatform = Path.GetExtension(objectPathExtensionless);
            if (!string.IsNullOrEmpty(objectPlatform))
            {
                // Checking if platform information is defined and it is of type TargetPlatform.
                if (objectPlatform == string.Concat('.', platform))
                {
                    var libraryConfiguration = Path.GetExtension(Path.GetFileNameWithoutExtension(objectPathExtensionless));
                    if (!string.IsNullOrEmpty(libraryConfiguration))
                    {
                        // Checking if platform information is defined and it is of type TargetConfiguration.
                        return libraryConfiguration == string.Concat('.', configuration);
                    }
                }
                else
                {
                    // Library filename contains some platform information, but it not matches target platform.
                    return false;
                }
            }
            return true;
        }
    }   // class TargetCollector

    /// <summary>
    /// Represents a data structure, that contains pre-cached from collector for different platforms/configurations.
    /// </summary>
    /// <typeparam name="T">Type of data stored in the container.</typeparam>
    public sealed class CollectorContainer<T>
    {
        readonly Dictionary<ushort, T> m_Container;

        static ushort CompressPlatformConfiguration(TargetPlatform platform, TargetConfiguration configuration)
        {
            ushort platformValue = (byte)Convert.ChangeType(platform, platform.GetTypeCode());
            ushort configurationValue = (byte)Convert.ChangeType(configuration, platform.GetTypeCode());

            return (ushort)(platformValue | (configurationValue << 8));
        }

        /// <summary>
        /// Initializes the container with all values.
        /// </summary>
        /// <param name="accessor">Function that returns a value depending on platform/configuration.</param>
        public CollectorContainer(Func<TargetPlatform, TargetConfiguration, T> accessor)
        {
            m_Container = new Dictionary<ushort, T>();
            foreach (var platform in TargetInfo.EnumerateAllPlatforms())
            {
                foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                {
                    m_Container.Add(CompressPlatformConfiguration(platform, configuration), accessor(platform, configuration));
                }
            }
        }

        /// <summary>
        /// Accesses the platform/configuration specific value of the container.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        /// <returns>Platform/configuration specific value of the container.</returns>
        public T this[TargetPlatform platform, TargetConfiguration configuration]
        {
            get { return m_Container[CompressPlatformConfiguration(platform, configuration)]; }
            set { m_Container[CompressPlatformConfiguration(platform, configuration)] = value; }
        }

    }   // class CollectorContainer<T>

    /// <summary>
    /// Contains cached data of some abstract collector.
    /// </summary>
    public class CollectorCache
    {
        public readonly Collector Collector;
        public readonly dynamic Misc;
        public readonly bool IsSupported;
        public readonly string CachedSource;
        public readonly string CachedName;
        public readonly string CachedLocation;

        /// <summary>
        /// Initializes a new generic collector cache.
        /// </summary>
        /// <param name="collector">Collector which dynamic properties would be cached.</param>
        protected CollectorCache(Collector collector)
        {
            Collector = collector;
            IsSupported = true;//collector.GetIsSupported();
            if (IsSupported)
            {
                Misc = new ExpandoObject();
                CachedSource = collector.GetSource();
                CachedName = collector.GetName();
                CachedLocation = collector.GetLocation();
            }
        }
    }   // class TargetCollectorCache

    /// <summary>
    /// Collects data and generates cache.
    /// </summary>
    /// <typeparam name="TCollector">Type of collector.</typeparam>
    /// <typeparam name="TCollectorCache">Type of cache of collector.</typeparam>
    public static class CollectorFactory<TCollector, TCollectorCache> 
        where TCollector : Collector 
        where TCollectorCache : CollectorCache
    {
#pragma warning disable RECS0108
        static readonly Dictionary<string, TCollectorCache> s_CachedCache = new Dictionary<string, TCollectorCache>();
#pragma warning restore RECS0108

        /// <summary>
        /// Constructs new collector instance and cached it data.
        /// </summary>
        /// <param name="collectorSourcePath">Path so source file of the collector.</param>
        /// <returns>Created instance of cached collector data.</returns>
        public static TCollectorCache Create(string collectorSourcePath)
        {
            var safeCollectorSourcePath = collectorSourcePath ?? typeof(TCollector).Name;
            if (!s_CachedCache.ContainsKey(safeCollectorSourcePath))
            {
                TCollector collector = null;
                if (collectorSourcePath != null)
                {
                    var assembly = CSharpCompiler.CompileSourceFile(collectorSourcePath);
                    var assemblyExportedTypes = assembly.GetExportedTypes();
                    foreach (var assemblyExportedType in assemblyExportedTypes)
                    {
                        if (assemblyExportedType.IsSubclassOf(typeof (TCollector)))
                        {
                            collector = (TCollector) Activator.CreateInstance(assemblyExportedType);
                            break;
                        }
                    }
                }

                collector = collector ?? Activator.CreateInstance<TCollector>();
                collector.Source = collectorSourcePath;

                var cache = (TCollectorCache)Activator.CreateInstance(typeof(TCollectorCache), collector);
                s_CachedCache.Add(safeCollectorSourcePath, cache);
                return cache;
            }
            return s_CachedCache[safeCollectorSourcePath];
        }

    }   // class TargetCollectorFactory<TCollector, TCache>

}   // namespace GoddamnEngine.BuildSystem
