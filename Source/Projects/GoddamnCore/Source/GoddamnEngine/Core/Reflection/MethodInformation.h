/// ==========================================================================================
/// MethodInformation.h: Common interface for method information
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_REFLECTION_METHODINFORMATION
#define GD_CORE_REFLECTION_METHODINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Delegate.h>
#include <GoddamnEngine/Core/Reflection/MemberInformation.h>

GD_NAMESPACE_BEGIN

	/// @brief Provides information about method.
	class IMethodInformation : public IMemberInformation
	{
		/// @brief Returns true if this method was defined as static.
		/// @returns True if this method was defined as static.
		GDAPI virtual bool IsStatic() const abstract;

		/// @brief Returns true if this methods does not modifies instance that invokes it.
		/// @returns True if this methods does not modifies instance that invokes it.
		GDAPI virtual bool IsConst() const abstract;

	//	/// @brief Returns true if this method was defined as virtual.
	//	/// @returns True if this method was defined as virtual.
	//	GDAPI virtual bool IsVirtual() const abstract;

	//	/// @brief Returns true if this method was defined as override final.
	//	/// @returns True if this method was defined as override final.
	//	GDAPI virtual bool IsOverride final() const abstract;

	//	/// @brief Returns true if this method is abstract.
	//	/// @returns True if this method is abstract.
	//	GDAPI virtual bool IsAbstract() const abstract;

		/// @brief Returns type information for class that contains this method.
		/// @returns Type information for class that contains this method.
		GDAPI virtual ITypeInformation const* GetOwnerType() const abstract;

		/// @brief Returns type information for method return type.
		/// @returns Type information for method return type.
		GDAPI virtual ITypeInformation const* GetReturnType() const abstract;
		
		/// @brief Returns list or type information for method arguments list.
		/// @returns List or type information for method arguments list.
		GDAPI virtual Vector<ITypeInformation const*> const& GetArgumentsTypes() const abstract;

	//	/// @brief Returns true if this methods has variable number of arguments.
	//	/// @returns True if this methods has variable number of arguments.
	//	GDAPI virtual bool DoesContainsVariableArguments() const abstract;

		/// @brief Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
		/// @param Reflectable Reflectable object we are operating on.
		/// @param ReturnValueOutputPtr Pointer to the methods execution result. Null pointer may be specified for voids.
		GDAPI virtual void Invoke(IReflectable* const Instance, handle const ReturnValueOutputPtr, ...) abstract;
	};	// class IMethodInformation

	/// @brief Namespace includes some support stuff for working on methods reflections.
	namespace MethodInformationDetails
	{
#define GD_METHOD_CTORS(ThisClass, BaseClass) \
		/** @brief Initializes a method information. */ \
		/** @param Method Method Object */\
		template<typename MethodSignature> \
		GDINL ThisClass(MethodSignature const& Method) \
			: BaseClass(Method) \
		{ \
		} \

		/// Type information and invocation.
		template<typename OwnerType, typename ReturnType, typename... ArgumentTypes>
		class BasicMethodInformation : public IMethodInformation, protected Delegate<ReturnType(ArgumentTypes...)>
		{
		protected:
			typedef Delegate<ReturnType(ArgumentTypes...)> DelegateType;
			GD_METHOD_CTORS(BasicMethodInformation, DelegateType);

		public:
			/// @brief Returns type information for class that contains this method.
			/// @returns Type information for class that contains this method.
			GDINL virtual ITypeInformation const* GetOwnerType()  const override final
			{
				return TypeOf<OwnerType>();
			}

			/// @brief Returns type information for method return type.
			/// @returns Type information for method return type.
			GDINL virtual ITypeInformation const* GetReturnType() const override final 
			{ 
				return TypeOf<ReturnType>(); 
			}

			/// @brief Returns list or type information for method arguments list.
			/// @returns List or type information for method arguments list.
			GDINL virtual Vector<ITypeInformation const*> const& GetArgumentsTypes() const override final
			{
				static Vector<ITypeInformation const*> const ArgumentTypesList = { TypeOf<ArgumentTypes>()... };
				return ArgumentTypesList;
			}

			/// @brief Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param ReturnValueOutputPtr Pointer to the methods execution result. Null pointer may be specified for voids.
			GDINL virtual void Invoke(IReflectable* const Instance, handle const ReturnValueOutputPtr, ...) override final
			{
				va_list ArgumentsList = va_list(); 
				va_start(ArgumentsList, ReturnValueOutputPtr);
				this->ThisMethodExType::Invoke(Instance, ReturnValueOutputPtr, ArgumentsList);
				va_end(ArgumentsList);
			}
		};	// class BasicMethodInformation

		template<typename MethodSignatureType> 
		class MethodInformation;

		/// STATIC Method information.
		template<typename ReturnType, typename... ArgumentTypes>
		class MethodInformation<ReturnType(*)(ArgumentTypes...)> : public BasicMethodInformation<nullptr_t, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInformationDetails::BasicMethodInformation<nullptr_t, ReturnType, ArgumentTypes...> BasicMethodType;
			GD_METHOD_CTORS(MethodInformation, BasicMethodType);

		public:

			/// @brief Returns true if this method was defined as static.
			/// @returns True if this method was defined as static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return true; 
			}

			/// @brief Returns true if this methods does not modifies instance that invokes it.
			/// @returns True if this methods does not modifies instance that invokes it.
			GDINL virtual bool IsConst()  const override final 
			{ 
				return false; 
			}
		};	// class MethodInformation<ReturnType(*)(ArgumentTypes...)>

		// MUTABLE MEMBER Method information.
		template<typename ClassType, typename ReturnType, typename... ArgumentTypes>
		class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...)> : public BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInformationDetails::BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...> BasicMethodType;
			GD_METHOD_CTORS(MethodInformation, BasicMethodType);

		public:

			/// @brief Returns true if this method was defined as static.
			/// @returns True if this method was defined as static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return false; 
			}

			/// @brief Returns true if this methods does not modifies instance that invokes it.
			/// @returns True if this methods does not modifies instance that invokes it.
			GDINL virtual bool IsConst()  const override final 
			{ 
				return false; 
			}
		};	// class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...)>

		// CONSTANT MEMBER Method information.
		template<typename ClassType, typename ReturnType, typename... ArgumentTypes>
		class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...) const> : public BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInformationDetails::BasicMethodInformation<ClassType, ReturnType, ArgumentTypes...> BasicMethodType;
			GD_METHOD_CTORS(MethodInformation, BasicMethodType);

		public:

			/// @brief Returns true if this method was defined as static.
			/// @returns True if this method was defined as static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return false; 
			}

			/// @brief Returns true if this methods does not modifies instance that invokes it.
			/// @returns True if this methods does not modifies instance that invokes it.
			GDINL virtual bool IsConst()  const override final 
			{ 
				return true; 
			}
		};	// class MethodInformation<ReturnType(ClassType::*)(ArgumentTypes...) const>

#undef GD_METHOD_CTORS
	}	// namespace MethodInformationDetails

	/// @brief	Provides information about method. Some of the interface functions are implemented.
	/// @code
	/// 	GDINT virtual ITypeInformation const* GetReturnType() const override final;
	///		GDINT virtual ITypeInformation const* GetOwnerType()  const override final;
	///		GDINT virtual Vector<ITypeInformation const*> const& GetArgumentsTypes() const override final;
	///		GDINT virtual void Invoke(IReflectable* const Instance, handle const ReturnValueOutputPtr, ...) override final;
	///		GDINT virtual bool IsStatic() const override final;
	///		GDINT virtual bool IsConst()  const override final;
	/// @endcode
	/// @tparam MethodSignatureType Type of the method signature.
	/// @tparam MethodPointer Pointer to the method.
	template<typename MethodSignatureType, MethodSignatureType const MethodPointer>
	class MethodInformation : public MethodInformationDetails::MethodInformation<MethodSignatureType>
	{
	public:
		GDINL MethodInformation() 
			: MethodInformationDetails::MethodInformation<MethodSignatureType>(MethodPointer)
		{
		}
	};	// class MethodInformation

GD_NAMESPACE_END

#endif