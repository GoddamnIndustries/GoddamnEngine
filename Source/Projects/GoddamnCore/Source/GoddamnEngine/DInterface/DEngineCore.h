// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/DInterface/EngineCore.h
 * File contains export interface for the creation and basic functionality of the 
 * base engine layer.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/DInterface/DInterface.h>

GD_NAMESPACE_BEGIN

	class DVideoInterface;
	
	class DEngineCoreIterface : public DRuntimeInterface
	{
	public:

		static void RaiseError(...) {}

		/*!
		 * Creates an instance of the Video subsystem interface.
		 */
		//GD_DFUNCTION_0P_R(GDAPI, DVideoInterface*, CreateDVideoInterface);


	};	// class DEngineCoreIterface

	/*!
	 * Creates an instance of the class that maintains the whole C++ engine core functionality
	 * in D programming language-written runtime.
	 * \returns Newly created instance of the main interface or nullptr on internal error.
	 */
	GDAPI extern DEngineCoreIterface* CreateDEngineCoreIterface();

GD_NAMESPACE_END
