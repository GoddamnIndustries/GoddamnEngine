// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Pipeline/Pipeline.cpp
 * @todo Rewrite this awful code one day.
 * @todo Implement this code using the correct multi-threading API.
 * File contains base class for all engine runtime pipelines.
 */
#include <GoddamnEngine/Engine/Pipeline/Pipeline.h>

GD_NAMESPACE_BEGIN
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IUniqueInterfacePointer struct.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes a global pointer to the unique interface.
	//! @param UniqueInterfacePipeline PPipeline, to which the this unique interface would be attached.
	//! @param UniqueInterfaceCreatorProc Pointer to function, that creates instance of the implementation for this interface.
	//! @{
	GDAPI IUniqueInterfacePointerBase::IUniqueInterfacePointerBase(IInterface* const UniqueInterface
		, PPipeline* const UniqueInterfacePipeline)
	{
		GD_DEBUG_ASSERT(UniqueInterface != nullptr, "Invalid interface instance specified.");
		GD_DEBUG_ASSERT(UniqueInterfacePipeline != nullptr, "Invalid interface pipeline specified.");

		UniqueInterfacePipeline->_PipelineStaticInterfaces.InsertLast(UniqueInterfacePipeline);
	}
	GDAPI IUniqueInterfacePointerBase::IUniqueInterfacePointerBase(IRuntimeUniqueInterface* const UniqueRuntimeInterface
		, PPipeline* const UniqueInterfacePipeline)
	{
		GD_DEBUG_ASSERT(UniqueRuntimeInterface != nullptr, "Invalid interface instance specified.");
		GD_DEBUG_ASSERT(UniqueInterfacePipeline != nullptr, "Invalid interface pipeline specified.");
	
		UniqueInterfacePipeline->_PipelineRuntimeInterfaces.InsertLast(UniqueRuntimeInterface);
	}
	//! @}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipelinePerThreadGroup class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents current stage of the pipeline group.
	enum PipelinePerThreadGroupStage : UInt8
	{
		PPIPELINE_STAGE_NOT_INITIALIZED,
		PPIPELINE_STAGE_PRE_INITIALIZING,
		PPIPELINE_STAGE_NOW_INITIALIZING,
		PPIPELINE_STAGE_POST_INITIALIZING,
		PPIPELINE_STAGE_PRE_UPDATING,
		PPIPELINE_STAGE_NOW_UPDATING,
		PPIPELINE_STAGE_POST_UPDATING,
		PPIPELINE_STAGE_PRE_DEINITIALIZING,
		PPIPELINE_STAGE_NOW_DEINITIALIZING,
		PPIPELINE_STAGE_POST_DEINITIALIZING,
	};	// enum PipelineStage

	// ------------------------------------------------------------------------------------------
	//! A m_Container that packs all pipelines inside the specific thread.
	class PPipelinePerThreadGroup
	{
		friend class PPipeline;
		friend class PPipelineManager;

	private:
		Vector<PPipeline*> _PipelineGroup;
		UniquePtr<Thread>  _PipelineGroupThread;
		PipelinePerThreadGroupStage _PipelineGroupStage;

	public:
		GDAPI void RunAsMasterGroup() {}
		GDAPI void RunThreadPipelines(bool const PipelineGroupIsMaster = false);
		GDAPI void ShutDownThreadPipelines();
		GDAPI void TickThreadPipelines();

	private:
		GDINL void _WaitOrTriggerEvent(ThreadEvent& _ThreadEvent);
		GDAPI IResult PreInitializeGroup();
		GDAPI IResult InitializeGroup();
		GDAPI IResult PostInitializeGroup();
		GDAPI IResult PreDeinitializeGroup();
		GDAPI IResult DeinitializeGroup();
		GDAPI IResult PostDeinitializeGroup();
		GDAPI void PreUpdateGroup();
		GDAPI void UpdateGroup();
		GDAPI void PostUpdateGroup();
	};	// class PPipelinePerThreadGroup

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class PPipeline
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes a new engine pipeline.
	//! @param PipelineName Name of the pipeline. Should be unique.
	//! @param PPipelineFlags Flags for this pipeline.
	//! @param PipelineOwnerThread In case this pipeline can be merged into the thread of the other one, that pipeline.
	GDAPI PPipeline::PPipeline(UInt16 const PipelineFlags /*= PPIPELINE_FLAG_NONE*/
		, PPipeline* const PipelineOwnerThread /*= nullptr*/)
	{
		if (!PipelineManager->_PipelinePerMainThreadGroup)
			PipelineManager->_PipelinePerMainThreadGroup = GD_NEW(PPipelinePerThreadGroup);

		_PipelineOwnerGroup = PipelineManager->_PipelinePerMainThreadGroup;
		_PipelineOwnerGroup->_PipelineGroup.InsertLast(this);

#if 0
		if (PipelineManager->_PipelineGroupsLayout == PPIPELINE_LAYOUT_UNKNOWN)
		{
			// No pipeline layout was creates, we will need to determine the optimal one.
			//! @todo Determine the optimal layout for our 
			PipelineManager->_PipelineGroupsLayout = PPIPELINE_LAYOUT_FORCE_SINGLE_THREADED;
		}

		// Now we will try to distribute current pipeline into one of the thread groups.
		switch (PipelineManager->_PipelineGroupsLayout)
		{
			// Forcedly attaching this pipeline to the main thread..
		case PPIPELINE_LAYOUT_FORCE_SINGLE_THREADED: 
			_PipelineOwnerGroup = PipelineManager->_PipelinePerMainThreadGroup;
			_PipelineOwnerGroup->_PipelineGroup.InsertLast(this);
			break;

			// Forcedly attaching this pipeline to the separate thread..
		case PPIPELINE_LAYOUT_FORCE_EACH_PIPELINE_IN_SEPARATE_THREAD:
			_PipelineOwnerGroup = GD_NEW(PPipelinePerThreadGroup);
			_PipelineOwnerGroup->_PipelineGroup.InsertLast(this);
			PipelineManager->_PipelinePerOtherThreadsGroups.InsertLast(_PipelineOwnerGroup);
			break;

			// We will manually automatically manage per thread pipeline distribution.
		case PPIPELINE_LAYOUT_AUTO_MULTITHREADED:
			GD_NOT_IMPLEMENTED();

		default: 
			GD_DEBUG_ASSERT_FALSE("Invalid pipelines groups layout.");
		}
#endif	// if 0
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipelineManager class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GDAPI PPipeline* PPipelineManager::GetMainThreadPipeline()
	{
		PPipeline static p;
		return &p;
	}

	GDAPI extern PPipelineManager* GetPipelineManager()
	{
		static PPipelineManager* const _PipelineManager = GD_NEW(PPipelineManager);
		return _PipelineManager;
	}
	GD_SINGLETON_IMPLEMENTATION(PPipelineManager);

	// ------------------------------------------------------------------------------------------
	//! Distributes all pipelines between threads and initializes the pipeline and
	//! and interfaces inside it.
	//! @param PipelineGroupsLayout Layout of the pipelines to consider while distributing between threads
	GDAPI void PPipelineManager::InitializeAllPipelines(
		PPipelineGroupsLayout const PipelineGroupsLayout /*= PPIPELINE_LAYOUT_AUTO_MULTITHREADED*/)
	{
		//! @todo Implement distributing the pipelines across threads.
		GD_NOT_USED(PipelineGroupsLayout);

		// And finally we are initializing our Master Group - it will trigger the events for all 
		// other existing pipeline threads.
		//! @todo Implement this correctly.
		_PipelinePerMainThreadGroup->PreInitializeGroup();
		_PipelinePerMainThreadGroup->InitializeGroup();
		_PipelinePerMainThreadGroup->PostInitializeGroup();
	}

	// ------------------------------------------------------------------------------------------
	//! Stops all updating loops inside the pipeline group threads and deinitializes all
	//! running pipelines.
	GDAPI void PPipelineManager::DeinitializeAllPipelines()
	{
		//! @todo Implement this correctly.
		_PipelinePerMainThreadGroup->PreDeinitializeGroup();
		_PipelinePerMainThreadGroup->DeinitializeGroup();
		_PipelinePerMainThreadGroup->PostDeinitializeGroup();
	}

	// ------------------------------------------------------------------------------------------
	//! Updates all pipelines across their thread groups.
	GDAPI void PPipelineManager::PreUpdateAllPipelines()
	{
		//! @todo Implement this correctly.
		_PipelinePerMainThreadGroup->PreUpdateGroup();
		_PipelinePerMainThreadGroup->UpdateGroup();
	}

	// ------------------------------------------------------------------------------------------
	//! Updates all pipelines across their thread groups.
	GDAPI void PPipelineManager::UpdateAllPipelines()
	{
		//! @todo Implement this correctly.
		_PipelinePerMainThreadGroup->PostUpdateGroup();
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipelinePerThreadGroup class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes all pipelines inside this group int the separate thread.
	//! @param PPipelineGroupIsMaster Specify true to male this thread a master - it will run in main thread and control all others.
	GDAPI void PPipelinePerThreadGroup::RunThreadPipelines(bool const PipelineGroupIsMaster)
	{
		if (!PipelineGroupIsMaster)
		{
			GD_NOT_IMPLEMENTED();
		}

		PreInitializeGroup();

		// Now we need to wait for all threads to finish pre-initialization..
		//! @todo Sync (wait) here..

		InitializeGroup();

		// Now we need to wait for all threads to finish initialization..
		//! @todo Sync (wait) here..

		PostInitializeGroup();
	}

	// ------------------------------------------------------------------------------------------
	//! Stops the updating loop inside the pipeline group and deinitializes it.
	GDAPI void PPipelinePerThreadGroup::ShutDownThreadPipelines()
	{
		if (_PipelineGroupThread == nullptr)
		{
			// This is a Master pipeline group, it must tick the events to trigger events and make
			// other pipeline threads update pipelines and interfaces.
			PreDeinitializeGroup();

			// Now we need to wait for all threads to finish pre-updating..
			//! @todo Sync (wait) here..

			DeinitializeGroup();

			// Now we need to wait for all threads to finish updating..
			//! @todo Sync (wait) here..

			PostDeinitializeGroup();
		}
	}

	GDAPI void PPipelinePerThreadGroup::TickThreadPipelines()
	{
		if (_PipelineGroupThread == nullptr)
		{
			// This is a Master pipeline group, it must tick the events to trigger events and make
			// other pipeline threads update pipelines and interfaces.
			PreUpdateGroup();

			// Now we need to wait for all threads to finish pre-updating..
			//! @todo Sync (wait) here..

			UpdateGroup();
			
			// Now we need to wait for all threads to finish updating..
			//! @todo Sync (wait) here..

			PostUpdateGroup();
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Triggers (if is a Master thread) or wait for the specified event.
	//! @param _ThreadEvent Event to trigger or wait for.
	GDINL void PPipelinePerThreadGroup::_WaitOrTriggerEvent(ThreadEvent& _ThreadEvent)
	{
		if (_PipelineGroupThread == nullptr)	
		{
			// This is a Master pipeline group, it triggers the event.
			_ThreadEvent.Trigger();
		}
		else
		{
			// This is a regular pipeline group, it waits for the Master to trigger the event.
			_ThreadEvent.Wait();
		}
	}

	// .. A little bit shitty code for all event loops of the engine ..

	// ------------------------------------------------------------------------------------------
	//! Function that pre-initializes all runtime components&interfaces inside the pipeline group.
	//! @returns Non-negative value if all the interfaces where successfully initialized.
	GDAPI IResult PPipelinePerThreadGroup::PreInitializeGroup()
	{
		// Synchronizing all pipeline threads to the pre-initializing phase..
		_WaitOrTriggerEvent(PipelineManager->_PipelinePreInitializationTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_PRE_INITIALIZING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Pre-initializing all runtime interfaces inside this group..
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
			{
				IResult const _PipelineRuntimeInterfacePreInitializationResult = _PipelineRuntimeInterface->OnRuntimePreInitialize();
				if (IFailed(_PipelineRuntimeInterfacePreInitializationResult))
				{
					// We are unable to pre-initialize some component inside this pipeline. 
					//! @todo Sync here..
					return _PipelineRuntimeInterfacePreInitializationResult;
				}
			}

			// Pre-initializing the pipeline itself.
			IResult const _PipelinePreInitializationResult = _Pipeline->OnRuntimePreInitialize();
			if (IFailed(_PipelinePreInitializationResult))
			{
				// We are unable to pre-initialize the pipeline itself.. 
				//! @todo Sync here..
				return _PipelinePreInitializationResult;
			}
		}
		//! @todo Sync here..
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function that initializes all runtime components&interfaces inside the pipeline group.
	//! @returns Non-negative value if all the interfaces where successfully initialized.
	GDAPI IResult PPipelinePerThreadGroup::InitializeGroup()
	{
		// Synchronizing all pipeline threads to the pre-initializing phase..
		_WaitOrTriggerEvent(PipelineManager->_PipelineInitializationTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_NOW_INITIALIZING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Initializing all runtime interfaces inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
			{
				IResult const _PipelineRuntimeInterfaceInitializationResult = _PipelineRuntimeInterface->OnRuntimeInitialize();
				if (IFailed(_PipelineRuntimeInterfaceInitializationResult))
				{
					// We are unable to initialize some component inside this pipeline. 
					//! @todo Sync here..
					return _PipelineRuntimeInterfaceInitializationResult;
				}
			}

			// Initializing the pipeline itself.
			IResult const _PipelineInitializationResult = _Pipeline->OnRuntimeInitialize();
			if (IFailed(_PipelineInitializationResult))
			{
				// We are unable to initialize the pipeline itself.. 
				//! @todo Sync here..
				return _PipelineInitializationResult;
			}
		}
		//! @todo Sync here..
		return IResult::Ok;
	}
		
	// ------------------------------------------------------------------------------------------
	//! Function that post-initializes all runtime components&interfaces inside the pipeline group.
	//! @returns Non-negative value if all the interfaces where successfully initialized.
	GDAPI IResult PPipelinePerThreadGroup::PostInitializeGroup()
	{
		// Synchronizing all pipeline threads to the post-initializing phase..
		_WaitOrTriggerEvent(PipelineManager->_PipelinePostInitializationTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_POST_INITIALIZING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Pre-initializing all runtime interfaces inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
			{
				IResult const _PipelineRuntimeInterfacePostInitializationResult = _PipelineRuntimeInterface->OnRuntimePostInitialize();
				if (IFailed(_PipelineRuntimeInterfacePostInitializationResult))
				{
					// We are unable to post-initialize some component inside this pipeline. 
					//! @todo Sync here..
					return _PipelineRuntimeInterfacePostInitializationResult;
				}
			}

			// Post-initializing the pipeline itself.
			IResult const _PipelinePostInitializationResult = _Pipeline->OnRuntimePostInitialize();
			if (IFailed(_PipelinePostInitializationResult))
			{
				// We are unable to post-initialize some component inside this pipeline. 
				//! @todo Sync here..
				return _PipelinePostInitializationResult;
			}
		}
		//! @todo Sync here..
		return IResult::Ok;
	}

	// ------------------------------------------------------------------------------------------
	//! Function that pre-deinitializes all runtime components&interfaces inside the pipeline group.
	//! @returns Non-negative value if all the interfaces where successfully deinitialized.
	GDAPI IResult PPipelinePerThreadGroup::PreDeinitializeGroup()
	{
		// Synchronizing all pipeline threads to the pre-deinitialization phase..
		IResult _PipelineGroupPreDeinitializationResult = IResult::Ok;
		_WaitOrTriggerEvent(PipelineManager->_PipelinePreDeinitializationTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_PRE_DEINITIALIZING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Deinitializing all runtime interfaces inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
			{
				IResult const _PipelineRuntimeInterfacePreDeinitializationResult = _PipelineRuntimeInterface->OnRuntimePreDeinitialize();
				if (IFailed(_PipelineRuntimeInterfacePreDeinitializationResult))
				{
					// Pre-deinitialization failed, but we will try to deinitialize other runtime interfaces anyway.
					_PipelineGroupPreDeinitializationResult = _PipelineRuntimeInterfacePreDeinitializationResult;
				}
			}

			// Pre-deinitializing the pipeline itself.
			IResult const _PipelinePreDeinitializationResult = _Pipeline->OnRuntimePreDeinitialize();
			if (IFailed(_PipelinePreDeinitializationResult))
			{
				// Pre-deinitialization failed, but we will try to deinitialize other runtime pipelines anyway.
				_PipelineGroupPreDeinitializationResult = _PipelinePreDeinitializationResult;
			}
		}
		//! @todo Sync here..
		return _PipelineGroupPreDeinitializationResult;
	}

	// ------------------------------------------------------------------------------------------
	//! Function that deinitializes all runtime components&interfaces inside the pipeline group.
	//! @returns Non-negative value if all the interfaces where successfully deinitialized.
	GDAPI IResult PPipelinePerThreadGroup::DeinitializeGroup()
	{
		// Synchronizing all pipeline threads to the deinitialization phase..
		IResult _PipelineGroupDeinitializationResult = IResult::Ok;
		_WaitOrTriggerEvent(PipelineManager->_PipelineDeinitializationTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_NOW_DEINITIALIZING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Deinitializing all runtime interfaces inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
			{
				IResult const _PipelineRuntimeInterfaceDeinitializationResult = _PipelineRuntimeInterface->OnRuntimeDeinitialize();
				if (IFailed(_PipelineRuntimeInterfaceDeinitializationResult))
				{
					// Pre-deinitialization failed, but we will try to deinitialize other runtime interfaces anyway.
					_PipelineGroupDeinitializationResult = _PipelineRuntimeInterfaceDeinitializationResult;
				}
			}

			// Pre-deinitializing the pipeline itself.
			IResult const _PipelineDeinitializationResult = _Pipeline->OnRuntimeDeinitialize();
			if (IFailed(_PipelineDeinitializationResult))
			{
				// Pre-deinitialization failed, but we will try to deinitialize other runtime pipelines anyway.
				_PipelineGroupDeinitializationResult = _PipelineDeinitializationResult;
			}
		}
		//! @todo Sync here..
		return _PipelineGroupDeinitializationResult;
	}

	// ------------------------------------------------------------------------------------------
	//! Function that post-deinitializes all runtime components&interfaces inside the pipeline group.
	//! @returns Non-negative value if all the interfaces where successfully deinitialized.
	GDAPI IResult PPipelinePerThreadGroup::PostDeinitializeGroup()
	{
		// Synchronizing all pipeline threads to the deinitialization phase..
		IResult _PipelineGroupPostDeinitializationResult = IResult::Ok;
		_WaitOrTriggerEvent(PipelineManager->_PipelinePostDeinitializationTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_POST_DEINITIALIZING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Deinitializing all runtime interfaces inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
			{
				IResult const _PipelineRuntimeInterfacePostDeinitializationResult = _PipelineRuntimeInterface->OnRuntimePostDeinitialize();
				if (IFailed(_PipelineRuntimeInterfacePostDeinitializationResult))
				{
					// Pre-deinitialization failed, but we will try to deinitialize other runtime interfaces anyway.
					_PipelineGroupPostDeinitializationResult = _PipelineRuntimeInterfacePostDeinitializationResult;
				}
			}

			// Post-deinitializing the pipeline itself.
			IResult const _PipelinePostDeinitializationResult = _Pipeline->OnRuntimePostDeinitialize();
			if (IFailed(_PipelinePostDeinitializationResult))
			{
				// Pre-deinitialization failed, but we will try to deinitialize other runtime pipelines anyway.
				_PipelineGroupPostDeinitializationResult = _PipelinePostDeinitializationResult;
			}
		}
		//! @todo Sync here..
		return _PipelineGroupPostDeinitializationResult;
	}

	// ------------------------------------------------------------------------------------------
	//! Function that pre-updates all runtime components&interfaces inside the pipeline group.
	GDAPI void PPipelinePerThreadGroup::PreUpdateGroup()
	{
		// Synchronizing all pipeline threads to the pre-updating phase..
		_WaitOrTriggerEvent(PipelineManager->_PipelinePreUpdatingTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_PRE_UPDATING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Pre-updating all runtime interfaces and the pipelines inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
				_PipelineRuntimeInterface->OnRuntimePreUpdate();
			_Pipeline->OnRuntimePreUpdate();
		}
		//! @todo Sync here..
	}

	// ------------------------------------------------------------------------------------------
	//! Function that updates all runtime components&interfaces inside the pipeline group.
	GDAPI void PPipelinePerThreadGroup::UpdateGroup()
	{
		// Synchronizing all pipeline threads to the updating phase..
		_WaitOrTriggerEvent(PipelineManager->_PipelineUpdatingTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_NOW_UPDATING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Updating all runtime interfaces and the pipelines inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
				_PipelineRuntimeInterface->OnRuntimeUpdate();
			_Pipeline->OnRuntimeUpdate();
		}
		//! @todo Sync here..
	}

	// ------------------------------------------------------------------------------------------
	//! Function that post-updates all runtime components&interfaces inside the pipeline group.
	GDAPI void PPipelinePerThreadGroup::PostUpdateGroup()
	{
		// Synchronizing all pipeline threads to the post-updating phase..
		_WaitOrTriggerEvent(PipelineManager->_PipelinePostUpdatingTriggerEvent);
		_PipelineGroupStage = PPIPELINE_STAGE_POST_UPDATING;
		for (auto _Pipeline : _PipelineGroup)
		{
			// Post-updating all runtime interfaces and the pipelines inside this group.
			for (auto _PipelineRuntimeInterface : _Pipeline->_PipelineRuntimeInterfaces)
				_PipelineRuntimeInterface->OnRuntimePostUpdate();
			_Pipeline->OnRuntimePostUpdate();
		}
		//! @todo Sync here..
	}

GD_NAMESPACE_END
