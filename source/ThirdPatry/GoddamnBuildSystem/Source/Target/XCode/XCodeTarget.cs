﻿/*************************************************************************
 * XCodeTarget.cs - basic XCode project file generator                   *
 * Copyright (C) $(IE_DEV) 2011 - Present. All Rights Reserved.          *
 *************************************************************************
 * History:                                                              *
 *		- 05.04.2014 - Created by James Jhuighuy                         *
 *************************************************************************/

#if FALSE

using System;
using System.IO;
using System.Text;
using System.Reflection;
using System.Diagnostics;
using System.Collections.Generic;

namespace IncredibleEngine.BuildSystem.XCode
{
    #region [-- XCode serialization Core --]

    //! <summary>
    //! This attribute marks all fields of XCodeObject that are going to be serialized
    //! </summary>
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class XCodePropertyAttribute : Attribute
    {
        //! <summary> Name of this property in XCode project file </summary>
        public readonly string PropertyName;
        public XCodePropertyAttribute(string PropertyName) { this.PropertyName = PropertyName; }
    }   // public sealed class XCodeProperty

    //! <summary>
    //! Class represents convertible to string reference for XCode project file
    //! </summary>
    public sealed class XCodeReference
    {
        //! <summary>Name of this reference</summary>
        public readonly string ReferenceName;
        public XCodeReference(string ReferenceName) { this.ReferenceName = ReferenceName; }

        //! <summary>Translates this reference into 96-bit XCode GUID string</summary>
        //! <returns>String representation of this reference</returns>
        public sealed override string ToString()
        {
            string PropertyNameHash = this.ReferenceName.GetHashCode().ToString("X");
            return new StringBuilder().Append(new string('0', 24 - PropertyNameHash.Length))   // Filling with leading zeros
                                      .Append(PropertyNameHash).ToString();
        }
    }   // public sealed class XCodeReference

    //! <summary>
    //! Class contains instruments for properties serialization into XCode format
    //! </summary>
    public class XCodeObject
    {
        //! <summary>Reference on this object</summary>
        public readonly XCodeReference Reference;
        public XCodeObject(string ObjectUniqueIdenetifier) { this.Reference = new XCodeReference(ObjectUniqueIdenetifier); }

        //! <summary>Serializes all object properties into XCode recognizable format</summary>
        //! <returns>String representation of this object in XCode recognizable format</returns>
        public sealed override string ToString()
        {
            StringBuilder Builder = new StringBuilder();
            Builder.Append(this.Reference.ToString()).Append(" = {");
            foreach (FieldInfo Property in this.GetType().GetFields())
            {   // Searching throw all fields inside object
                XCodeProperty[] PropertyAttributes = Property.GetCustomAttributes(typeof(XCodeProperty), true) as XCodeProperty[];
                Debug.Assert(PropertyAttributes.Length <= 1, "Invalid XCodeProperty attribute was found");
                if (PropertyAttributes.Length == 1)
                {
                    string PropertyName = PropertyAttributes[0].PropertyName;
                    string PropertyValue = Property.GetValue(this).ToString();
                    Builder.AppendFormat("{0} = {1}; ", PropertyName, PropertyValue);
                }
            }

            return Builder.Append("};").ToString();
        }
    }   // public class XCodeObject

    //! <summary>
    //! Class represents serializable into XCode format list of object references
    //! </summary>
    public sealed class XCodeObjectRefList : List<XCodeReference>
    {
        public XCodeObjectRefList() { }

        //! <summary>Serializes all object properties into XCode recognizable format</summary>
        //! <returns>String representation of this object in XCode recognizable format</returns>
        public sealed override string ToString()
        {
            StringBuilder Builder = new StringBuilder().Append('(');
            foreach (var Reference in this) Builder.AppendLine("\t").Append(Reference.ToString());
            return Builder.AppendLine(")").ToString();
        }
    }   // public sealed class XCodeObjectRefList

    //! <summary>
    //! Class represents serializable into XCode format list of objects
    //! </summary>
    public sealed class XCodeObjectList : List<XCodeObject>
    {
        public XCodeObjectList() { }

