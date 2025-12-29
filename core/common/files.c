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

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include "files.h"

#if LIBRETRO_CORE

extern struct RFILE * fopen_libretro ( const char * filename, const char * mode );
extern int fclose_libretro( struct RFILE* file);
extern int feof_libretro(struct RFILE *stream);
extern int fflush_libretro(struct RFILE *stream);
extern size_t fread_libretro(void *ptr, size_t size, size_t nmemb, struct RFILE *stream);
extern int fseek_libretro(struct RFILE *stream, long int offset, int whence);
extern long int ftell_libretro(struct RFILE *stream);
extern size_t fwrite_libretro(const void *ptr, size_t size, size_t nmemb, struct RFILE *stream);
extern int remove_libretro(const char *filename);
extern int rename_libretro(const char *old_filename, const char *new_filename);
extern char *fgets_libretro(char *str, int n, struct RFILE *stream);

extern struct gzFile_libretro* gzopen_libretro(const char* path, const char* mode);
extern int gzclose_libretro(struct gzFile_libretro* file);
extern int64_t gzread_libretro(struct gzFile_libretro* file, void* buf, size_t len);
extern int gzeof_libretro(struct gzFile_libretro* file);
extern int64_t gzwrite_libretro(struct gzFile_libretro* file, const void* buf, size_t len);
extern long int gztell_libretro (struct gzFile_libretro* file );
extern long int gzseek_libretro(struct gzFile_libretro* file, long int offset, int whence);
extern int gzrewind_libretro(struct gzFile_libretro* file);
extern char * gzgets_libretro(struct gzFile_libretro* file, char *buf, size_t len);

const char* resolve_bgd_path(const char * dir);

#ifdef feof
#undef feof
#endif

#define fopen(filename,mode)                            fopen_libretro(filename, mode)
#define fclose(stream)                                  fclose_libretro(stream)
#define feof(stream)                                    feof_libretro(stream)
#define fflush(stream)                                  fflush_libretro(stream)
#define fread(ptr,size,nmemb,stream)                    fread_libretro(ptr,size,nmemb,stream)
#define fseek(stream,offset,whence)                     fseek_libretro(stream,offset,whence)
#define ftell(stream)                                   ftell_libretro(stream)
#define fwrite(ptr,size,nmemb,stream)                   fwrite_libretro(ptr,size,nmemb,stream)
#define remove(filename)                                remove_libretro(filename)
#define rename(old_filename,new_filename)               rename_libretro(old_filename,new_filename)
#define fgets(str, n, stream)                           fgets_libretro(str, n, stream)

#define gzopen(path, mode)                              gzopen_libretro(path,mode)
#define gzclose(stream)                                 gzclose_libretro(stream)
#define gzread(stream,buf,len)                          gzread_libretro(stream,buf,len)
#define gzeof(stream)                                   gzeof_libretro(stream)
#define gzwrite(stream,buf,len)                         gzwrite_libretro(stream, buf,len)
#define gztell(stream)                                  gztell_libretro(stream)
#define gzseek(stream,offset,whence)                    gzseek_libretro(stream,offset,whence)
#define gzrewind(stream)                                gzrewind_libretro(stream)
#define gzgets(stream, buf, len)                        gzgets_libretro(stream,buf,len)

#endif


#define MAX_POSSIBLE_PATHS  128

char * possible_paths[MAX_POSSIBLE_PATHS] = { NULL } ;

int opened_files = 0;

typedef struct
{
    char * stubname ;
    char * name ;
    int  offset ;
    int  size ;
#if LIBRETRO_CORE
    struct RFILE* fp;
#else
    FILE *  fp ;
#endif 
}
XFILE ;

XFILE * x_file = NULL ;
int max_x_files = 0;

int x_files_count = 0 ;

/* Add new file to PATH */

void xfile_init( int maxfiles )
{
    x_file = ( XFILE * ) bgd_calloc( sizeof( XFILE ), maxfiles );
    max_x_files = maxfiles;
}

