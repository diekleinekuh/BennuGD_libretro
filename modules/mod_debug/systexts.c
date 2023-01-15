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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "bgdcore.h"
#include "bgdrtm.h"

#include "dcb.h"

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

#define CHARWIDTH 6

/* --------------------------------------------------------------------------- */

static char sysfont[][8][16] =
{
    {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "XXXXX " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "      " }
    }, {
        { "XXXXX " },
        { "X     " },
        { "X     " },
        { "XXXX  " },
        { "X     " },
        { "X     " },
        { "XXXXX " },
        { "      " }
    }, {
        { "XXXXX " },
        { "X     " },
        { "X     " },
        { "XXXX  " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X     " },
        { "X  XX " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "XXXXX " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "XXXXX " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "XXXXX " },
        { "      " }
    }, {
        { "    X " },
        { "    X " },
        { "    X " },
        { "    X " },
        { "    X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "X   X " },
        { "X  X  " },
        { "X X   " },
        { "XX    " },
        { "X X   " },
        { "X  X  " },
        { "X   X " },
        { "      " }
    }, {
        { "X     " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "XXXXX " },
        { "      " }
    }, {
        { "X   X " },
        { "XX XX " },
        { "X X X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "XX  X " },
        { "XX  X " },
        { "X X X " },
        { "X X X " },
        { "X X X " },
        { "X  XX " },
        { "X  XX " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X X X " },
        { "XX  X " },
        { " XXX  " },
        { "      " }
    }, {
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "X X   " },
        { "X  X  " },
        { "X   X " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X     " },
        { " XXX  " },
        { "    X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "XXXXX " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "      " }
    }, {
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " X X  " },
        { " X X  " },
        { "  X   " },
        { "      " }
    }, {
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X X X " },
        { "X X X " },
        { " X X  " },
        { "      " }
    }, {
        { "X   X " },
        { "X   X " },
        { " X X  " },
        { "  X   " },
        { " X X  " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "X   X " },
        { "X   X " },
        { " X X  " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "      " }
    }, {
        { "XXXXX " },
        { "    X " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "X     " },
        { "XXXXX " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "  X   " },
        { " XX   " },
        { "X X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "XXXXX " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "    X " },
        { "  XX  " },
        { " X    " },
        { "X     " },
        { "XXXXX " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "    X " },
        { " XXX  " },
        { "    X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "    X " },
        { "X   X " },
        { "X   X " },
        { "XXXXX " },
        { "    X " },
        { "    X " },
        { "    X " },
        { "      " }
    }, {
        { "XXXXX " },
        { "X     " },
        { "XXXX  " },
        { "    X " },
        { "    X " },
        { "    X " },
        { "XXXX  " },
        { "      " }
    }, {
        { " XXX  " },
        { "X     " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "XXXXX " },
        { "    X " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "X     " },
        { "X     " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXXX " },
        { "    X " },
        { " XXX  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "  XX  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "  XX  " },
        { "      " },
        { "      " },
        { "  XX  " },
        { "      " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "  XX  " },
        { "      " },
        { "  XX  " },
        { " XX   " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { " XXXX " },
        { "      " },
        { " XXXX " },
        { "      " },
        { "      " }
    }, {
        { "XX    " },
        { "X   X " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "X   X " },
        { "   XX " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { " XXX  " },
        { "    X " },
        { " XXXX " },
        { "X   X " },
        { " XXXX " },
        { "      " }
    }, {
        { "X     " },
        { "X     " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { " XXX  " },
        { "X   X " },
        { "X     " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "    X " },
        { "    X " },
        { " XXXX " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXXX " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { " XXX  " },
        { "X   X " },
        { "XXXX  " },
        { "X     " },
        { " XXXX " },
        { "      " }
    }, {
        { " XXX  " },
        { "X   X " },
        { "X     " },
        { "XXX   " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { " XXXX " },
        { "X   X " },
        { "X   X " },
        { " XXXX " },
        { "    X " },
        { " XXX  " }
    }, {
        { "X     " },
        { "X     " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "      " },
        { "  X   " },
        { "      " },
        { " XX   " },
        { "  X   " },
        { "  X   " },
        { " XXX  " },
        { "      " }
    }, {
        { "      " },
        { "    X " },
        { "      " },
        { "  XXX " },
        { "    X " },
        { "    X " },
        { "X   X " },
        { " XXX  " }
    }, {
        { "X     " },
        { "X     " },
        { "X     " },
        { "X  X  " },
        { "XXX   " },
        { "X  X  " },
        { "X   X " },
        { "      " }
    }, {
        { " XX   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "XXXXX " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "XX X  " },
        { "X X X " },
        { "X X X " },
        { "X X X " },
        { "X X X " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "XXXX  " },
        { "X     " },
        { "X     " }
    }, {
        { "      " },
        { "      " },
        { " XXXX " },
        { "X   X " },
        { "X   X " },
        { " XXXX " },
        { "    X " },
        { "    X " }
    }, {
        { "      " },
        { "      " },
        { "X XX  " },
        { "XX  X " },
        { "X     " },
        { "X     " },
        { "X     " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { " XXX  " },
        { "X     " },
        { " XXX  " },
        { "    X " },
        { "XXXX  " },
        { "      " }
    }, {
        { " X    " },
        { " X    " },
        { "XXXX  " },
        { " X    " },
        { " X    " },
        { " X  X " },
        { "  XX  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " X X  " },
        { "  X   " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "X   X " },
        { "X   X " },
        { "X X X " },
        { "X X X " },
        { " X X  " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "X   X " },
        { " X X  " },
        { "  X   " },
        { " X X  " },
        { "X   X " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXXX " },
        { "    X " },
        { "XXXX  " }
    }, {
        { "      " },
        { "      " },
        { "XXXXX " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "XXXXX " },
        { "      " }
    }, {
        { " XXX  " },
        { " X    " },
        { " X    " },
        { " X    " },
        { " X    " },
        { " X    " },
        { " XXX  " },
        { "      " }
    }, {
        { " XXX  " },
        { "   X  " },
        { "   X  " },
        { "   X  " },
        { "   X  " },
        { "   X  " },
        { " XXX  " },
        { "      " }
    }, {
        { "  XX  " },
        { " X    " },
        { " X    " },
        { " X    " },
        { " X    " },
        { " X    " },
        { "  XX  " },
        { "      " }
    }, {
        { " XX   " },
        { "   X  " },
        { "   X  " },
        { "   X  " },
        { "   X  " },
        { "   X  " },
        { " XX   " },
        { "      " }
    }, {
        { "  XX  " },
        { " X    " },
        { " X    " },
        { "X     " },
        { " X    " },
        { " X    " },
        { "  XX  " },
        { "      " }
    }, {
        { " XX   " },
        { "   X  " },
        { "   X  " },
        { "    X " },
        { "   X  " },
        { "   X  " },
        { " XX   " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "XXXXX " },
        { "      " },
        { "      " },
        { "      " }
    }, {
        { " X    " },
        { "  X   " },
        { "   X  " },
        { "    X " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "      " }
    }, {
        { "    X " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "  X   " },
        { "   X  " },
        { "    X " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "XXXXX " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "   X  " },
        { " XX   " }
    }, {
        { "      " },
        { "X     " },
        { " X    " },
        { "  X   " },
        { "   X  " },
        { "    X " },
        { "     X" },
        { "      " }
    }, {
        { "      " },
        { "     X" },
        { "    X " },
        { "   X  " },
        { "  X   " },
        { " X    " },
        { "X     " },
        { "      " }
    }, {
        { "      " },
        { "      " },
        { "  X   " },
        { "  X   " },
        { "XXXXX " },
        { "  X   " },
        { "  X   " },
        { "      " }
    }, {
        { "      " },
        { " X X  " },
        { "  X   " },
        { "XXXXX " },
        { "  X   " },
        { " X X  " },
        { "      " },
        { "      " }
    }, {
        { "      " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "      " },
        { "  X   " },
        { "      " }
    }, {
        { "      " },
        { "  X   " },
        { "      " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "      " }
    }, {
        { "      " },
        { " XXX  " },
        { "X   X " },
        { "   X  " },
        { "  X   " },
        { "      " },
        { "  X   " },
        { "      " }
    }, {
        { "      " },
        { "  X   " },
        { "      " },
        { "  X   " },
        { " X    " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { " X X  " },
        { " X X  " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "      " }
    }, {
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  XXXX" },
        { "  X   " },
        { "  X   " },
        { "  X   " }
    }, {
        { "      " },
        { "      " },
        { "      " },
        { "      " },
        { "XXXXXX" },
        { "      " },
        { "      " },
        { "      " }
    }, {
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "  X   " }
    }, {
        { "   X  " },
        { "  X   " },
        { " XXX  " },
        { "X   X " },
        { "XXXXX " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { "XXXXX " },
        { "X     " },
        { "XXXX  " },
        { "X     " },
        { "XXXXX " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { "XXXXX " },
        { "  X   " },
        { "  X   " },
        { "  X   " },
        { "XXXXX " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { " XXX  " },
        { "      " },
        { "XX  X " },
        { "XX  X " },
        { "X X X " },
        { "X  XX " },
        { "X  XX " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { " XXX  " },
        { "    X " },
        { " XXXX " },
        { "X   X " },
        { " XXXX " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { " XXX  " },
        { "X   X " },
        { "XXXX  " },
        { "X     " },
        { " XXXX " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { "      " },
        { " XX   " },
        { "  X   " },
        { "  X   " },
        { " XXX  " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { " XXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "   X  " },
        { "  X   " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { " XXX  " },
        { "      " }
    }, {
        { "XXXX  " },
        { "      " },
        { "XXXX  " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "X   X " },
        { "      " }
    }
};

/* --------------------------------------------------------------------------- */

void systext_color( int cfg, int cbg );
void systext_puts( GRAPH * map, int x, int y, char * str, int len );

/* --------------------------------------------------------------------------- */

//static uint8_t * letters = ( uint8_t * ) " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:;=%abcdefghijklmnopqrstuvwxyz[](){}-><_,\\/+*!¡?¿\"'\x01\x02\x03ÁÉÍÓÚÑáéíóúñ" ;
static uint8_t letters[] = { 
    0x20,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x30,0x31,0x32,0x33,0x34,
    0x35,0x36,0x37,0x38,0x39,0x2e,0x3a,0x3b,0x3d,0x25,0x61,0x62,0x63,0x64,0x65,0x66,
    0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
    0x77,0x78,0x79,0x7a,0x5b,0x5d,0x28,0x29,0x7b,0x7d,0x2d,0x3e,0x3c,0x5f,0x2c,0x5c,
    0x5c,0x2f,0x2b,0x2a,0x21,0xa1,0x3f,0xbf,0x5c,0x22,0x27,0x5c,0x78,0x30,0x31,0x5c,
    0x78,0x30,0x32,0x5c,0x78,0x30,0x33,0xc1,0xc9,0xcd,0xd3,0xda,0xd1,0xe1,0xe9,0xed,
    0xf3,0xfa,0xf1,0x0 } ;

static int fg = 0, bg = 0;

/* --------------------------------------------------------------------------- */

void systext_putchar( GRAPH * map, int ox, int oy, uint8_t c )
{
    int x, y ;
    static int corr[256] ;
    static int corr_init = 0 ;

    if ( !corr_init )
    {
        uint8_t * ptr ;

        for ( ptr = letters; *ptr; ptr++ )
            corr[*ptr] = corr_init++ ;
    }

#define PUTSYS(TYPE)                                            \
    for (y = oy ; y < oy+8 ; y++)                               \
    {                                                           \
        TYPE * ptr; uint8_t * cptr ;                            \
        if (y < 0 || y >= (int)map->height) continue ;          \
        ptr = (TYPE *)((uint8_t*)map->data + map->pitch*y) ;    \
        ptr += ox;                                              \
        cptr = (uint8_t*)sysfont[c][y-oy];                      \
        for (x = ox ; x < ox+6 ; x++, cptr++)                   \
        {                                                       \
            if (x < 0 || x >= (int)map->width)                  \
            {                                                   \
                ptr++ ;                                         \
                continue ;                                      \
            }                                                   \
            if (*cptr == 'X')                                   \
                *ptr++ = fg ;                                   \
            else                                                \
                if (bg)                                         \
                    *ptr++ = bg ;                               \
                else                                            \
                    ptr++ ;                                     \
        }                                                       \
    }

    switch ( map->format->depth )
    {
        case 8 :
        {
            c = corr[c] ;
            PUTSYS( uint8_t )
        }
        break;
        case 16 :
        {
            c = corr[c] ;
            PUTSYS( uint16_t )
        }
        break;
        case 32 :
        {
            c = corr[c] ;
            PUTSYS( uint32_t )
        }
    }

#undef PUTSYS
}

/* --------------------------------------------------------------------------- */

static int text_colors[] =
{
    0x000000, 0x0000C0, 0xC00000, 0xC000C0, 0x00C000, 0x00C0C0, 0xC0C000, 0xC0C0C0,
    0x808080, 0x0000FF, 0xFF0000, 0xFF00FF, 0x00FF00, 0x00FFFF, 0xFFFF00, 0xFFFFFF
} ;

/* --------------------------------------------------------------------------- */

void systext_puts( GRAPH * map, int x, int y, char * str, int len )
{
    while ( *str && len )
    {
        if ( *str == '\xac' )
        {
            uint8_t color = 0 ;
            str++ ;
            if ( isdigit( *str ) ) color = *str++ - '0' ;
            if ( isdigit( *str ) ) color = color * 10 + *str++ - '0' ;

            if ( color < 32 )
            {
                if ( color > 15 )
                    systext_color( -1, text_colors[color - 16] ) ;
                else
                    systext_color( text_colors[color], -1 ) ;
            }
            if ( !*str ) break ;
            continue;
        }
        systext_putchar( map, x, y, *str++ ) ;
        x += CHARWIDTH ;
        len--;
    }

    while ( len-- > 0 )
    {
        systext_putchar( map, x, y, ' ' ) ;
        x += CHARWIDTH ;
    }
}

/* --------------------------------------------------------------------------- */

void systext_color( int cfg, int cbg )
{
    if ( !cbg ) bg = 0;
    if ( sys_pixel_format->depth == 8 )
    {
        if ( !trans_table_updated ) gr_make_trans_table() ;

        fg = gr_find_nearest_color((( cfg & 0xFF0000 ) >> 16 ), (( cfg & 0x00FF00 ) >>  8 ), ( cfg & 0x0000FF ) ) ;
        if ( cbg > 0 ) bg = gr_find_nearest_color((( cbg & 0xFF0000 ) >> 16 ), (( cbg & 0x00FF00 ) >>  8 ), ( cbg & 0x0000FF ) ) ;
    }
    else
    {
        fg = gr_rgb((( cfg & 0xFF0000 ) >> 16 ), (( cfg & 0x00FF00 ) >>  8 ), ( cfg & 0x0000FF ) ) ;
        if ( cbg > 0 ) bg = gr_rgb((( cbg & 0xFF0000 ) >> 16 ), (( cbg & 0x00FF00 ) >>  8 ), ( cbg & 0x0000FF ) ) ;
    }
}

/* --------------------------------------------------------------------------- */
