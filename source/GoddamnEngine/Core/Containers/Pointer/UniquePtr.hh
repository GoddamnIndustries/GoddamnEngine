//////////////////////////////////////////////////////////////////////////
/// UniquePtr.hh - unique smart pointer interface/implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_UNIQUE_PTR
#define GD_CORE_CONTAINERS_UNIQUE_PTR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Utility.hh>

GD_NAMESPACE_BEGIN

	/// Stores a pointer on object and provides automatic object deletion on destructor.
	template<typename PointerType>
	class UniquePtr final
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
			OtherPtr.Pointer  = nullptr; 
		}

		GDINL ~UniquePtr()
		{ 
			delete self->Pointer; 
			self->Pointer = nullptr; 
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
			self->~UniquePtr();
			return (self->Pointer = Pointer);
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = self->GetPointer();
			self->Pointer = nullptr;
			return Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return  (*self->GetPointer()); }
		GDINL PointerType* operator-> () const { return  ( self->GetPointer()); }

		GDINL bool operator== (          PointerType  const* const OtherPointer) const { return (self->GetPointer() == OtherPointer); }
		GDINL bool operator== (UniquePtr<PointerType> const&       OtherPointer) const { return (self->GetPointer() == OtherPointer.GetPointer()); }
		
		GDINL bool operator!= (          PointerType  const* const OtherPointer) const { return !(*self == OtherPointer); }
		GDINL bool operator!= (UniquePtr<PointerType> const&       OtherPointer) const { return !(*self == OtherPointer); }

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;

		GDINL UniquePtr& operator= (UniquePtr&& OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~UniquePtr();
				self->Pointer = OtherPtr.Pointer;
				OtherPtr.Pointer = nullptr;
			}

			return (*self);
		}

		GDINL UniquePtr& operator= (PointerType* const Other)
		{
			if (self->Pointer != Other)
			{
				self->~UniquePtr();
				self->Pointer = Other;
			}

			return (*self);
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
			delete[] self->Pointer;
			self->Pointer = nullptr;
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
			self->~UniquePtr();
			return (self->Pointer = Pointer);
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = self->GetPointer();
			self->Pointer = nullptr;
			return Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return  (*self->GetPointer()); }
		GDINL PointerType* operator-> () const { return  (self->GetPointer()); }

		GDINL bool operator== (          PointerType  const* const OtherPointer) const { return (self->GetPointer() == OtherPointer); }
		GDINL bool operator== (UniquePtr<PointerType> const&       OtherPointer) const { return (self->GetPointer() == OtherPointer.GetPointer()); }
		
		GDINL bool operator!= (          PointerType  const* const OtherPointer) const { return !(*self == OtherPointer); }
		GDINL bool operator!= (UniquePtr<PointerType> const&       OtherPointer) const { return !(*self == OtherPointer); }

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;

		GDINL UniquePtr& operator= (UniquePtr&& OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~UniquePtr();
				self->Pointer = OtherPtr.Pointer;
				OtherPtr.Pointer = nullptr;
			}

			return (*self);
		}

		GDINL UniquePtr& operator= (PointerType* const Other)
		{
			if (self->Pointer != Other)
			{
				self->~UniquePtr();
				self->Pointer = Other;
			}

			return (*self);
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
