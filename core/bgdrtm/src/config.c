/*
 *  Copyright Â© 2006-2019 SplinterGU (Fenix/Bennugd)
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
#include <ctype.h>
#include "bgdrtm.h"

#include "sysprocs_p.h"

#define INI_MAX_LINE    2048

/*-------------------------------------------------------------------------------- */

static char *trim( char *buffer ) {
    char *p = strchr( buffer, '\0' ) - 1;
    while ( ( p >= buffer ) && isspace( *p ) ) *p-- = '\0';
    p = buffer; while ( *p && isspace( *p ) ) p++;
    return p;
}

/*-------------------------------------------------------------------------------- */

static char * getvalue( char * string ) {
    char * p, ch;
    p = trim( string );
    if ( ( ch = *p ) == '"' || ch == '\'' ) {
        string = ++p;
        while(*string && *string != ch) string++;
        if ( *string != ch ) return NULL;
        *string = '\0';
    }
    return p;
}

/*-------------------------------------------------------------------------------- */

int config_read() {
    int line = 0, n;
    file * fp;
    char * section = NULL, * key = NULL, * value = NULL, * p;
    char * buffer = bgd_malloc(INI_MAX_LINE);
    if ( !buffer ) return -1;

    fp = file_open( configfile, "rb0" ) ;
    if ( !fp ) {
        bgd_free( buffer );
        return -1;
    }

    while ( file_gets( fp, buffer, INI_MAX_LINE ) ) {
        line++;
        p = trim( buffer );

        if ( ( *p == '\0' ) || ( *p == '#' ) || ( *p == ';' ) ) continue;

        if ( *p == '[' ) {
            if ( section ) bgd_free( section );
            section = bgd_strdup( p + 1 );
            p = strchr( section, ']');
            if ( p ) *p = '\0';
            continue;
        }

        value = strchr( p, '=' );
        if ( !value ) {
            fprintf( stderr, "error: syntax error at %s line %d\n", configfile, line );
            break;
        }
        *value++ = '\0';

        key = trim( p );
        value = getvalue( value );
        if ( *key == '\0' || value == NULL ) {
            fprintf( stderr, "error: syntax error at %s line %d\n", configfile, line );
            break;
        }

        /* Initialize all modules */
        if ( module_config_count )
            for ( n = 0; n < module_config_count; n++ )
                module_config_list[n](line, section, key, value);
    }


    file_close( fp );
    bgd_free( section );

    return 0;
}

/*-------------------------------------------------------------------------------- */
