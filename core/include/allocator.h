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

extern void* ptr_from_int(uint32_t input);
extern int32_t int_from_ptr(const void* ptr);

//extern uint8_t* allocated_chunk;
//
//inline void* ptr_from_int(uint32_t input)
//{
//    return (void*)(size_t)input;
//
//    if (input>0)
//        return allocated_chunk + input - 1;
//    else
//        return NULL;
//
//    // uint32_t low_part = (uint32_t)(size_t)allocated_chunk;
//    // uint32_t high_part = ((size_t)allocated_chunk) & 0xFFFFFFFF00000000;
//    // if (input<low_part)
//    // {
//    //     high_part +=0x100000000;
//    // }
//
//    // size_t result = high_part |  (size_t)input;
//
//    // return (void*)result;
//}
//
//inline int32_t int_from_ptr(const void* ptr)
//{
//    assert((((size_t)ptr)&0xFFFFFFFF00000000) == 0);
//    return (int32_t)(size_t)ptr;
//
//    if (!ptr)
//        return 0;
//
//    assert(ptr>=allocated_chunk);
//    assert(allocated_chunk!=NULL);
//    return (uint8_t*)ptr - allocated_chunk + 1;
//    // assert((((size_t)ptr)>>32)==1);
//    // return ((size_t)ptr)&0xFFFFFFFF;
//}

#endif
