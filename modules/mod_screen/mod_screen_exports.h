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

#ifndef __MOD_SCREEN_EXPORTS
#define __MOD_SCREEN_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_screen, functions_exports )[] =
{
    /* Regiones */
    FUNC( "REGION_DEFINE"        , "IIIII"      , TYPE_INT      , modscreen_define_region   ),
    FUNC( "REGION_OUT"           , "II"         , TYPE_INT      , modscreen_out_region      ),

    /* Fondo de pantalla */
    FUNC( "PUT"                  , "IIII"       , TYPE_INT      , modscreen_put             ),
    FUNC( "XPUT"                 , "IIIIIIII"   , TYPE_INT      , modscreen_xput            ),
    FUNC( "SCREEN_PUT"           , "II"         , TYPE_INT      , modscreen_put_screen      ),
    FUNC( "SCREEN_CLEAR"         , ""           , TYPE_INT      , modscreen_clear_screen    ),

    /* Video */
    FUNC( "SCREEN_GET"           , ""           , TYPE_INT      , modscreen_get_screen      ),

    /* ------------ Compatibility ------------ */

    /* Regiones */
    FUNC( "DEFINE_REGION"        , "IIIII"      , TYPE_INT      , modscreen_define_region   ),
    FUNC( "OUT_REGION"           , "II"         , TYPE_INT      , modscreen_out_region      ),

    /* Fondo de pantalla */
    FUNC( "PUT_SCREEN"           , "II"         , TYPE_INT      , modscreen_put_screen      ),
    FUNC( "CLEAR_SCREEN"         , ""           , TYPE_INT      , modscreen_clear_screen    ),

    /* Video */
    FUNC( "GET_SCREEN"           , ""           , TYPE_INT      , modscreen_get_screen      ),

    FUNC( 0                     , 0             , 0             , 0                         )
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_screen, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libblit",
    "librender",
    NULL
};

/* --------------------------------------------------------------------------- */

#endif
