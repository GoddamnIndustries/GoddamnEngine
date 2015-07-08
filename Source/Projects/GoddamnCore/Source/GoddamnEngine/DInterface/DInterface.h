// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/DInterface/DInterface.h
 * File contains common interfaces that would be exported to the higher engine abstraction
 * layers that are written in the D programming language.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN
	
#define GD_DLOG(String, ...) printf_s("\n" String, ##__VA_ARGS__)

#define GD_DCODE(...)
#define GD_DFUNCTION(...)
#define GD_DINTERFACE(...)
#define GD_DALIAS(...)

#define GD_DASSERT GD_ASSERT
#define GD_DDEBUG_ASSERT GD_DEBUG_ASSERT

	enum class DIResult : Int32
	{
		Ok = 0,
		NothingWasDone,
		NothingWasDone2,
		Error = Int32Min,
		NotImplemented,
		InterfaceAlreadyInitialized,
		InterfaceWasNotInitialized,
		NativeFunctionFailed,
	};	// enum class DIResult

	// ------------------------------------------------------------------------------------------
	//! Tests, whether the result value represents successful one.
	//! @param Result Result value to test.
	//! @returns True if result represents successful value.
	GD_DFUNCTION("public static bool DISucceeded(const DIResult Result) { return cast(int)Result >= 0; }")
	GDINL bool DISucceeded(DIResult const Result)
	{
		return static_cast<Int32>(Result) >= 0;
	}

	// ------------------------------------------------------------------------------------------
	//! Tests, whether the result value represents failed one.
	//! @param Result Result value to test.
	//! @returns True if result represents failed value.
	GD_DFUNCTION("public static bool DIFailed(const DIResult Result) { return cast(int)Result < 0; }")
	GDINL bool DIFailed(DIResult const Result)
	{
		return static_cast<Int32>(Result) < 0;
	}

	/*!
	 * The most base class for all D export classes.
	 */
	GD_DINTERFACE("interface DInterface")
	struct DInterface : public IUncopiable
	{
	private:
		friend class Allocator;
		mutable Int32 DReferenceCount = 1;

	public:
		GDINL explicit DInterface() {}
		GDINL virtual ~DInterface() {}


#if GD_REFLECTION

		// ------------------------------------------------------------------------------------------
		//! A dummy function, that makes an offset for a D's interface VFT to match C++'s.
		GD_DFUNCTION(protected void __CallVirtualDestructor() @nogo nothrow)
		GDAPI /*virtual*/ void __CallVirtualDestructor() { }

#endif	// if GD_REFLECTION

	public:

		// ------------------------------------------------------------------------------------------
		//! Atomically increments the reference counter of this object.
		//! @returns Value of of the counter after operation proceeded.
		GD_DFUNCTION("public int AddReference() @nogo nothrow;")
		GDAPI virtual Int32 AddRereference() const final
		{
			//! @todo Add atomic operation for this increment.
			++this->DReferenceCount;
			return this->DReferenceCount;
		}

		// ------------------------------------------------------------------------------------------
		//! Atomically decrements the reference counter of this object.
		//! If the value of the counter reaches zero, then object is going to be safely destroyed.
		//! \returns Value of of the counter after operation proceeded.
		GD_DFUNCTION("public int Release() @nogc nothrow;")
		GDAPI virtual Int32 Release() const final
		{
			//! @todo Add atomic operation for this decrement.
			--this->DReferenceCount;
			if (this->DReferenceCount == 0)
				GD_DELETE(const_cast<DInterface*>(this));
			return this->DReferenceCount;
		}
	};	// struct DInterface

	GD_DINTERFACE("public interface DRuntimeInterface : DInterface")
	struct DRuntimeInterface : public DInterface
	{
	public:

		// ... Initialization operations ...

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global initialization step, before all other interfaces
		//! are initialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Runtime_PreInitialize() @nogc nothrow")
		GDAPI virtual DIResult Runtime_PreInitialize()
		{
			return DIResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global initialization step.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Runtime_Initialize() @nogc nothrow")
		GDAPI virtual DIResult Runtime_Initialize()
		{
			return DIResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global initialization step, after all other interfaces
		//! are initialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Runtime_PostInitialize() @nogc nothrow")
		GDAPI virtual DIResult Runtime_PostInitialize()
		{
			return DIResult::Ok;
		}

		// ... Deinitialization operations ...

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global deinitialization step, before all other interfaces
		//! are deinitialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Runtime_PreDeinitialize() @nogc nothrow")
		GDAPI virtual DIResult Runtime_PreDeinitialize()
		{
			return DIResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global deinitialization step.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Runtime_Deinitialize() @nogc nothrow")
		GDAPI virtual DIResult Runtime_Deinitialize()
		{
			return DIResult::Ok;
		}

		// ------------------------------------------------------------------------------------------
		//! Function would be called on the global deinitialization step, after all other interfaces
		//! are deinitialized.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Runtime_PostDeinitialize() @nogc nothrow")
		GDAPI virtual DIResult Runtime_PostDeinitialize()
		{
			return DIResult::Ok;
		}




		GDAPI virtual void Runtime_PreUpdate() {}
		GDAPI virtual void Runtime_Update() {}
		GDAPI virtual void Runtime_PostUpdate() {}

	};	// struct DRuntimeInterface

GD_NAMESPACE_END
