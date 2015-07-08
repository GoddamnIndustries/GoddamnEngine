// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Templates/UniquePtr.h
//! Unique smart pointer interface/implementation.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>

GD_NAMESPACE_BEGIN

	//! Stores a pointer on object and provides automatic object deletion on destructor.
	//! @tparam PointerType Type of the object stored in this object.
	template<typename PointerType>
	class UniquePtr final : IUncopiable
	{
	private:
		PointerType* Pointer = nullptr;

	public:

		//! Initializes a unique pointer.
		//! @param Pointer Raw pointer to the object.
		GDINL UniquePtr(PointerType* const Pointer = nullptr) 
			: Pointer(Pointer)
		{
		}

		//! Moves other unique pointer to this object.
		//! @param Other Other unique pointer to move here.
		GDINL UniquePtr(UniquePtr&& Other) 
			: Pointer(Other.Pointer)
		{ 
			Other.Pointer = nullptr;
		}

		//! Deinitializes a unique pointer and destroys it's value.
		GDINL ~UniquePtr()
		{ 
			delete this->Pointer; 
			this->Pointer = nullptr; 
		}

		//! Returns native pointer stored in this object.
		//! @returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const 
		{ 
			return this->Pointer; 
		}

		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @param Pointer New pointer to assign.
		//! @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			this->~UniquePtr();
			this->Pointer = Pointer;
			return this->Pointer;
		}

		//! Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		//! @returns Value of the pointer.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = this->GetPointer();
			this->Pointer = nullptr;
			return Pointer;
		}

		GDINL UniquePtr& operator= (UniquePtr const& OtherPtr) = delete;

		//! Assigns other unique pointer here.
		//! @param Other Other pointer to be assigned.
		//! @returns Self.
		GDINL UniquePtr& operator= (UniquePtr&& Other)
		{
			if (this != &Other) {
				this->~UniquePtr();
				this->Pointer = Other.Pointer;
				Other.Pointer = nullptr;
			}

			return *this;
		}

		//! Assigns other raw pointer here.
		//! @param Other Other pointer to be assigned.
		//! @returns Self.
		GDINL UniquePtr& operator= (PointerType* const Other)
		{
			if (this->Pointer != Other) {
				this->~UniquePtr();
				this->Pointer = Other;
			}

			return *this;
		}

		//! References value of the pointer.
		//! @returns Referenced value of the pointer.
		GDINL PointerType& operator* () const 
		{ 
			return *this->Pointer;
		}

		//! Dereferences value of the pointer.
		//! @returns Dereferenced value of the pointer.
		GDINL PointerType* operator-> () const 
		{ 
			return this->Pointer;
		}

		GDINL bool operator== (PointerType const* const OtherPointer) const { return (this->GetPointer() == OtherPointer); }
		GDINL bool operator== (UniquePtr<PointerType> const& OtherPointer) const { return (this->GetPointer() == OtherPointer.GetPointer()); }
		
		GDINL bool operator!= (PointerType const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (UniquePtr<PointerType> const& OtherPointer) const { return !(*this == OtherPointer); }

	private:
		template<typename OtherPointerType>
		GDINL friend void Swap(UniquePtr<OtherPointerType>& First, UniquePtr<OtherPointerType>& Second);
	};	// class UniquePtr 

	template<typename PointerType>
	class UniquePtr<PointerType[]> final : IUncopiable
	{	
	private /*Class members*/:
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
		//! Returns native pointer stored in this object.
		GDINL PointerType* GetPointer() const
		{
			return this->Pointer;
		}

		//! Deletes pointer, stored in this object and assigns it new specified value.
		//! @returns New specified pointer.
		GDINL PointerType* Reset(PointerType* const Pointer)
		{
			this->~UniquePtr();
			return (this->Pointer = Pointer);
		}

		//! Releases ownership on this pointer, by returning it`s value and replacing it with nullptr.
		GDINL PointerType* Release()
		{
			PointerType* const Pointer = this->GetPointer();
			this->Pointer = nullptr;
			return Pointer;
		}

	public /*Operators*/:
		GDINL PointerType& operator*  () const { return  (*this->GetPointer()); }
		GDINL PointerType* operator-> () const { return  (this->GetPointer()); }

		GDINL bool operator== (    PointerType  const* const OtherPointer) const { return (this->GetPointer() == OtherPointer); }
		GDINL bool operator== (UniquePtr<PointerType> const&    OtherPointer) const { return (this->GetPointer() == OtherPointer.GetPointer()); }
		
		GDINL bool operator!= (    PointerType  const* const OtherPointer) const { return !(*this == OtherPointer); }
		GDINL bool operator!= (UniquePtr<PointerType> const&    OtherPointer) const { return !(*this == OtherPointer); }

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
		template<typename OtherPointerType>
		GDINL friend void Swap(UniquePtr<OtherPointerType>& First, UniquePtr<OtherPointerType>& Second);
	};	// class UniquePtr<Type[]>

	template<typename OtherPointerType>
	GDINL void Swap(UniquePtr<OtherPointerType>& First, UniquePtr<OtherPointerType>& Second)
	{
		Swap(First.Pointer, Second.Pointer);
	}

GD_NAMESPACE_END
