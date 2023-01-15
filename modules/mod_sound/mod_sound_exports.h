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

#ifndef __MOD_SOUND_EXPORTS
#define __MOD_SOUND_EXPORTS

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

#if defined(__BGDC__) || !defined(__STATIC__)

/* ----------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)                   */

DLCONSTANT  __bgdexport( mod_sound, constants_def )[] =
{
    { "MODE_MONO"   , TYPE_INT, 0  },
    { "MODE_STEREO" , TYPE_INT, 1  },
    { "ALL_SOUND"   , TYPE_INT, -1 },
    { NULL          , 0       , 0  }
} ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion)           */

char * __bgdexport( mod_sound, globals_def ) =
    "   sound_freq = 22050 ;\n"
    "   sound_mode = MODE_STEREO ;\n"
    "   sound_channels = 8 ;\n";

#endif

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_sound, functions_exports )[] =
{
    FUNC( "SOUND_INIT"          , ""     , TYPE_INT , modsound_init               ),
    FUNC( "SOUND_CLOSE"         , ""     , TYPE_INT , modsound_close              ),

    FUNC( "LOAD_SONG"           , "S"    , TYPE_INT , modsound_load_song          ),
    FUNC( "LOAD_SONG"           , "SP"   , TYPE_INT , modsound_bgload_song        ),
    FUNC( "UNLOAD_SONG"         , "I"    , TYPE_INT , modsound_unload_song        ),
    FUNC( "UNLOAD_SONG"         , "P"    , TYPE_INT , modsound_unload_song2       ),

    FUNC( "PLAY_SONG"           , "II"   , TYPE_INT , modsound_play_song          ),
    FUNC( "STOP_SONG"           , ""     , TYPE_INT , modsound_stop_song          ),
    FUNC( "PAUSE_SONG"          , ""     , TYPE_INT , modsound_pause_song         ),
    FUNC( "RESUME_SONG"         , ""     , TYPE_INT , modsound_resume_song        ),

    FUNC( "SET_SONG_VOLUME"     , "I"    , TYPE_INT , modsound_set_song_volume    ),

    FUNC( "IS_PLAYING_SONG"     , ""     , TYPE_INT , modsound_is_playing_song    ),

    FUNC( "LOAD_WAV"            , "S"    , TYPE_INT , modsound_load_wav           ),
    FUNC( "LOAD_WAV"            , "SP"   , TYPE_INT , modsound_bgload_wav         ),
    FUNC( "UNLOAD_WAV"          , "I"    , TYPE_INT , modsound_unload_wav         ),
    FUNC( "UNLOAD_WAV"          , "P"    , TYPE_INT , modsound_unload_wav2        ),

    FUNC( "PLAY_WAV"            , "II"   , TYPE_INT , modsound_play_wav           ),
    FUNC( "PLAY_WAV"            , "III"  , TYPE_INT , modsound_play_wav_channel   ),
    FUNC( "STOP_WAV"            , "I"    , TYPE_INT , modsound_stop_wav           ),
    FUNC( "PAUSE_WAV"           , "I"    , TYPE_INT , modsound_pause_wav          ),
    FUNC( "RESUME_WAV"          , "I"    , TYPE_INT , modsound_resume_wav         ),

    FUNC( "IS_PLAYING_WAV"      , "I"    , TYPE_INT , modsound_is_playing_wav     ),

    FUNC( "FADE_MUSIC_IN"       , "III"  , TYPE_INT , modsound_fade_music_in      ),
    FUNC( "FADE_MUSIC_OFF"      , "I"    , TYPE_INT , modsound_fade_music_off     ),

    FUNC( "SET_WAV_VOLUME"      , "II"   , TYPE_INT , modsound_set_wav_volume     ),
    FUNC( "SET_CHANNEL_VOLUME"  , "II"   , TYPE_INT , modsound_set_channel_volume ),

    FUNC( "RESERVE_CHANNELS"    , "I"    , TYPE_INT , modsound_reserve_channels   ),

    FUNC( "SET_PANNING"         , "III"  , TYPE_INT , modsound_set_panning        ),
    FUNC( "SET_POSITION"        , "III"  , TYPE_INT , modsound_set_position       ),
    FUNC( "SET_DISTANCE"        , "II"   , TYPE_INT , modsound_set_distance       ),

    FUNC( "REVERSE_STEREO"      , "II"   , TYPE_INT , modsound_reverse_stereo     ),

    FUNC( "SET_MUSIC_POSITION"  , "F"    , TYPE_INT , modsound_set_music_position ),

    FUNC( 0                     , 0      , 0        , 0                           )
};

/* --------------------------------------------------------------------------- */

#endif