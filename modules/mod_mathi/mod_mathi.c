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

#define _GNU_SOURCE
#define _USE_MATH_DEFINES 1
#include <math.h>

#include <stdio.h>

#include "bgddl.h"

/* --------------------------------------------------------------------------- */
/* Funciones matemáticas */
/* --------------------------------------------------------------------------- */

static int math_abs( INSTANCE * my, int * params )
{
    float num = *( float * ) &params[0] ;
    float res = ( num < 0 ) ? -num : num ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_pow( INSTANCE * my, int * params )
{
    float res = ( float )pow( *( float * ) & params[0], *( float * ) & params[1] ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_sqrt( INSTANCE * my, int * params )
{
    float res = ( float )sqrt( *( float * ) & params[0] ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_cos( INSTANCE * my, int * params )
{
    return cos(( double )( params[0] * M_PI / 180000.0 )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_sin( INSTANCE * my, int * params )
{
    return sin(( double )( params[0] * M_PI / 180000.0 )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_tan( INSTANCE * my, int * params )
{
    return tan(( double )( params[0] * M_PI / 180000.0 )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_acos( INSTANCE * my, int * params )
{
    return acos(( double )( params[0] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_asin( INSTANCE * my, int * params )
{
    return asin(( double )( params[0] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_atan( INSTANCE * my, int * params )
{
    return atan(( double )( params[0] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_atan2( INSTANCE * my, int * params )
{
    return atan2(( double )( params[0] * 180000.0 / M_PI ), ( double )( params[1] * 180000.0 / M_PI )) * 1000 ;
}

/* --------------------------------------------------------------------------- */

static int math_isinf( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return isinf( param );
}

/* --------------------------------------------------------------------------- */

static int math_isnan( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return isnan( param );
}

/* --------------------------------------------------------------------------- */

static int math_finite( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return isfinite ( param );
}

/* --------------------------------------------------------------------------- */

static int math_fget_angle( INSTANCE * my, int * params )
{
    double dx = params[2] - params[0] ;
    double dy = params[3] - params[1] ;
    int angle ;

    if ( dx == 0 ) return ( dy > 0 ) ? 270000L : 90000L ;

    angle = ( int )( atan( dy / dx ) * 180000.0 / M_PI ) ;

    return ( dx > 0 ) ? -angle : -angle + 180000L ;
}

/* --------------------------------------------------------------------------- */

static int math_fget_dist( INSTANCE * my, int * params )
{
    double dx = params[2] - params[0] ;
    double dy = params[3] - params[1] ;

    return ( int )sqrt( dx*dx + dy*dy ) ;
}

/* --------------------------------------------------------------------------- */

static int math_near_angle( INSTANCE * my, int * params )
{
    int angle = params[0] ;
    int dest  = params[1] ;
    int incr  = params[2] ;

    if ( angle < dest && dest - angle > 180000L )
        angle += 360000L ;

    if ( angle > dest && angle - dest > 180000L )
        angle -= 360000L ;

    if ( angle < dest )
    {
        angle += incr ;
        if ( angle > dest ) angle = dest ;
    }
    else
    {
        angle -= incr ;
        if ( angle < dest ) angle = dest ;
    }

    if ( angle < 0 ) return angle + 360000L ;
    if ( angle >= 360000L ) return angle - 360000L ;
    return angle ;
}

/* --------------------------------------------------------------------------- */

static int math_get_distx( INSTANCE * my, int * params )
{
    double angle = params[0] * M_PI / 180000.0 ;
    return ( int )( params[1] * cos( angle ) ) ;
}

/* --------------------------------------------------------------------------- */

static int math_get_disty( INSTANCE * my, int * params )
{
    double angle = params[0] * M_PI / 180000.0 ;
    return ( int )( params[1] * -sin( angle ) ) ;
}

/* ----------------------------------------------------------------- */
/* exports                                                           */
/* ----------------------------------------------------------------- */

#include "mod_mathi_exports.h"

/* ----------------------------------------------------------------- */