        //! <summary>Serializes all object properties into XCode recognizable format</summary>
        //! <returns>String representation of this object in XCode recognizable format</returns>
        public sealed override string ToString()
        {
            StringBuilder Builder = new StringBuilder().Append('{');
            foreach (var Obj in this) Builder.AppendLine("\t").Append(Obj.ToString());
            return Builder.AppendLine("}").ToString();
        }
    }   // public sealed class XCodeObjectList

    #endregion

    #region [-- XCode project file nodes --]

    //! <summary>
    //! Class provides information about file with source code that would be added to build
    //! </summary>
    public sealed class XCodeFileReference : XCodeObject
    {
        [XCodeProperty("isa")]
        public readonly string IsA = "PBXFileReference";

        [XCodeProperty("fileEncoding")]
        public readonly string FileEncoding = "4";

        [XCodeProperty("lastKnownFileType")]
        public readonly string LastKnownFileType = null;

        [XCodeProperty("path")]
        public readonly string FileReferencePath = null;

        [XCodeProperty("useTabs")]
        public readonly string UseTabs = "1";

        public XCodeFileReference(string FileReferencePath)
            : base(FileReferencePath)
        {
            switch (Path.GetExtension(this.FileReferencePath = FileReferencePath))
            {   // Selecting code type on the base of extension
                case ".h": this.LastKnownFileType = "sourcecode.c.h"; break;
                case ".c": this.LastKnownFileType = "sourcecode.c.c"; break;
                case ".m": this.LastKnownFileType = "sourcecode.c.objc"; break;
                case ".hh": this.LastKnownFileType = "sourcecode.cpp.hpp"; break;
                case ".cc": this.LastKnownFileType = "sourcecode.cpp.cpp"; break;
                case ".mm": this.LastKnownFileType = "sourcecode.cpp.objcpp"; break;
                case ".hpp": this.LastKnownFileType = "sourcecode.cpp.hpp"; break;
                case ".cpp": this.LastKnownFileType = "sourcecode.cpp.cpp"; break;
                case ".cxx": this.LastKnownFileType = "sourcecode.cpp.cpp"; break;
                case ".hxx": this.LastKnownFileType = "sourcecode.cpp.hpp"; break;
                case ".inl": this.LastKnownFileType = "sourcecode.cpp.hpp"; break;
                default: throw new ArgumentException("Invalid extension detected in specified path");
            }
        }
    }   // internal sealed class XCodeFileReference

    public sealed class XCodeBuildFile : XCodeObject
    {
        public readonly string Isa = "PBXBuildFile";
        public readonly XCodeReference FileRef = null;

        public XCodeBuildFile(XCodeFileReference FileRef)
            : base(FileRef.FileReferencePath + "_BuildFile")
        {
            this.FileRef = FileRef.Reference;
        }
    }   // internal sealed class XCodeBuildFile

    internal class XCodeProjectFile : XCodeObject
    {
        public /*XCodeProperty*/ readonly int ArchiveVersion = 1;
        public /*XCodeProperty*/ readonly XCodeList Classes = new XCodeList();
        public /*XCodeProperty*/ readonly int ObjectVersion = 46;
        public /*XCodeProperty*/ readonly XCodeMap Objects = new XCodeMap();
        public /*XCodeProperty*/ readonly XCodeReference RootObject = new XCodeReference("Root");

        public XCodeProjectFile()
            : base(null)
        {
        }
    }

    #endregion

    public class XCodeGeneratable
    {
    }
}   // namespace IncredibleEngine.BuildSystem.XCode

namespace IncredibleEngine.BuildSystem
{
    public class OsXTarget : XCode.XCodeGeneratable, ITarget
    {
        public string TargetPlatformName { get { return "OS X"; } }
        public string TargetPlatformCodeName { get { return "OsX"; } }
        public void GenerateProjectFiles(SolutionProject CurrentProject)
        {

            Console.WriteLine(new XCodeFileReference(CurrentProject.GetSourceFiles()[0]).ToString());
        }
    }
}   // namespace IncredibleEngine.BuildSystem
#endif