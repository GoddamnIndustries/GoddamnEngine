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
		GDINL void _Delete(TPointee* const rawPointer)
		{
			gd_delete rawPointer;
		}
	};	// class UniquePtrBase

	template<typename TPointee>
	class UniquePtrBase<TPointee[]> : TNonCopyable
	{
	protected:
		GDINL void _Delete(TPointee* const rawPointer)
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
	private:
		TPointee* m_RawPointer = nullptr;

	public:

		/*!
		 * Initializes a unique pointer.
		 * @param rawPointer Raw pointer to the object.
		 */
		//! @{
		GDINL explicit UniquePtr() 
			: m_RawPointer(nullptr)
		{
		}
		GDINL explicit UniquePtr(nullptr_t) 
			: m_RawPointer(nullptr)
		{
		}
		GDINL explicit UniquePtr(TPointee* const rawPointer) 
			: m_RawPointer(rawPointer)
		{
		}
		//! @}

		/*!
		 * Moves other unique pointer to this object.
		 * @param other The other unique pointer to move here.
		 */
		GDINL UniquePtr(UniquePtr&& other) 
			: m_RawPointer(other.m_RawPointer)
		{ 
			other.m_RawPointer = nullptr;
		}

		GDINL ~UniquePtr()
		{ 
			this->_Delete(m_RawPointer); 
			m_RawPointer = nullptr; 
		}

	public:

		/*!
		 * Returns reference to the stored raw pointer.
		 */
		GDINL TPointee* Get() const 
		{ 
			return m_RawPointer; 
		}

		/*!
		 * Deletes pointer, stored in this object and assigns it new specified value.
		 *
		 * @param pointer New pointer to assign.
		 * @returns New specified pointer.
		 */
		GDINL TPointee* Reset(TPointee* const newRawPointer)
		{
			this->~UniquePtr();
			m_RawPointer = newRawPointer;
			return m_RawPointer;
		}

		/*!
		 * Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		 * @returns Value of the pointer.
		 */
		GDINL TPointee* Release()
		{
			TPointee* const previousPointer = Get();
			m_RawPointer = nullptr;
			return previousPointer;
		}

	public:
		GDINL UniquePtr& operator= (UniquePtr&& other)
		{
			if (this != &other) 
			{
				this->~UniquePtr();
				m_RawPointer = other.m_RawPointer;
				other.m_RawPointer = nullptr;
			}
			return *this;
		}
		GDINL UniquePtr& operator= (TPointee* const other)
		{
			if (m_RawPointer != other) 
			{
				this->~UniquePtr();
				m_RawPointer = other;
			}
			return *this;
		}

		GDINL bool operator== (TPointee const* const other) const 
		{ 
			return Get() == other; 
		}
		GDINL bool operator== (UniquePtr<TPointee> const& other) const 
		{ 
			return Get() == other.Get(); 
		}
		
		GDINL bool operator!= (TPointee const* const other) const 
		{ 
			return !(*this == other); 
		}
		GDINL bool operator!= (UniquePtr<TPointee> const& other) const 
		{ 
			return !(*this == other); 
		}

		GDINL TPointee& operator* () const
		{
			return *m_RawPointer;
		}
		GDINL TPointee* operator-> () const
		{
			return m_RawPointer;
		}
	};	// class UniquePtr 

GD_NAMESPACE_END
