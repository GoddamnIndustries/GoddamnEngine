using System;
using System.IO;
using System.Threading;

namespace GoddamnEngine.BuildSystem
{
    partial class Project
    {
        private static class Workflow
        {
            private static Project ProcessProjectConfigSource(string ProjectConfigLocation)
            {
                string ProjectConfigPath = Path.Combine(ProjectConfigLocation, ".gdproj.cs");
                foreach (var AsmTypeInfo in Compiler.CompileSourceFile(ProjectConfigPath).GetTypes())
                    if ((AsmTypeInfo.IsSubclassOf(typeof(Project))) && (!AsmTypeInfo.IsAbstract))
                    {
                        Project TheProject = Activator.CreateInstance(AsmTypeInfo) as Project;
                        TheProject._Location = ProjectConfigLocation;
                        TheProject._SourceFiles.Add(new SourceFile(SourceFileType.SupportFile, ProjectConfigPath, ""));
                        TheProject.InitializeSelf();
                        return TheProject;
                    }

                throw new ProjectException("\n\nSpecified project file does not contains any project definitions.");
            }

            private static void ProcessSourceFilesInDirectory(Project TheProject, string CurrentSourceFilesDirectory)
            {
                Uri ProjectRootDirectoryUri = new Uri(TheProject.Location);
                string ProjectDirectoryName = Path.GetFileName(TheProject.Location);
                string FolderRelativePath = ProjectRootDirectoryUri.MakeRelativeUri(new Uri(CurrentSourceFilesDirectory)).ToString();
                if (!string.IsNullOrEmpty(FolderRelativePath))
                {
                    FolderRelativePath = FolderRelativePath.Substring(ProjectDirectoryName.Length);
                    if (!string.IsNullOrEmpty(FolderRelativePath))
                    {
                        FolderRelativePath = FolderRelativePath.Remove(0, 1);
                        TheProject._Folders.Add(FolderRelativePath);
                    }
                }

                foreach (var SourceAbsoluteFilePath in Directory.EnumerateFiles(CurrentSourceFilesDirectory))
                {
                    string SourceFileName = Path.GetFileNameWithoutExtension(SourceAbsoluteFilePath);
                    string SourceFileExtension = Path.GetExtension(SourceAbsoluteFilePath);
                    if (string.IsNullOrEmpty(SourceFileExtension) || string.IsNullOrEmpty(SourceFileName))
                        continue;

                    SourceFileType Type;
                    switch (SourceFileExtension)
                    {
                        case ".c":
                        case ".cc":
                        case ".cpp":
                        case ".cxx":
                            Type = SourceFileType.SourceCode;
                            break;

                        case ".h":
                        case ".hh":
                        case ".hpp":
                        case ".hxx":
                            Type = SourceFileType.HeaderFile;
                            break;

                        case ".inl":
                            Type = SourceFileType.InlineImpl;
                            break;

                        case ".natvis":
                            if (Target.IsWinAPIPlatform())
                            {   // NATive VISualizer Visual Studio file.
                                Type = SourceFileType.SupportFile;
                                break;
                            }

                            continue;

                        case ".rc":
                            if (Target.IsWinAPIPlatform())
                            {   // We support .RC files only on windows platforms.
                                Type = SourceFileType.ResourceScript;
                                break;
                            }

                            continue;

                        case ".m":
                        case ".mm":
                        case ".swift":
                            if (Target.IsCocoaPlatform())
                            {   // We support Objective-C files only on Cocoa platforms.
                                Type = SourceFileType.SourceCode;
                                break;
                            }

                            continue;

                        default:
                            ConsoleOutput.WriteLine(SourceAbsoluteFilePath, "warning: skipping file '{0}' with unknown extension '{1}'", SourceAbsoluteFilePath, SourceFileExtension);
                            continue;
                    }

                    TheProject._SourceFiles.Add(new SourceFile(Type, SourceAbsoluteFilePath, FolderRelativePath));
                    TheProject._Checksumm += SourceAbsoluteFilePath.GetHashCode();
                }
            }

            private static void ProcessProjectSubdirectory(Project TheProject, string CurrentSourceFilesDirectory = null)
            {
                if (CurrentSourceFilesDirectory == null)
                    CurrentSourceFilesDirectory = TheProject.Location;
                ProcessSourceFilesInDirectory(TheProject, CurrentSourceFilesDirectory);

                foreach (var CurrentSubdirectory in Directory.EnumerateDirectories(CurrentSourceFilesDirectory))
                {
                    if (File.Exists(Path.Combine(CurrentSubdirectory, ".gdproj.cs")) && (!Target.IsMonolithicBuild()))
                    {
                        ProcessProjectDirectory(CurrentSourceFilesDirectory);
                        continue;
                    }

                    string ProjectSubdirectoryName = Path.GetFileName(CurrentSubdirectory);
                    if (ProjectSubdirectoryName[0] == '_')
                    {   // This is a specific service directory.
                        switch(ProjectSubdirectoryName)
                        {
                            case "_Build": continue;
                            case "_DLLResources": goto ProcessProjectSubdirectoryLabel;
                            case "_Dependencies": DependenciesResolver.ProcessDirectory(TheProject, CurrentSubdirectory); continue;
                            default:
                                throw new ProjectException(string.Format("Unparsed service directory '{0}' is located in '{1}'", ProjectSubdirectoryName, CurrentSubdirectory));
                        }
                    }

                ProcessProjectSubdirectoryLabel:
                    ProcessProjectSubdirectory(TheProject, CurrentSubdirectory);
                }
            }

            public static void ProcessProjectDirectory(string ProjectConfigLocation)
            {
                Thread ProjectProcessingThread = new Thread(() =>
                {
                    string ProjectConfigPath = Path.Combine(ProjectConfigLocation, ".gdproj.cs");
                    try
                    {
                        int ProjectParsingStartTime = Environment.TickCount;
                        ConsoleOutput.WriteLine(ProjectConfigPath, "log: parsing project file started...");

                        Project TheProject = ProcessProjectConfigSource(ProjectConfigLocation);
                        ProcessProjectSubdirectory(TheProject);
                        new VisualStudioTarget().GenerateProject(TheProject);

                        ConsoleOutput.WriteLine(ProjectConfigPath, "parsing project file finished. Time: {0}ms", (Environment.TickCount - ProjectParsingStartTime).ToString());
                    }
                    catch (ProjectException PE)
                    {   // Project building failed.
                        ConsoleOutput.WriteLine(ProjectConfigPath, "error: parsing project failed with exception: \n{1}", ProjectConfigLocation, PE.ToString());
                        throw new SolutionException(String.Format("Failed to process \"{0}\" project", ProjectConfigLocation));
                    }
                });

                ProjectProcessingThread.Name = String.Format("({0}) Project Processing Thread", Path.GetFileName(ProjectConfigLocation));
                ProjectProcessingThread.Start();
            }
        }   // private static class Workflow

        public static void ProcessProjectsInDirectory(string Location)
        {
            foreach (var ProjectFile in Directory.EnumerateFiles(BuildSystem.SDKPath, ".gdproj.cs", SearchOption.AllDirectories))
                Workflow.ProcessProjectDirectory(Path.GetDirectoryName(ProjectFile));
        }
    }   // partial class Project
}   // namespace GoddamnEngine.BuildSystem
