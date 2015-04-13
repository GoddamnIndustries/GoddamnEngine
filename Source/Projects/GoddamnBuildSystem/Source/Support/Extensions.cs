// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file Extensions.cs
//! Extensions to .NET standart library.

using System;
using System.Xml;

namespace GoddamnEngine.BuildSystem
{
    //! @brief Represents extensions to .NET standart library's "Array" class.
    internal static class ArrayExtensions
    {
        //! @brief Similar to String's "Substring" function.
        internal static T[] SubArray<T>(this T[] Data, int Index, int Length = -1)
        {
            Length = (Length > 0) ? Length : (Data.Length - Index);
            T[] Result = new T[Length];
            Array.Copy(Data, Index, Result, 0, Length);

            return Result;
        }

        //! @brief Similar to String's "Concat" function.
        public static T[] Concat<T>(this T[] First, T[] Second)
        {
            int OldLength = First.Length;
            Array.Resize<T>(ref First, First.Length + Second.Length);
            Array.Copy(Second, 0, First, OldLength, Second.Length);

            return First;
        }
    }   // class ArrayExtensions

    //! @brief Represents extensions to .NET standart library's "XmlTextWriter" class.
    internal static class XmlTextWriterExtensions
    {
        //! @brief Adds formatting support for "WriteAttributeString" function.
        public static void WriteAttributeStringFormat(this XmlTextWriter Writer, string LocalName, string ValueFormat, params object[] ValueArguments)
        {
            Writer.WriteAttributeString(LocalName, string.Format(ValueFormat, ValueArguments));
        }

        //! @brief Adds formatting support for "WriteElementString" function.
        public static void WriteElementStringFormat(this XmlTextWriter Writer, string LocalName, string ValueFormat, params object[] ValueArguments)
        {
            Writer.WriteElementString(LocalName, string.Format(ValueFormat, ValueArguments));
        }
    }   // class XmlTextWriterExtensions
}   // namespace GoddamnEngine.BuildSystem
