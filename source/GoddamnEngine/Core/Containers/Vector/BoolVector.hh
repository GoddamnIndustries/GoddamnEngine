//////////////////////////////////////////////////////////////////////////
/// BoolVector.hh - Dynamic bitset interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.05.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_BOOL_VECTOR
#define GD_CORE_CONTAINERS_BOOL_VECTOR

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Containers/Vector/Vector.hh>

#include <stdint.h>
#define GD_BITS_PER_BYTE 8

GD_NAMESPACE_BEGIN

	template<typename MemoryProviderType /*= VectorMemoryProviders::HeapMemoryProvider<bool>*/>
	class Vector<bool, MemoryProviderType>
	{
	public:
		template<typename Tag>
		struct Iterator final
		{
			typedef bool ElementType;
			typedef typename Conditional<Tag::IsConst, UInt8 const*, UInt8*>::Type ThisPtrType;
			typedef typename bool ThisRefType;

		private:
			ThisPtrType Pointer;
			size_t BitIndex = 0;

		public:
			GDINL  Iterator(ThisPtrType const Pointer, UInt8 const BitIndex) : Pointer(      Pointer), BitIndex(      BitIndex) { }
			GDINL  Iterator(Iterator    const& Other                       ) : Pointer(Other.Pointer), BitIndex(Other.BitIndex) { }
			GDINL ~Iterator(                                               ) { }

			/// Compares iterators.
			GDINL bool operator== (Iterator const& Other) const { return ((self->Pointer == Other.Pointer) && (self->BitIndex == Other.BitIndex)); }
			GDINL bool operator!= (Iterator const& Other) const { return (!((*self) == Other)); }
			
			/// (De)referensing iterator.
			GDINL bool operator*  () const { return (((*self->Pointer) & GD_BIT(self->BitIndex)) != 0); }
			GDINL bool operator-> () const { return   (self->operator*()); }

			/// Assigment operators. 
			GDINL Iterator& operator= (Iterator     && Other) = delete;
			GDINL Iterator& operator= (Iterator const& Other)
			{
				self->Pointer  = Other.Pointer;
				self->BitIndex = Other.BitIndex;
				return (*self);
			}

			/// Increments iterator.
			GDINL Iterator& operator++ (int const) { return ++(*self); }
			GDINL Iterator& operator++ (         )
			{
				if ((++self->BitIndex) == GD_BITS_PER_BYTE)
				{	// We have reached bytes border.
					self->BitIndex = 0;
					self->Pointer += 1;
				}

				return (*self);
			}

			/// Decrements iterator.
			GDINL Iterator& operator-- (int const) { return --(*self); }
			GDINL Iterator& operator-- (         )
			{
				if ((--self->BitIndex) == SIZE_MAX)
				{	// We have reached bytes border.
					self->BitIndex = 0;
					self->Pointer -= 1;
				}

				return (*self);
			}
		};	// struct Iterator

		/// Iterator type this container uses.
		typedef Iterator<ContainerDetails::IteratorTagConst  >   ConstIterator;
		typedef Iterator<ContainerDetails::IteratorTagMutable> MutableIterator;

		/// Reverse iterator type this container uses.
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;

		typedef MemoryProviderType ThisMemoryProviderType;
		typedef bool ThisElementType;

	private:
		Vector<UInt8, MemoryProviderType> ByteStorage;
		size_t BitIndex = 0;

	public /* Constructors and destructor */:
		GDINL Vector(size_t const InitialElemntsCount = 0, bool const InitialElement = bool(), size_t const Capacity = -1);
		GDINL Vector(ConstIterator const StartIterator, ConstIterator const EndIterator);
		GDINL Vector(std::initializer_list<bool> const& InitializerList);
		GDINL Vector(Vector     && OtherVector);
		GDINL Vector(Vector const& OtherVector);
#if (defined(__cplusplus_cli))
		GDINL Vector(array<bool>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

	public /* Class API */:
		/// Returns iterator that points to first container bit.
		GDINL MutableIterator Begin()       { return MutableIterator(&self->ByteStorage[0], self->BitIndex); }
		GDINL   ConstIterator Begin() const { return   ConstIterator(&self->ByteStorage[0], self->BitIndex); }

		/// Returns iterator that points to past the end container bit.
		GDINL MutableIterator End  ()       { return MutableIterator(&self->ByteStorage.GetLastElement(), self->BitIndex + 1); }
		GDINL   ConstIterator End  () const { return   ConstIterator(&self->ByteStorage.GetLastElement(), self->BitIndex + 1); }

		/// Returns iterator that points to last container bit.
		GDINL ReverseMutableIterator ReverseBegin()       { return ReverseMutableIterator(MutableIterator(&self->ByteStorage.GetLastElement(), self->BitIndex)); }
		GDINL   ReverseConstIterator ReverseBegin() const { return   ReverseConstIterator(  ConstIterator(&self->ByteStorage.GetLastElement(), self->BitIndex)); }

		/// Returns iterator that points to preceding the first container bit
		GDINL ReverseMutableIterator ReverseEnd  ()       { return ReverseMutableIterator(MutableIterator(&self->ByteStorage.GetLastElement(), self->BitIndex - 1)); }
		GDINL   ReverseConstIterator ReverseEnd  () const { return   ReverseConstIterator(  ConstIterator(&self->ByteStorage.GetLastElement(), self->BitIndex - 1)); }

		/// Returns number of bits that exist in vector.
		GDINL size_t GetSize    () const;
		/// Returns number of bits that can be placed into vector without reallocation.
		GDINL size_t GetCapacity() const;

		/// Resizes vector to make it contain specified number of bits.
		GDINL void Resize(size_t const NewBitsCount, bool const InitialBit = bool());
		/// Reserves memory for vector to make it contain specified number of bits without reallocation when calling Resize method.
		GDINL void Reserve(size_t const Capacity);

		/// Destroys all bits in container without memory deallocation.
		GDINL void Emptify();
		/// Destroys all bits in container with memory deallocation.
		GDINL void Clear();

		/// Returns reference on bit at specified index.
		/// @note Index is validated only in debug builds.
		GDINL bool GetElementAt(size_t const Index) const;
		
		/// Returns reference on first bit in container.
		/// @note In debug build this method checks if any bit exists in container.
		GDINL bool GetFirstElement() const;
		
		/// Returns reference on last bit in container.
		/// @note In debug build this method checks if any bit exists in container.
		GDINL bool GetLastElement() const;
		
		/// Assigns new bit into place in container with specified index.
		/// @note Index is validated only in debug builds.
		GDINL void SetElementAt(size_t const Index, bool const Bit);
		
		/// Inserts specified bit into collection at desired index.
		/// @note Index is validated only in debug builds.
		GDINL void InsertElementAt(size_t const Index, bool const Bit);
		
		/// Removes existing bit from array at specified index.
		/// @note Index is validated only in debug builds.
		GDINL void RemoveElementAt(size_t const Index);

		/// Appends new bit to container.
		GDINL void PushLast(bool NewBit = bool());
		
		/// Removes last bit from container.
		GDINL void PopLast();

		/// Searches for first bit in container by predicate that contains boolean returning () operator overloaded.
		/// @returns Index of found bit or @c SIZE_MAX if nothing was found.
		template<typename SearchingPredicateType>
		GDINL size_t FindFirstElement(SearchingPredicateType const& SearchingPredicate) const;
		/// Searches for first bit in container by checking equality.
		/// @returns Index of found bit or @c SIZE_MAX if nothing was found.
		GDINL size_t FindFirstElement(bool const Bit) const;

		/// Searches for last bit in container by predicate that contains boolean returning () operator overloaded.
		/// @returns Index of found bit or @c SIZE_MAX if nothing was found.
		template<typename SearchingPredicateType>
		GDINL size_t FindLastElement(SearchingPredicateType const& SearchingPredicate) const;
		/// Searches for last bit in container by checking equality.
		/// @returns Index of found bit or @c SIZE_MAX if nothing was found.
		GDINL size_t FindLastElement(bool const Bit) const;

		/// Sorts all bits in collection by predicate that contains boolean returning () operator overloaded using Quick Sort.
		/// If functor returns true that this object should locate upper in collection.
		template<typename SortingPredicateType>
		GDINL void Sort(SortingPredicateType const& SortingPredicate, MutableIterator const Left, MutableIterator const Right);
		template<typename SortingPredicateType>
		GDINL void Sort(SortingPredicateType const& SortingPredicate);
		GDINL void Sort();

	public /* Operators */:
		/// @note Assignment operators does not change capacity if it is not required.
		GDINL Vector& operator= (Vector     && OtherVector);
		GDINL Vector& operator= (Vector const& OtherVector);
		GDINL Vector& operator= (std::initializer_list<bool> const& InitializerList);
#if (defined(__cplusplus_cli))
		GDINL Vector& operator= (array<bool>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		GDINL bool operator>  (Vector const& OtherVector) const;
		GDINL bool operator<  (Vector const& OtherVector) const;
		GDINL bool operator== (Vector const& OtherVector) const;
		GDINL bool operator!= (Vector const& OtherVector) const;
		GDINL bool operator>= (Vector const& OtherVector) const;
		GDINL bool operator<= (Vector const& OtherVector) const;

		GDINL bool operator[] (size_t const Index) const;

#if (!defined(GD_DOCUMENTATION)) /// @todo Check if this is full compatibility list for std::vector class.
	private /* STL compatibility */:
		GDINL friend MutableIterator begin(Vector      & some_vector) { return some_vector.Begin(); }
		GDINL friend   ConstIterator begin(Vector const& some_vector) { return some_vector.Begin(); }
		GDINL friend MutableIterator end  (Vector      & some_vector) { return some_vector.End(); }
		GDINL friend   ConstIterator end  (Vector const& some_vector) { return some_vector.End(); }

		GDINL friend void Swap(Vector& First, Vector& Second)
		{	using GD Swap;
			Swap(First.ByteStorage, Second.ByteStorage);
			Swap(First.BitIndex,    Second.BitIndex);
		}
#endif	// if (!defined(GD_DOCUMENTATION))
	};	// class Vector<bool>

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Vector/BoolVector.inl>

#endif
