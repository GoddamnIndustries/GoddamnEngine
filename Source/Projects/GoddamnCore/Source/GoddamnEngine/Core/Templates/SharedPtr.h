// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Templates/SharedPtr.h
 * File contains smart pointer for reference-countable classes. 
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Implements base non-thread safe reference counting.
	// **------------------------------------------------------------------------------------------**
	template<typename TCounter = UInt32>
	struct TReferenceTarget : IVirtuallyDestructible
	{
	private:
		TCounter m_ReferenceCount = 1;

	public:

		/*!
		 * @brief Increments reference counter for this object.
		 * This method should be called for each copy of the pointer to the object.
		 */
		GDINL void AddRef()
		{
			++m_ReferenceCount;
		}

		/*!
		 * @brief Decrements reference counter for this object.
		 * When reference counter reaches zero, object is recycled.
		 * This method should be called for each copy of the pointer to the object.
		 */
		GDINL void Release()
		{
			--m_ReferenceCount; 
			if (m_ReferenceCount == 0)
			{
				gd_delete this;
			}
		}
	};	// struct ReferenceTarget
	using ReferenceTarget = TReferenceTarget<>;

	GD_HAS_MEMBER_FUNCTION(AddRef);
	GD_HAS_MEMBER_FUNCTION(Release);

	/*!
	 * Increments reference counter for this object (if it is non-null).
	 */
	template<typename TReferenceTargetPtr>
	GDINL static void SafeAddRef(TReferenceTargetPtr const& refTarget)
	{
		if (refTarget != nullptr)
		{
			refTarget->AddRef();
		}
	}

	/*!
	 * Decrements reference counter for this object (if it is non-null).
	 */
	template<typename TReferenceTargetPtr>
	GDINL static void SafeRelease(TReferenceTargetPtr const& refTarget)
	{
		if (refTarget != nullptr)
		{
			refTarget->Release();
		}
	}

	// **------------------------------------------------------------------------------------------**
	//! Implements casting operations for shared pointers.
	// **------------------------------------------------------------------------------------------**
	struct SharedPtrCastOperation final : public TNonCreatable
	{
		/*!
		 * Casts to types using static_cast.
		 */
		template<typename TCastTo, typename TCastFrom>
		GDINL static TCastTo Cast(TCastFrom castFrom)
		{
			return static_cast<TCastTo>(castFrom);
		}
	};	// struct SharedPtrCastOperation

	// **------------------------------------------------------------------------------------------**
	//! Helper class that implements a reference-counting pattern for objects.
	// **------------------------------------------------------------------------------------------**
	template<typename TPointee, typename TCast = SharedPtrCastOperation>
	struct SharedPtr final
	{
		template<typename, typename>
		friend struct SharedPtr;

		static_assert(TypeTraits::HasMemberFunction_AddRef<TPointee>::Value, "Target type for shared pointers should be contain 'AddRef' function.");
		static_assert(TypeTraits::HasMemberFunction_Release<TPointee>::Value, "Target type for shared pointers should be contain 'Release' function.");

	public:
		using PointeeType = TPointee;
		using CastType = TCast;

	private:
		TPointee* m_RawPointer;

	public:

		/*!
		 * Initializes an empty reference-counted shared pointer.
		 */
		GDINL SharedPtr()
			: m_RawPointer(nullptr)
		{
		}

		/*!
		 * Initializes the shared reference-counted pointer with specified raw pointer.
		 * @param rawPointer Raw pointer.
		 */
		//! @{
		GDINL implicit SharedPtr(nullptr_t)
			: m_RawPointer(nullptr)
		{
		}
		GDINL implicit SharedPtr(TPointee* const rawPointer)
			: m_RawPointer(rawPointer)
		{
		}
		template<typename TOtherPointee>
		GDINL implicit SharedPtr(TOtherPointee* const rawPointer)
			: m_RawPointer(TCast::template Cast<TPointee*>(rawPointer))
		{
		}
		//! @}

		/*!
		 * Initializes an reference-counted pointer with copy of the other pointer.
		 * @param otherSharedPtr Other automated pointer.
		 */
		//! @{
		GDINL implicit SharedPtr(SharedPtr const& otherSharedPtr)
			: m_RawPointer(otherSharedPtr.Get())
		{
			GD::SafeAddRef(m_RawPointer);
		}
		template<typename TOtherPointee>
		GDINL implicit SharedPtr(SharedPtr<TOtherPointee, TCast> const& otherSharedPtr)
			: m_RawPointer(TCast::template Cast<TPointee*>(otherSharedPtr.Get()))
		{
			GD::SafeAddRef(m_RawPointer);
		}
		//! @}

		/*!
		 * Moves other shared pointer to this object.
		 * @param other The other shared pointer to move here.
		 */
		GDINL SharedPtr(SharedPtr&& other) noexcept
			: m_RawPointer(other.m_RawPointer)
		{ 
			other.m_RawPointer = nullptr;
		}

		GDINL ~SharedPtr()
		{
			GD::SafeRelease(m_RawPointer);
		}

	public:

		/*!
		 * Returns reference to the stored raw pointer.
		 */
		GDINL TPointee* Get() const
		{
			return m_RawPointer;
		}

	public:

		// shared_ptr = ...
		GDINL SharedPtr& operator= (TPointee* const rawPointer)
		{
			GD::SafeRelease(m_RawPointer);
			m_RawPointer = rawPointer;
			GD::SafeAddRef(m_RawPointer);

			return *this;
		}
		GDINL SharedPtr& operator= (SharedPtr&& other) noexcept
		{
			if (this != &other)
			{
				GD::SafeRelease(m_RawPointer);
				m_RawPointer = other.m_RawPointer;
				other.m_RawPointer = nullptr;
			}
			return *this;
		}
		GDINL SharedPtr& operator= (SharedPtr const& otherSharedPtr)
		{
			*this = otherSharedPtr.m_RawPointer;
			return *this;
		}

		// shared_ptr == nullptr
		GDINL bool operator== (nullptr_t) const
		{
			return m_RawPointer == nullptr;
		}
		GDINL bool operator!= (nullptr_t) const
		{
			return m_RawPointer != nullptr;
		}

		// shared_ptr == T*
		template<typename TOtherPointee>
		GDINL bool operator== (TOtherPointee* const rawPointer) const
		{
			return m_RawPointer == rawPointer;
		}
		template<typename TOtherPointee>
		GDINL bool operator!= (TOtherPointee* const rawPointer) const
		{
			return m_RawPointer != rawPointer;
		}

		// shared_ptr == shared_ptr
		template<typename TOtherPointee>
		GDINL bool operator== (SharedPtr<TOtherPointee, TCast> const& otherSharedPtr) const
		{
			return m_RawPointer == otherSharedPtr.m_RawPointer;
		}
		template<typename TOtherPointee>
		GDINL bool operator!= (SharedPtr<TOtherPointee, TCast> const& otherSharedPtr) const
		{
			return m_RawPointer != otherSharedPtr.m_RawPointer;
		}

		// shared_ptr->
		GDINL TPointee* operator-> () const
		{
			return m_RawPointer;
		}
		GDINL TPointee& operator* () const
		{
			return *m_RawPointer;
		}

		// (shared_ptr<U>)shared_ptr<T>
		template<typename TOtherPointee>
		GDINL explicit operator SharedPtr<TOtherPointee, TCast>() const
		{
			static_assert(TypeTraits::IsBase<TOtherPointee, TPointee>::Value || TypeTraits::IsBase<TPointee, TOtherPointee>::Value, "Cast types should be related.");
			return TCast::template Cast<TOtherPointee*>(m_RawPointer);
		}
		GDINL operator SharedPtr<TPointee const, TCast>() const
		{
			return m_RawPointer;
		}

	};	// struct SharedPtr<TPointee>

GD_NAMESPACE_END
