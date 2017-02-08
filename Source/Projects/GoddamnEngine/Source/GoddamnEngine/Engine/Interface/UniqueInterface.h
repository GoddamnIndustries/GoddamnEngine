// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Interface/UniqueInterface.h
 * File contains common interfaces that would be exported to the higher engine abstraction
 * layers that are written in the D programming language.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Object/Object.h>

#define GD_DEFINE_CLASS_INFORMATION(...)
#define GD_IMPLEMENT_CLASS_INFORMATION(...)
#define GD_IMPLEMENT_CLASS_INFORMATION_NOT_CONSTRUCTIBLE(...)
#define GD_IMPLEMENT_CLASS_INFORMATION_FORCE_NO_TAGS(...)
#define IClassConstrutorProc void*
// Attributes declaration for the reflector utility.
#if !GD_D_REFLECTOR
#	define GD_DCODE(...)
#	define GD_DENUM(...)
#	define GD_DSTRUCT(...)
#	define GD_DCTOR(...)
#	define GD_DFGDLD(...)
#	define GD_DFUNCTION(...)
#	define GD_DSTATIC_VARIABLE(...)
#	define GD_DINTERFACE(...)
#	define GD_DCOMPONENT GD_DINTERFACE
#	define GD_DALIAS(...)
#endif	// if !GD_D_REFLECTOR

 // ------------------------------------------------------------------------------------------
 //! Keyword to declare the unique interfaces.
#define uinterface struct

// ------------------------------------------------------------------------------------------
//! Defines a base type for the unique interface partial declaration.
//! @param IInterfaceType Type of the interface to declare.
//! @param IBaseInterfaceType Base type for this interface.
#define GD_UNIQUE_INTERFACE_DEFINE_PARTIAL_BASE(IInterfaceType, IBaseInterfaceType) \
	uinterface __ ## IInterfaceType ## Zero : public IBaseInterfaceType {}; \
	enum __ ## IInterfaceType ## ZeroCounterType { __ ## IInterfaceType ## CounterBase = __COUNTER__ };\
	template<int __ ## IInterfaceType ## ChainID> struct __ ## IInterfaceType ## ByChainID { typedef __ ## IInterfaceType ## Zero Type; };

// ------------------------------------------------------------------------------------------
//! Defines a partial interface declaration.
//! @param IInterfaceType Type of the partial interface to declare.
//! @param IPartialBaseInterfaceType Base for the partial interface type.
#define GD_UNIQUE_INTERFACE_DEFINE_PARTIAL(IInterfaceType, IPartialBaseInterfaceType) \
	uinterface IInterfaceType; \
	enum __ ## IInterfaceType ## CounterType { __ ## IInterfaceType ## ChainID = __COUNTER__ - __ ## IPartialBaseInterfaceType ## CounterBase - 1 };\
	template<> struct __ ## IPartialBaseInterfaceType ## ByChainID<__ ## IInterfaceType ## ChainID> { typedef IInterfaceType Type; }; \
	uinterface IInterfaceType : public __ ## IPartialBaseInterfaceType ## ByChainID<__ ## IInterfaceType ## ChainID - 1>::Type

