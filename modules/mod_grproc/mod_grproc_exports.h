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

#ifndef __MOD_GRPROC_EXPORTS
#define __MOD_GRPROC_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */
/* Locals */

char * __bgdexport( mod_grproc, locals_def ) =
    "STRUCT _mod_grproc_reserved\n"
    "int type_scan;\n"
    "int id_scan;\n"
    "int context;\n"
    "END\n";

#endif

/* ---------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_grproc, functions_exports )[] =
{
    FUNC( "ADVANCE"             , "I"   , TYPE_INT  , grproc_advance            ),
    FUNC( "XADVANCE"            , "II"  , TYPE_INT  , grproc_xadvance           ),

    FUNC( "GET_ANGLE"           , "I"   , TYPE_INT  , grproc_get_angle          ),
    FUNC( "GET_DIST"            , "I"   , TYPE_INT  , grproc_get_dist           ),
    FUNC( "COLLISION"           , "I"   , TYPE_INT  , grproc_collision          ),
    FUNC( "COLLISION_BOX"       , "I"   , TYPE_INT  , grproc_collision_box      ),
    FUNC( "COLLISION_CIRCLE"    , "I"   , TYPE_INT  , grproc_collision_circle   ),

    FUNC( "GET_REAL_POINT"      , "IPP" , TYPE_INT  , grproc_get_real_point     ),

    FUNC( NULL                  , 0     , 0         , 0                         )
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_grproc, modules_dependency )[] =
{
    "libmouse",
    "libgrbase",
    "libvideo",
    "librender",
    "libblit",
    NULL
};

/* --------------------------------------------------------------------------- */

#endif
