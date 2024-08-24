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

#include "xstrings.h"

#include <SDL.h>

#include "bgload.h"

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* - bgload functions -------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/**
 * PREP
 * Helper function preparing params
 **/

static bgdata *prep( int *params )
{
    bgdata *t = ( bgdata* )bgd_malloc( sizeof( bgdata ) );
    t->file = bgd_strdup(( char * )string_get( params[0] ));
    t->id = ( int* )params[1];
    *( t->id ) = -2 ; // WAIT STATUS
    string_discard( params[0] );
    return t;
}

/* --------------------------------------------------------------------------- */
/**
 * bgDoLoad
 * Helper function executed in the new thread
 **/

int bgDoLoad( void *d )
{
    bgdata *t = ( bgdata* )d;
    *( t->id ) = ( *t->fn )( t->file );
    bgd_free( t->file );
    bgd_free( t );
    return 0;
}

/* --------------------------------------------------------------------------- */

int bgload( int ( *fn )(const char*), int * params )
{
    bgdata *t = prep( params );
    t->fn = fn;
    SDL_CreateThread( bgDoLoad, ( void * )t );
    return 0 ;
}

/* --------------------------------------------------------------------------- */
