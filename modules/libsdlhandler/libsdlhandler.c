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

#include "bgddl.h"

#include <SDL.h>

/* ----------------------------------------------------------------- */
/* Public functions                                                  */

static void  dump_new_events()
{
    SDL_Event event;
    /* Remove all pendings events */

    /* We can't return -1, just return 0 (no event) on error */
    while ( SDL_PeepEvents( &event, 1, SDL_GETEVENT, SDL_ALLEVENTS ) > 0 );

    /* Get new events */
    SDL_PumpEvents();
}

/* ----------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                     */

void __bgdexport( libsdlhandler, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_EVENTTHREAD ) ) SDL_InitSubSystem( SDL_INIT_EVENTTHREAD );
}

/* ----------------------------------------------------------------- */

void __bgdexport( libsdlhandler, module_finalize )()
{
    if ( SDL_WasInit( SDL_INIT_EVENTTHREAD ) ) SDL_QuitSubSystem( SDL_INIT_EVENTTHREAD );
}

/* ----------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libsdlhandler, handler_hooks )[] =
{
    { 5000, dump_new_events                   },
    {    0, NULL                              }
} ;

/* ----------------------------------------------------------------- */
