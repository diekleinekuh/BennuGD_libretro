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

#include <stdlib.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

#include "xstrings.h"
#include "mod_map.h"

#include "librender.h"

#include "bgload.h"

/* --------------------------------------------------------------------------- */

static int modmap_graphic_set( INSTANCE * my, int * params )
{
    GRAPH * map ;

    map = bitmap_get( params[0], params[1] ) ;
    if ( !map ) return 0 ;

    switch ( params[2] )
    {
        case 2:     /* g_center_x */
            bitmap_set_cpoint( map, 0, params[3], ( map->ncpoints ) ? ( map->cpoints[0].y ) : ( map->height / 2 ) ) ;
            return 1 ;

        case 3:     /* g_center_y */
            bitmap_set_cpoint( map, 0, ( map->ncpoints ) ? ( map->cpoints[0].x ) : ( map->width / 2 ), params[3] ) ;
            return 1 ;
    }
    return 1 ;
}

static int modmap_graphic_info( INSTANCE * my, int * params )
{
    GRAPH * map ;

    map = bitmap_get( params[0], params[1] ) ;
    if ( !map ) return 0 ;

    switch ( params[2] )
    {
        case G_WIDTH:           /* g_wide */
            return map->width ;

        case G_HEIGHT:          /* g_height */
            return map->height ;

        case G_PITCH:           /* g_pitch */
            return map->pitch ;

        case G_DEPTH:           /* g_depth */
            return map->format->depth ;

        case G_CENTER_X:        /* g_center_x */
            if ( map->ncpoints > 0 )
                if ( map->cpoints[0].x != CPOINT_UNDEFINED )
                    return map->cpoints[0].x ;
            return map->width / 2 ;

        case G_CENTER_Y:        /* g_center_y */
            if ( map->ncpoints > 0 )
                if ( map->cpoints[0].y != CPOINT_UNDEFINED )
                    return map->cpoints[0].y ;
            return map->height / 2 ;
    }

    return 1 ;
}

static int modmap_set_point( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    if ( !bmp || params[2] < 0 || params[2] > 999 ) return -1 ;
    bitmap_set_cpoint( bmp, ( uint32_t )params[2], params[3], params[4] );
    return 1 ;
}

static int modmap_set_center( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    if ( !bmp ) return -1 ;
    bitmap_set_cpoint( bmp, 0, params[2], params[3] );
    return 1 ;
}

