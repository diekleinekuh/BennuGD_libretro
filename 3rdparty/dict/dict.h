/* ******************************************** */
/* File Name: dict.h                            */
/* Author: (C) 2008-2009 - Juan José Ponteprino */
/* Description: Dictionary API Headers          */
/* License: This code is licenced as LGPL       */
/* ******************************************** */

#ifndef _DICT_H
#define _DICT_H

/* --------------------------------------------------------------------------- */

// Hash 8
#define DICT_HASH_MAX8      256
#define DICT_HASH_MASK8     0xff
#define DICT_HASH_SHIFT8    1

// Hash 16 (default)
#define DICT_HASH_MAX16     65536
#define DICT_HASH_MASK16    0xffff
#define DICT_HASH_SHIFT16   4

#ifdef USE_HASH8
// Hash 8
#define DICT_HASH_MAX       DICT_HASH_MAX8
#define DICT_HASH_MASK      DICT_HASH_MASK8
#define DICT_HASH_SHIFT     DICT_HASH_SHIFT8
#else
// Hash 16 (default)
#define DICT_HASH_MAX       DICT_HASH_MAX16
#define DICT_HASH_MASK      DICT_HASH_MASK16
#define DICT_HASH_SHIFT     DICT_HASH_SHIFT16
#endif

/* --------------------------------------------------------------------------- */

#define DICT_ENTRY_FLAG_NORMAL      0x00000000
#define DICT_ENTRY_FLAG_DONT_ALLOC  0x00000001

/* --------------------------------------------------------------------------- */

typedef struct DICT_ENTRY_T
{
    char * key;

    long len;
    void * val;

    int flags;

    struct DICT_ENTRY_T * prev;
    struct DICT_ENTRY_T * next;

    struct DICT_ENTRY_T * prev_hashed;
    struct DICT_ENTRY_T * next_hashed;

} DICT_ENTRY_T;

typedef struct DICT_T
{
    char * id;
    int hash;

    DICT_ENTRY_T * entries;
    DICT_ENTRY_T * entries_hashed[DICT_HASH_MAX];

    struct DICT_T * prev;
    struct DICT_T * next;

    struct DICT_T * prev_hashed;
    struct DICT_T * next_hashed;

} DICT_T;

/* --------------------------------------------------------------------------- */

extern void DICT_Init();
extern void DICT_Exit();

extern DICT_T * DICT_Create( char * id );
extern int DICT_Destroy( DICT_T * dict );

extern int DICT_AddEntry( DICT_T * dict, char * key, void * value, int len, int flags );
extern void * DICT_GetEntry( DICT_T * dict, char * key, int * len );
extern DICT_ENTRY_T * DICT_FindEntry( DICT_T * dict, char * key );

extern void DICT_DelEntry( DICT_T * dict, char * key );
extern void DICT_DelAllEntries( DICT_T * dict );
extern int DICT_Merge( DICT_T * target, DICT_T * source );

extern DICT_ENTRY_T * DICT_GetNextEntry( DICT_T * dict, DICT_ENTRY_T * current );
extern DICT_ENTRY_T * DICT_GetPrevEntry( DICT_T * dict, DICT_ENTRY_T * current );

extern void DICT_ShowAllEntries( DICT_T * dict, FILE * fp );

/* --------------------------------------------------------------------------- */

#endif
