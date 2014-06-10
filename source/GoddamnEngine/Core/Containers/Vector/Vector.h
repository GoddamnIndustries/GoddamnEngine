//////////////////////////////////////////////////////////////////////////
/// Vector.h - Dynamically sized array interface
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* --.06.2013 - Created by James Jhuighuy
///		* 24.04.2014 - Rewritten from scratch by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_CONTAINERS_VECTOR
#define GD_CORE_CONTAINERS_VECTOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Utility.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/Allocator/Allocator.h>
#include <GoddamnEngine/Core/Containers/Containers.h>

#include <initializer_list>
#include <string.h>

GD_NAMESPACE_BEGIN

	/// Contains some types that would be used as container memory providers.
	/// By specifying different providers we can transform our container into 
	/// stack or dynamic array or provide custom memory allocators.
	namespace VectorMemoryProviders
	{
#if (defined(GD_DOCUMENTATION))
		/// Shows basic usage of custom memory providers for Vector container class.
		/// All custom memory providers should contain same API to this example class.  
		struct CustomMemoryProvider final
		{
			/// Memory provider can specify this variable that can be casted to 
			/// @c ElementType* type. This variable allows Visual Studio visualizer
			/// visualize container data.
			/// @note This field can (and obviously should) be private. 
			SomeCastableToElementTypePtr Memory;

			/// This variable allows Visual Studio visualizer visualize container capacity.
			/// @note This field can (and obviously should) be private. 
			size_t const Capacity = 0;

		public:
			/// Class should contain constructor with specified initial capacity. 
			GDINT CustomMemoryProvider(size_t const Capacity);

			/// Class should contain move constructor and not contain copy constructor. 
			GDINT CustomMemoryProvider(CustomMemoryProvider const& OtherCustomMemoryProvider) = delete;
			GDINT CustomMemoryProvider(CustomMemoryProvider     && OtherCustomMemoryProvider);

			/// Should return current capacity.
			GDINT size_t GetCapacity() const;

			/// Should return pointer to allocated memory. 
			GDINT ElementType const* GetPointer() const;
			GDINT ElementType      * GetPointer();

			/// Class should contain move assignment operator and not contain copy assignment operator. 
			GDINT CustomMemoryProvider& operator= (CustomMemoryProvider const& OtherCustomMemoryProvider) = delete;
			GDINT CustomMemoryProvider& operator= (CustomMemoryProvider     && OtherCustomMemoryProvider);

			/// Swap operator overloading is recommended.
			GDINT friend void Swap(VectorMemoryProviders&, VectorMemoryProviders&);
		};	// struct CustomMemoryProvider
#endif	//if (defined(GD_DOCUMENTATION))

		/// Stack allocator type. Elements would be located in same place where 
		/// container exists, but no dynamic capacity management is available.
		/// Note that memory is not initialized by default.
		template<typename ElementType, size_t const _Capacity>
		struct StackMemoryProvider final
		{
#if (!defined(GD_DOCUMENTATION))
		private:
#	if (defined(GD_DEBUG) && (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPHONE8)))
			size_t const Capacity = _Capacity;				// For Natvis. This variable is required only in debug builds for MSVC compilers.
#	endif	// if (defined(GD_DEBUG) && (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_XBOX_ONE) || defined(GD_PLATFORM_WINPHONE8)))
			UInt8 Memory[_Capacity * sizeof(ElementType)];	// Memory block is provided as array of bytes to prevent automatic objects initialization.

		public:
			inline StackMemoryProvider(size_t const Capacity){ GD_ASSERT((Capacity <= _Capacity), "Stack memory provider size is out of range."); }
			inline StackMemoryProvider(StackMemoryProvider const& OtherStackMemoryProvider) = delete;
			inline StackMemoryProvider(StackMemoryProvider     && OtherStackMemoryProvider) { (*self) = OtherStackMemoryProvider; }

			inline ~StackMemoryProvider() { }

			GDINL size_t             GetCapacity() const { return _Capacity; }
			GDINL ElementType const* GetPointer()  const { return reinterpret_cast<ElementType const*>(&self->Memory[0]); }
			GDINL ElementType	   * GetPointer()		 { return reinterpret_cast<ElementType      *>(&self->Memory[0]); }

			inline StackMemoryProvider& operator= (StackMemoryProvider const& OtherStackMemoryProvider) = delete;
			inline StackMemoryProvider& operator= (StackMemoryProvider     && OtherStackMemoryProvider)
			{
				if ((&OtherStackMemoryProvider) != self)
				{	// This operator is quiet dangerous because it does not moves object into new places but just copies memory.
					// And do we actually need to fill with zeros all moved memory? So leave it be how it is.
					self->~StackMemoryProvider();
					memcpy(&self->Memory[0], &OtherStackMemoryProvider.Memory[0], sizeof(self->Memory));
					memset(&OtherStackMemoryProvider.Memory[0], 0, sizeof(OtherStackMemoryProvider.Memory));
				}

				return *self;
			}

		private:
            inline friend void Swap(StackMemoryProvider& First, StackMemoryProvider& Second)
			{	using GD DeclValue;	
				if (&First != &Second)
				{
					UInt8   Memory[_Capacity * sizeof(ElementType)];
					memcpy(&Memory[0],           &First.Memory[0], sizeof(DeclValue<StackMemoryProvider>().Memory));
					memcpy(&First.Memory[0], &Second.Memory[0], sizeof(DeclValue<StackMemoryProvider>().Memory));
					memcpy(&Second.Memory[0], &Memory[0],           sizeof(DeclValue<StackMemoryProvider>().Memory));
				}
			}
#endif		// if (!defined(GD_DOCUMENTATION))
		};	// struct StackMemoryProvider
		
		/// MemoryProvider that supports dynamic capacity management.
		/// Elements are stored somewhere in the heap.
		template<typename ElementType>
		struct HeapMemoryProvider final
		{
#if (!defined(GD_DOCUMENTATION))
		private:
			ElementType* Memory;
			size_t Capacity;

		public:
			inline HeapMemoryProvider(size_t const Capacity)
				: Capacity(                                                         Capacity                        )
				, Memory  (reinterpret_cast<ElementType*>(Allocator::AllocateMemory(Capacity * sizeof(ElementType))))
			{ 
			}
			
			inline HeapMemoryProvider(HeapMemoryProvider const& OtherHeapMemoryProvider) = delete;
			inline HeapMemoryProvider(HeapMemoryProvider     && OtherHeapMemoryProvider)
				: Capacity(OtherHeapMemoryProvider.Capacity)
				, Memory  (OtherHeapMemoryProvider.Memory  )
			{
				OtherHeapMemoryProvider.Memory = nullptr;
				OtherHeapMemoryProvider.Capacity = 0;
			}

			inline ~HeapMemoryProvider()
			{
				Allocator::DeallocateMemory(static_cast<handle>(self->Memory));
			}

			GDINL size_t             GetCapacity() const { return self->Capacity; }
			GDINL ElementType const* GetPointer () const { return self->Memory; }
			GDINL ElementType      * GetPointer ()		 { return self->Memory; }

			inline HeapMemoryProvider& operator= (HeapMemoryProvider const& OtherHeapMemoryProvider) = delete;
			inline HeapMemoryProvider& operator= (HeapMemoryProvider     && OtherHeapMemoryProvider)
			{
				if ((&OtherHeapMemoryProvider) != self)
				{
					self->~HeapMemoryProvider();
					self->Memory   = OtherHeapMemoryProvider.Memory;
					self->Capacity = OtherHeapMemoryProvider.Capacity;
					OtherHeapMemoryProvider.Memory = nullptr;
					OtherHeapMemoryProvider.Capacity = 0;
				}

				return *self;
			}

		private:
            inline friend void Swap(HeapMemoryProvider& First, HeapMemoryProvider& Second)
			{	using GD Swap;	/// @todo Does not compiles without this line. Do not know the reason.
				Swap(First.Memory,   Second.Memory);
				Swap(First.Capacity, Second.Capacity);
			}
#endif		// if (!defined(GD_DOCUMENTATION))
		};	// struct HeapMemoryProvider
	}	// namespace VectorMemoryProviders

	/// Dynamic array implementation. First template parameter specifies this container`s element type.
	/// Second one can specify some heap or stack memory providing, so this class can be treated as stack vector too.
	/// This class if fully compatiable with @c std::vector class. Our implemntation is faster than standart one.
	/// @note This class uses a lot move constructors. So try to provide them for all objects that would be placed in container.
	template<typename ElementType, typename MemoryProviderType = VectorMemoryProviders::HeapMemoryProvider<ElementType>>
	class Vector
	{
	public:
		template<typename Tag>
		struct Iterator final
		{
			typedef ElementType ThisElementType;
			typedef typename Conditional<Tag::IsConst, ElementType const*, ElementType*>::Type ThisPtrType;
			typedef typename Conditional<Tag::IsConst, ElementType const&, ElementType&>::Type ThisRefType;

		private:
			ThisPtrType Pointer = nullptr;

		public:
			GDINL  Iterator(ThisPtrType const  Pointer ) : Pointer(Pointer) { }
			GDINL  Iterator(Iterator    const& Iterator) : Pointer(Iterator.Pointer) { }
			GDINL ~Iterator(                           ) { }

			/// Increases/decreases iterator.
			GDINL Iterator& operator++ (int const) { ++self->Pointer; return (*self); }
			GDINL Iterator& operator++ (         ) { ++self->Pointer; return (*self); }
			GDINL Iterator& operator-- (int const) { --self->Pointer; return (*self); }
			GDINL Iterator& operator-- (         ) { --self->Pointer; return (*self); }

			/// Increases/decreases iterator on specified value.
			inline Iterator& operator+= (ptrdiff_t const Offset)	      { self->Pointer += Offset; return (*self); }
			inline Iterator& operator-= (ptrdiff_t const Offset)       { self->Pointer -= Offset; return (*self); }
			inline Iterator  operator+  (ptrdiff_t const Offset) const { return Iterator(self->Pointer + Offset); }
			inline Iterator  operator-  (ptrdiff_t const Offset) const { return Iterator(self->Pointer - Offset); }

			/// Computes difference between iterators.
			inline ptrdiff_t operator- (Iterator           const& Iterator) const { return (self->Pointer - Iterator.Pointer); }
			inline ptrdiff_t operator- (ElementType const* const  Pointer ) const { return (self->Pointer -          Pointer); }

			/// Compares iterators.
			GDINL bool operator== (Iterator    const&       Other  ) const { return (self->Pointer == Other.Pointer); }
			GDINL bool operator!= (Iterator    const&       Other  ) const { return (self->Pointer != Other.Pointer); }
			GDINL bool operator== (ElementType const* const Pointer) const { return (self->Pointer == Pointer); }
			GDINL bool operator!= (ElementType const* const Pointer) const { return (self->Pointer != Pointer); }

			/// Assigns this iterator other value.
			GDINL Iterator& operator= (ThisPtrType const  Pointer ) { self->Pointer =           Pointer; return (*self); }
			GDINL Iterator& operator= (Iterator    const& Iterator) { self->Pointer = Iterator->Pointer; return (*self); }

			/// (De)referensing iterator.
			GDINL ThisRefType operator*  () const { return (*self->Pointer); }
			GDINL ThisPtrType operator-> () const { return (self->Pointer); }
		};	// struct Iterator

		/// Iterator type this container uses.
		typedef Iterator<ContainerDetails::IteratorTagConst  >   ConstIterator;
		typedef Iterator<ContainerDetails::IteratorTagMutable> MutableIterator;
		
		/// Reverse iterator type this container uses.
		typedef ContainerDetails::ReverseIterator<MutableIterator> ReverseMutableIterator;
		typedef ContainerDetails::ReverseIterator<ConstIterator  > ReverseConstIterator;

		typedef MemoryProviderType ThisMemoryProviderType;
		typedef ElementType ThisElementType;

	private:
		MemoryProviderType MemoryProviderInstance;
		size_t Count = 0;

	public /* Constructors and destructor */:
		inline Vector(size_t const InitialElemntsCount = 0, ElementType const& InitialElement = ElementType(), size_t const Capacity = -1);
		inline Vector(ConstIterator const StartIterator, ConstIterator const EndIterator);
		inline Vector(std::initializer_list<ElementType> const& InitializerList);
		inline Vector(Vector     && OtherVector);
		inline Vector(Vector const& OtherVector);
#if (defined(__cplusplus_cli))
		inline Vector(array<ElementType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		inline ~Vector();

	public /* Class API */:
		/// Returns iterator that points to first container element.
		GDINL MutableIterator Begin()       { return MutableIterator(self->MemoryProviderInstance.GetPointer()); }
		GDINL   ConstIterator Begin() const { return   ConstIterator(self->MemoryProviderInstance.GetPointer()); }

		/// Returns iterator that points to past the end container element.
		GDINL MutableIterator End  ()       { return MutableIterator(self->MemoryProviderInstance.GetPointer() + self->Count); }
		GDINL   ConstIterator End  () const { return   ConstIterator(self->MemoryProviderInstance.GetPointer() + self->Count); }

		/// Returns iterator that points to last container element.
		GDINL ReverseMutableIterator ReverseBegin()       { return ReverseMutableIterator(MutableIterator(self->MemoryProviderInstance.GetPointer() + (self->Count - 1))); }
		GDINL   ReverseConstIterator ReverseBegin() const { return   ReverseConstIterator(ConstIterator(self->MemoryProviderInstance.GetPointer() + (self->Count - 1))); }

		/// Returns iterator that points to preceding the first container element
		GDINL ReverseMutableIterator ReverseEnd  ()       { return ReverseMutableIterator(MutableIterator(self->MemoryProviderInstance.GetPointer() - 1)); }
		GDINL   ReverseConstIterator ReverseEnd  () const { return   ReverseConstIterator(  ConstIterator(self->MemoryProviderInstance.GetPointer() - 1)); }

		/// Returns number of elements that exist in vector.
		GDINL size_t GetSize    () const;
		/// Returns number of elements that can be placed into vector without reallocation.
		GDINL size_t GetCapacity() const;

		/// Resizes vector to make it contain specified number of elements.
		inline void Resize(size_t const NewElementsCount, ElementType const& InitialElement = ElementType());
		/// Reserves memory for vector to make it contain specified number of elements without reallocation when calling Resize method.
		inline void Reserve(size_t const Capacity);
		/// Reserves memory for vector to make it contain best fitting number of elements to predicted new size
		inline void ReserveToSize(size_t const NewSize);

		/// Destroys all elements in container without memory deallocation.
		inline void Emptify();
		/// Destroys all elements in container with memory deallocation.
		inline void Clear();

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
		inline void				 PushLast(ElementType const& NewElement = ElementType());
		
		/// Removes last element from container.
		inline void				 PopLast();

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

		/// Sorts all elements in collection by predicate that contains boolean returning () operator overloaded using Quick Sort.
		/// If functor returns true that this object should locate upper in collection.
		template<typename SortingPredicateType>
		inline void				 Sort(SortingPredicateType const& SortingPredicate, MutableIterator const Left, MutableIterator const Right);
		template<typename SortingPredicateType>
		inline void				 Sort(SortingPredicateType const& SortingPredicate);
		inline void				 Sort();

	public /* Operators */:
		/// @note Assignment operators does not change capacity if it is not required.
		inline Vector& operator= (Vector     && OtherVector);
		inline Vector& operator= (Vector const& OtherVector);
		inline Vector& operator= (std::initializer_list<ElementType> const& InitializerList);
#if (defined(__cplusplus_cli))
		inline Vector& operator= (array<ElementType>^ CliArray);
#endif	// if (defined(__cplusplus_cli))

		inline bool operator>  (Vector const& OtherVector) const;
		inline bool operator<  (Vector const& OtherVector) const;
		inline bool operator== (Vector const& OtherVector) const;
		inline bool operator!= (Vector const& OtherVector) const;
		inline bool operator>= (Vector const& OtherVector) const;
		inline bool operator<= (Vector const& OtherVector) const;

		GDINL ElementType const& operator[] (size_t const Index) const;
		GDINL ElementType      & operator[] (size_t const Index);

#if (!defined(GD_DOCUMENTATION)) 
	private /* STL compatibility */:	
		GDINL friend MutableIterator begin(Vector      & some_vector) { return some_vector.Begin(); }
		GDINL friend   ConstIterator begin(Vector const& some_vector) { return some_vector.Begin(); }
		GDINL friend MutableIterator end  (Vector      & some_vector) { return some_vector.End(); }
		GDINL friend   ConstIterator end  (Vector const& some_vector) { return some_vector.End(); }

        inline friend void Swap(Vector& First, Vector& Second)
		{	using GD Swap;	/// @todo Does not compiles without this line. Do not know the reason.
			Swap(First.MemoryProviderInstance, Second.MemoryProviderInstance);
			Swap(First.Count,                  Second.Count);
		}
#endif	// if (!defined(GD_DOCUMENTATION))
	};	// class Vector

	/// Heap vector.
	template<typename ElementType>
	using HeapVector = Vector<ElementType, VectorMemoryProviders::HeapMemoryProvider<ElementType>>;

	/// Stack vector. Same as Vector container but does not allows dynamically change capacity.
	template<typename ElementType, size_t const Capacity>
	using StackVector = Vector<ElementType, VectorMemoryProviders::StackMemoryProvider<ElementType, Capacity>>;

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Containers/Vector/Vector.inl>

#	 define  GD_USE_BOOL_VECTOR
#if (defined(GD_USE_BOOL_VECTOR) && (!defined(GD_DOCUMENTATION)))
#	include <GoddamnEngine/Core/Containers/Vector/BoolVector.h>
#endif	// if (defined(GD_USE_BOOL_VECTOR))

#endif
