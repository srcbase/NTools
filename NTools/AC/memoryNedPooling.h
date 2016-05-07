#ifndef __MEMORYNEDPOOLING_H__
#define __MEMORYNEDPOOLING_H__
#pragma once
#include <limits>
namespace gtc {

/* Initial CPU stuff to set.
*/
#define GTC_CPU_UNKNOWN    0
#define GTC_CPU_X86        1
#define GTC_CPU_PPC        2
#define GTC_CPU_ARM        3
#define GTC_CPU_MIPS       4
/** Find perfect alignment (should supports SIMD alignment if SIMD available)
*/
#if GTC_CPU == GTC_CPU_X86
#   define GTC_SIMD_ALIGNMENT  16

#else
#   define GTC_SIMD_ALIGNMENT  16
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	nedPoolingImpl  ¿‡    2015-5-24
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class nedPoolingImpl
{
public:
	static void* allocBytes(size_t count, 
		const char* file, int line, const char* func);
	static void deallocBytes(void* ptr);
	static void* allocBytesAligned(size_t align, size_t count, 
		const char* file, int line, const char* func);
	static void deallocBytesAligned(size_t align, void* ptr);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	nedPoolingPolicy  ¿‡    2015-5-24
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class nedPoolingPolicy
{
public:
	static inline void* allocateBytes(size_t count, 
		const char* file = 0, int line = 0, const char* func = 0)
	{
		return nedPoolingImpl::allocBytes(count, file, line, func);
	}
	static inline void deallocateBytes(void* ptr)
	{
		nedPoolingImpl::deallocBytes(ptr);
	}
	/// Get the maximum size of a single allocation
	static inline size_t getMaxAllocationSize()
	{
		return std::numeric_limits<size_t>::max();
	}

private:
	// No instantiation
	nedPoolingPolicy()
	{ }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	nedPoolingAlignedPolicy  ¿‡    2015-5-24
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <size_t Alignment = 0>
class nedPoolingAlignedPolicy
{
public:
	// compile-time check alignment is available.
	typedef int isValidAlignment
		[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

	static inline void* allocateBytes(size_t count, 
		const char* file = 0, int line = 0, const char* func = 0)
	{
		return nedPoolingImpl::allocBytesAligned(Alignment, count, file, line, func);
	}

	static inline void deallocateBytes(void* ptr)
	{
		nedPoolingImpl::deallocBytesAligned(Alignment, ptr);
	}

	/// Get the maximum size of a single allocation
	static inline size_t getMaxAllocationSize()
	{
		return std::numeric_limits<size_t>::max();
	}
private:
	// no instantiation allowed
	nedPoolingAlignedPolicy()
	{ }
};


}	// end gtc
#endif	// end __MEMORYNEDPOOLING_H__