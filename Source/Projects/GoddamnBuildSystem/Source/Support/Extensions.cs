//! ==========================================================================================
//! Extensions.cs - extensions to .NET standard library.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! @author Created by James Jhuighuy
//! ==========================================================================================

using System;
using System.Xml;

namespace GoddamnEngine.BuildSystem
{
    //! Represents extensions to .NET standard library's "Array" class.
    internal static class ArrayExtensions
    {
        //! Similar to String's "Substring" function.
        internal static T[] SubArray<T>(this T[] Data, int Index, int Length = -1)
        {
            Length = (Length > 0) ? Length : (Data.Length - Index);
            T[] Result = new T[Length];
            Array.Copy(Data, Index, Result, 0, Length);

            return Result;
        }

        //! Similar to String's "Concat" function.
        public static T[] Concat<T>(this T[] First, T[] Second)
        {
            int OldLength = First.Length;
            Array.Resize<T>(ref First, First.Length + Second.Length);
            Array.Copy(Second, 0, First, OldLength, Second.Length);

            return First;
        }
    }   // class ArrayExtensions

    //! Represents extensions to .NET standard library's "XmlTextWriter" class.
    internal static class XmlTextWriterExtensions
    {
        //! Adds formatting support for "WriteAttributeString" function.
        public static void WriteAttributeStringFormat(this XmlTextWriter Writer, string LocalName, string ValueFormat, params object[] ValueArguments)
        {
            Writer.WriteAttributeString(LocalName, string.Format(ValueFormat, ValueArguments));
        }

        //! Adds formatting support for "WriteElementString" function.
        public static void WriteElementStringFormat(this XmlTextWriter Writer, string LocalName, string ValueFormat, params object[] ValueArguments)
        {
            Writer.WriteElementString(LocalName, string.Format(ValueFormat, ValueArguments));
        }
    }   // class XmlTextWriterExtensions
}   // namespace GoddamnEngine.BuildSystem
