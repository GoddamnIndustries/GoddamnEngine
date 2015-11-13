// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Pipeline/Pipeline.h
 * @todo Rewrite this awful code one day.
 * File contains base class for all engine runtime pipelines.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Singleton.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Platform/Threading/Threading.h>
#include <GoddamnEngine/Engine/Interface/UniqueInterface.h>

GD_NAMESPACE_BEGIN

	class PPipeline;
	class PPipelinePerThreadGroup;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class PPipeline
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines the list of the supported pipeline flags.
	enum PPipelineFlags : UInt16
	{
		PPIPELINE_FLAG_NONE                    = 0,
		PPIPELINE_FLAG_DESIRES_SEPARATE_THREAD = GD_BIT(0),
		PPIPELINE_FLAG_CAN_BE_MERGED_INTO_THE_THREAD_OF_THE_EXISTING_PIPELINE = GD_BIT(1),
	};	// enum PPipelineFlags

	// ------------------------------------------------------------------------------------------
	//! Provides handling runtime unique interfaces and corresponding components.
	//! Each pipeline may run in it's own thread, but all threads are synchronized between
	//! (pre-/post-) event loops.
	class PPipeline : public IRuntimeUniqueInterface
	{
		friend class PPipelineManager;
		friend class PPipelinePerThreadGroup;
		friend struct IUniqueInterfacePointerBase;

	private:
		PPipelinePerThreadGroup*         _PipelineOwnerGroup;
		Vector<IInterface*>              _PipelineStaticInterfaces;
		Vector<IRuntimeUniqueInterface*> _PipelineRuntimeInterfaces;

	protected /*public*/:

		// ------------------------------------------------------------------------------------------
		//! Initializes a new engine pipeline.
		//! @param PipelineName Name of the pipeline. Should be unique.
		//! @param PPipelineFlags Flags for this pipeline.
		//! @param PipelineOwnerThread In case this pipeline can be merged into the thread of the other one, that pipeline.
		GDAPI explicit PPipeline(UInt16 const PipelineFlags = PPIPELINE_FLAG_NONE, PPipeline* const PipelineOwnerThread = nullptr);
	
	};	// class PPipeline

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PPipelineManager class.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Represents a layout, in which all pipeline groups are running.
	enum PPipelineGroupsLayout
	{
		PPIPELINE_LAYOUT_FORCE_SINGLE_THREADED,
		PPIPELINE_LAYOUT_FORCE_EACH_PIPELINE_IN_SEPARATE_THREAD,
		PPIPELINE_LAYOUT_AUTO_MULTITHREADED,
		PPIPELINE_LAYOUT_UNKNOWN,
	};	// enum PPipelineGroupsLayout

	// ------------------------------------------------------------------------------------------
	//! Class that manages all existing pipelines, threading layout, ticking and other
	//! complicated stuff.
	class PPipelineManager final : public Singleton<PPipelineManager>
	{
		friend class PPipeline;
		friend class PPipelinePerThreadGroup;

	private:
		PPipelineGroupsLayout            _PipelineGroupsLayout = PPIPELINE_LAYOUT_UNKNOWN;
		PPipelinePerThreadGroup*         _PipelinePerMainThreadGroup;
		Vector<PPipelinePerThreadGroup*> _PipelinePerOtherThreadsGroups;
		ThreadEvent                      _PipelinePreInitializationTriggerEvent;
		ThreadEvent                      _PipelineInitializationTriggerEvent;
		ThreadEvent                      _PipelinePostInitializationTriggerEvent;
		ThreadEvent                      _PipelinePreDeinitializationTriggerEvent;
		ThreadEvent                      _PipelineDeinitializationTriggerEvent;
		ThreadEvent                      _PipelinePostDeinitializationTriggerEvent;
		ThreadEvent                      _PipelinePreUpdatingTriggerEvent;
		ThreadEvent                      _PipelineUpdatingTriggerEvent;
		ThreadEvent                      _PipelinePostUpdatingTriggerEvent;
		Atomic<UInt32>                   _PipelineProgressCount;

	public:

		// ------------------------------------------------------------------------------------------
		//! Distributes all pipelines between threads and initializes the pipeline and
		//! and interfaces inside it.
		//! @param PipelineGroupsLayout Layout of the pipelines to consider while distributing between threads
		GDAPI void InitializeAllPipelines(PPipelineGroupsLayout const PipelineGroupsLayout = PPIPELINE_LAYOUT_AUTO_MULTITHREADED);
		
		// ------------------------------------------------------------------------------------------
		//! Stops all updating loops inside the pipeline group threads and deinitializes all
		//! running pipelines.
		GDAPI void DeinitializeAllPipelines();

		// ------------------------------------------------------------------------------------------
		//! Updates all pipelines across their thread groups.
		GDAPI void PreUpdateAllPipelines();
		GDAPI void UpdateAllPipelines();

	public:
		
		GDAPI PPipeline* GetMainThreadPipeline();
		GDINL PPipeline* GetAppropriateRenderingPipeline()
		{
			return GetMainThreadPipeline();
		}

	};	// class PPipelineManager final

	GDAPI extern PPipelineManager* GetPipelineManager();
#define PipelineManager GetPipelineManager()

GD_NAMESPACE_END
