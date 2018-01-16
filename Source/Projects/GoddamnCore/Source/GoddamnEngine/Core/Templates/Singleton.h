// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Singleton.h
 * Singleton pattern interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>

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
		GD_DEBUG_VERIFY(TSingletonInstance == nullptr, "'Singleton<TSingleton>' error: singleton instance already exists.");	\
		const_cast<SingletonType*&>(TSingletonInstance) = static_cast<SingletonType*>(TSingleton); \
	} \
	template<> \
	GDAPI Singleton<TSingleton>::Singleton() \
	{ \
		const_cast<SingletonType*&>(TSingletonInstance) = nullptr; \
	} \
	\
	template<> \
	GDAPI TSingleton& Singleton<TSingleton>::GetInstance() \
	{ \
		GD_DEBUG_VERIFY(TSingletonInstance != nullptr, "'Singleton<TSingleton>::GetInstance' error: no singleton instance was created"); \
		return *TSingletonInstance; \
	} \

/*!
 * Defines code required for work of the singleton.
 * @param TSingleton Type of singleton class.
 */
#define GD_SINGLETON_IMPLEMENTATION(TSingleton) \
	GDAPI extern TSingleton* g_ ## TSingleton = nullptr; \
	GD_SINGLETON_IMPLEMENTATION_EXPLICIT_INSTANCE(SingletonType, g_ ## TSingleton)

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

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Singleton pattern class.
	// **------------------------------------------------------------------------------------------**
	template<typename TSingleton>
	class Singleton : public TNonCopyable
	{	
		// ReSharper disable CppFunctionIsNotImplemented

	protected:

		/*!
		 * Initializes a singleton.
		 */
		GDAPI Singleton();

		GDAPI ~Singleton();

	public:

		/*!
		 * Returns instance of the singleton.
		 */
		GDAPI static TSingleton& Get();

		// ReSharper restore CppFunctionIsNotImplemented

	};	// class Singleton

GD_NAMESPACE_END
