#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stddef.h>

extern void* bgd_malloc( size_t size );
extern void *bgd_calloc( size_t num, size_t size );
extern void *bgd_realloc( void *p, size_t new_size );
extern void bgd_free( void *p );
extern char* bgd_strdup(const char* s);

#endif
