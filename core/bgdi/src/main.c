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

/*
 * INCLUDES
 */

#ifdef _WIN32
#define  _WIN32_WINNT 0x0500
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "bgdi.h"
#include "bgdrtm.h"
#include "xstrings.h"
#include "dirs.h"

/* ---------------------------------------------------------------------- */

static char * dcb_exts[] = { ".dcb", ".dat", ".bin", NULL };

static int standalone  = 0;  /* 1 only if this is an standalone interpreter   */
static int embedded    = 0;  /* 1 only if this is a stub with an embedded DCB */

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : main
 *
 *  Main function
 *
 *  PARAMS:
 *      INT n: ERROR LEVEL to return to OS
 *
 *  RETURN VALUE:
 *      No value
 *
 */
#if LIBRETRO_CORE
int bgdi_main(int argc, char *argv[])
#else
int main( int argc, char *argv[] )
#endif
{
    char * filename = NULL, dcbname[ __MAX_PATH ], *ptr, *arg0;
    int i, j, ret = -1;
    file * fp = NULL;
    dcb_signature dcb_signature;

    /* disable stdout buffering */
    setvbuf( stdout, NULL, _IONBF, BUFSIZ );

    /* get my executable name */

#ifdef _WIN32
    if ( strlen( argv[0] ) < 4 || strncmpi( &argv[0][strlen( argv[0] ) - 4], ".exe", 4 ) )
    {
        arg0 = bgd_malloc( strlen( argv[0] ) + 5 );
        sprintf( arg0, "%s.exe", argv[0] );
    }
    else
    {
#endif
        arg0 = bgd_strdup( argv[0] );
#ifdef _WIN32
    }
#endif

    ptr = arg0 + strlen( arg0 );
    while ( ptr > arg0 && ptr[-1] != '\\' && ptr[-1] != '/' ) ptr-- ;

    appexename = bgd_strdup( ptr );

    /* get executable full pathname  */
    fp = NULL;
    appexefullpath = getfullpath( arg0 );
    if ( ( !strchr( arg0, '\\' ) && !strchr( arg0, '/' ) ) )
    {
        struct stat st;
        if ( stat( appexefullpath, &st ) || !S_ISREG( st.st_mode ) )
        {
            char *p = whereis( appexename );
            if ( p )
            {
                char * tmp = bgd_calloc( 1, strlen( p ) + strlen( appexename ) + 2 );
                bgd_free( appexefullpath );
                sprintf( tmp, "%s/%s", p, appexename );
                appexefullpath = getfullpath( tmp );
                bgd_free( tmp );
            }
        }
    }

    /* get pathname of executable */
    ptr = strstr( appexefullpath, appexename );
    appexepath = bgd_calloc( 1, ptr - appexefullpath + 1 );
    strncpy( appexepath, appexefullpath, ptr - appexefullpath );

    standalone = ( strncmpi( appexename, "bgdi", 4 ) == 0 ) ;

    /* add binary path */
    file_addp( appexepath );

    if ( !standalone )
    {
        /* Hand-made interpreter: search for DCB at EOF */
        fp = file_open( appexefullpath, "rb0" );
        if ( fp )
        {
            file_seek( fp, -( int )sizeof( dcb_signature ), SEEK_END );
            file_read( fp, &dcb_signature, sizeof( dcb_signature ) );

            if ( strcmp( dcb_signature.magic, DCB_STUB_MAGIC ) == 0 )
            {
                ARRANGE_DWORD( &dcb_signature.dcb_offset );
                embedded = 1;
            }
        }

        filename = appexefullpath;
    }

    if ( standalone )
    {
        /* Calling BGDI.EXE so we must get all command line params */

        for ( i = 1 ; i < argc ; i++ )
        {
            if ( argv[i][0] == '-' )
            {
                j = 1 ;
                while ( argv[i][j] )
                {
                    if ( argv[i][j] == 'd' ) debug++;
                    if ( argv[i][j] == 'i' )
                    {
                        if ( argv[i][j+1] == 0 )
                        {
                            if ( i == argc - 1 )
                            {
                                fprintf( stderr, "You must provide a directory" ) ;
                                exit( 0 );
                            }
                            file_addp( argv[i+1] );
                            i++ ;
                            break ;
                        }
                        file_addp( &argv[i][j + 1] ) ;
                        break ;
                    }
                    j++ ;
                }
            }
            else
            {
                if ( !filename )
                {
                    filename = argv[i] ;
                    if ( i < argc - 1 ) memmove( &argv[i], &argv[i+1], sizeof( char* ) * ( argc - i - 1 ) ) ;
                    argc-- ;
                    i-- ;
                }
            }
        }

        if ( !filename )
        {
            printf( BGDI_VERSION "\n"
                    "Bennu Game Development Interpreter\n"
                    "\n"
                    "Copyright (c) 2006-2019 SplinterGU (Fenix/BennuGD)\n"
                    "Copyright (c) 2002-2006 Fenix Team (Fenix)\n"
                    "Copyright (c) 1999-2002 José Luis Cebrián Pagüe (Fenix)\n"
                    "\n"
                    "Usage: %s [options] <data code block file>[.dcb]\n"
                    "\n"
                    "   -d       Activate DEBUG mode (several -d for increment debug level)\n"
                    "   -i dir   Adds the directory to the PATH\n"
                    "\n"
                    "This software is provided 'as-is', without any express or implied\n"
                    "warranty. In no event will the authors be held liable for any damages\n"
                    "arising from the use of this software.\n"
                    "\n"
                    "Permission is granted to anyone to use this software for any purpose,\n"
                    "including commercial applications, and to alter it and redistribute it\n"
                    "freely, subject to the following restrictions:\n"
                    "\n"
                    "   1. The origin of this software must not be misrepresented; you must not\n"
                    "   claim that you wrote the original software. If you use this software\n"
                    "   in a product, an acknowledgment in the product documentation would be\n"
                    "   appreciated but is not required.\n"
                    "\n"
                    "   2. Altered source versions must be plainly marked as such, and must not be\n"
                    "   misrepresented as being the original software.\n"
                    "\n"
                    "   3. This notice may not be removed or altered from any source\n"
                    "   distribution.\n"
                    , appexename ) ;

            ret = -1;
            goto exit;
        }
    }

    /* Initialization (modules needed before dcb_load) */

    bennugd_internal_string_init() ;
    init_c_type() ;

    /* Init application title for windowed modes */

    strcpy( dcbname, filename ) ;

    ptr = filename + strlen( filename );
    while ( ptr > filename && ptr[-1] != '\\' && ptr[-1] != '/' ) ptr-- ;

    appname = bgd_strdup( ptr ) ;
    if ( strlen( appname ) > 3 )
    {
        char ** dcbext = dcb_exts, *ext = &appname[ strlen( appname ) - 4 ];
#if defined _WIN32 || defined LIBRETRO_CORE
        if ( !strncmpi( ext, ".exe", 4 ) )
        {
            *ext = '\0';
        }
        else
#endif
        while ( dcbext && *dcbext )
        {
            if ( !strncmpi( ext, *dcbext, 4 ) )
            {
                *ext = '\0';
                break;
            }
            dcbext++;
        }
    }

#ifdef __DEBUG__
printf( "appname        %s\n", appname);
printf( "appexename     %s\n", appexename);
printf( "appexepath     %s\n", appexepath);
printf( "appexefullpath %s\n", appexefullpath);
printf( "dcbname        %s\n", dcbname);
fflush(stdout);
#endif

    if ( !embedded )
    {
        /* First try to load directly (we expect myfile.dcb) */
        if ( !dcb_load( dcbname ) )
        {
            char ** dcbext = dcb_exts;
            int dcbloaded = 0;

            while ( dcbext && *dcbext )
            {
                strcpy( dcbname, appname ) ;
                strcat( dcbname, *dcbext ) ;
                if (( dcbloaded = dcb_load( dcbname ) ) ) break;
                dcbext++;
            }

            if ( !dcbloaded )
            {
                printf( "%s: doesn't exist or isn't version %d DCB compatible\n", filename, DCB_VERSION >> 8 ) ;
                ret = -1;
                goto exit;
            }
        }
    }
    else
    {
        dcb_load_from( fp, (const char *) dcbname, dcb_signature.dcb_offset );
    }

    /* If the dcb is not in debug mode */

    if ( dcb.data.NSourceFiles == 0 ) debug = 0;

    /* Initialization (modules needed after dcb_load) */

    /* config file name */

    configfile = bgd_calloc( 1, strlen( appname ) + 4 + 1 );
    sprintf( configfile, "%s.ini", appname );

    sysproc_init() ;

#ifdef _WIN32
    HWND hWnd = /*GetForegroundWindow()*/ GetConsoleWindow();
    DWORD dwProcessId;
    GetWindowThreadProcessId( hWnd, &dwProcessId );
    if ( dwProcessId == GetCurrentProcessId() ) ShowWindow( hWnd, SW_HIDE );
#endif

    argv[0] = filename;
    bgdrtm_entry( argc, argv );

    if ( mainproc )
    {
        (void) instance_new( mainproc, NULL ) ;
        ret = instance_go_all() ;
    }

    bgdrtm_exit( ret );

exit:
    file_close(fp);
    bgd_free( configfile        );
    bgd_free( appexename        );
    bgd_free( appexepath        );
    bgd_free( appexefullpath    );
    bgd_free( appname           );

    return ret;
}
