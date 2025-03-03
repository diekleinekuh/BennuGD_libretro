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

/* --------------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

#include "libvideo.h"

#ifdef _WIN32

#if LIBRETRO_CORE
// assume we have compiled in compat_posix_string
extern char *retro_strtok_r__(char *str, const char *delim, char **saveptr);
#define strtok_r retro_strtok_r__

#else //LIBRETRO_CORE

#ifdef _MSC_VER
static char *strtok_r(char *str, const char *delim, char **saveptr)
{
   char *first = NULL;
   if (!saveptr || !delim)
      return NULL;

   if (str)
      *saveptr = str;

   do
   {
      char *ptr = NULL;
      first = *saveptr;
      while (*first && strchr(delim, *first))
         *first++ = '\0';

      if (*first == '\0')
         return NULL;

      ptr = first + 1;

      while (*ptr && !strchr(delim, *ptr))
         ptr++;

      *saveptr = ptr + (*ptr ? 1 : 0);
      *ptr     = '\0';
   } while (strlen(first) == 0);

   return first;
}
#endif

#include <initguid.h>
#include "ddraw.h"

#endif 
#endif

/* --------------------------------------------------------------------------- */

GRAPH * icon = NULL ;

SDL_Surface * screen = NULL ;
SDL_Surface * scale_screen = NULL ;

char * apptitle = NULL ;

int scr_width = 0 ;
int scr_height = 0 ;

int scr_initialized = 0 ;

int enable_16bits = 0 ;
int enable_32bits = 0 ;
int enable_scale = 0 ;
int full_screen = 0 ;
int double_buffer = 0 ;
int hardware_scr = 0 ;
int grab_input = 0 ;
int frameless = 0 ;
int scale_mode = SCALE_NONE ;
int waitvsync = 0 ;

int scale_resolution = -1 ;
int * scale_resolution_table_w = NULL;
int * scale_resolution_table_h = NULL;
int scale_resolution_aspectratio = 0;
int scale_resolution_orientation = 0;

int scale_resolution_aspectratio_offx = 0;
int scale_resolution_aspectratio_offy = 0;

/* --------------------------------------------------------------------------- */

static struct {
    int width;
    int height;
    int depth;
    int fullscreen;
    int scale_resolution;
    int scale_resolution_aspectratio;
    int scale_resolution_orientation;
} video_config = { 0, 0, 0, 0, -1, -1, -1 };

/* --------------------------------------------------------------------------- */

