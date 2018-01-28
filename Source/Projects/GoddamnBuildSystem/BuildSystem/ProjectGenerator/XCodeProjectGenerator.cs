﻿// ==========================================================================================
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
            public readonly int A = s_Random.Next(), B = s_Random.Next();
            public readonly int C = s_Random.Next();
            public override string ToString() 
            {
                return $"{A:X}{B:X}{C:X}";
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
        /// <returns>Path to XCode's '.xcodeproj' file.</returns>
        public sealed override string GenerateProjectFiles(ProjectCache project)
        {
            var xcodeProjPath = Path.Combine(base.GenerateProjectFiles(project), project.CachedName) + ".xcodeproj";
            var xcodePbxProjPath = Path.Combine(xcodeProjPath, "project.pbxproj");
            Directory.CreateDirectory(xcodeProjPath);

            using (var xcodeProj = new StreamWriter(xcodePbxProjPath))
            {
                xcodeProj.WriteLine($@"// !$*UTF8*$!
                    {{
                        archiveVersion = 1;
                        classes = {{
                        }};
                        objectVersion = 48;
                        objects = {{");
                
                // ------------------------------------------------------------------------------------------
                // Referenced files section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXFileReference section */");

                // Writing down product references..
                var xcodeProductRefUUID = new PbxUUID();
                xcodeProj.WriteLine($@"
                    {xcodeProductRefUUID} /*{project.CachedName}*/ = {{
                        isa = PBXFileReference;
                        explicitFileType = ""compiled.mach-o.executable"";
                        includeInIndex = 0;
                        path = {project.CachedName};
                        sourceTree = BUILT_PRODUCTS_DIR;
                    }};");

                // Writing down referenced files..
                foreach (var projectSource in project.CachedSourceFiles)
                {
                    string xcodeFileRefLastKnownType = null;
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
                        {projectSource.FileMisc.RefUUID} /*{projectSource.FileName}*/ = {{
                            isa = PBXFileReference;
                            lastKnownFileType = {xcodeFileRefLastKnownType};
                            path = {projectSource.FileName};
                            name = {Path.GetFileName(projectSource.FileName)};
                            sourceTree = ""<group>"";
                        }};");
                }
                xcodeProj.WriteLine("/* End PBXFileReference section */\n");

                // ------------------------------------------------------------------------------------------
                // Build Files section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXBuildFile section */");
                foreach (var projectSource in project.CachedSourceFiles)
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
                        {projectSource.FileMisc.BuildUUID} /*{projectSource.FileName}*/ = {{
                            isa = PBXBuildFile;
                            fileRef = {projectSource.FileMisc.RefUUID};
                        }};");
                }
                xcodeProj.WriteLine("/* End PBXBuildFile section */\n");

                // ------------------------------------------------------------------------------------------
                // Group section.
                // ------------------------------------------------------------------------------------------

                xcodeProj.WriteLine("/* Begin PBXGroup section */");

                // Generating groups for sources.
                var xcodeGroupSource = new PbxGroup();
                foreach (var projectSource in project.CachedSourceFiles)
                {
                    var projectSourceDirectory = Path.GetDirectoryName(projectSource.FileName);
                    if (projectSourceDirectory != null && projectSourceDirectory.StartsWith(project.CachedSourcesFilterOrigin, StringComparison.Ordinal)
                                                       && projectSourceDirectory.Length > project.CachedSourcesFilterOrigin.Length + 1)
                    {
                        // Source would be added to some inner group.
                        var projectSourceGroupPath = projectSourceDirectory.Substring(project.CachedSourcesFilterOrigin.Length + 1);
                        var projectSourceGroupPathSplitted = projectSourceGroupPath.Trim().Split('/');
                        xcodeGroupSource.Add(projectSourceGroupPathSplitted, projectSource.FileMisc.RefUUID);
                    }
                    else
                    {
                        // Source would be added to root group.
                        xcodeGroupSource.Add(projectSource.FileMisc.RefUUID);
                    }
                }

                // Writing inner groups..
                xcodeGroupSource.Traverse((string xcodeProjGroupName, PbxGroup xcodeProjGroup) =>
                {
                    var xcodeProjGroupChildrenUUIDList = new StringBuilder();
                    foreach (var xcodeProjGroupChildPair in xcodeProjGroup.ChildGroups)
                    {
                        xcodeProjGroupChildrenUUIDList.Append($"{xcodeProjGroupChildPair.Value.UUID}, ");
                    }
                    foreach (var xcodeProjGroupChildPair in xcodeProjGroup.ChildFileRefs)
                    {
                        xcodeProjGroupChildrenUUIDList.Append($"{xcodeProjGroupChildPair}, ");
                    }

                    xcodeProj.WriteLine($@"
                        {xcodeProjGroup.UUID} = {{
                            isa = PBXGroup;
                            children = (
                                {xcodeProjGroupChildrenUUIDList}
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
                            {xcodeProductRefUUID} /*{project.CachedName}*/,
                        );
                        name = Products;
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
                        );
                        sourceTree = ""<group>"";
                        usesTabs = 0;
                    }};");
                xcodeProj.WriteLine("/* End PBXGroup section */\n");

                // ------------------------------------------------------------------------------------------
                // Source build phase section.
                // ------------------------------------------------------------------------------------------

                var xcodeBuildPhaseSourcesUUIDList = new StringBuilder();
                foreach (var projectSource in project.CachedSourceFiles)
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
                xcodeProj.WriteLine("/* Begin PBXSourcesBuildPhase section */");
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

                var xcodeBuildPhaseFrameworksUUID = new PbxUUID();
                xcodeProj.WriteLine("/* Begin PBXFrameworksBuildPhase section */");
                xcodeProj.WriteLine($@"
                    {xcodeBuildPhaseFrameworksUUID} /*Frameworks*/ = {{
                        isa = PBXFrameworksBuildPhase;
                        buildActionMask = 2147483647;
                        files = (
                        );
                        runOnlyForDeploymentPostprocessing = 0;
                    }};");
                xcodeProj.WriteLine("/* End PBXFrameworksBuildPhase section */\n");

                // ------------------------------------------------------------------------------------------
                // Copy files build phase section.
                // ------------------------------------------------------------------------------------------

                var xcodeBuildPhaseCopyFilesUUID = new PbxUUID();
                xcodeProj.WriteLine("/* Begin PBXCopyFilesBuildPhase section */");
                xcodeProj.WriteLine($@"
                    {xcodeBuildPhaseCopyFilesUUID} /*Copy Files*/ = {{
                        isa = PBXCopyFilesBuildPhase;
                        buildActionMask = 2147483647;
                        dstPath = /usr/share/man/man1/;
                        dstSubfolderSpec = 0;
                        files = (
                        );
                        runOnlyForDeploymentPostprocessing = 1;
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
                                    {project.GenerateIncludePaths(", ")}
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
                                DEBUG_INFORMATION_FORMAT = ""dwarf-with-dsym"";
                                ENABLE_NS_ASSERTIONS = NO;
                                ENABLE_STRICT_OBJC_MSGSEND = YES;
                                GCC_C_LANGUAGE_STANDARD = gnu11;
                                GCC_NO_COMMON_BLOCKS = YES;
                                GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
                                GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
                                GCC_WARN_UNDECLARED_SELECTOR = YES;
                                GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
                                GCC_WARN_UNUSED_FUNCTION = YES;
                                GCC_WARN_UNUSED_VARIABLE = YES;
                                MACOSX_DEPLOYMENT_TARGET = 10.13;
                                MTL_ENABLE_DEBUG_INFO = NO;
                                SDKROOT = macosx;
                            }};
                        }};");
                }
                xcodeProj.WriteLine("/* End XCBuildConfiguration section */\n");

                // ------------------------------------------------------------------------------------------
                // Configuration List section.
                // ------------------------------------------------------------------------------------------
                
                var xcodeConfigurationListNativeTarget = new PbxUUID();
                var xcodeConfigurationListProjectUUID = new PbxUUID();
                
                xcodeProj.WriteLine("/* Begin XCConfigurationList section */");
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

                var xcodeNativeTargetUUID = new PbxUUID();
                xcodeProj.WriteLine("/* Begin PBXNativeTarget section */");
                xcodeProj.WriteLine($@"
                    {xcodeNativeTargetUUID} /*{project.CachedName}*/ = {{
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
                        name = {project.CachedName};
                        productName = ""{project.CachedName}"";
                        productReference = {xcodeProductRefUUID};
                        productType = ""com.apple.product-type.tool"";
                    }};");
                xcodeProj.WriteLine("/* End PBXNativeTarget section */\n");
        
                // ------------------------------------------------------------------------------------------
                // Project section.
                // ------------------------------------------------------------------------------------------

                var xcodeProjectUUID = new PbxUUID();
                xcodeProj.WriteLine("/* Begin PBXProject section */");
                xcodeProj.WriteLine($@"
                    {xcodeProjectUUID} /*{project.CachedName}*/ = {{ 
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

                xcodeProj.WriteLine($@"
                    }};
                    rootObject = {xcodeProjectUUID};
                }}");

            }

            return xcodeProjPath;
        }

    }   // public class XCodeProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
