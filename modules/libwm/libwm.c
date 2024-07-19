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

/* --------------------------------------------------------------------------- */

#include <SDL.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

#include "libwm_exports.h"

/* --------------------------------------------------------------------------- */

enum {
    EXIT_STATUS = 0,
    WINDOW_STATUS,
    FOCUS_STATUS,
    MOUSE_STATUS
};

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP  __bgdexport( libwm, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "exit_status"     , NULL, -1, -1 },
    { "window_status"   , NULL, -1, -1 },
    { "focus_status"    , NULL, -1, -1 },
    { "mouse_status"    , NULL, -1, -1 },
    { NULL              , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/* Gestión de eventos de ventana                                               */
/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : wm_events
 *
 *  Process all pending wm SDL events, updating all global variables
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void wm_events()
{
    SDL_Event e ;

    /* Procesa los eventos de ventana pendientes */

    GLODWORD( libwm, EXIT_STATUS ) = 0 ;

    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_QUITMASK | SDL_ACTIVEEVENTMASK )>0 )
    {
        switch ( e.type )
        {
                /* WINDOW MANAGER EVENTS */
            case SDL_QUIT:
                /* UPDATE  exit status... initilized each frame */
                GLODWORD( libwm, EXIT_STATUS ) = 1 ;
//                bgdrtm_exit( -1 );
                break ;

            case SDL_ACTIVEEVENT:
                if ( e.active.state & SDL_APPACTIVE ) GLODWORD( libwm, WINDOW_STATUS ) = ( e.active.gain ) ? 1 : 0 ;
                if ( e.active.state & SDL_APPINPUTFOCUS ) GLODWORD( libwm, FOCUS_STATUS ) = ( e.active.gain ) ? 1 : 0 ;
                if ( e.active.state & SDL_APPMOUSEFOCUS ) GLODWORD( libwm, MOUSE_STATUS ) = ( e.active.gain ) ? 1 : 0 ;
                break ;
        }
    }
}

/* --------------------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libwm, handler_hooks )[] =
{
    { 4700, wm_events   },
    {    0, NULL        }
} ;

/* --------------------------------------------------------------------------- */
