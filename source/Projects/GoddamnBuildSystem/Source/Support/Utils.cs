//! ==========================================================================================
//! Utils.cs - some very common API.
//! Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
//! 
//! History:
//!		* 02.12.2014 - Created by James Jhuighuy.
//! ==========================================================================================

using System;
using System.IO;

namespace GoddamnEngine.BuildSystem
{
    //! Some very common API.
    internal static class Utils
    {
        //! ------------------------------------------------------------------------------------------
        //! @name File system API.
        //! @{
        //! ------------------------------------------------------------------------------------------

        //! Checks if specified file is empty.
        internal static bool IsFileEmpty(string Location)
        {
            return (new FileInfo(Location).Length == 0);
        }

        //! Creates a new empty file at specified path.
        internal static void CreateEmptyFile(string Location)
        {
            File.Create(Location).Dispose();
        }

        //! @}
    }   // class Support
}   // namespace GoddamnEngine.BuildSystem