void file_add_xfile( file * fp, const char * stubname, long offset, char * name, int size )
{
    char * ptr ;

    assert( x_files_count < max_x_files ) ;
    assert( fp->type == F_FILE ) ;

    x_file[x_files_count].stubname = bgd_strdup( stubname );
    x_file[x_files_count].fp = fp->fp ;
    x_file[x_files_count].offset = offset ;
    x_file[x_files_count].size = size ;
    x_file[x_files_count].name = bgd_strdup( name ) ;

    ptr = x_file[x_files_count].name;
    while ( *ptr )
    {
        if ( *ptr == '\\' ) *ptr = '/'; /* Unix style */
        ptr++;
    }

    x_files_count++ ;
}

/* Read a datablock from file */

int file_read( file * fp, void * buffer, int len )
{
    assert( len != 0 );

    if ( fp->type == F_XFILE )
    {
        XFILE * xf ;
        int result ;

        xf = &x_file[fp->n] ;

        if (( len + fp->pos ) > ( xf->offset + xf->size ) )
        {
            fp->eof = 1 ;
            len = xf->size + xf->offset - fp->pos ;
        }

        fseek( fp->fp, fp->pos, SEEK_SET ) ;
        result = fread( buffer, 1, len, fp->fp ) ;

        fp->pos = ftell( fp->fp ) ;
        return result ;
    }

#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE )
    {
        int result = gzread( fp->gz, buffer, len ) ;
        fp->error = ( result < len );
        if ( result < 0 ) result = 0;
        return result ;
    }
#endif

    return fread( buffer, 1, len, fp->fp ) ;
}

/* Save a unquoted string to a file */

int file_qputs( file * fp, char * buffer )
{
    char dest[1024], * optr ;
    const char * ptr ;

    ptr = buffer ;
    optr = dest ;
    while ( *ptr )
    {
        if ( optr > dest + 1000 )
        {
            *optr++ = '\\' ;
            *optr++ = '\n' ;
            *optr   = 0 ;
            file_write( fp, dest, optr - dest ) ;
            optr = dest ;
        }
        if ( *ptr == '\n' )
        {
            *optr++ = '\\' ;
            *optr++ = 'n' ;
            ptr++ ;
            continue ;
        }
        if ( *ptr == '\\' )
        {
            *optr++ = '\\' ;
            *optr++ = *ptr++ ;
            continue ;
        }
        *optr++ = *ptr++ ;
    }
    *optr++ = '\n' ;
    return file_write( fp, dest, optr - dest ) ;
}

/* Load a string from a file and unquoted it */

int file_qgets( file * fp, char * buffer, int len )
{
    char * ptr, * result = NULL ;
    size_t sz;

    if ( fp->type == F_XFILE )
    {
        XFILE * xf ;
        int l = 0;
        char * ptr = result = buffer ;

        xf = &x_file[fp->n] ;

        fseek( fp->fp, fp->pos, SEEK_SET ) ;
        while ( l < len )
        {
            if ( fp->pos >= xf->offset + xf->size )
            {
                fp->eof = 1 ;
                break ;
            }
            sz = fread( ptr, 1, 1, fp->fp ) ;
            if ( sz <= 0 ) {
                if ( feof( fp->fp ) ) fp->eof = 1;
                break;
            }
            l++ ;
            fp->pos++ ;
            if ( *ptr++ == '\n' ) break ;
        }
        *ptr = 0 ;
        fp->pos = ftell( fp->fp ) ;

        if ( l == 0 ) return 0 ;
    }
#ifndef NO_ZLIB
    else if ( fp->type == F_GZFILE )
    {
        result = gzgets( fp->gz, buffer, len ) ;
    }
#endif
    else
    {
        result = fgets( buffer, len, fp->fp );
    }

    if ( result == NULL )
    {
        buffer[0] = 0 ; return 0 ;
    }

    ptr = buffer ;
    while ( *ptr )
    {
        if ( *ptr == '\\' )
        {
            if ( ptr[1] == 'n' ) ptr[1] = '\n' ;
            strcpy( ptr, ptr + 1 ) ;
            ptr++ ;
            continue ;
        }
        if ( *ptr == '\n' )
        {
            *ptr = 0 ;
            break ;
        }
        ptr++ ;
    }
    return strlen( buffer ) ;
}

