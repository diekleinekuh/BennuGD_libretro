/*
 *  Copyright © 2003 MaxSt (maxst@hiend3d.com)
 *
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
#include <string.h>

/* --------------------------------------------------------------------------- */

#include "librender.h"

/* --------------------------------------------------------------------------- */

#define PIXEL00_0 *(q) = w5;
#define PIXEL00_10 *(q) = interpolate16_2(3,1,w5, w1);
#define PIXEL00_11 *(q) = interpolate16_2(3,1,w5, w4);
#define PIXEL00_12 *(q) = interpolate16_2(3,1,w5, w2);
#define PIXEL00_20 *(q) = interpolate16_3(2,1,1,w5, w4, w2);
#define PIXEL00_21 *(q) = interpolate16_3(2,1,1,w5, w1, w2);
#define PIXEL00_22 *(q) = interpolate16_3(2,1,1,w5, w1, w4);
#define PIXEL00_60 *(q) = interpolate16_3(5,2,1,w5, w2, w4);
#define PIXEL00_61 *(q) = interpolate16_3(5,2,1,w5, w4, w2);
#define PIXEL00_70 *(q) = interpolate16_3(6,1,1,w5, w4, w2);
#define PIXEL00_90 *(q) = interpolate16_3(2,3,3,w5, w4, w2);
#define PIXEL00_100 *(q) = interpolate16_3(14,1,1,w5, w4, w2);

#define PIXEL01_0 *(q+1) = w5;
#define PIXEL01_10 *(q+1) = interpolate16_2(3,1,w5, w3);
#define PIXEL01_11 *(q+1) = interpolate16_2(3,1,w5, w2);
#define PIXEL01_12 *(q+1) = interpolate16_2(3,1,w5, w6);
#define PIXEL01_20 *(q+1) = interpolate16_3(2,1,1,w5, w2, w6);
#define PIXEL01_21 *(q+1) = interpolate16_3(2,1,1,w5, w3, w6);
#define PIXEL01_22 *(q+1) = interpolate16_3(2,1,1,w5, w3, w2);
#define PIXEL01_60 *(q+1) = interpolate16_3(5,2,1,w5, w6, w2);
#define PIXEL01_61 *(q+1) = interpolate16_3(5,2,1,w5, w2, w6);
#define PIXEL01_70 *(q+1) = interpolate16_3(6,1,1,w5, w2, w6);
#define PIXEL01_90 *(q+1) = interpolate16_3(2,3,3,w5, w2, w6);
#define PIXEL01_100 *(q+1) = interpolate16_3(14,1,1,w5, w2, w6);

#define PIXEL10_0 *(q+nextlineDst) = w5;
#define PIXEL10_10 *(q+nextlineDst) = interpolate16_2(3,1,w5, w7);
#define PIXEL10_11 *(q+nextlineDst) = interpolate16_2(3,1,w5, w8);
#define PIXEL10_12 *(q+nextlineDst) = interpolate16_2(3,1,w5, w4);
#define PIXEL10_20 *(q+nextlineDst) = interpolate16_3(2,1,1,w5, w8, w4);
#define PIXEL10_21 *(q+nextlineDst) = interpolate16_3(2,1,1,w5, w7, w4);
#define PIXEL10_22 *(q+nextlineDst) = interpolate16_3(2,1,1,w5, w7, w8);
#define PIXEL10_60 *(q+nextlineDst) = interpolate16_3(5,2,1,w5, w4, w8);
#define PIXEL10_61 *(q+nextlineDst) = interpolate16_3(5,2,1,w5, w8, w4);
#define PIXEL10_70 *(q+nextlineDst) = interpolate16_3(6,1,1,w5, w8, w4);
#define PIXEL10_90 *(q+nextlineDst) = interpolate16_3(2,3,3,w5, w8, w4);
#define PIXEL10_100 *(q+nextlineDst) = interpolate16_3(14,1,1,w5, w8, w4);

