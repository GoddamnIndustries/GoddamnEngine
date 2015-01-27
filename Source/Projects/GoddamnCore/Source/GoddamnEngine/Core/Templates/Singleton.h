/// ==========================================================================================
/// Singleton.h: Singleton pattern interface.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_TEMPLATES_SINGLETON
#define GD_TEMPLATES_SINGLETON

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

/// @brief Defines some code required for work of the singleton.
/// @param SingletonType Type of singleton class.
#define GD_SINGLETON_IMPLEMENTATION(SingletonType) \
	\
	/** @brief Instance of the singleton. */\
	GDAPI extern SingletonType* __Singleton##SingletonType##Instance = nullptr; \
	\
	/** @brief Initializes a singleton. */\
	template<> \
	GD::Singleton<SingletonType>::Singleton() \
	{ \
		GD_ASSERT(__Singleton##SingletonType##Instance == nullptr, "'Singleton<SingletonType>' error: singleton instance already exists.");	\
		__Singleton##SingletonType##Instance = reinterpret_cast<SingletonType*>(this); \
	} \
	\
	/** @brief Returns instance of the singleton. */ \
	/** @returns Instance of the singleton. */ \
	template<> \
	GD::SingletonType& Singleton<SingletonType>::GetInstance() \
	{ \
		GD_ASSERT(__Singleton##SingletonType##Instance != nullptr, "'Singleton<SingletonType>::GetInstance' error: no singleton instance was created"); \
		return *__Singleton##SingletonType##Instance; \
	} \

/// @brief Overrides singleton instance getter function for specified type.
/// @param SingletonType Required type to be used.
/// @param SingletonBaseType Base type to be overridden.
#define GD_SINGLETON_OVERRIDE(SingletonType, SingletonBaseType) \
public: \
	/** @brief Returns instance of the singleton. */ \
	/** @returns Instance of the singleton. */ \
	GDINL static SingletonType& GetInstance() \
	{ \
		return object_cast<SingletonType&>(SingletonBaseType::GetInstance()); \
	} \
private: \

GD_NAMESPACE_BEGIN

	/// @brief Singleton class descendants are objects, for which only only one instance required. Than are alternatives to classes
	///	       with static functions and members. Project coding standards do not recommends using static members, so singletons are 
	///		   located in code (somewhere above 'main' function). Common usage:
	///	@code
	///		// Somewhere in header file:
	///		class MySingletonClass : public BaseClass, public Singleton<MySingletonClass>
	///		{ 
	///			... 
	///		};	// class MySingletonClass
	/// 
	///		// Somewhere is source file:
	///		GD_SINGLETON_IMPLEMENTATION(MySingletonClass);
	///			
	///		// Usage:
	///		MySingletonClass::GetInstance().DoSomething(a, b + c, &d);
	///	@endcode
	/// @tparam SingletonType Type that should be singleton.
	template<typename SingletonType>
	class Singleton
	{	
	private:
		static SingletonType* Instance;

	protected:
		/// Initializes a singleton.
		GDAPI  Singleton();

	public:

		/// @brief Returns instance of the singleton.
		/// @returns Instance of the singleton.
		GDAPI static SingletonType& GetInstance();
	};	// class Singleton

GD_NAMESPACE_END

#endif	// ifndef GD_TEMPLATES_SINGLETON
