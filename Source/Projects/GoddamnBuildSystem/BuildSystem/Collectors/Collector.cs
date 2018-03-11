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
using System.Linq;
using GoddamnEngine.BuildSystem.Target;
using GoddamnEngine.BuildSystem.Support;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Abstract data collector enumerator.
    /// </summary>
    public class CollectorEnumerator
    {
        internal string Source;
        private string m_Location;
        private string m_Name;

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
        protected static bool MatchesPlatformConfiguration(string objectPath, TargetPlatform platform, TargetConfiguration configuration)
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
    }   // public class CollectorEnumerator

    /// <summary>
    /// Pre-cached from collector enumerator for different platforms/configurations.
    /// </summary>
    /// <typeparam name="T">Type of data stored in the container.</typeparam>
    public sealed class CollectorContainer<T>
    {
        private readonly Dictionary<ushort, T> m_Container;
        private static ushort CompressPlatformConfiguration(TargetPlatform platform, TargetConfiguration configuration)
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
        public T this[TargetPlatform platform, TargetConfiguration configuration]
        {
            get
            {
                if (platform == TargetPlatform.Any)
                {
                    platform = TargetInfo.EnumerateAllPlatforms().First();
                }
                if (configuration == TargetConfiguration.Any)
                {
                    configuration = TargetInfo.EnumerateAllConfigurations().First();
                }
                return m_Container[CompressPlatformConfiguration(platform, configuration)];
            }
        }
    }   // public class CollectorContainer<T>

    /// <summary>
    /// Abstract collector.
    /// </summary>
    public abstract class Collector
    {
        /// <summary>
        /// Name of the collector.
        /// </summary>
        public readonly string Name;

        /// <summary>
        /// Path to the source file of the collector.
        /// </summary>
        public readonly string Source;

        /// <summary>
        /// Directory of the collector.
        /// </summary>
        public readonly string Location;

        /// <summary>
        /// Enumerator of the collector.
        /// </summary>
        public readonly CollectorEnumerator Enumerator;

        /// <summary>
        /// List of supported platforms.
        /// </summary>
        public readonly CollectorContainer<bool> IsSupported;

        /// <summary>
        /// Empty object that can be used internally.
        /// </summary>
        internal readonly dynamic Misc = new ExpandoObject();

        /// <summary>
        /// Initializes a new generic collector cache.
        /// </summary>
        /// <param name="enumerator">Collector which dynamic properties would be cached.</param>
        protected Collector(CollectorEnumerator enumerator)
        {
            Name = enumerator.GetName();
            Source = enumerator.GetSource();
            Location = enumerator.GetLocation();
            Enumerator = enumerator;
            IsSupported = new CollectorContainer<bool>(enumerator.GetIsSupported);
        }
    }   // public class Collector

    /// <summary>
    /// Collects data and generates cache.
    /// </summary>
    /// <typeparam name="TCollectorEnumerator">Type of the collector enumerator.</typeparam>
    /// <typeparam name="TCollector">Type of the collector.</typeparam>
    public static class CollectorFactory<TCollectorEnumerator, TCollector> where TCollectorEnumerator : CollectorEnumerator where TCollector : Collector
    {
#pragma warning disable RECS0108
        private static readonly Dictionary<string, TCollector> s_CachedCache = new Dictionary<string, TCollector>();
#pragma warning restore RECS0108

        /// <summary>
        /// Constructs new collector instance and cached it data.
        /// </summary>
        /// <param name="collectorSourcePath">Path so source file of the collector.</param>
        /// <returns>Created instance of cached collector data.</returns>
        public static TCollector Create(string collectorSourcePath)
        {
            var safeCollectorSourcePath = collectorSourcePath ?? typeof(TCollectorEnumerator).Name;
            if (!s_CachedCache.ContainsKey(safeCollectorSourcePath))
            {
                TCollectorEnumerator collector = null;
                if (collectorSourcePath != null)
                {
                    var assembly = CSharpCompiler.CompileSourceFile(collectorSourcePath);
                    var assemblyExportedTypes = assembly.GetExportedTypes();
                    foreach (var assemblyExportedType in assemblyExportedTypes)
                    {
                        if (assemblyExportedType.IsSubclassOf(typeof (TCollectorEnumerator)))
                        {
                            collector = (TCollectorEnumerator) Activator.CreateInstance(assemblyExportedType);
                            break;
                        }
                    }
                }

                collector = collector ?? Activator.CreateInstance<TCollectorEnumerator>();
                collector.Source = collectorSourcePath;

                var cache = (TCollector)Activator.CreateInstance(typeof(TCollector), collector);
                s_CachedCache.Add(safeCollectorSourcePath, cache);
                return cache;
            }
            return s_CachedCache[safeCollectorSourcePath];
        }
    }   // class CollectorFactory<TCollector, TCache>
}   // namespace GoddamnEngine.BuildSystem
