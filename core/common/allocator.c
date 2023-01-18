#include "allocator.h"
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>

struct Header
{
    size_t allocated_size;
    size_t used_size;
};

void* bgd_malloc( size_t size )
{
    //void* desired_addres = (void*)(4*1024*1024);
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

void *bgd_calloc( size_t num, size_t size )
{
    return bgd_malloc(num*size);
}

void *bgd_realloc( void *p, size_t new_size )
{
    void* new_ptr = bgd_malloc(new_size);
    if (p)
    {
        struct Header* header = p;
        --header;
        size_t copy_size = new_size < header->used_size ? new_size : header->used_size;
        memcpy(new_ptr, p, copy_size);
    }
    
    return new_ptr;
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

char* bgd_strdup(const char* s)
{
    size_t size = strlen(s)+1;
    char * result = (char*)bgd_malloc(size);
    memcpy(result, s, size);
    return result;
}