enum {
    GRAPH_MODE = 0,
    SCALE_MODE,
    FULL_SCREEN,
    SCALE_RESOLUTION,
    SCALE_RESOLUTION_ASPECTRATIO,
    SCALE_RESOLUTION_ORIENTATION
};

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( libvideo, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaï¿½o del elemento, cantidad de elementos */
    { "graph_mode" , NULL, -1, -1 },
    { "scale_mode" , NULL, -1, -1 },
    { "full_screen" , NULL, -1, -1 },
    { "scale_resolution", NULL, -1, -1 },

    /* new vars for use with scale_resolution */
    { "scale_resolution_aspectratio", NULL, -1, -1 },
    { "scale_resolution_orientation", NULL, -1, -1 },

    { NULL , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

#if !LIBRETRO_CORE
#ifdef _WIN32
/* Based allegro */

LPDIRECTDRAW2 directdraw = NULL;
DDCAPS ddcaps;

HRESULT  (WINAPI *_DirectDrawCreate )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

/* --------------------------------------------------------------------------- */

int init_dx( void )
{
    HINSTANCE handle;
    LPDIRECTDRAW directdraw1;
    HRESULT hr;
    LPVOID temp;

    handle = LoadLibrary( "DDRAW.DLL" );
    if ( handle == NULL ) return -1;

    _DirectDrawCreate = (HRESULT(WINAPI*)(GUID*,LPDIRECTDRAW*,IUnknown*))GetProcAddress( handle, "DirectDrawCreate" );

    hr = _DirectDrawCreate( NULL, &directdraw1, NULL );
    if ( FAILED( hr ) ) return -1;

    hr = IDirectDraw_QueryInterface( directdraw1, &IID_IDirectDraw2, (void**)&directdraw );
    if ( FAILED( hr ) ) return -1;

    IDirectDraw_Release( directdraw1 );

    hr = IDirectDraw2_SetCooperativeLevel( directdraw, NULL, DDSCL_NORMAL );
    if ( FAILED( hr ) ) return -1;

    /* get capabilities */
    ddcaps.dwSize = sizeof( ddcaps );
    hr = IDirectDraw2_GetCaps( directdraw, &ddcaps, NULL );
    if ( FAILED( hr ) ) return -1;

    return 0;
}
#endif
#endif // !LIBRETRO_CORE
/* --------------------------------------------------------------------------- */

void gr_wait_vsync()
{
#if !LIBRETRO_CORE
#ifdef _WIN32
    if ( directdraw ) IDirectDraw2_WaitForVerticalBlank( directdraw, DDWAITVB_BLOCKBEGIN, NULL );
#endif
#endif
}

/* --------------------------------------------------------------------------- */

void gr_set_caption( char * title )
{
    SDL_WM_SetCaption( apptitle = title, "" ) ;
}

/* --------------------------------------------------------------------------- */

int gr_set_icon( GRAPH * map )
{
    if (( icon = map ))
    {
        SDL_Surface *ico = NULL;
        if ( icon->format->depth == 8 )
        {
            SDL_Color palette[256];
            if ( sys_pixel_format && sys_pixel_format->palette )
            {
                int n ;
                for ( n = 0 ; n < 256 ; n++ )
                {
                    palette[ n ].r = sys_pixel_format->palette->rgb[ n ].r;
                    palette[ n ].g = sys_pixel_format->palette->rgb[ n ].g;
                    palette[ n ].b = sys_pixel_format->palette->rgb[ n ].b;
                }
            }

            ico = SDL_CreateRGBSurfaceFrom( icon->data, 32, 32, 8, 32, 0x00, 0x00, 0x00, 0x00 ) ;
            SDL_SetPalette( ico, SDL_LOGPAL, palette, 0, 256 );
        }
        else
        {
            ico = SDL_CreateRGBSurfaceFrom( icon->data, 32, 32, icon->format->depth, icon->pitch, icon->format->Rmask, icon->format->Gmask, icon->format->Bmask, icon->format->Amask ) ;
        }

        SDL_SetColorKey( ico, SDL_SRCCOLORKEY, SDL_MapRGB( ico->format, 0, 0, 0 ) ) ;
        SDL_WM_SetIcon( ico, NULL );
        SDL_FreeSurface( ico ) ;
    }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

int gr_set_mode( int width, int height, int depth )
{
#if LIBRETRO_CORE
extern int libretro_depth;
extern int bennugd_content_width;
extern int bennugd_content_height;

    depth=libretro_depth;
    bennugd_content_width = width;
    bennugd_content_height = height;
#endif
    int n ;
    int sdl_flags = 0;
    int surface_width = width;
    int surface_height = height;

    enable_scale = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_2XSCALE ) ? 1 : 0 ;
    full_screen = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_FULLSCREEN ) ? 1 : 0 ;
    double_buffer = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_DOUBLEBUFFER ) ? 1 : 0 ;
    hardware_scr = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_HARDWARE ) ? 1 : 0 ;
    grab_input = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_MODAL ) ? 1 : 0 ;
    frameless = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_FRAMELESS ) ? 1 : 0 ;
    waitvsync = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_WAITVSYNC ) ? 1 : 0 ;
    scale_mode = GLODWORD( libvideo, SCALE_MODE );
    full_screen |= GLODWORD( libvideo, FULL_SCREEN );

    scale_resolution = GLODWORD( libvideo, SCALE_RESOLUTION );

    if ( GLOEXISTS( libvideo, SCALE_RESOLUTION_ASPECTRATIO ) ) scale_resolution_aspectratio = GLODWORD( libvideo, SCALE_RESOLUTION_ASPECTRATIO );
    if ( GLOEXISTS( libvideo, SCALE_RESOLUTION_ORIENTATION ) ) scale_resolution_orientation = GLODWORD( libvideo, SCALE_RESOLUTION_ORIENTATION );

    /* Overwrite all params */

    if ( ( video_config.scale_resolution             != -1 ) ) scale_resolution = video_config.scale_resolution;
    if ( ( video_config.scale_resolution_aspectratio != -1 ) ) scale_resolution_aspectratio = video_config.scale_resolution_aspectratio;
    if ( ( video_config.scale_resolution_orientation != -1 ) ) scale_resolution_orientation = video_config.scale_resolution_orientation;

    if ( scale_resolution_orientation < 0 || scale_resolution_orientation > 4 ) scale_resolution_orientation = 0;

