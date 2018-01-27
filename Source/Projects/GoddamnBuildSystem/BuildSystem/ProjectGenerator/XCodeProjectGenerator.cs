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
using System.Diagnostics;
using GoddamnEngine.BuildSystem.Collectors;
using GoddamnEngine.BuildSystem.Target;

namespace GoddamnEngine.BuildSystem.ProjectGenerator
{
    /// <summary>
    /// Generator of the project/solution files for XCode.
    /// </summary>
    /// <inheritdoc />
    public class XCodeProjectGenerator : ProjectGenerator
    {
        private static Random s_Random = new Random();
        private static string CreateXCodeUuid()
        {
            return String.Format("{0:X}{1:X}{2:X}", s_Random.Next(), s_Random.Next(), s_Random.Next());
        }

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

        private StreamWriter m_XCodeProj;
        private ProjectCache m_Project;

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

            m_Project = project;

            // ==========================================================================================
            // Generating XCODEPROJ/PBXPROJ files.
            // ==========================================================================================
            using (m_XCodeProj = new StreamWriter(xcodePbxProjPath))
            {
                m_XCodeProj.WriteLine("// !$*UTF8*$! ");
                m_XCodeProj.WriteLine("{");
                m_XCodeProj.WriteLine("\tarchiveVersion = 1;");
                m_XCodeProj.WriteLine("\tclasses = {");
                m_XCodeProj.WriteLine("\t};");
                m_XCodeProj.WriteLine("\tobjectVersion = 48;");
                m_XCodeProj.WriteLine("\tobjects = {\n");

                GenerateProjectFiles_Section_Files_Referenced();
                GenerateProjectFiles_Section_Files_Build();
                GenerateProjectFiles_Section_Files_Group();

                GenerateProjectFiles_Section_BuildPhase_Sources();
                GenerateProjectFiles_Section_BuildPhase_Frameworks();
                GenerateProjectFiles_Section_BuildPhase_CopyFiles();

                GenerateProjectFiles_Section_Configurations_NativeTarget();
                GenerateProjectFiles_Section_Configurations_Project();

                GenerateProjectFiles_Section_Project_NativeTarget();
                GenerateProjectFiles_Section_Project();

                m_XCodeProj.WriteLine("\t}; ");
                m_XCodeProj.WriteLine("\trootObject = {0};", m_XCodeProject);
                m_XCodeProj.WriteLine("}");
            }

            return xcodeProjPath;
        }

        // ==========================================================================================
        // Files sections.
        // ==========================================================================================

        // ------------------------------------------------------------------------------------------
        // Referenced files section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeProductReference = "";
        private void GenerateProjectFiles_Section_Files_Referenced()
        {
            m_XCodeProj.WriteLine("/* Begin PBXBuildFile section */");

            // Writing down product references..
            m_XCodeProductReference = CreateXCodeUuid();
            m_XCodeProj.WriteLine("\t\t{0} = /*{1}*/ {{ isa = PBXFileReference; ", m_XCodeProductReference, m_Project.CachedName);
            m_XCodeProj.WriteLine("\t\t\texplicitFileType = \"compiled.mach-o.executable\";");
            m_XCodeProj.WriteLine("\t\t\tincludeInIndex = 0;");
            m_XCodeProj.WriteLine("\t\t\tpath = {0};", m_Project.CachedName);
            m_XCodeProj.WriteLine("\t\t\tsourceTree = BUILT_PRODUCTS_DIR;");
            m_XCodeProj.WriteLine("\t\t};");

            // Writing down referenced files..
            foreach (var projectSource in m_Project.CachedSourceFiles)
            {
                projectSource.FileMisc.RefUuid = CreateXCodeUuid();
                m_XCodeProj.WriteLine("\t\t{0} = /*{1}*/ {{ isa = PBXFileReference; ", projectSource.FileMisc.RefUuid, projectSource.FileName);
                switch (projectSource.FileType)
                {
                    case ProjectSourceFileType.SourceCode:
                        m_XCodeProj.WriteLine("\t\t\tlastKnownFileType = sourcecode.cpp.cpp;");
                        break;
                    case ProjectSourceFileType.SourceCodeObjective:
                        m_XCodeProj.WriteLine("\t\t\tlastKnownFileType = sourcecode.cpp.objcpp;");
                        break;
                    case ProjectSourceFileType.SourceCodeAssembler:
                        m_XCodeProj.WriteLine("\t\t\tlastKnownFileType = sourcecode.asm;");
                        break;
                    case ProjectSourceFileType.HeaderFile:
                    case ProjectSourceFileType.InlineImplementation:
                        m_XCodeProj.WriteLine("\t\t\tlastKnownFileType = sourcecode.cpp.h;");
                        break;
                    case ProjectSourceFileType.ResourceScript:
                        m_XCodeProj.WriteLine("\t\t\tlastKnownFileType = file.txt;");
                        break;
                    default:
                        throw new NotSupportedException();
                }
                m_XCodeProj.WriteLine("\t\t\tpath = {0};", projectSource.FileName);
                //m_XCodeProj.WriteLine("\t\t\tsourceTree = \"<group>\";");
                m_XCodeProj.WriteLine("\t\t};");
            }
            m_XCodeProj.WriteLine("/* End PBXBuildFile section */\n");
        }

