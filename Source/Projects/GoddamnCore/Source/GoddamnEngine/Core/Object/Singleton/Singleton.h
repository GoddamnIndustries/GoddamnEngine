#ifndef GD_CORE_OBJECT_SINGLETON
#define GD_CORE_OBJECT_SINGLETON

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>

#define GD_SINGLETON_IMPLEMENTATION(SingletonType)										\
	GDAPI extern SingletonType* __Singleton_##SingletonType##_Instance = nullptr;		\
	template<> Singleton<SingletonType>::Singleton()									\
	{																					\
		GD_ASSERT((__Singleton_##SingletonType##_Instance == nullptr),					\
			"'Singleton<SingletonType>' error: singleton instance already exists.");	\
		__Singleton_##SingletonType##_Instance = ((SingletonType*)this);				\
	}																					\
	template<> SingletonType& Singleton<SingletonType>::GetInstance()					\
	{																					\
		GD_ASSERT((__Singleton_##SingletonType##_Instance != nullptr),					\
			"'Singleton<SingletonType>::GetInstance' error: no singleton instance "		\
			"was created");																\
		return (*__Singleton_##SingletonType##_Instance);								\
	}

#define GD_SINGLETON_OVERRIDE(SingletonType, SingletonBaseType)							\
public:																					\
	GDINL static SingletonType& GetInstance(											\
	) { return object_cast<SingletonType&>(SingletonBaseType::GetInstance()); }			\
private:
GD_NAMESPACE_BEGIN

	/// Singleton class descendants are objects, for which only only one instance required. Than are alternatives to classes
	///	with static functions and members. Project coding standards do not recommends using static members, so singletons are 
	///	located in code (somewhere above 'main' function).
	///			
	///	Common usage:
	///	@code
	///		// Somewhere in header file:
	///		class MySingletonClass : public BaseClass,
	///								 public Singleton<MySingletonClass>
	///		{ ... };
	/// 
	///		// Somewhere is source file:
	///		GD_SINGLETON_IMPLEMENTATION(MySingletonClass);
	///			
	///		// Usage:
	///		MySingletonClass::GetInstance().DoSomething(a, b + c, &d);
	///	@endcode
	template<typename SingletonType>
	class Singleton
	{	
	private:
		static SingletonType* Instance;

	protected:
		/// Constructs Singleton's instance
		GDAPI  Singleton();// { Singleton<SingletonType>::Instance = static_cast<SingletonType*>(this); }
		//GDAPI ~Singleton();// { Singleton<SingletonType>::Instance = nullptr; }

	public:
		/// Returns instance of the singleton
		GDAPI static SingletonType& GetInstance();/*
		{
			GD_ASSERT((Singleton<SingletonType>::Instance != nullptr), "No instance of this singleton was created.");
			return   (*Singleton<SingletonType>::Instance);
		}*/
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Object/Singleton/Singleton.inl>

#endif
