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

#include <SDL.h>

#include "xctype.h"
#include "bgddl.h"
#include "dlvaracc.h"

#define __LIBKEY
#include "libkey.h"

/* ---------------------------------------------------------------------- */

#include "libkey_exports.h"

/* ---------------------------------------------------------------------- */

typedef struct
{
    SDLKey sym;   /* SDL virtual keysym */
    SDLMod mod;   /* current key modifiers */
    HOTKEY_CALLBACK callback;
} hotkey ;

static hotkey * hotkey_list = NULL ;
static int hotkey_allocated = 0 ;
static int hotkey_count = 0 ;

/* ---------------------------------------------------------------------- */

/* Publicas */
key_equiv key_table[127] ;              /* Now we have a search table with equivs */
unsigned char * keystate = NULL;        /* Pointer to key states */

/* ---------------------------------------------------------------------- */

static int sdl_equiv[SDLK_LAST+1] ;

static int equivs[] =
{
    SDLK_ESCAPE,        1,
    SDLK_1,             2,
    SDLK_KP1,           2,
    SDLK_2,             3,
    SDLK_KP2,           3,
    SDLK_3,             4,
    SDLK_KP3,           4,
    SDLK_4,             5,
    SDLK_KP4,           5,
    SDLK_5,             6,
    SDLK_KP5,           6,
    SDLK_6,             7,
    SDLK_KP6,           7,
    SDLK_7,             8,
    SDLK_KP7,           8,
    SDLK_8,             9,
    SDLK_KP8,           9,
    SDLK_9,             10,
    SDLK_KP9,           10,
    SDLK_0,             11,
    SDLK_KP0,           11,
    SDLK_MINUS,         12,
    SDLK_EQUALS,        13,
    SDLK_BACKSPACE,     14,
    SDLK_TAB,           15,
    SDLK_q,             16,
    SDLK_w,             17,
    SDLK_e,             18,
    SDLK_r,             19,
    SDLK_t,             20,
    SDLK_y,             21,
    SDLK_u,             22,
    SDLK_i,             23,
    SDLK_o,             24,
    SDLK_p,             25,
    SDLK_LEFTBRACKET,   26,
    SDLK_RIGHTBRACKET,  27,
    SDLK_RETURN,        28,
    SDLK_KP_ENTER,      28,
    SDLK_LCTRL,         29,
    SDLK_RCTRL,         29,
    SDLK_LCTRL,         96,
    SDLK_RCTRL,         94,
    SDLK_a,             30,
    SDLK_s,             31,
    SDLK_d,             32,
    SDLK_f,             33,
    SDLK_g,             34,
    SDLK_h,             35,
    SDLK_j,             36,
    SDLK_k,             37,
    SDLK_l,             38,
    SDLK_SEMICOLON,     39,
    SDLK_QUOTE,         40,
    SDLK_BACKQUOTE,     41,
    SDLK_LSHIFT,        42,
    SDLK_BACKSLASH,     43,
    SDLK_z,             44,
    SDLK_x,             45,
    SDLK_c,             46,
    SDLK_v,             47,
    SDLK_b,             48,
    SDLK_n,             49,
    SDLK_m,             50,
    SDLK_COMMA,         51,
    SDLK_PERIOD,        52,
    SDLK_KP_PERIOD,     52,
    SDLK_SLASH,         53,
    SDLK_KP_DIVIDE,     53,
    SDLK_RSHIFT,        54,
    /*  SDLK_PRINT,         55,*/
    SDLK_KP_MULTIPLY,   55,
    SDLK_LALT,          56,
    SDLK_RALT,          56,
    SDLK_LALT,          95,
    SDLK_RALT,          93,
    SDLK_MODE,          93,
    SDLK_MODE,          56,
    SDLK_RMETA,         56,
    SDLK_LMETA,         56,
    SDLK_SPACE,         57,
    SDLK_CAPSLOCK,      58,
    SDLK_F1,            59,
    SDLK_F2,            60,
    SDLK_F3,            61,
    SDLK_F4,            62,
    SDLK_F5,            63,
    SDLK_F6,            64,
    SDLK_F7,            65,
    SDLK_F8,            66,
    SDLK_F9,            67,
    SDLK_F10,           68,
    SDLK_NUMLOCK,       69,
    SDLK_SCROLLOCK,     70,
    SDLK_HOME,          71,
    SDLK_UP,            72,
    SDLK_PAGEUP,        73,
    SDLK_KP_MINUS,      74,
    SDLK_LEFT,          75,
    SDLK_RIGHT,         77,
    SDLK_KP_PLUS,       78,
    SDLK_END,           79,
    SDLK_DOWN,          80,
    SDLK_PAGEDOWN,      81,
    SDLK_INSERT,        82,
    SDLK_DELETE,        83,
    SDLK_F11,           87,
    SDLK_F12,           88,
    SDLK_LESS,          89,
    SDLK_PLUS,          90,
    SDLK_GREATER,       91,
    SDLK_QUESTION,      92,
    SDLK_CARET,         93,
    SDLK_SYSREQ,        55,
    SDLK_PAUSE,         95,
    SDLK_MENU,          97,
    SDLK_LSUPER,        98,
    SDLK_RSUPER,        99,
    SDLK_COMPOSE,       99,
    -1, -1
} ;

