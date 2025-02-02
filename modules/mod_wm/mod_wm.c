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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"
#include "bgdrtm.h"
#include "xstrings.h"

#include "libgrbase.h"
#include "libvideo.h"

#include <SDL.h>

#if defined( WIN32 ) || ( __linux && ( defined( SDL_VIDEO_DRIVER_X11 ) ) )
#include <SDL_syswm.h>
#endif

/* --------------------------------------------------------------------------- */
/* Window Manager                                                              */
/* --------------------------------------------------------------------------- */

static int bgd_set_title( INSTANCE * my, int * params )
{
    gr_set_caption( ( char * )string_get( params[0] ) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_set_icon( INSTANCE * my, int * params )
{
    gr_set_icon( bitmap_get( params[0], params[1] ) );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_minimize( INSTANCE * my, int * params )
{
    return SDL_WM_IconifyWindow();
}

/* --------------------------------------------------------------------------- */

static int bgd_move_window( INSTANCE * my, int * params )
{
    int res = 0;
    if ( full_screen ) return 0;

#if !LIBRETRO_CORE

#if defined( WIN32 ) || ( __linux && ( defined( SDL_VIDEO_DRIVER_X11 ) ) )
    SDL_SysWMinfo wminfo ;

    SDL_VERSION( &wminfo.version );
    if ( SDL_GetWMInfo( &wminfo ) != 1 ) return 0 ;
#endif

#ifdef WIN32
    /* Set the new window position */
    res = SetWindowPos(
                wminfo.window,     // handle to window
                HWND_NOTOPMOST,    // Possibly will have no effect... it stays below top-most
                params[0],          // horizontal position
                params[1],          // vertical position
                0,                  // keep the old window width
                0,                  // keep the old window height
                SWP_SHOWWINDOW | SWP_NOSIZE // Make it visible and retain size
            ) ;
#elif __linux
#ifdef SDL_VIDEO_DRIVER_X11
    Window root, parent, *children = NULL;
    unsigned int children_count;

    if ( wminfo.subsystem == SDL_SYSWM_X11 )
    {
        if ( XQueryTree( wminfo.info.x11.display, wminfo.info.x11.window, &root, &parent, &children, &children_count ) != BadWindow )
        {
            wminfo.info.x11.lock_func();
            res = XMoveWindow( wminfo.info.x11.display, parent, params[0], params[1] );
            XMapRaised(wminfo.info.x11.display, parent); /* Show Window */
            wminfo.info.x11.unlock_func();
            if ( children ) XFree( children );
        }
    }
#endif
#endif
#endif // !LIBRETRO_CORE
    // Missing BeOS & MAC support
    return res ;
}

/* --------------------------------------------------------------------------- */

static int bgd_get_window_pos( INSTANCE * my, int * params )
{
    if ( full_screen ) return -1;

#if !LIBRETRO_CORE

#if defined( WIN32 ) || ( __linux && ( defined( SDL_VIDEO_DRIVER_X11 ) ) )
    SDL_SysWMinfo wminfo ;

    SDL_VERSION( &wminfo.version );
    if ( SDL_GetWMInfo( &wminfo ) != 1 ) return -1 ;
#endif

#ifdef WIN32
    RECT Rect;

    if ( GetWindowRect( wminfo.window, &Rect ) )
    {
        if ( params[0] ) *(( int * )ptr_from_int( params[0] ) ) = Rect.left;
        if ( params[1] ) *(( int * )ptr_from_int( params[1] ) ) = Rect.top;
    }
#elif __linux
#ifdef SDL_VIDEO_DRIVER_X11
    Window root, parent, *children = NULL;
    unsigned int children_count;
    XWindowAttributes wattr;
    int res ;

    wminfo.info.x11.lock_func();
    if ( XQueryTree( wminfo.info.x11.display, wminfo.info.x11.window, &root, &parent, &children, &children_count ) != BadWindow )
    {
        if ( children ) XFree( children );

        if ( XQueryTree( wminfo.info.x11.display, parent, &root, &parent, &children, &children_count ) != BadWindow )
        {
            if ( children ) XFree( children );

            res = XGetWindowAttributes( wminfo.info.x11.display, parent, &wattr );
            if ( res != BadDrawable && res != BadWindow )
            {
                if ( params[0] ) *(( int * )ptr_from_int( params[0] ) ) = wattr.x;
                if ( params[1] ) *(( int * )ptr_from_int( params[1] ) ) = wattr.y;
            }
        }
    }
    wminfo.info.x11.unlock_func();
#endif
#endif
#endif // !LIBRETRO_CORE
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_get_window_size( INSTANCE * my, int * params )
{

#if !LIBRETRO_CORE

#if defined( WIN32 ) || ( __linux && ( defined( SDL_VIDEO_DRIVER_X11 ) ) )
    SDL_SysWMinfo wminfo ;

    SDL_VERSION( &wminfo.version );
    if ( SDL_GetWMInfo( &wminfo ) != 1 ) return -1 ;
#endif

#ifdef WIN32
    RECT Rect;

    if ( GetWindowRect( wminfo.window, &Rect ) )
    {
        if ( params[0] ) *(( int * )ptr_from_int( params[0] ) ) = Rect.right - Rect.left;
        if ( params[1] ) *(( int * )ptr_from_int( params[1] ) ) = Rect.bottom - Rect.top;

        if ( GetClientRect( wminfo.window, &Rect ) )
        {
            if ( params[2] ) *(( int * )ptr_from_int( params[2] ) ) = Rect.right - Rect.left;
            if ( params[3] ) *(( int * )ptr_from_int( params[3] ) ) = Rect.bottom - Rect.top;
        }
    }
#elif __linux
#ifdef SDL_VIDEO_DRIVER_X11
    int res ;
    XWindowAttributes wattr;

    Window root, parent, *children = NULL;
    unsigned int children_count;

    wminfo.info.x11.lock_func();
    if ( XQueryTree( wminfo.info.x11.display, wminfo.info.x11.window, &root, &parent, &children, &children_count ) != BadWindow )
    {
        if ( children ) XFree( children );

        res = XGetWindowAttributes( wminfo.info.x11.display, parent, &wattr );
        if ( res != BadDrawable && res != BadWindow )
        {
            if ( params[2] ) *(( int * )ptr_from_int( params[2] ) ) = wattr.width;
            if ( params[3] ) *(( int * )ptr_from_int( params[3] ) ) = wattr.height;

            if ( XQueryTree( wminfo.info.x11.display, parent, &root, &parent, &children, &children_count ) != BadWindow )
            {
                if ( children ) XFree( children );

                res = XGetWindowAttributes( wminfo.info.x11.display, parent, &wattr );
                if ( res != BadDrawable && res != BadWindow )
                {
                    if ( params[0] ) *(( int * )ptr_from_int( params[0] ) ) = wattr.width;
                    if ( params[1] ) *(( int * )ptr_from_int( params[1] ) ) = wattr.height;
                }
            }
        }
    }
    wminfo.info.x11.unlock_func();
#endif
#endif
#endif // #if !LIBRETRO_CORE
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_get_desktop_size( INSTANCE * my, int * params )
{
#if !LIBRETRO_CORE

#ifdef WIN32
    RECT Rect;

    if ( GetClientRect( GetDesktopWindow(), &Rect ) )
    {
        *(( int * )ptr_from_int( params[0] ) ) = Rect.right - Rect.left;
        *(( int * )ptr_from_int( params[1] ) ) = Rect.bottom - Rect.top;
    }
#elif __linux
#ifdef SDL_VIDEO_DRIVER_X11
    int res ;
    Window root, parent, *children = NULL;
    XWindowAttributes wattr;
    unsigned int children_count;

    SDL_SysWMinfo wminfo ;

    SDL_VERSION( &wminfo.version );
    if ( SDL_GetWMInfo( &wminfo ) != 1 ) return -1 ;

    wminfo.info.x11.lock_func();
    if ( XQueryTree(wminfo.info.x11.display, wminfo.info.x11.window, &root, &parent, &children, &children_count ) != BadWindow )
    {
        if ( children ) XFree( children );

        res = XGetWindowAttributes( wminfo.info.x11.display, root, &wattr );
        if ( res != BadDrawable && res != BadWindow )
        {
            if ( params[0] ) *(( int * )ptr_from_int( params[0] ) ) = wattr.width;
            if ( params[1] ) *(( int * )ptr_from_int( params[1] ) ) = wattr.height;
        }
    }
    wminfo.info.x11.unlock_func();
#endif
#endif
#endif // #if !LIBRETRO_CORE
    return 1 ;
}

/* --------------------------------------------------------------------------- */
/* exports                                                                     */
/* --------------------------------------------------------------------------- */

#include "mod_wm_exports.h"

/* --------------------------------------------------------------------------- */
