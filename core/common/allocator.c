#include "allocator.h"

#if SIZE_MAX>UINT32_MAX // check for 64 bit system

#include <assert.h>
#include <stdatomic.h>
#include <stddef.h>

uint8_t* allocated_chunk=NULL;

#if LIBRETRO_CORE
#include "libretro.h"
extern retro_log_printf_t log_cb;
#else
enum retro_log_level
{
   RETRO_LOG_DEBUG = 0,
   RETRO_LOG_INFO,
   RETRO_LOG_WARN,
   RETRO_LOG_ERROR,
   RETRO_LOG_DUMMY = INT_MAX
};

void log_cb(enum retro_log_level level, const char *fmt, ...)
{
	va_list arg;
    int cnt;
    va_start(arg, format);
	vfprintf(level==RETRO_LOG_ERROR ? sterr : stdout, fmt, arg);
	va_end(arg);
}
#endif

#ifdef __SWITCH__
#include <switch.h>
#define NO_RPMALLOC 1

int bgd_malloc_initialize()
{
	u64 heap_address=0;
	Result heap_address_result = svcGetInfo( &heap_address, InfoType_HeapRegionAddress,CUR_PROCESS_HANDLE, 0);
	if (R_FAILED(heap_address_result))
	{
		log_cb(RETRO_LOG_ERROR, "svcGetInfo: failed to get HeapRegionAddress: %u\n", heap_address_result);
		return false;
	}

	if (heap_address==0)
	{
		log_cb(RETRO_LOG_ERROR, "svcGetInfo: HeapRegionAddress is NULL\n");
		return false;
	}

	allocated_chunk = (void*)heap_address;
	log_cb(RETRO_LOG_DEBUG, "heap_address = %p\n", allocated_chunk);
	return true;
}

void bgd_malloc_cleanup()
{
}
#endif

#if NO_RPMALLOC
#include <stdlib.h>
#include <string.h>
#define rpmalloc malloc
#define rpcalloc calloc
#define rprealloc realloc
#define rpfree free
#else
#define TLS_MODEL
//#define ENABLE_THREAD_CACHE 0
//#define ENABLE_GLOBAL_CACHE 0
//#define RPMALLOC_FIRST_CLASS_HEAPS 1
#define ENABLE_PRELOAD 1
#include "rpmalloc.c"

_Atomic ptrdiff_t allocation_offset = 0;

static void* bgd_mmap(size_t size, size_t* offset)
{
	// taken from rpmalloc implementation
	size_t alignment = ((size >= _memory_span_size) && (_memory_span_size > _memory_map_granularity)) ? _memory_span_size : 0;
	rpmalloc_assert(size >= _memory_page_size, "Invalid mmap size");

    // get a virtual address
	void* requested_address = allocated_chunk + atomic_fetch_add_explicit(&allocation_offset, size + alignment,  memory_order_relaxed);

	// adjust virtual address to match alignment
	if (alignment)
	{
		size_t final_padding = alignment - ((uintptr_t)requested_address & ~_memory_span_mask);
		rpmalloc_assert(final_padding <= _memory_span_size, "Internal failure in padding");
		rpmalloc_assert(final_padding <= padding, "Internal failure in padding");
		rpmalloc_assert(!(final_padding % 8), "Internal failure in padding");
		requested_address = pointer_offset(requested_address, final_padding);
	}	

#if PLATFORM_WINDOWS
	void* ptr = VirtualAlloc(requested_address, size, MEM_COMMIT, PAGE_READWRITE);
	if (!ptr)
	{
        rpmalloc_assert(ptr, "Failed to map virtual memory block");
		return 0;
	}
#elif PLATFORM_POSIX
    if (mprotect(requested_address, size, PROT_READ | PROT_WRITE))
    {
		rpmalloc_assert((ptr != MAP_FAILED) && ptr, "Failed to map virtual memory block");
		return 0;
	}
    void* ptr = requested_address;
#else
	void* ptr = requested_address;
#endif
	log_cb(RETRO_LOG_DEBUG, "bgd_mmap address=%p size=%d\n", ptr, size);
	return ptr;
}

static void bgd_unmap(void* address, size_t size, size_t offset, size_t release)
{
	if (size==0 && release==0)
	{
		return;
	}

	log_cb(RETRO_LOG_DEBUG, "bgd_unmap address=%p size=%u offset=%u release=%u\n", address, size, offset, release);

	if (release)
	{
		size = release;
	}

#if PLATFORM_WINDOWS
    if (!VirtualFree(address, size, MEM_DECOMMIT))
	{
		rpmalloc_assert(0, "Failed to unmap virtual memory block");
	}
#elif PLATFORM_POSIX
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
}

size_t reseved_address_space = 4294967296ull;

int bgd_malloc_initialize()
{
	log_cb(RETRO_LOG_DEBUG, "bgd_malloc_initialize\n");
    // Reserve (not commit) 4GB address space.
    // Later all allocated memory will be served by pages committed in this window.
    // This makes it posible to use 32 bit offset addressing relative to the start of the window.
#if PLATFORM_WINDOWS
	allocated_chunk = VirtualAlloc(0, reseved_address_space, MEM_RESERVE, PAGE_READWRITE);
#elif PLATFORM_POSIX
    allocated_chunk = mmap(NULL, reseved_address_space, PROT_NONE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_UNINITIALIZED, 0, 0 );
#else
	reseved_address_space = 1024 * 1024 * 256;
	allocated_chunk = malloc(reseved_address_space);
#endif

    assert(allocated_chunk);
	if (!allocated_chunk)
	{
		return false;
	}

	log_cb(RETRO_LOG_DEBUG, "rpmalloc_set_main_thread\n");
	rpmalloc_set_main_thread();

    rpmalloc_config_t config = {
        .memory_map = &bgd_mmap,
        .memory_unmap = &bgd_unmap,
		//.span_map_count =1,
    };
	log_cb(RETRO_LOG_DEBUG, "rpmalloc_initialize_config\n");
    rpmalloc_initialize_config(&config);
	
	log_cb(RETRO_LOG_DEBUG, "bgd_malloc_initialize complete\n");
	return true;
}

void bgd_malloc_cleanup()
{
	log_cb(RETRO_LOG_DEBUG, "bgd_malloc_cleanup\n");

#if RPMALLOC_FIRST_CLASS_HEAPS
	log_cb(RETRO_LOG_DEBUG, "rpmalloc_heap_free_all\n");
	for (size_t list_idx = 0; list_idx < HEAP_ARRAY_SIZE; ++list_idx) {
		heap_t* heap = _memory_heaps[list_idx];
		while (heap) {
			heap_t* next_heap = heap->next_heap;
			rpmalloc_heap_free_all(heap);
			heap = next_heap;
		}
	}
#endif
	log_cb(RETRO_LOG_DEBUG, "rpmalloc_finalize\n");
    rpmalloc_finalize();
#if PLATFORM_WINDOWS
	if (!VirtualFree(allocated_chunk, 0, MEM_RELEASE))
    {
        assert(0);
    }
#elif PLATFORM_POSIX
    if (munmap(allocated_chunk, reseved_address_space))
    {
        assert(0);
    }
#else
	free(allocated_chunk);
#endif
	log_cb(RETRO_LOG_DEBUG, "bgd_malloc_cleanup complete\n");
}

#endif // !NO_RPMALLOC

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

#endif