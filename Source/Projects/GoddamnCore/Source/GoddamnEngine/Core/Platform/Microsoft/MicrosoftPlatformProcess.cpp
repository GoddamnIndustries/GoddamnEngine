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
#include <GoddamnEngine/Core/Platform/PlatformProcess.h>
#if GD_PLATFORM_API_MICROSOFT

#include <GoddamnEngine/Core/IO/Paths.h>
#include <GoddamnEngine/Core/Platform/PlatformEncoding.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#include <Windows.h>
#include <TlHelp32.h>

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	//! Process on Microsoft platforms.
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	class GD_PLATFORM_KERNEL MicrosoftPlatformProcess : public IPlatformProcess
	{
	private:

		// ------------------------------------------------------------------------------------------
		// Dynamic-link library utilities.
		// ------------------------------------------------------------------------------------------

#if 0
	private:
		Stack<DLL_DIRECTORY_COOKIE> m_DynamicLibraryDirectoriesStack;
		/*!
		 * Adds a new directory to look for dynamic-link libraries.
		 * 
		 * @param libraryDirectoryPath Path to the directory with libraries.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryPushDirectory(WideString const& libraryDirectoryPath) override final
		{
			auto const libraryDirectoryPathSystem = Paths::Platformize(libraryDirectoryPath);
			auto const libraryDirectoryHandle = AddDllDirectory(libraryDirectoryPathSystem.CStr());
			if (libraryDirectoryHandle != nullptr)
			{
				m_DynamicLibraryDirectoriesStack.InsertLast(libraryDirectoryHandle);
				return true;
			}
			return false;
		}

		/*!
		 * Removes last added dynamic-link libraries directory.
		 */
		GDINT virtual void DynamicLibraryPopDirectory() override final
		{
			RemoveDllDirectory(m_DynamicLibraryDirectoriesStack.GetLast());
			m_DynamicLibraryDirectoriesStack.EraseLast();
		}
#endif	// if 0

		/*!
		 * Loads the dynamic-link library.
		 * 
		 * @param libraryPath Path to the library.
		 * @param libraryHandle Library Handle.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryLoad(WideString const& libraryPath, Handle& libraryHandle) override final
		{
			SetErrorMode(SEM_NOOPENFILEERRORBOX);
			auto const libraryPathSystem = Paths::Platformize(libraryPath);
			auto const libraryHandleSystem = LoadLibraryExW(libraryPathSystem.CStr(), nullptr, LOAD_LIBRARY_SEARCH_USER_DIRS);
			if (libraryHandleSystem != nullptr)
			{
				libraryHandle = libraryHandleSystem;
				return true;
			}
			return false;
		}

		/*!
		 * Unloads the dynamic-link library.
		 * 
		 * @param libraryHandle Library handle.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryUnload(Handle const libraryHandle) override final
		{
			GD_ASSERT(libraryHandle != nullptr);
			auto const libraryHandleSystem = static_cast<HMODULE>(libraryHandle);
			return FreeLibrary(libraryHandleSystem) == TRUE;
		}

		/*!
		 * Resolves the exported function with specified name.
		 * 
		 * @param libraryHandle Library handle.
		 * @param libraryExportedFunctionName Name of exported function.
		 * @param libraryExportedFunctionPointer Pointer to the exported function.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool DynamicLibraryResolve(Handle const libraryHandle, String const& libraryExportedFunctionName, Handle& libraryExportedFunctionPointer) const override final
		{
			GD_ASSERT(libraryHandle != nullptr);
			auto const libraryHandleSystem = static_cast<HMODULE>(libraryHandle);
			auto const libraryExportedFunctionPointerSystem = GetProcAddress(libraryHandleSystem, libraryExportedFunctionName.CStr());
			if (libraryHandleSystem != nullptr)
			{
				libraryExportedFunctionPointer = libraryExportedFunctionPointerSystem;
				return true;
			}
			return false;
		}
		
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
		GDINT virtual bool PipeCreate(Handle& pipeReadEndHandle, Handle& pipeWriteEndHandle) override final
		{
			HANDLE pipeReadEndHandleSystem, pipeWriteEndHandleSystem;
			SECURITY_ATTRIBUTES pipeSecurityAttributes = {};
			pipeSecurityAttributes.nLength = sizeof(pipeSecurityAttributes);
			pipeSecurityAttributes.bInheritHandle = TRUE;
			if (CreatePipe(&pipeReadEndHandleSystem, &pipeWriteEndHandleSystem, &pipeSecurityAttributes, 0) == TRUE)
			{
				pipeReadEndHandle = pipeReadEndHandleSystem;
				pipeWriteEndHandle = pipeWriteEndHandleSystem;
				return true;
			}
			return false;
		}

		/*!
		 * Destroys the pipe.
		 * 
		 * @param pipeReadEndHandle Handle of the pipe read end.
		 * @param pipeWriteEndHandle Handle of the pipe read end.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PipeDestroy(Handle const pipeReadEndHandle, Handle const pipeWriteEndHandle) override final
		{
			GD_ASSERT(pipeReadEndHandle != nullptr && pipeWriteEndHandle != nullptr);
			return CloseHandle(pipeReadEndHandle) == TRUE && CloseHandle(pipeWriteEndHandle) == TRUE;
		}

		/*!
		 * Reads all pending content from the pipe read end.
		 * 
		 * @param pipeReadEndHandle Handle of the pipe read end.
		 * @param pipeContent Output for pipe content.
		 * 
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool PipeReadPendingContent(Handle const pipeReadEndHandle, WideString& pipeContent) override final
		{
			GD_ASSERT(pipeReadEndHandle != nullptr);
			DWORD pipeContentUTF8Length;
			if (PeekNamedPipe(pipeReadEndHandle, nullptr, 0, nullptr, &pipeContentUTF8Length, nullptr) == TRUE)
			{
				DWORD numBytesRead;
				String pipeContentUTF8(static_cast<SizeTp>(pipeContentUTF8Length));
				if (ReadFile(pipeReadEndHandle, pipeContentUTF8.CStr(), pipeContentUTF8Length, &numBytesRead, nullptr) == TRUE && numBytesRead > 0)
				{
					pipeContent = StringConv::DecodeUTF8(pipeContentUTF8.CStr(), numBytesRead);
					return true;
				}
			}
			return false;
		}

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
		GDINT virtual bool ProcessCreate(ProcessCreateInfo const& processCreateInfo, Handle& processHandle, UInt64* const processId) override final
		{
			// Setting the process create flags..
			DWORD processCreateFlags = 0;
			switch (processCreateInfo.ProcessPriority)
			{
				case ProcessPriority::Lowest:
					processCreateFlags |= IDLE_PRIORITY_CLASS;
					break;
				case ProcessPriority::BelowNormal:
					processCreateFlags |= BELOW_NORMAL_PRIORITY_CLASS;
					break;
				case ProcessPriority::Normal:
					processCreateFlags |= NORMAL_PRIORITY_CLASS;
					break;
				case ProcessPriority::AboveNormal:
					processCreateFlags |= ABOVE_NORMAL_PRIORITY_CLASS;
					break;
				case ProcessPriority::Highest:
					processCreateFlags |= HIGH_PRIORITY_CLASS;
					break;
			}
			if ((processCreateInfo.ProcessLaunchFlags & ProcessLaunchFlags::LaunchDetached) != 0)
			{
				processCreateFlags |= DETACHED_PROCESS;
			}

			// Setting the process startup info..
			PROCESS_INFORMATION processInfo = {};
			STARTUPINFOW processStartupInfo = {};
			processStartupInfo.cb = sizeof(processStartupInfo);
			processStartupInfo.dwX = processStartupInfo.dwY = static_cast<DWORD>(CW_USEDEFAULT);
			processStartupInfo.dwXSize = processStartupInfo.dwYSize = static_cast<DWORD>(CW_USEDEFAULT);
			processStartupInfo.hStdInput = processCreateInfo.ProcessStandardInput;
			processStartupInfo.hStdError = processCreateInfo.ProcessStandardError;
			processStartupInfo.hStdOutput = processCreateInfo.ProcessStandardOutput;
			if ((processCreateInfo.ProcessLaunchFlags & ProcessLaunchFlags::LaunchHidden) != 0)
			{
				processStartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
				processStartupInfo.wShowWindow = SW_HIDE;
			}
			else if ((processCreateInfo.ProcessLaunchFlags & ProcessLaunchFlags::LaunchMinimized) != 0)
			{
				processStartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
				processStartupInfo.wShowWindow = SW_SHOWMINNOACTIVE;
			}
			if (processCreateInfo.ProcessStandardInput != nullptr || processCreateInfo.ProcessStandardOutput != nullptr)
			{
				processStartupInfo.dwFlags |= STARTF_USESTDHANDLES;
			}

			// Setting the process security attributes..
			SECURITY_ATTRIBUTES processSecurityAttributes = {};
			processSecurityAttributes.nLength = sizeof(processSecurityAttributes);
			processSecurityAttributes.bInheritHandle = TRUE;

			// Launching the process..
			auto const processLaunchCommandSystem = processCreateInfo.ProcessExecutablePath + L" " + processCreateInfo.ProcessExecutableArguments;
			if (CreateProcessW(nullptr, const_cast<LPWSTR>(processLaunchCommandSystem.CStr())
				, &processSecurityAttributes, &processSecurityAttributes, TRUE, processCreateFlags, nullptr, nullptr, &processStartupInfo, &processInfo) == TRUE)
			{
				if (processId != nullptr)
				{
					*processId = processInfo.dwProcessId;
				}
				CloseHandle(processInfo.hThread);
				processHandle = processInfo.hProcess;
				return true;
			}
			return false;
		}

		/*!
		 * Terminaties a process.
		 * 
		 * @param processHandle Handle of the process on success.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool ProcessTerminate(Handle const processHandle) override final
		{
			GD_ASSERT(processHandle != nullptr);
			return TerminateProcess(processHandle, 0) == TRUE && CloseHandle(processHandle) == TRUE;
		}

		/*!
		 * Terminaties a process and all it's child processes.
		 * 
		 * @param processHandle Handle of the process on success.
		 * @returns True if operation succeeded.
		 */
		GDINT virtual bool ProcessTerminateRecursive(Handle const processHandle) override final
		{
			GD_ASSERT(processHandle != nullptr);
			auto result = false;
			auto const processId = GetProcessId(processHandle);
			auto const runningProcessesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (runningProcessesSnapshot != INVALID_HANDLE_VALUE)
			{
				PROCESSENTRY32W runningPrcessesEntry = {};
				runningPrcessesEntry.dwSize = sizeof(runningPrcessesEntry);
				if (Process32FirstW(runningProcessesSnapshot, &runningPrcessesEntry) == TRUE)
				{
					do
					{
						if (runningPrcessesEntry.th32ParentProcessID != processId)
						{
							continue;
						}
						auto const processChildHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, runningPrcessesEntry.th32ProcessID);
						if (processChildHandle != nullptr)
						{
							ProcessTerminateRecursive(processHandle);
						}
					} while (Process32NextW(runningProcessesSnapshot, &runningPrcessesEntry) == TRUE);
					result = true;
				}
			}
			result &= CloseHandle(runningProcessesSnapshot) == TRUE;
			result &= ProcessTerminate(processHandle);
			return result;
		}
	};	// class MicrosoftPlatformProcess

	GD_IMPLEMENT_SINGLETON(IPlatformProcess, MicrosoftPlatformProcess);

GD_NAMESPACE_END

#endif	// if GD_PLATFORM_API_MICROSOFT
