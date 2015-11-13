// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Engine/Interface/Input/Input.cpp
 * File contains export interface for the input subsystem.
 */
#include <GoddamnEngine/Engine/Interface/Input/Input.h>
#include <GoddamnEngine/Engine/Pipeline/Pipeline.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// IInput interface.
	// ==========================================================================================

	GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IInput);
	GDAPI extern IInput* CreateIInputInstance();

	// ------------------------------------------------------------------------------------------
	//! Global input interface pointer.
	GDAPI IUniqueInterfacePointer<IInput> Input(CreateIInputInstance(), PipelineManager->GetMainThreadPipeline());
	GD_SINGLETON_IMPLEMENTATION(IInput);

	// ==========================================================================================
	// IInputManager interface.
	// ==========================================================================================

	//! @todo Uncomment this.
	/*GD_IMPLEMENT_CLASS_INFORMATION*/GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(IInputManager);
	GDAPI extern IInputManager* CreateIInputManagerInstance();

#if 0
	// ------------------------------------------------------------------------------------------
	//! Global input manager interface pointer.
	GDAPI IUniqueInterfacePointer<IInputManager> inputManager(CreateIInputManagerInstance(), nullptr);
	GD_SINGLETON_IMPLEMENTATION(IInputManager);
#endif	// if 0

GD_NAMESPACE_END