#define PIXEL11_0 *(q+1+nextlineDst) = w5;
#define PIXEL11_10 *(q+1+nextlineDst) = interpolate16_2(3,1,w5, w9);
#define PIXEL11_11 *(q+1+nextlineDst) = interpolate16_2(3,1,w5, w6);
#define PIXEL11_12 *(q+1+nextlineDst) = interpolate16_2(3,1,w5, w8);
#define PIXEL11_20 *(q+1+nextlineDst) = interpolate16_3(2,1,1,w5, w6, w8);
#define PIXEL11_21 *(q+1+nextlineDst) = interpolate16_3(2,1,1,w5, w9, w8);
#define PIXEL11_22 *(q+1+nextlineDst) = interpolate16_3(2,1,1,w5, w9, w6);
#define PIXEL11_60 *(q+1+nextlineDst) = interpolate16_3(5,2,1,w5, w8, w6);
#define PIXEL11_61 *(q+1+nextlineDst) = interpolate16_3(5,2,1,w5, w6, w8);
#define PIXEL11_70 *(q+1+nextlineDst) = interpolate16_3(6,1,1,w5, w6, w8);
#define PIXEL11_90 *(q+1+nextlineDst) = interpolate16_3(2,3,3,w5, w6, w8);
#define PIXEL11_100 *(q+1+nextlineDst) = interpolate16_3(14,1,1,w5, w6, w8);

#define YUV(x) RGBtoYUV[w ## x]

/* --------------------------------------------------------------------------- */

int hq2xinited = 0;

#define RGBtoYUV _RGBtoYUV
#define LUT16to32 _LUT16to32

static uint32_t   *LUT16to32;
static uint32_t   *RGBtoYUV;

/* --------------------------------------------------------------------------- */
/**
 * Interpolate two 16 bit pixels with the weights specified in the template
 * parameters. Used by the hq scaler family.
 * @note w1 and w2 must sum up to 2, 4, 8 or 16.
 */

static inline uint16_t interpolate16_2( int w1, int w2, uint16_t p1, uint16_t p2 )
{
    return (((( p1 & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) * w1 + ( p2 & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) * w2 ) / ( w1 + w2 ) ) & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) |
            (((( p1 & sys_pixel_format->Gmask ) * w1 + ( p2 & sys_pixel_format->Gmask ) * w2 ) / ( w1 + w2 ) ) & sys_pixel_format->Gmask );
}

/* --------------------------------------------------------------------------- */
/**
 * Interpolate three 16 bit pixels with the weights specified in the template
 * parameters. Used by the hq scaler family.
 * @note w1, w2 and w3 must sum up to 2, 4, 8 or 16.
 */

static inline uint16_t interpolate16_3( int w1, int w2, int w3, uint16_t p1, uint16_t p2, uint16_t p3 )
{
    return (((( p1 & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) * w1 + ( p2 & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) * w2 + ( p3 & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) * w3 ) / ( w1 + w2 + w3 ) ) & ( sys_pixel_format->Rmask | sys_pixel_format->Bmask ) ) |
            (((( p1 & sys_pixel_format->Gmask ) * w1 + ( p2 & sys_pixel_format->Gmask ) * w2 + ( p3 & sys_pixel_format->Gmask ) * w3 ) / ( w1 + w2 + w3 ) ) & sys_pixel_format->Gmask );
}

/* --------------------------------------------------------------------------- */
/**
 * Compare two YUV values (encoded 8-8-8) and check if they differ by more than
 * a certain hard coded threshold. Used by the hq scaler family.
 */
static inline int diffYUV( int yuv1, int yuv2 )
{
    static const int Ymask = 0x00FF0000;
    static const int Umask = 0x0000FF00;
    static const int Vmask = 0x000000FF;
    static const int trY   = 0x00300000;
    static const int trU   = 0x00000700;
    static const int trV   = 0x00000006;

    int diff;
    int mask;

    diff = (( yuv1 & Ymask ) - ( yuv2 & Ymask ) );
    mask = diff >> 31; // -1 if value < 0, 0 otherwise
    diff = ( diff ^ mask ) - mask; //-1: ~value + 1; 0: value
    if ( diff > trY ) return -1;

    diff = (( yuv1 & Umask ) - ( yuv2 & Umask ) );
    mask = diff >> 31; // -1 if value < 0, 0 otherwise
    diff = ( diff ^ mask ) - mask; //-1: ~value + 1; 0: value
    if ( diff > trU ) return -1;

    diff = (( yuv1 & Vmask ) - ( yuv2 & Vmask ) );
    mask = diff >> 31; // -1 if value < 0, 0 otherwise
    diff = ( diff ^ mask ) - mask; //-1: ~value + 1; 0: value
    if ( diff > trV ) return -1;

    return 0;
    /*
     return
       ((ABS((yuv1 & Ymask) - (yuv2 & Ymask)) > trY) ||
        (ABS((yuv1 & Umask) - (yuv2 & Umask)) > trU) ||
        (ABS((yuv1 & Vmask) - (yuv2 & Vmask)) > trV));
    */
}

