// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/SharedPtr.h
//! Shared smart pointer interface/implementation.

#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Templates/Utility.h>
#include <GoddamnEngine/Core/Templates/TypeTraits.h>
#include <GoddamnEngine/Core/Platform/Atomics.h>

GD_NAMESPACE_BEGIN

	//! Describes types of available shared pointers.
	enum SharedPtrType : UInt8
	{
		GD_SHARED_PTR_FAST  = 0,	//!< Fast, but not thread-safe shared pointer.
		GD_SHARED_PTR_THREAD_SAFE = 1,	//!< Slow, but thread-safe shared pointer.
	};	// enum SharedPtrType	

	template<typename PointerType, SharedPtrType const PtrType = GD_SHARED_PTR_FAST>
	struct SharedPtr;

	template<typename PointerType>
	struct SharedPtr<PointerType, GD_SHARED_PTR_FAST> final
	{
	private:
		PointerType* Pointer  = nullptr;
		UInt32  * ReferenceCount = nullptr;

	public /*Constructors/Destructor*/:
		//! Initializes shared pointer with raw pointer value.
		//! Initial reference count is 1.
		GDINL SharedPtr(PointerType* const Pointer = nullptr)
			: Pointer(Pointer)
			, ReferenceCount(this->Pointer != nullptr ? new UInt32(1) : nullptr)
		{
		}

		//! Initializes shared pointer with other shared pointer.
		//! Reference counter is incremeneted.
		GDINL SharedPtr(SharedPtr const& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			if (this->ReferenceCount != nullptr)
				++(*this->ReferenceCount);
		}

		//! Moves other shared pointer to this object.
		//! Reference counter is not incremeneted.
		GDINL SharedPtr(SharedPtr&& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			Other.Pointer = nullptr;
			Other.ReferenceCount = nullptr;
		}

		//! Deinitializes this shared pointer.
		//! If decremeneted reference counter values is 0, than object would be deallocated.
		GDINL ~SharedPtr()
		{
			if (this->ReferenceCount != nullptr)
				if ((--(*this->ReferenceCount)) == 0)
				{
					delete this->Pointer;
					delete this->ReferenceCount;
				}
		}

	public /*Class API*/:
		//! Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return this->Pointer;
		}

		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			this->~SharedPtr();
			this->Pointer = Pointer;
			this->ReferenceCount = new UInt32(1);
			return this->Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return (*this->GetPointer()); }
		GDINL PointerType* operator-> () const { return ( this->GetPointer()); }

		GDINL bool operator== (PointerType   const* const OtherPointer) const { return (this->GetPointer() == OtherPointer); }
		GDINL bool operator== (SharedPtr<PointerType> const&    OtherPointer) const { return (this->GetPointer() == OtherPointer.GetPointer()); }
		GDINL bool operator!= (PointerType   const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (SharedPtr<PointerType> const&    OtherPointer) const { return !(*this == OtherPointer); }

		GDINL SharedPtr& operator= (SharedPtr const& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~SharedPtr();
				this->Pointer = OtherPtr.Pointer;
				this->ReferenceCount = OtherPtr.ReferenceCount;
				if (this->ReferenceCount != nullptr)
					++(*this->ReferenceCount);
			}
			return (*this);
		}

		GDINL SharedPtr& operator= (SharedPtr&& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~UniquePtr();
				this->Pointer = OtherPtr.Pointer;
				this->ReferenceCount = OtherPtr.ReferenceCount;
				OtherPtr.Pointer = nullptr;
				OtherPtr.ReferenceCount = nullptr;
			}

			return (*this);
		}

		GDINL SharedPtr& operator= (PointerType* const Other)
		{
			if (this->Pointer != Other)
			{
				this->~SharedPtr();
				this->Pointer = Pointer;
				this->ReferenceCount = ((this->Pointer != nullptr) ? new UInt32(1) : nullptr);
			}

			return (*this);
		}

	private:
		GDINL friend void Swap(SharedPtr& First, SharedPtr& Second)
		{
			using GD::Swap;
			Swap(First.Pointer, Second.Pointer);
			Swap(First.ReferenceCount, Second.ReferenceCount);
		}
	};	// struct SharedPtr<PointerType, GD_SHARED_PTR_FAST>

	template<typename PointerType>
	struct SharedPtr<PointerType, GD_SHARED_PTR_THREAD_SAFE> final
	{
	private:
		PointerType   * Pointer = nullptr;
		Int32 volatile* ReferenceCount = nullptr;

	public /*Constructors/Destructor*/:
		//! Initializes shared pointer with raw pointer value.
		//! Initial reference count is 1.
		GDINL SharedPtr(PointerType* const Pointer = nullptr)
			: Pointer(Pointer)
			, ReferenceCount(new Int32(1))
		{
		}

		//! Initializes shared pointer with other shared pointer.
		//! Reference counter is incremeneted.
		GDINL SharedPtr(SharedPtr const& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			if (this->ReferenceCount != nullptr)
				AtomicsIntrinsics::InterlockedIncrement(this->ReferenceCount);
		}

		//! Moves other shared pointer to this object.
		//! Reference counter is not incremeneted.
		GDINL SharedPtr(SharedPtr&& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			Other.Pointer = nullptr;
			Other.ReferenceCount = nullptr;
		}

		//! Deinitializes this shared pointer.
		//! If decremeneted reference counter values is 0, than object would be deallocated.
		GDINL ~SharedPtr()
		{
			if (this->ReferenceCount != nullptr)
				if (AtomicsIntrinsics::InterlockedDecrement(this->ReferenceCount) == 0)
				{
					delete this->Pointer;
					delete this->ReferenceCount;
				}
		}

	public /*Class API*/:
		//! Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return this->Pointer;
		}

		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			this->~SharedPtr();
			this->Pointer = Pointer;
			this->ReferenceCount = new UInt32(1);
			return this->Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return (*this->GetPointer()); }
		GDINL PointerType* operator-> () const { return (this->GetPointer()); }

		GDINL bool operator== (PointerType   const* const OtherPointer) const { return (this->GetPointer() == OtherPointer); }
		GDINL bool operator== (SharedPtr<PointerType> const&    OtherPointer) const { return (this->GetPointer() == OtherPointer.GetPointer()); }

		GDINL bool operator!= (PointerType   const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (SharedPtr<PointerType> const&    OtherPointer) const { return !(*this == OtherPointer); }

		GDINL SharedPtr& operator= (SharedPtr const& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~SharedPtr();
				this->Pointer = OtherPtr.Pointer;
				this->ReferenceCount = OtherPtr.ReferenceCount;
				if (this->ReferenceCount != nullptr)
					AtomicsIntrinsics::InterlockedIncrement(this->ReferenceCount);
			}
			return (*this);
		}

		GDINL SharedPtr& operator= (SharedPtr&& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~SharedPtr();
				this->Pointer = OtherPtr.Pointer;
				this->ReferenceCount = OtherPtr.ReferenceCount;
				OtherPtr.Pointer = nullptr;
				OtherPtr.ReferenceCount = nullptr;
			}

			return (*this);
		}

		GDINL SharedPtr& operator= (PointerType* const Other)
		{
			if (this->Pointer != Other)
			{
				this->~SharedPtr();
				this->Pointer = Other;
				this->ReferenceCount = new Int32(1);
			}

			return (*this);
		}

	private:
		GDINL friend void Swap(SharedPtr& First, SharedPtr& Second)
		{
			using GD::Swap;
			Swap(First.Pointer, Second.Pointer);
			Swap(First.ReferenceCount, Second.ReferenceCount);
		}
	};	// struct SharedPtr<PointerType, GD_SHARED_PTR_THREAD_SAFE>

GD_NAMESPACE_END