        // ------------------------------------------------------------------------------------------
        // Build Files section.
        // ------------------------------------------------------------------------------------------

        private void GenerateProjectFiles_Section_Files_Build()
        {
            m_XCodeProj.WriteLine("/* Begin PBXBuildFile section */");
            foreach (var projectSource in m_Project.CachedSourceFiles)
            {
                // Writing down only files that need to be compiled..
                switch (projectSource.FileType)
                {
                    case ProjectSourceFileType.SourceCode:
                    case ProjectSourceFileType.SourceCodeObjective:
                    case ProjectSourceFileType.SourceCodeAssembler:
                        projectSource.FileMisc.BuildUuid = CreateXCodeUuid();
                        m_XCodeProj.WriteLine("\t\t{0} = /*{1}*/ {{isa = PBXBuildFile; ", projectSource.FileMisc.BuildUuid, projectSource.FileName);
                        m_XCodeProj.WriteLine("\t\t\tfileRef = {0}; ", projectSource.FileMisc.RefUuid);
                        m_XCodeProj.WriteLine("\t\t};");
                        break;
                }
            }
            m_XCodeProj.WriteLine("/* End PBXBuildFile section */\n");
        }

        // ------------------------------------------------------------------------------------------
        // Group section.
        // ------------------------------------------------------------------------------------------

        private sealed class PBXGroup
        {
            public readonly string Name;
            public readonly string Uuid = CreateXCodeUuid();
            public readonly Dictionary<string, PBXGroup> InnerGroups = new Dictionary<string, PBXGroup>();
            public readonly Dictionary<string, string> InnerFileRefs = new Dictionary<string, string>();
            public PBXGroup(string name = null) { Name = name; }
        }   // class PBXGroup

