/* ******************************************** */
/* File Name: dict.c                            */
/* Author: (C) 2008-2009 - Juan José Ponteprino */
/* Description: Dictionary API Headers          */
/* License: This code is licenced as LGPL       */
/* ******************************************** */

/* --------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* --------------------------------------------------------------------------- */

#ifndef __GNUC__
#define inline __inline
#endif

/* --------------------------------------------------------------------------- */

#include "dict.h"

/* --------------------------------------------------------------------------- */

static DICT_T * _DICT_table = NULL;
static DICT_T * _DICT_table_hashed[ DICT_HASH_MAX ] = { NULL } ;

/* --------------------------------------------------------------------------- */

static int make_hash8( char * key )
{
    int hash = 0, i;

    for ( i = 0; key[i]; i++ )
#ifdef USE_ALTERNATIVE_MODE
        hash += key[i] ^( hash << DICT_HASH_SHIFT8 );
#else
        hash ^= key[i] + ( hash << DICT_HASH_SHIFT8 ); /* Best dispersion */
#endif

    return ( hash & DICT_HASH_MASK8 );
}

/* --------------------------------------------------------------------------- */

static int make_hash16( char * key )
{
    int hash = 0, i;

    for ( i = 0; key[i]; i++ )
#ifdef USE_ALTERNATIVE_MODE
        hash += key[i] ^( hash << DICT_HASH_SHIFT16 );
#else
        hash ^= key[i] + ( hash << DICT_HASH_SHIFT16 ); /* Best dispersion */
#endif

    return ( hash & DICT_HASH_MASK16 );
}

/* --------------------------------------------------------------------------- */

#ifdef USE_HASH8
#define make_hash   make_hash8
#else
#define make_hash   make_hash16
#endif

/* --------------------------------------------------------------------------- */

#define ihash8(x)   ((x)&0xff)

/* --------------------------------------------------------------------------- */

void DICT_Init()
{
}

/* --------------------------------------------------------------------------- */

void DICT_Exit()
{
}

/* --------------------------------------------------------------------------- */

DICT_T * DICT_Create( char * id )
{
    DICT_T * dict = bgd_calloc( 1, sizeof( DICT_T ) );

    if ( dict )
    {
        int hash = make_hash( id );

        /* Fill struct */

        dict->id = bgd_strdup( id );
        dict->hash = hash;

        /* Insert dict in table */

        dict->prev = NULL;
        dict->next = _DICT_table;
        if ( _DICT_table ) _DICT_table->prev = dict;
        _DICT_table = dict;

        /* Insert dict in hashed table */

        dict->prev_hashed = NULL;
        dict->next_hashed = _DICT_table_hashed[hash];
        if ( _DICT_table_hashed[ hash ] ) _DICT_table_hashed[ hash ]->prev_hashed = dict;
        _DICT_table_hashed[ hash ] = dict ;

        /* Cleanup entries structs */

        dict->entries = NULL;
        memset( dict->entries_hashed, '\0', sizeof( dict->entries_hashed ) );
    }

    return ( dict );
}

/* --------------------------------------------------------------------------- */

int DICT_Destroy( DICT_T * dict )
{
    if ( dict )
    {
        /* Remove dict from table */

        if ( dict->next ) dict->next->prev = dict->prev ;
        if ( dict->prev ) dict->prev->next = dict->next ;
        else _DICT_table = dict->next ; /* No prev, then this is first _DICT_table element */

        /* Remove dict from hashed table */

        if ( dict->next_hashed ) dict->next_hashed->prev_hashed = dict->prev_hashed ;
        if ( dict->prev_hashed ) dict->prev_hashed->next_hashed = dict->next_hashed ;
        else _DICT_table_hashed[ dict->hash ] = dict->next_hashed ; /* No prev, then this is first _DICT_table_hashed element */

        /* Destroy entries */

        DICT_DelAllEntries( dict );

        /* Free data  */

        bgd_free( dict->id );

        bgd_free( dict );
    }

    return ( 0 );
}

/* --------------------------------------------------------------------------- */

static inline DICT_ENTRY_T * __DICT_FINDENTRY( DICT_T * dict, char * key, int hash )
{
    DICT_ENTRY_T * entry = dict->entries_hashed[ hash ] ;

    while ( entry )
    {
        if ( !strcmp( entry->key, key ) ) return ( entry );
        entry = entry->next_hashed;
    }

    return ( NULL );
}

/* --------------------------------------------------------------------------- */

#define __DICT_SET_DATA \
    if ( flags & DICT_ENTRY_FLAG_DONT_ALLOC ) \
    { \
        entry->val = value; \
    } \
    else \
    { \
        entry->val = bgd_malloc( len ); \
        memmove( entry->val, value, len ); \
    } \
    \
    entry->flags = flags; \
    entry->len = len;

/* --------------------------------------------------------------------------- */

