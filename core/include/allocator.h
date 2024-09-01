#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

extern void* bgd_malloc( size_t size );
extern void *bgd_calloc( size_t num, size_t size );
extern void *bgd_realloc( void *p, size_t new_size );
extern void bgd_free( void *p );
extern char* bgd_strdup(const char* s);
extern void bgd_malloc_initialize();
extern void bgd_malloc_cleanup();

extern uint8_t* allocated_chunk;

static inline void* ptr_from_int(uint32_t input)
{
    if (input > 0)
        return allocated_chunk + input - 1;
    else
        return NULL;
}

static inline int32_t int_from_ptr(const void* ptr)
{
    if (!ptr)
        return 0;

    assert((uint8_t*)ptr >= allocated_chunk);
    assert(allocated_chunk != NULL);
    return (uint8_t*)ptr - allocated_chunk + 1;
}

#endif