/* Save a string to file */

int file_puts( file * fp, char * buffer )
{
    return file_write( fp, buffer, strlen( buffer ) ) ;
}

/* Load a string from a file and unquoted it */

int file_gets( file * fp, char * buffer, int len )
{
    char * result = NULL ;
    size_t sz;

    if ( fp->type == F_XFILE )
    {
        XFILE * xf ;
        int l = 0;
        char * ptr = result = buffer ;

        xf = &x_file[fp->n] ;

        fseek( fp->fp, fp->pos, SEEK_SET ) ;
        while ( l < len )
        {
            if ( fp->pos >= xf->offset + xf->size )
            {
                fp->eof = 1 ;
                break ;
            }
            sz = fread( ptr, 1, 1, fp->fp ) ;
            if ( sz <= 0 ) {
                if ( feof( fp->fp ) ) fp->eof = 1;
                break;
            }
            l++ ;
            fp->pos++ ;
            if ( *ptr++ == '\n' ) break ;
        }
        *ptr = 0 ;
        fp->pos = ftell( fp->fp ) ;

        if ( l == 0 ) return 0 ;
    }
#ifndef NO_ZLIB
    else if ( fp->type == F_GZFILE )
    {
        result = gzgets( fp->gz, buffer, len ) ;
    }
#endif
    else
    {
        result = fgets( buffer, len, fp->fp );
    }

    if ( result == NULL )
    {
        buffer[0] = 0 ;
        return 0 ;
    }

    return strlen( buffer ) ;
}

/* Save an int data to a binary file */

int file_writeSint8( file * fp, int8_t * buffer )
{
    return file_write( fp, buffer, sizeof( uint8_t ) );
}

int file_writeUint8( file * fp, uint8_t * buffer )
{
    return file_write( fp, buffer, sizeof( uint8_t ) );
}

int file_writeSint16( file * fp, int16_t * buffer )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_write( fp, buffer, sizeof( int16_t ) );
#else
    file_write( fp, ( uint8_t * )buffer + 1, 1 );
    return file_write( fp, ( uint8_t * )buffer, 1 );
#endif
}

int file_writeUint16( file * fp, uint16_t * buffer )
{
    return file_writeSint16( fp, ( int16_t * ) buffer );
}

int file_writeSint32( file * fp, int32_t * buffer )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_write( fp, buffer, sizeof( int32_t ) );
#else
    file_write( fp, ( uint8_t * )buffer + 3, 1 );
    file_write( fp, ( uint8_t * )buffer + 2, 1 );
    file_write( fp, ( uint8_t * )buffer + 1, 1 );
    return file_write( fp, ( uint8_t * )buffer, 1 );
#endif
}

int file_writeUint32( file * fp, uint32_t * buffer )
{
    return file_writeSint32( fp, ( int32_t * )buffer );
}

/* Save an array to a binary file */

int file_writeSint8A( file * fp, int8_t * buffer, int n )
{
    return file_write( fp, buffer, n );
}

int file_writeUint8A( file * fp, uint8_t * buffer, int n )
{
    return file_write( fp, buffer, n );
}

int file_writeSint16A( file * fp, int16_t * buffer, int n )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_write( fp, buffer, n << 1 ) >> 1 ;
#else
    int i;
    for ( i = 0; i < n; i++ )
        if ( !file_writeSint16( fp, ( int16_t * )buffer++ ) ) break;
    return i;
#endif
}

