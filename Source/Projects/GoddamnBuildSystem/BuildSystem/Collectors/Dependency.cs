// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Collections.Generic;
using System.Dynamic;
using System.IO;
using System.Linq;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.Collectors
{
    /// <summary>
    /// Dependency file type.
    /// </summary>
    public enum DependencyFileType
    {
        StaticLibrary,
        DynamicLibrary,
    }   // enum DependencyFileType

    /// <summary>
    /// Dependency file.
    /// </summary>
    public class DependencyFile
    {
        /// <summary>
        /// Path to the dependency file.
        /// </summary>
        public readonly string FilePath;

        /// <summary>
        /// Type of the dependency file.
        /// </summary>
        public readonly DependencyFileType FileType;

        /// <summary>
        /// Empty object that can be used internally.
        /// </summary>
        internal readonly dynamic FileMisc = new ExpandoObject();

        /// <summary>
        /// Initializes a dependency file.
        /// </summary>
        /// <param name="filePath">File path.</param>
        /// <param name="fileType">File type.</param>
        public DependencyFile(string filePath, DependencyFileType fileType)
        {
            FilePath = filePath;
            FileType = fileType;
        }
    }   // class DependencyFile

    /// <summary>
    /// Project dependency enumerator.
    /// </summary>
    /// <inheritdoc />
    public class DependencyEnumerator : CollectorEnumerator
    {
        /// <summary>
        /// Enumerates list of directories that contain header files.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual IEnumerable<string> EnumerateHeaderDirectories(TargetPlatform platform, TargetConfiguration configuration)
        {
            // TODO: '.framework' bundles on Apple platforms.
            var standardIncludePath = Path.Combine(GetLocation(), "include");
            if (Directory.Exists(standardIncludePath))
            {
                yield return standardIncludePath;
            }
        }

        /// <summary>
        /// Enumerates list of libraries that should be linked with project build file.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual IEnumerable<DependencyFile> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
        {
            foreach (var libraryFile in Directory.EnumerateFiles(GetLocation()))
            {
                if (!MatchesPlatformConfiguration(libraryFile, platform, configuration))
                {
                    continue;
                }
                var libraryFileExtension = (Path.GetExtension(libraryFile) ?? "").ToLowerInvariant();
                if (TargetInfo.IsMicrosoftPlatform(platform))
                {
                    switch (libraryFileExtension)
                    {
                        case ".lib":
                            yield return new DependencyFile(libraryFile, DependencyFileType.StaticLibrary);
                            break;
                    }
                }
                else if (TargetInfo.IsApplePlatform(platform))
                {
                    // TODO: '.framework' bundles on Apple platforms.
                    switch (libraryFileExtension)
                    {
                        case ".a":
                            yield return new DependencyFile(libraryFile, DependencyFileType.StaticLibrary);
                            break;
                        case ".dylib":
                            yield return new DependencyFile(libraryFile, DependencyFileType.DynamicLibrary);
                            break;
                    }
                }
                else if (TargetInfo.IsPosixPlatform(platform))
                {
                    switch (libraryFileExtension)
                    {
                        case ".a":
                            yield return new DependencyFile(libraryFile, DependencyFileType.StaticLibrary);
                            break;
                        case ".so":
                            yield return new DependencyFile(libraryFile, DependencyFileType.DynamicLibrary);
                            break;
                    }
                }
                else
                {
                    throw new NotSupportedException("Unknown target platform for depenedency linked libraries.");
                }
            }
        }

        /// <summary>
        /// Enumerates list of files that should be copied to project build output directory.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <param name="configuration">One of the target configurations.</param>
        public virtual IEnumerable<DependencyFile> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
        {
            foreach (var copyFilePath in Directory.EnumerateFiles(GetLocation()))
            {
                if (!MatchesPlatformConfiguration(copyFilePath, platform, configuration))
                {
                    continue;
                }

                var copyFileExtension = (Path.GetExtension(copyFilePath) ?? "").ToLowerInvariant();
                if (TargetInfo.IsMicrosoftPlatform(platform))
                {
                    switch (copyFileExtension)
                    {
                        case ".dll":
                            yield return new DependencyFile(copyFilePath, DependencyFileType.DynamicLibrary);
                            break;
                    }
                }
                else if (TargetInfo.IsApplePlatform(platform))
                {
                    switch (copyFileExtension)
                    {
                        case ".dylib":
                            yield return new DependencyFile(copyFilePath, DependencyFileType.DynamicLibrary);
                            break;
                    }
                }
				else if (TargetInfo.IsPosixPlatform(platform))
				{
					switch (copyFileExtension)
					{
						case ".so":
							yield return new DependencyFile(copyFilePath, DependencyFileType.DynamicLibrary);
							break;
					}
				}
                else
                {
                    throw new NotSupportedException("Unknown target platform for depenedency copy files.");
                }
            }
        }
	}   // public class DependencyEnumerator

	public class PkgConfigDependencyEnumerator : DependencyEnumerator
	{
		private string Run(string cmd, string args)
		{
			var process = new System.Diagnostics.Process();
			process.StartInfo.FileName = cmd;
			process.StartInfo.Arguments = args;
            process.StartInfo.EnvironmentVariables["PKG_CONFIG_PATH"] = "/usr/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig";
			process.StartInfo.RedirectStandardOutput = true;
			process.StartInfo.RedirectStandardError = true;
			process.StartInfo.CreateNoWindow = true;
			process.StartInfo.UseShellExecute = false;
			process.Start();

			process.WaitForExit();
			var v = process.ExitCode;
			return process.StandardOutput.ReadToEnd() + process.StandardError.ReadToEnd();
		}

		public override bool GetIsSupported(TargetPlatform platform, TargetConfiguration configuration)
		{
			return platform == TargetPlatform.Linux;
		}

		public override IEnumerable<string> EnumerateHeaderDirectories(TargetPlatform platform, TargetConfiguration configuration)
		{
            var s = Run("pkg-config", "--cflags-only-I gtk+-3.0").Split(' ');
            foreach (var i in s)
            {
                yield return i.Substring(2);
            }
		}

		public override IEnumerable<DependencyFile> EnumerateLinkedLibraries(TargetPlatform platform, TargetConfiguration configuration)
		{
            var s = Run("pkg-config", "--libs-only-l gtk+-3.0").Split(' ');
            foreach (var i in s)
            {
                yield return new DependencyFile(i.Substring(2), DependencyFileType.DynamicLibrary);
            }
		}

		public override IEnumerable<DependencyFile> EnumerateCopyFiles(TargetPlatform platform, TargetConfiguration configuration)
		{
			yield break;
		}
	}	// public class PkgConfigDependencyEnumerator

    /// <summary>
    /// Dependency object.
    /// </summary>
    /// <inheritdoc />
    public sealed class Dependency : Collector
    {
        /// <summary>
        /// List of directories that contain header files.
        /// </summary>
        public readonly CollectorContainer<string[]> HeaderDirectories;

        /// <summary>
        /// List of libraries that should be linked with project build file.
        /// </summary>
        public readonly CollectorContainer<DependencyFile[]> LinkedLibraries;

        /// <summary>
        /// List of files that should be copied to project build output directory.
        /// </summary>
        public readonly CollectorContainer<DependencyFile[]> CopyFiles;

        /// <inheritdoc />
        public Dependency(DependencyEnumerator dependencyEnumerator)
            : base(dependencyEnumerator)
        {
            HeaderDirectories 
                = new CollectorContainer<string[]>((p, c) => dependencyEnumerator.EnumerateHeaderDirectories(p, c).ToArray());
            LinkedLibraries 
                = new CollectorContainer<DependencyFile[]>((p, c) => dependencyEnumerator.EnumerateLinkedLibraries(p, c).ToArray());
            CopyFiles 
                = new CollectorContainer<DependencyFile[]>((p, c) => dependencyEnumerator.EnumerateCopyFiles(p, c).ToArray());
        }
    }   // public class Dependency

    /// <summary>
    /// Factory of dependencies.
    /// </summary>
    public static class DependencyFactory
    {
        /// <summary>
        /// Constructs new dependency instance and it's cached data.
        /// </summary>
        /// <param name="dependencySourcePath">Path so source file of the dependency.</param>
        /// <returns>Created instance of cached dependency data.</returns>
        public static Dependency Create(string dependencySourcePath)
        {
            return CollectorFactory<DependencyEnumerator, Dependency>.Create(dependencySourcePath);
        }
    }   // class DependencyFactory
}   // namespace GoddamnEngine.BuildSystem
