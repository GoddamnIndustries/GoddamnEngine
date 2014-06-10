#include <GoddamnEngine/Core/Allocator/Allocator.h>

#include <stdlib.h>
#include <string.h>

GD_NAMESPACE_BEGIN
 
	//void* operator new(size_t const size) { return Allocator::AllocateMemory(size); }
	//void  operator delete(void* const pointer) { Allocator::DeallocateMemory(pointer); }

#if 0
	void Allocator::ShiftRight(handle _memory, const size_t memorySize, const size_t from, const size_t to, const size_t offset)
	{
		byte* memory = (byte*)_memory;
		byte* result = (byte*)Allocator::Allocate(memorySize);

		// Copying front non-shifted data
		memcpy(result, memory, from);	

		// Copying back non-shifted data
		memcpy(result + (to + offset), memory + (to + offset), memorySize - (to + offset));	

		// Copying data, shifted left
		memcpy(result + from, memory + to, offset);

		// Copying data, shifted right
		memcpy(result + (from + offset), memory + from, to - from);

		// Copying result back
		memcpy(memory, result, memorySize);
		
		// Deallocating
		Allocator::Deallocate(result);
	}

	void Allocator::ShiftLeft(handle _memory, const size_t memorySize, const size_t from, const size_t to, const size_t offset)
	{
		byte* memory = (byte*)_memory;
		byte* result = (byte*)Allocator::Allocate(memorySize);

		// Copying front non-shifted data
		memcpy(result, memory, from - offset);	

		// Copying back non-shifted data
		memcpy(result + to, memory + to, memorySize - to);	

		// Copying data, shifted left
		memcpy(result + (from - offset), memory + from, to - from);

		// Copying data, shifted right
		memcpy(result + (to - offset), memory + (from - offset), offset);

		// Copying result back
		memcpy(memory, result, memorySize);
		
		// Deallocating
		Allocator::Deallocate(result);
	}
#endif

GD_NAMESPACE_END