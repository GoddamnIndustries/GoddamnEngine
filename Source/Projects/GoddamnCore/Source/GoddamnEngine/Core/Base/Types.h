// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Definitions/Types.h
//! @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
//! Contains basic types definitions for GoddamnEngine.
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	// Never use this type directly!
#if !GD_DOCUMENTATION
	typedef bool Bool;
	typedef float Matrix3x3[3][3];
	typedef float Matrix3x4[3][4];
#endif	// if !GD_DOCUMENTATION

	typedef char    const* CStr;
	typedef wchar_t const* WideCStr;

	typedef void volatile* VHandle;
	typedef void const   * CHandle;
	typedef void         *  Handle;

	template<typename Tp>
	struct Dummy { };

#if GD_COMPILER_MSVC_COMPATIBLE
	typedef ::std::nullptr_t NullptrTp;
#else	// if GD_COMPILER_MSVC_COMPATIBLE
	typedef decltype(nullptr) NullptrTp;
#endif	// if GD_COMPILER_MSVC_COMPATIBLE

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Platform-Independent types. Size is constant on all architectures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename IntegerTp>
	struct IntegerLimits;
#define GD_DEFINE_INT_LIMITS(Tp) \
	template<> struct IntegerLimits<Tp> { \
		typedef Tp Type; \
		Tp static const Min = Tp##Min; \
		Tp static const Max = Tp##Max; \
	};	/* struct IntegerLimits<Tp>*/

	typedef ::std::int8_t	Int8;		
	Int8 static const		Int8Min		= INT8_MAX;
	Int8 static const		Int8Max		= INT8_MAX;
	GD_DEFINE_INT_LIMITS(	Int8		);

	typedef ::std::uint8_t	UInt8;		
	UInt8 static const		UInt8Min	= 0;
	UInt8 static const		UInt8Max	= UINT8_MAX;
	GD_DEFINE_INT_LIMITS(	UInt8		);
	typedef UInt8 Byte;

	typedef ::std::int16_t	Int16;		
	Int16 static const		Int16Min	= INT16_MIN;
	Int16 static const		Int16Max	= INT16_MAX;
	GD_DEFINE_INT_LIMITS(	Int16		);

	typedef ::std::uint16_t	UInt16;		
	UInt16 static const		UInt16Min	= 0;
	UInt16 static const		UInt16Max	= UINT16_MAX;
	GD_DEFINE_INT_LIMITS(	UInt16		);

	typedef ::std::int32_t	Int32;
	Int32 static const		Int32Min	= INT32_MIN;
	Int32 static const		Int32Max	= INT32_MAX;
	GD_DEFINE_INT_LIMITS(	Int32		);

	typedef ::std::uint32_t	UInt32;
	UInt32 static const		UInt32Min	= 0;
	UInt32 static const		UInt32Max	= UINT32_MAX;
	GD_DEFINE_INT_LIMITS(	UInt32		);

	typedef ::std::int64_t	Int64;
	Int64 static const		Int64Min	= INT64_MIN;
	Int64 static const		Int64Max	= INT64_MAX;
	GD_DEFINE_INT_LIMITS(	Int64		);

	typedef ::std::uint64_t	UInt64;
	UInt64 static const		UInt64Min	= 0;
	UInt64 static const		UInt64Max	= UINT64_MAX;
	GD_DEFINE_INT_LIMITS(	UInt64		);

#undef GD_DEFINE_INT_LIMITS

	typedef UInt8			Float8		[1];
	typedef UInt8			Float16		[2];
	typedef float			Float32;
	typedef double			Float64;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Platform-Dependent types. Size may vary on different architectures.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
	static_assert(sizeof(void*) == 8, "Invalidely selected 64-bit platform.");

	typedef UInt64			UIntPtr;
	UIntPtr static const	UIntPtrMin	= UInt64Min;
	UIntPtr static const	UIntPtrMax	= UInt64Max;

	typedef Int64			IntPtr;
	IntPtr static const		IntPtrMin	= Int64Min;
	IntPtr static const		IntPtrMax	= Int64Max;

	typedef UInt64			SizeTp;
	SizeTp static const		SizeTpMin	= UInt64Min;
	SizeTp static const		SizeTpMax	= UInt64Max;

	typedef Int64			PtrDiffTp;
	PtrDiffTp static const	PtrDiffTpMin = Int64Min;
	PtrDiffTp static const	PtrDiffTpMax = Int64Max;

