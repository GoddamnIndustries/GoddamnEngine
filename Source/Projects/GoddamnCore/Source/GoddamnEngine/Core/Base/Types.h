// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Base/Types.h
 * @note This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
 * Contains basic types definitions for GoddamnEngine.
 */
#pragma once
#if !defined(GD_INSIDE_INCLUDE_H)
#	error This file should be never directly included, please consider using <GoddamnEngine/Include.h> instead.
#endif	// if !defined(GD_INSIDE_INCLUDE_H)

/*!
 * Computes length of the array.
 * Should be used only in the code that requires compile-time results.
 */
#define GD_ARRAY_LENGTH(array) static_cast<GD::SizeTp>(sizeof(GD::GetArrayLengthHelper((array))))		

/*!
 * Notifies about stubbed code section.
 */
#define GD_STUBBED(x) \
	do { \
		static auto alreadySeenThisStubbedSection = false; \
		if (!alreadySeenThisStubbedSection) \
		{ \
			alreadySeenThisStubbedSection = true; \
			fprintf_s(stderr, "STUBBED: %s at %s:%d (%s)\n", #x, __FILE__, __LINE__, __FUNCTION__); \
		} \
	} while (0)

GD_NAMESPACE_BEGIN

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                         Platform independent types.                              ******
	// ******                       (Constant size on all platforms)                           ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	
	/*!
	 * Type of 'nullptr' constant.
	 */
	typedef decltype(nullptr) NullptrTp;

	typedef void         *  Handle;
	typedef void const   * CHandle;
	typedef void volatile* VHandle;

	/*!
	 * This and only this types should be used for C string operations.
	 * No 'char' or 'char const*' are allowed.
	 */
	typedef char		 Char;
	typedef char const*	 CStr;
	Char static	const	 CharMin		= CHAR_MIN;
	Char static	const	 CharMax		= CHAR_MAX;
	static_assert(sizeof(Char) == 1, "Invalid 'Char' size.");

	/*!
	 * This and only this types should be used for wide C string operations.
	 * No 'wchar_t' or 'wchar_t const*' are allowed.
	 */
	typedef wchar_t			WideChar;
	typedef wchar_t const*	WideCStr;
	WideChar static	const	WideCharMin = WCHAR_MIN;
	WideChar static	const	WideCharMax = WCHAR_MAX;
	static_assert(sizeof(WideChar) == 2, "Invalid 'WideChar' size.");	// This would not compile in GCC/Clang. But who cares?

	typedef ::int8_t	 Int8;		
	Int8 static const	 Int8Min		= INT8_MAX;
	Int8 static const	 Int8Max		= INT8_MAX;
	static_assert(sizeof(Int8) == 1, "Invalid 'Int8' size.");

	typedef ::uint8_t	 UInt8;		
	UInt8 static const	 UInt8Min	= 0;
	UInt8 static const	 UInt8Max	= UINT8_MAX;
	static_assert(sizeof(UInt8) == 1, "Invalid 'UInt8' size.");

	typedef UInt8		 Byte;	// Should be used for byte operations only.

	typedef ::int16_t	 Int16;		
	Int16 static const	 Int16Min	= INT16_MIN;
	Int16 static const	 Int16Max	= INT16_MAX;
	static_assert(sizeof(Int16) == 2, "Invalid 'Int16' size.");

	typedef ::uint16_t	 UInt16;		
	UInt16 static const	 UInt16Min	= 0;
	UInt16 static const	 UInt16Max	= UINT16_MAX;
	static_assert(sizeof(UInt16) == 2, "Invalid 'UInt16' size.");

	typedef ::int32_t	 Int32;
	Int32 static const	 Int32Min	= INT32_MIN;
	Int32 static const	 Int32Max	= INT32_MAX;
	static_assert(sizeof(Int32) == 4, "Invalid 'Int32' size.");

	typedef ::uint32_t	 UInt32;
	UInt32 static const	 UInt32Min	= 0;
	UInt32 static const	 UInt32Max	= UINT32_MAX;
	static_assert(sizeof(UInt32) == 4, "Invalid 'UInt32' size.");

	typedef ::int64_t	 Int64;
	Int64 static const	 Int64Min	= INT64_MIN;
	Int64 static const	 Int64Max	= INT64_MAX;
	static_assert(sizeof(Int64) == 8, "Invalid 'Int32' size.");

	typedef ::uint64_t	 UInt64;
	UInt64 static const	 UInt64Min	= 0;
	UInt64 static const	 UInt64Max	= UINT64_MAX;
	static_assert(sizeof(UInt64) == 8, "Invalid 'UInt64' size.");

	typedef UInt8		 Float8[1];		// Reserved for future use. Interaction with shaders and etc.
	typedef UInt8		 Float16[2];

	typedef float		 Float32;
	Float32 static const Float32Min = FLT_MIN;
	Float32 static const Float32Max = FLT_MAX;
	static_assert(sizeof(Float32) == 4, "Invalid 'Float32' size.");

	typedef double		 Float64;
	Float64 static const Float64Min = DBL_MIN;
	Float64 static const Float64Max = DBL_MAX;
	static_assert(sizeof(Float64) == 8, "Invalid 'Float64' size.");

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                          Platform dependent types.                               ******
	// ******                    (Size may vary on different platforms)                        ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	GD_DEPRECATED("Please, use 'SizeTp' instead of 'size_t'")		
	typedef ::std::size_t size_t;
	
	GD_DEPRECATED("Please, use 'PtrDiffTp' instead of 'ptrdiff_t'") 
	typedef ::std::ptrdiff_t ptrdiff_t;

#if GD_ARCHITECTURE_X86 || GD_ARCHITECTURE_ARM32

	static_assert(sizeof(void*) == 4, "Invalidly selected 32-bit platform.");

	typedef UInt32		 UIntPtr;
	UIntPtr static const UIntPtrMin = UInt32Min;
	UIntPtr static const UIntPtrMax = UInt32Max;

	typedef Int32		 IntPtr;
	IntPtr static const	 IntPtrMin = Int32Min;
	IntPtr static const	 IntPtrMax = Int32Max;

	typedef UInt32		 SizeTp;
	SizeTp static const	 SizeTpMin = UInt32Min;
	SizeTp static const	 SizeTpMax = UInt32Max;

	typedef Int32			PtrDiffTp;
	PtrDiffTp static const	PtrDiffTpMin = Int32Min;
	PtrDiffTp static const	PtrDiffTpMax = Int32Max;

#endif	// if GD_ARCHITECTURE_X86 || GD_ARCHITECTURE_ARM32

#if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64

	static_assert(sizeof(void*) == 8, "Invalidely selected 64-bit platform.");

	typedef UInt64		 UIntPtr;
	UIntPtr static const UIntPtrMin	= UInt64Min;
	UIntPtr static const UIntPtrMax	= UInt64Max;

	typedef Int64		 IntPtr;
	IntPtr static const	 IntPtrMin	= Int64Min;
	IntPtr static const	 IntPtrMax	= Int64Max;

	typedef UInt64		 SizeTp;
	SizeTp static const	 SizeTpMin	= UInt64Min;
	SizeTp static const	 SizeTpMax	= UInt64Max;

	typedef Int64			PtrDiffTp;
	PtrDiffTp static const	PtrDiffTpMin = Int64Min;
	PtrDiffTp static const	PtrDiffTpMax = Int64Max;

#endif	// if GD_ARCHITECTURE_X64 || GD_ARCHITECTURE_ARM64

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                            Deleted functionality.                                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*!
	 * Template interface that disables any constructor or copy assignment operators in all 
	 * child classes.
	 */
	class TNonCreatable
	{
	public:
		GDINT TNonCreatable() = delete;
		GDINT TNonCreatable(TNonCreatable const& other) = delete;
		GDINT TNonCreatable& operator= (TNonCreatable const& other) = delete;
	};	// class TNonCreatable

	/*!
	 * Template interface that disables copy constructor and copy assignment operators in all 
	 * child classes.
	 */
	class TNonCopyable
	{
	public:
		GDINT TNonCopyable() = default;
		GDINT TNonCopyable(TNonCopyable const& other) = delete;
		GDINT TNonCopyable& operator= (TNonCopyable const& other) = delete;
	};	// class NonCopyable

	/*!
	 * Template that disables move constructor and move assignment operators in all
	 * child classes.
	 */
	class TNonMovable
	{
	public:
		GDINT TNonMovable() = default;
		GDINT TNonMovable(TNonMovable&& other) = delete;
		GDINT TNonMovable& operator= (TNonMovable&& other) = delete;
	};	// class TNonMovable

	/*!
	 * Interface that disables assignment operators in all child classes.
	 */
	class TNonAssignable
	{
	public:
		GDINT TNonAssignable() = default;
		GDINT TNonAssignable& operator= (TNonAssignable const& other) = delete;
		GDINT TNonAssignable& operator= (TNonAssignable && other) = delete;
	};	// class TNonAssignable

	/*!
	 * An non-copyable class with a virtual constructor.
	 */
	struct IVirtuallyDestructible : public TNonCopyable
	{
	public:
		GDINL virtual ~IVirtuallyDestructible() = default;
	};	// struct IVirtuallyDestructible

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                      Array size counting safe macros.                            ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	template <typename TType, SizeTp TLength>
	GDINT Byte (&GetArrayLengthHelper(TType const(&array)[TLength]))[TLength] = delete;

	/*!
	 * Returns size of the specified array.
	 */
	template <typename TType, SizeTp TLength>
	GDINL SizeTp GetLength(TType const(&array)[TLength])
	{
		GD_NOT_USED(array);
		return TLength;
	}

GD_NAMESPACE_END
