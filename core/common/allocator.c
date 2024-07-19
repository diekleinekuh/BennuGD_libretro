#include "allocator.h"
#include <assert.h>

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

char* bgd_strdup(const char* s)
{
    size_t size = strlen(s) + 1;
    char* result = (char*)bgd_malloc(size);
    memcpy(result, s, size);
    return result;
}