        private string m_XCodeGroupRoot;
        private string m_XCodeGroupProducts;
        private void GenerateProjectFiles_Section_Files_Group()
        {
            var xcodeProjSourceRootGroup = new PBXGroup("Source");
            m_XCodeProj.WriteLine("/* Begin PBXGroup section */");

            // Generating groups for sources.
            foreach (var projectSource in m_Project.CachedSourceFiles)
            {
                GenerateProjectFiles_Section_Files_Group_InsertIntoGroup(xcodeProjSourceRootGroup, projectSource, m_Project.CachedSourcesFilterOrigin);
            }
            GenerateProjectFiles_Section_Files_Group_Recursive(xcodeProjSourceRootGroup);

            // Writing products group..
            m_XCodeGroupProducts = CreateXCodeUuid();
            m_XCodeProj.WriteLine("\t\t{0} = {{ isa = PBXGroup; ", m_XCodeGroupProducts);
            m_XCodeProj.WriteLine("\t\t\tchildren = (");
            m_XCodeProj.WriteLine("\t\t\t\t{0} /*{1}*/,", m_XCodeProductReference, m_Project.CachedName);
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tname = Products;");
            m_XCodeProj.WriteLine("\t\t\tsourceTree = \"<group>\";");
            m_XCodeProj.WriteLine("\t\t};");

            // Writing root group..
            m_XCodeGroupRoot = CreateXCodeUuid();
            m_XCodeProj.WriteLine("\t\t{0} = {{ isa = PBXGroup; ", m_XCodeGroupRoot);
            m_XCodeProj.WriteLine("\t\t\tchildren = (");
            m_XCodeProj.WriteLine("\t\t\t\t{0} /*Source*/,", xcodeProjSourceRootGroup.Uuid);
            m_XCodeProj.WriteLine("\t\t\t\t{0} /*Products*/,", m_XCodeGroupProducts);
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tsourceTree = \"<group>\";");
            m_XCodeProj.WriteLine("\t\t\tusesTabs = 0;");
            m_XCodeProj.WriteLine("\t\t};");

