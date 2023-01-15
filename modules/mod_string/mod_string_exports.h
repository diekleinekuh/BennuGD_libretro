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

#ifndef __MOD_STRING_EXPORTS
#define __MOD_STRING_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_string, functions_exports )[] =
{
    FUNC( "STRLEN"     , "S"   , TYPE_INT   , modstring_strlen     ),
    FUNC( "LEN"        , "S"   , TYPE_INT   , modstring_strlen     ),
    FUNC( "UCASE"      , "S"   , TYPE_STRING, modstring_strupper   ),
    FUNC( "LCASE"      , "S"   , TYPE_STRING, modstring_strlower   ),
    FUNC( "STRCASECMP" , "SS"  , TYPE_INT   , modstring_strcasecmp ),
    FUNC( "SUBSTR"     , "SII" , TYPE_STRING, modstring_substr     ),
    FUNC( "SUBSTR"     , "SI"  , TYPE_STRING, modstring_substr2    ),
    FUNC( "FIND"       , "SS"  , TYPE_INT   , modstring_strfind    ),
    FUNC( "FIND"       , "SSI" , TYPE_INT   , modstring_strfindSSI ),
    FUNC( "LPAD"       , "SI"  , TYPE_STRING, modstring_lpad       ),
    FUNC( "RPAD"       , "SI"  , TYPE_STRING, modstring_rpad       ),
    FUNC( "ITOA"       , "I"   , TYPE_STRING, modstring_itos       ),
    FUNC( "FTOA"       , "F"   , TYPE_STRING, modstring_ftos       ),
    FUNC( "ATOI"       , "S"   , TYPE_INT   , modstring_stoi       ),
    FUNC( "ATOF"       , "S"   , TYPE_FLOAT , modstring_stof       ),
    FUNC( "ASC"        , "S"   , TYPE_BYTE  , modstring_asc        ),
    FUNC( "CHR"        , "I"   , TYPE_STRING, modstring_chr        ),
    FUNC( "TRIM"       , "S"   , TYPE_STRING, modstring_trim       ),
    FUNC( "STRREV"     , "S"   , TYPE_STRING, modstring_strrev     ),
    FUNC( "FORMAT"     , "I"   , TYPE_STRING, modstring_formatI    ),
    FUNC( "FORMAT"     , "F"   , TYPE_STRING, modstring_formatF    ),
    FUNC( "FORMAT"     , "FI"  , TYPE_STRING, modstring_formatFI   ),

    FUNC( "STRING_BUFFER"   , "S"   , TYPE_POINTER  , modstring_get_buffer      ),
    FUNC( "STRING_ALLOC"    , ""    , TYPE_POINTER  , modstring_string_alloc    ),
    FUNC( "STRING_ALLOC"    , "S"   , TYPE_POINTER  , modstring_string_alloc2   ),
    FUNC( "STRING_RELEASE"  , "P"   , TYPE_UNDEFINED, modstring_string_release  ),

    FUNC( "STRING_DUMP"     , ""    , TYPE_UNDEFINED, modstring_string_dump     ),

    FUNC( 0            , 0     , 0          , 0                    )
};

/* ----------------------------------------------------------------- */

#endif