int file_writeUint16A( file * fp, uint16_t * buffer, int n )
{
    return file_writeSint16A( fp, ( int16_t * )buffer, n );
}

int file_writeSint32A( file * fp, int32_t * buffer, int n )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_write( fp, buffer, n << 2 ) >> 2;
#else
    int i;
    for ( i = 0; i < n; i++ )
        if ( !file_writeSint32( fp, ( int32_t * )buffer++ ) ) break;
    return i;
#endif
}

int file_writeUint32A( file * fp, uint32_t * buffer, int n )
{
    return file_writeSint32A( fp, ( int32_t * )buffer, n );
}

/* Read an int data from a binary file */

int file_readSint8( file * fp, int8_t * buffer )
{
    return file_read( fp, buffer, sizeof( int8_t ) );
}

int file_readUint8( file * fp, uint8_t * buffer )
{
    return file_read( fp, buffer, sizeof( int8_t ) );
}

int file_readSint16( file * fp, int16_t * buffer )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_read( fp, buffer, sizeof( int16_t ) );
#else
    file_read( fp, ( uint8_t * )buffer + 1, 1 );
    return file_read( fp, ( uint8_t * )buffer, 1 );
#endif
}

int file_readUint16( file * fp, uint16_t * buffer )
{
    return file_readSint16( fp, (int16_t *)buffer );
}

int file_readSint32( file * fp, int32_t * buffer )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_read( fp, buffer, sizeof( int32_t ) );
#else
    file_read( fp, ( uint8_t * )buffer + 3, 1 );
    file_read( fp, ( uint8_t * )buffer + 2, 1 );
    file_read( fp, ( uint8_t * )buffer + 1, 1 );
    return file_read( fp, ( uint8_t * )buffer, 1 );
#endif
}

int file_readUint32( file * fp, uint32_t * buffer )
{
    return file_readSint32( fp, ( int32_t * )buffer );
}

/* Read an array from a binary file */

int file_readSint8A( file * fp, int8_t * buffer, int n )
{
    return file_read( fp, buffer, n );
}

int file_readUint8A( file * fp, uint8_t * buffer, int n )
{
    return file_read( fp, buffer, n );
}

int file_readSint16A( file * fp, int16_t * buffer, int n )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_read( fp, buffer, n << 1 ) >> 1;
#else
    int i;
    for ( i = 0; i < n; i++ )
        if ( !file_readSint16( fp, ( int16_t * )buffer++ ) ) break;
    return i;
#endif
}

int file_readUint16A( file * fp, uint16_t * buffer, int n )
{
    return file_readSint16A( fp, ( int16_t * )buffer, n );
}

int file_readSint32A( file * fp, int32_t * buffer, int n )
{
#if __BYTEORDER == __LIL_ENDIAN
    return file_read( fp, buffer, n << 2 ) >> 2;
#else
    int i;
    for ( i = 0; i < n; i++ )
        if ( !file_readSint32( fp, ( int32_t * )buffer++ ) ) break;
    return i;
#endif
}

int file_readUint32A( file * fp, uint32_t * buffer, int n )
{
    return file_readSint32A( fp, ( int32_t * )buffer, n );
}


/* Write a datablock to a file */

int file_write( file * fp, void * buffer, int len )
{
    if ( fp->type == F_XFILE )
    {
        XFILE * xf ;
        int result ;

        xf = &x_file[fp->n] ;

        if (( len + fp->pos ) > ( xf->offset + xf->size ) )
        {
            fp->eof = 1 ;
            len = xf->size + xf->offset - fp->pos ;
        }
        fseek( fp->fp, fp->pos, SEEK_SET ) ;
        result = fwrite( buffer, 1, len, fp->fp ) ;
        fp->pos = ftell( fp->fp ) ;
        return result ;
    }

#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE )
    {
        int result = gzwrite( fp->gz, buffer, len ) ;
        if (( fp->error = ( result < 0 ) ) != 0 ) result = 0 ;
        return ( result < len ) ? 0 : len ;
    }
#endif

    return fwrite( buffer, 1, len, fp->fp ) ;
}