#if LIBRETRO_CORE
    extern int libretro_scale_override;
    if (libretro_scale_override>=0)
    {
        scale_mode = libretro_scale_override;
    }
#endif

    if ( !depth )
    {
        enable_32bits = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_32BITS ) ? 1 : 0 ;
        if ( !enable_32bits )
            enable_16bits = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_16BITS ) ? 1 : 0 ;
        else
            enable_16bits = 0;
        depth = enable_32bits ? 32 : ( enable_16bits ? 16 : 8 );
    }
    else if ( depth == 16 )
    {
        enable_16bits = 1;
        enable_32bits = 0;
    }
    else if ( depth == 32 )
    {
        enable_16bits = 0;
        enable_32bits = 1;
    }

    if ( scale_resolution_table_w )
    {
        bgd_free( scale_resolution_table_w );
        scale_resolution_table_w = NULL;
    }

    if ( scale_resolution_table_h )
    {
        bgd_free( scale_resolution_table_h );
        scale_resolution_table_h = NULL;
    }

    if ( scale_resolution != -1 )
    {
        surface_width  = scale_resolution / 10000 ;
        surface_height = scale_resolution % 10000 ;
    }
    else
    {
        if ( scale_mode != SCALE_NONE ) enable_scale = 1;
        if ( enable_scale && scale_mode == SCALE_NONE ) scale_mode = SCALE_SCALE2X;

        if ( enable_scale )
        {
            enable_16bits = 1;
            depth = 16;

            surface_width  *= 2;
            surface_height *= 2;
        }
    }

    /* Inicializa el modo grï¿½fico */

    if ( scrbitmap )
    {
        bitmap_destroy( scrbitmap ) ;
        scrbitmap = NULL ;
    }

    /* Setup the SDL Video Mode */

    sdl_flags = SDL_HWPALETTE;
    if ( double_buffer ) sdl_flags |= SDL_DOUBLEBUF;
    if ( full_screen ) sdl_flags |= SDL_FULLSCREEN;
    if ( frameless ) sdl_flags |= SDL_NOFRAME;

    sdl_flags |= hardware_scr ? SDL_HWSURFACE : SDL_SWSURFACE;

    if ( scale_screen )
    {
        SDL_FreeSurface( screen );
        screen = NULL;
        scale_screen = NULL;
    }

    if ( scale_resolution != -1 )
    {
        switch ( scale_resolution_orientation )
        {
            case    SRO_LEFT:
            case    SRO_RIGHT:
            {
                    int aux =  surface_width;
                    surface_width = surface_height;
                    surface_height = aux;
                    break;
            }
        }

        scale_screen = SDL_SetVideoMode( surface_width, surface_height, depth, sdl_flags );

        if ( !scale_screen ) return -1;

        if ( !width && !height )
        {
            width = scale_screen->w;
            height = scale_screen->h;
        }

        screen = SDL_CreateRGBSurface( sdl_flags,
                                       width,
                                       height,
                                       scale_screen->format->BitsPerPixel,
                                       scale_screen->format->Rmask,
                                       scale_screen->format->Gmask,
                                       scale_screen->format->Bmask,
                                       scale_screen->format->Amask
                                     );

        /* scale tables */

        int     lim_w = 0, lim_h = 0, pitch_w = 0, pitch_h = 0;
        double  fw = 0.0, fh = 0.0, fx = 0.0, fy = 0.0;
        int     h, w;
        int     start_w = 0, start_h = 0, fix = 1;

        scale_resolution_aspectratio_offx = 0;
        scale_resolution_aspectratio_offy = 0;

        switch ( scale_resolution_orientation )
        {
            case    SRO_NORMAL:
            case    SRO_DOWN:
                    lim_w = screen->w;
                    lim_h = screen->h;

                    pitch_w = 1;
                    pitch_h = screen->pitch;

                    fw = (double)screen->w / (double)scale_screen->w;
                    fh = (double)screen->h / (double)scale_screen->h;
                    break;

            case    SRO_LEFT:
            case    SRO_RIGHT:
                    lim_w = screen->h;
                    lim_h = screen->w;

                    pitch_w = screen->pitch;
                    pitch_h = 1;

                    fh = (double)screen->w / (double)scale_screen->h;
                    fw = (double)screen->h / (double)scale_screen->w;
                    break;
        }

        switch ( scale_resolution_orientation )
        {
            case    SRO_NORMAL:
            case    SRO_LEFT:
                    start_w = 0;
                    start_h = 0;
                    fix = -1;
                    break;

            case    SRO_DOWN:
            case    SRO_RIGHT:
                    start_w = scale_screen->w - 1;
                    start_h = scale_screen->h - 1;
                    fix = 1;
                    break;
        }

        if ( scale_resolution_aspectratio == SRA_PRESERVE )
        {
            if ( scale_screen->w > scale_screen->h )
            {
                fw = fh;
                scale_resolution_aspectratio_offx = ( scale_screen->w - lim_w / fw ) / 2 ;
                scale_resolution_aspectratio_offy = 0;
            }
            else
            {
                fh = fw;
                scale_resolution_aspectratio_offx = 0;
                scale_resolution_aspectratio_offy = ( scale_screen->h - lim_h / fh ) / 2 ;
            }
        }

        if ( !( scale_resolution_table_w = bgd_malloc( scale_screen->w * sizeof( int ) ) ) ) return -1;
        if ( !( scale_resolution_table_h = bgd_malloc( scale_screen->h * sizeof( int ) ) ) ) return -1;

        for ( w = 0; w < scale_screen->w; w++ )
        {
            if ( w < scale_resolution_aspectratio_offx )
                scale_resolution_table_w[ start_w - w * fix ] = -1;
            else
            {
                scale_resolution_table_w[ start_w - w * fix ] = ( fx < lim_w ) ? pitch_w * ( int ) fx : -1 ;
                fx += fw;
            }
        }

        for ( h = 0; h < scale_screen->h; h++ )
        {
            if ( h < scale_resolution_aspectratio_offy )
                scale_resolution_table_h[ start_h - h * fix ] = -1;
            else
            {
                scale_resolution_table_h[ start_h - h * fix ] = ( fy < lim_h ) ? pitch_h * ( int ) fy : -1 ;
                fy += fh;
            }
        }
    }
    else
    {
        screen = SDL_SetVideoMode( surface_width, surface_height, depth, sdl_flags );
    }

    if ( !screen ) return -1;

    SDL_WM_GrabInput( grab_input ? SDL_GRAB_ON : SDL_GRAB_OFF ) ;

    /* Set window title */
    gr_set_caption( apptitle ) ;

    if ( !sys_pixel_format )
    {
        sys_pixel_format = bitmap_create_format( depth );
    }
    else
    {
        PALETTE * p = sys_pixel_format->palette;

        bgd_free( sys_pixel_format );
        sys_pixel_format = bitmap_create_format( depth );

        if ( p )
        {
            sys_pixel_format->palette = p;
            pal_refresh( sys_pixel_format->palette ) ;
        }
    }

    if ( sys_pixel_format->depth == 16 )
    {
        for ( n = 0 ; n < 65536 ; n++ )
        {
            colorghost[ n ] =
                ((( n & screen->format->Rmask ) >> 1 ) & screen->format->Rmask ) +
                ((( n & screen->format->Gmask ) >> 1 ) & screen->format->Gmask ) +
                ((( n & screen->format->Bmask ) >> 1 ) & screen->format->Bmask ) ;
        }
//        bitmap_16bits_conversion();
    }

    scr_initialized = 1 ;

    SDL_ShowCursor( 0 ) ;

    pal_refresh( NULL ) ;
    palette_changed = 1 ;