static int modmap_get_point( INSTANCE * my, int * params )
{
    GRAPH * bmp ;

    bmp = bitmap_get( params[0], params[1] ) ;
    if ( !bmp ) return 0 ;

    /* Use the center as control point if it is not there */
    if ( params[2] == 0 && ( bmp->ncpoints == 0 || bmp->cpoints[0].x == CPOINT_UNDEFINED ) )
    {
        *( int * )ptr_from_int(params[3]) = bmp->width / 2;
        *( int * )ptr_from_int(params[4]) = bmp->height / 2;
        return 1 ;
    }

    if (( uint32_t )params[2] >= bmp->ncpoints || params[2] < 0 )
        return 0 ;
    if ( bmp->cpoints[params[2]].x == CPOINT_UNDEFINED && bmp->cpoints[params[2]].y == CPOINT_UNDEFINED )
        return 0;

    *( int * )ptr_from_int(params[3]) = bmp->cpoints[params[2]].x ;
    *( int * )ptr_from_int(params[4]) = bmp->cpoints[params[2]].y ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_load_png( INSTANCE * my, int * params )
{
    int r = gr_load_png( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modmap_load_pcx( INSTANCE * my, int * params )
{
    int r = ( int ) gr_load_pcx( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modmap_save_png( INSTANCE * my, int * params )
{
    int r = ( int ) gr_save_png( bitmap_get( params[0], params[1] ), string_get( params[2] ) ) ;
    string_discard( params[2] ) ;
    return r ;
}

/* ---------------------------------------------------------------------- */

static int modmap_map_buffer( INSTANCE * my, int * params )
{
    GRAPH * map ;

    if ( params[0] || params[1] )
        map = bitmap_get( params[0], params[1] ) ;
    else
        map = background ;

    return map ? ( int )int_from_ptr(map->data) : 0 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_clear( INSTANCE * my, int * params )
{
    GRAPH *map = bitmap_get( params[0], params[1] ) ;
    if ( map ) gr_clear_as( map, params[2] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_new_map( INSTANCE * my, int * params )
{
    GRAPH * map ;
    map = bitmap_new_syslib( params[0], params[1], params[2] ) ;
    if ( map ) gr_clear( map );
    return map ? map->code : 0 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_new_map_extend( INSTANCE * my, int * params )
{
    GRAPH * map ;
    map = bitmap_new_syslib( params[0], params[1], params[2] ) ;
    if ( map && ( params[3] & B_CLEAR )) gr_clear( map );
    return map ? map->code : 0 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_clone( INSTANCE * my, int * params )
{
    GRAPH * origin, * map = NULL ;
    origin = bitmap_get( params[0], params[1] ) ;
    if ( origin ) map = bitmap_clone( origin ) ;
    if ( !map ) return 0;
    map->code = bitmap_next_code();
    grlib_add_map( 0, map );
    return map->code ;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_put( INSTANCE * my, int * params )
{
    GRAPH * dest = bitmap_get( params[0], params[1] ) ;
    GRAPH * orig = bitmap_get( params[0], params[2] ) ;

    if ( !dest || !orig ) return 0 ;

    gr_blit( dest, NULL, params[3], params[4], 0, orig ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */
/** MAP_XPUT (FILE, GRAPH_DEST, GRAPH_SRC, X, Y, ANGLE, SIZE, FLAGS)
 *  Draws a map into another one, with most blitter options including flags and alpha
 */

static int modmap_map_xput( INSTANCE * my, int * params )
{
    GRAPH * dest = bitmap_get( params[0], params[1] ) ;
    GRAPH * orig = bitmap_get( params[0], params[2] ) ;

    if ( !dest || !orig ) return 0;

    if ( params[5] == 0 && params[6] == 100 )
        gr_blit( dest, 0, params[3], params[4], params[7], orig ) ;
    else
        gr_rotated_blit( dest, 0, params[3], params[4], params[7], params[5], params[6], params[6], orig ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */
/** MAP_XPUTNP (FILE_DST, GRAPH_DST, FILE_SRC, GRAPH_SRC, X, Y, ANGLE, SCALE_X, SCALE_Y, FLAGS)
 *  Enhanced MAP_XPUT with all parametes and different FPG file and non-proportional scale
 */

static int modmap_map_xputnp( INSTANCE * my, int * params )
{
    GRAPH * dest = bitmap_get( params[0], params[1] ) ;
    GRAPH * orig = bitmap_get( params[2], params[3] ) ;

    if ( params[6] == 0 && params[7] == 100 && params[8] == 100 )
        gr_blit( dest, 0, params[4], params[5], params[9], orig ) ;
    else
        gr_rotated_blit( dest, 0, params[4], params[5], params[9], params[6], params[7], params[8], orig ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_name( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[0], params[1] );
    int result;

    if ( !map ) return 0;
    result = string_new( map->name );
    string_use( result );
    return result;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_set_name( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[0], params[1] );
    const char * ptr = string_get( params[2] ) ;
    if ( map )
    {
        strncpy( map->name, ptr, sizeof( map->name ) );
        map->name[sizeof( map->name )-1] = 0;
    }
    string_discard( params[2] );
    return 0;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_exists( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[0], params[1] );
    return map == NULL ? 0 : 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_map_block_copy( INSTANCE * my, int * params )
{
    GRAPH * dest, * orig ;
    REGION clip ;
    int centerx, centery, flag ;
    uint32_t x, y, w, h, dx, dy ;

    dest = bitmap_get( params[0], params[1] ) ;
    if ( !dest ) return 0;

    orig = bitmap_get( params[0], params[4] ) ;
    if ( !orig ) return 0;

    x  = params[5] ;
    y  = params[6] ;
    w  = params[7] ;
    h  = params[8] ;
    dx = params[2] ;
    dy = params[3] ;
    flag = params[9] ;

    if ( orig->ncpoints > 0 && orig->cpoints[0].x != CPOINT_UNDEFINED )
    {
        centerx = orig->cpoints[0].x ;
        centery = orig->cpoints[0].y ;
    }
    else
    {
        centery = orig->height / 2 ;
        centerx = orig->width / 2 ;
    }

    if ( flag & B_HMIRROR ) centerx = orig->width - 1 - centerx;
    if ( flag & B_VMIRROR ) centery = orig->height - 1 - centery;

    if ( x < 0 )
    {
        dx += x ;
        w  += x ;
        x   = 0 ;
    }
    if ( y < 0 )
    {
        dy += y ;
        h  += y ;
        y   = 0 ;
    }
    if ( dx < 0 )
    {
        x += dx ;
        w += dx ;
        dx = 0 ;
    }
    if ( dy < 0 )
    {
        y += dy ;
        h += dy ;
        dy = 0 ;
    }

    if ( x + w > orig->width ) w = orig->width - x ;

    if ( y + h > orig->height ) h = orig->height - y ;

    if ( dx + w > dest->width ) w = dest->width - dx ;

    if ( dy + h > dest->height ) h = dest->height - dy ;

    if ( w <= 0 || h <= 0 ) return 0;

    clip.x = dx ;
    clip.y = dy ;
    clip.x2 = dx + w - 1 ;
    clip.y2 = dy + h - 1 ;
    gr_blit( dest, &clip, dx - x + centerx, dy - y + centery, flag, orig ) ;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_load_map( INSTANCE * my, int * params )
{
    int r = gr_load_map( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modmap_unload_map( INSTANCE * my, int * params )
{
    return grlib_unload_map( params[0], params[1] ) ;
}

/* ---------------------------------------------------------------------- */

static int modmap_save_map( INSTANCE * my, int * params )
{
    int r = ( int ) gr_save_map( bitmap_get( params[0], params[1] ), ( char * )string_get( params[2] ) ) ;
    string_discard( params[2] ) ;
    return r ;
}

/* ---------------------------------------------------------------------- */

static int modmap_load_pal( INSTANCE * my, int * params )
{
    int r = gr_load_pal( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* ---------------------------------------------------------------------- */

static int modmap_save_pal( INSTANCE * my, int * params )
{
    int r = gr_save_pal( string_get( params[0] ), ( PALETTE * )ptr_from_int(params[1]) );
    string_discard( params[0] );
    return r;
}

/* --------------------------------------------------------------------------- */

static int modmap_save_system_pal( INSTANCE * my, int * params )
{
    int r = gr_save_system_pal( string_get( params[0] ) );
    string_discard( params[0] );
    return r;
}

/* --------------------------------------------------------------------------- */

static int modmap_convert_palette( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[0], params[1] ) ;
    int * newpal = ( int * ) ptr_from_int(params[2]);
    uint32_t x, y ;
    uint8_t * orig, * ptr ;

    if ( !map ) return -1 ;
    if ( map->format->depth != 8 ) return 0;

    orig = ( uint8_t * )map->data ;
    for ( y = 0 ; y < map->height; y++ )
    {
        ptr = orig + map->pitch * y ;
        for ( x = 0 ; x < map->width ; x++, ptr++ ) *ptr = newpal[*ptr] ;
    }

    map->modified = 1;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_set_colors( INSTANCE * my, int * params )
{
    gr_set_colors( params[0], params[1], ( uint8_t * )ptr_from_int(params[2]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_get_colors( INSTANCE * my, int * params )
{
    gr_get_colors( params[0], params[1], ( uint8_t * )ptr_from_int(params[2]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_roll_palette( INSTANCE * my, int * params )
{
    gr_roll_palette( params[0], params[1], params[2] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_find_color( INSTANCE * my, int * params )
{
    return gr_find_nearest_color( params[0], params[1], params[2] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_find_color2( INSTANCE * my, int * params )
{
    return find_nearest_color( ( PALETTE * ) ptr_from_int(params[0]), 0, 255, params[1], params[2], params[3] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_find_color3( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    if ( !bmp || bmp->format->depth != 8 ) return 0 ;
    return find_nearest_color( bmp->format->palette, 0, 255, params[2], params[3], params[4] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_get_rgb( INSTANCE * my, int * params )
{
    gr_get_rgb( params[0], ( int * )ptr_from_int(params[1]), ( int * )ptr_from_int(params[2]), ( int * )ptr_from_int(params[3]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_get_rgba( INSTANCE * my, int * params )
{
    gr_get_rgba( params[0], ( int * )ptr_from_int(params[1]), ( int * )ptr_from_int(params[2]), ( int * )ptr_from_int(params[3]), ( int * )ptr_from_int(params[4]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_rgb( INSTANCE * my, int * params )
{
    return gr_rgb( params[0], params[1], params[2] );
}

/* --------------------------------------------------------------------------- */

static int modmap_rgba( INSTANCE * my, int * params )
{
    return gr_rgba( params[0], params[1], params[2], params[3] );
}

/* --------------------------------------------------------------------------- */

static int modmap_get_rgb_depth( INSTANCE * my, int * params )
{
    gr_get_rgb_depth( params[4], params[0], ( int * )ptr_from_int(params[1]), ( int * )ptr_from_int(params[2]), ( int * )ptr_from_int(params[3]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_get_rgba_depth( INSTANCE * my, int * params )
{
    gr_get_rgba_depth( params[5], params[0], ( int * )ptr_from_int(params[1]), ( int * )ptr_from_int(params[2]), ( int * )ptr_from_int(params[3]), ( int * )ptr_from_int(params[4]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_rgb_depth( INSTANCE * my, int * params )
{
    return params[3] > 8 ?
            gr_rgb_depth( params[3], params[0], params[1], params[2] ) :
            gr_find_nearest_color( params[0], params[1], params[2] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_rgba_depth( INSTANCE * my, int * params )
{
    return params[4] > 8 ?
            gr_rgba_depth( params[4], params[0], params[1], params[2], params[3] ) :
            gr_find_nearest_color( params[0], params[1], params[2] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_get_rgb_map( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    _get_rgb( bmp->format, params[2], ( int * )ptr_from_int(params[3]), ( int * )ptr_from_int(params[4]), ( int * )ptr_from_int(params[5]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_get_rgba_map( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    _get_rgba( bmp->format, params[2], ( int * )ptr_from_int(params[3]), ( int * )ptr_from_int(params[4]), ( int * )ptr_from_int(params[5]), ( int * )ptr_from_int(params[6]) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_rgb_map( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    return _rgb( bmp->format, params[2], params[3], params[4] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_rgba_map( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    return _rgba( bmp->format, params[2], params[3], params[4], params[5] ) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_fade( INSTANCE * my, int * params )
{
    gr_fade_init( params[0], params[1], params[2], params[3] );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_fade_on( INSTANCE * my, int * params )
{
    gr_fade_init( 100, 100, 100, 16 ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_fade_off( INSTANCE * my, int * params )
{
    gr_fade_init( 0, 0, 0, 16 ) ;
    return 1;
}

/* --------------------------------------------------------------------------- */
/* Palette */

static int modmap_pal_create( INSTANCE * my, int * params )
{
    return ( int ) int_from_ptr( pal_new(( PALETTE * )NULL ) );
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_clone( INSTANCE * my, int * params )
{
    return ( int ) int_from_ptr( pal_new(( PALETTE * )( ptr_from_int(params[0]) ) ) );
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_unload( INSTANCE * my, int * params )
{
    pal_destroy(( PALETTE * )( ptr_from_int(params[0]) ) ) ;
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_refresh( INSTANCE * my, int * params )
{
    pal_refresh( NULL ) ;
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_refresh_2( INSTANCE * my, int * params )
{
    pal_refresh(( PALETTE * )( ptr_from_int(params[0]) ) );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_map_assign( INSTANCE * my, int * params )
{
    return pal_map_assign( params[0], params[1], ( PALETTE * )ptr_from_int( params[2] ) );
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_map_remove( INSTANCE * my, int * params )
{
    return pal_map_remove( params[0], params[1] );
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_map_getid( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    if ( !bmp || bmp->format->depth != 8 ) return 0 ;
    return ( int ) int_from_ptr(bmp->format->palette) ;
}

/* --------------------------------------------------------------------------- */

static int modmap_set_system_pal( INSTANCE * my, int * params )
{
    if ( pal_set(( PALETTE * )NULL, 0, 256, ( uint8_t * )(( PALETTE * )ptr_from_int(params[0]))->rgb ) )
    {
        pal_refresh( sys_pixel_format->palette );
        return 1;
    }
    return 0;
}

/* --------------------------------------------------------------------------- */

static int modmap_set_system_pal_raw( INSTANCE * my, int * params )
{
    if ( pal_set(( PALETTE * )NULL, 0, 256, ( uint8_t * )ptr_from_int(params[0]) ) )
    {
        pal_refresh( sys_pixel_format->palette );
        return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------- */

static int modmap_pal_set( INSTANCE * my, int * params )
{
    int ret = pal_set(( PALETTE * )ptr_from_int( params[0] ), params[1], params[2], ( uint8_t * )ptr_from_int(params[3]) ) ;
    if ( ret && !params[0] ) pal_refresh( sys_pixel_format->palette );
    return ret;
}

/* --------------------------------------------------------------------------- */

static int modmap_pal_get( INSTANCE * my, int * params )
{
    return ( pal_get(( PALETTE * )ptr_from_int( params[0] ), params[1], params[2], ( uint8_t * )ptr_from_int(params[3]) ) ) ;
}

/* ---------------------------------------------------------------------- */

/* Funciones de FPG */

static int modmap_load_fpg( INSTANCE * my, int * params )
{
    int r;
    r = gr_load_fpg( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modmap_save_fpg( INSTANCE * my, int * params )
{
    int r;
    r = gr_save_fpg( params[0], string_get( params[1] ) ) ;
    string_discard( params[1] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modmap_unload_fpg( INSTANCE * my, int * params )
{
    grlib_destroy( params[0] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modmap_fpg_exists( INSTANCE * my, int * params )
{
    GRLIB * lib = grlib_get( params[0] );
    return lib == NULL ? 0 : 1;
}

/* --------------------------------------------------------------------------- */

static int modmap_fpg_add( INSTANCE * my, int * params )
{
    GRAPH * orig = bitmap_get( params[2], params[3] );
    GRAPH * dest ;

    if ( orig == NULL ) return 0;
    dest = bitmap_clone( orig ) ;
    dest->code = params[1] ;

    return grlib_add_map( params[0], dest );
}

/* --------------------------------------------------------------------------- */

static int modmap_fpg_new( INSTANCE * my, int * params )
{
    return grlib_new();
}

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/** LOAD_FNT (STRING FILENAME)
 *  Load a .FNT font from disk (returns the font ID)
 */

static int modmap_load_fnt( INSTANCE * my, int * params )
{
    int r = gr_font_load( ( char * )string_get( params[0] ) );
    string_discard( params[0] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/** LOAD_BDF (STRING FILENAME)
 *  Load a .BDF font from disk (returns the font ID)
 */

static int modmap_load_bdf( INSTANCE * my, int * params )
{
    int r = gr_load_bdf( ( char * )string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */
/** UNLOAD_FNT (FONT)
 *  Destroys a font in memory
 */

static int modmap_unload_fnt( INSTANCE * my, int * params )
{
    if ( params[0] > 0 ) gr_font_destroy( params[0] );
    return 0;
}

/* --------------------------------------------------------------------------- */
/** FNT_NEW (DEPTH)
 *  Create a new font in memory (returns the font ID)
 */

static int modmap_fnt_new( INSTANCE * my, int * params )
{
    return gr_font_new( CHARSET_CP850, params[0] );
}

/* --------------------------------------------------------------------------- */

/** FNT_NEW (CHARSET, DEPTH)
 *  Create a new font in memory (returns the font ID)
 */

static int modmap_fnt_new_charset( INSTANCE * my, int * params )
{
    return gr_font_new( params[0], params[1] );
}

/* --------------------------------------------------------------------------- */

/** FNT_NEW (FILE, GRAPH, CHARSET, WIDTH, HEIGHT, FIRST, LAST, FLAGS)
 *  Create a new font in memory (returns the font ID)
 */

static int modmap_fnt_new_from_bitmap( INSTANCE * my, int * params )
{
    GRAPH * bmp = bitmap_get( params[0], params[1] ) ;
    if ( !bmp ) return -1;
    return gr_font_newfrombitmap( bmp, params[2], params[3], params[4], params[5], params[6], params[7] );
}

/* --------------------------------------------------------------------------- */
/** GET_GLYPH (FONT, GLYPH)
 *  Create a system map as a copy of one of the font glyphs (returns the map ID)
 */

static int modmap_get_glyph( INSTANCE * my, int * params )
{
    FONT  * font = gr_font_get( params[0] );
    GRAPH * map ;
    unsigned char c = params[1];

    if ( font->charset == /*CHARSET_CP850*/ CHARSET_ISO8859 ) c = win_to_dos[c];
    if ( !font ) return 0;
    if ( !font->glyph[c].bitmap ) return 0;

    map = bitmap_clone( font->glyph[c].bitmap );
    if ( !map ) return 0;

    map->code = bitmap_next_code();

    if ( !map->ncpoints ) bitmap_add_cpoint( map, map->width / 2, map->height / 2 );
    bitmap_add_cpoint( map, font->glyph[c].xoffset, font->glyph[c].yoffset );
    bitmap_add_cpoint( map, font->glyph[c].xadvance, font->glyph[c].yadvance );

    grlib_add_map( 0, map );

    return map->code;
}

/* --------------------------------------------------------------------------- */
/** SET_GLYPH (FONT, GLYPH, LIBRARY, GRAPHIC)
 *  Change one of the font's glyphs
 */

static int modmap_set_glyph( INSTANCE * my, int * params )
{
    FONT  * font = gr_font_get( params[0] );
    GRAPH * map  = bitmap_get( params[2], params[3] );
    unsigned char c = params[1];

    if ( font->charset == /*CHARSET_CP850*/CHARSET_ISO8859 ) c = win_to_dos[c];

    if ( font && map )
    {
        if ( font->glyph[c].bitmap ) grlib_unload_map( 0, font->glyph[c].bitmap->code );
        font->glyph[c].bitmap = bitmap_clone( map );
        if ( font->glyph[c].bitmap )
        {
            font->glyph[c].bitmap->code = bitmap_next_code();

            if ( map->ncpoints >= 3 && map->cpoints )
            {
                font->glyph[c].xoffset = map->cpoints[1].x;
                font->glyph[c].yoffset = map->cpoints[1].y;
                font->glyph[c].xadvance = map->cpoints[2].x;
                font->glyph[c].yadvance = map->cpoints[2].y;
            }
            else
            {
                font->glyph[c].xoffset = 0;
                font->glyph[c].yoffset = 0;
                font->glyph[c].xadvance = map->width + map->width / 5 ;
                font->glyph[c].yadvance = map->height + map->height / 5 ;
                bitmap_add_cpoint( font->glyph[c].bitmap, 0, 0 ) ;
            }
            grlib_add_map( 0, font->glyph[c].bitmap );
        }
    }
    return 0;
}

/* --------------------------------------------------------------------------- */
/** SAVE_FNT (FONT, STRING FILENAME)
 *  Saves a font to disk
 */

static int modmap_save_fnt( INSTANCE * my, int * params )
{
    int r = gr_font_save( params[0], ( char * )string_get( params[1] ) ) ;
    string_discard( params[1] ) ;
    return r ;
}

/* --------------------------------------------------------------------------- */
/**
   int LOAD_FPG(STRING FICHERO, INT POINTER VARIABLE)
   Loads fpg file FICHERO on a separate thread
   VARIABLE is -2 while waiting, -1 on error, >=0 otherwise
 **/

static int modmap_bgload_fpg( INSTANCE * my, int * params )
{
    bgload( gr_load_fpg, params );
    return 0 ;
}

static int modmap_bgload_map( INSTANCE * my, int * params )
{
    bgload( gr_load_map, params ) ;
    return 0 ;
}

static int modmap_bgload_png( INSTANCE * my, int * params )
{
    bgload( gr_load_png, params ) ;
    return 0 ;
}

static int modmap_bgload_pcx( INSTANCE * my, int * params )
{
    bgload( gr_load_pcx, params );
    return 0 ;
}

static int modmap_bgload_pal( INSTANCE * my, int * params )
{
    bgload( gr_load_pal, params ) ;
    return 0 ;
}

static int modmap_bgload_fnt( INSTANCE * my, int * params )
{
    bgload( gr_font_load, params );
    return 0 ;
}

static int modmap_bgload_bdf( INSTANCE * my, int * params )
{
    bgload( gr_load_bdf, params ) ;
    return 0 ;
}

/* ----------------------------------------------------------------- */
/* exports                                                           */
/* ----------------------------------------------------------------- */

#include "mod_map_exports.h"

/* ----------------------------------------------------------------- */