/* Return file size */

int file_size( file * fp )
{
    long pos, size ;

    if ( fp->type == F_XFILE ) return x_file[fp->n].size ;

    pos = file_pos( fp );
#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE )
    {
        char buffer[8192];
        size = pos;
        while ( !file_eof( fp ) ) size += file_read( fp, buffer, sizeof(buffer) );
    }
    else
#endif
    {
        file_seek(fp, 0, SEEK_END ) ;
        size = file_pos( fp ) ;
    }
    file_seek(fp, pos, SEEK_SET ) ;

    return size ;
}

/* Get current file pointer position */

int file_pos( file * fp )
{
    if ( fp->type == F_XFILE ) return fp->pos - x_file[fp->n].offset ;

#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE ) return gztell( fp->gz ) ;
#endif

    return ftell( fp->fp ) ;
}

int file_flush( file * fp )
{
    if ( fp->type == F_XFILE ) return 0 ;

#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE ) return 0 ;
#endif

    return fflush( fp->fp ) ;
}

/* Set current file pointer position */

int file_seek( file * fp, int pos, int where )
{
    assert( fp );
    if ( fp->type == F_XFILE )
    {
        if ( where == SEEK_END )
            pos = x_file[fp->n].size - pos + 1 ;
        else if ( where == SEEK_CUR )
            pos += ( fp->pos - x_file[fp->n].offset );

        if ( x_file[fp->n].size < pos ) pos = x_file[fp->n].size ;

        if ( pos < 0 ) pos = 0 ;

        fp->pos = pos + x_file[fp->n].offset ;
        return pos ;
    }

#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE )
    {
        assert( fp->gz );
        return gzseek( fp->gz, pos, where ) ;
    }
#endif

    assert( fp->fp );
    return fseek( fp->fp, pos, where ) ;
}

void file_rewind( file * fp )
{
    fp->error = 0;

    switch ( fp->type )
    {
        case F_XFILE:
            fp->pos = x_file[fp->n].offset ;
            break;

#ifndef NO_ZLIB
        case F_GZFILE:
            gzrewind( fp->gz ) ;
            break;
#endif

        default:
            fseek(fp->fp, 0L, SEEK_SET) ;
    }
}

// Test case path here
#if !defined(_WIN32)
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

static const char* casepath(char const *path, size_t start_offset)
{
    struct stat statbuf;
    if (stat(path, &statbuf)==0)
    {
        return path;
    }

    _Thread_local static char buffer[4096];

    size_t l = strlen(path);
    if (start_offset>l )
    {
        return path;
    }

    
    char* dest = buffer + start_offset;
    

    const char* current = path+start_offset;
    // hack for now
    if (*path!='/')
    {
        buffer[0]='.';
        buffer[1]='/';
        strncpy(buffer+2, path, start_offset);
        dest += 2;
        *dest = 0;
    }
    else
    {
        strncpy(buffer, path, start_offset);
        *dest = 0;
    }
    // 

    
    while(*current)
    {
        DIR* dir = opendir(buffer);
        if (!dir)
        {
            return path;
        }

        const char* next=current;
        
        while(*next && *next!='/' && *next!='\\') ++next;
        const size_t element_length=next-current;
        
        strncpy(dest, current, element_length);
        dest[element_length]=0;

        struct dirent *entry;
        for (entry = readdir(dir); entry!=NULL; entry = readdir(dir))
        {
            if (strcasecmp(dest, entry->d_name) == 0)
            {
                strncpy(dest, entry->d_name, element_length);
                dest+=element_length;
                break;
            }
        }

        closedir(dir);

        if (!entry)
        {            
            return path;
        }

        if (*next)
        {
            *dest = *next;
            ++next;
            ++dest;
            *dest=0;
        }

        current = next;
    }

    return buffer;
}
#else
static const char* casepath(char const *path, size_t start_offset)
{
    return path;
}
#endif
//

