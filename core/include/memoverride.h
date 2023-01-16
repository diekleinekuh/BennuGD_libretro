#ifndef __MEMOVERRIDE_H__
#define __MEMOVERRIDE_H__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <sys/mman.h>
#include <stddef.h>
#include <string.h>

struct Header
{
    size_t allocated_size;
    size_t used_size;
};

static inline void* bgdi_malloc( size_t size )
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

static inline void *bgdi_calloc( size_t num, size_t size )
{
    return bgdi_malloc(num*size);
}

static inline void *bgdi_realloc( void *p, size_t new_size )
{
    void* new_ptr = bgdi_malloc(new_size);
    if (p)
    {
        struct Header* header = p;
        --header;
        size_t copy_size = new_size < header->used_size ? new_size : header->used_size;
        memcpy(new_ptr, p, copy_size);
    }
    
    return new_ptr;
}

static inline void bgdi_free( void *p )
{
    if (!p)
    {
        return;
    }

     struct Header* header = p;
     --header;

    munmap(header, header->allocated_size);
}

static inline const char* bgdi_strdup(const char* s)
{
    size_t size = strlen(s)+1;
    const char * result = (const char*)bgdi_malloc(size);
    memcpy(result, s, size);
    return result;
}

#define malloc(a) bgdi_malloc(a)
#define calloc(a,b) bgdi_calloc( a, b)
#define realloc(a,b) bgdi_realloc(a,b)
#define free(a) bgdi_free(a)
#define strdup(a) bgdi_strdup(a)

#endif
