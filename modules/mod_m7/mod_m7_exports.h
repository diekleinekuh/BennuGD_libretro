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

#ifndef __MOD_M7_EXPORTS
#define __MOD_M7_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */

#include "mod_m7.h"

/* --------------------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)         */

DLCONSTANT __bgdexport( mod_m7, constants_def )[] =
{
    { "C_M7", TYPE_INT, C_M7 },
    { NULL  , 0       , 0 }
} ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */
char * __bgdexport( mod_m7, globals_def ) =
    "STRUCT m7[9]\n"
    "camera;\n"
    "height = 32;\n"
    "distance = 64;\n"
    "horizon = 0;\n"
    "focus = 256;\n"
    "z = 256;\n"
    "color = 0;\n"
    "flags = 0;\n"
    "END\n" ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables locales (usada en tiempo de compilacion)  */
char * __bgdexport( mod_m7, locals_def ) =
    "ctype;\n"
    "cnumber;\n"
    "height = 32;\n"
    "STRUCT _m7_reserved\n"
    "  distance1;\n"
    "END;\n";

#endif

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_m7, functions_exports )[] =
{
    FUNC( "MODE7_START" , "IIIIIIII"    , TYPE_INT , modm7_start2 ),
    FUNC( "MODE7_START" , "IIIIII"      , TYPE_INT , modm7_start  ),
    FUNC( "MODE7_STOP"  , "I"           , TYPE_INT , modm7_stop   ),

    FUNC( "START_MODE7" , "IIIIIIII"    , TYPE_INT , modm7_start2 ),
    FUNC( "START_MODE7" , "IIIIII"      , TYPE_INT , modm7_start  ),
    FUNC( "STOP_MODE7"  , "I"           , TYPE_INT , modm7_stop   ),

    FUNC( NULL          , NULL          , 0        , NULL         )
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_m7, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "librender",
    NULL
};

/* --------------------------------------------------------------------------- */

#endif