int DICT_AddEntry( DICT_T * dict, char * key, void * value, int len, int flags )
{
    DICT_ENTRY_T * entry;

    if ( dict && key )
    {
        int hash = make_hash( key );

        if ( ( entry = __DICT_FINDENTRY( dict, key, hash ) ) )
        {
            if ( !( entry->flags & DICT_ENTRY_FLAG_DONT_ALLOC ) && entry->val ) bgd_free( entry->val );

            __DICT_SET_DATA;

            return 0;
        }
        else
        {
            if ( ( entry = bgd_malloc( sizeof( DICT_ENTRY_T ) ) ) )
            {
                if ( flags & DICT_ENTRY_FLAG_DONT_ALLOC )
                {
                    entry->key = key;
                }
                else
                {
                    if ( !( entry->key = bgd_strdup( key ) ) )
                    {
                        bgd_free( entry );
                        return -1;
                    }
                }

                __DICT_SET_DATA;

                /* Insert entry in table */

                entry->prev = NULL;
                entry->next = dict->entries;
                if ( dict->entries ) dict->entries->prev = entry;
                dict->entries = entry;

                /* Insert entry in hashed table */

                entry->prev_hashed = NULL;
                entry->next_hashed = dict->entries_hashed[ hash ];
                if ( dict->entries_hashed[ hash ] ) dict->entries_hashed[ hash ]->prev_hashed = entry;
                dict->entries_hashed[ hash ] = entry ;

                return 0;
            }
        }
    }

    return -1;
}

/* --------------------------------------------------------------------------- */

void * DICT_GetEntry( DICT_T * dict, char * key, int * len )
{
    DICT_ENTRY_T * entry;

    if ( dict && key )
    {
        int hash = make_hash( key );
        if (( entry = __DICT_FINDENTRY( dict, key, hash ) ) )
        {
            if ( len ) *len = entry->len ;
            return ( entry->val );
        }
    }

    return ( NULL );
}

/* --------------------------------------------------------------------------- */

void DICT_DelEntry( DICT_T * dict, char * key )
{
    int hash = make_hash( key );
    DICT_ENTRY_T * entry = __DICT_FINDENTRY( dict, key, hash ) ;

    if ( !entry ) return ;

    if ( !( entry->flags & DICT_ENTRY_FLAG_DONT_ALLOC ) && entry->val ) bgd_free( entry->val );

    /* Remove entry from table */

    if ( entry->next ) entry->next->prev = entry->prev ;
    if ( entry->prev ) entry->prev->next = entry->next ;
    else dict->entries = entry->next ; /* No prev */

    /* Remove entry from hashed table */

    if ( entry->next_hashed ) entry->next_hashed->prev_hashed = entry->prev_hashed ;
    if ( entry->prev_hashed ) entry->prev_hashed->next_hashed = entry->next_hashed ;
    else dict->entries_hashed[ hash ] = entry->next_hashed ; /* No prev */

    bgd_free( entry->key );
    bgd_free( entry );
}

/* --------------------------------------------------------------------------- */

void DICT_DelAllEntries( DICT_T * dict )
{
    DICT_ENTRY_T * entry = NULL;
    while (( entry = DICT_GetNextEntry( dict, NULL ) ) ) DICT_DelEntry( dict, entry->key );
}

/* --------------------------------------------------------------------------- */

DICT_ENTRY_T * DICT_FindEntry( DICT_T * dict, char * key )
{
    int hash = make_hash( key );
    return ( __DICT_FINDENTRY( dict, key, hash ) );
}

/* --------------------------------------------------------------------------- */

int DICT_Merge( DICT_T * target, DICT_T * source )
{
    DICT_ENTRY_T * entry = NULL ;

    while (( entry = DICT_GetNextEntry( source, entry ) ) )
    {
        DICT_AddEntry( target, entry->key, entry->val, entry->len, DICT_ENTRY_FLAG_NORMAL );
    }

    return 0;
}

/* --------------------------------------------------------------------------- */

DICT_ENTRY_T * DICT_GetNextEntry( DICT_T * dict, DICT_ENTRY_T * current )
{
    if ( !current ) return ( dict->entries );
    return ( current->next );
}

/* --------------------------------------------------------------------------- */

DICT_ENTRY_T * DICT_GetPrevEntry( DICT_T * dict, DICT_ENTRY_T * current )
{
    if ( !current )
    {
        current = dict->entries;
        while ( current->next ) current = current->next;
        return ( current );
    }

    return ( current->prev );
}

/* --------------------------------------------------------------------------- */

void DICT_ShowAllEntries( DICT_T * dict, FILE * fp )
{
    DICT_ENTRY_T * entry = NULL;

    while (( entry = DICT_GetNextEntry( dict, entry ) ) )
    {
        fprintf( fp, "[%s]=[", entry->key );
        fwrite( entry->val, 1, entry->len, fp );
        fprintf( fp, "]\n" );
    }
}

/* --------------------------------------------------------------------------- */
