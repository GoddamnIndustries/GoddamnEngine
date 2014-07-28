#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// ==========================================================================================
	Assembly::Assembly(
		_In_ String const& FileName
	) : NativePointer(nullptr)
	{
		size_t const DLLPathExtentionIndex = FileName.ReverseFind('.');
		if (DLLPathExtentionIndex != -1)
		{
			String const DLLExtention = FileName.GetSubstring(DLLPathExtentionIndex + 1);
			if (DLLExtention.ToUpper() == "DLL") {
				// Disabling errors for DLL opening when system throws message box
#if (defined(GD_DEBUG)) && (defined(GD_COMPILER_MSC))
				UINT const DefaultErrorMode = GetErrorMode();
				SetErrorMode(SEM_FAILCRITICALERRORS);
				self->NativePointer = (reinterpret_cast<handle>(::LoadLibraryA(&FileName[0])));
				SetErrorMode(DefaultErrorMode);
#else	// if (defined(GD_DEBUG)) && (defined(GD_COMPILER_MSC))
				self->NativePointer = (reinterpret_cast<handle>(::LoadLibraryA(&FileName[0])));
#endif	// if (defined(GD_DEBUG)) && (defined(GD_COMPILER_MSC))
			}
		}
	}

	/// ==========================================================================================
	/// ==========================================================================================
	Assembly::~Assembly()
	{
		if (self->WasSuccessfullyLoaded()) {
			FreeModule(reinterpret_cast<HMODULE>(self->NativePointer));
		}
	}

	/// ==========================================================================================
	/// ==========================================================================================
	chandle Assembly::GetNativeMethod(_In_ String const& FunctionName) const
	{
		GD_ASSERT((self->WasSuccessfullyLoaded()),
			"Library was not loaded, but attempted to obtain procedure address");

		return reinterpret_cast<chandle>(GetProcAddress(
			   reinterpret_cast<HMODULE>(self->NativePointer), (&FunctionName[0])));
	}

GD_NAMESPACE_END
