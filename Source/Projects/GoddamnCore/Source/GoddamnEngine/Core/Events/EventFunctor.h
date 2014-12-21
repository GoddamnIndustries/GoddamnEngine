#ifndef GD_CORE_EVENTS_FUNCTOR
#define GD_CORE_EVENTS_FUNCTOR

#include <GoddamnEngine/Include.h>

#define GD_FUNCTOR_DEFINE()						\
private:										\
	const Function function;					\
public:											\
	GDINL Functor(const Function function) :	\
		function(function)						\
	{											\
	}											\
	GDINL Functor(const Functor& functor) :		\
		function(functor.function)				\
	{											\
	}


GD_NAMESPACE_BEGIN

	/// @brief		Namespace contains wrapper for pointers on member-functions
	namespace Functor
	{
		struct Empty { };

		template<class ObjectType,
			class ReturnType = Empty, 
			class FirstArgType = Empty, 
			class SecondArgType = Empty>
		struct Functor;

		/// @brief		Functor class for functions with no arguments
		template<class ObjectType, class ReturnType, class FirstArgType, class SecondArgType>
		struct Functor<ObjectType, ReturnType(), FirstArgType, SecondArgType>
		{
			typedef ReturnType (ObjectType::* Function)();
			GD_FUNCTOR_DEFINE();

			GDINL ReturnType operator() (ObjectType* object) const
			{
				(object->*this->function)();
			}
		};

		/// @brief		Functor class for functions with one argument
		template<class ObjectType, class ReturnType, class FirstArgType, class SecondArgType>
		struct Functor<ObjectType, ReturnType(FirstArgType), SecondArgType>
		{
			typedef ReturnType (ObjectType::* Function)(FirstArgType);
			GD_FUNCTOR_DEFINE();

			GDINL ReturnType operator() (ObjectType* object, FirstArgType firstArg) const
			{
				(object->*this->function)(firstArg);
			}
		};

		/// @brief		Functor class for functions with two arguments
		template<class ObjectType, class ReturnType, class FirstArgType, class SecondArgType>
		struct Functor<ObjectType, ReturnType(FirstArgType, SecondArgType)>
		{
			typedef ReturnType (ObjectType::* Function)(FirstArgType, SecondArgType);
			GD_FUNCTOR_DEFINE();

			GDINL ReturnType operator() (ObjectType* object, FirstArgType firstArg, SecondArgType secondArg) const
			{
				(object->*this->function)(firstArg, secondArg);
			}
		};
	}

GD_NAMESPACE_END

#endif