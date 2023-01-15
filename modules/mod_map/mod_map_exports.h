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

#ifndef __MOD_MAP_EXPORTS
#define __MOD_MAP_EXPORTS

/* ----------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */

#include "mod_map.h"
#include "libfont.h"

/* ----------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_map, constants_def )[] =
{
    { "G_WIDE"          , TYPE_INT, G_WIDE              },    /* Obsolete */
    { "G_WIDTH"         , TYPE_INT, G_WIDTH             },
    { "G_HEIGHT"        , TYPE_INT, G_HEIGHT            },
    { "G_CENTER_X"      , TYPE_INT, G_CENTER_X          },
    { "G_X_CENTER"      , TYPE_INT, G_X_CENTER          },
    { "G_CENTER_Y"      , TYPE_INT, G_CENTER_Y          },
    { "G_Y_CENTER"      , TYPE_INT, G_Y_CENTER          },
    { "G_PITCH"         , TYPE_INT, G_PITCH             },
    { "G_DEPTH"         , TYPE_INT, G_DEPTH             },

    { "B_CLEAR"         , TYPE_INT, B_CLEAR             },

    { "CHARSET_ISO8859" , TYPE_INT, CHARSET_ISO8859     },
    { "CHARSET_CP850"   , TYPE_INT, CHARSET_CP850       },

    { "NFB_VARIABLEWIDTH", TYPE_INT, 0                  },
    { "NFB_FIXEDWIDTH"  , TYPE_INT, NFB_FIXEDWIDTH      },

    { NULL              , 0       , 0                   }
} ;

