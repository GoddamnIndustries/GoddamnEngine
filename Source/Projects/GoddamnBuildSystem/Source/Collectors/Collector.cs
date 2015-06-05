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

namespace GoddamnEngine.BuildSystem
{
    // ------------------------------------------------------------------------------------------
    //! Represents an abstract data collector.
    public abstract class Collector
    {
        internal string Source = null;
        private  string Location;
        private  string Name;

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
            return Location ?? (Location = Path.GetDirectoryName(GetSource()));
        }

        // ------------------------------------------------------------------------------------------
        //! Returns name of this collector.
        //! @returns Name of this collector.
        public string GetName()
        {
            if (Name == null)
            {
                Name = GetSource();
                while (!string.IsNullOrEmpty(Path.GetExtension(Name)))
                {
                    Name = Path.GetFileNameWithoutExtension(Name);
                }
            }

            return Name;
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
        //! @param Platform One of the target platforms.
        //! @param Configuration One of the target configurations.
        //! @returns True if object on specified path has platform/configuration data and matches it.
        public static bool MatchesPlatformConfiguration(string ObjectPath, TargetPlatform Platform, TargetConfiguration Configuration)
        {
            var ObjectPathExtensionless = Path.GetFileNameWithoutExtension(ObjectPath);
            var ObjectPlatform = Path.GetExtension(ObjectPathExtensionless);
            if (!string.IsNullOrEmpty(ObjectPlatform))
            {
                // Checking if platform information is defined and it is of type TargetPlatform.
                if (ObjectPlatform == string.Concat('.', Platform))
                {
                    var LibraryConfiguration = Path.GetExtension(Path.GetFileNameWithoutExtension(ObjectPathExtensionless));
                    if (!string.IsNullOrEmpty(LibraryConfiguration))
                    {
                        // Checking if platform information is defined and it is of type TargetConfiguration.
                        return LibraryConfiguration == string.Concat('.', Configuration);
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
    //! @tparam T Type of data stored in the container.
    public sealed class CollectorContainer<T>
    {
        private Dictionary<UInt16, T> Container;

        // ------------------------------------------------------------------------------------------
        //! Packs target platform and configuration into single value.
        //! @param Platform Platform to be packed.
        //! @param Configuration Configuration to be packed.
        //! @returns Packed target platform and configuration as a single value.
        private static UInt16 CompressPlatformConfiguration(TargetPlatform Platform, TargetConfiguration Configuration)
        {
            ushort PlatformValue = (byte)Convert.ChangeType(Platform, Platform.GetTypeCode());
            ushort ConfigurationValue = (byte)Convert.ChangeType(Configuration, Platform.GetTypeCode());

            return (ushort)(PlatformValue | (ConfigurationValue << 8));
        }

        // ------------------------------------------------------------------------------------------
        //! Initializes the container with all values.
        //! @param Accessor Function that returns a value depending on platform/configuration.
        public CollectorContainer(Func<TargetPlatform, TargetConfiguration, T> Accessor)
        {
            Container = new Dictionary<ushort, T>();
            foreach (var Platform in Target.EnumerateAllPlatforms())
            {
                foreach (var Configuration in Target.EnumerateAllConfigurations())
                {
                    Container.Add(CompressPlatformConfiguration(Platform, Configuration), Accessor(Platform, Configuration));
                }
            }
        }

        // ------------------------------------------------------------------------------------------
        //! Accesses the platform/configuration specific value of the container.
        //! @param Platform Some platform.
        //! @param Configuration Some configuration.
        //! @return Platform/configuration specific value of the container.
        public T this[TargetPlatform Platform, TargetConfiguration Configuration]
        {
            get { return Container[CompressPlatformConfiguration(Platform, Configuration)]; }
            set { Container[CompressPlatformConfiguration(Platform, Configuration)] = value; }
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
        //! @param Collector Collector which dynamic properties would be cached.
        protected CollectorCache(Collector Collector)
        {
            this.Collector = Collector;
            IsSupported = Collector.GetIsSupported();
            if (IsSupported)
            {
                AdditionalCache = new ExpandoObject();
                CachedSource = Collector.GetSource();
                CachedName = Collector.GetName();
                CachedLocation = Collector.GetLocation();
            }
        }
    }   // class TargetCollectorCache

    // ------------------------------------------------------------------------------------------
    //! Collects data and generates cache.
    //! @tparam TCollector Type of collector.
    //! @tparam TCache Type of cache of collector.
    public abstract class CollectorFactory<TCollector, TCache>
        where TCollector : Collector
        where TCache : CollectorCache
    {
        private readonly static Dictionary<string, TCache> CachedCache = new Dictionary<string, TCache>();

        // ------------------------------------------------------------------------------------------
        //! Constructs new collector instance and cached it data.
        //! @param CollectorSourcePath Path so source file of the collector.
        //! @returns Created instance of cached collector data.
        public static TCache Create(string CollectorSourcePath)
        {
            if (!CachedCache.ContainsKey(CollectorSourcePath))
            {
                TCollector Collector = null;
                if (CollectorSourcePath != null)
                {
                    foreach (Type InternalType in CSharpCompiler.CompileSourceFile(CollectorSourcePath).GetTypes())
                    {
                        if (InternalType.IsSubclassOf(typeof(TCollector)))
                        {
                            Collector = (TCollector)Activator.CreateInstance(InternalType);
                            break;
                        }
                    }
                }

                if (Collector == null)
                {
                    Collector = Activator.CreateInstance<TCollector>();
                }

                Collector.Source = CollectorSourcePath;

                var Cache = (TCache)Activator.CreateInstance(typeof(TCache), Collector);
                CachedCache.Add(CollectorSourcePath, Cache);

                return Cache;
            }
            else
            {
                return CachedCache[CollectorSourcePath];
            }
        }
    }   // class TargetCollectorFactory<TCollector, TCache>
}   // namespace GoddamnEngine.BuildSystem
