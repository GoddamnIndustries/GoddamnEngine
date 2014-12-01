//! ==========================================================================================
//! Extensions.cs - extensions to .NET standart library.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 26.11.2014 - Created by James Jhuighuy
//! ==========================================================================================

using System;
using System.Reflection;
using System.Collections.Generic;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Represents extensions to .NET standart library.
    /// </summary>
    internal static class Extensions
    {
        /// <summary>
        /// Similar to String's "Substring" function.
        /// </summary>
        public static T[] SubArray<T>(this T[] Data, int Index, int Length = -1)
        {
            Length = (Length > 0) ? Length : (Data.Length - Index);
            T[] Result = new T[Length];
            Array.Copy(Data, Index, Result, 0, Length);

            return Result;
        }

        /// <summary>
        /// Enumerates list of types in current assembly which are not abstract and are dirived from specified.
        /// </summary>
        public static IEnumerable<Type> GetInstancableTypesDerivedFrom<T>(this Assembly Asm)
        {
            foreach (Type InternalType in Asm.GetTypes()) {
                if ((!InternalType.IsAbstract) && (InternalType.BaseType == typeof(T))) {
                    yield return InternalType;
                }
            }
        }
    }   // class Extensions
}   // namespace GoddamnEngine.BuildSystem
