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

/* By Splinter 2007 */

/* --------------------------------------------------------------------------- */

#include "librender.h"

/* --------------------------------------------------------------------------- */

void scale_normal2x( uint8_t *srcPtr, uint32_t srcPitch, uint8_t *dstPtr, uint32_t dstPitch, int width, int height )
{
    const uint32_t nextlineSrc = srcPitch / sizeof( uint16_t );
    const uint16_t *p = ( const uint16_t * )srcPtr;

    const uint32_t nextlineDst = dstPitch / sizeof( uint16_t );
    uint16_t *q = ( uint16_t * )dstPtr;

    while ( height-- )
    {
        int tmpWidth = width;
        while ( tmpWidth-- )
        {
            *( q + nextlineDst ) = *q = *p;
            q++;
            *( q + nextlineDst ) = *q = *p;
            q++;
            p++;
        }
        p += nextlineSrc - width;
        q += ( nextlineDst - width ) * 2;
    }
}

/* --------------------------------------------------------------------------- */
