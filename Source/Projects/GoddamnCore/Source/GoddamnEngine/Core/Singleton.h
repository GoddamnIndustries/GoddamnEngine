// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/Singleton.h
//! Singleton pattern interface.

#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

//! Defines some code required for work of the singleton.
//! @param SingletonType Type of singleton class.
#define GD_SINGLETON_IMPLEMENTATION(SingletonType) \
	\
	/** Instance of the singleton. */\
	GDAPI extern SingletonType* __Singleton##SingletonType##Instance = nullptr; \
	\
	/** Initializes a singleton. */\
	template<> \
	GD::Singleton<SingletonType>::Singleton() \
	{ \
		GD_ASSERT(__Singleton##SingletonType##Instance == nullptr, "'Singleton<SingletonType>' error: singleton instance already exists.");	\
		__Singleton##SingletonType##Instance = reinterpret_cast<SingletonType*>(this); \
	} \
	\
	/** Returns instance of the singleton. */ \
	/** @returns Instance of the singleton. */ \
	template<> \
	GD::SingletonType& Singleton<SingletonType>::GetInstance() \
	{ \
		GD_ASSERT(__Singleton##SingletonType##Instance != nullptr, "'Singleton<SingletonType>::GetInstance' error: no singleton instance was created"); \
		return *__Singleton##SingletonType##Instance; \
	} \

//! Overrides singleton instance getter function for specified type.
//! @param SingletonType Required type to be used.
//! @param SingletonBaseType Base type to be overridden.
#define GD_SINGLETON_OVERRIDE(SingletonType, SingletonBaseType) \
public: \
	/** Returns instance of the singleton. */ \
	/** @returns Instance of the singleton. */ \
	GDINL static SingletonType& GetInstance() \
	{ \
		return object_cast<SingletonType&>(SingletonBaseType::GetInstance()); \
	} \
private: \

GD_NAMESPACE_BEGIN

	//! Singleton class descendants are objects, for which only only one instance required. Than are alternatives to classes
	///	       with static functions and members. Project coding standards do not recommends using static members, so singletons are 
	///		   located in code (somewhere above 'main' function). Common usage:
	///	@code
	///		// Somewhere in header file:
	///		class MySingletonClass : public BaseClass, public Singleton<MySingletonClass>
	///		{ 
	///			... 
	///		};	// class MySingletonClass
	//! 
	///		// Somewhere is source file:
	///		GD_SINGLETON_IMPLEMENTATION(MySingletonClass);
	///			
	///		// Usage:
	///		MySingletonClass::GetInstance().DoSomething(a, b + c, &d);
	///	@endcode
	//! @tparam SingletonType Type that should be singleton.
	template<typename SingletonType>
	class Singleton
	{	
	private:
		static SingletonType* Instance;

	protected:
		//! Initializes a singleton.
		GDAPI  Singleton();

	public:

		//! Returns instance of the singleton.
		//! @returns Instance of the singleton.
		GDAPI static SingletonType& GetInstance();
	};	// class Singleton

GD_NAMESPACE_END
