//////////////////////////////////////////////////////////////////////////
/// MathCompilerSetupStart.hh - disables useless warnings in vector and matrix headers.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014  - Rewritten from scratch by James Jhuighuy,
//////////////////////////////////////////////////////////////////////////

// This warnings needs to be disabled to make correct vector swizzling.
#if (defined(GD_COMPILER_MSC))
#	pragma warning(push)
#	pragma warning(disable : 4587)	// behavior change: constructor is no longer implicitly called
#	pragma warning(disable : 4588)	// behavior change: destructor is no longer implicitly called
#	pragma warning(disable : 4595)	// behavior change: destructor will no longer be implicitly called if an exception is thrown
#	pragma warning(disable : 4624)	// destructor was implicitly defined as deleted because a base class destructor is inaccessible or deleted
#endif	// if (defined(GD_COMPILER_MSC))