/* ---------------------------------------------------------------------- */

enum {
    SHIFTSTATUS = 0,
    ASCII,
    SCANCODE
};

/* ---------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP  __bgdexport( libkey, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "shift_status" , NULL, -1, -1 },
    { "ascii"        , NULL, -1, -1 },
    { "scan_code"    , NULL, -1, -1 },
    { NULL           , NULL, -1, -1 }
};

/* ---------------------------------------------------------------------- */

static void add_key_equiv( int equiv, int keyconst )
{
    key_equiv * curr = &key_table[keyconst] ;

    if ( curr->next != NULL ) while ( curr->next != NULL ) curr = curr->next ;

    if ( curr->sdlk_equiv != 0 )
    {
        curr->next = malloc( sizeof( key_equiv ) ) ;
        curr = curr->next ;
        curr->next = NULL ;
    }

    curr->sdlk_equiv = equiv ;
}

/* ---------------------------------------------------------------------- */

void hotkey_add( int mod, int sym, HOTKEY_CALLBACK callback )
{
    if ( hotkey_count >= hotkey_allocated )
    {
        hotkey_allocated = hotkey_count + 5;
        hotkey_list = realloc( hotkey_list, hotkey_allocated * sizeof( hotkey_list[0] ) );
    }

    if ( !hotkey_list )
    {
        fprintf( stderr, "No memory for alloc hotkey\n" );
        exit( -1 );
    }

    hotkey_list [hotkey_count].mod = mod;
    hotkey_list [hotkey_count].sym = sym;
    hotkey_list [hotkey_count].callback = callback;
    hotkey_count++;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : process_key_events
 *
 *  Process all pending SDL events, updating all global variables
 *  and handling debug key presses and cannot be changed
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void process_key_events()
{
    SDL_Event e ;
    SDLMod m ;
    int k, asc ;
    int pressed ;
    key_equiv * curr ;
    int keypress ;
    static struct
    {
        int ascii ;
        int scancode ;
    }
    keyring [64] ;
    static int keyring_start = 0, keyring_tail = 0 ;
    int ignore_key, n;

    /* Actualizar eventos */

    keypress = 0 ;
    m = SDL_GetModState() ;

    /* Procesa los eventos pendientes */
    /* Reset ascii and scancode if last key was released... */
    /* must check all the linked equivs */

    pressed = 0 ;
    if ( GLODWORD( libkey,  SCANCODE ) )
    {
        curr = &key_table[GLODWORD( libkey,  SCANCODE )] ;
        while ( curr != NULL && pressed == 0 )
        {
            if ( keystate[curr->sdlk_equiv] ) pressed = 1 ;
            curr = curr->next ;
        }
    }

    if ( !pressed )
    {
        GLODWORD( libkey,  ASCII )     = 0 ;
        GLODWORD( libkey,  SCANCODE )  = 0 ;
    }

    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN)|SDL_EVENTMASK(SDL_KEYUP) ) > 0 )
    {
        switch ( e.type )
        {
            case SDL_KEYDOWN:
                ignore_key = 0;
                /* KeyDown HotKey */
                if ( hotkey_count )
                    for ( n = 0; n < hotkey_count; n++ )
                    {
                        if ((( hotkey_list[n].mod & e.key.keysym.mod ) == hotkey_list[n].mod ) &&
                                ( !hotkey_list[n].sym || ( hotkey_list[n].sym == e.key.keysym.sym ) ) )
                        {
                            ignore_key = hotkey_list[n].callback( e.key.keysym );
                        }
                    }
                /* KeyDown HotKey */

                if ( ignore_key ) break ;

                /* Almacena la pulsación de la tecla */

                k = sdl_equiv[e.key.keysym.sym];

                m = e.key.keysym.mod ;

                if ( !keypress )
                {
                    GLODWORD( libkey,  SCANCODE )  = k ;
                    if ( e.key.keysym.unicode )
                    {
                        asc = win_to_dos[e.key.keysym.unicode & 0xFF] ;

                        /* ascii mayusculas */
                        if ( asc >= 'a' && asc <= 'z' && ( m & KMOD_LSHIFT || m & KMOD_RSHIFT || keystate[SDLK_CAPSLOCK] ) )
                            asc -= 0x20 ;
                    }
                    else
                    {
                        asc = 0 ; /* NON PRINTABLE */
                    }

                    GLODWORD( libkey,  ASCII ) = asc ;
                    keypress = 1 ;
                }
                else
                {
                    keyring[keyring_tail].scancode = k ;
                    if ( e.key.keysym.unicode )
                    {
                        asc = win_to_dos[e.key.keysym.unicode & 0x7F] ;

                        /*ascii mayusculas */
                        if ( asc >= 'a' && asc <= 'z' && ( m & KMOD_LSHIFT || m & KMOD_RSHIFT || keystate[SDLK_CAPSLOCK] ) )
                            asc -= 0x20 ;
                    }
                    else
                    {
                        asc = 0 ; /* NON PRINTABLE */
                    }
                    keyring[keyring_tail].ascii = asc ;
                    if ( ++keyring_tail == 64 ) keyring_tail = 0 ;
                }

                break ;

            case SDL_KEYUP:
                /* Do nothing, Bennu is key_up unsensitive */
                break ;
        }
    }

    if ( !keypress && keyring_start != keyring_tail )
    {
        GLODWORD( libkey,  ASCII )     = keyring[keyring_start].ascii ;
        GLODWORD( libkey,  SCANCODE )  = keyring[keyring_start].scancode ;
        if ( ++keyring_start == 64 ) keyring_start = 0 ;
    }

    /* Now actualized every frame... */
    GLODWORD( libkey,  SHIFTSTATUS ) =
        ( ( m & KMOD_RSHIFT ) ? STAT_RSHIFT : 0 ) |
        ( ( m & KMOD_LSHIFT ) ? STAT_LSHIFT : 0 ) |

        ( ( m & KMOD_CTRL   ) ? STAT_CTRL   : 0 ) |
        ( ( m & KMOD_ALT    ) ? STAT_ALT    : 0 ) |

        ( ( m & KMOD_RCTRL  ) ? STAT_RCTRL  : 0 ) |
        ( ( m & KMOD_LCTRL  ) ? STAT_LCTRL  : 0 ) |

        ( ( m & KMOD_RALT   ) ? STAT_RALT   : 0 ) |
        ( ( m & KMOD_LALT   ) ? STAT_LALT   : 0 ) |

        ( ( m & KMOD_NUM    ) ? STAT_NUM    : 0 ) |
        ( ( m & KMOD_CAPS   ) ? STAT_CAPS   : 0 ) |
        ( ( m & KMOD_SHIFT  ) ? STAT_SHIFT  : 0 ) ;
}

