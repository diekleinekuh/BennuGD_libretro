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

#ifndef __MOD_CRYPT_EXPORTS
#define __MOD_CRYPT_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* ----------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */

#include "b_crypt.h"

/* ----------------------------------------------------------------- */

DLCONSTANT  __bgdexport( mod_crypt, constants_def)[] =
{
    { "CRYPT_NONE"  , TYPE_INT  , CRYPT_NONE    },
    { "CRYPT_DES"   , TYPE_INT  , CRYPT_DES     },
    { "CRYPT_3DES"  , TYPE_INT  , CRYPT_3DES    },

    { NULL          , 0         , 0             }
} ;

#endif

/* ----------------------------------------------------------------- */
/* Functions declaration                                             */

DLSYSFUNCS  __bgdexport( mod_crypt, functions_exports)[] =
{
    FUNC( "CRYPT_NEW"       , "IP"      , TYPE_POINTER     , modcrypt_new       ),
    FUNC( "CRYPT_DEL"       , "P"       , TYPE_INT         , modcrypt_del       ),
    FUNC( "CRYPT_ENCRYPT"   , "PPPI"    , TYPE_INT         , modcrypt_encrypt   ),
    FUNC( "CRYPT_DECRYPT"   , "PPPI"    , TYPE_INT         , modcrypt_decrypt   ),
    FUNC( "CRYPT_ENCRYPT"   , "IPPPI"   , TYPE_INT         , modcrypt_encrypt2  ),
    FUNC( "CRYPT_DECRYPT"   , "IPPPI"   , TYPE_INT         , modcrypt_decrypt2  ),

    FUNC( 0                 , 0         , 0                , 0                  )
};

/* ----------------------------------------------------------------- */

#endif
