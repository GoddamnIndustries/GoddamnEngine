#ifndef GD_CORE_ALLOCATOR_OPERATOR_NEW_DELETE
#define GD_CORE_ALLOCATOR_OPERATOR_NEW_DELETE

#include <GoddamnEngine/Include.hh>
#include <new>

GD_NAMESPACE_BEGIN
	// GDINT void* operator new(size_t const size);
	// GDINL void* operator new(size_t const size, void* const pointer){ return pointer; }
	// GDINT void  operator delete(void* const pointer);

#if (defined(GD_DEBUG) && defined(GD_DEBUG_ALLOCATION))
	GDAPI void* operator new(size_t const size, char const* const file, int const line);
	GDAPI void  operator delete(void* const pointer);
#endif

GD_NAMESPACE_END

#if (defined(GD_DEBUG) && defined(GD_DEBUG_ALLOCATION))
#	define GD_NEW new(__FILE__, __LINE__)
#	define new GD_NEW
#endif

#endif