/* Open file */

static int open_raw( file * f, const char * filename, const char * mode )
{
#if !LIBRETRO_CORE
//     extern const char* libretro_adjustpath(const char*, int writable);
//     filename = libretro_adjustpath(filename, strpbrk(mode,"wa")!=NULL);
// #else
    filename = casepath(filename, 0);
#endif
    char    _mode[5];
    char    *p;

#ifndef NO_ZLIB
    if ( !strchr( mode, '0' ) )
    {
        f->type = F_GZFILE ;
        f->gz = gzopen( filename, mode ) ;
        f->eof  = 0 ;
        if ( f->gz ) return 1 ;
    }
#endif

    p = _mode;
    while ( *mode )
    {
        if ( *mode != '0' )
        {
            *p = *mode;
            p++;
        }
        mode++;
    }
    *p = '\0';

    f->eof  = 0 ;
    f->type = F_FILE ;
    f->fp = fopen( filename, _mode ) ;
    if ( f->fp ) return 1 ;
    return 0 ;
}

file * file_open( const char * filename, char * mode )
{
    char work [__MAX_PATH];
    char here [__MAX_PATH];

    char * name = NULL ;
    char * p, c ;
    int i;

    file * f ;

    f = ( file * ) bgd_calloc( 1, sizeof( file ) ) ;
    assert( f ) ;

    p = f->name;
    while ( *filename )
    {
        *p++ = *filename++;
        if ( p[-1] == '\\' ) p[-1] = '/'; /* Unix style */
    }
    p[0] = '\0';

    filename = f->name;

    if ( open_raw( f, filename, mode ) )
    {
        opened_files++;
        return f ;
    }


    /* if real file don't exists in disk */
    if (  strchr( mode, 'r' ) &&  strchr( mode, 'b' ) &&  /* Only read-only files */
         !strchr( mode, '+' ) && !strchr( mode, 'w' ) )
    {
        for ( i = 0 ; i < x_files_count ; i++ )
        {
            if ( strcmp( filename, x_file[i].name ) == 0 )
            {
                f->eof  = 0;
                f->pos  = x_file[i].offset;
                f->type = F_XFILE;
                f->n    = i;
                f->fp = fopen( x_file[i].stubname, "rb" );

                opened_files++;
                return f ;
            }
        }
    }

    p = name = work;
    while (( c = *filename ) )
    {
        if ( c == '\\' || c == '/' )
        {
            c = 0;
            name = p + 1;
        }
        *p++ = c;
        filename++;
    }
    *p = '\0';

    /* Use file extension for search in a directory named as it (for example: FPG dir for .FPG files) */
    if ( strchr( name, '.' ) )
    {
        strcpy( here, strrchr( name, '.' ) + 1 ) ;
        strcat( here, PATH_SEP ) ;
        strcat( here, name ) ;
        if ( open_raw( f, here, mode ) )
        {
            opened_files++;
            return f ;
        }
    }

    for ( i = 0 ; possible_paths[i] ; i++ )
    {
        strcpy( here, possible_paths[i] ) ;
        strcat( here, name ) ;
        if ( open_raw( f, here, mode ) )
        {
            opened_files++;
            return f ;
        }
    }

    bgd_free( f ) ;
    return 0 ;
}

/* Close file */

void file_close( file * fp )
{
    if ( fp == NULL ) return;
    if ( fp->type == F_FILE ) fclose( fp->fp ) ;
    if ( fp->type == F_XFILE ) fclose( fp->fp ) ;
#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE ) gzclose( fp->gz ) ;
#endif
    opened_files--;
    bgd_free( fp ) ;
}

/* Add a new dir to PATH */

