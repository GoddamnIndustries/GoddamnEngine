//////////////////////////////////////////////////////////////////////////
/// RefPtr.hh - unique reference-contable pointer interface/implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 13.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_REF_PTR
#define GD_CORE_CONTAINERS_REF_PTR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Object/Object.hh>

GD_NAMESPACE_BEGIN

	/// Provides automatic reference counting on object-derived objects.
	template<typename ObjectType, typename = typename EnableIf<TypeTraits::IsBaseType<Object, ObjectType>::Value>::Type>
	class RefPtr final
	{
	private /* Class members */:
		ObjectType* Pointer;

	public /* Constructors / Destructors. */:
		/// Initializes smart pointer with existing object.
		/// Reference counter is not incremented.
		GDINL explicit RefPtr(ObjectType* const Pointer = nullptr)
			: Pointer(Pointer)
		{
		}

		/// Initializes smart pointer with other pointer.
		/// Reference counter is incremented.
		GDINL RefPtr(RefPtr const& Other)
			: Pointer(Other.Pointer)
		{
			SafeObtain(self->Pointer);
		}

		/// Moves other smart pointer to this new object.
		/// Reference counter is not incremented.
		GDINL RefPtr(RefPtr&& Other)
			: Pointer(Other.Pointer)
		{
			Other.Pointer = nullptr;
		}

		GDINL ~RefPtr()
		{
			SafeRelease(self->Pointer);
		}

	public /*Class API*/:
		/// Returns native pointer stored in this object.
		GDINL ObjectType* GetPointer() const
		{
			return self->Pointer;
		}

		/// Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL ObjectType* Release()
		{
			ObjectType* Pointer = self->Pointer;
			self->Pointer = nullptr;
			return Pointer;
		}

	public /*Operators*/:
		GDINL ObjectType& operator*  () const { return  (*self->GetPointer()); }
		GDINL ObjectType* operator-> () const { return  ( self->GetPointer()); }

		GDINL bool operator== (       ObjectType  const* const OtherPointer) const { return (self->GetPointer() == OtherPointer); }
		GDINL bool operator== (RefPtr<ObjectType> const&       OtherPointer) const { return (self->GetPointer() == OtherPointer.GetPointer()); }

		GDINL bool operator!= (       ObjectType  const* const OtherPointer) const { return !(*self == OtherPointer); }
		GDINL bool operator!= (RefPtr<ObjectType> const&       OtherPointer) const { return !(*self == OtherPointer); }

		GDINL RefPtr& operator= (RefPtr&& Other)
		{
			if ((&OtherPtr) != self)
			{
				SafeRelease(self->Pointer);
				self->Pointer = Other.Pointer;
				Other.Pointer = nullptr;
			}

			return (*self);
		}

		GDINL RefPtr& operator= (RefPtr const& Other)
		{
			if ((&Other) != self)
			{
				SafeRelease(self->Pointer);
				SafeObtain(self->Pointer = Other.Pointer);
			}

			return (*self);
		}

		GDINL RefPtr& operator= (ObjectType* const Other)
		{
			if (self->Pointer != Other)
			{
				SafeRelease(self->Pointer);
				SafeObtain(self->Pointer = Other);
			}

			return (*self);
		}


	private:
		GDINL friend void Swap(RefPtr& First, RefPtr& Second)
		{	using GD Swap;
			Swap(First.Pointer, Second.Pointer);
		}
	};	// class RefPtr

#if (!defined(GD_DOCUMENTATION))
	template<typename ObjectType, typename ThisObjectType>
	auto IterateChildObjects(RefPtr<ThisObjectType> const& TheObject) -> decltype(IterateChildObjects<ObjectType>(TheObject.GetPointer()))
	{	// Returning object wrapped with iteration adapter.
		return IterateChildObjects<ObjectType>(TheObject.GetPointer());
	}
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_END

#endif
