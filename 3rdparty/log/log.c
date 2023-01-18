/* ******************************************** */
/* File Name: log.c                             */
/* Author: (C) 2007-2009 - Juan José Ponteprino */
/* Description: Loggin API                      */
/* License: This code is licenced as LGPL       */
/* ******************************************** */

#include <stdlib.h>
#include <stdio.h>
#if WIN32
#define vsnprintf _vsnprintf
#endif
#include <stdarg.h>
#include <string.h>

#include <log.h>

/* ---------------------------------------------------- */

static char * log_str_priority[] =
{
    "EMERG"     ,
    "ALERT"     ,
    "CRIT"      ,
    "ERR"       ,
    "WARN"      ,
    "NOTICE"    ,
    "INFO"      ,
    "DEBUG"     ,
    "N/D"
};

static int log_level = -1;

/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* Default Backend                                      */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */

static FILE * __log_file_roll( log_be_file_t * _privData )
{
    time_t  Moment = time( NULL );
    char    MomentStr[ 18 ];
    struct tm Now ;

    FILE  * filelog;
    char * nfilename = bgd_malloc( strlen( _privData->fullpathname ) + 16 );

#if WIN32
    struct tm * _Now = localtime( &Moment );
    Now = * _Now;
#else
    localtime_r( &Moment, &Now );
#endif

    fclose( _privData->handle );

    strftime( MomentStr, sizeof( MomentStr ), "%y%m%d%H%M%S", &Now );

    sprintf( nfilename, "%s.%s", _privData->fullpathname, MomentStr );

    rename( _privData->fullpathname, nfilename );
    bgd_free( nfilename );

    filelog = _privData->handle = fopen( _privData->fullpathname, "a" );
    if ( !filelog )
    {
        filelog = stdout;
    }

    return filelog;
}

/* ---------------------------------------------------- */

log_t * log_file_be_create()
{
    log_t * lt = bgd_malloc( sizeof( log_t ) );

    if ( !lt )
    {
        return NULL;
    }

    lt->log_open  = log_file_open;
    lt->log_close = log_file_close;
    lt->log_write = log_file_write;
    lt->log_control = log_file_control;

    return lt;
}

/* ---------------------------------------------------- */

void * log_file_open( char * name, int flags )
{
    char * p;

    log_be_file_t * privData = ( log_be_file_t * ) bgd_malloc( sizeof( log_be_file_t ) );
    if ( !privData )
    {
        return NULL;
    }

    privData->flags = flags;

    privData->fullpathname = bgd_strdup( name );
    if ( !privData->fullpathname )
    {
        bgd_free( privData );
        return NULL;
    }

    privData->filename = strrchr( privData->fullpathname, '/' );
    if (!privData->filename)
    {
        privData->filename = strrchr( privData->fullpathname, '\\' );
    }
    if ( !privData->filename )
        privData->filename = privData->fullpathname;
    else
        privData->filename++;

    privData->handle = fopen( name, "a" );
    if ( !privData->handle )
    {
        bgd_free( privData->fullpathname );
        bgd_free( privData );
        return NULL;
    }

    return privData;
}

/* ---------------------------------------------------- */

void log_file_close( void * privData )
{
    if ( privData && (( log_be_file_t * ) privData )->handle )
    {
        bgd_free((( log_be_file_t * ) privData )->fullpathname );
        fclose((( log_be_file_t * ) privData )->handle );
        bgd_free( privData );
    }
}

/* ---------------------------------------------------- */

void log_file_write( void * privData, int priority, char * message )
{
    int     doroll = 0;

    time_t  Moment = time( NULL );
    char    MomentStr[ 18 ];
    struct tm Now ;

    log_be_file_t  * _privData = privData;
    FILE  * filelog = _privData->handle;

#if WIN32
    struct tm * _Now = localtime( &Moment );
    Now = * _Now;
#else
    localtime_r( &Moment, &Now );
#endif

    if ( !filelog )
    {
        filelog = stdout;
    }

    if ( _privData->flags & LOG_FILE_ROLL_SIZE_MASK )
    {
        long pos = ftell( filelog );

        if ( pos / 1024 >= ( _privData->flags & LOG_FILE_ROLL_SIZE_MASK ) )
        {
            doroll = 1;
        }
    }

    if ( _privData->flags & LOG_FILE_ROLL_DAY )
    {
        /* If day of year is different of last log, then roll it
           (tm_mday is used only for check valid date in internal structs)
         */
        if ( Now.tm_yday != _privData->lastTime.tm_yday && _privData->lastTime.tm_mday != 0 )
        {
            doroll = 1;
        }
    }

    if ( doroll )
    {
        filelog = __log_file_roll( _privData );
    }

    if ( priority >= LOG_PRIO_MAX )
    {
        priority = LOG_PRIO_NODEF ;
    }

    if ( message && * message != '\0' )
    {
        strftime( MomentStr, sizeof( MomentStr ), "%y/%m/%d-%H:%M:%S", &Now );

        if ( _privData->flags & LOG_FILE_DISPLAY_FNAME )
            fprintf( filelog, "%s:%s %7.4f %s %s\n", _privData->filename, MomentStr, clock() / 1000.00, log_str_priority[ priority ], message );
        else
            fprintf( filelog, "%s %7.4f %s %s\n", MomentStr, clock() / 1000.00, log_str_priority[ priority ], message );

        if ( !( _privData->flags & LOG_FILE_DONT_FLUSH ) ) fflush( filelog );
    }

    _privData->lastTime = Now;
}