/* --------------------------------------------------------------------------- */

static void inline hq2x_main( int w1, int w2, int w3, int w4, int w5, int w6, int w7, int w8, int w9,
        uint32_t nextlineSrc, uint32_t nextlineDst, uint16_t *p, uint16_t *q, int skiplastline )
{
    int pattern = 0;

    const int yuv5 = YUV( 5 );

    if ( w5 != w1 && diffYUV( yuv5, YUV( 1 ) ) ) pattern |= 0x0001;
    if ( w5 != w2 && diffYUV( yuv5, YUV( 2 ) ) ) pattern |= 0x0002;
    if ( w5 != w3 && diffYUV( yuv5, YUV( 3 ) ) ) pattern |= 0x0004;
    if ( w5 != w4 && diffYUV( yuv5, YUV( 4 ) ) ) pattern |= 0x0008;
    if ( w5 != w6 && diffYUV( yuv5, YUV( 6 ) ) ) pattern |= 0x0010;
    if ( w5 != w7 && diffYUV( yuv5, YUV( 7 ) ) ) pattern |= 0x0020;
    if ( w5 != w8 && diffYUV( yuv5, YUV( 8 ) ) ) pattern |= 0x0040;
    if ( w5 != w9 && diffYUV( yuv5, YUV( 9 ) ) ) pattern |= 0x0080;

    switch ( pattern )
    {
        case 0:
        case 1:
        case 4:
        case 32:
        case 128:
        case 5:
        case 132:
        case 160:
        case 33:
        case 129:
        case 36:
        case 133:
        case 164:
        case 161:
        case 37:
        case 165:
            PIXEL00_20
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 2:
        case 34:
        case 130:
        case 162:
            PIXEL00_22
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 16:
        case 17:
        case 48:
        case 49:
            PIXEL00_20
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 64:
        case 65:
        case 68:
        case 69:
            PIXEL00_20
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 8:
        case 12:
        case 136:
        case 140:
            PIXEL00_21
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 3:
        case 35:
        case 131:
        case 163:
            PIXEL00_11
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 6:
        case 38:
        case 134:
        case 166:
            PIXEL00_22
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 20:
        case 21:
        case 52:
        case 53:
            PIXEL00_20
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 144:
        case 145:
        case 176:
        case 177:
            PIXEL00_20
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 192:
        case 193:
        case 196:
        case 197:
            PIXEL00_20
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 96:
        case 97:
        case 100:
        case 101:
            PIXEL00_20
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 40:
        case 44:
        case 168:
        case 172:
            PIXEL00_21
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 9:
        case 13:
        case 137:
        case 141:
            PIXEL00_12
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 18:
        case 50:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 80:
        case 81:
            PIXEL00_20
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_10
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 72:
        case 76:
            PIXEL00_21
            PIXEL01_20
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 10:
        case 138:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 66:
            PIXEL00_22
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 24:
            PIXEL00_21
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 7:
        case 39:
        case 135:
            PIXEL00_11
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 148:
        case 149:
        case 180:
            PIXEL00_20
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 224:
        case 228:
        case 225:
            PIXEL00_20
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 41:
        case 169:
        case 45:
            PIXEL00_12
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 22:
        case 54:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 208:
        case 209:
            PIXEL00_20
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 104:
        case 108:
            PIXEL00_21
            PIXEL01_20
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 11:
        case 139:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 19:
        case 51:
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL00_11
                PIXEL01_10
            }
            else
            {
                PIXEL00_60
                PIXEL01_90
            }
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 146:
        case 178:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
                if ( !skiplastline ) PIXEL11_12
                }
            else
            {
                PIXEL01_90
                if ( !skiplastline ) PIXEL11_61
                }
            if ( !skiplastline ) PIXEL10_20
                break;
        case 84:
        case 85:
            PIXEL00_20
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                PIXEL01_11
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                PIXEL01_60
                if ( !skiplastline ) PIXEL11_90
                }
            if ( !skiplastline ) PIXEL10_21
                break;
        case 112:
        case 113:
            PIXEL00_20
            PIXEL01_22
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL10_12
                    if ( !skiplastline ) PIXEL11_10
                    }
            else
            {
                if ( !skiplastline ) PIXEL10_61
                    if ( !skiplastline ) PIXEL11_90
                    }
            break;
        case 200:
        case 204:
            PIXEL00_21
            PIXEL01_20
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                    if ( !skiplastline ) PIXEL11_11
                    }
            else
            {
                if ( !skiplastline ) PIXEL10_90
                    if ( !skiplastline ) PIXEL11_60
                    }
            break;
        case 73:
        case 77:
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                PIXEL00_12
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                PIXEL00_61
                if ( !skiplastline ) PIXEL10_90
                }
            PIXEL01_20
            if ( !skiplastline ) PIXEL11_22
                break;
        case 42:
        case 170:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
                if ( !skiplastline ) PIXEL10_11
                }
            else
            {
                PIXEL00_90
                if ( !skiplastline ) PIXEL10_60
                }
            PIXEL01_21
            if ( !skiplastline ) PIXEL11_20
                break;
        case 14:
        case 142:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
                PIXEL01_12
            }
            else
            {
                PIXEL00_90
                PIXEL01_61
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 67:
            PIXEL00_11
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 70:
            PIXEL00_22
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 28:
            PIXEL00_21
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 152:
            PIXEL00_21
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 194:
            PIXEL00_22
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 98:
            PIXEL00_22
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 56:
            PIXEL00_21
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 25:
            PIXEL00_12
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 26:
        case 31:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 82:
        case 214:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 88:
        case 248:
            PIXEL00_21
            PIXEL01_22
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_20
                }
            break;
        case 74:
        case 107:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_21
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 27:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_10
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 86:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_10
                    break;
        case 216:
            PIXEL00_21
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_10
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 106:
            PIXEL00_10
            PIXEL01_21
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 30:
            PIXEL00_10
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 210:
            PIXEL00_22
            PIXEL01_10
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 120:
            PIXEL00_21
            PIXEL01_22
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_10
                break;
        case 75:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_10
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 29:
            PIXEL00_12
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 198:
            PIXEL00_22
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 184:
            PIXEL00_21
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 99:
            PIXEL00_11
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 57:
            PIXEL00_12
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 71:
            PIXEL00_11
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 156:
            PIXEL00_21
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 226:
            PIXEL00_22
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 60:
            PIXEL00_21
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 195:
            PIXEL00_11
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 102:
            PIXEL00_22
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 153:
            PIXEL00_12
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 58:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 83:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_10
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_70
                    }
            break;
        case 92:
            PIXEL00_21
            PIXEL01_11
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 202:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            PIXEL01_21
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 78:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 154:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 114:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_10
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_70
                    }
            break;
        case 89:
            PIXEL00_12
            PIXEL01_22
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 90:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 55:
        case 23:
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL00_11
                PIXEL01_0
            }
            else
            {
                PIXEL00_60
                PIXEL01_90
            }
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 182:
        case 150:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
                if ( !skiplastline ) PIXEL11_12
                }
            else
            {
                PIXEL01_90
                if ( !skiplastline ) PIXEL11_61
                }
            if ( !skiplastline ) PIXEL10_20
                break;
        case 213:
        case 212:
            PIXEL00_20
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                PIXEL01_11
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                PIXEL01_60
                if ( !skiplastline ) PIXEL11_90
                }
            if ( !skiplastline ) PIXEL10_21
                break;
        case 241:
        case 240:
            PIXEL00_20
            PIXEL01_22
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL10_12
                    if ( !skiplastline ) PIXEL11_0
                    }
            else
            {
                if ( !skiplastline ) PIXEL10_61
                    if ( !skiplastline ) PIXEL11_90
                    }
            break;
        case 236:
        case 232:
            PIXEL00_21
            PIXEL01_20
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                    if ( !skiplastline ) PIXEL11_11
                    }
            else
            {
                if ( !skiplastline ) PIXEL10_90
                    if ( !skiplastline ) PIXEL11_60
                    }
            break;
        case 109:
        case 105:
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                PIXEL00_12
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                PIXEL00_61
                if ( !skiplastline ) PIXEL10_90
                }
            PIXEL01_20
            if ( !skiplastline ) PIXEL11_22
                break;
        case 171:
        case 43:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
                if ( !skiplastline ) PIXEL10_11
                }
            else
            {
                PIXEL00_90
                if ( !skiplastline ) PIXEL10_60
                }
            PIXEL01_21
            if ( !skiplastline ) PIXEL11_20
                break;
        case 143:
        case 15:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
                PIXEL01_12
            }
            else
            {
                PIXEL00_90
                PIXEL01_61
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 124:
            PIXEL00_21
            PIXEL01_11
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_10
                break;
        case 203:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_10
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 62:
            PIXEL00_10
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 211:
            PIXEL00_11
            PIXEL01_10
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 118:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_10
                    break;
        case 217:
            PIXEL00_12
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_10
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 110:
            PIXEL00_10
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 155:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_10
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 188:
            PIXEL00_21
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 185:
            PIXEL00_12
            PIXEL01_22
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 61:
            PIXEL00_12
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 157:
            PIXEL00_12
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 103:
            PIXEL00_11
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_22
                    break;
        case 227:
            PIXEL00_11
            PIXEL01_21
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 230:
            PIXEL00_22
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 199:
            PIXEL00_11
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_21
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 220:
            PIXEL00_21
            PIXEL01_11
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_20
                }
            break;
        case 158:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 234:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            PIXEL01_21
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 242:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 59:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 121:
            PIXEL00_12
            PIXEL01_22
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 87:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_10
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_70
                    }
            break;
        case 79:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 122:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 94:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 218:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_20
                }
            break;
        case 91:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 229:
            PIXEL00_20
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 167:
            PIXEL00_11
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 173:
            PIXEL00_12
            PIXEL01_20
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 181:
            PIXEL00_20
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 186:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 115:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_10
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_70
                    }
            break;
        case 93:
            PIXEL00_12
            PIXEL01_11
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_10
                }
            else
            {
                if ( !skiplastline ) PIXEL11_70
                }
            break;
        case 206:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 205:
        case 201:
            PIXEL00_12
            PIXEL01_20
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_10
                }
            else
            {
                if ( !skiplastline ) PIXEL10_70
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 174:
        case 46:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_10
            }
            else
            {
                PIXEL00_70
            }
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 179:
        case 147:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_10
            }
            else
            {
                PIXEL01_70
            }
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 117:
        case 116:
            PIXEL00_20
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_10
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_70
                    }
            break;
        case 189:
            PIXEL00_12
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 231:
            PIXEL00_11
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 126:
            PIXEL00_10
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_10
                break;
        case 219:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_10
            if ( !skiplastline ) PIXEL10_10
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 125:
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                PIXEL00_12
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                PIXEL00_61
                if ( !skiplastline ) PIXEL10_90
                }
            PIXEL01_11
            if ( !skiplastline ) PIXEL11_10
                break;
        case 221:
            PIXEL00_12
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                PIXEL01_11
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                PIXEL01_60
                if ( !skiplastline ) PIXEL11_90
                }
            if ( !skiplastline ) PIXEL10_10
                break;
        case 207:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
                PIXEL01_12
            }
            else
            {
                PIXEL00_90
                PIXEL01_61
            }
            if ( !skiplastline ) PIXEL10_10
                if ( !skiplastline ) PIXEL11_11
                    break;
        case 238:
            PIXEL00_10
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                    if ( !skiplastline ) PIXEL11_11
                    }
            else
            {
                if ( !skiplastline ) PIXEL10_90
                    if ( !skiplastline ) PIXEL11_60
                    }
            break;
        case 190:
            PIXEL00_10
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
                if ( !skiplastline ) PIXEL11_12
                }
            else
            {
                PIXEL01_90
                if ( !skiplastline ) PIXEL11_61
                }
            if ( !skiplastline ) PIXEL10_11
                break;
        case 187:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
                if ( !skiplastline ) PIXEL10_11
                }
            else
            {
                PIXEL00_90
                if ( !skiplastline ) PIXEL10_60
                }
            PIXEL01_10
            if ( !skiplastline ) PIXEL11_12
                break;
        case 243:
            PIXEL00_11
            PIXEL01_10
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL10_12
                    if ( !skiplastline ) PIXEL11_0
                    }
            else
            {
                if ( !skiplastline ) PIXEL10_61
                    if ( !skiplastline ) PIXEL11_90
                    }
            break;
        case 119:
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL00_11
                PIXEL01_0
            }
            else
            {
                PIXEL00_60
                PIXEL01_90
            }
            if ( !skiplastline ) PIXEL10_12
                if ( !skiplastline ) PIXEL11_10
                    break;
        case 237:
        case 233:
            PIXEL00_12
            PIXEL01_20
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 175:
        case 47:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            PIXEL01_12
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_20
                    break;
        case 183:
        case 151:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( !skiplastline ) PIXEL10_20
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 245:
        case 244:
            PIXEL00_20
            PIXEL01_11
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_100
                    }
            break;
        case 250:
            PIXEL00_10
            PIXEL01_10
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_20
                }
            break;
        case 123:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_10
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_10
                break;
        case 95:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_10
                if ( !skiplastline ) PIXEL11_10
                    break;
        case 222:
            PIXEL00_10
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_10
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 252:
            PIXEL00_21
            PIXEL01_11
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_100
                }
            break;
        case 249:
            PIXEL00_12
            PIXEL01_22
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_20
                }
            break;
        case 235:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_21
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 111:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_22
                break;
        case 63:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_21
                    break;
        case 159:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( !skiplastline ) PIXEL10_22
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 215:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( !skiplastline ) PIXEL10_21
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 246:
            PIXEL00_22
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_100
                    }
            break;
        case 254:
            PIXEL00_10
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_100
                }
            break;
        case 253:
            PIXEL00_12
            PIXEL01_11
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_100
                }
            break;
        case 251:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            PIXEL01_10
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_20
                }
            break;
        case 239:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            PIXEL01_12
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( !skiplastline ) PIXEL11_11
                break;
        case 127:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_20
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_20
                }
            if ( !skiplastline ) PIXEL11_10
                break;
        case 191:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( !skiplastline ) PIXEL10_11
                if ( !skiplastline ) PIXEL11_12
                    break;
        case 223:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_20
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( !skiplastline ) PIXEL10_10
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_20
                    }
            break;
        case 247:
            PIXEL00_11
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( !skiplastline ) PIXEL10_12
                if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
                {
                    if ( !skiplastline ) PIXEL11_0
                    }
                else
                {
                    if ( !skiplastline ) PIXEL11_100
                    }
            break;
        case 255:
            if ( diffYUV( YUV( 4 ), YUV( 2 ) ) )
            {
                PIXEL00_0
            }
            else
            {
                PIXEL00_100
            }
            if ( diffYUV( YUV( 2 ), YUV( 6 ) ) )
            {
                PIXEL01_0
            }
            else
            {
                PIXEL01_100
            }
            if ( diffYUV( YUV( 8 ), YUV( 4 ) ) )
            {
                if ( !skiplastline ) PIXEL10_0
                }
            else
            {
                if ( !skiplastline ) PIXEL10_100
                }
            if ( diffYUV( YUV( 6 ), YUV( 8 ) ) )
            {
                if ( !skiplastline ) PIXEL11_0
                }
            else
            {
                if ( !skiplastline ) PIXEL11_100
                }
            break;
    }
}