//    gr_make_trans_table();
/*
    if ( scale_resolution == -1 && enable_scale )
    {
        surface_width  = screen->w * 2;
        surface_height = screen->h * 2;
    }
*/
    /* Bitmaps de fondo */

    scr_width = screen->w / ( scale_mode != 0 ? 2 : 1 ) ;
    scr_height = screen->h / ( scale_mode != 0 ? 2 : 1 ) ;

    /* Only allow background with same properties that video mode */
    if (
        !background ||
        background->width != scr_width || background->height != scr_height ||
        sys_pixel_format->depth != background->format->depth )
    {
        if ( background ) bitmap_destroy( background );
        background = bitmap_new( 0, scr_width, scr_height, sys_pixel_format->depth ) ;
        if ( background )
        {
            gr_clear( background ) ;
            bitmap_add_cpoint( background, 0, 0 ) ;
        }
    }

    regions[0].x  = 0 ;
    regions[0].y  = 0 ;
    regions[0].x2 = scr_width - 1 ;
    regions[0].y2 = scr_height - 1 ;

    // Finalmente seteamos icono de aplicacion
    // Necesitamos crear una surface a partir de un MAP generico de 16x16...
    gr_set_icon( icon );

    if ( background ) background->modified = 1;

//    gr_rects_clear();
    return 0;
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_initialize )()
{
    char * e;

    GLODWORD( libvideo, SCALE_RESOLUTION ) = -1; // hack for backward compatibility

    if ( !SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_InitSubSystem( SDL_INIT_VIDEO );

#if !LIBRETRO_CORE
#ifdef _WIN32
    if ( !directdraw ) init_dx();
#endif
#endif
    apptitle = appname;

#if LIBRETRO_CORE
extern int libretro_width;
extern int libretro_height;
    video_config.width=libretro_width;
    video_config.height=libretro_height;
#endif

    if ( ( e = getenv( "SCALE_RESOLUTION"             ) ) ) video_config.scale_resolution = atol( e );
    if ( ( e = getenv( "SCALE_RESOLUTION_ASPECTRATIO" ) ) ) video_config.scale_resolution_aspectratio = atol( e );
    if ( ( e = getenv( "SCALE_RESOLUTION_ORIENTATION" ) ) ) video_config.scale_resolution_orientation = atol( e );

    if ( ( e = getenv( "VIDEO_WIDTH"                  ) ) ) video_config.width = atoi(e);
    if ( ( e = getenv( "VIDEO_HEIGHT"                 ) ) ) video_config.height = atoi(e);
    if ( ( e = getenv( "VIDEO_DEPTH"                  ) ) ) video_config.depth = atoi(e);
    if ( ( e = getenv( "VIDEO_FULLSCREEN"             ) ) ) GLODWORD( libvideo, GRAPH_MODE ) = atoi(e) ? MODE_FULLSCREEN : 0;
    else                                                    GLODWORD( libvideo, GRAPH_MODE ) = video_config.fullscreen ? MODE_FULLSCREEN : 0;

    gr_set_mode( video_config.width, video_config.height, video_config.depth ) ;
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_finalize )()
{
#if !LIBRETRO_CORE
#ifdef _WIN32
    if ( directdraw )
    {
        /* set cooperative level back to normal */
        IDirectDraw2_SetCooperativeLevel( directdraw, NULL, DDSCL_NORMAL );

        /* release DirectDraw interface */
        IDirectDraw2_Release( directdraw );

        directdraw = NULL;
    }
#endif
#endif
    if ( SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_config )(int line, char * section, char * key, char * value )
{
    if ( !strcmp( section, "bennugd" ) ) {
        if ( !strcmp( key, "video.mode" ) ) {
            char * saveptr = NULL, * token;
            // width
            if ( ( token = strtok_r(value, "x", &saveptr) ) ) {
                video_config.width = atoi( token );
                // height
                if ( ( token = strtok_r(NULL, "x", &saveptr) ) ) {
                    video_config.height = atoi( token );
                    // depth
                    if ( ( token = strtok_r(NULL, "x", &saveptr) ) ) {
                        video_config.depth = atoi( token );
                    }
                }
            }
        } else
        if ( !strcmp( key, "video.fullscreen" ) ) {
            video_config.fullscreen = atoi( value );
        } else
        if ( !strcmp( key, "video.scale_resolution" ) ) {
            char * saveptr = NULL, * token;
            int w, h;
            // width
            if ( ( token = strtok_r(value, "x", &saveptr) ) ) {
                w = atoi( token );
                // height
                if ( ( token = strtok_r(NULL, "x", &saveptr) ) ) {
                    h = atoi( token );
                    video_config.scale_resolution = w * 10000 + h;
                }
            }
        } else
        if ( !strcmp( key, "video.scale_resolution_aspectratio" ) ) {
            video_config.scale_resolution_aspectratio = atoi( value );
        } else
        if ( !strcmp( key, "video.scale_resolution_orientation" ) ) {
            video_config.scale_resolution_orientation = atoi( value );
        }
    }
}

/* --------------------------------------------------------------------------- */
/* exports                                                                     */
/* --------------------------------------------------------------------------- */

#include "libvideo_exports.h"

/* --------------------------------------------------------------------------- */
