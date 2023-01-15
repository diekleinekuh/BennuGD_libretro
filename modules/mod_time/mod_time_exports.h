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

#ifndef __MOD_TIME_EXPORTS
#define __MOD_TIME_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */
/* Declaracion de funciones                                                    */

DLSYSFUNCS __bgdexport( mod_time, functions_exports )[] =
{
    /* Fecha/Hora */
    FUNC( "GET_TIMER"   , ""    , TYPE_INT      , modtime_get_timer     ),
    FUNC( "TIME"        , ""    , TYPE_INT      , modtime_time          ),
    FUNC( "FTIME"       , "SI"  , TYPE_STRING   , modtime_ftime         ),
    FUNC( 0             , 0     , 0             , 0                     )
};

/* --------------------------------------------------------------------------- */

#endif
