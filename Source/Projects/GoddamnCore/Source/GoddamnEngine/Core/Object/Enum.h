// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Enum.h
 * File contains base declaration for enums in the entities system.
 */
#pragma once

 // ReSharper disable CppUnusedIncludeDirective
#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/VaryingArgsMacro.h>
#include <GoddamnEngine/Core/Object/Base.h>
 // ReSharper restore CppUnusedIncludeDirective

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Implements zero-overhead reflection for enumerations.
	// **------------------------------------------------------------------------------------------**
	template<typename TEnum>
	GD_OBJECT_GENERATED class EnumClass final : public TNonCreatable
	{
	public:

		

	};	// class EnumClass

	/*! 
	 * A const-expression class that is used to strip away value of the enum constants (if declared). 
	 * Do not use manually.
	 */
	template<typename TEnum>
	class EnumValue final
	{
	private:
		TEnum m_Value;

	public:
		GDINL explicit constexpr EnumValue(TEnum const value) 
			: m_Value(value)
		{
		}

	public:
		// enum_value =  ...
		template<typename TAny>
		GDINL constexpr EnumValue const& operator= (TAny const& any) const
		{
			GD_NOT_USED(any);
			return *this;
		}

		// (T)enum_value
		GDINL constexpr operator TEnum() const
		{
			return m_Value;
		}

	};	// class EnumValue final

	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                Very scary macros for generating boilerplate code.                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	 // ------------------------------------------------------------------------------------------
	 // High-level macros.
	 // ------------------------------------------------------------------------------------------

	/*! 
	 * A high-level macro, that extract names from enum declaration. 
	 * Can be used for both enums and enum classes. 
	 */
	#define GD_DECLARE_ENUM_IMPL_EXTRACT_NAMES_(TEnum, TIndex, TValue) #TValue,
	#define GD_DECLARE_ENUM_IMPL_EXTRACT_NAMES(TEnum, ...) \
			{ \
				GD_PASS(GD_VA_ARGS_ITER(GD_DECLARE_ENUM_IMPL_EXTRACT_NAMES_, TEnum, __VA_ARGS__)) \
			}

	/*! Generates a case in 'CStr ToString(TEnum)' function for managed enumerations. */
	#define GD_DECLARE_ENUM_IMPL_TO_STRING_(TEnum, TIndex, TValue) \
			case static_cast<TEnum>((EnumValue<TEnum>)TEnum::TValue): return #TValue;
	
	#define GD_DECLARE_ENUM_CLASS(TEnum, TUnderlying, ...) GD_OBJECT_KERNEL \
		extern "C++" \
		{ \
			enum class TEnum : TUnderlying \
			{ \
				__VA_ARGS__\
			}; \
			template<> \
			GD_OBJECT_KERNEL class GD::EnumClass<TEnum> final : public TNonCreatable \
			{ \
			private: \
				GDINL GD_OBJECT_GENERATED static CStr const* GetNames() \
				{ \
				\
				\
				\
				} \
				\
			public: \
				GDINL GD_OBJECT_GENERATED static CStr ToString(TEnum const e) \
				{ \
					switch(e) \
					{ \
						GD_PASS(GD_VA_ARGS_ITER(GD_DECLARE_ENUM_IMPL_TO_STRING_, TEnum, __VA_ARGS__)) \
						default: return nullptr; \
					} \
				} \
			}; \
		}

GD_NAMESPACE_END
