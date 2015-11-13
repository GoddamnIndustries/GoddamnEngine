// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Singleton.h
//! Singleton pattern interface.

#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>

// ------------------------------------------------------------------------------------------
//! Defines some code required for work of the singleton.
//! @param SingletonType Type of singleton class.
//! @param singletonInstance Explicit declaration of the singleton pointer.
#define GD_SINGLETON_IMPLEMENTATION_EXPLICIT_INSTANCE(SingletonType, singletonInstance) \
	/** Initializes a singleton. */\
	template<> \
	GD::Singleton<SingletonType>::Singleton() \
	{ \
		GD_ASSERT(singletonInstance == nullptr, "'Singleton<SingletonType>' error: singleton instance already exists.");	\
		const_cast<SingletonType*&>(singletonInstance) = static_cast<SingletonType*>(this); \
	} \
	\
	/** Returns instance of the singleton. */ \
	/** @returns instance of the singleton. */ \
	template<> \
	GD::SingletonType& Singleton<SingletonType>::GetInstance() \
	{ \
		GD_ASSERT(singletonInstance != nullptr, "'Singleton<SingletonType>::GetInstance' error: no singleton instance was created"); \
		return *singletonInstance; \
	} \

// ------------------------------------------------------------------------------------------
//! Defines some code required for work of the singleton.
//! @param SingletonType Type of singleton class.
//! @param SingletonInstance Explicit declaration of the singleton pointer.
#define GD_SINGLETON_IMPLEMENTATION(SingletonType) \
	\
	/** instance of the singleton. */\
	GDAPI extern SingletonType* __singleton##SingletonType##instance = nullptr; \
	\
	GD_SINGLETON_IMPLEMENTATION_EXPLICIT_INSTANCE(SingletonType, __singleton##SingletonType##instance)

// ------------------------------------------------------------------------------------------
//! Overrides singleton instance getter function for specified type.
//! @param SingletonType Required type to be used.
//! @param SingletonBaseType Base type to be overridden.
#define GD_SINGLETON_OVERRIDE(SingletonType, SingletonBaseType) \
public: \
	/** Returns instance of the singleton. */ \
	/** @returns instance of the singleton. */ \
	GDINL static SingletonType& GetInstance() \
	{ \
		return object_cast<SingletonType&>(SingletonBaseType::GetInstance()); \
	} \
private: \

GD_NAMESPACE_BEGIN

	// ------------------------------------------------------------------------------------------
	//! Singleton class descendants are objects, for which only only one instance required. Than are alternatives to classes
	//! with static functions and members. Project coding standards do not recommends using static members, so singletons are 
	//! located in code (somewhere above 'main' function).
	template<typename SingletonType>
	class Singleton : public IUncopiable
	{	
	private:
		static SingletonType* instance;

	protected:

		// ------------------------------------------------------------------------------------------
		//! Initializes a singleton.
		GDAPI  Singleton();

	public:

		// ------------------------------------------------------------------------------------------
		//! Returns instance of the singleton.
		//! @returns instance of the singleton.
		GDAPI static SingletonType& GetInstance();

	};	// class Singleton

GD_NAMESPACE_END
