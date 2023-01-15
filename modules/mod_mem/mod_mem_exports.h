/*
 *  Copyright © 2006-2019 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

#ifndef __MOD_MEM_EXPORTS
#define __MOD_MEM_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* ----------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_mem, functions_exports )[] =
{
    /* Manipulacion de Memoria */
    FUNC( "MEM_CALLOC"      , "II"    , TYPE_POINTER, modmem_calloc         ),
    FUNC( "MEM_ALLOC"       , "I"     , TYPE_POINTER, modmem_alloc          ),
    FUNC( "MEM_FREE"        , "P"     , TYPE_INT    , modmem_free           ),
    FUNC( "MEM_REALLOC"     , "PI"    , TYPE_POINTER, modmem_realloc        ),
    FUNC( "MEM_CMP"         , "PPI"   , TYPE_INT    , modmem_memcmp         ),
    FUNC( "MEM_SET"         , "PBI"   , TYPE_INT    , modmem_memset         ),
    FUNC( "MEM_SETW"        , "PWI"   , TYPE_INT    , modmem_memsetw        ),
    FUNC( "MEM_SETI"        , "PII"   , TYPE_INT    , modmem_memseti        ),
    FUNC( "MEM_COPY"        , "PPI"   , TYPE_INT    , modmem_memcopy        ),
    FUNC( "MEM_MOVE"        , "PPI"   , TYPE_INT    , modmem_memmove        ),
    FUNC( "MEM_AVAILABLE"   , ""      , TYPE_INT    , modmem_memory_free    ),
    FUNC( "MEM_TOTAL"       , ""      , TYPE_INT    , modmem_memory_total   ),

    FUNC( "CALLOC"          , "II"    , TYPE_POINTER, modmem_calloc         ),
    FUNC( "ALLOC"           , "I"     , TYPE_POINTER, modmem_alloc          ),
    FUNC( "FREE"            , "P"     , TYPE_INT    , modmem_free           ),
    FUNC( "REALLOC"         , "PI"    , TYPE_POINTER, modmem_realloc        ),
    FUNC( "MEMCMP"          , "PPI"   , TYPE_INT    , modmem_memcmp         ),
    FUNC( "MEMSET"          , "PBI"   , TYPE_INT    , modmem_memset         ),
    FUNC( "MEMSETW"         , "PWI"   , TYPE_INT    , modmem_memsetw        ),
    FUNC( "MEMSETI"         , "PII"   , TYPE_INT    , modmem_memseti        ),
    FUNC( "MEMCOPY"         , "PPI"   , TYPE_INT    , modmem_memcopy        ),
    FUNC( "MEMMOVE"         , "PPI"   , TYPE_INT    , modmem_memmove        ),
    FUNC( "MEMORY_FREE"     , ""      , TYPE_INT    , modmem_memory_free    ),
    FUNC( "MEMORY_TOTAL"    , ""      , TYPE_INT    , modmem_memory_total   ),
    FUNC( 0                 , 0       , 0           , 0                     )
};

/* ----------------------------------------------------------------- */

#endif
