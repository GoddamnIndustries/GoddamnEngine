// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Templates/UniquePtr.h
 * Unique smart pointer interface.
 */
#pragma once

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	template<typename TPointee>
	class UniquePtrBase : TNonCopyable
	{
	protected:
		typedef TPointee TTPointee;
		GDINL static void DeleteSelf(TTPointee* const rawPointer)
		{
			gd_delete rawPointer;
		}
	};	// class UniquePtrBase

	template<typename TPointee>
	class UniquePtrBase<TPointee[]> : TNonCopyable
	{
	protected:
		typedef TPointee TTPointee;
		GDINL static void DeleteSelf(TTPointee* const rawPointer)
		{
			gd_delete[] rawPointer;
		}
	};	// class UniquePtrBase

	// **------------------------------------------------------------------------------------------**
	//! Stores a pointer on object and provides automatic object deletion on destructor.
	// **------------------------------------------------------------------------------------------**
	template<typename TPointee>
	class UniquePtr : UniquePtrBase<TPointee>
	{
		using TBase = UniquePtrBase<TPointee>;
		using TTPointee = typename UniquePtrBase<TPointee>::TTPointee;

	private:
		TTPointee* m_RawPointer = nullptr;

	public:

		/*!
		 * Initializes an empty unique pointer.
		 */
		GDINL explicit UniquePtr()
			: m_RawPointer(nullptr)
		{
		}

		/*!
		 * Initializes a unique pointer.
		 * @param rawPointer Raw pointer to the object.
		 */
		//! @{
		GDINL explicit UniquePtr(nullptr_t) 
			: m_RawPointer(nullptr)
		{
		}
		GDINL explicit UniquePtr(TTPointee* const rawPointer)
			: m_RawPointer(rawPointer)
		{
		}
		//! @}

		/*!
		 * Moves other unique pointer to this object.
		 * @param other The other unique pointer to move here.
		 */
		GDINL UniquePtr(UniquePtr&& other) noexcept
			: m_RawPointer(other.m_RawPointer)
		{ 
			other.m_RawPointer = nullptr;
		}

		GDINL ~UniquePtr()
		{ 
			TBase::DeleteSelf(m_RawPointer);
			m_RawPointer = nullptr; 
		}

	public:

		/*!
		 * Returns reference to the stored raw pointer.
		 */
		GDINL TTPointee* Get() const
		{ 
			return m_RawPointer; 
		}

		/*!
		 * Deletes pointer, stored in this object and assigns it new specified value.
		 *
		 * @param newRawPointer New pointer to assign.
		 * @returns New specified pointer.
		 */
		GDINL TTPointee* Reset(TTPointee* const newRawPointer)
		{
			TBase::DeleteSelf(m_RawPointer);
			m_RawPointer = newRawPointer;
			return m_RawPointer;
		}

		/*!
		 * Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		 * @returns Value of the pointer.
		 */
		GDINL TTPointee* Release()
		{
			auto const previousPointer = Get();
			m_RawPointer = nullptr;
			return previousPointer;
		}

	public:

		// unique_ptr = ...
		GDINL UniquePtr& operator= (UniquePtr&& other) noexcept
		{
			if (this != &other) 
			{
				TBase::DeleteSelf(m_RawPointer);
				m_RawPointer = other.m_RawPointer;
				other.m_RawPointer = nullptr;
			}
			return *this;
		}
		GDINL UniquePtr& operator= (TTPointee* const other)
		{
			if (m_RawPointer != other) 
			{
				TBase::DeleteSelf(m_RawPointer);
				m_RawPointer = other;
			}
			return *this;
		}

		// unique_ptr == nullptr
		GDINL bool operator== (nullptr_t) const
		{ 
			return m_RawPointer == nullptr;
		}
		GDINL bool operator!= (nullptr_t) const
		{ 
			return m_RawPointer != nullptr;
		}

		// unique_ptr->
		GDINL TTPointee& operator* () const
		{
			return *m_RawPointer;
		}
		GDINL TTPointee* operator-> () const
		{
			return m_RawPointer;
		}

	};	// class UniquePtr 

GD_NAMESPACE_END
