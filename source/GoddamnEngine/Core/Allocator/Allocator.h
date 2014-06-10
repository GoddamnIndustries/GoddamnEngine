#ifndef GD_CORE_ALLOCATOR_ALLOCATOR
#define GD_CORE_ALLOCATOR_ALLOCATOR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/TypeTraits.h>
#include <GoddamnEngine/Core/Object/Singleton/Singleton.h>

GD_NAMESPACE_BEGIN

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Enumeration describing supported allocator types
	enum AllocatorType : UInt8
	{
		//////////////////////////////////////////////////////////////////////////
		/// Allocator uses standard system allocation method, such as malloc
		/// realloc and free
		/// Pros: valid almost everywhere
		/// Cons: very slow
		GD_ALLOCATOR_TYPE_STANDART,

		//////////////////////////////////////////////////////////////////////////
		/// Allocator uses custom memory management functions created by engine team
		/// Pros: very fast allocations, using SIMD to provide awesome performance
		/// Cons: valid only on x86-64 platforms
		/// @note Unimplemented
		GD_ALLOCATOR_TYPE_CUSTOM = GD_ALLOCATOR_TYPE_STANDART,

		//////////////////////////////////////////////////////////////////////////
		/// Default allocator available on build platform
#if	  (defined(GD_PLATFORM_DESKTOP))
		GD_ALLOCATOR_TYPE_DEFAULT = GD_ALLOCATOR_TYPE_CUSTOM
#elif (defined(GD_PLATFORM_MOBILE) || defined(GD_PLATFORM_CONSOLE))
		GD_ALLOCATOR_TYPE_DEFAULT = GD_ALLOCATOR_TYPE_TLSF
#else
		GD_ALLOCATOR_TYPE_DEFAULT = GD_ALLOCATOR_TYPE_STANDART
#endif
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Implementation of allocator traits class for specific 
	/// allocator types. This class provides just simple implementation
	/// for replacement of system functions, such as malloc, realloc,
	/// free, _msize
	template<AllocatorType const SpecifiedAllocatorType>
	struct AllocatorTraits;

	//////////////////////////////////////////////////////////////////////////
	/// Implementation of allocator traits class for STD allocator
	template<>
	struct AllocatorTraits<GD_ALLOCATOR_TYPE_STANDART>
	{
		//////////////////////////////////////////////////////////////////////////
		GDINL static handle	AllocateMemory(
			_In_	 size_t const allocationSize
		); 
		
		//////////////////////////////////////////////////////////////////////////
		GDINL static handle	ReallocateMemory(
			_In_opt_ handle const memory,
			_In_	 size_t const allocationSize
		);
		
		//////////////////////////////////////////////////////////////////////////
		GDINL static void	DeallocateMemory(
			_In_opt_ handle const memory
		);

		//////////////////////////////////////////////////////////////////////////
		GDINL static size_t	ObtainMemorySize(
			_In_opt_ chandle const memory
		);
	};

#if 0
	//////////////////////////////////////////////////////////////////////////
	/// Implementation of allocator traits class for custom allocator
	template<>
	struct AllocatorTraits<GD_ALLOCATOR_TYPE_CUSTOM>
	{
		//////////////////////////////////////////////////////////////////////////
		GDINL static handle	AllocateMemory(
			_In_	 size_t const allocationSize
		);

		//////////////////////////////////////////////////////////////////////////
		GDINL static handle	ReallocateMemory(
			_In_opt_ handle const memory,
			_In_	 size_t const allocationSize
		);

		//////////////////////////////////////////////////////////////////////////
		GDINL static void	DeallocateMemory(
			_In_opt_ handle const memory
		);

		//////////////////////////////////////////////////////////////////////////
		GDINL static size_t	ObtainMemorySize(
			_In_opt_ chandle const memory
		);
	};
#endif

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Wrapper around default allocator with support of non-POD types
	template<class AllocatorTraitsType = AllocatorTraits<GD_ALLOCATOR_TYPE_DEFAULT>>
	class AllocatorWithTraits final 
	{
	private:
		GD_CLASS_UNASSIGNABLE(AllocatorWithTraits);
		GD_CLASS_UNCOPIABLE(AllocatorWithTraits);

		GDINL  AllocatorWithTraits() { }
		GDINL ~AllocatorWithTraits() { }
		
	public:
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// Allocates block of memory with specified size and returns pointer to it
		/// @note This function never returns nullptr except specified size is 0
		GDINL static handle AllocateMemory(
			_In_	 size_t const allocationSize
		);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// Allocates block of memory with specified block type and number of elements 
		/// and returns pointer to it
		/// @note This function never returns nullptr except specified count is 0
		template<class BlockType>
		GDINL static BlockType* AllocateMemory(
			_In_	 size_t const elementsCount
		);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// Reallocates block of memory to specified size
		/// @note This function never returns nullptr except specified size is 0
		GDINL static handle ReallocateMemory(
			_In_opt_ handle const memory,
			_In_	 size_t const allocationSize
		);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// Reallocates block of memory with specified block type to specified elements count
		/// @note This function never returns nullptr except specified count is 0
		template<class BlockType>
		GDINL static BlockType* ReallocateMemory(
			_In_opt_ BlockType* const memory,
			_In_	 size_t const elementsCount
		);
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// Deallocates block of memory
		/// @note If specified block is nullptr then does nothing
		GDINL static void DeallocateMemory(
			_In_opt_ handle const memory
		);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// Deallocates block of memory with specified block type
		/// @note If specified block is nullptr then does nothing
		template<class BlockType>
		GDINL static void DeallocateMemory(
			_In_opt_ BlockType* const memory
		);
	};

	typedef AllocatorWithTraits<> Allocator;
	/*typedef AllocatorWithTraits<AllocatorTraits<GD_ALLOCATOR_TYPE_STANDART>> AllocatorStandart;
	typedef AllocatorWithTraits<AllocatorTraits<GD_ALLOCATOR_TYPE_CUSTOM>> AllocatorCustom;
	typedef AllocatorWithTraits<AllocatorTraits<GD_ALLOCATOR_TYPE_TLSF>> AllocatorTlsf;*/

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Allocator/Allocator.inl>

#endif
