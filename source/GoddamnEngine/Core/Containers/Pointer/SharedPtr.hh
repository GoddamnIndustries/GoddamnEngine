//////////////////////////////////////////////////////////////////////////
/// SharedPtr.hh - shared smart pointer interface/implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 26.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_SHARED_PTR
#define GD_CORE_CONTAINERS_SHARED_PTR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Utility.hh>
#include <GoddamnEngine/Core/TypeTraits.hh>
#include <GoddamnEngine/Core/Threading/Atomic/Atomic.hh>

GD_NAMESPACE_BEGIN

	/// Describes types of avaliable shared pointers.
	enum SharedPtrType : UInt8
	{
		GD_SHARED_PTR_FAST        = 0,	///< Fast, but not thread-safe shared pointer.
		GD_SHARED_PTR_THREAD_SAFE = 1,	///< Slow, but thread-safe shared pointer.
	};	// enum SharedPtrType	

	template<typename PointerType, SharedPtrType const PtrType = GD_SHARED_PTR_THREAD_SAFE>
	struct SharedPtr;

	template<typename PointerType>
	struct SharedPtr<PointerType, GD_SHARED_PTR_FAST> final
	{
	private:
		PointerType* Pointer        = nullptr;
		UInt32     * ReferenceCount = nullptr;

	public /*Constructors/Destructor*/:
		/// Initializes shared pointer with raw pointer value.
		/// Initial reference count is 1.
		GDINL explicit SharedPtr(PointerType* const Pointer)
			: Pointer(Pointer)
			, ReferenceCount(self->Pointer != nullptr ? new UInt32(1) : nullptr)
		{
		}

		/// Initializes shared pointer with other shared pointer.
		/// Reference counter is incremeneted.
		GDINL SharedPtr(SharedPtr const& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			if (self->ReferenceCount != nullptr)
				++(*self->ReferenceCount);
		}

		/// Moves other shared pointer to this object.
		/// Reference counter is not incremeneted.
		GDINL SharedPtr(SharedPtr&& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			Other->Pointer = nullptr;
			Other->ReferenceCount = nullptr;
		}

		/// Deinitializes this shared pointer.
		/// If decremeneted reference counter values is 0, than object would be deallocated.
		GDINL ~SharedPtr()
		{
			if (self->ReferenceCount != nullptr)
				if ((--(*self->ReferenceCount)) == 0)
				{
					delete self->Pointer;
					delete self->ReferenceCount;
				}
		}

	public /*Class API*/:
		/// Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return self->Pointer;
		}

		/// Deletes pointer, stored in this object and assigns it new specified value.
		/// @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			self->~SharedPtr();
			self->Pointer = Pointer;
			self->ReferenceCount = new UInt32(1);
			return self->Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return (*self->GetPointer()); }
		GDINL PointerType* operator-> () const { return ( self->GetPointer()); }

		GDINL bool operator== (PointerType            const* const OtherPointer) const { return (self->GetPointer() == OtherPointer); }
		GDINL bool operator== (SharedPtr<PointerType> const&       OtherPointer) const { return (self->GetPointer() == OtherPointer.GetPointer()); }
		GDINL bool operator!= (PointerType            const* const OtherPointer) const { return !(*self == OtherPointer); }
		GDINL bool operator!= (SharedPtr<PointerType> const&       OtherPointer) const { return !(*self == OtherPointer); }

		GDINL SharedPtr& operator= (SharedPtr const& OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~SharedPtr();
				self->Pointer = OtherPtr.Pointer;
				self->ReferenceCount = OtherPtr.ReferenceCount;
				if (self->ReferenceCount != nullptr)
					++(*self->ReferenceCount);
			}
			return (*self);
		}

		GDINL SharedPtr& operator= (SharedPtr&& OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~UniquePtr();
				self->Pointer = OtherPtr.Pointer;
				self->ReferenceCount = OtherPtr.ReferenceCount;
				OtherPtr.Pointer = nullptr;
				OtherPtr.ReferenceCount = nullptr;
			}

			return (*self);
		}

		GDINL SharedPtr& operator= (PointerType* const Other)
		{
			if (self->Pointer != Other)
			{
				self->~SharedPtr();
				self->Pointer = Pointer;
				self->ReferenceCount = ((self->Pointer != nullptr) ? new UInt32(1) : nullptr);
			}

			return (*self);
		}

	private:
		GDINL friend void Swap(SharedPtr& First, SharedPtr& Second)
		{
			using GD Swap;
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
		/// Initializes shared pointer with raw pointer value.
		/// Initial reference count is 1.
		GDINL explicit SharedPtr(PointerType* const Pointer = nullptr)
			: Pointer(Pointer)
			, ReferenceCount(new Int32(1))
		{
		}

		/// Initializes shared pointer with other shared pointer.
		/// Reference counter is incremeneted.
		GDINL SharedPtr(SharedPtr const& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			if (self->ReferenceCount != nullptr)
				AtomicsIntrinsics::InterlockedIncrement(self->ReferenceCount);
		}

		/// Moves other shared pointer to this object.
		/// Reference counter is not incremeneted.
		GDINL SharedPtr(SharedPtr&& Other)
			: Pointer(Other.Pointer)
			, ReferenceCount(Other.ReferenceCount)
		{
			Other.Pointer = nullptr;
			Other.ReferenceCount = nullptr;
		}

		/// Deinitializes this shared pointer.
		/// If decremeneted reference counter values is 0, than object would be deallocated.
		GDINL ~SharedPtr()
		{
			if (self->ReferenceCount != nullptr)
				if (AtomicsIntrinsics::InterlockedDecrement(self->ReferenceCount) == 0)
				{
					delete self->Pointer;
					delete self->ReferenceCount;
				}
		}

	public /*Class API*/:
		/// Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return self->Pointer;
		}

		/// Deletes pointer, stored in this object and assigns it new specified value.
		/// @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			self->~SharedPtr();
			self->Pointer = Pointer;
			self->ReferenceCount = new UInt32(1);
			return self->Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return (*self->GetPointer()); }
		GDINL PointerType* operator-> () const { return (self->GetPointer()); }

		GDINL bool operator== (PointerType            const* const OtherPointer) const { return (self->GetPointer() == OtherPointer); }
		GDINL bool operator== (SharedPtr<PointerType> const&       OtherPointer) const { return (self->GetPointer() == OtherPointer.GetPointer()); }

		GDINL bool operator!= (PointerType            const* const OtherPointer) const { return !(*self == OtherPointer); }
		GDINL bool operator!= (SharedPtr<PointerType> const&       OtherPointer) const { return !(*self == OtherPointer); }

		GDINL SharedPtr& operator= (SharedPtr const& OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~SharedPtr();
				self->Pointer = OtherPtr.Pointer;
				self->ReferenceCount = OtherPtr.ReferenceCount;
				if (self->ReferenceCount != nullptr)
					AtomicsIntrinsics::InterlockedIncrement(self->ReferenceCount);
			}
			return (*self);
		}

		GDINL SharedPtr& operator= (SharedPtr&& OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~UniquePtr();
				self->Pointer = OtherPtr.Pointer;
				self->ReferenceCount = OtherPtr.ReferenceCount;
				OtherPtr.Pointer = nullptr;
				OtherPtr.ReferenceCount = nullptr;
			}

			return (*self);
		}

		GDINL SharedPtr& operator= (PointerType* const Other)
		{
			if (self->Pointer != Other)
			{
				self->~SharedPtr();
				self->Pointer = Other;
				self->ReferenceCount = new Int32(1);
			}

			return (*self);
		}

	private:
		GDINL friend void Swap(SharedPtr& First, SharedPtr& Second)
		{
			using GD Swap;
			Swap(First.Pointer, Second.Pointer);
			Swap(First.ReferenceCount, Second.ReferenceCount);
		}
	};	// struct SharedPtr<PointerType, GD_SHARED_PTR_THREAD_SAFE>

GD_NAMESPACE_END

#endif
