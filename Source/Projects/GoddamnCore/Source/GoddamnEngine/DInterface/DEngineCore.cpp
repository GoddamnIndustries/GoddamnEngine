// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#include <GoddamnEngine/DInterface/DEngineCore.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Creates an instance of the class that maintains the whole C++ engine core functionality
	 * in D programming language-written runtime.
	 * \returns Newly created instance of the main interface or nullptr on internal error.
	 */
	GDAPI extern DEngineCoreIterface* CreateDEngineCoreIterface()
	{
		return GD_NEW(DEngineCoreIterface);
	}

GD_NAMESPACE_END