/* --------------------------------------------------------------------------- */

void InitLUT()
{
    int r, g, b;
    int Y, u, v;
    int color;

    // Allocate the YUV/LUT buffers on the fly if needed.
    if ( RGBtoYUV == 0 ) RGBtoYUV = ( uint32_t * )bgd_malloc( 65536 * sizeof( uint32_t ) );

    if ( LUT16to32 == 0 ) LUT16to32 = ( uint32_t * )bgd_malloc( 65536 * sizeof( uint32_t ) );

    for ( color = 0; color < 65536; ++color )
    {
        r = (( color & sys_pixel_format->Rmask ) >> sys_pixel_format->Rshift ) << sys_pixel_format->Rloss;
        g = (( color & sys_pixel_format->Gmask ) >> sys_pixel_format->Gshift ) << sys_pixel_format->Gloss;
        b = (( color & sys_pixel_format->Bmask ) >> sys_pixel_format->Bshift ) << sys_pixel_format->Bloss;
        LUT16to32[color] = ( r << 16 ) | ( g << 8 ) | b;

        Y = ( r + g + b ) >> 2;
        u = 128 + (( r - b ) >> 2 );
        v = 128 + (( -r + 2 * g - b ) >> 3 );
        RGBtoYUV[color] = ( Y << 16 ) | ( u << 8 ) | v;
    }
}

