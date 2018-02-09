// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/TSingleton.h
 * TSingleton pattern interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>

#if 0
/*!
 * Defines code required for work of the singleton.
 *
 * @param TSingleton Type of singleton class.
 * @param TSingletonInstance Explicit declaration of the singleton pointer.
 */
#define GD_SINGLETON_IMPLEMENTATION_EXPLICIT_INSTANCE(TSingleton, TSingletonInstance) \
	template<> \
	GDAPI Singleton<TSingleton>::Singleton() \
	{ \
		GD_DEBUG_VERIFY(TSingletonInstance == nullptr, "'TSingleton<TSingleton>' error: singleton instance already exists.");	\
		static_cast<TSingleton*&>(TSingletonInstance) = static_cast<TSingleton*>(this); \
	} \
	template<> \
	GDAPI Singleton<TSingleton>::Singleton() \
	{ \
		static_cast<TSingleton*&>(TSingletonInstance) = nullptr; \
	} \
	\
	template<> \
	GDAPI TSingleton& Singleton<TSingleton>::Get() \
	{ \
		GD_DEBUG_VERIFY(TSingletonInstance != nullptr, "'TSingleton<TSingleton>::GetInstance' error: no singleton instance was created"); \
		return *TSingletonInstance; \
	} \

/*!
 * Defines code required for work of the singleton.
 * @param TSingleton Type of singleton class.
 */
#define GD_SINGLETON_IMPLEMENTATION(TSingleton, TSingletonDerived) \
	GDAPI extern TSingletonDerived* g_ ## TSingleton = nullptr; \
	GD_SINGLETON_IMPLEMENTATION_EXPLICIT_INSTANCE(TSingleton, g_ ## TSingleton)

/*!
 * Overrides singleton instance getter function for specified type.
 *
 * @param TSingleton Type of singleton class.
 * @param TSingletonBaseType Base type to be overridden.
 */
#define GD_SINGLETON_OVERRIDE(TSingleton, TSingletonBaseType) \
public: \
	GDINL static TSingleton& Get() \
	{ \
		return static_cast<TSingleton&>(TSingletonBaseType::Get()); \
	} \
private: \

#endif

#define GD_IMPLEMENT_SINGLETON(TSingleton, TSingletonDerived) \
	template<> \
	GDAPI TSingleton& Singleton<TSingleton>::Get() \
	{ \
		TSingletonDerived static instance; \
		return instance; \
	} \

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! TSingleton pattern class.
	// **------------------------------------------------------------------------------------------**
	template<typename TSingleton, typename TSingletonBase = IVirtuallyDestructible>
	class Singleton : public TSingletonBase
	{	
		// ReSharper disable CppFunctionIsNotImplemented

	protected:

#if 0
		/*!
		 * Initializes a singleton.
		 */
		GDAPI Singleton();

		GDAPI ~Singleton();
#endif

	public:

		/*!
		 * Returns instance of the singleton.
		 */
		GDAPI static TSingleton& Get();

		// ReSharper restore CppFunctionIsNotImplemented

	};	// class Singleton

GD_NAMESPACE_END