GD_NAMESPACE_BEGIN

	class PPipeline;

	template<typename Tp>
	using DIRefPtr = Tp*;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IResult enumeration.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Defines a list of the unique interfaces return code.
	GD_DENUM()
	enum class IResult : Int32
	{
		Ok = 0,
		NothingWasDone,
		NothingWasDone2,
		Error = Int32Min,
		NotImplemented,
		InvalidArguments,
		InterfaceAlreadyInitialized,
		InterfaceWasNotInitialized,
		NativeFunctionFailed,
	};	// enum class IResult

	// ------------------------------------------------------------------------------------------
	//! Tests, whether the result value represents successful one.
	//! @param Result The result value to test.
	//! @returns True if result represents successful value.
	GD_DFUNCTION()
	GDINL bool ISucceeded(IResult const Result)
	{
		return static_cast<Int32>(Result) >= 0;
	}

	// ------------------------------------------------------------------------------------------
	//! Tests, whether the result value represents failed one.
	//! @param Result The result value to test.
	//! @returns True if result represents failed value.
	GD_DFUNCTION()
	GDINL bool IFailed(IResult const Result)
	{
		return static_cast<Int32>(Result) < 0;
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IInterface&IRuntimeUniqueInterface interfaces.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//!	The most base class for all engine (unique) interfaces.
	GD_DINTERFACE()
	uinterface IInterface : public ReferenceTarget
	{
		GD_DEFINE_CLASS_INFORMATION(IInterface, Object);

	private:
		friend class Allocator;
		mutable Int32 DReferenceCount = 1;

	public:
		GDINL explicit IInterface() {}
		GDINL virtual ~IInterface() {}

	public:

	};	// struct IInterface

	// ------------------------------------------------------------------------------------------
	//! Adds a reference to the interface, if it is not a null pointer.
	//! @param IInterface Interface to proceed.
	//! @returns The specified interface.
	template<typename IInterfaceTp
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<TypeTraits::IsBase<IInterface, IInterfaceTp>::Value>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL IInterfaceTp const* ISafeAddReference(IInterfaceTp* const IInterface)
	{
		if (IInterface != nullptr)
			IInterface->AddRereference();
		return IInterface;
	}

	// ------------------------------------------------------------------------------------------
	//! Adds a reference to the interface, if it is not a null pointer.
	//! @param IInterface Interface to proceed.
	//! @returns The specified interface.
	template<typename IInterfaceTp
#if !GD_DOCUMENTATION
		, typename = typename EnableIf<TypeTraits::IsBase<IInterface, IInterfaceTp>::Value>::Type
#endif	// if !GD_DOCUMENTATION
	>
	GDINL IInterfaceTp const* ISafeRelease(IInterfaceTp* const IInterface)
	{
		if (IInterface != nullptr)
			IInterface->Release();
		return IInterface;
	}

	// ------------------------------------------------------------------------------------------
	//! The most base class for all runtime unique interfaces.
	GD_DINTERFACE()
	uinterface IRuntimeUniqueInterface : public IInterface
	{
		GD_DEFINE_CLASS_INFORMATION(IRuntimeUniqueInterface, IInterface);

	public:

		// ... Initialization operations ...

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global initialization step, before all other runtime
		//! interfaces are initialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimePreInitialize()
		{
			return IResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global initialization step.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimeInitialize()
		{
			return IResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global initialization step, after all other runtime
		//! interfaces are initialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimePostInitialize()
		{
			return IResult::Ok;
		}

		// ... Deinitialization operations ...

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global deinitialization step, before all other runtime
		//! interfaces are deinitialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimePreDeinitialize()
		{
			return IResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global deinitialization step.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimeDeinitialize()
		{
			return IResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global deinitialization step, after all other runtime 
		//! interfaces are deinitialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION()
		GDAPI virtual IResult OnRuntimePostDeinitialize()
		{
			return IResult::Ok;
		}

		// ... Updating operations ...

		// ------------------------------------------------------------------------------------------
		//! Function would be called once per frame, before all other runtime
		//! interfaces are updated.
		GD_DFUNCTION()
		GDAPI virtual void OnRuntimePreUpdate()
		{
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called once per frame.
		GD_DFUNCTION()
		GDAPI virtual void OnRuntimeUpdate()
		{
		}
		
		// ------------------------------------------------------------------------------------------
		//! Function would be called once per frame, after all other runtime 
		//! interfaces are deinitialized.
		GD_DFUNCTION()
		GDAPI virtual void OnRuntimePostUpdate()
		{
		}
	};	// uinterface IRuntimeUniqueInterface

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IPtr struct.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename Tp>
	using IPtr = Tp*;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// IUniqueInterfacePointer struct.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !GD_DOCUMENTATION
	struct IUniqueInterfacePointerBase : public TNonCopyable
	{
	public:
		// See PPipeline.cpp for the implementations.
		GDAPI IUniqueInterfacePointerBase(IInterface* const UniqueInterface, PPipeline* const UniqueInterfacePipeline) {}
		GDAPI IUniqueInterfacePointerBase(IRuntimeUniqueInterface* const UniqueRuntimeInterface, PPipeline* const UniqueInterfacePipeline) {}
	};	//struct IUniqueInterfacePointerBase
#endif	// if !GD_DOCUMENTATION

	// ------------------------------------------------------------------------------------------
	//! Struct that handles all unique runtime interfaces.
	template<typename Tp>
	struct IUniqueInterfacePointer : public IUniqueInterfacePointerBase, public UniquePtr<Tp>
	{
	public:
		// ------------------------------------------------------------------------------------------
		//! Initializes a global pointer to the unique interface.
		//! @param UniqueInterfacePipeline PPipeline, to which the this unique interface would be attached.
		//! @param UniqueInterfaceCreatorProc Pointer to function, that creates instance of the implementation for this interface.
		GDINL IUniqueInterfacePointer(Tp* const UniqueInterface, PPipeline* const UniqueInterfacePipeline)
			: IUniqueInterfacePointerBase(UniqueInterface, UniqueInterfacePipeline), UniquePtr<Tp>(UniqueInterface) {}
	};	// struct IUniqueInterfacePointer

GD_NAMESPACE_END
