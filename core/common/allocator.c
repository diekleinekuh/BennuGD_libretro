#include "allocator.h"
#include <assert.h>

#if 1

#define __HAIKU__ 1 // This prevents changing the tls-model to initial-exec which doesn't work in a library
#include "rpmalloc.c"
#undef __HAIKU__

static char* address = 0x0;

static void* memory_map_at_address(size_t size, size_t* offset, void* desired_address)
{
#if PLATFORM_WINDOWS
	//Ok to MEM_COMMIT - according to MSDN, "actual physical pages are not allocated unless/until the virtual addresses are actually accessed"
	void* ptr = VirtualAlloc(desired_address, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!ptr) {
		return 0;
	}
#else
	int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED | MAP_FIXED;

	void* ptr = mmap(desired_address, size, PROT_READ | PROT_WRITE, flags, -1, 0);
	if ((ptr == MAP_FAILED) || !ptr) {
	 	return 0;
	}
#endif

	_rpmalloc_stat_add(&_mapped_pages_os, (int32_t)((size) >> _memory_page_size_shift));
	return ptr;
}

static void* memory_map(size_t size, size_t* offset)
{
    char* current_address=address;
    char* end_address = address+0x100000000;
    void* result;
    do 
    {
        void * desired_address = (void*)(((uintptr_t)current_address)&0xFFFFFFFF);
        result = memory_map_at_address(size, offset,  desired_address);
        current_address += _memory_span_size;
    }
    while(result==NULL && current_address<end_address);

    address = (char*)result + size;

    return result;
}

static void memory_unmap(void* address, size_t size, size_t offset, size_t release)
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
	if (!VirtualFree(address, release ? 0 : size, release ? MEM_RELEASE : MEM_DECOMMIT)) {
		rpmalloc_assert(0, "Failed to unmap virtual memory block");
	}
#else
	if (release) {
		if (munmap(address, release)) {
			rpmalloc_assert(0, "Failed to unmap virtual memory block");
		}
	} else {
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
	}
#endif
	if (release)
		_rpmalloc_stat_sub(&_mapped_pages_os, release >> _memory_page_size_shift);
}

void bgd_malloc_initialize()
{
	rpmalloc_set_main_thread();	

    rpmalloc_config_t config = {
        .memory_map = &memory_map,
        .memory_unmap = &memory_unmap
    };

    rpmalloc_initialize_config(&config);
}

void bgd_malloc_cleanup()
{
    rpmalloc_finalize();
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

#else




struct Header
{
    size_t allocated_size;
    size_t used_size;
};

#ifdef _WIN32
#include <Windows.h>
#include <Memoryapi.h>

const size_t page_size = 0x1000;
const char* limit = (const char*)(1ull << 32 );

char* address = (char*)0x1000;

void* bgd_malloc( size_t size )
{
    const size_t pages = (size + sizeof(struct Header)+ page_size - 1) / page_size;
    const size_t allocated_size = pages * page_size;


    while (address + size < limit)
    {
        void* result = VirtualAlloc(address, allocated_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        if (address == result)
        {
            struct Header* header = (struct Header*)address;
            header->allocated_size = allocated_size;
            header->used_size = size;

            address += allocated_size;

            return ++header;
        }

        address += page_size;
    }

    assert(0);
    return NULL;
}

void bgd_free( void *p )
{
    if (!p)
    {
        return;
    }

    struct Header* header = p;
    --header;

    BOOL result = VirtualFree(header, 0, MEM_RELEASE);
    assert(result);
}

#else

#include <sys/mman.h>
#include <stddef.h>
#include <string.h>

void* bgd_malloc( size_t size )
{
    void* desired_addres = NULL;

    size_t allocated_size = size+sizeof(struct Header);

    void* result = mmap(desired_addres, allocated_size, PROT_READ| PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_32BIT, -1, 0);

    if (result==MAP_FAILED)
    {
        return NULL;
    }

    struct Header* header = result;
    header->allocated_size=allocated_size;
    header->used_size=size;

    return ++header;
}

void bgd_free( void *p )
{
    if (!p)
    {
        return;
    }

     struct Header* header = p;
     --header;

    munmap(header, header->allocated_size);
}

#endif //_WIN32

void* bgd_realloc(void* p, size_t new_size)
{
    if (p)
    {
        struct Header* header = p;
        --header;
        if (new_size <= header->allocated_size-sizeof(header))
        {
            header->used_size = new_size;
            return p;
        }
        else
        {
            void* new_ptr = bgd_malloc(new_size);
            size_t copy_size = new_size < header->used_size ? new_size : header->used_size;
            memcpy(new_ptr, p, copy_size);
            return new_ptr;
        }
    }
    else
    {
        return bgd_malloc(new_size);
    }
}

void* bgd_calloc(size_t num, size_t size)
{
    return bgd_malloc(num * size);
}

void bgd_malloc_initialize()
{}
#endif

char* bgd_strdup(const char* s)
{
    size_t size = strlen(s) + 1;
    char* result = (char*)bgd_malloc(size);
    memcpy(result, s, size);
    return result;
}