            m_XCodeProj.WriteLine("/* End PBXGroup section */\n");
        }

        private void GenerateProjectFiles_Section_Files_Group_Recursive(PBXGroup projectGroup)
        {
            m_XCodeProj.WriteLine("\t\t{0} = {{ isa = PBXGroup; ", projectGroup.Uuid);
            m_XCodeProj.WriteLine("\t\t\tchildren = (");
            foreach (var projectInnerGroup in projectGroup.InnerGroups)
            {
                m_XCodeProj.WriteLine("\t\t\t\t{0} /*{1}*/,", projectInnerGroup.Value.Uuid, projectInnerGroup.Value.Name);
            }
            foreach (var projectSource in projectGroup.InnerFileRefs)
            {
                m_XCodeProj.WriteLine("\t\t\t\t{0} /*{1}*/,", projectSource.Value, projectSource.Key);
            }
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tpath = \"{0}\";", projectGroup.Name);
            m_XCodeProj.WriteLine("\t\t\tsourceTree = \"<group>\";");
            m_XCodeProj.WriteLine("\t\t};");

            foreach (var projectInnerGroup in projectGroup.InnerGroups)
            {
                // Inner groups..
                GenerateProjectFiles_Section_Files_Group_Recursive(projectInnerGroup.Value);
            }
        }

        private static void GenerateProjectFiles_Section_Files_Group_InsertIntoGroup(PBXGroup projectGroup, ProjectSourceFile projectSource, string projectGroupOrigin)
        {
            // Checking if this source can have a relative group: it should start with a group origin.
            var projectSourceFileDirectory = Path.GetDirectoryName(projectSource.FileName);
            if (projectSourceFileDirectory != null && projectSourceFileDirectory.StartsWith(projectGroupOrigin, StringComparison.InvariantCultureIgnoreCase)
                                                   && projectSourceFileDirectory.Length > projectGroupOrigin.Length + 1)
            {
                // Source would be added to some inner group.
                var projectGroupName = projectSourceFileDirectory.Substring(projectGroupOrigin.Length + 1);
                var projectGroupNameSeparatorIndex = projectGroupName.IndexOf('/');
                var projectGroupRootName = projectGroupNameSeparatorIndex != -1 ? projectGroupName.Substring(0, projectGroupNameSeparatorIndex) : projectGroupName;

                PBXGroup projectInnerGroup;
                if (!projectGroup.InnerGroups.ContainsKey(projectGroupRootName))
                {
                    projectInnerGroup = new PBXGroup(projectGroupRootName);
                    projectGroup.InnerGroups.Add(projectGroupRootName, projectInnerGroup);
                }
                else
                {
                    projectInnerGroup = projectGroup.InnerGroups[projectGroupRootName];
                }

                GenerateProjectFiles_Section_Files_Group_InsertIntoGroup(projectInnerGroup, projectSource, Path.Combine(projectGroupOrigin, projectGroupRootName));
            }
            else
            {
                // Source would be added to this group.
                projectGroup.InnerFileRefs.Add(projectSource.FileName, projectSource.FileMisc.RefUuid);
            }
        }

        // ==========================================================================================
        // Build phases sections.
        // ==========================================================================================

        // ------------------------------------------------------------------------------------------
        // Source build phase section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeBuildPhaseSources;
        private void GenerateProjectFiles_Section_BuildPhase_Sources()
        {
            m_XCodeBuildPhaseSources = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin PBXSourcesBuildPhase section */");
            m_XCodeProj.WriteLine("\t\t{0} /*CopyFiles*/ = {{ isa = PBXSourcesBuildPhase; ", m_XCodeBuildPhaseSources);
            m_XCodeProj.WriteLine("\t\t\tbuildActionMask = 2147483647;");
            m_XCodeProj.WriteLine("\t\t\tfiles = (");
            foreach (var projectSource in m_Project.CachedSourceFiles)
            {
                // Writing down only files that need to be compiled..
                switch (projectSource.FileType)
                {
                    case ProjectSourceFileType.SourceCode:
                    case ProjectSourceFileType.SourceCodeObjective:
                    case ProjectSourceFileType.SourceCodeAssembler:
                        m_XCodeProj.WriteLine("\t\t\t\t{0} /*{1}*/,", projectSource.FileMisc.BuildUuid, projectSource.FileName);
                        break;
                }
            }
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\trunOnlyForDeploymentPostprocessing = 0;");
            m_XCodeProj.WriteLine("\t\t};");
            m_XCodeProj.WriteLine("/* End PBXSourcesBuildPhase section */\n");
        }

        // ------------------------------------------------------------------------------------------
        // Framework build phase section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeBuildPhaseFramework;
        private void GenerateProjectFiles_Section_BuildPhase_Frameworks()
        {
            m_XCodeBuildPhaseFramework = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin PBXFrameworksBuildPhase section */");
            m_XCodeProj.WriteLine("\t\t{0} /*CopyFiles*/ = {{ isa = PBXFrameworksBuildPhase; ", m_XCodeBuildPhaseFramework);
            m_XCodeProj.WriteLine("\t\t\tbuildActionMask = 2147483647;");
            m_XCodeProj.WriteLine("\t\t\tfiles = (");
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t};");
            m_XCodeProj.WriteLine("/* End PBXFrameworksBuildPhase section */\n");
        }

        // ------------------------------------------------------------------------------------------
        // Copy files build phase section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeBuildPhaseCopyFiles;
        private void GenerateProjectFiles_Section_BuildPhase_CopyFiles()
        {
            m_XCodeBuildPhaseCopyFiles = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin PBXCopyFilesBuildPhase section */");
            m_XCodeProj.WriteLine("\t\t{0} /*CopyFiles*/ = {{ isa = PBXCopyFilesBuildPhase; ", m_XCodeBuildPhaseCopyFiles);
            m_XCodeProj.WriteLine("\t\t\tbuildActionMask = 2147483647;");
            m_XCodeProj.WriteLine("\t\t\tdstPath = /usr/share/man/man1/;");
            m_XCodeProj.WriteLine("\t\t\tdstSubfolderSpec = 0;");
            m_XCodeProj.WriteLine("\t\t\tfiles = (");
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\trunOnlyForDeploymentPostprocessing = 1;");
            m_XCodeProj.WriteLine("\t\t};");
            m_XCodeProj.WriteLine("/* End PBXCopyFilesBuildPhase section */\n");
        }

        // ==========================================================================================
        // Configuration sections.
        // ==========================================================================================

        // ------------------------------------------------------------------------------------------
        // Native Target configuration section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeConfigurationListNativeTarget;
        private void GenerateProjectFiles_Section_Configurations_NativeTarget()
        {
            var xcodeNativeTargetBuildConfigurations = new List<string>();

            m_XCodeProj.WriteLine("/* Begin XCBuildConfiguration section */");
            foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
            {
                var xcodeNativeTargetBuildConfiguration = CreateXCodeUuid();
                xcodeNativeTargetBuildConfigurations.Add(xcodeNativeTargetBuildConfiguration);

                m_XCodeProj.WriteLine("\t\t{0} /*{1}*/ = {{ isa = XCBuildConfiguration; ", xcodeNativeTargetBuildConfiguration, configuration.ToString());
                m_XCodeProj.WriteLine("\t\t\tname = {0};", configuration.ToString());
                m_XCodeProj.WriteLine("\t\t\tbuildSettings = {");
                m_XCodeProj.WriteLine("\t\t\t\tCODE_SIGN_STYLE = Automatic;");
                m_XCodeProj.WriteLine("\t\t\t\tPRODUCT_NAME = \"$(TARGET_NAME)\";");
                m_XCodeProj.WriteLine("\t\t\t};");
                m_XCodeProj.WriteLine("\t\t};");
            }
            m_XCodeProj.WriteLine("/* End XCBuildConfiguration section */\n");

            m_XCodeConfigurationListNativeTarget = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin XCConfigurationList section */");
            m_XCodeProj.WriteLine("\t\t{0} /*PBXNativeTarget*/ = {{ isa = XCConfigurationList; ", m_XCodeConfigurationListNativeTarget);
            m_XCodeProj.WriteLine("\t\t\tbuildConfigurations = (");
            foreach (var xcodeNativeTargetBuildConfiguration in xcodeNativeTargetBuildConfigurations)
            {
                m_XCodeProj.WriteLine("\t\t\t\t{0},", xcodeNativeTargetBuildConfiguration);
            }
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tdefaultConfigurationIsVisible = 0;");
            m_XCodeProj.WriteLine("\t\t\tdefaultConfigurationName = {0};", TargetConfiguration.Release.ToString());
            m_XCodeProj.WriteLine("\t\t};");
        }

        // ------------------------------------------------------------------------------------------
        // Project configuration section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeConfigurationListProject;
        private void GenerateProjectFiles_Section_Configurations_Project()
        {
            var xcodeProjectBuildConfigurations = new List<string>();

            m_XCodeProj.WriteLine("/* Begin XCBuildConfiguration section */");
            foreach (var configuration in TargetInfo.EnumerateAllConfigurations())
            {
                var xcodeProjectBuildConfiguration = CreateXCodeUuid();
                xcodeProjectBuildConfigurations.Add(xcodeProjectBuildConfiguration);

                m_XCodeProj.WriteLine("\t\t{0} /*{1}*/ = {{ isa = XCBuildConfiguration; ", xcodeProjectBuildConfiguration, configuration.ToString());
                m_XCodeProj.WriteLine("\t\t\tname = {0};", configuration.ToString());
                m_XCodeProj.WriteLine("\t\t\tbuildSettings = {");  // TODO: Configure this shit correctly!
                m_XCodeProj.WriteLine(@"ALWAYS_SEARCH_USER_PATHS = NO;
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
                SDKROOT = macosx;");
                m_XCodeProj.WriteLine("\t\t\t};");
                m_XCodeProj.WriteLine("\t\t};");
            }
            m_XCodeProj.WriteLine("/* End XCBuildConfiguration section */\n");

            m_XCodeConfigurationListProject = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin XCConfigurationList section */");
            m_XCodeProj.WriteLine("\t\t{0} /*PBXProject*/ = {{ isa = XCConfigurationList; ", m_XCodeConfigurationListProject);
            m_XCodeProj.WriteLine("\t\t\tbuildConfigurations = (");
            foreach (var xcodeProjectBuildConfiguration in xcodeProjectBuildConfigurations)
            {
                m_XCodeProj.WriteLine("\t\t\t\t{0},", xcodeProjectBuildConfiguration);
            }
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tdefaultConfigurationIsVisible = 0;");
            m_XCodeProj.WriteLine("\t\t\tdefaultConfigurationName = {0};", TargetConfiguration.Release.ToString());
            m_XCodeProj.WriteLine("\t\t};");
            m_XCodeProj.WriteLine("/* End XCConfigurationList section */\n");
        }

        // ==========================================================================================
        // Projects sections.
        // ==========================================================================================

        // ------------------------------------------------------------------------------------------
        // Native Target section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeNativeTarget;
        private void GenerateProjectFiles_Section_Project_NativeTarget()
        {
            m_XCodeNativeTarget = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin PBXNativeTarget section */");
            m_XCodeProj.WriteLine("\t\t{0} /*{1}*/ = {{ isa = PBXNativeTarget; ", m_XCodeNativeTarget, m_Project.CachedName);
            m_XCodeProj.WriteLine("\t\t\tbuildConfigurationList = {0};", m_XCodeConfigurationListNativeTarget);
            m_XCodeProj.WriteLine("\t\t\tbuildPhases = (");
            m_XCodeProj.WriteLine("\t\t\t\t{0} /* Sources */,", m_XCodeBuildPhaseSources);
            m_XCodeProj.WriteLine("\t\t\t\t{0} /* Frameworks */,", m_XCodeBuildPhaseFramework);
            m_XCodeProj.WriteLine("\t\t\t\t{0} /* Copy Files */,", m_XCodeBuildPhaseCopyFiles);
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tbuildRules = (");
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tdependencies = (");
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t\tname = {0};", m_Project.CachedName);
            m_XCodeProj.WriteLine("\t\t\tproductName = \"{0}\";", m_Project.CachedName);
            m_XCodeProj.WriteLine("\t\t\tproductReference = {0};", m_XCodeProductReference);
            m_XCodeProj.WriteLine("\t\t\tproductType = \"com.apple.product-type.tool\";");
            m_XCodeProj.WriteLine("\t\t};");
            m_XCodeProj.WriteLine("/* End PBXNativeTarget section */\n");
        }

        // ------------------------------------------------------------------------------------------
        // Project section.
        // ------------------------------------------------------------------------------------------

        private string m_XCodeProject;
        private void GenerateProjectFiles_Section_Project()
        {
            m_XCodeProject = CreateXCodeUuid();
            m_XCodeProj.WriteLine("/* Begin PBXProject section */");
            m_XCodeProj.WriteLine("\t\t{0} /*{1}*/ = {{ isa = PBXProject; ", m_XCodeProject, m_Project.CachedName);
            m_XCodeProj.WriteLine(@"
                attributes = {
                    LastUpgradeCheck = 0920;
                    ORGANIZATIONNAME = Goddamn;
                    TargetAttributes = {
                        7CC50132201C539100AA2808 = {
                            CreatedOnToolsVersion = 9.2;
                            ProvisioningStyle = Automatic;
                        };
                    };
                };
            ");
            m_XCodeProj.WriteLine("\t\t\tbuildConfigurationList = {0};", m_XCodeConfigurationListProject);
            m_XCodeProj.WriteLine(@"
                compatibilityVersion = ""Xcode 8.0"";
                developmentRegion = en;
                hasScannedForEncodings = 0;
                knownRegions = (
                    en,

                );
            ");
            m_XCodeProj.WriteLine("\t\t\tmainGroup = {0};", m_XCodeGroupRoot);
            m_XCodeProj.WriteLine("\t\t\tproductRefGroup = {0};", m_XCodeGroupProducts);
            m_XCodeProj.WriteLine("\t\t\tprojectDirPath = \"\";");
            m_XCodeProj.WriteLine("\t\t\tprojectRoot = \"\";");
            m_XCodeProj.WriteLine("\t\t\ttargets = (");
            m_XCodeProj.WriteLine("\t\t\t\t{0} /* Test */,", m_XCodeNativeTarget);
            m_XCodeProj.WriteLine("\t\t\t);");
            m_XCodeProj.WriteLine("\t\t};");
            m_XCodeProj.WriteLine("/* End PBXProject section */\n");
        }

    }   // public class XCodeProjectGenerator
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator
