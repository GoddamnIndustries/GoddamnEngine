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
	//! Implements base reference counting.
	// **------------------------------------------------------------------------------------------**
	struct ReferenceTarget : IVirtuallyDestructible
	{
	private:
		UInt32 m_ReferenceCount = 1;

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

	// **------------------------------------------------------------------------------------------**
	//! Implements casting operations for shared pointers.
	// **------------------------------------------------------------------------------------------**
	struct SharedPtrCastOperation final : public TNonCreatable
	{
		/*!
		 * Casts to POD objects using union.
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
		GDINL implicit SharedPtr(SharedPtr<TPointee> const& otherSharedPtr)
			: m_RawPointer(otherSharedPtr.Get())
		{
			if (m_RawPointer != nullptr)
			{
				m_RawPointer->AddRef();
			}
		}
		template<typename TOtherPointee>
		GDINL implicit SharedPtr(SharedPtr<TOtherPointee> const& otherSharedPtr)
			: m_RawPointer(TCast::template Cast<TPointee*>(otherSharedPtr.Get()))
		{
			if (m_RawPointer != nullptr)
			{
				m_RawPointer->AddRef();
			}
		}
		//! @}

		GDINL ~SharedPtr()
		{
			if (m_RawPointer != nullptr)
			{
				m_RawPointer->Release();
			}
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
		GDINL SharedPtr& operator= (TPointee* const rawPointer)
		{
			if (m_RawPointer != nullptr)
			{
				m_RawPointer->Release();
			}

			m_RawPointer = rawPointer;

			if (m_RawPointer != nullptr)
			{
				m_RawPointer->AddRef();
			}
			return *this;
		}
		GDINL SharedPtr<TPointee>& operator= (SharedPtr const& otherSharedPtr)
		{
			*this = otherSharedPtr.m_RawPointer;
			return *this;
		}

		GDINL bool operator== (nullptr_t) const
		{
			return m_RawPointer == nullptr;
		}
		GDINL bool operator!= (nullptr_t) const
		{
			return m_RawPointer != nullptr;
		}

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

		template<typename TOtherPointee>
		GDINL bool operator== (SharedPtr<TOtherPointee> const& otherSharedPtr) const
		{
			return m_RawPointer == otherSharedPtr.m_RawPointer;
		}
		template<typename TOtherPointee>
		GDINL bool operator!= (SharedPtr<TOtherPointee> const& otherSharedPtr) const
		{
			return m_RawPointer != otherSharedPtr.m_RawPointer;
		}

		GDINL TPointee* operator-> () const
		{
			return m_RawPointer;
		}
		GDINL TPointee& operator* () const
		{
			return *m_RawPointer;
		}

		template<typename TOtherPointee>
		GDINL explicit operator SharedPtr<TOtherPointee>() const
		{
			return TCast::template Cast<TOtherPointee*>(m_RawPointer);
		}
	};	// struct SharedPtr<TPointee>

GD_NAMESPACE_END
