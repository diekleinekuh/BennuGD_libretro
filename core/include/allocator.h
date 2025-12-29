#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>
#include <stdint.h>

#if SIZE_MAX>UINT32_MAX // check for 64 bit system

#include <assert.h>

extern void* bgd_malloc( size_t size );
extern void *bgd_calloc( size_t num, size_t size );
extern void *bgd_realloc( void *p, size_t new_size );
extern void bgd_free( void *p );
extern char* bgd_strdup(const char* s);
extern int bgd_malloc_initialize();
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
#else // 32 bit system
#include <malloc.h>
#include <string.h>

static inline void* bgd_malloc( size_t size )
{
    return malloc(size);
}

static inline void *bgd_calloc( size_t num, size_t size )
{
    return calloc(num, size);
}

static inline void *bgd_realloc( void *p, size_t new_size )
{
    return realloc(p, new_size);
}

static inline void bgd_free( void *p )
{
    return free(p);
}

static inline char* bgd_strdup(const char* s)
{
#ifdef _MSC_VER
    return _strdup(s);
#else
    return strdup(s);
#endif
}

static inline int bgd_malloc_initialize()
{
    return 1;
}

static inline void bgd_malloc_cleanup()
{

}

static inline void* ptr_from_int(uint32_t input)
{
    return (void*)input;
}

static inline int32_t int_from_ptr(const void* ptr)
{
    return (int32_t)ptr;
}
#endif


#endif
