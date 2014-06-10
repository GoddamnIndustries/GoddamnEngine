//////////////////////////////////////////////////////////////////////////
/// MethodInformation.h: Common interface for field information
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.04.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_REFLECTION_METHODINFORMATION
#define GD_CORE_REFLECTION_METHODINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String/String.h>
#include <GoddamnEngine/Core/Containers/Vector/Vector.h>
#include <GoddamnEngine/Core/Containers/Pointer/Pointer.h>
#include <GoddamnEngine/Core/Reflection/MemberInformation.h>

#include <stdarg.h> // For variadic arguments
#include <tuple>    // For std::tuple

GD_NAMESPACE_BEGIN

	/// Provides information about method.
	class IMethodInformation : public IMemberInformation
	{
		/// @name Qualifiers access
		/// @{

		/// Returns true if this method was defined as static.
		GDAPI virtual bool IsStatic() const abstract;

		/// Returns true if this methods does not modifies instance that invokes it.
		GDAPI virtual bool IsConst() const abstract;

		// /// Returns true if this method was defined as virtual.
		// GDAPI virtual bool IsVirtual() const abstract;

		// /// Returns true if this method was defined as final.
		// GDAPI virtual bool IsFinal() const abstract;

		// /// Returns true if this method is abstract.
		// GDAPI virtual bool IsAbstract() const abstract;

		/// @}

		/// Returns type information for class that contains this method.
		GDAPI virtual ITypeInformation const* GetOwnerType() const abstract;

		/// Returns type information for method return type.
		GDAPI virtual ITypeInformation const* GetReturnType() const abstract;
		
		/// Returns list or type information for method arguments list.
		GDAPI virtual Vector<ITypeInformation const*> const& GetArgumentsTypes() const abstract;

		// /// Returns true if this methods has variable number of arguments.
		// GDAPI virtual bool DoesContainsVariableArguments() const abstract;

		/// Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
		GDAPI virtual void Invoke(_In_ IReflectable* const Instance, handle const ReturnValueOutputPtr, ...) abstract;
	};	// class IMethodInformation

	/// Namespace includes some support stuff for working on methods reflections.
	namespace MethodInformationDetails
	{
#if (!defined(GD_DOCUMENTATION))
		template<typename MethodSignatureType> struct MethodBase;
#endif // if (!defined(GD_DOCUMENTATION))

#define GD_METHOD_CTORS(ThisClass, BaseClass)																			\
		template<typename MethodSignature>																				\
		GDINL ThisClass(MethodSignature const Method) : BaseClass(Method) { }											\
		GDINL ThisClass(ThisClass const& ThisClass##Ref) : BaseClass(ThisClass##Ref) { }

		/// Basic template wrapper on method pointers. This class is same to @c std::function class with some differences (for member functions).
		/// When specifying member function it is not required to pass instance type as first params arguments. 
		/// Invocation operator takes pointer to instance as it`s first argument (handle type, it is better to make a const_cast for constant objects). 
		/// For pointers to to static functions instance param can take any value (nullptr is preferred).
		template<typename ReturnType, typename... ArgumentTypes>
		struct MethodBase<ReturnType(ArgumentTypes...)> /*: public std::function<ReturnType(ArgumentTypes...)>*/
		{
		private:		
			struct IMethodTraitsBase
			{
			private:
				GD_CLASS_UNASSIGNABLE(IMethodTraitsBase);
				GD_CLASS_UNCOPIABLE(IMethodTraitsBase);

			public:
				GDINL IMethodTraitsBase() { }
				GDINT virtual IMethodTraitsBase const* Clone() const abstract;
				GDINT virtual ReturnType Invoke(handle const Instance, ArgumentTypes&&... Arguments) const abstract;
			};	// struct IMethodTraitsBase

			template<typename MethodSignatureType> struct MethodTraits;

			// Traits for STATIC METHOD
			template<>
			struct MethodTraits<ReturnType(*)(ArgumentTypes...)> final : public IMethodTraitsBase
			{
			private:
				typedef ReturnType(*ThisMethodType)(ArgumentTypes...);
				ThisMethodType const MethodPtr = nullptr;

			public:
				GDINL MethodTraits(ThisMethodType const MethodPtr) : MethodPtr(MethodPtr) { }
				GDINL virtual IMethodTraitsBase const* Clone() const { return new MethodTraits<ReturnType(*)(ArgumentTypes...)>(self->MethodPtr); }
				GDINL virtual ReturnType Invoke(handle const Instance, ArgumentTypes&&... Arguments) const
				{
					GD_UNUSED(Instance); // Since no instance is required to be used while invocation of static function
					return (*self->MethodPtr)(Forward<ArgumentTypes>(Arguments)...);
				}
			};	// struct MethodTraits<ReturnType(*)(ArgumentTypes...)>

			// Traits for MUTABLE MEMBER METHOD
			template<typename ClassType>
			struct MethodTraits<ReturnType(ClassType::*)(ArgumentTypes...)> final : public IMethodTraitsBase
			{
			private:
				typedef ReturnType(ClassType::* ThisMethodType)(ArgumentTypes...);
				ThisMethodType const MethodPtr = nullptr;

			public:
				GDINL MethodTraits(ThisMethodType const MethodPtr) : MethodPtr(MethodPtr) { }
				GDINL virtual IMethodTraitsBase const* Clone() const { return new MethodTraits<ReturnType(ClassType::*)(ArgumentTypes...)>(self->MethodPtr); }
				GDINL virtual ReturnType Invoke(handle const Instance, ArgumentTypes&&... Arguments) const
				{
					ClassType* const ClassInstance = static_cast<ClassType*>(Instance);
					return (ClassInstance->*self->MethodPtr)(Forward<ArgumentTypes>(Arguments)...);
				}
			};	// struct MethodTraits<ReturnType(ClassType::*)(ArgumentTypes...)>

			// Traits for CONSTANT MEMBER METHOD
			template<typename ClassType>
			struct MethodTraits<ReturnType(ClassType::*)(ArgumentTypes...) const> final : public IMethodTraitsBase
			{
			private:
				typedef ReturnType(ClassType::* ThisMethodType)(ArgumentTypes...) const;
				ThisMethodType const MethodPtr = nullptr;

			public:
				GDINL MethodTraits(ThisMethodType const MethodPtr) : MethodPtr(MethodPtr) { }
				GDINL virtual IMethodTraitsBase const* Clone() const { return new MethodTraits<ReturnType(ClassType::*)(ArgumentTypes...) const>(self->MethodPtr); }
				GDINL virtual ReturnType Invoke(handle const Instance, ArgumentTypes&&... Arguments) const
				{
					ClassType const* const ClassInstance = static_cast<ClassType const*>(Instance);
					return (ClassInstance->*self->MethodPtr)(Forward<ArgumentTypes>(Arguments)...);
				}
			};	// struct MethodTraits<ReturnType(ClassType::*)(ArgumentTypes...) const>

			UniquePtr<IMethodTraitsBase> MethodTraitsPtr;

		public:
			/// @todo Rewrite this stuff without using any STL code
			template<size_t const Index> using ThisArgumentType = typename std::tuple_element<Index, std::tuple<ArgumentTypes...>>::type;
			template<size_t const Index> using ThisUsedArgumentType = typename TypeTraits::RemoveReference<ThisArgumentType<Index>>::Type*;
			
			template<typename MethodSignature>
			GDINL MethodBase(MethodSignature const Method) : MethodTraitsPtr(new MethodTraits<MethodSignature>(Method)) { }
			GDINL MethodBase(MethodBase const& MethodBaseRef) : MethodTraitsPtr(MethodBaseRef->Clone()) { }

		public:
			GDINL MethodBase& operator= (MethodBase const& MethodBaseRef) { self->MethodTraitsPtr = MethodBaseRef.MethodTraitsPtr; return (*self); }
			GDINL ReturnType  operator()(handle const Instance, ArgumentTypes&&... Arguments) const { return self->MethodTraitsPtr->Invoke(Instance, Forward<ArgumentTypes>(Arguments)...); }
			GDINL bool operator== (MethodBase const& MethodBaseRef) const { return ((*self->MethodTraitsPtr) == *(MethodBaseRef.MethodTraitsPtr)); }
			GDINL bool operator!= (MethodBase const& MethodBaseRef) const { return !((*self) == MethodBaseRef); }
		};	// struct MethodBase

#if (!defined(GD_DOCUMENTATION))
#	define GD_METHOD_INV_MAX_ARGUMENTS 4
#	define GD_METHOD_INV_GET_ARGUMENT(ArgIndex) auto&& A##ArgIndex = Forward<ThisArgumentType<ArgIndex>>(*va_arg(ArgumentsList, ThisUsedArgumentType<ArgIndex>))
#	define GD_METHOD_INV_CTORS() GD_METHOD_CTORS(MethodInvBase, MethodBase<ReturnType(ArgumentTypes...)>)
		namespace MethodPrivate
		{
			template<size_t const ArgumentsCount, typename ReturnType, typename... ArgumentTypes>
			struct MethodInvBase
			{	// If this error is thrown here than you should add your impl for your number of arguments
				static_assert(ArgumentsCount <= GD_METHOD_INV_MAX_ARGUMENTS, "Arguments count is more than maximum implemented.");
			};	// struct MethodInvBase

			template<typename ReturnType, typename... ArgumentTypes>
			struct MethodInvBase<0, ReturnType(ArgumentTypes...)> : public MethodBase<ReturnType(ArgumentTypes...)>
			{	GD_METHOD_INV_CTORS();
				GDINL ReturnType Invoke(handle const Instance, va_list ArgumentsList) const 
				{
					GD_UNUSED(ArgumentsList);
					return (*self)(Instance); 
				}
			};	// struct MethodInvBase<0>

			template<typename ReturnType, typename... ArgumentTypes>
			struct MethodInvBase<1, ReturnType(ArgumentTypes...)> : public MethodBase<ReturnType(ArgumentTypes...)>
			{	GD_METHOD_INV_CTORS();
				GDINL ReturnType Invoke(handle const Instance, va_list ArgumentsList) const 
				{
					GD_METHOD_INV_GET_ARGUMENT(0);
					return (*self)(Instance, A0);
				}
			};	// struct MethodInvBase<1>

			template<typename ReturnType, typename... ArgumentTypes>
			struct MethodInvBase<2, ReturnType(ArgumentTypes...)> : public MethodBase<ReturnType(ArgumentTypes...)>
			{	GD_METHOD_INV_CTORS();
				GDINL ReturnType Invoke(handle const Instance, va_list ArgumentsList) const 
				{ 
					GD_METHOD_INV_GET_ARGUMENT(0);
					GD_METHOD_INV_GET_ARGUMENT(1);
					return (*self)(Instance, A0, A1);
				}
			};	// struct MethodInvBase<2>

			template<typename ReturnType, typename... ArgumentTypes>
			struct MethodInvBase<3, ReturnType(ArgumentTypes...)> : public MethodBase<ReturnType(ArgumentTypes...)>
			{	GD_METHOD_INV_CTORS();
				GDINL ReturnType Invoke(handle const Instance, va_list ArgumentsList) const 
				{ 
					GD_METHOD_INV_GET_ARGUMENT(0);
					GD_METHOD_INV_GET_ARGUMENT(1);
					GD_METHOD_INV_GET_ARGUMENT(2);
					return (*self)(Instance, A0, A1, A2);
				}
			};	// struct MethodInvBase<3>

			template<typename ReturnType, typename... ArgumentTypes>
			struct MethodInvBase<4, ReturnType(ArgumentTypes...)> : public MethodBase<ReturnType(ArgumentTypes...)>
			{	GD_METHOD_INV_CTORS();
				GDINL ReturnType Invoke(handle const Instance, va_list ArgumentsList) 
				{ 
					GD_METHOD_INV_GET_ARGUMENT(0);
					GD_METHOD_INV_GET_ARGUMENT(1);
					GD_METHOD_INV_GET_ARGUMENT(2);
					GD_METHOD_INV_GET_ARGUMENT(3);
					return (*self)(Instance, A0, A1, A2, A3);
				}
			};	// struct MethodInvBase<4>
		}	// namespace MethodPrivate
#	undef GD_METHOD_INV_MAX_ARGUMENTS
#	undef GD_METHOD_INV_GET_ARGUMENT
#	undef GD_METHOD_INV_CTORS
#endif	// if (!defined(GD_DOCUMENTATION))

		// User does need to know MethodEx`s real base type
#if (!defined(GD_DOCUMENTATION))
#	define GD_METHOD_EX_BASE_TYPE MethodPrivate::MethodInvBase<sizeof...(ArgumentTypes), ReturnType(ArgumentTypes...)>
		template<typename ReturnType, typename... ArgumentTypes> struct MethodEx;
#else	
#	define GD_METHOD_EX_BASE_TYPE MethodBase<ReturnType(ArgumentTypes...)>
#endif	// if (!defined(GD_DOCUMENTATION))

		/// Advanced template wrapper on method pointers that allows to invoke method with arguments specified as variadic list.
		template<typename ReturnType, typename... ArgumentTypes>
		struct MethodEx<ReturnType(ArgumentTypes...)> : public GD_METHOD_EX_BASE_TYPE
		{
			typedef GD_METHOD_EX_BASE_TYPE ThisBaseType;
			GD_METHOD_CTORS(MethodEx, ThisBaseType);

			/// Invokes method and returns it`s return value. If this is a member function then owner object should be passed first into variadic function arguments.
			/// All parameters that are passed by pointers on parameters instances. For example:
			/// @todo Add invocation example here
			GDINL void Invoke(handle const Instance, handle const ReturnValueOutputPtr, va_list ArgumentsList) const
			{
				*static_cast<ReturnType*>(ReturnValueOutputPtr) = Forward<ReturnType>(self->ThisBaseType::Invoke(Instance, ArgumentsList));
			}
		};	// struct MethodEx<ReturnType>
#undef GD_METHOD_EX_BASE_TYPE

#if (!defined(GD_DOCUMENTATION))
		// Specialization for void return type
		template<typename... ArgumentTypes>
		struct MethodEx<void(ArgumentTypes...)> : public MethodPrivate::MethodInvBase<sizeof...(ArgumentTypes), void(ArgumentTypes...)>
		{
			typedef MethodPrivate::MethodInvBase<sizeof...(ArgumentTypes), void(ArgumentTypes...)> ThisBaseType;
			GD_METHOD_CTORS(MethodEx, ThisBaseType);

			GDINL void Invoke(handle const Instance, handle const ReturnValueOutputPtr, va_list ArgumentsList) const
			{
				GD_UNUSED(ReturnValueOutputPtr);
				self->ThisMethodExBaseType::Invoke(Instance, ArgumentsList);
			}
		};	// struct MethodEx<void>
#endif	// if (!defined(GD_DOCUMENTATION))
	}	// namespace MethodInformationDetails

#if (!defined(GD_DOCUMENTATION))
	// Contains some basic interface implementations that does not require to be exported
	namespace MethodInformationPrivate
	{
		// Type information and invocation
		template<typename OwnerType, typename ReturnType, typename... ArgumentTypes>
		class BasicMethodInformation : public   IMethodInformation, 
									   protected MethodInformationDetails::MethodEx<ReturnType(ArgumentTypes...)>
		{
		protected:
			typedef MethodInformationDetails::MethodEx<ReturnType(ArgumentTypes...)> ThisMethodExType;
			GD_METHOD_CTORS(BasicMethodInformation, ThisMethodExType);

		public:
			GDINL virtual ITypeInformation const* GetReturnType() const final { return TypeOf<ReturnType>(); }
			GDINL virtual ITypeInformation const* GetOwnerType()  const final { return TypeOf<OwnerType>(); }
			GDINL virtual Vector<ITypeInformation const*> const& GetArgumentsTypes() const final
			{
				static Vector<ITypeInformation const*> const ArgumentTypesList = { TypeOf<ArgumentTypes>()... };
				return ArgumentTypesList;
			}

			GDINL virtual void Invoke(_In_ IReflectable* const Instance, handle const ReturnValueOutputPtr, ...) final
			{
				va_list ArgumentsList = va_list(); 
				va_start(ArgumentsList, ReturnValueOutputPtr);
				self->ThisMethodExType::Invoke(Instance, ReturnValueOutputPtr, ArgumentsList);
				va_end(ArgumentsList);
			}
		};	// class BasicMethodInformation

		template<typename MethodSignatureType> class MethodInformation;

		// STATIC Method information
		template<typename ReturnType, typename... ArgumentTypes>
		class MethodInformation<ReturnType(*)(ArgumentTypes...)> : public BasicMethodInformation<nullptr_t, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInformationPrivate::BasicMethodInformation<nullptr_t, ReturnType, ArgumentTypes...> ThisBaseMethodType;
			GD_METHOD_CTORS(MethodInformation, ThisBaseMethodType);

		public:
			GDINL virtual bool IsStatic() const final { return true; }
			GDINL virtual bool IsConst()  const final { return false; }
		};	// class MethodInformation<ReturnType(*)(ArgumentTypes...)>

		// MUTABLE MEMBER Method information
		template<typename ClassType, typename ReturnType, typename... ArgumentTypes>
		class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...)> : public BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInformationPrivate::BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...> ThisBaseMethodType;
			GD_METHOD_CTORS(MethodInformation, ThisBaseMethodType);

		public:
			GDINL virtual bool IsStatic() const final { return false; }
			GDINL virtual bool IsConst()  const final { return false; }
		};	// class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...)>

		// CONSTANT MEMBER Method information
		template<typename ClassType, typename ReturnType, typename... ArgumentTypes>
		class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...) const> : public BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInformationPrivate::BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...> ThisBaseMethodType;
			GD_METHOD_CTORS(MethodInformation, ThisBaseMethodType);

		public:
			GDINL virtual bool IsStatic() const final { return false; }
			GDINL virtual bool IsConst()  const final { return true; }
		};	// class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...) const>
	}	// namespace MethodInformationPrivate