/* ---------------------------------------------------- */

void log_file_control( void * privData, long kid, char * args )
{
    switch ( kid )
    {
        case    LOG_FILE_ROLL_NOW:
            __log_file_roll(( log_be_file_t * ) privData );
            break;

        case    LOG_FILE_GET_FD:
            ( ( FILE ** ) args )[0] = ( FILE * )( ( ( log_be_file_t * ) privData )->handle );
            break;
    }
}

/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* Log core functions                                   */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */

int log_open( log_t * backend, char * name, int flags )
{
    if ( !backend || !backend->log_open )
    {
        return 0;
    }

    backend->internalBuffer = bgd_malloc( LOG_INIT_INTERNAL_BUFFER_SIZE );
    if ( !backend->internalBuffer )
    {
        return 0;
    }

    backend->privData = backend->log_open( name, flags );
    if ( !backend->privData )
    {
        bgd_free( backend->internalBuffer ) ;
        return 0;
    }

    return 1; /* OK */
}

/* ---------------------------------------------------- */

void log_close( log_t * backend )
{
    if ( backend && backend->privData && backend->log_close )
    {
        backend->log_close( backend->privData );
        if ( backend->internalBuffer )
        {
            bgd_free( backend->internalBuffer ) ;
        }
    }
}

/* ---------------------------------------------------- */

void log_write( log_t * backend, int priority, char * format, ... )
{
    /* Suponemos que no necesitamos más de 100 bytes. */
    int n;
    va_list ap;

    if ( !backend || !backend->privData || !backend->internalBuffer || !backend->log_write )
    {
        return;
    }

    if ( log_level != -1 && priority > log_level ) return;

    /* Intenta imprimir en el espacio reservado. */
    va_start( ap, format );
    n = vsnprintf( backend->internalBuffer, LOG_INIT_INTERNAL_BUFFER_SIZE, format, ap );
    va_end( ap );

    if ( n <= -1 || n > LOG_INIT_INTERNAL_BUFFER_SIZE )
    {
        int size = LOG_INIT_INTERNAL_BUFFER_SIZE * 2;
        char * p, * p1;
        if (( p = bgd_malloc( size ) ) == NULL )
        {
            return;
        }

        while ( 1 )
        {
            /* Intenta imprimir en el espacio reservado. */
            va_start( ap, format );
            n = vsnprintf( p, size, format, ap );
            va_end( ap );

            /* Si ha funcionado, devuelve la cadena. */
            if ( n > -1 && n < size )
            {
                break ;
            }

            /* Si no, inténtalo de nuevo con más espacio. */
            if ( n > -1 )  /* glibc 2.1 */
            {
                size = n + 1; /* exactamente lo que se necesita */
            }
            else           /* glibc 2.0 */
            {
                size *= 2;  /* el doble del tamaño anterior*/
            }

            if (( p1 = bgd_realloc( p, size ) ) == NULL )
            {
                bgd_free( p );
                return;
            }
            p = p1;
        }

        backend->log_write( backend->privData, priority, p );
        bgd_free( p );
    }
    else
    {
        backend->log_write( backend->privData, priority, backend->internalBuffer );
    }
}

/* ---------------------------------------------------- */

void log_control( log_t * backend, long kid, char * args )
{
    if ( !backend || !backend->privData || !backend->log_control )
    {
        return;
    }

    if ( ( kid & LOG_SYSTEM_CTRL ) == LOG_SYSTEM_CTRL )
    {
        switch ( kid )
        {
            case    LOG_LEVEL:
                log_level = ( int ) args;
                break;
        }
        return;
    }

    backend->log_control(( void * )( backend->privData ), kid, args );
}

/* ---------------------------------------------------- */

FILE * log_get_handle( log_t * backend )
{
    log_be_file_t * _privData;
    _privData = ( log_be_file_t * ) backend->privData;
    return ( FILE *) _privData->handle;
}

/* ---------------------------------------------------- */