#endif

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_map, functions_exports )[] =
{
    /* Bitmaps */
    FUNC( "MAP_BLOCK_COPY"      , "IIIIIIIIII"  , TYPE_INT      , modmap_map_block_copy     ),
    FUNC( "MAP_PUT"             , "IIIII"       , TYPE_INT      , modmap_map_put            ),
    FUNC( "MAP_XPUT"            , "IIIIIIII"    , TYPE_INT      , modmap_map_xput           ),
    FUNC( "MAP_NEW"             , "III"         , TYPE_INT      , modmap_new_map            ),
    FUNC( "MAP_NEW"             , "IIII"        , TYPE_INT      , modmap_new_map_extend     ),
    FUNC( "MAP_CLEAR"           , "III"         , TYPE_INT      , modmap_map_clear          ),
    FUNC( "MAP_CLONE"           , "II"          , TYPE_INT      , modmap_map_clone          ),
    FUNC( "MAP_NAME"            , "II"          , TYPE_STRING   , modmap_map_name           ),
    FUNC( "MAP_SET_NAME"        , "IIS"         , TYPE_INT      , modmap_map_set_name       ),
    FUNC( "MAP_EXISTS"          , "II"          , TYPE_INT      , modmap_map_exists         ),
    FUNC( "MAP_XPUTNP"          , "IIIIIIIIII"  , TYPE_INT      , modmap_map_xputnp         ),
    FUNC( "MAP_DEL"             , "II"          , TYPE_INT      , modmap_unload_map         ),
    FUNC( "MAP_UNLOAD"          , "II"          , TYPE_INT      , modmap_unload_map         ),
    FUNC( "MAP_LOAD"            , "S"           , TYPE_INT      , modmap_load_map           ),
    FUNC( "MAP_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_map         ),
    FUNC( "MAP_SAVE"            , "IIS"         , TYPE_INT      , modmap_save_map           ),
    FUNC( "MAP_BUFFER"          , "II"          , TYPE_POINTER  , modmap_map_buffer         ),

    /* FPG */
    FUNC( "FPG_ADD"             , "IIII"        , TYPE_INT      , modmap_fpg_add            ),
    FUNC( "FPG_NEW"             , ""            , TYPE_INT      , modmap_fpg_new            ),
    FUNC( "FPG_EXISTS"          , "I"           , TYPE_INT      , modmap_fpg_exists         ),
    FUNC( "FPG_LOAD"            , "S"           , TYPE_INT      , modmap_load_fpg           ),
    FUNC( "FPG_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_fpg         ),
    FUNC( "FPG_SAVE"            , "IS"          , TYPE_INT      , modmap_save_fpg           ),
    FUNC( "FPG_DEL"             , "I"           , TYPE_INT      , modmap_unload_fpg         ),
    FUNC( "FPG_UNLOAD"          , "I"           , TYPE_INT      , modmap_unload_fpg         ),

    /* Palette */
    FUNC( "PAL_NEW"             , ""            , TYPE_INT      , modmap_pal_create         ),
    FUNC( "PAL_DEL"             , "I"           , TYPE_INT      , modmap_pal_unload         ),
    FUNC( "PAL_UNLOAD"          , "I"           , TYPE_INT      , modmap_pal_unload         ),
    FUNC( "PAL_CLONE"           , "I"           , TYPE_INT      , modmap_pal_clone          ),
    FUNC( "PAL_REFRESH"         , ""            , TYPE_INT      , modmap_pal_refresh        ),
    FUNC( "PAL_REFRESH"         , "I"           , TYPE_INT      , modmap_pal_refresh_2      ),
    FUNC( "PAL_MAP_GETID"       , "II"          , TYPE_INT      , modmap_pal_map_getid      ),
    FUNC( "PAL_MAP_ASSIGN"      , "III"         , TYPE_INT      , modmap_pal_map_assign     ),
    FUNC( "PAL_MAP_REMOVE"      , "II"          , TYPE_INT      , modmap_pal_map_remove     ),
    FUNC( "PAL_GET"             , "IIP"         , TYPE_INT      , modmap_get_colors         ),
    FUNC( "PAL_GET"             , "IIIP"        , TYPE_INT      , modmap_pal_get            ),
    FUNC( "PAL_SYS_SET"         , "I"           , TYPE_INT      , modmap_set_system_pal     ),
    FUNC( "PAL_SYS_SET"         , "P"           , TYPE_INT      , modmap_set_system_pal_raw ),
    FUNC( "PAL_SET"             , "IIP"         , TYPE_INT      , modmap_set_colors         ),
    FUNC( "PAL_SET"             , "IIIP"        , TYPE_INT      , modmap_pal_set            ),
    FUNC( "PAL_SAVE"            , "S"           , TYPE_INT      , modmap_save_system_pal    ),
    FUNC( "PAL_SAVE"            , "SI"          , TYPE_INT      , modmap_save_pal           ),
    FUNC( "PAL_LOAD"            , "S"           , TYPE_INT      , modmap_load_pal           ),
    FUNC( "PAL_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_pal         ),

    FUNC( "COLORS_SET"          , "IIP"         , TYPE_INT      , modmap_set_colors         ),
    FUNC( "COLORS_SET"          , "IIIP"        , TYPE_INT      , modmap_pal_set            ),
    FUNC( "COLORS_GET"          , "IIP"         , TYPE_INT      , modmap_get_colors         ),
    FUNC( "COLORS_GET"          , "IIIP"        , TYPE_INT      , modmap_pal_get            ),

    FUNC( "PALETTE_ROLL"        , "III"         , TYPE_INT      , modmap_roll_palette       ),
    FUNC( "PALETTE_CONVERT"     , "IIP"         , TYPE_INT      , modmap_convert_palette    ),

    FUNC( "COLOR_FIND"          , "BBB"         , TYPE_INT      , modmap_find_color         ),
    FUNC( "COLOR_FIND"          , "IBBB"        , TYPE_INT      , modmap_find_color2        ),
    FUNC( "COLOR_FIND"          , "IIBBB"       , TYPE_INT      , modmap_find_color3        ),

    FUNC( "RGB"                 , "IIBBB"       , TYPE_INT      , modmap_rgb_map            ),
    FUNC( "RGBA"                , "IIBBBB"      , TYPE_INT      , modmap_rgba_map           ),
    FUNC( "RGB"                 , "BBBI"        , TYPE_INT      , modmap_rgb_depth          ),
    FUNC( "RGBA"                , "BBBBI"       , TYPE_INT      , modmap_rgba_depth         ),
    FUNC( "RGB"                 , "BBB"         , TYPE_INT      , modmap_rgb                ),
    FUNC( "RGBA"                , "BBBB"        , TYPE_INT      , modmap_rgba               ),

    FUNC( "RGB_GET"             , "IIIPPP"      , TYPE_INT      , modmap_get_rgb_map        ),
    FUNC( "RGBA_GET"            , "IIIPPPP"     , TYPE_INT      , modmap_get_rgba_map       ),
    FUNC( "RGB_GET"             , "IPPPI"       , TYPE_INT      , modmap_get_rgb_depth      ),
    FUNC( "RGBA_GET"            , "IPPPPI"      , TYPE_INT      , modmap_get_rgba_depth     ),
    FUNC( "RGB_GET"             , "IPPP"        , TYPE_INT      , modmap_get_rgb            ),
    FUNC( "RGBA_GET"            , "IPPPP"       , TYPE_INT      , modmap_get_rgba           ),

    FUNC( "FADE"                , "IIII"        , TYPE_INT      , modmap_fade               ),
    FUNC( "FADE_ON"             , ""            , TYPE_INT      , modmap_fade_on            ),
    FUNC( "FADE_OFF"            , ""            , TYPE_INT      , modmap_fade_off           ),

    /* Informacion de graficos */
    FUNC( "MAP_INFO_SET"        , "IIII"        , TYPE_INT      , modmap_graphic_set        ),
    FUNC( "MAP_INFO_GET"        , "III"         , TYPE_INT      , modmap_graphic_info       ),
    FUNC( "MAP_INFO"            , "III"         , TYPE_INT      , modmap_graphic_info       ),

    FUNC( "GRAPHIC_SET"         , "IIII"        , TYPE_INT      , modmap_graphic_set        ),
    FUNC( "GRAPHIC_INFO"        , "III"         , TYPE_INT      , modmap_graphic_info       ),

    /* Puntos de control */
    FUNC( "POINT_GET"           , "IIIPP"       , TYPE_INT      , modmap_get_point          ),
    FUNC( "POINT_SET"           , "IIIII"       , TYPE_INT      , modmap_set_point          ),

    FUNC( "CENTER_SET"          , "IIII"        , TYPE_INT      , modmap_set_center         ),

    /* Fonts */
    FUNC( "FNT_LOAD"            , "S"           , TYPE_INT      , modmap_load_fnt           ),
    FUNC( "FNT_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_fnt         ),
    FUNC( "FNT_UNLOAD"          , "I"           , TYPE_INT      , modmap_unload_fnt         ),
    FUNC( "FNT_SAVE"            , "IS"          , TYPE_INT      , modmap_save_fnt           ),
    FUNC( "FNT_NEW"             , "I"           , TYPE_INT      , modmap_fnt_new            ),
    FUNC( "FNT_NEW"             , "II"          , TYPE_INT      , modmap_fnt_new_charset    ),
    FUNC( "FNT_NEW"             , "IIIIIIII"    , TYPE_INT      , modmap_fnt_new_from_bitmap),

    FUNC( "BDF_LOAD"            , "S"           , TYPE_INT      , modmap_load_bdf           ),
    FUNC( "BDF_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_bdf         ),

    FUNC( "GLYPH_GET"           , "II"          , TYPE_INT      , modmap_get_glyph          ),
    FUNC( "GLYPH_SET"           , "IIII"        , TYPE_INT      , modmap_set_glyph          ),

    /* Importacion de archivos graficos */
    FUNC( "PNG_LOAD"            , "S"           , TYPE_INT      , modmap_load_png           ),
    FUNC( "PNG_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_png         ),
    FUNC( "PCX_LOAD"            , "S"           , TYPE_INT      , modmap_load_pcx           ),
    FUNC( "PCX_LOAD"            , "SP"          , TYPE_INT      , modmap_bgload_pcx         ),

    /* Exportacion de mapas Graficos */
    FUNC( "PNG_SAVE"            , "IIS"         , TYPE_INT      , modmap_save_png           ),

    /* ------------ Compatibility ------------ */

    /* Mapas */
    FUNC( "NEW_MAP"             , "III"         , TYPE_INT      , modmap_new_map            ),
    FUNC( "LOAD_MAP"            , "S"           , TYPE_INT      , modmap_load_map           ),
    FUNC( "LOAD_MAP"            , "SP"          , TYPE_INT      , modmap_bgload_map         ),
    FUNC( "UNLOAD_MAP"          , "II"          , TYPE_INT      , modmap_unload_map         ),
    FUNC( "SAVE_MAP"            , "IIS"         , TYPE_INT      , modmap_save_map           ),

    /* Palette */
    FUNC( "NEW_PAL"             , ""            , TYPE_INT      , modmap_pal_create         ),
    FUNC( "LOAD_PAL"            , "S"           , TYPE_INT      , modmap_load_pal           ),
    FUNC( "LOAD_PAL"            , "SP"          , TYPE_INT      , modmap_bgload_pal         ),
    FUNC( "UNLOAD_PAL"          , "I"           , TYPE_INT      , modmap_pal_unload         ),
    FUNC( "SAVE_PAL"            , "S"           , TYPE_INT      , modmap_save_system_pal    ),
    FUNC( "SAVE_PAL"            , "SI"          , TYPE_INT      , modmap_save_pal           ),

    FUNC( "SET_COLORS"          , "IIP"         , TYPE_INT      , modmap_set_colors         ),
    FUNC( "SET_COLORS"          , "IIIP"        , TYPE_INT      , modmap_pal_set            ),
    FUNC( "GET_COLORS"          , "IIP"         , TYPE_INT      , modmap_get_colors         ),
    FUNC( "GET_COLORS"          , "IIIP"        , TYPE_INT      , modmap_pal_get            ),

    FUNC( "ROLL_PALETTE"        , "III"         , TYPE_INT      , modmap_roll_palette       ),
    FUNC( "CONVERT_PALETTE"     , "IIP"         , TYPE_INT      , modmap_convert_palette    ),

    FUNC( "FIND_COLOR"          , "BBB"         , TYPE_INT      , modmap_find_color         ),
    FUNC( "FIND_COLOR"          , "IBBB"        , TYPE_INT      , modmap_find_color2        ),
    FUNC( "FIND_COLOR"          , "IIBBB"       , TYPE_INT      , modmap_find_color3        ),

    FUNC( "GET_RGB"             , "IIIPPP"      , TYPE_INT      , modmap_get_rgb_map        ),
    FUNC( "GET_RGBA"            , "IIIPPPP"     , TYPE_INT      , modmap_get_rgba_map       ),
    FUNC( "GET_RGB"             , "IPPPI"       , TYPE_INT      , modmap_get_rgb_depth      ),
    FUNC( "GET_RGBA"            , "IPPPPI"      , TYPE_INT      , modmap_get_rgba_depth     ),
    FUNC( "GET_RGB"             , "IPPP"        , TYPE_INT      , modmap_get_rgb            ),
    FUNC( "GET_RGBA"            , "IPPPP"       , TYPE_INT      , modmap_get_rgba           ),

    /* FPG */
    FUNC( "NEW_FPG"             , ""            , TYPE_INT      , modmap_fpg_new            ),
    FUNC( "LOAD_FPG"            , "S"           , TYPE_INT      , modmap_load_fpg           ),
    FUNC( "LOAD_FPG"            , "SP"          , TYPE_INT      , modmap_bgload_fpg         ),
    FUNC( "SAVE_FPG"            , "IS"          , TYPE_INT      , modmap_save_fpg           ),
    FUNC( "UNLOAD_FPG"          , "I"           , TYPE_INT      , modmap_unload_fpg         ),

    /* Puntos de control */
    FUNC( "GET_POINT"           , "IIIPP"       , TYPE_INT      , modmap_get_point          ),
    FUNC( "SET_POINT"           , "IIIII"       , TYPE_INT      , modmap_set_point          ),
    FUNC( "SET_CENTER"          , "IIII"        , TYPE_INT      , modmap_set_center         ),

    /* Fonts */
    FUNC( "NEW_FNT"             , "I"           , TYPE_INT      , modmap_fnt_new            ),
    FUNC( "NEW_FNT"             , "II"          , TYPE_INT      , modmap_fnt_new_charset    ),
    FUNC( "NEW_FNT"             , "IIIIIIII"    , TYPE_INT      , modmap_fnt_new_from_bitmap),
    FUNC( "LOAD_FNT"            , "S"           , TYPE_INT      , modmap_load_fnt           ),
    FUNC( "LOAD_FNT"            , "SP"          , TYPE_INT      , modmap_bgload_fnt         ),
    FUNC( "UNLOAD_FNT"          , "I"           , TYPE_INT      , modmap_unload_fnt         ),
    FUNC( "SAVE_FNT"            , "IS"          , TYPE_INT      , modmap_save_fnt           ),
    FUNC( "LOAD_BDF"            , "S"           , TYPE_INT      , modmap_load_bdf           ),
    FUNC( "LOAD_BDF"            , "SP"          , TYPE_INT      , modmap_bgload_bdf         ),
    FUNC( "GET_GLYPH"           , "II"          , TYPE_INT      , modmap_get_glyph          ),
    FUNC( "SET_GLYPH"           , "IIII"        , TYPE_INT      , modmap_set_glyph          ),

    /* Importacion de archivos graficos */
    FUNC( "LOAD_PNG"            , "S"           , TYPE_INT      , modmap_load_png           ),
    FUNC( "LOAD_PNG"            , "SP"          , TYPE_INT      , modmap_bgload_png         ),
    FUNC( "LOAD_PCX"            , "S"           , TYPE_INT      , modmap_load_pcx           ),
    FUNC( "LOAD_PCX"            , "SP"          , TYPE_INT      , modmap_bgload_pcx         ),

    /* Exportacion de mapas Graficos */
    FUNC( "SAVE_PNG"            , "IIS"         , TYPE_INT      , modmap_save_png           ),

    FUNC( 0                     , 0             , 0             , 0                         )
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_map, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libblit",
    "libfont",
    NULL
};

/* --------------------------------------------------------------------------- */

#endif
