/* ******************************************** */
/* File Name: log.h                             */
/* Author: (C) 2007-2009 - Juan José Ponteprino */
/* Description: Loggin API Headers              */
/* License: This code is licenced as LGPL       */
/* ******************************************** */

#ifndef _LOG_H
#define _LOG_H

#include <time.h>

/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* Core                                                 */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */

#define LOG_INIT_INTERNAL_BUFFER_SIZE   16384

enum {
    LOG_PRIO_EMERGENCY  = 0      ,  /* el sistema está inutilizable */
    LOG_PRIO_ALERT               ,  /* debe tomarse una acción correctora inmediatamente */
    LOG_PRIO_CRITICAL            ,  /* condiciones críticas */
    LOG_PRIO_ERROR               ,  /* condiciones de error */
    LOG_PRIO_WARNNING            ,  /* condiciones de advertencia */
    LOG_PRIO_NOTICE              ,  /* condición normal, pero significativa */
    LOG_PRIO_INFO                ,  /* mensaje informativo */
    LOG_PRIO_DEBUG               ,  /* mensaje del nivel de depuración */
    LOG_PRIO_NODEF               ,  /* No definido */
    LOG_PRIO_MAX = LOG_PRIO_NODEF   /* valor maximo permitido ( - 1 ) */
};

typedef struct
{
    void * privData; /* Internal Use */

    char * internalBuffer;

    void * ( * log_open )( char * name, int flags );
    void ( * log_close )( void * privData );
    void ( * log_write )( void * privData, int priority, char * message );
    void ( * log_control )( void * privData, long kid, char * args );
} log_t;

/* ---------------------------------------------------- */
/* Control format                                       */
/* --------------                                       */
/* 0xBBUUUUUU                                           */
/* BB = Internal backend id ( 0xff - core control )     */
/* UUUUUU = User data                                   */
/* ---------------------------------------------------- */

#define LOG_SYSTEM_CTRL         0xFF000000

#define LOG_LEVEL               0xFF000001

/* ---------------------------------------------------- */

int log_open( log_t * backend, char * name, int flags );
void log_close( log_t * backend );
void log_write( log_t * backend, int priority, char * format, ... );
void log_control( log_t * backend, long kid, char * args );
FILE * log_get_handle( log_t * backend );

/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* Default backend                                      */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */

/* controls keyword id for file backend */

#define LOG_FILE_ROLL_NOW       0x00000001
#define LOG_FILE_GET_FD         0x00000002

/* flags for file backend */

#define LOG_FILE_ROLL_SIZE_MASK 0x000007FF /* Max is 2047MB, 0 is disable size roll */
#define LOG_FILE_ROLL_DAY       0x00001000
#define LOG_FILE_DONT_FLUSH     0x00002000

#define LOG_FILE_DISPLAY_FNAME  0x00010000

/* file backend internal struct */

typedef struct
{
    char * fullpathname;
    char * filename;
    FILE * handle;
    int flags;
    struct tm lastTime;
} log_be_file_t;

/* file backend functions */

log_t * log_file_be_create();
void * log_file_open( char * name, int flags );
void log_file_close( void * privData );
void log_file_write( void * privData, int priority, char * message );
void log_file_control( void * privData, long kid, char * args );

/* ---------------------------------------------------- */

#endif
