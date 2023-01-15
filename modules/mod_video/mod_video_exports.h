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

#ifndef __MOD_VIDEO_EXPORTS
#define __MOD_VIDEO_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* ----------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_video, functions_exports )[] =
{

    /* Video */
    FUNC( "SET_MODE"        , "I"     , TYPE_INT        , modvideo_set_mode         ),
    FUNC( "SET_MODE"        , "II"    , TYPE_INT        , modvideo_set_mode_2       ),
    FUNC( "SET_MODE"        , "III"   , TYPE_INT        , modvideo_set_mode_3       ),
    FUNC( "SET_MODE"        , "IIII"  , TYPE_INT        , modvideo_set_mode_4       ),
    FUNC( "SET_FPS"         , "II"    , TYPE_INT        , modvideo_set_fps          ),

    FUNC( "GET_MODES"       , "II"    , TYPE_POINTER    , modvideo_list_modes       ),
    FUNC( "MODE_IS_OK"      , "IIII"  , TYPE_INT        , modvideo_mode_is_ok       ),

    FUNC( 0                 , 0       , 0               , 0                         )
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_video, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "librender",
    NULL
};

/* --------------------------------------------------------------------------- */

#endif
