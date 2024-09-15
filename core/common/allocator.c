#include "allocator.h"
#include <assert.h>
#include <stdatomic.h>
#include <stddef.h>

uint8_t* allocated_chunk=NULL;

#define __HAIKU__ 1 // This prevents changing the tls-model to initial-exec which doesn't work in a library
#include "rpmalloc.c"
#undef __HAIKU__

_Atomic ptrdiff_t allocation_offset = 0;

//! Copied from default implementation to map new pages to virtual memory
static void* bgd_mmap(size_t size, size_t* offset)
{
	//Either size is a heap (a single page) or a (multiple) span - we only need to align spans, and only if larger than map granularity
	size_t padding = ((size >= _memory_span_size) && (_memory_span_size > _memory_map_granularity)) ? _memory_span_size : 0;
	rpmalloc_assert(size >= _memory_page_size, "Invalid mmap size");

    //change offset of address to use
    void* requested_address;
    {
        ptrdiff_t expected = allocation_offset;
        ptrdiff_t desired;

        do
        {
            desired = expected + size + padding;
        }    
        while(!atomic_compare_exchange_weak(&allocation_offset, &expected, desired));

        requested_address = allocated_chunk + expected;
    }

#if PLATFORM_WINDOWS
	//Ok to MEM_COMMIT - according to MSDN, "actual physical pages are not allocated unless/until the virtual addresses are actually accessed"
	void* ptr = VirtualAlloc(requested_address, size + padding, MEM_COMMIT, PAGE_READWRITE);
	if (!ptr) {
        rpmalloc_assert(ptr, "Failed to map virtual memory block");
		return 0;
	}
#else
    if (mprotect(requested_address, size + padding, PROT_READ | PROT_WRITE))
    {
		rpmalloc_assert((ptr != MAP_FAILED) && ptr, "Failed to map virtual memory block");
		return 0;
	}
    void* ptr = requested_address;
#endif
	_rpmalloc_stat_add(&_mapped_pages_os, (int32_t)((size + padding) >> _memory_page_size_shift));
	if (padding) {
		size_t final_padding = padding - ((uintptr_t)ptr & ~_memory_span_mask);
		rpmalloc_assert(final_padding <= _memory_span_size, "Internal failure in padding");
		rpmalloc_assert(final_padding <= padding, "Internal failure in padding");
		rpmalloc_assert(!(final_padding % 8), "Internal failure in padding");
		ptr = pointer_offset(ptr, final_padding);
		*offset = final_padding >> 3;
	}
	rpmalloc_assert((size < _memory_span_size) || !((uintptr_t)ptr & ~_memory_span_mask), "Internal failure in padding");
    
	return ptr;
}

//! Copied from default implementation to unmap pages from virtual memory
static void bgd_unmap(void* address, size_t size, size_t offset, size_t release)
{
	rpmalloc_assert(release || (offset == 0), "Invalid unmap size");
	rpmalloc_assert(!release || (release >= _memory_page_size), "Invalid unmap size");
	rpmalloc_assert(size >= _memory_page_size, "Invalid unmap size");
	if (release && offset) {
		offset <<= 3;
		address = pointer_offset(address, -(int32_t)offset);
		if ((release >= _memory_span_size) && (_memory_span_size > _memory_map_granularity)) {
			//Padding is always one span size
			release += _memory_span_size;
		}
	}
#if PLATFORM_WINDOWS
	//if (!VirtualFree(address, release ? 0 : size, release ? MEM_RELEASE : MEM_DECOMMIT)) {
    if (!VirtualFree(address, size, MEM_DECOMMIT)) {
		rpmalloc_assert(0, "Failed to unmap virtual memory block");
	}
#else
	// if (release) {
	// 	if (munmap(address, release)) {
	// 		rpmalloc_assert(0, "Failed to unmap virtual memory block");
	// 	}
	// } else {
#if defined(MADV_FREE_REUSABLE)
		int ret;
		while ((ret = madvise(address, size, MADV_FREE_REUSABLE)) == -1 && (errno == EAGAIN))
			errno = 0;
		if ((ret == -1) && (errno != 0)) {
#elif defined(MADV_DONTNEED)
		if (madvise(address, size, MADV_DONTNEED)) {
#elif defined(MADV_PAGEOUT)
		if (madvise(address, size, MADV_PAGEOUT)) {
#elif defined(MADV_FREE)
		if (madvise(address, size, MADV_FREE)) {
#else
		if (posix_madvise(address, size, POSIX_MADV_DONTNEED)) {
#endif
			rpmalloc_assert(0, "Failed to madvise virtual memory block as free");
		}
	//}
#endif

	if (release)
		_rpmalloc_stat_sub(&_mapped_pages_os, release >> _memory_page_size_shift);
}

const size_t reseved_address_space = 4294967296ull;

void bgd_malloc_initialize()
{
    // Reserve (not commit) 4GB address space.
    // Later all allocated memory will be served by pages committed in this window.
    // This makes it posible to use 32 bit offset addressing relative to the start of the window.
#if PLATFORM_WINDOWS    	
	allocated_chunk = VirtualAlloc(0, reseved_address_space, MEM_RESERVE, PAGE_READWRITE);
    assert(allocated_chunk);
#else
    allocated_chunk = mmap(NULL, reseved_address_space, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_UNINITIALIZED, 0, 0 );
#endif

    assert(allocated_chunk);
	rpmalloc_set_main_thread();

    rpmalloc_config_t config = {
        .memory_map = &bgd_mmap,
        .memory_unmap = &bgd_unmap,
    };

    rpmalloc_initialize_config(&config);
}

void bgd_malloc_cleanup()
{
    rpmalloc_finalize();
#if PLATFORM_WINDOWS    	
	if (!VirtualFree(allocated_chunk, 0, MEM_RELEASE))
    {
        assert(false);
    }    
#else
    if (munmap(allocated_chunk, reseved_address_space))
    {
        assert(0);
    }
#endif
}

void* bgd_malloc( size_t size )
{
    return rpmalloc(size);
}

void bgd_free( void *p )
{
    rpfree(p);
}

void* bgd_realloc(void* p, size_t new_size)
{
    return rprealloc(p, new_size);
}

void* bgd_calloc(size_t num, size_t size)
{
    return rpcalloc(num, size);
}



char* bgd_strdup(const char* s)
{
    size_t size = strlen(s) + 1;
    char* result = (char*)bgd_malloc(size);
    memcpy(result, s, size);
    return result;
}
