//! ==========================================================================================
//! Collector.cs - Collector class definition.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! ==========================================================================================

using System;
using System.Diagnostics;
using System.Dynamic;
using System.IO;

namespace GoddamnEngine.BuildSystem
{
    //! Represents an abstract data collector.
    public abstract class Collector
    {
        internal string m_Source = null;
        private string m_Location;
        private string m_Name;

        //! Returns path to source file of this collector.
        public string GetSource()
        {
            Debug.Assert(m_Source != null);
            return m_Source;
        }

        //! Returns path directory of this collector.
        public string GetLocation()
        {
            if (m_Location == null) {
                m_Location = Path.GetDirectoryName(GetSource());
            }

            return m_Location;
        }

        //! Returns name of this collector.
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

        //! Returns false is this object should be skipped.
        public virtual bool GetIsSupported()
        {
            return true;
        }
    }   // class TargetCollector

    //! Contains cached data of some abstract collector.
    //! Caching properties rules:
    //! @li Returns target/configuration independent result - just store.
    //! @li Returns target/configuration dependent result - avoid this.
    //! @li Returns target dependent result: store as platform-value dictionary.
    //! @li Returns configuration dependent result: store as delegate of configuration.
    public class CollectorCache
    {
        public dynamic m_AdditionalCache;
        public readonly bool m_IsSupported;
        public readonly string m_CachedSource;
        public readonly string m_CachedName;
        public readonly string m_CachedLocation;

        public CollectorCache(Collector BuildTargetCollectorObject)
        {
            m_IsSupported = BuildTargetCollectorObject.GetIsSupported();
            if (m_IsSupported) {
                m_AdditionalCache = new ExpandoObject();
                m_CachedSource = BuildTargetCollectorObject.GetSource();
                m_CachedName = BuildTargetCollectorObject.GetName();
                m_CachedLocation = BuildTargetCollectorObject.GetLocation();
            }
        }
    }   // class TargetCollectorCache

    //! Collects data and generates cache.
    //! @tparam TCollector Type of collector.
    //! @tparam TCache Type of cache of collector.
    public /*static*/ class CollectorFactory<TCollector, TCache>
        where TCollector : Collector
        where TCache : CollectorCache
    {
        //! Constructs new collector instance and cached it data.
        //! @param CollectorSourcePath Path so source file of the collector.
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
