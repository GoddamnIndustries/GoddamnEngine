#include <GoddamnEngine/Core/Reflection/Assembly/Assembly.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// ==========================================================================================
	Assembly::Assembly(
		String const& FileName
	) : NativePointer(nullptr)
	{
		size_t const DLLPathExtentionIndex = FileName.ReverseFind('.');
		if (DLLPathExtentionIndex != -1)
		{
			String const DLLExtention = FileName.Subtring(DLLPathExtentionIndex + 1);
			if (DLLExtention.ToUpper() == "DLL") {
				// Disabling errors for DLL opening when system throws message box
#if (defined(GD_DEBUG)) && (defined(GD_COMPILER_MSVC))
				UINT const DefaultErrorMode = GetErrorMode();
				SetErrorMode(SEM_FAILCRITICALERRORS);
				this->NativePointer = (reinterpret_cast<handle>(::LoadLibraryA(&FileName[0])));
				SetErrorMode(DefaultErrorMode);
#else	// if (defined(GD_DEBUG)) && (defined(GD_COMPILER_MSVC))
				this->NativePointer = (reinterpret_cast<handle>(::LoadLibraryA(&FileName[0])));
#endif	// if (defined(GD_DEBUG)) && (defined(GD_COMPILER_MSVC))
			}
		}
	}

	/// ==========================================================================================
	/// ==========================================================================================
	Assembly::~Assembly()
	{
		if (this->WasSuccessfullyLoaded()) {
			FreeModule(reinterpret_cast<HMODULE>(this->NativePointer));
		}
	}

	/// ==========================================================================================
	/// ==========================================================================================
	chandle Assembly::GetNativeMethod(String const& FunctionName) const
	{
		GD_ASSERT((this->WasSuccessfullyLoaded()),
			"Library was not loaded, but attempted to obtain procedure address");

		return reinterpret_cast<chandle>(GetProcAddress(
			   reinterpret_cast<HMODULE>(this->NativePointer), (&FunctionName[0])));
	}

GD_NAMESPACE_END