void file_addp( const char * path )
{
    char truepath[__MAX_PATH];
    int n ;

    if ( !path || !*path ) return;

    strcpy( truepath, path ) ;

    for ( n = 0 ; truepath[n] ; n++ ) if ( truepath[n] == '\\' ) truepath[n] = '/' ;
    if ( truepath[strlen( truepath )-1] != '/' ) strcat( truepath, "/" ) ;

    for ( n = 0 ; n < MAX_POSSIBLE_PATHS - 1 && possible_paths[n] ; n++ ) ;

    possible_paths[n] = bgd_strdup( truepath ) ;
    possible_paths[n+1] = NULL ;
}

/* --- */

int file_remove( const char * filename )
{
    return ( remove( filename ) );
}

/* --- */

int file_move( const char * source_file, const char * target_file )
{
    return ( rename( source_file, target_file ) );
}

/* Check for file exists */

int file_exists( const char * filename )
{
    file * fp ;

    fp = file_open( filename, "rb0" ) ;
    if ( fp )
    {
        file_close( fp ) ;
        return 1 ;
    }
    return 0 ;
}

/* Check for file end is reached */

int file_eof( file * fp )
{
    if ( fp->type == F_XFILE )
    {
        return fp->eof ? 1 : 0;
    }

#ifndef NO_ZLIB
    if ( fp->type == F_GZFILE )
    {
        if ( fp->error ) return 1 ;
        return gzeof( fp->gz ) ? 1 : 0;
    }
#endif

    return feof( fp->fp ) ? 1 : 0;
}

/* Get the FILE * of the file */

// FILE * file_fp( file * f )
// {
//     if ( f->type == F_XFILE )
//     {
// //        XFILE * xf = &x_file[f->n] ;
//         fseek( f->fp, f->pos, SEEK_SET ) ;
//         return f->fp ;
//     }

//     return f->fp ;
// }

/* ------------------------------------------------------------------------------------ */

char * getfullpath( char *rel_path )
{    
#if LIBRETRO_CORE
    const char* fixed_path=resolve_bgd_path(rel_path);
    return bgd_strdup( fixed_path );
#else
    char fullpath[ __MAX_PATH ] = "";

#ifdef _WIN32
    char * fpath = NULL;
    DWORD sz = GetFullPathName( rel_path, sizeof( fullpath ), fullpath, NULL );
    if ( sz > sizeof( fullpath ) ) {
        fpath = bgd_malloc( sz + 1 );
        if ( fpath ) {
            if ( GetFullPathName( rel_path, sz, fpath, NULL ) ) return fpath;
            bgd_free( fpath );
        }
        return NULL;
    }
    if ( sz ) return bgd_strdup( fullpath );
    return NULL;
#else
    char * r = realpath( rel_path, fullpath );
    (void) &r; // avoid compiler warning
    if ( !*fullpath ) return NULL;
    return bgd_strdup( fullpath );
#endif
#endif
}

/* ------------------------------------------------------------------------------------ */

#ifdef _WIN32
    #define ENV_PATH_SEP    ';'    
#else
    #define ENV_PATH_SEP    ':'
#endif

char * whereis( char *file )
{
#if LIBRETRO_CORE
    return NULL;
#endif

    char * path = getenv( "PATH" ), *pact = path, *p;
    char fullname[ __MAX_PATH ];

    while ( pact && *pact )
    {
        struct stat st;

        if ( ( p = strchr( pact, ENV_PATH_SEP ) ) ) *p = '\0';
        sprintf( fullname, "%s%s%s", pact, ( pact[ strlen( pact ) - 1 ] == ENV_PATH_SEP ) ? "" : PATH_SEP, file );

        if ( !stat( fullname, &st ) && S_ISREG( st.st_mode ) )
        {
            pact = bgd_strdup( pact );
            if ( p ) *p = ENV_PATH_SEP;
            return ( pact );
        }

        if ( !p ) break;

        *p = ENV_PATH_SEP;
        pact = p + 1;
    }

    return NULL;
}

/* ------------------------------------------------------------------------------------ */
