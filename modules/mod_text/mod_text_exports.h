/*
 *  Copyright © 2006-2019 SplinterGU( Fenix/Bennugd )
 *  Copyright © 2002-2006 Fenix Team( Fenix )
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe( Fenix )
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

#ifndef __MOD_TEXT_EXPORTS
#define __MOD_TEXT_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* ----------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */

#include "libtext.h"

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_text, constants_def )[] =
{
    { "ALL_TEXT"            , TYPE_INT  , 0                     },

    /* Alineacion de textos */
    { "ALIGN_TOP_LEFT"      , TYPE_INT  , ALIGN_TOP_LEFT        },
    { "ALIGN_TOP"           , TYPE_INT  , ALIGN_TOP             },
    { "ALIGN_TOP_RIGHT"     , TYPE_INT  , ALIGN_TOP_RIGHT       },
    { "ALIGN_CENTER_LEFT"   , TYPE_INT  , ALIGN_CENTER_LEFT     },
    { "ALIGN_CENTER"        , TYPE_INT  , ALIGN_CENTER          },
    { "ALIGN_CENTER_RIGHT"  , TYPE_INT  , ALIGN_CENTER_RIGHT    },
    { "ALIGN_BOTTOM_LEFT"   , TYPE_INT  , ALIGN_BOTTOM_LEFT     },
    { "ALIGN_BOTTOM"        , TYPE_INT  , ALIGN_BOTTOM          },
    { "ALIGN_BOTTOM_RIGHT"  , TYPE_INT  , ALIGN_BOTTOM_RIGHT    },

    { NULL                  , 0         , 0                     }
} ;

#endif

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_text, functions_exports )[] =
{
    /* Textos */
    FUNC( "WRITE"               , "IIIIS"   , TYPE_INT  , modtext_write             ),
    FUNC( "WRITE"               , "IIIIIS"  , TYPE_INT  , modtext_write2            ),
    FUNC( "WRITE_INT"           , "IIIIP"   , TYPE_INT  , modtext_write_int         ),
    FUNC( "WRITE_INT"           , "IIIIIP"  , TYPE_INT  , modtext_write_int2        ),
    FUNC( "MOVE_TEXT"           , "III"     , TYPE_INT  , modtext_move_text         ),
    FUNC( "MOVE_TEXT"           , "IIII"    , TYPE_INT  , modtext_move_text2        ),
    FUNC( "DELETE_TEXT"         , "I"       , TYPE_INT  , modtext_delete_text       ),
    FUNC( "WRITE_IN_MAP"        , "ISI"     , TYPE_INT  , modtext_write_in_map      ),
    FUNC( "TEXT_WIDTH"          , "IS"      , TYPE_INT  , modtext_text_width        ),
    FUNC( "TEXT_HEIGHT"         , "IS"      , TYPE_INT  , modtext_text_height       ),
    FUNC( "GET_TEXT_COLOR"      , ""        , TYPE_INT  , modtext_get_text_color    ),
    FUNC( "GET_TEXT_COLOR"      , "I"       , TYPE_INT  , modtext_get_text_color2   ),
    FUNC( "SET_TEXT_COLOR"      , "I"       , TYPE_INT  , modtext_set_text_color    ),
    FUNC( "SET_TEXT_COLOR"      , "II"      , TYPE_INT  , modtext_set_text_color2   ),

    /* Extended Write */
    FUNC( "WRITE_VAR"           , "IIIIV++" , TYPE_INT  , modtext_write_var         ),
    FUNC( "WRITE_VAR"           , "IIIIIV++", TYPE_INT  , modtext_write_var2        ),
    FUNC( "WRITE_FLOAT"         , "IIIIP"   , TYPE_INT  , modtext_write_float       ),
    FUNC( "WRITE_FLOAT"         , "IIIIIP"  , TYPE_INT  , modtext_write_float2      ),
    FUNC( "WRITE_STRING"        , "IIIIP"   , TYPE_INT  , modtext_write_string      ),
    FUNC( "WRITE_STRING"        , "IIIIIP"  , TYPE_INT  , modtext_write_string2     ),

    FUNC( 0                     , 0         , 0         , 0                         )
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_text, modules_dependency )[] =
{
    "libgrbase",
    "libblit",
    "libtext",
    "libfont",
    NULL
};

/* --------------------------------------------------------------------------- */

#endif