#endif	// if (!defined(GD_DOCUMENTATION))
#undef GD_METHOD_CTORS

#if (!defined(GD_DOCUMENTATION))
	template<typename MethodSignatureType, MethodSignatureType const MethodPtr>
	class MethodInformation : public MethodInformationPrivate::MethodInformation<MethodSignatureType>
	{
	public:
		GDINL MethodInformation() : MethodInformationPrivate::MethodInformation<MethodSignatureType>(MethodPtr) { }
	};	// class MethodInformation
#else	// if (!defined(GD_DOCUMENTATION))
	/// Implements some staff about methods that can be generated via templates 
	template<typename MethodSignatureType, MethodSignatureType const MethodPtr>
	class MethodInformation : public IMethodInformation
	{
	public:
		/// Implemented in this class: 
		GDINT virtual ITypeInformation const* GetReturnType() const final;
		GDINT virtual ITypeInformation const* GetOwnerType()  const final;
		GDINT virtual Vector<ITypeInformation const*> const& GetArgumentsTypes() const final;
		GDINT virtual void Invoke(_In_ IReflectable* const Instance, handle const ReturnValueOutputPtr, ...) final;
		GDINT virtual bool IsStatic() const final;
		GDINT virtual bool IsConst()  const final;
	};	// class MethodInformation
#endif	// if (!defined(GD_DOCUMENTATION))

	/// @see MethodInformationDetails::MethodBase
	template<typename MethodSignatureType>
	using Method = MethodInformationDetails::MethodBase<MethodSignatureType>;

GD_NAMESPACE_END

#endif