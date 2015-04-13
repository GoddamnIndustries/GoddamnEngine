// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/StaticVector.h
//! Statically sized array class.
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/InitializerList.h>

GD_NAMESPACE_BEGIN

	//! @brief Static array implementation.
	//! @tparam ElementType Container element type.
	//! @tparam Capacity Capacity of the static vector.
	template<typename ElementType, SizeTp const Capacity>
	class StaticVector final : public IContainer
		, public ContainerIteratableTag
		, public ContainerReverseIteratableTag
		, public ContainerPtrIteratableTag
		, public ContainerPtrReverseIteratableTag
	{
	public:
		typedef IndexedContainerIterator<StaticVector, ElementType> Iterator;
		typedef IndexedContainerIterator<StaticVector const, ElementType const> ConstIterator;
		GD_CONTAINER_CHECK_ITERATORS(StaticVector);

		typedef ReverseContainerIterator<Iterator> ReverseIterator;
		typedef ReverseContainerIterator<ConstIterator> ReverseConstIterator;
		GD_CONTAINER_CHECK_REVERSE_ITERATORS(StaticVector);

		typedef ElementType* PtrIterator;
		typedef ElementType const* PtrConstIterator;
		GD_CONTAINER_CHECK_PTR_ITERATORS(StaticVector);

		typedef ReverseContainerIterator<PtrIterator> ReversePtrIterator;
		typedef ReverseContainerIterator<PtrConstIterator> ReversePtrConstIterator;
		GD_CONTAINER_CHECK_REVERSE_PTR_ITERATORS(StaticVector);

		GD_CONTAINER_DEFINE_PTR_ITERATION_SUPPORT(StaticVector);

	private:
		UInt8 Memory[sizeof(ElementType) * Capacity];
		SizeTp Length = 0;

	public:

		//! @brief Initializes vector with specified number of allocated and initialized elements.
		//! @param InitialLength Number of elements been initialized.
		//! @param InitialCapacity Initial capacity of vector. 
		GDINL StaticVector(SizeTp const InitialLength = 0)
		{
			this->Resize(InitialLength);
			InitializeRange(this->PtrBegin(), this->PtrEnd());
		}

		//! @brief Initializes vector with copy of values of specified iterators. 
		//! @param StartIterator First iterator would be copied.
		//! @param EndIterator Last iterator would be copied.
		GDINL StaticVector(ConstIterator const StartIterator, ConstIterator const EndIterator)
		{
			this->Resize(EndIterator - StartIterator);
			CopyFromRange(StartIterator, EndIterator, this->PtrBegin());
		}

		//! @brief Initializes vector with default C++11's initializer list. You should not use this constructor manually.
		//! @param InitializerList Initializer list passed by the compiler.
		GDINL StaticVector(InitializerList<ElementType> const& InitializerList)
		{
			this->Resize(InitializerList.size());
			CopyFromRange(InitializerList.begin(), InitializerList.end(), this->PtrBegin());
		}

		//! @brief Initializes vector with copy of other vector.
		//! @param OtherVector Vector would be copied.
		GDINL StaticVector(StaticVector const& OtherVector)
		{
			this->Resize(OtherVector.Length);
			CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
		}

		//! @brief Deinitializes all elements and deallocates memory.
		GDINL ~StaticVector()
		{
			this->Emptify();
		}

	private:

		//! @brief Returns pointer iterator that points to first container element.
		//! @returns Simple pointer iterator that points to first container element.
		//! @{
		GDINL PtrIterator PtrBegin()
		{
			return reinterpret_cast<PtrIterator>(this->Memory);
		}
		GDINL PtrConstIterator PtrBegin() const
		{
			return reinterpret_cast<PtrConstIterator>(this->Memory);
		}
		//! @}

		//! @brief Returns pointer iterator that points to past the end container element.
		//! @returns Simple pointer iterator that points to past the end container element.
		//! @{
		GDINL PtrIterator PtrEnd()
		{
			return this->PtrBegin() + this->Length;
		}
		GDINL PtrConstIterator PtrEnd() const
		{
			return this->PtrBegin() + this->Length;
		}
		//! @}

		//! @brief Returns pointer iterator that points to last container element.
		//! @returns Simple pointer iterator that points to last container element.
		//! @{
		GDINL ReversePtrIterator ReversePtrBegin()
		{
			return ReversePtrIterator(this->PtrEnd() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrBegin() const
		{
			return ReversePtrConstIterator(this->PtrEnd() - 1);
		}
		//! @}

		//! @brief Returns pointer iterator that points to preceding the first container element.
		//! @returns Simple pointer iterator that points to preceding the first container element.
		//! @{
		GDINL ReversePtrIterator ReversePtrEnd()
		{
			return ReversePtrIterator(this->PtrBegin() - 1);
		}
		GDINL ReversePtrConstIterator ReversePtrEnd() const
		{
			return ReversePtrConstIterator(this->PtrBegin() - 1);
		}
		//! @}

	public:

		//! @brief Returns iterator that points to first container element.
		//! @returns Iterator that points to first container element.
		//! @{
		GDINL Iterator Begin()
		{
			return Iterator(*this);
		}
		GDINL ConstIterator Begin() const
		{
			return ConstIterator(*this);
		}
		//! @}

		//! @brief Returns iterator that points to past the end container element.
		//! @returns Iterator that points to past the end container element.
		//! @{
		GDINL Iterator End()
		{
			return this->Begin() + this->Length;
		}
		GDINL ConstIterator End() const
		{
			return this->Begin() + this->Length;
		}
		//! @}

		//! @brief Returns iterator that points to last container element.
		//! @returns Iterator that points to last container element.
		//! @{
		GDINL ReverseIterator ReverseBegin()
		{
			return ReverseIterator(this->End() - 1);
		}
		GDINL ReverseConstIterator ReverseBegin() const
		{
			return ReverseConstIterator(this->End() - 1);
		}
		//! @}

		//! @brief Returns iterator that points to preceding the first container element.
		//! @returns Iterator that points to preceding the first container element.
		//! @{
		GDINL ReverseIterator ReverseEnd()
		{
			return ReverseIterator(this->Begin() - 1);
		}
		GDINL ReverseConstIterator ReverseEnd() const
		{
			return ReverseConstIterator(this->Begin() - 1);
		}
		//! @}

	public:

		//! @brief Returns number of elements that exist in vector.
		//! @returns Number of elements that exist in vector.
		GDINL SizeTp GetLength() const
		{
			return this->Length;
		}


		//! @brief Returns number of elements that can be placed into vector without reallocation.
		//! @returns Number of elements that can be placed into vector without reallocation.
		GDINL SizeTp static GetCapacity()
		{
			return Capacity;
		}

		//! @brief Returns true if this container is empty.
		//! @returns True if this container is empty, false otherwise.
		GDINL bool IsEmpty() const
		{
			return this->Length == 0;
		}

		//! @brief Resizes vector to make it able to contain specified number of elements.
		//! @param NewLength New required length of the container.
		GDINL void Resize(SizeTp const NewLength)
		{
			// We do not need to resize our container with same size.
			if (this->Length != NewLength)
			{
				GD_ASSERT(NewLength <= Capacity, "Static vector is running out of capacity.");
				if (this->Length < NewLength)
				{
					// Increasing size of container, we need to initialize new elements.
					InitializeRange(this->PtrEnd(), this->PtrBegin() + NewLength);
				}
				else
				{
					// Decreasing size of container, we need to destroy last elements there.
					DeinitializeRange(this->PtrBegin() + NewLength, this->PtrEnd());
				}

				this->Length = NewLength;
			}
		}

		//! @brief Destroys all elements in container without memory deallocation.
		GDINL void Emptify()
		{
			this->Resize(0);
		}

	public:

		//! @brief Returns reference on last element in container.
		//! @returns Reference on last element in the container.
		//! @{
		GDINL ElementType const& GetLastElement() const
		{
			return (*this)[this->Length - 1];
		}
		GDINL ElementType& GetLastElement()
		{
			return (*this)[this->Length - 1];
		}
		//! @}

		//! @brief Appends new element to container.
		//! @param NewElement New element that would be inserted into the end of container.
		//! @{
		GDINL void InsertLast(ElementType&& NewElement = ElementType())
		{
			this->Length += 1;
			InitializeIterator(this->PtrEnd() - 1, Forward<ElementType>(NewElement));
		}
		GDINL void InsertLast(ElementType const& NewElement)
		{
			this->Length += 1;
			InitializeIterator(this->PtrEnd() - 1, NewElement);
		}
		//! @}

		//! @brief Removes last element from container.
		GDINL void RemoveLast()
		{
			GD_ASSERT((this->Length != 0), "Container size is zero");
			this->Resize(this->Length - 1);
		}

	public:

		//! @brief Assigns vector with copy of other vector.
		//! @param OtherVector Vector would be assigned.
		//! @returns this.
		GDINL StaticVector& operator= (StaticVector const& OtherVector)
		{
			if (&OtherVector != this)
			{
				this->Length = OtherVector.Length;
				CopyFromRange(OtherVector.PtrBegin(), OtherVector.PtrEnd(), this->PtrBegin());
			}

			return *this;
		}

		//! @brief Returns reference on element at specified index.
		//! @param Index Index of the desired element in vector.
		//! @returns Reference on some element in the container.
		//! @{
		GDINL ElementType const& operator[] (SizeTp const Index) const
		{
			GD_ASSERT(Index < this->Length, "Index is out of bounds");
			return *(this->PtrBegin() + Index);
		}
		GDINL ElementType& operator[] (SizeTp const Index)
		{
			return const_cast<ElementType&>(const_cast<StaticVector const&>(*this)[Index]);
		}
		//! @}

		//! @brief Checks if this vector is same to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if both vectors have same length and elements.
		GDINL bool operator== (StaticVector const& OtherVector) const
		{
			if (this->Length == OtherVector.Length)
			{
				return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
				{
					return (First == Second);
				});
			}
			else
			{
				return false;
			}
		}

		//! @brief Checks if this vector is different to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if both vectors have different length or elements.
		GDINL bool operator!= (StaticVector const& OtherVector) const
		{
			return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
			{
				return (First != Second);
			});
		}

		//! @brief Checks if this vector is greater to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if this's of the first different elements in vectors is greater than other's or strings are equal.
		GDINL bool operator> (StaticVector const& OtherVector) const
		{
			return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
			{
				return (First >= Second);
			});
		}

		//! @brief Checks if this vector is greater to specified one.
		//! @param OtherVector Other vector to compare to.
		//! @returns True if this's of the first different elements in vectors is less than other's or strings are equal.
		GDINL bool operator< (StaticVector const& OtherVector) const
		{
			return CompareTo(*this, OtherVector, [](ElementType const& First, ElementType const& Second) -> bool
			{
				return (First <= Second);
			});
		}

		//! @brief Concatenates two vectors.
		//! @param OtherVector Other vector to concatenate with.
		//! @returns Result of the vector concatenation.
		//! @{
		GDINL StaticVector& operator+= (StaticVector&& OtherVector)
		{
			for (Iterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator)
			{
				this->InsertLast(Move(*Iterator));
			}

			return *this;
		}
		GDINL StaticVector& operator+= (StaticVector const& OtherVector)
		{
			for (Iterator Iterator = OtherVector.Begin(); Iterator != OtherVector.End(); ++Iterator)
			{
				this->InsertLast(*Iterator);
			}

			return *this;
		}
		GDINL StaticVector operator+ (StaticVector&& OtherVector) const
		{
			return StaticVector(*this) += Forward<StaticVector>(OtherVector);
		}
		GDINL StaticVector operator+ (StaticVector const& OtherVector) const
		{
			return StaticVector(*this) + Move(StaticVector(OtherVector));
		}
		//! @}

	private:

		template<typename SomeElementType, SizeTp const SomeCapacity>
		GDINL friend void Swap(StaticVector<SomeElementType, SomeCapacity>& First, StaticVector<SomeElementType, SomeCapacity>& Second);
	};	// class StaticVector

GD_NAMESPACE_END
