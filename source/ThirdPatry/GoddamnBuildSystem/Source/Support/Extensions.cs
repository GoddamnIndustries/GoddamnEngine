//! ==========================================================================================
//! Extensions.cs - extensions to .NET standard library.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 26.11.2014 - Created by James Jhuighuy
//! ==========================================================================================

using System;

namespace GoddamnEngine.BuildSystem
{
    //! Represents extensions to .NET standard library.
    internal static class Extensions
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
            int OldLen = First.Length;
            Array.Resize<T>(ref First, First.Length + Second.Length);
            Array.Copy(Second, 0, First, OldLen, Second.Length);
            return First;
        }
    }   // class Extensions
}   // namespace GoddamnEngine.BuildSystem
