//////////////////////////////////////////////////////////////////////////
/// Pointer.hh - smart pointers interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_POINTER
#define GD_CORE_CONTAINERS_POINTER

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Utility.hh>

GD_NAMESPACE_BEGIN

	/// Stores a pointer on object and provides automatic object deletion on destructor.
	template<typename PointerType>
	class UniquePtr final
	{
	private:
		GD_CLASS_UNCOPIABLE(UniquePtr);
		PointerType* Pointer = nullptr;

	public:
		GDINL  UniquePtr(PointerType* const Pointer = nullptr) : Pointer(Pointer)          { }
		GDINL  UniquePtr(UniquePtr       && OtherPtr         ) : Pointer(OtherPtr.Pointer) { OtherPtr.Pointer  = nullptr; }
		GDINL ~UniquePtr(                                    )                             { delete self->Pointer; self->Pointer = nullptr; }

		/// Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const 
		{ 
			return self->Pointer; 
		}

		/// Deletes pointer, stored in this object and assigns it new specified value.
		/// @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			delete (self->Pointer);
			return (self->Pointer = Pointer);
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = self->GetPointer();
			self ->Pointer = nullptr;
			return Pointer;
		}

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;
		GDINL UniquePtr& operator= (UniquePtr     && OtherPtr)
		{ 
			if ((&OtherPtr) != self)
			{
				self->~UniquePtr();
				self->Pointer = OtherPtr.Pointer; 
				OtherPtr.Pointer = nullptr; 
			}

			return *self;
		}

		GDINL PointerType& operator*  (                                     ) const { return  (*self->GetPointer()); }
		GDINL PointerType* operator-> (                                     ) const { return  ( self->GetPointer()); }
		GDINL bool         operator== (PointerType const* const OtherPointer) const { return  ( self->GetPointer() == OtherPointer); }
		GDINL bool         operator!= (PointerType const* const OtherPointer) const { return !(*self               == OtherPointer); }

	private:
		GDINL friend void Swap(UniquePtr& First, UniquePtr& Second)
		{	using GD Swap;
			Swap(First.Pointer, Second.Pointer);
		}
	};	// UniquePtr 

	template<typename PointerType>
	class UniquePtr  <PointerType[]> final
	{	
	private:
		GD_CLASS_UNCOPIABLE(UniquePtr);
		PointerType* Pointer = nullptr;

	public:
		GDINL  UniquePtr(PointerType* const Pointer = nullptr) : Pointer(Pointer)          { }
		GDINL  UniquePtr(UniquePtr       && OtherPtr         ) : Pointer(OtherPtr.Pointer) { OtherPtr.Pointer = nullptr; }
		GDINL ~UniquePtr()                                                                 { delete[] self->Pointer; self->Pointer = nullptr; }

		/// Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return self->Pointer;
		}

		/// Deletes pointer, stored in this object and assigns it new specified value.
		/// @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			delete[] (self->Pointer);
			return   (self->Pointer = Pointer);
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = self->GetPointer();
			self->Pointer = nullptr;
			return Pointer;
		}

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;
		GDINL UniquePtr& operator= (UniquePtr     && OtherPtr)
		{
			if ((&OtherPtr) != self)
			{
				self->~UniquePtr();
				self->Pointer = OtherPtr.Pointer;
				OtherPtr.Pointer = nullptr;
			}

			return *self;
		}

		GDINL PointerType& operator*  () const { return  (*self->GetPointer()); }
		GDINL PointerType* operator-> () const { return  (self->GetPointer()); }
		GDINL bool         operator== (PointerType const* const OtherPointer) const { return  (self->GetPointer() == OtherPointer); }
		GDINL bool         operator!= (PointerType const* const OtherPointer) const { return !(*self == OtherPointer); }

	private:
		GDINL friend void Swap(UniquePtr& First, UniquePtr& Second)
		{
			using GD Swap;
			Swap(First.Pointer, Second.Pointer);
		}
	};	//class UniquePtr[]

GD_NAMESPACE_END

#endif
