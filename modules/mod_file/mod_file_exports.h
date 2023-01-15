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

#ifndef __MOD_FILE_EXPORTS
#define __MOD_FILE_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */

DLCONSTANT  __bgdexport( mod_file, constants_def)[] =
{
    { "O_READ"      , TYPE_INT, 0  },
    { "O_READWRITE" , TYPE_INT, 1  },
    { "O_RDWR"      , TYPE_INT, 1  },
    { "O_WRITE"     , TYPE_INT, 2  },
    { "O_ZREAD"     , TYPE_INT, 3  },
    { "O_ZWRITE"    , TYPE_INT, 4  },

    { "SEEK_SET"    , TYPE_INT, 0  },
    { "SEEK_CUR"    , TYPE_INT, 1  },
    { "SEEK_END"    , TYPE_INT, 2  },

    { NULL          , 0       , 0  }
} ;

#endif

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_file, functions_exports)[] =
{
    /* Ficheros */
    FUNC( "SAVE"        , "SV++" , TYPE_INT         , modfile_save        ),
    FUNC( "LOAD"        , "SV++" , TYPE_INT         , modfile_load        ),
    FUNC( "FOPEN"       , "SI"   , TYPE_INT         , modfile_fopen       ),
    FUNC( "FCLOSE"      , "I"    , TYPE_INT         , modfile_fclose      ),
    FUNC( "FREAD"       , "IV++" , TYPE_INT         , modfile_fread       ),
    FUNC( "FREAD"       , "PII"  , TYPE_INT         , modfile_freadC      ),
    FUNC( "FWRITE"      , "IV++" , TYPE_INT         , modfile_fwrite      ),
    FUNC( "FWRITE"      , "PII"  , TYPE_INT         , modfile_fwriteC     ),
    FUNC( "FSEEK"       , "III"  , TYPE_INT         , modfile_fseek       ),
    FUNC( "FREWIND"     , "I"    , TYPE_UNDEFINED   , modfile_frewind     ),
    FUNC( "FTELL"       , "I"    , TYPE_INT         , modfile_ftell       ),
    FUNC( "FFLUSH"      , "I"    , TYPE_INT         , modfile_fflush      ),
    FUNC( "FLUSH"       , "I"    , TYPE_INT         , modfile_fflush      ),
    FUNC( "FLENGTH"     , "I"    , TYPE_INT         , modfile_filelength  ),
    FUNC( "FPUTS"       , "IS"   , TYPE_INT         , modfile_fputs       ),
    FUNC( "FGETS"       , "I"    , TYPE_STRING      , modfile_fgets       ),
    FUNC( "FEOF"        , "I"    , TYPE_INT         , modfile_feof        ),
    FUNC( "FILE"        , "S"    , TYPE_STRING      , modfile_file        ),
    FUNC( "FEXISTS"     , "S"    , TYPE_INT         , modfile_exists      ) ,
    FUNC( "FILE_EXISTS" , "S"    , TYPE_INT         , modfile_exists      ) ,
    FUNC( "FREMOVE"     , "S"    , TYPE_INT         , modfile_remove      ) ,
    FUNC( "FMOVE"       , "SS"   , TYPE_INT         , modfile_move        ) ,
    FUNC( 0             , 0      , 0                , 0                   )
};

/* ----------------------------------------------------------------- */

#endif
