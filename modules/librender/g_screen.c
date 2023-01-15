/*
 *  Copyright ï¿½ 2006-2019 SplinterGU (Fenix/Bennugd)
 *  Copyright ï¿½ 2002-2006 Fenix Team (Fenix)
 *  Copyright ï¿½ 1999-2002 Josï¿½ Luis Cebriï¿½n Pagï¿½e (Fenix)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librender.h"

/* --------------------------------------------------------------------------- */

static GRAPH * scrbitmap_extra = NULL ;

static int updaterects_count = 0;
static REGION updaterects[ DIRTYCOLS * DIRTYROWS ];
static SDL_Rect rects[ DIRTYCOLS * DIRTYROWS ];

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_draw_screen
 *
 *  Draw the current screen, using the given restore_type / dump_type parameters
 *
 *  PARAMS :
 *      dest            Destination graphic (MUST have a correct size)
 *
 *      restore_type
 *          -1          No background restore
 *           0          Partial (old process bounding box) background restore
 *           1          Full background restore
 *
 *      dump_type
 *           0          Partial (new process bounding box) drawing
 *           1          Full drawing
 *
 *  RETURN VALUE :
 *      None
 */

void gr_draw_screen( GRAPH * dest, int restore_type, int dump_type )
{
    int a ;
    GRAPH * orig_scrbitmap = scrbitmap;

    scrbitmap = dest ;

    if ( background && background->modified )
    {
        restore_type = 1;
        dump_type = 1;
    }

    /* Update the object list */
    gr_update_objects_mark_rects( restore_type, dump_type );

    /* Restore the background */
    if ( !dump_type || !restore_type )
    {
        updaterects_count = gr_mark_rects( updaterects );
    }
    else
    {
        updaterects_count = 1;
        updaterects[ 0 ].x = 0;
        updaterects[ 0 ].y = 0;
        updaterects[ 0 ].x2 = scr_width - 1;
        updaterects[ 0 ].y2 = scr_height - 1;
    }

    if ( !restore_type )
    {
        /* PARTIAL_RESTORE */
        if ( background->info_flags & GI_CLEAN )
        {
            for ( a = 0; a < updaterects_count; a++ )
                gr_clear_region( scrbitmap, &updaterects[ a ] );
        }
        else
        {
            for ( a = 0; a < updaterects_count; a++ )
                gr_blit( scrbitmap, &updaterects[ a ], 0, 0, B_NOCOLORKEY, background );
        }
    }
    else if ( restore_type == 1 )
    {
        /* COMPLETE_RESTORE */
        if ( background->info_flags & GI_CLEAN )
            gr_clear( scrbitmap ) ;
        else
            gr_blit( scrbitmap, NULL, 0, 0, B_NOCOLORKEY, background );

        updaterects_count = 1;
        updaterects[ 0 ].x = 0;
        updaterects[ 0 ].y = 0;
        updaterects[ 0 ].x2 = scr_width - 1;
        updaterects[ 0 ].y2 = scr_height - 1;
    }

    /* Dump the objects */
    if ( dump_type == 0 )
    {
        /* Dump only changed & enabled objects */
        gr_draw_objects( updaterects, updaterects_count );
    }
    else
    {
        /* Dump everything */
        gr_draw_objects_complete();
    }

    /* Reset the zone-to-update array for the next frame */
    gr_rects_clear();

    if ( background && background->modified ) background->modified = 0;
    if ( scrbitmap && scrbitmap->modified ) scrbitmap->modified = 0;

    scrbitmap = orig_scrbitmap;
}

/* --------------------------------------------------------------------------- */

static int screen_locked = 0 ;

