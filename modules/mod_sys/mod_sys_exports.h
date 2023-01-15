/*
 *  Copyright © 2006-2019 SplinterGU (Fenix/Bennugd)
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


#ifndef __MOD_SYS_EXPORTS
#define __MOD_SYS_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

#include "mod_sys.h"

/* ---------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_sys, constants_def )[] =
{
    { "_P_WAIT"     , TYPE_DWORD,  _P_WAIT   },
    { "_P_NOWAIT"   , TYPE_DWORD,  _P_NOWAIT },
    { NULL          , 0         ,  0         }
} ;

#endif

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS __bgdexport( mod_sys, functions_exports )[] =
{
    FUNC( "GETENV"  , "S"    , TYPE_STRING, modsys_getenv ),
    FUNC( "EXEC"    , "ISIP" , TYPE_INT   , modsys_exec   ),
    FUNC( 0         , 0      , 0          , 0             )
};

/* ----------------------------------------------------------------- */

#endif
