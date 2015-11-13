// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Collector.cs
//! Collector class definition.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.IO;
using System.Linq;

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! Represents an abstract data collector.
    public class Collector
    {
        internal string Source;
        private  string _location;
        private  string _name;

        // ------------------------------------------------------------------------------------------
        //! Returns path to source file of this collector.
        //! @returns Path to source file of this collector.
        public string GetSource()
        {
            Debug.Assert(Source != null);
            return Source;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns path directory of this collector.
        //! @returns Path directory of this collector.
        public string GetLocation()
        {
            return _location ?? (_location = Path.GetDirectoryName(GetSource()));
        }

        // ------------------------------------------------------------------------------------------
        //! Returns name of this collector.
        //! @returns Name of this collector.
        public string GetName()
        {
            if (_name == null)
            {
                _name = GetSource();
                while (!string.IsNullOrEmpty(Path.GetExtension(_name)))
                {
                    _name = Path.GetFileNameWithoutExtension(_name);
                }
            }

            return _name;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns false is this object should be skipped.
        //! @returns False is this object should be skipped.
        public virtual bool GetIsSupported()
        {
            return true;
        }

        // ------------------------------------------------------------------------------------------
        //! Returns true if object on specified path has platform/configuration data and matches it.
        //! @param objectPath Path to check.
        //! @param platform One of the target platforms.
        //! @param configuration One of the target configurations.
        //! @returns True if object on specified path has platform/configuration data and matches it.
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

    // ------------------------------------------------------------------------------------------
    //! Represents a data structure, that contains pre-cached from collector for different platforms/configurations.
    //! @tparam T Type of data stored in the m_Container.
    public sealed class CollectorContainer<T>
    {
        private readonly Dictionary<ushort, T> _container;

        // ------------------------------------------------------------------------------------------
        //! Packs target platform and configuration into single value.
        //! @param platform Platform to be packed.
        //! @param configuration Configuration to be packed.
        //! @returns Packed target platform and configuration as a single value.
        private static ushort CompressPlatformConfiguration(TargetPlatform platform, TargetConfiguration configuration)
        {
            // ReSharper disable once PossibleNullReferenceException
            ushort platformValue = (byte)Convert.ChangeType(platform, platform.GetTypeCode());
            // ReSharper disable once PossibleNullReferenceException
            ushort configurationValue = (byte)Convert.ChangeType(configuration, platform.GetTypeCode());

            return (ushort)(platformValue | (configurationValue << 8));
        }

        // ------------------------------------------------------------------------------------------
        //! Initializes the m_Container with all values.
        //! @param accessor Function that returns a value depending on platform/configuration.
        public CollectorContainer(Func<TargetPlatform, TargetConfiguration, T> accessor)
        {
            _container = new Dictionary<ushort, T>();
            foreach (var platform in Target.EnumerateAllPlatforms())
            {
                foreach (var configuration in Target.EnumerateAllConfigurations())
                {
                    _container.Add(CompressPlatformConfiguration(platform, configuration), accessor(platform, configuration));
                }
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Accesses the platform/configuration specific value of the m_Container.
        //! @param platform Some platform.
        //! @param configuration Some configuration.
        //! @return Platform/configuration specific value of the m_Container.
        public T this[TargetPlatform platform, TargetConfiguration configuration]
        {
            get { return _container[CompressPlatformConfiguration(platform, configuration)]; }
            set { _container[CompressPlatformConfiguration(platform, configuration)] = value; }
        }
    }   // class CollectorContainer<T>

    // ------------------------------------------------------------------------------------------
    //! Contains cached data of some abstract collector.
    public class CollectorCache
    {
        public readonly Collector Collector;
        public dynamic AdditionalCache;
        public readonly bool IsSupported;
        public readonly string CachedSource;
        public readonly string CachedName;
        public readonly string CachedLocation;

        // ------------------------------------------------------------------------------------------
        //! Initializes a new generic collector cache.
        //! @param collector Collector which dynamic properties would be cached.
        protected CollectorCache(Collector collector)
        {
            Collector = collector;
            IsSupported = collector.GetIsSupported();
            if (IsSupported)
            {
                AdditionalCache = new ExpandoObject();
                CachedSource = collector.GetSource();
                CachedName = collector.GetName();
                CachedLocation = collector.GetLocation();
            }
        }
    }   // class TargetCollectorCache

    // ------------------------------------------------------------------------------------------
    //! Collects data and generates cache.
    //! @tparam TCollector Type of collector.
    //! @tparam TCache Type of cache of collector.
    public class CollectorFactory<TCollector, TCache>
        where TCollector : Collector
        where TCache : CollectorCache
    {
        private readonly static Dictionary<string, TCache> CachedCache = new Dictionary<string, TCache>();

        // ------------------------------------------------------------------------------------------
        //! Constructs new collector instance and cached it data.
        //! @param collectorSourcePath Path so source file of the collector.
        //! @returns Created instance of cached collector data.
        public static TCache Create(string collectorSourcePath)
        {
            var safeCollectorSourcePath = collectorSourcePath ?? typeof(TCollector).Name;
            if (!CachedCache.ContainsKey(safeCollectorSourcePath))
            {
                TCollector collector = null;
                if (collectorSourcePath != null)
                {
                    foreach (var internalType in CSharpCompiler.CompileSourceFile(collectorSourcePath).GetTypes()
                        .Where(internalType => internalType.IsSubclassOf(typeof(TCollector))))
                    {
                        collector = (TCollector)Activator.CreateInstance(internalType);
                        break;
                    }
                }

                if (collector == null)
                {
                    collector = Activator.CreateInstance<TCollector>();
                }

                collector.Source = collectorSourcePath;
                var cache = (TCache)Activator.CreateInstance(typeof(TCache), collector);
                CachedCache.Add(safeCollectorSourcePath, cache);

                return cache;
            }
            return CachedCache[safeCollectorSourcePath];
        }
    }   // class TargetCollectorFactory<TCollector, TCache>
}   // namespace GoddamnEngine.BuildSystem