/* --------------------------------------------------------------------------- */

void hq2x( uint8_t *srcPtr, uint32_t srcPitch, uint8_t *dstPtr, uint32_t dstPitch, int width, int height )
{
    register int w1, w2, w3, w4, w5, w6, w7, w8, w9;

    const uint32_t nextlineSrc = srcPitch / sizeof( uint16_t );
    uint16_t *p = ( uint16_t * )srcPtr;
    const uint32_t nextlineDst = dstPitch / sizeof( uint16_t );
    uint16_t *q = ( uint16_t * )dstPtr;

    int tmpWidth;

    if ( !hq2xinited )
    {
        InitLUT();
        hq2xinited = 1;
    }

    //  +----+----+----+
    //  |    |    |    |
    //  | w1 | w2 | w3 |
    //  +----+----+----+
    //  |    |    |    |
    //  | w4 | w5 | w6 |
    //  +----+----+----+
    //  |    |    |    |
    //  | w7 | w8 | w9 |
    //  +----+----+----+


    /********************* Primera fila **************************/
    w1 = 0; w4 = 0; w7 = 0;
    w2 = 0; w5 = *( p ); w8 = *( p + nextlineSrc );

    tmpWidth = width - 1;
    while ( tmpWidth-- )
    {
        p++;

        w3 = 0; w6 = *( p ); w9 = *( p + nextlineSrc );

        hq2x_main( w1, w2, w3, w4, w5, w6, w7, w8, w9, nextlineSrc, nextlineDst, p, q, 0 );

        w1 = w2; w4 = w5; w7 = w8;
        w2 = w3; w5 = w6; w8 = w9;

        q += 2;
    }

    /* Ultima columna de la primera fila */
    p++;

    w3 = 0; w6 = 0; w9 = 0;

    hq2x_main( w1, w2, w3, w4, w5, w6, w7, w8, w9, nextlineSrc, nextlineDst, p, q, 0 );

    w1 = w2; w4 = w5; w7 = w8;
    w2 = w3; w5 = w6; w8 = w9;

    q += 2;

    /* Proxima linea */
    p += nextlineSrc - width;
    q += ( nextlineDst - width ) * 2;

    height -= 2;

    /********************* cuerpo principal **************************/

    while ( height-- )
    {
//  w1 = *(p - 1 - nextlineSrc); w4 = *(p - 1); w7 = *(p - 1 + nextlineSrc);
        w1 = 0; w4 = 0; w7 = 0;
        w2 = *( p - nextlineSrc ); w5 = *( p ); w8 = *( p + nextlineSrc );

        tmpWidth = width - 1;
        while ( tmpWidth-- )
        {
            p++;

            w3 = *( p - nextlineSrc ); w6 = *( p ); w9 = *( p + nextlineSrc );

            hq2x_main( w1, w2, w3, w4, w5, w6, w7, w8, w9, nextlineSrc, nextlineDst, p, q, 0 );

            w1 = w2; w4 = w5; w7 = w8;
            w2 = w3; w5 = w6; w8 = w9;

            q += 2;
        }

        /* Ultima columna de la fila N */
        p++;

        w3 = 0; w6 = 0; w9 = 0;

        hq2x_main( w1, w2, w3, w4, w5, w6, w7, w8, w9, nextlineSrc, nextlineDst, p, q, 0 );

        w1 = w2; w4 = w5; w7 = w8;
        w2 = w3; w5 = w6; w8 = w9;

        q += 2;

        p += nextlineSrc - width;
        q += ( nextlineDst - width ) * 2;
    }

    /********************* ultima linea **************************/

    w1 = 0; w4 = 0; w7 = 0;
    w2 = *( p - nextlineSrc ); w5 = *( p ); w8 = 0;

    tmpWidth = width - 1;
    while ( tmpWidth-- )
    {
        p++;

        w3 = *( p - nextlineSrc ); w6 = *( p ); w9 = 0;

        hq2x_main( w1, w2, w3, w4, w5, w6, w7, w8, w9, nextlineSrc, nextlineDst, p, q, 0 );

        w1 = w2; w4 = w5; w7 = w8;
        w2 = w3; w5 = w6; w8 = w9;

        q += 2;
    }

    /* Ultima columna de la utima fila */
    p++;

    w3 = 0; w6 = 0; w9 = 0;

    hq2x_main( w1, w2, w3, w4, w5, w6, w7, w8, w9, nextlineSrc, nextlineDst, p, q, 1 );
}

/* --------------------------------------------------------------------------- */