/* ---------------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libkey, handler_hooks )[] =
{
    { 4900, process_key_events },
    {    0, NULL               }
} ;

/* ---------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                     */

void __bgdexport( libkey, module_initialize )()
{
    int * ptr = equivs ;

    if ( !SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_InitSubSystem( SDL_INIT_VIDEO );

    memset( sdl_equiv, 0, sizeof( sdl_equiv ) );
    memset( key_table, 0, sizeof( key_table ) ) ;

    while ( *ptr != -1 )
    {
        sdl_equiv[*ptr] = ptr[1] ;
        add_key_equiv( ptr[0], ptr[1] ) ;
        ptr += 2 ;
    }

    if ( !keystate ) keystate = SDL_GetKeyState( NULL );

    SDL_EnableUNICODE( 1 );
}

/* ---------------------------------------------------------------------- */

void __bgdexport( libkey, module_finalize )()
{
    /* FREE used key_equivs... note base 127 equivs are static not allocated... */
    int i ;
    key_equiv * aux ;
    key_equiv * curr = key_table ;

    for ( i = 0;i < 127;i++ )
    {
        if ( curr->next != NULL )
        {
            curr = curr->next ;
            while ( curr->next != NULL )
            {
                aux = curr ;
                curr = curr->next;
                free( aux ) ;
            }
            free( curr ) ;
        }
    }

    if ( SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

/* ---------------------------------------------------------------------- */
