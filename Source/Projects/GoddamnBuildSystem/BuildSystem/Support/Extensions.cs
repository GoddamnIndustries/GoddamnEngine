// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

using System;
using System.Xml;

namespace GoddamnEngine.BuildSystem.Support
{
    /// <summary>
    /// Represents extensions to .NET standard library's "Array" class.
    /// </summary>
    internal static class ArrayExtensions
    {
        /// <summary>
        /// Similar to string's "Substring" function.
        /// </summary>
        internal static T[] SubArray<T>(this T[] data, int index, int length = -1)
        {
            length = length > 0 ? length : data.Length - index;
            var result = new T[length];
            Array.Copy(data, index, result, 0, length);

            return result;
        }

        /// <summary>
        /// Similar to string's "Concat" function.
        /// </summary>
        public static T[] Concat<T>(this T[] first, T[] second)
        {
            var oldLength = first.Length;
            Array.Resize(ref first, first.Length + second.Length);
            Array.Copy(second, 0, first, oldLength, second.Length);

            return first;
        }
    }   // class ArrayExtensions

    /// <summary>
    /// Represents extensions to .NET standard library's "XmlTextWriter" class.
    /// </summary>
    internal static class XmlTextWriterExtensions
    {
        /// <summary>
        /// Adds formatting support for "WriteAttributeString" function.
        /// </summary>
        public static void WriteAttributeStringFormat(this XmlTextWriter writer, string localName, string valueFormat, params object[] valueArguments)
        {
            writer.WriteAttributeString(localName, string.Format(valueFormat, valueArguments));
        }

        /// <summary>
        /// Adds formatting support for "WriteElementString" function.
        /// </summary>
        public static void WriteElementStringFormat(this XmlTextWriter writer, string localName, string valueFormat, params object[] valueArguments)
        {
            writer.WriteElementString(localName, string.Format(valueFormat, valueArguments));
        }
    }   // class XmlTextWriterExtensions

}   // namespace GoddamnEngine.BuildSystem
