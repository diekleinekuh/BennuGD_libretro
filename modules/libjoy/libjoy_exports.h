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

#ifndef __LIBJOY_EXPORTS
#define __LIBJOY_EXPORTS

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

#include <SDL.h>

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( libjoy, constants_def )[] =
{
    { "JOY_HAT_CENTERED"    , TYPE_DWORD, SDL_HAT_CENTERED  },
    { "JOY_HAT_UP"          , TYPE_DWORD, SDL_HAT_UP        },
    { "JOY_HAT_RIGHT"       , TYPE_DWORD, SDL_HAT_RIGHT     },
    { "JOY_HAT_DOWN"        , TYPE_DWORD, SDL_HAT_DOWN      },
    { "JOY_HAT_LEFT"        , TYPE_DWORD, SDL_HAT_LEFT      },
    { "JOY_HAT_RIGHTUP"     , TYPE_DWORD, SDL_HAT_RIGHTUP   },
    { "JOY_HAT_RIGHTDOWN"   , TYPE_DWORD, SDL_HAT_RIGHTDOWN },
    { "JOY_HAT_LEFTUP"      , TYPE_DWORD, SDL_HAT_LEFTUP    },
    { "JOY_HAT_LEFTDOWN"    , TYPE_DWORD, SDL_HAT_LEFTDOWN  },
    { NULL                  , 0         , 0                 }
} ;

#endif

/* ----------------------------------------------------------------- */

char * __bgdexport( libjoy, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};

/* ----------------------------------------------------------------- */

#endif
