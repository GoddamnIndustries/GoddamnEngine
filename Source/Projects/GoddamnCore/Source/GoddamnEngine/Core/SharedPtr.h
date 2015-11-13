// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/SharedPtr.h
//! Shared smart pointer interface/Implementation.

#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/TypeTraits.h>

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
		PointerType* m_RawPointer  = nullptr;
		UInt32*      m_ReferenceCount = nullptr;

	public /*Constructors/Destructor*/:
		//! Initializes shared pointer with raw pointer value.
		//! Initial reference count is 1.
		GDINL SharedPtr(PointerType* const pointer = nullptr)
			: m_RawPointer(pointer)
			, m_ReferenceCount(pointer != nullptr ? new UInt32(1) : nullptr)
		{
		}

		//! Initializes shared pointer with other shared pointer.
		//! Reference counter is incremeneted.
		GDINL SharedPtr(SharedPtr const& other)
			: m_RawPointer(other.m_RawPointer)
			, m_ReferenceCount(other.m_ReferenceCount)
		{
			if (m_ReferenceCount != nullptr)
				++*m_ReferenceCount;
		}

		//! Moves other shared pointer to this object.
		//! Reference counter is not incremeneted.
		GDINL SharedPtr(SharedPtr&& other)
			: m_RawPointer(other.m_RawPointer)
			, m_ReferenceCount(other.m_ReferenceCount)
		{
			other.m_RawPointer = nullptr;
			other.m_ReferenceCount = nullptr;
		}

		//! Deinitializes this shared pointer.
		//! If decremeneted reference counter values is 0, than object would be deallocated.
		GDINL ~SharedPtr()
		{
			if (m_ReferenceCount != nullptr)
				if (--*m_ReferenceCount == 0)
				{
					delete m_RawPointer;
					delete m_ReferenceCount;
				}
		}

	public /*Class API*/:
		//! Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return m_RawPointer;
		}

		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const _pointer)
		{
			this->~SharedPtr();
			this->m_RawPointer = _pointer;
			m_ReferenceCount = new UInt32(1);
			return _pointer;
		}

	public:

		GDINL SharedPtr& operator= (SharedPtr const& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~SharedPtr();
				m_RawPointer = OtherPtr.m_RawPointer;
				m_ReferenceCount = OtherPtr.m_ReferenceCount;
				if (m_ReferenceCount != nullptr)
					++(*m_ReferenceCount);
			}
			return (*this);
		}
		GDINL SharedPtr& operator= (SharedPtr&& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~SharedPtr();
				m_RawPointer = OtherPtr.m_RawPointer;
				m_ReferenceCount = OtherPtr.m_ReferenceCount;
				OtherPtr.m_RawPointer = nullptr;
				OtherPtr.m_ReferenceCount = nullptr;
			}

			return (*this);
		}
		GDINL SharedPtr& operator= (PointerType* const other)
		{
			if (m_RawPointer != other)
			{
				this->~SharedPtr();
				m_RawPointer = m_RawPointer;
				m_ReferenceCount = ((m_RawPointer != nullptr) ? new UInt32(1) : nullptr);
			}

			return (*this);
		}

		GDINL bool operator== (PointerType   const* const OtherPointer) const 
		{ 
			return (GetPointer() == OtherPointer); 
		}
		GDINL bool operator== (SharedPtr<PointerType> const& OtherPointer) const 
		{ 
			return (GetPointer() == OtherPointer.GetPointer()); 
		}
		
		GDINL bool operator!= (PointerType   const* const OtherPointer) const 
		{ 
			return !(*this == OtherPointer); 
		}
		GDINL bool operator!= (SharedPtr<PointerType> const& OtherPointer) const 
		{ 
			return !(*this == OtherPointer); 
		}

		GDINL PointerType& operator*  () const 
		{ 
			return (*GetPointer()); 
		}
		GDINL PointerType* operator-> () const 
		{ 
			return (GetPointer()); 
		}

	};	// struct SharedPtr<PointerType, GD_SHARED_PTR_FAST>

	template<typename PointerType>
	struct SharedPtr<PointerType, GD_SHARED_PTR_THREAD_SAFE> final
	{
	private:
		PointerType   * pointer = nullptr;
		Int32 volatile* ReferenceCount = nullptr;

	public /*Constructors/Destructor*/:
		//! Initializes shared pointer with raw pointer value.
		//! Initial reference count is 1.
		GDINL SharedPtr(PointerType* const pointer = nullptr)
			: pointer(pointer)
			, ReferenceCount(new Int32(1))
		{
		}

		//! Initializes shared pointer with other shared pointer.
		//! Reference counter is incremeneted.
		GDINL SharedPtr(SharedPtr const& other)
			: pointer(other.pointer)
			, ReferenceCount(other.ReferenceCount)
		{
			if (ReferenceCount != nullptr)
				AtomicsIntrinsics::InterlockedIncrement(ReferenceCount);
		}

		//! Moves other shared pointer to this object.
		//! Reference counter is not incremeneted.
		GDINL SharedPtr(SharedPtr&& other)
			: pointer(other.pointer)
			, ReferenceCount(other.ReferenceCount)
		{
			other.pointer = nullptr;
			other.ReferenceCount = nullptr;
		}

		//! Deinitializes this shared pointer.
		//! If decremeneted reference counter values is 0, than object would be deallocated.
		GDINL ~SharedPtr()
		{
			if (ReferenceCount != nullptr)
				if (AtomicsIntrinsics::InterlockedDecrement(ReferenceCount) == 0)
				{
					delete pointer;
					delete ReferenceCount;
				}
		}

	public /*Class API*/:
		//! Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return pointer;
		}

		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const pointer)
		{
			~SharedPtr();
			pointer = pointer;
			ReferenceCount = new UInt32(1);
			return pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return (*GetPointer()); }
		GDINL PointerType* operator-> () const { return (GetPointer()); }

		GDINL bool operator== (PointerType   const* const OtherPointer) const { return (GetPointer() == OtherPointer); }
		GDINL bool operator== (SharedPtr<PointerType> const&    OtherPointer) const { return (GetPointer() == OtherPointer.GetPointer()); }

		GDINL bool operator!= (PointerType   const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (SharedPtr<PointerType> const&    OtherPointer) const { return !(*this == OtherPointer); }

		GDINL SharedPtr& operator= (SharedPtr const& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				~SharedPtr();
				pointer = OtherPtr.pointer;
				ReferenceCount = OtherPtr.ReferenceCount;
				if (ReferenceCount != nullptr)
					AtomicsIntrinsics::InterlockedIncrement(ReferenceCount);
			}
			return (*this);
		}

		GDINL SharedPtr& operator= (SharedPtr&& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				~SharedPtr();
				pointer = OtherPtr.pointer;
				ReferenceCount = OtherPtr.ReferenceCount;
				OtherPtr.pointer = nullptr;
				OtherPtr.ReferenceCount = nullptr;
			}

			return (*this);
		}

		GDINL SharedPtr& operator= (PointerType* const other)
		{
			if (pointer != other)
			{
				~SharedPtr();
				pointer = other;
				ReferenceCount = new Int32(1);
			}

			return (*this);
		}

	private:
		GDINL friend void Swap(SharedPtr& lhs, SharedPtr& rhs)
		{
			using GD::Swap;
			Swap(lhs.pointer, rhs.pointer);
			Swap(lhs.ReferenceCount, rhs.ReferenceCount);
		}
	};	// struct SharedPtr<PointerType, GD_SHARED_PTR_THREAD_SAFE>

	template<typename Tp, typename... Args>
	GDINL SharedPtr<Tp> MakeShared(Args&&... args)
	{
		return SharedPtr<Tp>(GD_NEW(Tp, args...));
	}

GD_NAMESPACE_END
