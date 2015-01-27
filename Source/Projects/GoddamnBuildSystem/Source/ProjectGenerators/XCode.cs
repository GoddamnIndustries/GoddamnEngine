//! ==========================================================================================
//! XCode.cs - XCode IDE project files generator.
//! Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
//! ==========================================================================================

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Text;

namespace GoddamnEngine.BuildSystem.ProjectGenerator.XCode
{
#if FALSE

    //! This attribute marks all fields of XCodeObject that are going to be serialized.
    [AttributeUsage(AttributeTargets.Field)]
    internal sealed class XCodePropertyAttribute : Attribute
    {
        public readonly string m_PropertyName;
        public XCodePropertyAttribute(string PropertyName)
        {
            this.m_PropertyName = PropertyName;
        }
    }   // class XCodePropertyAttribute

    //! Class represents convertible to string reference for XCode project file
    internal sealed class XCodeReference
    {
        //! @brief Name of this reference.
        public readonly string ReferenceName;
        public XCodeReference(string ReferenceName) { this.ReferenceName = ReferenceName; }
        public sealed override string ToString()
        {
            return new string('0', 24 - PropertyNameHash.Length) + this.ReferenceName.GetHashCode().ToString("X");
        }
    }   // public sealed class XCodeReference

    //! @brief 
    //! Class contains instruments for properties serialization into XCode format
    //! .
    public class XCodeObject
    {
        //! @brief Reference on this object.
        public readonly XCodeReference Reference;
        public XCodeObject(string ObjectUniqueIdenetifier) { this.Reference = new XCodeReference(ObjectUniqueIdenetifier); }

        //! @brief Serializes all object properties into XCode recognizable format.
        //! @returns String representation of this object in XCode recognizable format.
        public sealed override string ToString()
        {
            StringBuilder Builder = new StringBuilder();
            Builder.Append(this.Reference.ToString()).Append(" = {");
            foreach (FieldInfo Property in this.GetType().GetFields()) {   // Searching throw all fields inside object
                XCodeProperty[] PropertyAttributes = Property.GetCustomAttributes(typeof(XCodeProperty), true) as XCodeProperty[];
                Debug.Assert(PropertyAttributes.Length <= 1, "Invalid XCodeProperty attribute was found");
                if (PropertyAttributes.Length == 1) {
                    string PropertyName = PropertyAttributes[0].PropertyName;
                    string PropertyValue = Property.GetValue(this).ToString();
                    Builder.AppendFormat("{0} = {1}; ", PropertyName, PropertyValue);
                }
            }

            return Builder.Append("};").ToString();
        }
    }   // public class XCodeObject

    //! @brief 
    //! Class represents serializable into XCode format list of object references
    //! .
    public sealed class XCodeObjectRefList : List<XCodeReference>
    {
        public XCodeObjectRefList() { }

        //! @brief Serializes all object properties into XCode recognizable format.
        //! @returns String representation of this object in XCode recognizable format.
        public sealed override string ToString()
        {
            StringBuilder Builder = new StringBuilder().Append('(');
            foreach (var Reference in this)
                Builder.AppendLine("\t").Append(Reference.ToString());
            return Builder.AppendLine(")").ToString();
        }
    }   // public sealed class XCodeObjectRefList

    //! @brief 
    //! Class represents serializable into XCode format list of objects
    //! .
    public sealed class XCodeObjectList : List<XCodeObject>
    {
        public XCodeObjectList() { }

        //! @brief Serializes all object properties into XCode recognizable format.
        //! @returns String representation of this object in XCode recognizable format.
        public sealed override string ToString()
        {
            StringBuilder Builder = new StringBuilder().Append('{');
            foreach (var Obj in this)
                Builder.AppendLine("\t").Append(Obj.ToString());
            return Builder.AppendLine("}").ToString();
        }
    }   // public sealed class XCodeObjectList

    //! @brief 
    //! Class provides information about file with source code that would be added to build
    //! .
    public sealed class XCodeFileReference : XCodeObject
    {
        [XCodeProperty("isa")]
        public readonly string m_IsA = "PBXFileReference";

        [XCodeProperty("fileEncoding")]
        public readonly string m_FileEncoding = "4";

        [XCodeProperty("lastKnownFileType")]
        public readonly string m_LastKnownFileType = null;

        [XCodeProperty("path")]
        public readonly string m_FileReferencePath = null;

        [XCodeProperty("useTabs")]
        public readonly string m_UseTabs = "1";

        public XCodeFileReference(string FileReferencePath)
            : base(FileReferencePath)
        {
            switch (Path.GetExtension(this.FileReferencePath = FileReferencePath)) {   // Selecting code type on the base of extension
                case ".h":
                    this.LastKnownFileType = "sourcecode.c.h";
                    break;
                case ".c":
                    this.LastKnownFileType = "sourcecode.c.c";
                    break;
                case ".m":
                    this.LastKnownFileType = "sourcecode.c.objc";
                    break;
                case ".hh":
                    this.LastKnownFileType = "sourcecode.cpp.hpp";
                    break;
                case ".cc":
                    this.LastKnownFileType = "sourcecode.cpp.cpp";
                    break;
                case ".mm":
                    this.LastKnownFileType = "sourcecode.cpp.objcpp";
                    break;
                case ".hpp":
                    this.LastKnownFileType = "sourcecode.cpp.hpp";
                    break;
                case ".cpp":
                    this.LastKnownFileType = "sourcecode.cpp.cpp";
                    break;
                case ".cxx":
                    this.LastKnownFileType = "sourcecode.cpp.cpp";
                    break;
                case ".hxx":
                    this.LastKnownFileType = "sourcecode.cpp.hpp";
                    break;
                case ".inl":
                    this.LastKnownFileType = "sourcecode.cpp.hpp";
                    break;
                default:
                    throw new ArgumentException("Invalid extension detected in specified path");
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

    public class XCodeGeneratable
    {
    }

#endif  // FALSE
}   // namespace GoddamnEngine.BuildSystem.ProjectGenerator.XCode
