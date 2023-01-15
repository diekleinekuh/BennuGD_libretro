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

#ifndef __MOD_FLIC_EXPORTS
#define __MOD_FLIC_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_flic, functions_exports )[] =
{
    /* Animaciones FLI */
    FUNC( "START_FLI"       , "SII"         , TYPE_INT , modflic_start        ),
    FUNC( "END_FLI"         , ""            , TYPE_INT , modflic_end          ),
    FUNC( "FRAME_FLI"       , ""            , TYPE_INT , modflic_frame        ),
    FUNC( "RESET_FLI"       , ""            , TYPE_INT , modflic_reset        ),

    FUNC( "FLI_START"       , "SII"         , TYPE_INT , modflic_startx1      ),
    FUNC( "FLI_START"       , "SIIIIII"     , TYPE_INT , modflic_startx2      ),
    FUNC( "FLI_END"         , "I"           , TYPE_INT , modflic_endx         ),
    FUNC( "FLI_FRAME"       , "I"           , TYPE_INT , modflic_framex       ),
    FUNC( "FLI_RESET"       , "I"           , TYPE_INT , modflic_resetx       ),

    FUNC( "FLI_PARAMS"      , "IIIIIII"     , TYPE_INT , modflic_params       ),
    FUNC( "FLI_MOVE"        , "III"         , TYPE_INT , modflic_move         ),
    FUNC( "FLI_ANGLE"       , "II"          , TYPE_INT , modflic_angle        ),
    FUNC( "FLI_SIZE"        , "II"          , TYPE_INT , modflic_size         ),
    FUNC( "FLI_FLAGS"       , "II"          , TYPE_INT , modflic_flags        ),
    FUNC( "FLI_Z"           , "II"          , TYPE_INT , modflic_z            ),
    FUNC( "FLI_GETINFO"     , "IPPPPPPPPP"  , TYPE_INT , modflic_getinfo      ),

    FUNC( NULL              , NULL          , 0        , NULL                 )
};

/* ----------------------------------------------------------------- */

#endif
