#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.hh>

#include <new>

#  if (defined(GD_PLATFORM_WINDOWS) || defined(GD_PLATFORM_WINDOWS))
#	include <malloc.h>
#elif (defined(GD_PLATFORM_OSX) || defined(GD_PLATFORM_IOS))
#	include <malloc/malloc.h>
#	define _msize malloc_size
#else
#	if (!defined(_msize))
#		error No _msize function was found
#	endif
#endif

GD_NAMESPACE_BEGIN

	//////////////////////////////////////////////////////////////////////////
	// Standard Pool specifications
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	GDINL handle AllocatorTraits<GD_ALLOCATOR_TYPE_STANDART>::AllocateMemory(
		_In_ size_t const allocationSize
	) 
	{ 
		return (malloc(allocationSize));
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL handle AllocatorTraits<GD_ALLOCATOR_TYPE_STANDART>::ReallocateMemory(
		_In_opt_ handle const memory,
		_In_	 size_t const allocationSize
	) 
	{ 
		return (realloc(memory, allocationSize));
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL void AllocatorTraits<GD_ALLOCATOR_TYPE_STANDART>::DeallocateMemory(
		_In_opt_ handle const memory
	) 
	{ 
		free(memory);
	}

	//////////////////////////////////////////////////////////////////////////
	GDINL size_t AllocatorTraits<GD_ALLOCATOR_TYPE_STANDART>::ObtainMemorySize(
		_In_opt_ chandle const memory
	) 
	{ 
		return ((memory != nullptr) ? _msize(const_cast<void*>(memory)) : 0);
	}

	//////////////////////////////////////////////////////////////////////////
	// AllocatorWithTraits specification
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	template<class AllocatorTraitsType /* = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT> */>
	GDINL handle AllocatorWithTraits<AllocatorTraitsType>::AllocateMemory(
		_In_ size_t const allocationSize
	)
	{
		if (allocationSize == 0)
		{
			return nullptr;
		}

		handle const memoryAllocated = AllocatorTraitsType::AllocateMemory(allocationSize);
		GD_ASSERT((memoryAllocated != nullptr), "Failed to allocate memory");

		return memoryAllocated;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class AllocatorTraitsType /* = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT> */>
	template<class BlockType>
	GDINL BlockType* AllocatorWithTraits<AllocatorTraitsType>::AllocateMemory(
		_In_ size_t const elementsCount
	)
	{
		BlockType* const memoryBlock = ((BlockType*)AllocatorWithTraits::AllocateMemory(sizeof(BlockType) * elementsCount));
		for (size_t cnt = 0; ((cnt < elementsCount) && (!TypeTraits::IsPodType<AllocatorTraitsType>::Value)); cnt += 1)
		{
			new (&memoryBlock[cnt]) BlockType;
		}

		return memoryBlock;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class AllocatorTraitsType /* = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT> */>
	GDINL handle AllocatorWithTraits<AllocatorTraitsType>::ReallocateMemory(
		_In_opt_ handle const memory,
		_In_	 size_t const allocationSize
	)
	{
		if (allocationSize == 0)
		{
			AllocatorWithTraits::DeallocateMemory(memory);
			return nullptr;
		}

		if (memory == nullptr)
		{
			return AllocatorWithTraits::AllocateMemory(allocationSize);
		}

		handle const memoryBlock = AllocatorTraitsType::ReallocateMemory(memory, allocationSize);
		GD_ASSERT((memoryBlock != nullptr), "Failed to reallocate memory");

		return memoryBlock;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class AllocatorTraitsType /* = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT> */>
	template<class BlockType>
	GDINL BlockType* AllocatorWithTraits<AllocatorTraitsType>::ReallocateMemory(
		_In_opt_ BlockType* const memory,
		_In_	 size_t const elementsCount
	)
	{
		if (TypeTraits::IsPodType<BlockType>::Value)
		{
			return ((BlockType*)AllocatorWithTraits::ReallocateMemory(((handle)memory), 
				(elementsCount * sizeof(BlockType))));
		}

		size_t const oldElementsCount = (AllocatorTraitsType::ObtainMemorySize(memory) / sizeof(BlockType));
		size_t const minElementsCount = Min(oldElementsCount, elementsCount);
		BlockType* const newMemory = ((BlockType*)AllocatorWithTraits::AllocateMemory((sizeof(BlockType) * elementsCount)));
		
		for (size_t cnt = 0; cnt < minElementsCount; cnt += 1)
		{
			#pragma warning(suppress: 6011)
			new (&newMemory[cnt]) BlockType(memory[cnt]);
		}

		for (size_t cnt = oldElementsCount; cnt < elementsCount; cnt += 1)
		{
			new (&newMemory[cnt]) BlockType;
		}

		AllocatorWithTraits::DeallocateMemory<BlockType>(memory);
		return newMemory;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class AllocatorTraitsType /* = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT> */>
	GDINL void AllocatorWithTraits<AllocatorTraitsType>::DeallocateMemory(
		_In_opt_ handle const memory
	)
	{
		if (memory == nullptr)
		{
			return;
		}

		AllocatorTraitsType::DeallocateMemory(memory);
	}

	////////////////////////////////////////////////////////////////////////
	template<class AllocatorTraitsType /* = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT> */>
	template<class BlockType>
	GDINL void AllocatorWithTraits<AllocatorTraitsType>::DeallocateMemory(
		_In_opt_ BlockType* const memory
	)
	{
		size_t const elementsCount = (AllocatorTraitsType::ObtainMemorySize(memory) / sizeof(BlockType));
		for (size_t cnt = 0; ((cnt < elementsCount) && (!TypeTraits::IsPodType<BlockType>::Value)); cnt += 1)
		{
			memory[cnt].~BlockType();
		}

		AllocatorWithTraits::DeallocateMemory((handle)memory);
	}

GD_NAMESPACE_END
