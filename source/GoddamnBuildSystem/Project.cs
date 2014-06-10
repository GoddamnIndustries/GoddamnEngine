using System;
using System.Collections.Generic;
using System.Collections.Specialized;

namespace GoddamnEngine.BuildSystem
{
    public enum SourceFileType : byte
    {
        SourceCode,
        HeaderFile,
        InlineImpl,
        SupportFile = InlineImpl,
        ResourceScript
    }   // enum SourceFileType

    public enum ProjectBuildType : byte
    {
        Application,
        StaticLibrary,
        DynamicLibrary
    }   // public enum ProjectBuildType

    public sealed class ProjectException : Exception
    {
        public ProjectException(string Message) : base(Message) { }
    }

    public sealed class SourceFile
    {
        public readonly SourceFileType FileType;
        public readonly string AbsoluteFilePath;
        public readonly string RelativeFileFolder;

        public SourceFile(SourceFileType FileType, string AbsoluteFilePath, string RelativeFilePath)
        {
            this.FileType = FileType;
            this.AbsoluteFilePath = AbsoluteFilePath;
            this.RelativeFileFolder = RelativeFilePath;
        }
    }   // public struct SourceFile

    /// <summary> Class represents some block of code that requires separate build. </summary>
    public abstract partial class Project
    {
        private int _Checksumm = 0;

        private string _Location = null;
        private StringCollection _Folders = new StringCollection();
        private List<SourceFile> _SourceFiles = new List<SourceFile>();
        private List<Dependency> _Dependencies = new List<Dependency>();
        private StringCollection _PreprocessorDefinitions = new StringCollection();

        public int Checksumm { get { return this._Checksumm; } }
        public string Location { get { return this._Location; } }
        public StringCollection Folders { get { return this._Folders; } }
        public List<SourceFile> SourceFiles { get { return this._SourceFiles; } }
        public List<Dependency> Dependencies { get { return this._Dependencies; } }
        public StringCollection PreprocessorDefinitions { get { return this._PreprocessorDefinitions; } }

        public abstract string Name { get; }
        public abstract bool IsPlugin { get; }
        public abstract ProjectBuildType BuildType { get; }

        protected virtual void InitializeSelf()
        {
        }
    }   // public class TheProject
}   // namespace GoddamnEngine.BuildSystem
