// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using GoddamnEngine.BuildSystem.Collectors;
using GoddamnEngine.BuildSystem.Target;
using GoddamnEngine.BuildSystem.Support;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    /// <summary>
    /// Generator of the project/solution files for XCode.
    /// </summary>
    /// <inheritdoc />
    public class XCodeProjectGenerator : ProjectGenerator
    {
        
        /// <summary>
        /// Checks if specified platform is natively supported by VS.
        /// </summary>
        /// <param name="platform">One of the target platforms.</param>
        /// <returns>True if platform is natively supported by VS, false otherwise.</returns>
        public override bool IsPlatformNativelySupported(TargetPlatform platform)
        {
            switch (platform)
            {
                case TargetPlatform.MacOS:
                    return true;
            }
            return false;
        }

        // ==========================================================================================
        // Generating XCODEPROJ/PBXPROJ files.
        // ==========================================================================================

        private sealed class PbxUUID
        {
            private static readonly Random s_Random = new Random();
            private readonly int m_A = s_Random.Next();
            private readonly int m_B = s_Random.Next();
            private readonly int m_C = s_Random.Next();
            public override string ToString() 
            {
                return $"{m_A:X}{m_B:X}{m_C:X}";
            } 
        }   // class PbxUUID

        private sealed class PbxGroup
        {
            public readonly PbxUUID UUID = new PbxUUID();
            public readonly Dictionary<string, PbxGroup> ChildGroups = new Dictionary<string, PbxGroup>();
            public readonly List<PbxUUID> ChildFileRefs = new List<PbxUUID>();
            public void Add(string[] fileRefGroup, PbxUUID fileRefUUID)
            {
                if (fileRefGroup.Length != 0)
                {
                    if (!ChildGroups.TryGetValue(fileRefGroup[0], out PbxGroup childGroup))
                    {
                        childGroup = new PbxGroup();
                        ChildGroups.Add(fileRefGroup[0], childGroup);
                    }
                    childGroup.Add(fileRefGroup.SubArray(1), fileRefUUID);
                }
                else
                {
                    Add(fileRefUUID);
                }
            }
            public void Add(PbxUUID fileRefUUID)
            {
                ChildFileRefs.Add(fileRefUUID);
            }
            public void Traverse(Action<string, PbxGroup> traverseFunc)
            {
                traverseFunc(null, this);
                TraverseInternal(traverseFunc);
            }
            private void TraverseInternal(Action<string, PbxGroup> traverseFunc)
            {
                foreach (var childGroupPair in ChildGroups)
                {
                    childGroupPair.Value.TraverseInternal(traverseFunc);
                    traverseFunc(childGroupPair.Key, childGroupPair.Value);
                }
            }
        }   // class PbxGroup

        /// <summary>
        /// Generates project files for XCode: '.xcodeproj'.
        /// </summary>
        /// <param name="project">Parsed project object.</param>
        /// <returns>Path to XCode '.xcodeproj' file.</returns>
        /// <inheritdoc />
        public sealed override string GenerateProjectFiles(Project project)
        {
            var xcodeProjPath = Path.Combine(base.GenerateProjectFiles(project), project.Name) + ".xcodeproj";
            var xcodePbxProjPath = Path.Combine(xcodeProjPath, "project.pbxproj");
            Directory.CreateDirectory(xcodeProjPath);

            using (var xcodeProj = new StreamWriter(xcodePbxProjPath))
            {
                // ==========================================================================================
                // Begin .XCODEPROJ.PBXPROJ
                // ==========================================================================================

                var platform = TargetPlatform.MacOS;
                //var configuration = TargetConfiguration.Debug;

                xcodeProj.WriteLine(@"// !$*UTF8*$!
                    // Generated by GoddamnBuildSystem. Please, do not edit this file manually.
                    {
                        archiveVersion = 1;
                        classes = {
                        };
                        objectVersion = 48;
                        objects = {");
                
                // ------------------------------------------------------------------------------------------
                // Referenced files section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXFileReference section */");

                // Writing down product reference..
                string xcodeProductExplicitFileType;
                switch (project.BuildType[platform, TargetConfiguration.Debug])
                {
                    case ProjectBuildType.Application:
                        xcodeProductExplicitFileType = "compiled.mach-o.executable";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        xcodeProductExplicitFileType = "compiled.mach-o.a";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        xcodeProductExplicitFileType = "compiled.mach-o.dylib";
                        break;
                    default:
                        throw new NotSupportedException();
                }

                var xcodeProductRefUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeProductRefUUID} /*{project.Name}*/ = {{
                        isa = PBXFileReference;
                        explicitFileType = ""{xcodeProductExplicitFileType}"";
                        includeInIndex = 0;
                        path = {project.Name};
                        sourceTree = BUILT_PRODUCTS_DIR;
                    }};");

                // Writing down referenced dependencies..
                foreach (var projectDependency in project.Dependencies[platform, TargetConfiguration.Any])
                {
                    foreach (var projectDependencyFramework in projectDependency.LinkedLibraries[platform, TargetConfiguration.Debug])
                    {
                        string xcodeDependencyRefLastKnownType;
                        switch (projectDependencyFramework.FileType)
                        {
                            case DependencyFileType.StaticLibrary:
                                xcodeDependencyRefLastKnownType = "archive.ar";
                                break;
                            case DependencyFileType.DynamicLibrary:
                                xcodeDependencyRefLastKnownType = "compiled.mach-o.dylib";
                                break;
                            default:
                                throw new NotSupportedException();
                        }

                        projectDependencyFramework.FileMisc.RefUUID = new PbxUUID();
                        xcodeProj.WriteLine($@"
                            {projectDependencyFramework.FileMisc.RefUUID} /*{projectDependencyFramework.FilePath}*/ = {{
                                isa = PBXFileReference;
                                lastKnownFileType = {xcodeDependencyRefLastKnownType};
                                path = {projectDependencyFramework.FilePath};
                                name = {Path.GetFileName(projectDependencyFramework.FilePath)};
                                sourceTree = ""<group>"";
                            }};");
                    }
                }

                // Writing down referenced files..
                foreach (var projectSource in project.Files[platform, TargetConfiguration.Any])
                {
                    string xcodeFileRefLastKnownType;
                    switch (projectSource.FileType)
                    {
                        case ProjectSourceFileType.SourceCode:
                            xcodeFileRefLastKnownType = "sourcecode.cpp.cpp";
                            break;
                        case ProjectSourceFileType.SourceCodeObjective:
                            xcodeFileRefLastKnownType = "sourcecode.cpp.objcpp";
                            break;
                        case ProjectSourceFileType.SourceCodeAssembler:
                            xcodeFileRefLastKnownType = "sourcecode.asm";
                            break;
                        case ProjectSourceFileType.HeaderFile:
                        case ProjectSourceFileType.InlineImplementation:
                            xcodeFileRefLastKnownType = "sourcecode.cpp.h";
                            break;
                        case ProjectSourceFileType.ResourceScript:
                            xcodeFileRefLastKnownType = "file.txt";
                            break;
                        default:
                            throw new NotSupportedException();
                    }

                    projectSource.FileMisc.RefUUID = new PbxUUID();
                    xcodeProj.WriteLine($@"
                        {projectSource.FileMisc.RefUUID} /*{projectSource.FilePath}*/ = {{
                            isa = PBXFileReference;
                            lastKnownFileType = {xcodeFileRefLastKnownType};
                            path = {projectSource.FilePath};
                            name = {Path.GetFileName(projectSource.FilePath)};
                            sourceTree = ""<group>"";
                        }};");
                }

                xcodeProj.WriteLine("/* End PBXFileReference section */\n");

                // ------------------------------------------------------------------------------------------
                // Build Files section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXBuildFile section */");

                // Product..
                var xcodeProductBuildUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeProductBuildUUID} /*{project.Name}*/ = {{
                        isa = PBXBuildFile;
                        fileRef = {xcodeProductRefUUID};
                    }};");

                // Frameworks..
                foreach (var projectDependency in project.Dependencies[platform, TargetConfiguration.Any])
                {
                    foreach (var projectDependencyFramework in projectDependency.LinkedLibraries[platform, TargetConfiguration.Debug])
                    {
                        projectDependencyFramework.FileMisc.BuildUUID = new PbxUUID();
                        xcodeProj.WriteLine($@"
                            {projectDependencyFramework.FileMisc.BuildUUID} /*{projectDependencyFramework.FilePath}*/ = {{
                                isa = PBXBuildFile;
                                fileRef = {projectDependencyFramework.FileMisc.RefUUID};
                            }};");
                    }
                }

                // Source files..
                foreach (var projectSource in project.Files[platform, TargetConfiguration.Any])
                {
                    // Writing down only files that need to be compiled..
                    switch (projectSource.FileType)
                    {
                        case ProjectSourceFileType.SourceCode:
                        case ProjectSourceFileType.SourceCodeObjective:
                        case ProjectSourceFileType.SourceCodeAssembler:
                            break;
                        default:
                            continue;
                    }

                    projectSource.FileMisc.BuildUUID = new PbxUUID();
                    xcodeProj.WriteLine($@"
                        {projectSource.FileMisc.BuildUUID} /*{projectSource.FilePath}*/ = {{
                            isa = PBXBuildFile;
                            fileRef = {projectSource.FileMisc.RefUUID};
                        }};");
                }

                xcodeProj.WriteLine("/* End PBXBuildFile section */\n");

                // ------------------------------------------------------------------------------------------
                // Group section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXGroup section */");

                // Generating groups for sources..
                var xcodeGroupSource = new PbxGroup();
                foreach (var projectSource in project.Files[platform, TargetConfiguration.Any])
                {
                    var projectSourceDirectory = Path.GetDirectoryName(projectSource.FilePath);
                    if (projectSourceDirectory != null && projectSourceDirectory.StartsWith(project.SourcesFilterOrigin, StringComparison.Ordinal)
                                                       && projectSourceDirectory.Length > project.SourcesFilterOrigin.Length + 1)
                    {
                        // Source would be added to some child group.
                        var projectSourceGroupPath = projectSourceDirectory.Substring(project.SourcesFilterOrigin.Length + 1);
                        var projectSourceGroupPathSplitted = projectSourceGroupPath.Trim().Split(Path.DirectorySeparatorChar);
                        xcodeGroupSource.Add(projectSourceGroupPathSplitted, projectSource.FileMisc.RefUUID);
                    }
                    else
                    {
                        // Source would be added to the root group.
                        xcodeGroupSource.Add(projectSource.FileMisc.RefUUID);
                    }
                }

                // Writing child groups..
                xcodeGroupSource.Traverse((xcodeProjGroupName, xcodeProjGroup) =>
                {
                    var xcodeGroupChildrenUUIDList = new StringBuilder();
                    foreach (var xcodeProjGroupChildPair in xcodeProjGroup.ChildGroups)
                    {
                        xcodeGroupChildrenUUIDList.Append($"{xcodeProjGroupChildPair.Value.UUID}, ");
                    }
                    foreach (var xcodeProjGroupChildPair in xcodeProjGroup.ChildFileRefs)
                    {
                        xcodeGroupChildrenUUIDList.Append($"{xcodeProjGroupChildPair}, ");
                    }

                    // ReSharper disable once AccessToDisposedClosure
                    xcodeProj.WriteLine($@"
                        {xcodeProjGroup.UUID} = {{
                            isa = PBXGroup;
                            children = (
                                {xcodeGroupChildrenUUIDList}
                            );
                            path = ""{xcodeProjGroupName ?? "Source"}"";
                            sourceTree = ""<group>"";
                        }};");
                });

                // Writing products group..
                var xcodeGroupProductsUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeGroupProductsUUID} /*Products*/ = {{
                        isa = PBXGroup;
                        children = (
                            {xcodeProductRefUUID} /*{project.Name}*/,
                        );
                        name = Products;
                        sourceTree = ""<group>"";
                    }};");

                // Writing frameworks group..
                var xcodeGroupFrameworksChildrenUUIDList = new StringBuilder();
                foreach (var projectDependency in project.Dependencies[platform, TargetConfiguration.Any])
                {
                    foreach (var projectDependencyFramework in projectDependency.LinkedLibraries[platform, TargetConfiguration.Any])
                    {
                        xcodeGroupFrameworksChildrenUUIDList.Append($"{projectDependencyFramework.FileMisc.RefUUID}, ");
                    }
                }

                var xcodeGroupFrameworksUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeGroupFrameworksUUID} /*Products*/ = {{
                        isa = PBXGroup;
                        children = (
                            {xcodeGroupFrameworksChildrenUUIDList}
                        );
                        name = Frameworks;
                        sourceTree = ""<group>"";
                    }};");

                // Writing root group..
                var xcodeGroupRootUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeGroupRootUUID} = {{
                        isa = PBXGroup;
                        children = (
                            {xcodeGroupSource.UUID} /*Source*/,
                            {xcodeGroupProductsUUID} /*Products*/,
                            {xcodeGroupFrameworksUUID} /*Frameworks*/,
                        );
                        sourceTree = ""<group>"";
                        usesTabs = 0;
                    }};");

                xcodeProj.WriteLine("/* End PBXGroup section */\n");

                // ------------------------------------------------------------------------------------------
                // Source build phase section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXSourcesBuildPhase section */");

                var xcodeBuildPhaseSourcesUUIDList = new StringBuilder();
                foreach (var projectSource in project.Files[platform, TargetConfiguration.Any])
                {
                    // Writing down only files that need to be compiled..
                    switch (projectSource.FileType)
                    {
                        case ProjectSourceFileType.SourceCode:
                        case ProjectSourceFileType.SourceCodeObjective:
                        case ProjectSourceFileType.SourceCodeAssembler:
                            break;
                        default:
                            continue;
                    }
                    xcodeBuildPhaseSourcesUUIDList.Append($"{projectSource.FileMisc.BuildUUID},");
                }

                var xcodeBuildPhaseSourcesUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeBuildPhaseSourcesUUID} /*Sources*/ = {{
                        isa = PBXSourcesBuildPhase;
                        buildActionMask = 2147483647;
                        files = (
                            {xcodeBuildPhaseSourcesUUIDList}
                        );
                        runOnlyForDeploymentPostprocessing = 0;
                    }};");

                xcodeProj.WriteLine("/* End PBXSourcesBuildPhase section */\n");

                // ------------------------------------------------------------------------------------------
                // Framework build phase section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXFrameworksBuildPhase section */");

                var xcodeBuildPhaseFrameworksUUIDList = new StringBuilder();
                foreach (var projectDependency in project.Dependencies[platform, TargetConfiguration.Any])
                {
                    foreach (var projectDependencyFramework in projectDependency.LinkedLibraries[platform, TargetConfiguration.Debug])
                    {
                        xcodeBuildPhaseFrameworksUUIDList.Append($"{projectDependencyFramework.FileMisc.BuildUUID},");
                    }
                }

                var xcodeBuildPhaseFrameworksUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeBuildPhaseFrameworksUUID} /*Frameworks*/ = {{
                        isa = PBXFrameworksBuildPhase;
                        buildActionMask = 2147483647;
                        files = (
                            {xcodeBuildPhaseFrameworksUUIDList}
                        );
                        runOnlyForDeploymentPostprocessing = 0;
                    }};");

                xcodeProj.WriteLine("/* End PBXFrameworksBuildPhase section */\n");

                // ------------------------------------------------------------------------------------------
                // Copy files build phase section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXCopyFilesBuildPhase section */");

                // TODO: Copy files from dependencies.
                var xcodeBuildPhaseCopyFilesUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeBuildPhaseCopyFilesUUID} /*Copy Files*/ = {{
                        isa = PBXCopyFilesBuildPhase;
                        buildActionMask = 12;
                        dstPath = ""{Path.GetDirectoryName(project.OutputPath[platform, TargetConfiguration.Debug])}"";
                        dstSubfolderSpec = 0;
                        files = (
                            {xcodeProductBuildUUID}, /*Product*/
                        );
                        runOnlyForDeploymentPostprocessing = 0;
                    }};");

                xcodeProj.WriteLine("/* End PBXCopyFilesBuildPhase section */\n");

                // ------------------------------------------------------------------------------------------
                // Target configuration section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin XCBuildConfiguration section */");

                var xcodeConfigurationNativeTargetUUIDList = new StringBuilder();
                var xcodeConfigurationProjectUUIDList = new StringBuilder();

                foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
                {
                    var xcodeConfigurationNativeTargetUUID = new PbxUUID();
                    xcodeConfigurationNativeTargetUUIDList.Append($"{xcodeConfigurationNativeTargetUUID}, ");

                    // TODO: Configure this shit correctly!
                    xcodeProj.WriteLine($@"
                        {xcodeConfigurationNativeTargetUUID} /*{configuration}*/ = {{
                            isa = XCBuildConfiguration;
                            name = {configuration};
                            buildSettings = {{
                                CODE_SIGN_STYLE = Automatic;
                                HEADER_SEARCH_PATHS = (
                                    {project.GenerateIncludePaths(platform, configuration, ", ")}
                                );
                                LIBRARY_SEARCH_PATHS = (
                                    ""$(inherited)"",
                                    {project.GenerateLinkedLibrariesPaths(platform, configuration, ", ", Path.GetDirectoryName)}
                                );
                                PRODUCT_NAME = ""$(TARGET_NAME)"";
                            }};
                        }};");
                
                    var xcodeConfigurationProjectUUID = new PbxUUID();
                    xcodeConfigurationProjectUUIDList.Append($"{xcodeConfigurationProjectUUID}, ");

                    // TODO: Configure this shit correctly!
                    xcodeProj.WriteLine($@"
                        {xcodeConfigurationProjectUUID} /*{configuration}*/ = {{
                            isa = XCBuildConfiguration;
                            name = {configuration};
                            buildSettings = {{
                                ALWAYS_SEARCH_USER_PATHS = NO;
                                CLANG_ANALYZER_NONNULL = YES;
                                CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
                                CLANG_CXX_LANGUAGE_STANDARD = ""gnu++14"";
                                CLANG_CXX_LIBRARY = ""libc++"";
                                CLANG_ENABLE_MODULES = YES;
                                CLANG_ENABLE_OBJC_ARC = YES;
                                CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
                                CLANG_WARN_BOOL_CONVERSION = YES;
                                CLANG_WARN_COMMA = YES;
                                CLANG_WARN_CONSTANT_CONVERSION = YES;
                                CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
                                CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
                                CLANG_WARN_EMPTY_BODY = YES;
                                CLANG_WARN_ENUM_CONVERSION = YES;
                                CLANG_WARN_INFINITE_RECURSION = YES;
                                CLANG_WARN_INT_CONVERSION = YES;
                                CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
                                CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
                                CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
                                CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
                                CLANG_WARN_STRICT_PROTOTYPES = YES;
                                CLANG_WARN_SUSPICIOUS_MOVE = YES;
                                CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
                                CLANG_WARN_UNREACHABLE_CODE = YES;
                                CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
                                CODE_SIGN_IDENTITY = ""-"";
                                COPY_PHASE_STRIP = NO;
                                DEBUG_INFORMATION_FORMAT = dwarf;
                                ENABLE_STRICT_OBJC_MSGSEND = YES;
                                ENABLE_TESTABILITY = YES;
                                GCC_C_LANGUAGE_STANDARD = gnu11;
                                GCC_DYNAMIC_NO_PIC = NO;
                                GCC_NO_COMMON_BLOCKS = YES;
                                GCC_OPTIMIZATION_LEVEL = 0;
                                GCC_PREPROCESSOR_DEFINITIONS = (
                                    ""DEBUG=1"",
                                    ""$(inherited)"",
                                );
                                GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
                                GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
                                GCC_WARN_UNDECLARED_SELECTOR = YES;
                                GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
                                GCC_WARN_UNUSED_FUNCTION = YES;
                                GCC_WARN_UNUSED_VARIABLE = YES;
                                MACOSX_DEPLOYMENT_TARGET = 10.13;
                                MTL_ENABLE_DEBUG_INFO = YES;
                                ONLY_ACTIVE_ARCH = YES;
                                SDKROOT = macosx;
                            }};
                        }};");
                }
                xcodeProj.WriteLine("/* End XCBuildConfiguration section */\n");

                // ------------------------------------------------------------------------------------------
                // Configuration List section.
                // ------------------------------------------------------------------------------------------
                
                xcodeProj.WriteLine("/* Begin XCConfigurationList section */");

                var xcodeConfigurationListNativeTarget = new PbxUUID();
                var xcodeConfigurationListProjectUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeConfigurationListNativeTarget} /*PBXNativeTarget*/ = {{
                        isa = XCConfigurationList;
                        buildConfigurations = (
                            {xcodeConfigurationNativeTargetUUIDList}
                        );
                        defaultConfigurationIsVisible = 0;
                        defaultConfigurationName = {TargetConfiguration.Release};
                    }};");
                xcodeProj.WriteLine($@"
                    {xcodeConfigurationListProjectUUID} /*PBXProject*/ = {{
                        isa = XCConfigurationList;
                        buildConfigurations = (
                            {xcodeConfigurationProjectUUIDList}
                        );
                        defaultConfigurationIsVisible = 0;
                        defaultConfigurationName = {TargetConfiguration.Release};
                    }};");

                xcodeProj.WriteLine("/* End XCConfigurationList section */\n");

                // ------------------------------------------------------------------------------------------
                // Native Target section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXNativeTarget section */");

                string xcodeNativeTargetProductType;
                switch (project.BuildType[TargetPlatform.MacOS, TargetConfiguration.Release])
                {
                    case ProjectBuildType.Application:
                        // TODO: Actually, here should be .application.
                        // But this requires to embed a plist.
                        xcodeNativeTargetProductType = "com.apple.product-type.tool";
                        break;
                    case ProjectBuildType.StaticLibrary:
                        xcodeNativeTargetProductType = "com.apple.product-type.library.static";
                        break;
                    case ProjectBuildType.DynamicLibrary:
                        xcodeNativeTargetProductType = "com.apple.product-type.library.dynamic";
                        break;
                    default:
                        throw new NotSupportedException();
                }

                var xcodeNativeTargetUUID = project.Misc.NativeTargetUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeNativeTargetUUID} /*{project.Name}*/ = {{
                        isa = PBXNativeTarget;
                        buildConfigurationList = {xcodeConfigurationListNativeTarget};
                        buildPhases = (
                            {xcodeBuildPhaseSourcesUUID}, /*Sources*/
                            {xcodeBuildPhaseFrameworksUUID}, /* Frameworks */
                            {xcodeBuildPhaseCopyFilesUUID}, /* Copy Files */
                        );
                        buildRules = (
                        );
                        dependencies = (
                        );
                        name = {project.Name};
                        productName = ""{project.Name}"";
                        productReference = {xcodeProductRefUUID};
                        productType = ""{xcodeNativeTargetProductType}"";
                    }};");

                xcodeProj.WriteLine("/* End PBXNativeTarget section */\n");

                // ------------------------------------------------------------------------------------------
                // Project section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXProject section */");

                var xcodeProjectUUID = project.Misc.ProjectUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeProjectUUID} /*{project.Name}*/ = {{ 
                        isa = PBXProject; 
                        attributes = {{
                            LastUpgradeCheck = 0920;
                            ORGANIZATIONNAME = {Environment.UserName};
                            TargetAttributes = {{
                                7CC50132201C539100AA2808 = {{
                                    CreatedOnToolsVersion = 9.2;
                                    ProvisioningStyle = Automatic;
                                }};
                            }};
                        }};
                        buildConfigurationList = {xcodeConfigurationListProjectUUID};
                        compatibilityVersion = ""Xcode 8.0"";
                        developmentRegion = en;
                        hasScannedForEncodings = 0;
                        knownRegions = (
                            en,
                        );
                        mainGroup = {xcodeGroupRootUUID};
                        productRefGroup = {xcodeGroupProductsUUID};
                        projectDirPath = """";
                        projectRoot = """";
                        targets = (
                            {xcodeNativeTargetUUID} /*PBXNativeTarget*/
                        );
                    }};");

                xcodeProj.WriteLine("/* End PBXProject section */\n");

                // ==========================================================================================
                // End .XCODEPROJ.PBXPROJ
                // ==========================================================================================

                xcodeProj.WriteLine($@"
                    }};
                    rootObject = {xcodeProjectUUID};
                }}");

            }

            return xcodeProjPath;
        }

        /// <summary>
        /// Generates workspace files for XCode.
        /// </summary>
        /// <param name="solution">Parsed solution object.</param>
        /// <returns>Path to XCode's '.xcworkspace' file.</returns>
        /// <inheritdoc />
        public sealed override string GenerateSolutionFiles(Solution solution)
        {
            var xcodeWorkspacePath = Path.Combine(base.GenerateSolutionFiles(solution), solution.Name) + ".xcworkspace";
            Directory.CreateDirectory(xcodeWorkspacePath);

            // ------------------------------------------------------------------------------------------
            // Contents section.
            // ------------------------------------------------------------------------------------------

            var xcodeWorkspaceContentsPath = Path.Combine(xcodeWorkspacePath, "contents.xcworkspacedata");
            using (var xcodeWorkspaceContents = new StreamWriter(xcodeWorkspaceContentsPath))
            {
                xcodeWorkspaceContents.WriteLine($@"<?xml version=""1.0"" encoding=""UTF-8""?>
                    <Workspace>");
                foreach (var solutionProject in solution.Projects)
                {
                    if (solutionProject.IsBuildTool)
                        continue;
                    xcodeWorkspaceContents.WriteLine($@"
                        <FileRef 
                            location=""group:{solutionProject.GeneratorOutputPath}"">
                        </FileRef>");
                }
                xcodeWorkspaceContents.WriteLine(@"
                    </Workspace>");
            }

            // ------------------------------------------------------------------------------------------
            // Schemes section.
            // ------------------------------------------------------------------------------------------

            var xcodeWorkspaceSchemeDirectory = Path.Combine(xcodeWorkspacePath, "xcshareddata", "xcschemes");
            var xcodeWorkspaceSchemePath = Path.Combine(xcodeWorkspaceSchemeDirectory, "All.xcscheme");
            Directory.CreateDirectory(xcodeWorkspaceSchemeDirectory);

            using (var xcodeWorkspaceScheme = new StreamWriter(xcodeWorkspaceSchemePath))
            {
                xcodeWorkspaceScheme.WriteLine(@"<?xml version=""1.0"" encoding=""UTF-8""?>");
                xcodeWorkspaceScheme.WriteLine(@"
                    <Scheme 
                        LastUpgradeVersion = ""0920"" 
                        version = ""1.3"">");

                xcodeWorkspaceScheme.WriteLine(@"
                    <BuildAction 
                        parallelizeBuildables=""NO"" 
                        buildImplicitDependencies = ""YES"">
                        <BuildActionEntries>");
                foreach (var solutionProject in solution.Projects)
                {
                    if (solutionProject.IsBuildTool)
                        continue;
                    xcodeWorkspaceScheme.WriteLine($@"
                        <BuildActionEntry
                            buildForTesting = ""YES""
                            buildForRunning = ""YES""
                            buildForProfiling = ""YES""
                            buildForArchiving = ""YES""
                            buildForAnalyzing = ""YES"">
                            <BuildableReference
                               BuildableIdentifier = ""primary""
                               BlueprintIdentifier = ""{solutionProject.Misc.NativeTargetUUID}""
                               BuildableName = ""{solutionProject.Name}""
                               BlueprintName = ""{solutionProject.Name}""
                               ReferencedContainer = ""container:{solutionProject.GeneratorOutputPath}"">
                            </BuildableReference>
                         </BuildActionEntry>");
                }
                xcodeWorkspaceScheme.WriteLine(@"
                        </BuildActionEntries>
                    </BuildAction>");

                xcodeWorkspaceScheme.WriteLine(@"
                        <AnalyzeAction
                            buildConfiguration = ""Debug"">
                        </AnalyzeAction>
                        <ArchiveAction
                            buildConfiguration = ""Release""
                            revealArchiveInOrganizer = ""YES"">
                        </ArchiveAction>
                    </Scheme>");
            }

            // ------------------------------------------------------------------------------------------
            // Schemes management section.
            // ------------------------------------------------------------------------------------------

            var xcodeWorkspaceSchemeManagementPath = Path.Combine(xcodeWorkspaceSchemeDirectory, "xcschememanagement.plist");
            using (var xcodeWorkspaceSchemeManagement = new StreamWriter(xcodeWorkspaceSchemeManagementPath))
            {
                xcodeWorkspaceSchemeManagement.WriteLine(@"<?xml version=""1.0"" encoding=""UTF-8""?>");
                xcodeWorkspaceSchemeManagement.WriteLine(@"<!DOCTYPE plist PUBLIC ""-//Apple//DTD PLIST 1.0//EN"" ""http://www.apple.com/DTDs/PropertyList-1.0.dtd"">");
                xcodeWorkspaceSchemeManagement.WriteLine(@"
                    <plist version=""1.0"">
                        <dict>
                            <key>SchemeUserState</key>
                            <dict>
                                <key>All.xcscheme</key>
                                <dict>
                                    <key>orderHint</key>
                                    <integer>0</integer>
                                </dict>
                            </dict>
                        </dict>
                    </plist>");
            }

            return xcodeWorkspacePath;
        }

    }   // public class XCodeProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
