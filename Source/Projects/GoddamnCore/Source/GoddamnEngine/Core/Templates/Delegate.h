//! ==========================================================================================
//! Delegate.h - function delegate class.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_TEMPLATES_DELEGATE
#define GD_CORE_TEMPLATES_DELEGATE

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Tuple.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>

GD_NAMESPACE_BEGIN

	//! @brief Base class for all invokable objects.
	struct IDelegate
	{
	protected:
		GDINL IDelegate() {}
		GDINL virtual ~IDelegate() {}

	public:

		//! @brief Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
		//! @param Instance Instance object we are operating on.
		//! @param ReturnValueOutputPtr Pointer to the methods execution result. Null pointer may be specified for voids.
		GDAPI virtual void Invoke(Handle const Instance, Handle const ReturnValueOutputPtr, ...) const abstract;
	};	// struct IDelegate

	template<typename DelegateSignatureType> 
	struct Delegate;

	//! @brief Basic template wrapper on delegate pointers. Class is same to @c std::function class with some differences (for member functions).
	//!        When specifying member function it is not required to pass instance type as first params arguments. 
	//!        Invocation operator takes pointer to instance as it`s first argument (Handle type, it is better to make a const_cast for constant objects).
	//!        For pointers to to static functions instance param can take any value (nullptr is preferred).
	template<typename ReturnType, typename... ArgumentTypes>
	struct Delegate<ReturnType(ArgumentTypes...)> : public IDelegate
	{
	private:		
		//! @brief A delegate traits base class.
		struct IDelegateTraitsBase : public IUncopiable
		{
		protected:

			//! @brief Initializes a new delegate traits.
			GDINL IDelegateTraitsBase() { }
			GDINL virtual ~IDelegateTraitsBase() { }

		public:

			//! @brief Clones this object.
			//! @returns Clone of this object.
			GDINT virtual IDelegateTraitsBase const* Clone() const abstract;

			//! @brief Executes the given operation on a different thread, and waits for the result.
			//! @param Instance Object we are operating on.
			//! @param Arguments List of the delegate arguments.
			//! @return	A result of the delegate execution.
			GDINT virtual ReturnType Invoke(Handle const Instance, ArgumentTypes&&... Arguments) const abstract;
		};	// struct IDelegateTraitsBase

		template<typename DelegateSignatureType> 
		struct DelegateTraits;

		//! @brief Traits for STATIC DELEGATE.
		template<>
		struct DelegateTraits<ReturnType(*)(ArgumentTypes...)> final : public IDelegateTraitsBase
		{
		private:
			typedef ReturnType(*DelegateType)(ArgumentTypes...);
			DelegateType const DelegatePtr = nullptr;

		public:
			GDINL DelegateTraits(DelegateType const DelegatePtr) 
				: DelegatePtr(DelegatePtr) 
			{
			}

			GDINL virtual IDelegateTraitsBase const* Clone() const override final
			{ 
				return new DelegateTraits<ReturnType(*)(ArgumentTypes...)>(this->DelegatePtr); 
			}

			GDINL virtual ReturnType Invoke(Handle const Instance, ArgumentTypes&&... Arguments) const override final
			{
				GD_NOT_USED(Instance); // Since no instance is required to be used while invocation of static function
				return (*this->DelegatePtr)(Forward<ArgumentTypes>(Arguments)...);
			}
		};	// struct DelegateTraits<ReturnType(*)(ArgumentTypes...)>

		//! @brief Traits for MUTABLE MEMBER DELEGATE.
		template<typename ClassType>
		struct DelegateTraits<ReturnType(ClassType::*)(ArgumentTypes...)> final : public IDelegateTraitsBase
		{
		private:
			typedef ReturnType(ClassType::* DelegateType)(ArgumentTypes...);
			DelegateType const DelegatePtr = nullptr;

		public:
			GDINL DelegateTraits(DelegateType const DelegatePtr) 
				: DelegatePtr(DelegatePtr)
			{
			}

			GDINL virtual IDelegateTraitsBase const* Clone() const override final
			{ 
				return new DelegateTraits<ReturnType(ClassType::*)(ArgumentTypes...)>(this->DelegatePtr); 
			}

			GDINL virtual ReturnType Invoke(Handle const Instance, ArgumentTypes&&... Arguments) const override final
			{
				ClassType* const ClassInstance = static_cast<ClassType*>(Instance);
				return (ClassInstance->*this->DelegatePtr)(Instance, Forward<ArgumentTypes>(Arguments)...);
			}
		};	// struct DelegateTraits<ReturnType(ClassType::*)(ArgumentTypes...)>

		//! @brief Traits for CONSTANT MEMBER DELEGATE.
		template<typename ClassType>
		struct DelegateTraits<ReturnType(ClassType::*)(ArgumentTypes...) const> final : public IDelegateTraitsBase
		{
		private:
			typedef ReturnType(ClassType::* DelegateType)(ArgumentTypes...) const;
			DelegateType const DelegatePtr = nullptr;

		public:
			GDINL DelegateTraits(DelegateType const DelegatePtr) 
				: DelegatePtr(DelegatePtr)
			{
			}

			GDINL virtual IDelegateTraitsBase const* Clone() const override final
			{ 
				return new DelegateTraits<ReturnType(ClassType::*)(ArgumentTypes...) const>(this->DelegatePtr); 
			}

			GDINL virtual ReturnType Invoke(Handle const Instance, ArgumentTypes&&... Arguments) const override final
			{
				ClassType const* const ClassInstance = static_cast<ClassType const*>(Instance);
				return (ClassInstance->*this->DelegatePtr)(Forward<ArgumentTypes>(Arguments)...);
			}
		};	// struct DelegateTraits<ReturnType(ClassType::*)(ArgumentTypes...) const>

	private:
		UniquePtr<IDelegateTraitsBase> DelegateTraitsPtr;

	public:
		//! @todo Rewrite this stuff without using any STL code.
		template<SizeTp const Index> 
		using ArgumentType = typename TupleElement<Index, ArgumentTypes...>::Type;
		
		//! @brief Initializes a delegate.
		//! @param Delegate Pointer to delegate function. 
		template<typename DelegateSignature>
		GDINL Delegate(DelegateSignature const& DelegatePtr)
			: DelegateTraitsPtr(new DelegateTraits<DelegateSignature>(DelegatePtr))
		{
		}

		//! @brief Moves other delegate.
		//! @param Other Other delegate to move.
		GDINL Delegate(Delegate&& Other)
			: DelegateTraitsPtr(Move(Other.DelegateTraitsPtr))
		{
		}

		//! @brief Copies other delegate.
		//! @param Other Other delegate to copy.
		GDINL Delegate(Delegate const& Other)
			: DelegateTraitsPtr(Other.DelegateTraitsPtr->Clone())
		{
		}

		//! @brief Invokes the delegate.
		//! @param Instance Object to invoke delegate on. May be null for static function.
		//! @param ArgumentsList Varied arguments list pointer.
		//! @returns Invocation result.
		//! @{
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 0, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			GD_NOT_USED(ArgumentsList);
			return (*this)(Instance
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 1, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 2, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 3, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 4, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 5, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			auto Arg4 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<4>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				, Forward<ArgumentType<4>>(*Arg4)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 6, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			auto Arg4 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<4>>::Type*);
			auto Arg5 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<5>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				, Forward<ArgumentType<4>>(*Arg4)
				, Forward<ArgumentType<5>>(*Arg5)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 7, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			auto Arg4 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<4>>::Type*);
			auto Arg5 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<5>>::Type*);
			auto Arg6 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<6>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				, Forward<ArgumentType<4>>(*Arg4)
				, Forward<ArgumentType<5>>(*Arg5)
				, Forward<ArgumentType<6>>(*Arg6)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 8, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			auto Arg4 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<4>>::Type*);
			auto Arg5 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<5>>::Type*);
			auto Arg6 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<6>>::Type*);
			auto Arg7 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<7>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				, Forward<ArgumentType<4>>(*Arg4)
				, Forward<ArgumentType<5>>(*Arg5)
				, Forward<ArgumentType<6>>(*Arg6)
				, Forward<ArgumentType<7>>(*Arg7)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 9, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			auto Arg4 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<4>>::Type*);
			auto Arg5 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<5>>::Type*);
			auto Arg6 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<6>>::Type*);
			auto Arg7 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<7>>::Type*);
			auto Arg8 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<8>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				, Forward<ArgumentType<4>>(*Arg4)
				, Forward<ArgumentType<5>>(*Arg5)
				, Forward<ArgumentType<6>>(*Arg6)
				, Forward<ArgumentType<7>>(*Arg7)
				, Forward<ArgumentType<8>>(*Arg8)
				);
		}
		template<SizeTp const ArgumentsCount = sizeof...(ArgumentTypes)>
		GDINL typename EnableIf<ArgumentsCount == 10, ReturnType>::Type InvokeVa(Handle const Instance, va_list ArgumentsList) const
		{
			auto Arg0 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<0>>::Type*);
			auto Arg1 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<1>>::Type*);
			auto Arg2 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<2>>::Type*);
			auto Arg3 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<3>>::Type*);
			auto Arg4 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<4>>::Type*);
			auto Arg5 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<5>>::Type*);
			auto Arg6 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<6>>::Type*);
			auto Arg7 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<7>>::Type*);
			auto Arg8 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<8>>::Type*);
			auto Arg9 = va_arg(ArgumentsList, typename TypeTraits::RemoveReference<ArgumentType<9>>::Type*);
			return (*this)(Instance
				, Forward<ArgumentType<0>>(*Arg0)
				, Forward<ArgumentType<1>>(*Arg1)
				, Forward<ArgumentType<2>>(*Arg2)
				, Forward<ArgumentType<3>>(*Arg3)
				, Forward<ArgumentType<4>>(*Arg4)
				, Forward<ArgumentType<5>>(*Arg5)
				, Forward<ArgumentType<6>>(*Arg6)
				, Forward<ArgumentType<7>>(*Arg7)
				, Forward<ArgumentType<8>>(*Arg8)
				, Forward<ArgumentType<9>>(*Arg9)
				);
		}
		static_assert(sizeof...(ArgumentTypes) < 10, "Arguments count is more than maximum implemented.");
		//! @}

		//! @brief Invokes delegate and returns it`s return value. 
		//!        If this is a member function then owner object should be passed first into varied function arguments.
		//!        All parameters that are passed by pointers on parameters instances.
		//! @param Instance Object to invoke delegate on. May be null for static function.
		//! @param ReturnValueOutputPtr Pointer to function execution result.
		//! @param ArgumentsList Varied arguments list pointer.
		//! @{
		template<typename TheReturnType = ReturnType>
		GDINL typename EnableIf<TypeTraits::IsSame<TheReturnType, void>::Value>::Type InvokeVa(Handle const Instance, Handle const ReturnValueOutputPtr, va_list ArgumentsList) const
		{
			GD_NOT_USED(ReturnValueOutputPtr);
			this->InvokeVa(Instance, ArgumentsList);
			return {};
		}
		template<typename TheReturnType = ReturnType>
		GDINL typename DisableIf<TypeTraits::IsSame<TheReturnType, void>::Value>::Type InvokeVa(Handle const Instance, Handle const ReturnValueOutputPtr, va_list ArgumentsList) const
		{
			*reinterpret_cast<ReturnType*>(ReturnValueOutputPtr) = Forward<ReturnType>(this->InvokeVa(Instance, ArgumentsList));
			return {};
		}

		//! @}

		//! @brief Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
		//! @param Instance Object to invoke delegate on. May be null for static function.
		//! @param ... Invocation arguments.
		GDINL virtual void Invoke(Handle const Instance, Handle const ReturnValueOutputPtr, ...) const override final
		{
			va_list ArgumentsList;
			va_start(ArgumentsList, ReturnValueOutputPtr);
			this->InvokeVa(Instance, ReturnValueOutputPtr, ArgumentsList);
			va_end(ArgumentsList);
		}

	public:

		//! @brief Moves other delegate here.
		//! @param Other Other delegate.
		//! @returns Self.
		GDINL Delegate& operator= (Delegate&& Other)
		{
			this->DelegateTraitsPtr = Move(Other.DelegateTraitsPtr);
			return *this;
		}

		//! @brief Copies other delegate here.
		//! @param Other Other delegate.
		//! @returns Self.
		GDINL Delegate& operator= (Delegate const& Other)
		{ 
			this->DelegateTraitsPtr = Other.DelegateTraitsPtr->Clone();
			return *this; 
		}

		//! @brief Invokes the delegate.
		//! @param Instance Object to invoke delegate on. May be null for static function.
		//! @param Arguments Invocation arguments.
		//! @returns Result of the invocation.
		GDINL ReturnType operator()(Handle const Instance, ArgumentTypes&&... Arguments) const
		{ 
			return this->DelegateTraitsPtr->Invoke(Instance, Forward<ArgumentTypes>(Arguments)...); 
		}
	};	// struct Delegate

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_TEMPLATES_DELEGATE
