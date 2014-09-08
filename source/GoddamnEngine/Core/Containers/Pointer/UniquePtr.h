/// ==========================================================================================
/// UniquePtr.h - unique smart pointer interface/implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.05.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_UNIQUE_PTR
#define GD_CORE_CONTAINERS_UNIQUE_PTR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	/// Stores a pointer on object and provides automatic object deletion on destructor.
	template<typename PointerType>
	class UniquePtr final
	{
	private /*Class members*/:
		GD_CLASS_UNCOPIABLE(UniquePtr);
		PointerType* Pointer = nullptr;

	public /*Constructors / destructor.*/:
		GDINL UniquePtr(PointerType* const Pointer = nullptr) 
			: Pointer(Pointer)
		{
		}

		GDINL UniquePtr(UniquePtr&& OtherPtr) 
			: Pointer(OtherPtr.Pointer) 
		{ 
			OtherPtr.Pointer  = nullptr; 
		}

		GDINL ~UniquePtr()
		{ 
			delete this->Pointer; 
			this->Pointer = nullptr; 
		}

	public /*Class API*/:
		/// Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const 
		{ 
			return this->Pointer; 
		}

		/// Deletes pointer, stored in this object and assigns it new specified value.
		/// @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			this->~UniquePtr();
			return (this->Pointer = Pointer);
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = this->GetPointer();
			this->Pointer = nullptr;
			return Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return  (*this->GetPointer()); }
		GDINL PointerType* operator-> () const { return  ( this->GetPointer()); }

		GDINL bool operator== (          PointerType  const* const OtherPointer) const { return (this->GetPointer() == OtherPointer); }
		GDINL bool operator== (UniquePtr<PointerType> const&       OtherPointer) const { return (this->GetPointer() == OtherPointer.GetPointer()); }
		
		GDINL bool operator!= (          PointerType  const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (UniquePtr<PointerType> const&       OtherPointer) const { return !(*this == OtherPointer); }

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;

		GDINL UniquePtr& operator= (UniquePtr&& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~UniquePtr();
				this->Pointer = OtherPtr.Pointer;
				OtherPtr.Pointer = nullptr;
			}

			return (*this);
		}

		GDINL UniquePtr& operator= (PointerType* const Other)
		{
			if (this->Pointer != Other)
			{
				this->~UniquePtr();
				this->Pointer = Other;
			}

			return (*this);
		}

	private:
		GDINL friend void Swap(UniquePtr& First, UniquePtr& Second)
		{	using GD Swap;
			Swap(First.Pointer, Second.Pointer);
		}
	};	// class UniquePtr 

#if (!defined(GD_DOCUMENTATION))
	template<typename PointerType>
	class UniquePtr<PointerType[]> final
	{	
	private /*Class members*/:
		GD_CLASS_UNCOPIABLE(UniquePtr);
		PointerType* Pointer = nullptr;

	public /*Constructors / destructor.*/:
		GDINL explicit UniquePtr(PointerType* const Pointer = nullptr)
			: Pointer(Pointer)
		{
		}

		GDINL UniquePtr(UniquePtr&& OtherPtr)
			: Pointer(OtherPtr.Pointer)
		{
			OtherPtr.Pointer = nullptr;
		}

		GDINL ~UniquePtr()
		{
			delete[] this->Pointer;
			this->Pointer = nullptr;
		}

	public /*Class API*/:
		/// Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return this->Pointer;
		}

		/// Deletes pointer, stored in this object and assigns it new specified value.
		/// @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			this->~UniquePtr();
			return (this->Pointer = Pointer);
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = this->GetPointer();
			this->Pointer = nullptr;
			return Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return  (*this->GetPointer()); }
		GDINL PointerType* operator-> () const { return  (this->GetPointer()); }

		GDINL bool operator== (          PointerType  const* const OtherPointer) const { return (this->GetPointer() == OtherPointer); }
		GDINL bool operator== (UniquePtr<PointerType> const&       OtherPointer) const { return (this->GetPointer() == OtherPointer.GetPointer()); }
		
		GDINL bool operator!= (          PointerType  const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (UniquePtr<PointerType> const&       OtherPointer) const { return !(*this == OtherPointer); }

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;

		GDINL UniquePtr& operator= (UniquePtr&& OtherPtr)
		{
			if ((&OtherPtr) != this)
			{
				this->~UniquePtr();
				this->Pointer = OtherPtr.Pointer;
				OtherPtr.Pointer = nullptr;
			}

			return (*this);
		}

		GDINL UniquePtr& operator= (PointerType* const Other)
		{
			if (this->Pointer != Other)
			{
				this->~UniquePtr();
				this->Pointer = Other;
			}

			return (*this);
		}

	private:
		GDINL friend void Swap(UniquePtr& First, UniquePtr& Second)
		{
			using GD Swap;
			Swap(First.Pointer, Second.Pointer);
		}
	};	// class UniquePtr<Type[]>
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_END

#endif
