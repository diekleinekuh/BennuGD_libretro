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

#include "b_crypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

static int modcrypt_new( INSTANCE * my, int * params )
{
    return ( ( int ) int_from_ptr( crypt_create( params[0], ( char * ) ptr_from_int(params[1]) )) );
}

/* --------------------------------------------------------------------------- */

static int modcrypt_del( INSTANCE * my, int * params )
{
    crypt_destroy( ( crypt_handle * ) ptr_from_int(params[0]) );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int __crypt( crypt_handle * ch, char * in, char * out, int blocks, int enc )
{
    if ( !ch || !in || !out || blocks <= 0 ) return -1;

    while ( blocks-- )
    {
        if ( crypt_data( ch, in, out, 8, enc ) <= 0 ) return -1;
        in += 8;
        out += 8;
    }

    return 0;
}

/* --------------------------------------------------------------------------- */

static int modcrypt_encrypt( INSTANCE * my, int * params )
{
    return ( __crypt( ( crypt_handle * ) ptr_from_int(params[0]), ( char * ) ptr_from_int(params[1]), ( char * ) ptr_from_int(params[2]), params[3], 1 ) );
}

/* --------------------------------------------------------------------------- */

static int modcrypt_decrypt( INSTANCE * my, int * params )
{
    return ( __crypt( ( crypt_handle * ) ptr_from_int(params[0]), ( char * ) ptr_from_int(params[1]), ( char * ) ptr_from_int(params[2]), params[3], 0 ) );
}

/* --------------------------------------------------------------------------- */

static int modcrypt_encrypt2( INSTANCE * my, int * params )
{
    int r;
    crypt_handle * ch = crypt_create( params[0], ( char * ) ptr_from_int(params[1]) );
    r = __crypt( ch, ( char * ) ptr_from_int(params[2]), ( char * ) ptr_from_int(params[3]), params[4], 1 );
    crypt_destroy( ch );
    return r;
}

/* --------------------------------------------------------------------------- */

static int modcrypt_decrypt2( INSTANCE * my, int * params )
{
    int r;
    crypt_handle * ch = crypt_create( params[0], ( char * ) ptr_from_int(params[1]) );
    r = __crypt( ch, ( char * ) ptr_from_int(params[2]), ( char * ) ptr_from_int(params[3]), params[4], 0 );
    crypt_destroy( ch );
    return r;
}

/* --------------------------------------------------------------------------- */
/* exports                                                                     */
/* --------------------------------------------------------------------------- */

#include "mod_crypt_exports.h"

/* --------------------------------------------------------------------------- */
