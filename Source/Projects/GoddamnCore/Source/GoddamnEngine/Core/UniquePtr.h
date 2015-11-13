// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/UniquePtr.h
 * Unique smart pointer interface/Implementation.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// class UniquePtr<T>
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename TPointee>
	class UniquePtrBase : IUncopiable
	{
	protected:
		GDINL void _Delete(TPointee* const rawPointer)
		{
			GD_DELETE(rawPointer);
		}
	};	// class UniquePtrBase
	template<typename TPointee>
	class UniquePtrBase<TPointee[]> : IUncopiable
	{
	protected:
		GDINL void _Delete(TPointee* const rawPointer)
		{
			GD_DELETE_ARRAY(rawPointer);
		}
	};	// class UniquePtrBase

	// ------------------------------------------------------------------------------------------
	//! Stores a pointer on object and provides automatic object deletion on destructor.
	//! @tparam TPointee Type of the object stored in this object.
	template<typename TPointee>
	class UniquePtr : UniquePtrBase<TPointee>
	{
	public:
		using PointeeType = TPointee;
		using ElementType = PointeeType;

	private:
		TPointee* m_RawPointer = nullptr;

	public:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor and destructor.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Initializes a unique pointer.
		//! @param rawPointer Raw pointer to the object.
		//! @{
		GDINL explicit UniquePtr() : m_RawPointer(nullptr)
		{
		}
		GDINL explicit UniquePtr(nullptr_t const rawPointer) : m_RawPointer(nullptr)
		{
			GD_NOT_USED(rawPointer);
		}
		GDINL explicit UniquePtr(TPointee* const rawPointer) : m_RawPointer(rawPointer)
		{
		}
		//! @}

		// ------------------------------------------------------------------------------------------
		//! Moves other unique pointer to this object.
		//! @param other The other unique pointer to move here.
		GDINL UniquePtr(UniquePtr&& other) : m_RawPointer(other.m_RawPointer)
		{ 
			other.m_RawPointer = nullptr;
		}

		// ------------------------------------------------------------------------------------------
		//! Deinitializes a unique pointer and destroys it's value.
		GDINL ~UniquePtr()
		{ 
			this->_Delete(m_RawPointer); 
			m_RawPointer = nullptr; 
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Raw pointer manipulation.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Returns native pointer stored in this object.
		//! @returns native pointer stored in this object.
		GDINL TPointee* GetPointer() const 
		{ 
			return m_RawPointer; 
		}

		// ------------------------------------------------------------------------------------------
		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @param pointer New pointer to assign.
		//! @returns New specified pointer.
		GDINL TPointee* Reset(TPointee* const newRawPointer)
		{
			this->~UniquePtr();
			m_RawPointer = newRawPointer;
			return m_RawPointer;
		}

		// ------------------------------------------------------------------------------------------
		//! Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		//! @returns Value of the pointer.
		GDINL TPointee* Release()
		{
			TPointee* const previousPointer = GetPointer();
			m_RawPointer = nullptr;
			return previousPointer;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Overloaded operators.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;
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
			return (GetPointer() == other); 
		}
		GDINL bool operator== (UniquePtr<TPointee> const& other) const 
		{ 
			return (GetPointer() == other.GetPointer()); 
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

	//! @todo Document this.
	template<typename TPointee>
	GDINL static UniquePtr<TPointee> MakeUnique(TPointee* const pointer)
	{
		return UniquePtr<TPointee>(pointer);
	}

GD_NAMESPACE_END
