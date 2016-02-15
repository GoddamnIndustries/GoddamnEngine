// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/RefPtr.h
 * Smart pointer for reference-countable classes. 
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Base.h>

GD_NAMESPACE_BEGIN

	GD_OBJECT_HELPER struct RefPtrTarget : public IVirtuallyDestructible
	{
	};	// struct RefPtrTarget

	// **------------------------------------------------------------------------------------------**
	//! Helper class that implements a reference-counting pattern for objects.
	// **------------------------------------------------------------------------------------------**
	template<typename TObject>
	GD_OBJECT_HELPER struct RefPtr final
	{
	private:
		TObject* const m_ObjectPointer;

	public:
		
		/*!
		 * Initializes an empty reference-counted pointer.
		 */
		GDINL RefPtr() 
			: m_ObjectPointer(nullptr)
		{
		}

		/*!
		 * Initializes an reference-counted pointer with specified raw pointer.
		 * @param objectPointer Raw pointer.
		 */
		//! @{
		GDINL implicit RefPtr(nullptr_t)
			: m_ObjectPointer(nullptr)
		{
		}
		GDINL implicit RefPtr(TObject* const objectPointer)
			: m_ObjectPointer(objectPointer)
		{
		}
		template<typename TOtherObject>
		GDINL implicit RefPtr(TOtherObject* const objectPointer) 
			: m_ObjectPointer(object_cast<TObject*>(objectPointer))
		{
		}
		//! @}

		/*!
		 * Initializes an reference-counted pointer with copy of the other pointer.
		 * @param otherRefPtr Other automated pointer.
		 */
		//! @{
		GDINL implicit RefPtr(RefPtr<TObject> const& otherRefPtr)
			: m_ObjectPointer(otherRefPtr.m_ObjectPointer)
		{
			if (m_ObjectPointer != nullptr)
			{
				m_ObjectPointer->AddRef();
			}
		}
		template<typename TOtherObject>
		GDINL implicit RefPtr(RefPtr<TOtherObject> const& otherRefPtr)
			: m_ObjectPointer(object_cast<TObject*>(otherRefPtr.m_RawPointer))
		{
			if (m_ObjectPointer != nullptr)
			{
				m_ObjectPointer->AddRef();
			}
		}
		//! @}

		GDINL ~RefPtr()
		{
			if (m_ObjectPointer != nullptr)
			{
				m_ObjectPointer->Release();
			}
		}

	public:

		/*!
		 * Returns reference to the stored object.
		 */
		GDINL TObject* Get() const
		{
			return m_ObjectPointer;
		}

	public:
		GDINL RefPtr<TObject>& operator= (nullptr_t)
		{
			if (m_ObjectPointer != nullptr)
			{
				m_ObjectPointer->Release();
			}
			m_ObjectPointer = nullptr;
			return *this;
		}

		template<typename TOtherObject>
		GDINL RefPtr<TObject>& operator= (TOtherObject* const objectPointer)
		{
			if (m_ObjectPointer != nullptr) 
			{
				m_ObjectPointer->Release();
			}

			m_ObjectPointer = object_cast<TObject*>(objectPointer);

			if (m_ObjectPointer != nullptr)
			{
				m_ObjectPointer->AddRef();
			}
			return *this;
		}
		template<typename TOtherObject>
		GDINL RefPtr<TObject>& operator= (RefPtr<TOtherObject> const& otherRefPtr)
		{
			*this = otherRefPtr.m_RawPointer;
			return *this;
		}

		GDINL bool operator== (nullptr_t) const
		{
			return m_ObjectPointer == nullptr;
		}
		GDINL bool operator!= (nullptr_t) const
		{
			return m_ObjectPointer != nullptr;
		}
		template<typename TOtherObject>
		GDINL bool operator== (TOtherObject* const objectPointer) const
		{
			return m_ObjectPointer == objectPointer;
		}
		template<typename TOtherObject>
		GDINL bool operator!= (TOtherObject* const objectPointer) const
		{
			return m_ObjectPointer != objectPointer;
		}
		template<typename TOtherObject>
		GDINL bool operator== (RefPtr<TOtherObject> const& otherRefPtr) const
		{
			return m_ObjectPointer == otherRefPtr.m_RawPointer;
		}
		template<typename TOtherObject>
		GDINL bool operator!= (RefPtr<TOtherObject> const& otherRefPtr) const
		{
			return m_ObjectPointer != otherRefPtr.m_RawPointer;
		}

		GDINL TObject* operator-> () const
		{
			return m_ObjectPointer;
		}
		GDINL TObject& operator* () const
		{
			return *m_ObjectPointer;
		}

		template<typename TOtherObject>
		GDINL explicit operator RefPtr<TOtherObject>() const
		{
			return static_cast<TOtherObject*>(m_ObjectPointer);
		}
	};	// struct RefPtr

GD_NAMESPACE_END
