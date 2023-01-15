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

#ifndef __MOD_DIR_EXPORTS
#define __MOD_DIR_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( mod_dir, globals_def )=
    "STRUCT fileinfo\n"
    "    STRING path;\n"
    "    STRING name;\n"
    "    directory;\n"
    "    hidden;\n"
    "    readonly;\n"
    "    size;\n"
    "    STRING created;\n"
    "    STRING modified;\n"
    "    STRING accessed;\n"
    "    STRING statechg;\n"
    "END\n";

#endif

/* ---------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_dir, functions_exports)[] =
    {
        /* Archivos y directorios */
        FUNC( "CD"      , ""  , TYPE_STRING , moddir_cd     ),
        FUNC( "CD"      , "S" , TYPE_INT    , moddir_chdir  ),
        FUNC( "CHDIR"   , "S" , TYPE_INT    , moddir_chdir  ),
        FUNC( "MKDIR"   , "S" , TYPE_INT    , moddir_mkdir  ),
        FUNC( "RMDIR"   , "S" , TYPE_INT    , moddir_rmdir  ),
        FUNC( "GLOB"    , "S" , TYPE_STRING , moddir_glob   ),
        FUNC( "RM"      , "S" , TYPE_INT    , moddir_rm     ),

        FUNC( "DIROPEN" , "S" , TYPE_INT    , moddir_open   ),
        FUNC( "DIRCLOSE", "I" , TYPE_INT    , moddir_close  ),
        FUNC( "DIRREAD" , "I" , TYPE_STRING , moddir_read   ),

        FUNC( 0         , 0   , 0           , 0             )
    };

/* ---------------------------------------------------------------------- */

#endif
