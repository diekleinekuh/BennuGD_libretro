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

#ifndef __MOD_PROC_EXPORTS
#define __MOD_PROC_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */

#include "mod_proc.h"

/* ----------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)         */

DLCONSTANT __bgdexport( mod_proc, constants_def )[] =
{
    { "S_KILL"              , TYPE_INT, S_KILL              },
    { "S_WAKEUP"            , TYPE_INT, S_WAKEUP            },
    { "S_SLEEP"             , TYPE_INT, S_SLEEP             },
    { "S_FREEZE"            , TYPE_INT, S_FREEZE            },

    { "S_FORCE"             , TYPE_INT, S_FORCE             },
    { "S_TREE"              , TYPE_INT, S_TREE              },

    { "S_KILL_TREE"         , TYPE_INT, S_KILL_TREE         },
    { "S_WAKEUP_TREE"       , TYPE_INT, S_WAKEUP_TREE       },
    { "S_SLEEP_TREE"        , TYPE_INT, S_SLEEP_TREE        },
    { "S_FREEZE_TREE"       , TYPE_INT, S_FREEZE_TREE       },

    { "S_KILL_FORCE"        , TYPE_INT, S_KILL_FORCE        },
    { "S_WAKEUP_FORCE"      , TYPE_INT, S_WAKEUP_FORCE      },
    { "S_SLEEP_FORCE"       , TYPE_INT, S_SLEEP_FORCE       },
    { "S_FREEZE_FORCE"      , TYPE_INT, S_FREEZE_FORCE      },
    { "S_KILL_TREE_FORCE"   , TYPE_INT, S_KILL_TREE_FORCE   },
    { "S_WAKEUP_TREE_FORCE" , TYPE_INT, S_WAKEUP_TREE_FORCE },
    { "S_SLEEP_TREE_FORCE"  , TYPE_INT, S_SLEEP_TREE_FORCE  },
    { "S_FREEZE_TREE_FORCE" , TYPE_INT, S_FREEZE_TREE_FORCE },

    { "S_DFL"               , TYPE_INT, S_DFL               },
    { "S_IGN"               , TYPE_INT, S_IGN               },

    { "ALL_PROCESS"         , TYPE_INT, ALL_PROCESS         },

    { NULL                  , 0       , 0                   }
} ;

/* ----------------------------------------------------------------- */

char * __bgdexport( mod_proc, locals_def ) =
    "STRUCT mod_proc_reserved\n"
    "   int type_scan;\n"
    "   int id_scan;\n"
    "   int context;\n"
    "   dword signal_action;\n"
    "END\n";

#endif

/* ----------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_proc, functions_exports )[] =
{
    /* Interacción entre procesos */
    FUNC( "GET_ID"          , "I"   , TYPE_INT , modproc_get_id          ),
    FUNC( "GET_STATUS"      , "I"   , TYPE_INT , modproc_get_status      ),
    FUNC( "SIGNAL"          , "II"  , TYPE_INT , modproc_signal          ),
    FUNC( "SIGNAL_ACTION"   , "II"  , TYPE_INT , modproc_signal_action   ),
    FUNC( "SIGNAL_ACTION"   , "III" , TYPE_INT , modproc_signal_action3  ),
    FUNC( "LET_ME_ALONE"    , ""    , TYPE_INT , modproc_let_me_alone    ),
    FUNC( "EXIT"            , "SI"  , TYPE_INT , modproc_exit            ),
    FUNC( "EXIT"            , "S"   , TYPE_INT , modproc_exit_1          ),
    FUNC( "EXIT"            , ""    , TYPE_INT , modproc_exit_0          ),
    FUNC( "EXISTS"          , "I"   , TYPE_INT , modproc_running         ),
    FUNC( 0                 , 0     , 0        , 0                       )
};

/* ----------------------------------------------------------------- */

#endif