int gr_lock_screen()
{
    if ( screen_locked ) return 1 ;

    screen_locked = 1 ;

    if ( scale_resolution != -1 )
    {
        if ( SDL_MUSTLOCK( scale_screen ) ) SDL_LockSurface( scale_screen ) ;
    }
    else
    {
        if ( SDL_MUSTLOCK( screen ) ) SDL_LockSurface( screen ) ;
    }

    if ( enable_scale || scale_mode != SCALE_NONE )
    {
        if ( scrbitmap && ( scrbitmap->info_flags & GI_EXTERNAL_DATA ) )
        {
            bitmap_destroy( scrbitmap ) ;
            scrbitmap = NULL ;
        }

        if ( !scrbitmap )
        {
            scrbitmap = bitmap_new( 0, screen->w / 2, screen->h / 2, sys_pixel_format->depth ) ;
            bitmap_add_cpoint( scrbitmap, 0, 0 ) ;
        }
    }
    else
    {
        if ( !scrbitmap || !( scrbitmap->info_flags & GI_EXTERNAL_DATA ) )
        {
            if ( scrbitmap ) bitmap_destroy( scrbitmap ) ;
            scrbitmap = bitmap_new_ex( 0, screen->w, screen->h, screen->format->BitsPerPixel, screen->pixels, screen->pitch );
            bitmap_add_cpoint( scrbitmap, 0, 0 ) ;
        }
    }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

void gr_unlock_screen()
{
    if ( !screen_locked || !screen->pixels ) return ;

    screen_locked = 0 ;

    if ( scale_resolution != -1 )
    {
        uint8_t  * src8  = screen->pixels, * dst8  = scale_screen->pixels , * pdst = scale_screen->pixels ;
        uint16_t * src16 = screen->pixels, * dst16 = scale_screen->pixels ;
        uint32_t * src32 = screen->pixels, * dst32 = scale_screen->pixels ;
        int h, w;

        switch ( scale_screen->format->BitsPerPixel )
        {
            case    8:
                    if ( scale_resolution_orientation == 1 || scale_resolution_orientation == 3 )
                    {
                        if ( scale_resolution_aspectratio )
                        {
                            for ( w = 0; w < scale_screen->w; w++ )
                            {
                                if ( scale_resolution_table_w[w] != -1 )
                                {
                                    src8 = screen->pixels + scale_resolution_table_w[w];
                                    for ( h = scale_screen->h - 1; h-- ; )
                                    {
                                        if ( scale_resolution_table_h[h] != -1 ) *dst8 = src8[scale_resolution_table_h[h]];
                                        dst8 += scale_screen->pitch ;
                                    }
                                }
                                dst8 = pdst += scale_screen->format->BytesPerPixel ;
                            }
                        }
                        else
                        {
                            for ( w = 0; w < scale_screen->w; w++ )
                            {
                                src8 = screen->pixels + scale_resolution_table_w[w];
                                for ( h = scale_screen->h - 1; h-- ; )
                                {
                                    *dst8 = src8[scale_resolution_table_h[h]];
                                    dst8 += scale_screen->pitch ;
                                }
                            }
                            dst8 = pdst += scale_screen->format->BytesPerPixel ;
                        }
                    }
                    else
                    {
                        if ( scale_resolution_aspectratio )
                        {
                            for ( h = 0; h < scale_screen->h; h++ )
                            {
                                if ( scale_resolution_table_h[h] != -1 )
                                {
                                    src8 = screen->pixels + scale_resolution_table_h[h];
                                    for ( w = 0; w < scale_screen->w; w++ )
                                    {
                                        if ( scale_resolution_table_w[w] != -1 ) *dst8 = src8[scale_resolution_table_w[w]];
                                        dst8++;
                                    }
                                }
                                dst8 = pdst += scale_screen->pitch ;
                            }
                        }
                        else
                        {
                            for ( h = 0; h < scale_screen->h; h++ )
                            {
                                src8 = screen->pixels + scale_resolution_table_h[h];
                                for ( w = 0; w < scale_screen->w; w++ )
                                {
                                    *dst8 = src8[scale_resolution_table_w[w]];
                                    dst8++;
                                }
                                dst8 = pdst += scale_screen->pitch ;
                            }
                        }
                    }
                    break;

            case    16:
                    if ( scale_resolution_orientation == 1 || scale_resolution_orientation == 3 )
                    {
                        if ( scale_resolution_aspectratio )
                        {
                            int inc = scale_screen->pitch / scale_screen->format->BytesPerPixel;
                            for ( w = 0; w < scale_screen->w; w++ )
                            {
                                if ( scale_resolution_table_w[w] != -1 )
                                {
                                    src16 = screen->pixels + scale_resolution_table_w[w];
                                    for ( h = scale_screen->h - 1; h-- ; )
                                    {
                                        if ( scale_resolution_table_h[h] != -1 ) *dst16 = src16[scale_resolution_table_h[h]];
                                        dst16 += inc;
                                    }
                                }
                                dst16 = ( uint16_t * ) ( pdst += scale_screen->format->BytesPerPixel ) ;
                            }
                        }
                        else
                        {
                            int inc = scale_screen->pitch / scale_screen->format->BytesPerPixel;
                            for ( w = 0; w < scale_screen->w; w++ )
                            {
                                src16 = screen->pixels + scale_resolution_table_w[w];
                                for ( h = scale_screen->h - 1; h-- ; )
                                {
                                    *dst16 = src16[scale_resolution_table_h[h]];
                                    dst16 += inc;
                                }
                                dst16 = ( uint16_t * ) ( pdst += scale_screen->format->BytesPerPixel ) ;
                            }
                        }
                    }
                    else
                    {
                        if ( scale_resolution_aspectratio )
                        {
                            for ( h = 0; h < scale_screen->h; h++ )
                            {
                                if ( scale_resolution_table_h[h] != -1 )
                                {
                                    src16 = screen->pixels + scale_resolution_table_h[h];
                                    for ( w = 0; w < scale_screen->w; w++ )
                                    {
                                        if ( scale_resolution_table_w[w] != -1 ) *dst16 = src16[scale_resolution_table_w[w]];
                                        dst16++;
                                    }
                                }
                                dst16 = ( uint16_t * ) ( pdst += scale_screen->pitch ) ;
                            }
                        }
                        else
                        {
                            for ( h = 0; h < scale_screen->h; h++ )
                            {
                                src16 = screen->pixels + scale_resolution_table_h[h];
                                for ( w = 0; w < scale_screen->w; w++ )
                                {
                                    *dst16 = src16[scale_resolution_table_w[w]];
                                    dst16++;
                                }
                                dst16 = ( uint16_t * ) ( pdst += scale_screen->pitch ) ;
                            }
                        }
                    }
                    break;

            case    32:
                    if ( scale_resolution_orientation == 1 || scale_resolution_orientation == 3 )
                    {
                        if ( scale_resolution_aspectratio )
                        {
                            int inc = scale_screen->pitch / scale_screen->format->BytesPerPixel;
                            for ( w = 0; w < scale_screen->w; w++ )
                            {
                                if ( scale_resolution_table_w[w] != -1 )
                                {
                                    src32 = screen->pixels + scale_resolution_table_w[w];
                                    for ( h = scale_screen->h - 1; h-- ; )
                                    {
                                        if ( scale_resolution_table_h[h] != -1 ) *dst32 = src32[scale_resolution_table_h[h]];
                                        dst32 += inc;
                                    }
                                }
                                dst32 = ( uint32_t * ) ( pdst += scale_screen->format->BytesPerPixel ) ;
                            }
                        }
                        else
                        {
                            int inc = scale_screen->pitch / scale_screen->format->BytesPerPixel;
                            for ( w = 0; w < scale_screen->w; w++ )
                            {
                                src32 = screen->pixels + scale_resolution_table_w[w];
                                for ( h = scale_screen->h - 1; h-- ; )
                                {
                                    *dst32 = src32[scale_resolution_table_h[h]];
                                    dst32 += inc;
                                }
                                dst32 = ( uint32_t * ) ( pdst += scale_screen->format->BytesPerPixel ) ;
                            }
                        }
                    }
                    else
                    {
                        if ( scale_resolution_aspectratio )
                        {
                            for ( h = 0; h < scale_screen->h; h++ )
                            {
                                if ( scale_resolution_table_h[h] != -1 )
                                {
                                    src32 = screen->pixels + scale_resolution_table_h[h];
                                    for ( w = 0; w < scale_screen->w; w++ )
                                    {
                                        if ( scale_resolution_table_w[w] != -1 ) *dst32 = src32[scale_resolution_table_w[w]];
                                        dst32++;
                                    }
                                }
                                dst32 = ( uint32_t * ) ( pdst += scale_screen->pitch ) ;
                            }
                        }
                        else
                        {
                            for ( h = 0; h < scale_screen->h; h++ )
                            {
                                src32 = screen->pixels + scale_resolution_table_h[h];
                                for ( w = 0; w < scale_screen->w; w++ )
                                {
                                    *dst32 = src32[scale_resolution_table_w[w]];
                                    dst32++;
                                }
                                dst32 = ( uint32_t * ) ( pdst += scale_screen->pitch ) ;
                            }
                        }
                    }
                    break;
        }

        if ( SDL_MUSTLOCK( scale_screen ) ) SDL_UnlockSurface( scale_screen ) ;
        if ( waitvsync ) gr_wait_vsync();
        SDL_Flip( scale_screen ) ;
    }
    else if ( enable_scale )
    {
        GRAPH * scr;

        if ( scrbitmap->format->depth == 8 )
        {
            uint8_t * original, * poriginal;
            uint16_t * extra, * pextra;
            int n = scrbitmap->height, length;

            if (
                !scrbitmap_extra ||
                scrbitmap_extra->width != scrbitmap->width ||
                scrbitmap_extra->height != scrbitmap->height
            )
            {
                if ( scrbitmap_extra ) bitmap_destroy( scrbitmap_extra );
                scrbitmap_extra = bitmap_new( 0, scrbitmap->width, scrbitmap->height, 16 );
            }

            poriginal = scrbitmap->data;
            pextra = scrbitmap_extra->data;

            while ( n-- )
            {
                original = poriginal;
                extra = pextra;
                length = scrbitmap->width;
                while ( length-- ) *extra++ = sys_pixel_format->palette->colorequiv[ *original++ ];
                poriginal += scrbitmap->pitch;
                pextra = ( uint16_t * )((( uint8_t * ) pextra ) + scrbitmap_extra->pitch );
            }

            scr = scrbitmap_extra;
        }
        else
        {
            scr = scrbitmap;
        }

        /* Esto podria ir en un modulo aparte */
        switch ( scale_mode )
        {
            case SCALE_SCALE2X:
                scale2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_HQ2X:
                hq2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_SCANLINE2X:
                scanline2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_NOFILTER:
                scale_normal2x( scr->data, scr->pitch, screen->pixels, screen->pitch, scr->width, scr->height );
                break;

            case SCALE_NONE:
                // No usado
                break;
        }

        if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
        if ( waitvsync ) gr_wait_vsync();
        SDL_Flip( screen ) ;
    }
    else if ( scrbitmap->info_flags & GI_EXTERNAL_DATA )
    {
        if ( double_buffer ||
                (
                    updaterects_count == 1 &&
                    updaterects[0].x == 0 &&
                    updaterects[0].y == 0 &&
                    updaterects[0].x2 == scr_width - 1 &&
                    updaterects[0].y2 == scr_height - 1
                )
           )
        {
            if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
            if ( waitvsync ) gr_wait_vsync();
            SDL_Flip( screen ) ;
        }
        else
        {
            if ( updaterects_count )
            {
                int i;

                for ( i = 0 ; i < updaterects_count ; i++ )
                {
                    rects[ i ].x = updaterects[ i ].x;
                    rects[ i ].y = updaterects[ i ].y;
                    rects[ i ].w = ( updaterects[ i ].x2 - rects[ i ].x + 1 );
                    rects[ i ].h = ( updaterects[ i ].y2 - rects[ i ].y + 1 );
                }
                if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
                if ( waitvsync ) gr_wait_vsync();
                SDL_UpdateRects( screen, updaterects_count, rects ) ;
            }
        }
    }
}

/* --------------------------------------------------------------------------- */
