//! ==========================================================================================
//! Support.cs - some very common API.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 02.12.2014 - Created by James Jhuighuy.
//! ==========================================================================================

using System.IO;

namespace GoddamnEngine.BuildSystem
{
    /// <summary>
    /// Some very common API.
    /// </summary>
    internal static class Support
    {
        //! ------------------------------------------------------------------------------------------
        //! Filesystem API.
        //! ------------------------------------------------------------------------------------------

        /// <summary>
        /// Checks if specified file is empty.
        /// </summary>
        internal static bool IsFileEmpty(string Location)
        {
            return (new FileInfo(Location).Length == 0);
        }

        /// <summary>
        /// Creates a new empty file at specified path.
        /// </summary>
        internal static void CreateEmptyFile(string Location)
        {
            File.Create(Location).Dispose();
        }
    }   // class Support
}   // namespace GoddamnEngine.BuildSystem
