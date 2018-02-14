// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file
 * Process implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Templates/Singleton.h>

GD_NAMESPACE_BEGIN

	enum class ProcessLaunchFlags
	{
		None = 0,
		LaunchDetached  = GD_BIT(0),
		LaunchMinimized = GD_BIT(1),
		LaunchHidden    = GD_BIT(2),
	};	// enum class ProcessLaunchFlags
	GD_ENUM_CLASS_DEFINE_FLAG_OPERATORS(ProcessLaunchFlags)

	enum ProcessPriority
	{
		Lowest,
		BelowNormal,
		Normal,
		AboveNormal,
		Highest,
	};	// enum ProcessPriority

	struct ProcessCreateInfo final
	{
		WideString ProcessExecutablePath;
		WideString ProcessExecutableArguments;
		ProcessLaunchFlags ProcessLaunchFlags;
		ProcessPriority ProcessPriority;
		Handle ProcessStandardInput;
		Handle ProcessStandardError;
		Handle ProcessStandardOutput;
	};	// struct ProcessCreateInfo final

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Process.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL IPlatformProcess : public Singleton<IPlatformProcess>
	{
	public:

		// ------------------------------------------------------------------------------------------
		// Dynamic-link library utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Loads the dynamic-link library.
		 * 
		 * @param libraryPath Path to the library.
		 * @param libraryHandle Library Handle.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryLoad(WideString const& libraryPath, Handle& libraryHandle) GD_PURE_VIRTUAL;

		/*!
		 * Unloads the dynamic-link library.
		 * 
		 * @param libraryHandle Library handle.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryUnload(Handle const libraryHandle) GD_PURE_VIRTUAL;

		/*!
		 * Resolves the exported function with specified name.
		 * 
		 * @param libraryHandle Library handle.
		 * @param libraryExportedFunctionName Name of exported function.
		 * @param libraryExportedFunctionPointer Pointer to the exported function.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryResolve(Handle const libraryHandle, String const& libraryExportedFunctionName, Handle& libraryExportedFunctionPointer) const GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Pipe utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Creates a pipe.
		 * 
		 * @param pipeReadEndHandle Handle of the pipe read end.
		 * @param pipeWriteEndHandle Handle of the pipe read end.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PipeCreate(Handle& pipeReadEndHandle, Handle& pipeWriteEndHandle) GD_PURE_VIRTUAL;

		/*!
		 * Destroys the pipe.
		 * 
		 * @param pipeReadEndHandle Handle of the pipe read end.
		 * @param pipeWriteEndHandle Handle of the pipe read end.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PipeDestroy(Handle const pipeReadEndHandle, Handle const pipeWriteEndHandle) GD_PURE_VIRTUAL;

		/*!
		 * Reads all pending content from the pipe read end.
		 * 
		 * @param pipeReadEndHandle Handle of the pipe read end.
		 * @param pipeContent Output for pipe content.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PipeReadPendingContent(Handle const pipeReadEndHandle, WideString& pipeContent) GD_PURE_VIRTUAL;

		// ------------------------------------------------------------------------------------------
		// Process utilities.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Creates a process.
		 * 
		 * @param processCreateInfo Process create information.
		 * @param processHandle Handle of the process on success.
		 * @param processId Optional pointer for process ID output.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool ProcessCreate(ProcessCreateInfo const& processCreateInfo, Handle& processHandle, UInt64* const processId = nullptr) GD_PURE_VIRTUAL;

		/*!
		 * Terminaties a process.
		 * 
		 * @param processHandle Handle of the process on success.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool ProcessTerminate(Handle const processHandle) GD_PURE_VIRTUAL;

		/*!
		 * Terminaties a process and all it's child processes.
		 * 
		 * @param processHandle Handle of the process on success.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool ProcessTerminateRecursive(Handle const processHandle) GD_PURE_VIRTUAL;

	};	// class IPlatformProcess

GD_NAMESPACE_END
