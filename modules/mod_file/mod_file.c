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
#include <string.h>

#include "bgddl.h"

#include "arrange.h"
#include "files.h"
#include "xstrings.h"
#include "dcb.h"
#include "varspace_file.h"

/* ----------------------------------------------------------------- */

static int modfile_save( INSTANCE * my, int * params )
{
    file * fp ;
    const char * filename ;
    int result = 0 ;

    filename = string_get( params[0] ) ;
    if ( !filename ) return 0 ;

    fp = file_open( filename, "wb0" ) ;
    if ( fp )
    {
        result = savetypes( fp, ( void * )params[1], ( void * )params[2], params[3], 0 );
        file_close( fp ) ;
    }
    string_discard( params[0] ) ;
    return result ;
}

static int modfile_load( INSTANCE * my, int * params )
{
    file * fp ;
    const char * filename ;
    int result = 0 ;

    filename = string_get( params[0] ) ;
    if ( !filename ) return 0 ;

    fp = file_open( filename, "rb0" ) ;
    if ( fp )
    {
        result = loadtypes( fp, ( void * )params[1], ( void * )params[2], params[3], 0 );
        file_close( fp ) ;
    }
    string_discard( params[0] ) ;
    return result ;
}

static int modfile_fopen( INSTANCE * my, int * params )
{
    static char * ops[] = { "rb0", "r+b0", "wb0", "rb", "wb6" } ;
    int r ;

    if ( params[1] < 0 || params[1] > 4 )
        params[0] = 0 ;

    r = ( int ) file_open( string_get( params[0] ), ops[params[1]] ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modfile_fclose( INSTANCE * my, int * params )
{
    file_close(( file * )params[0] ) ;
    return 1 ;
}

static int modfile_fread( INSTANCE * my, int * params )
{
    return loadtypes(( file * )params[0], ( void * )params[1], ( void * )params[2], params[3], 0 );
}

static int modfile_fwrite( INSTANCE * my, int * params )
{
    return savetypes(( file * )params[0], ( void * )params[1], ( void * )params[2], params[3], 0 );
}

static int modfile_freadC( INSTANCE * my, int * params )
{
    return file_read(( file * )params[2], ( void * )params[0], params[1] );
}

static int modfile_fwriteC( INSTANCE * my, int * params )
{
    return file_write(( file * )params[2], ( void * )params[0], params[1] );
}

static int modfile_fseek( INSTANCE * my, int * params )
{
    return file_seek(( file * )params[0], params[1], params[2] ) ;
}

static int modfile_frewind( INSTANCE * my, int * params )
{
    file_rewind(( file * )params[0] ) ;
    return 1;
}

static int modfile_ftell( INSTANCE * my, int * params )
{
    return file_pos(( file * )params[0] ) ;
}

static int modfile_fflush( INSTANCE * my, int * params )
{
    return file_flush(( file * )params[0] ) ;
}

static int modfile_filelength( INSTANCE * my, int * params )
{
    return file_size(( file * )params[0] ) ;
}

static int modfile_fputs( INSTANCE * my, int * params )
{
    char * str = ( char * ) string_get( params[1] );
    int r = file_puts(( file * )params[0], str ) ;
    if ( str[strlen( str )-1] != '\n' ) file_puts(( file * )params[0], "\r\n" ) ;
    /*    int r = file_puts ((file *)params[0], string_get(params[1])) ; */
    string_discard( params[1] ) ;
    return r ;
}

static int modfile_fgets( INSTANCE * my, int * params )
{
    char buffer[1025] ;
    int len, done = 0 ;
    int str = string_new( "" );

    while ( !done )
    {
        len = file_gets(( file * )params[0], buffer, sizeof( buffer ) - 1) ;
        if ( len < 1 ) break;

        if ( buffer[len-1] == '\r' || buffer[len-1] == '\n' )
        {
            len--;
            if ( len && ( buffer[len-1] == '\r' || buffer[len-1] == '\n' )) len--;
            buffer[len] = '\0' ;
            done = 1;
        }
        string_concat( str, buffer );
    }
    string_use( str ) ;
    return str ;
}

static int modfile_file( INSTANCE * my, int * params )
{
    char buffer[1025] ;
    int str = string_new( "" ) ;
    file * f ;
    int l;

    f = file_open( string_get( params[0] ), "rb" ) ;
    string_discard( params[0] ) ;

    if ( f )
    {
        while ( !file_eof( f ) )
        {
            l = file_read( f, buffer, sizeof( buffer ) - 1 ) ;
            buffer[l] = '\0' ;
            if ( l )
            {
                string_concat( str, buffer ) ;
                buffer[0] = '\0' ;
            }
            else
                break;
        }
        file_close( f ) ;
    }

    string_use( str ) ;

    return str ;
}

static int modfile_feof( INSTANCE * my, int * params )
{
    return file_eof(( file * )params[0] ) ;
}

static int modfile_exists( INSTANCE * my, int * params )
{
    int r = file_exists( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modfile_remove( INSTANCE * my, int * params )
{
    int r = file_remove( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;
    return r ;
}

static int modfile_move( INSTANCE * my, int * params )
{
    int r = file_move( string_get( params[0] ), string_get( params[1] ) ) ;
    string_discard( params[1] ) ;
    string_discard( params[0] ) ;
    return r ;
}

/* ----------------------------------------------------------------- */
/* exports                                                           */
/* ----------------------------------------------------------------- */

#include "mod_file_exports.h"

/* ----------------------------------------------------------------- */
