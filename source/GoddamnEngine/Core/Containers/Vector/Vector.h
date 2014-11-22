/// ==========================================================================================
/// Vector.h - Dynamically sized array interface
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy.
///		* 24.04.2014 - Rewritten from scratch by James Jhuighuy.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_CONTAINERS_VECTOR
#define GD_CORE_CONTAINERS_VECTOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/Allocator/Allocator.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

#include <initializer_list>
#include <cstring>

GD_NAMESPACE_BEGIN

	namespace VectorMemoryProviders
	{
		template<size_t const TheCapacity>
		struct StackMemoryProvider;
		struct HeapMemoryProvider;
	}	// namespace VectorMemoryProviders

	template<typename ElementType, typename MemoryProviderType = VectorMemoryProviders::HeapMemoryProvider>
	class Vector;

	template<size_t const TheCapacity>
	inline void Swap(VectorMemoryProviders::StackMemoryProvider<TheCapacity>& First, VectorMemoryProviders::StackMemoryProvider<TheCapacity>& Second);
	inline void Swap(VectorMemoryProviders::HeapMemoryProvider& First, VectorMemoryProviders::HeapMemoryProvider& Second);

	template<typename ElementType, typename MemoryProviderType>
	inline void Swap(Vector<ElementType, MemoryProviderType>& First, Vector<ElementType, MemoryProviderType>& Second);

	/// Contains some types that would be used as container memory providers.
	/// By specifying different providers we can transform our container into stack or dynamic array or provide custom memory allocators.
	namespace VectorMemoryProviders
	{
		struct VectorMemoryProvider
		{
		};	// struct VectorMemoryProvider

		size_t static const FlexibleMaxCapacity = 0;

		/// Stack allocator type. Elements would be located in same place where container exists, but no dynamic capacity management is available.
		/// Note that memory is not initialized by default.
		template<size_t const Capacity>
		struct StackMemoryProvider final : VectorMemoryProvider
		{
		public:
			size_t static const MaxCapacity = Capacity;

		private:
			size_t static const MemoryBlockSize = (Capacity / sizeof(size_t)) + 1;
			size_t Memory[MemoryBlockSize]; // Memory block is provided as array of bytes to prevent automatic objects initialization.
#	if (defined(GD_DEBUG) && defined(GD_PLATFORM_API_WINAPI))
			size_t const Capacity = Capacity;
			// For Natvis. This variable is required only in debug builds for MSVC compilers.
#	endif	// if (defined(GD_DEBUG) && defined(GD_PLATFORM_API_WINAPI))

		public:
			inline  StackMemoryProvider(size_t const Capacity);
			inline  StackMemoryProvider(StackMemoryProvider const& OtherStackMemoryProvider) = delete;
			inline  StackMemoryProvider(StackMemoryProvider     && OtherStackMemoryProvider) { (*this) = OtherStackMemoryProvider; }
			inline ~StackMemoryProvider() { }

			GDINL size_t GetCapacity() const { return Capacity; }
			GDINL handle GetPointer () const { return &this->Memory[0]; }
			GDINL handle GetPointer ()		 { return &this->Memory[0]; }

			inline StackMemoryProvider& operator= (StackMemoryProvider const& OtherStackMemoryProvider) = delete;
			inline StackMemoryProvider& operator= (StackMemoryProvider     && OtherStackMemoryProvider);

		private:
			template<size_t const Capacity>
			inline friend void GD Swap(StackMemoryProvider<Capacity>& First, StackMemoryProvider<Capacity>& Second);
		};	// struct StackMemoryProvider
		
		/// MemoryProvider that supports dynamic capacity management.
		/// Elements are stored somewhere in the heap.
		struct HeapMemoryProvider final : VectorMemoryProvider
		{
		private:
			handle Memory;
			size_t Capacity;

		public:
			size_t static const MaxCapacity = FlexibleMaxCapacity;

			inline  HeapMemoryProvider(size_t const Capacity);
			inline  HeapMemoryProvider(HeapMemoryProvider const& OtherHeapMemoryProvider) = delete;
			inline  HeapMemoryProvider(HeapMemoryProvider     && OtherHeapMemoryProvider);
			inline ~HeapMemoryProvider();

			GDINL size_t GetCapacity() const { return this->Capacity; }
			GDINL handle GetPointer () const { return this->Memory; }
			GDINL handle GetPointer ()		 { return this->Memory; }

			inline HeapMemoryProvider& operator= (HeapMemoryProvider const& OtherHeapMemoryProvider) = delete;
			inline HeapMemoryProvider& operator= (HeapMemoryProvider     && OtherHeapMemoryProvider);

		private:
			inline friend void GD Swap(HeapMemoryProvider& First, HeapMemoryProvider& Second);
		};	// struct HeapMemoryProvider
	}	// namespace VectorMemoryProviders

	/// Dynamic array implementation. First template parameter specifies this container`s element type.
	/// Second one can specify some heap or stack memory providing, so this class can be treated as stack vector too.
	/// This class if fully compatiable with @c std::vector class. Our implemntation is faster than standart one.
	/// @note This class uses a lot move constructors. So try to provide them for all objects that would be placed in container.
	template<typename ElementType, typename MemoryProviderType>
	class Vector
	{
	public:
		typedef ElementType ThisElementType;
		typedef MemoryProviderType ThisMemoryProviderType;
		typedef Vector<ElementType, ThisMemoryProviderType> ThisVectorType;
		typedef ContainerDetails::IndexedContainerIterator<ThisVectorType const, ElementType const> ConstIterator;
		typedef ContainerDetails::IndexedContainerIterator<ThisVectorType      , ElementType      > MutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;
		typedef ThisElementType const* PtrConstIterator;
		typedef ThisElementType      * PtrMutableIterator;
		typedef ContainerDetails::ReverseIterator<PtrConstIterator  > ReversePtrConstIterator;
		typedef ContainerDetails::ReverseIterator<PtrMutableIterator> ReversePtrMutableIterator;
		
		// Theoretically, we can omit initialization/deinitialization for POD types.
	//	bool static const IsObjectElementType = (!TypeTraits::IsPodType<ElementType>::Value);
	//	bool static const IsBetterToPassElementTypeByValue = ((!IsPODElementType) && (sizeof(ElementType) <= sizeof(size_t)));
	//	typedef typename Conditional<IsBetterToPassElementTypeByValue, ElementType, ElementType&> ElementTypeRef

	private:
		MemoryProviderType MemoryProviderInstance;
		size_t Count = 0;

		template<typename OtherSourceIteratorType, typename OtherDestinationIteratorType>
		inline static void CopyFromRange(OtherSourceIteratorType const& StartSourceIterator, OtherSourceIteratorType const& EndSourceIterator, OtherDestinationIteratorType const& StartDestinationIterator);
		template<typename OtherSourceIteratorType, typename OtherDestinationIteratorType>
		inline static void MoveFromRange(OtherSourceIteratorType const& StartSourceIterator, OtherSourceIteratorType const& EndSourceIterator, OtherDestinationIteratorType const& StartDestinationIterator);
		inline static void   InitializeRange(PtrMutableIterator const StartIterator, PtrMutableIterator const EndIterator);
		inline static void DeinitializeRange(PtrMutableIterator const StartIterator, PtrMutableIterator const EndIterator);

		template<typename ComparasionPredicateType>
		inline bool CompareTo(Vector const& OtherVector, ComparasionPredicateType const& ComparasionPredicate);

	public:
		/// ------------------------------------------------------------------------------------------
		/// @name Constructors and destructor
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// Initializes vector with specified number of allocated and initialized elements.
		/// @param InitialElemntsCount Number of elements been initialized.
		/// @param InitialCapacity     Initial capacity of vector. 
		inline Vector(size_t const InitialElemntsCount = 0, size_t const InitialCapacity = SIZE_MAX);

		/// Initializes vector with copy of values of specified iterators.
		/// @oaram StartIterator First iterator would be copied.
		/// @param EndIterator   Last iterator would be copied.
		inline Vector(ConstIterator const StartIterator, ConstIterator const EndIterator);

		/// Intializes vector with default C++11's initializer list. You should not use this constructor manually.
		/// @param InitializerList Initializer list passed be the compiler.
		inline Vector(std::initializer_list<ElementType> const& InitializerList);
		
		/// Moves other vector here.
		/// @param OtherVector Vector would be moved into current object.
		inline Vector(Vector&& OtherVector);

		/// Initializes vector with copy of other vector.
		/// @param OtherVector Vector would be copied.
		inline Vector(Vector const& OtherVector);

#if (defined(__cplusplus_cli))
		/// Initializes vector with .NET array.
		/// @param CliArray
		inline Vector(array<ElementType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		/// Deinitializes all elements and deallocates memeory.
		inline ~Vector();

		/// @}

	public /* Class API */:
		/// ------------------------------------------------------------------------------------------
		/// @name Vector iteration.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// Returns pointer iterator that points to first container element.
		GDINL PtrMutableIterator PtrBegin()       { return reinterpret_cast<PtrMutableIterator>(this->MemoryProviderInstance.GetPointer()); }
		GDINL   PtrConstIterator PtrBegin() const { return const_cast<PtrConstIterator>(const_cast<Vector*>(this)->PtrBegin()); }

		/// Returns pointer iterator that points to past the end container element.
		GDINL PtrMutableIterator PtrEnd()       { return (this->PtrBegin() + this->Count); }
		GDINL   PtrConstIterator PtrEnd() const { return const_cast<PtrConstIterator>(const_cast<Vector*>(this)->PtrEnd()); }

		/// Returns iterator that points to first container element.
		GDINL MutableIterator    Begin()       { return MutableIterator(*this); }
		GDINL   ConstIterator    Begin() const { return   ConstIterator(*this); }

		/// Returns iterator that points to past the end container element.
		GDINL MutableIterator    End  ()       { return (this->Begin() + this->Count); }
		GDINL   ConstIterator    End  () const { return (this->Begin() + this->Count); }

		/// Returns pointer iterator that points to last container element.
		GDINL ReversePtrMutableIterator ReversePtrBegin()       { return ReversePtrMutableIterator(this->PtrEnd() - 1); }
		GDINL   ReversePtrConstIterator ReversePtrBegin() const { return   ReversePtrConstIterator(this->PtrEnd() - 1); }

		/// Returns pointer iterator that points to preceding the first container element
		GDINL ReversePtrMutableIterator ReversePtrEnd()       { return ReversePtrMutableIterator(this->PtrBegin()); }
		GDINL   ReversePtrConstIterator ReversePtrEnd() const { return   ReversePtrConstIterator(this->PtrBegin()); }

		/// Returns iterator that points to last container element.
		GDINL ReverseMutableIterator	ReverseBegin()       { return ReverseMutableIterator(this->End() - 1); }
		GDINL   ReverseConstIterator	ReverseBegin() const { return   ReverseConstIterator(this->End() - 1); }

		/// Returns iterator that points to preceding the first container element
		GDINL ReverseMutableIterator	ReverseEnd  ()       { return ReverseMutableIterator(this->Begin()); }
		GDINL   ReverseConstIterator	ReverseEnd  () const { return   ReverseConstIterator(this->Begin()); }

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Dynamic size management.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// Returns number of elements that exist in vector.
		GDINL size_t			 GetSize() const;
		/// Returns number of elements that can be placed into vector without reallocation.
		GDINL size_t			 GetCapacity() const;

		/// Returns true it this container is empty.
		GDINL bool				 IsEmpty() const { return this->GetSize() == 0; }

		/// Resizes vector to make it able to contain specified number of elements.
		inline void				 Resize(size_t const NewElementsCount);
		/// Reserves memory for vector to make it contain specified number of elements without reallocation when calling Resize method.
		inline void				 Reserve(size_t const Capacity);
		/// Reserves memory for vector to make it contain best fitting number of elements to predicted new size
		inline void				 ReserveToSize(size_t const NewSize);

		/// Shrinks vector's capacity to size.
		inline void				 ShrinkToFit();

		/// Destroys all elements in container without memory deallocation.
		inline void				 Emptify();
		/// Destroys all elements in container with memory deallocation.
		inline void				 Clear();

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Elements access.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// Returns reference on element at specified index.
		/// @note Index is validated only in debug builds.
		inline ElementType const& GetElementAt(size_t const Index) const;
		inline ElementType	    & GetElementAt(size_t const Index);
		
		/// Returns reference on first element in container.
		/// @note In debug build this method checks if any element exists in container.
		GDINL ElementType const& GetFirstElement() const;
		GDINL ElementType      & GetFirstElement();
		
		/// Returns reference on last element in container.
		/// @note In debug build this method checks if any element exists in container.
		GDINL ElementType const& GetLastElement() const;
		GDINL ElementType      & GetLastElement();
		
		/// Assigns new element into place in container with specified index.
		/// @note Index is validated only in debug builds.
		inline void				 SetElementAt(size_t const Index, ElementType     && Element);
		inline void				 SetElementAt(size_t const Index, ElementType const& Element);
		
		/// Inserts specified element into collection at desired index.
		/// @note Index is validated only in debug builds.
		inline void				 InsertElementAt(size_t const Index, ElementType     && Element);
		inline void				 InsertElementAt(size_t const Index, ElementType const& Element);
		
		/// Removes existing element from array at specified index.
		/// @note Index is validated only in debug builds.
		inline void				 RemoveElementAt(size_t const Index);

		/// Appends new element to container.
		inline void				 PushLast(ElementType     && NewElement = ElementType());
		inline void				 PushLast(ElementType const& NewElement);
		
		/// Removes last element from container.
		inline void				 PopLast();

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Searching of elements.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// Searches for first element in container by predicate that contains boolean returning () operator overloaded.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		template<typename SearchingPredicateType>
		inline size_t			 FindFirstElement(SearchingPredicateType const& SearchingPredicate) const;
		/// Searches for first element in container by checking equality.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		inline size_t			 FindFirstElement(ElementType const& Element) const;

		/// Searches for last element in container by predicate that contains boolean returning () operator overloaded.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		template<typename SearchingPredicateType>
		inline size_t			 FindLastElement(SearchingPredicateType const& SearchingPredicate) const;
		/// Searches for last element in container by checking equality.
		/// @returns Index of found element or @c SIZE_MAX if nothing was found.
		inline size_t			 FindLastElement(ElementType const& Element) const;

		/// Searches for all elements in container by predicate that contains boolean returning () operator overloaded.
		template<typename SearchingPredicateType>
		inline ThisVectorType	 FindAllElements(SearchingPredicateType const& SearchingPredicate) const;

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Sorting elements.
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// Sorts all elements in collection by predicate that contains boolean returning () operator overloaded using Quick Sort.
		/// If functor returns true that this object should locate upper in collection.
		template<typename SortingPredicateType>
		inline void				 Sort(SortingPredicateType const& SortingPredicate, MutableIterator const Left, MutableIterator const Right);
		template<typename SortingPredicateType>
		inline void				 Sort(SortingPredicateType const& SortingPredicate);
		inline void				 Sort();

		/// @}

		/// ------------------------------------------------------------------------------------------
		/// @name Operators
		/// @{
		/// ------------------------------------------------------------------------------------------

		/// @note Assignment operators does not change capacity if it is not required.
		inline Vector& operator= (Vector     && OtherVector);
		inline Vector& operator= (Vector const& OtherVector);
		inline Vector& operator= (std::initializer_list<ElementType> const& InitializerList);
#if (defined(__cplusplus_cli))
		inline Vector& operator= (array<ElementType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

	public:
		inline bool operator== (Vector const& OtherVector) const;
		inline bool operator!= (Vector const& OtherVector) const;
		inline bool operator>  (Vector const& OtherVector) const;
		inline bool operator<  (Vector const& OtherVector) const;
		inline bool operator>= (Vector const& OtherVector) const;
		inline bool operator<= (Vector const& OtherVector) const;

		inline Vector& operator+= (Vector     && OtherVector);
		inline Vector& operator+= (Vector const& OtherVector);
		inline Vector  operator+  (Vector     && OtherVector) const;
		inline Vector  operator+  (Vector const& OtherVector) const;

		GDINL ElementType const& operator[] (size_t const Index) const;
		GDINL ElementType      & operator[] (size_t const Index);

		/// @}

	private /* STL compatibility */:	
		GDINL friend MutableIterator begin(Vector      & some_vector) { return some_vector.Begin(); }
		GDINL friend   ConstIterator begin(Vector const& some_vector) { return some_vector.Begin(); }
		GDINL friend MutableIterator end  (Vector      & some_vector) { return some_vector.End(); }
		GDINL friend   ConstIterator end  (Vector const& some_vector) { return some_vector.End(); }

		template<typename ElementType, typename MemoryProviderType>
		inline friend void GD Swap(Vector<ElementType, MemoryProviderType>& First, Vector<ElementType, MemoryProviderType>& Second);
	};	// class Vector

	/// Heap vector.
	template<typename ElementType>
	using HeapVector = Vector<ElementType, VectorMemoryProviders::HeapMemoryProvider>;

	/// Stack vector. Same as Vector container but does not allows dynamically change capacity.
	template<typename ElementType, size_t const Capacity>
	using StackVector = Vector<ElementType, VectorMemoryProviders::StackMemoryProvider<Capacity * sizeof(ElementType)>>;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Vector/Vector.inl>

#endif
