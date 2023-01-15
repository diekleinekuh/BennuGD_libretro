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

#ifndef __LIBKEY_H
    #define __LIBKEY_H

    /* ---------------------------------------------------------------------- */

    #define STAT_RSHIFT             0x0000001
    #define STAT_LSHIFT             0x0000002
    #define STAT_CTRL               0x0000004
    #define STAT_ALT                0x0000008
    #define STAT_RCTRL              0x0000010
    #define STAT_LCTRL              0x0000020
    #define STAT_RALT               0x0000040
    #define STAT_LALT               0x0000080
    #define STAT_NUM                0x0000100
    #define STAT_CAPS               0x0000200
    #define STAT_SHIFT              0x0000400

    /* ---------------------------------------------------------------------- */

    #ifndef __BGDC__

    #include <SDL.h>

    typedef struct _keyequiv {
        int                 sdlk_equiv ;
        struct _keyequiv    * next ;
    } key_equiv ;

    typedef int (* HOTKEY_CALLBACK) (SDL_keysym);

    #ifndef __LIBKEY
    extern DLLIMPORT void hotkey_add(int mod, int sym, HOTKEY_CALLBACK callback);

    extern DLLIMPORT key_equiv key_table[] ;           /* Now we have a search table with equivs */
    extern DLLIMPORT unsigned char * keystate ;        /* Pointer to key states */
    #endif

    #endif

#endif