#elif GD_ARCHITECTURE_X86 || GD_ARCHITECTURE_ARM32	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64
	static_assert(sizeof(void*) == 4, "Invalidely selected 32-bit platform.");

	typedef UInt32			UIntPtr;
	UIntPtr static const	UIntPtrMin	= UInt32Min;
	UIntPtr static const	UIntPtrMax	= UInt32Max;

	typedef Int32			IntPtr;
	IntPtr static const		IntPtrMin	= Int32Min;
	IntPtr static const		IntPtrMax	= Int32Max;

	typedef UInt32			SizeTp;
	SizeTp static const		SizeTpMin	= UInt32Min;
	SizeTp static const		SizeTpMax	= UInt32Max;

	typedef Int32			PtrDiffTp;
	PtrDiffTp static const	PtrDiffTpMin = Int32Min;
	PtrDiffTp static const	PtrDiffTpMax = Int32Max;

#endif	// if GD_ARCHITECTURE_X86 || GD_ARCHITECTURE_ARM32

//	GD_DEPRECATED("Please, use 'SizeTp' instead of 'size_t'")		typedef ::std::size_t    size_t;
//	GD_DEPRECATED("Please, use 'PtrDiffTp' instead of 'ptrdiff_t'") typedef ::std::ptrdiff_t ptrdiff_t;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Container limits.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Interface that disables copy constructor and copy assignment operators in all 
	//! child classes.
	class IUncopiable
	{
	public:
		GDINL IUncopiable() = default;
	private:
		GDINT IUncopiable(IUncopiable const& Other) = delete;
		GDINT IUncopiable& operator= (IUncopiable const& Other) = delete;
	};	// class IUncopiable

	// ------------------------------------------------------------------------------------------
	//! Interface that disables move constructor and move assignment operators in all
	//!        child classes.
	class IUnmovable
	{
	public:
		GDINL IUnmovable() = default;
	private:
		GDINT IUnmovable(IUnmovable&& Other) = delete;
		GDINT IUnmovable& operator= (IUnmovable&& Other) = delete;
	};	// class IUnmovalbe

	// ------------------------------------------------------------------------------------------
	//! Interface that disables assignment operators in all child classes.
	class IUnassignable
	{
	public:
		GDINL IUnassignable() = default;
	private:
		GDINT IUnassignable& operator= (IUnassignable const& Other) = delete;
		GDINT IUnassignable& operator= (IUnassignable     && Other) = delete;
	};	// class IUnassignable

	// ------------------------------------------------------------------------------------------
	//! Interface that disables swab function in all child classes.
	class IUnswappable
	{
	public:
		GDINL IUnswappable() = default;
	private: 
		//! @todo GCC does not compiles this: 
		GDINT friend void Swap(IUnswappable&  First, IUnswappable&  Second) = delete;
		GDINT friend void Swap(IUnswappable&& First, IUnswappable&& Second) = delete;
	};	// class IUnswappable

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Basic interfaces.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	struct IVirtuallyDestructible : public IUncopiable
	{
		GDINL virtual ~IVirtuallyDestructible() = default;
	};	// struct IVirtuallyDestructible

	// ------------------------------------------------------------------------------------------
	struct IRunnable
	{
	protected:
		GDINL IRunnable() = default;
		GDINL virtual ~IRunnable() = default;

	public:

		GDAPI virtual void OnInitialize() { }
		GDAPI virtual void OnDestroy() { }

		GDAPI virtual void OnStart() { }
		GDAPI virtual void OnUpdate() { }
		GDAPI virtual void OnLateUpdate() { }
		GDAPI virtual void OnFixedUpdate() { }
	};	// struct IRunnable

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Array size counting safe macros.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// ------------------------------------------------------------------------------------------
	//! Useful macro for safely counting array length.
	//! @param Array Array, the length of ones would be determined.
#define GD_ARRAY_LENGTH(Array) static_cast<SizeTp>(sizeof(ArrayLengthHelper((Array))))				
	template <typename Tp, SizeTp Length>
	GDINT UInt8 (&ArrayLengthHelper(Tp(&Array)[Length]))[Length];

GD_NAMESPACE_END
