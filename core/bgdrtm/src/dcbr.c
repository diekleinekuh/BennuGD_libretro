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
#ifndef WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif
#include "bgdrtm.h"
#include "dcb.h"
#include "dirs.h"
#include "files.h"
#include "xstrings.h"

#define SYSPROCS_ONLY_DECLARE
#include "sysprocs.h"

#include "libretro.h"
extern retro_log_printf_t log_cb;
/* --------------------------------------------------------------------------- */

void * globaldata = 0 ;
void * localdata  = 0 ;
int    local_size = 0 ;
int  * localstr   = 0 ;
int    local_strings = 0 ;

PROCDEF * procs = NULL ;
PROCDEF * mainproc = NULL ;
int procdef_count = 0 ;

/* ---------------------------------------------------------------------- */

DCB_SYSPROC_CODE2 * sysproc_code_ref = NULL ;

/* ---------------------------------------------------------------------- */

void sysprocs_fixup( void )
{
    SYSPROC * proc = sysprocs ;
    DCB_SYSPROC_CODE2 * s = NULL ;
    int n;

    while ( proc->func )
    {
        proc->code = -1;

        s = sysproc_code_ref ;
        for ( n = 0; n < dcb.data.NSysProcsCodes; n++, s++ )
        {
            if (
                proc->type == s->Type && proc->params == s->Params &&
                s->Id == getid( proc->name ) && !strcmp( (const char *)s->ParamTypes, proc->paramtypes ) )
            {
                proc->code = s->Code ;
                break ;
            }
        }
        proc++ ;
    }
}

/* ---------------------------------------------------------------------- */

PROCDEF * procdef_get( int n )
{
    if ( n >= 0 && n < procdef_count )
        return &procs[n] ;

    return NULL ;
}

/* ---------------------------------------------------------------------- */

PROCDEF * procdef_get_by_name( char * name )
{
    int n;
    for ( n = 0 ; n < procdef_count; n++ )
        if ( strcmp( procs[n].name, name ) == 0 )
            return &procs[n];

    return NULL ;
}

/* ---------------------------------------------------------------------- */

DCB_HEADER dcb ;

/* ---------------------------------------------------------------------- */

static char * trim( char * ptr )
{
    char * ostr = ptr;
    char * bptr = ptr;

    while ( *ptr == ' ' || *ptr == '\n' || *ptr == '\r' || *ptr == '\t' ) ptr++;

    while ( *ptr ) *bptr++ = *ptr++;

    while ( bptr > ostr && ( bptr[-1] == ' ' || bptr[-1] == '\n' || bptr[-1] == '\r' || bptr[-1] == '\t' ) ) bptr--;

    *bptr = 0;

    return ( ostr );
}

/* ---------------------------------------------------------------------- */

static int load_file( const char * filename, int n )
{
    char line[2048] ;
    int allocated = 16 ;
    int count = 0 ;
    char ** lines ;
    file * fp ;

    fp = file_open( filename, "rb0" ) ;
    if ( !fp )
    {
        dcb.sourcefiles[n] = 0 ;
        dcb.sourcelines[n] = 0 ;
        dcb.sourcecount[n] = 0 ;
        return 0 ;
    }

    lines = ( char ** ) bgd_calloc( 16, sizeof( char* ) ) ;

    while ( !file_eof( fp ) )
    {
        file_qgets( fp, line, 2048 ) ;
        trim( line );
        if ( allocated == count )
        {
            allocated += 16 ;
            lines = bgd_realloc( lines, sizeof( char* ) * allocated ) ;
        }
        lines[count++] = bgd_strdup( line ) ;
    }
    file_close( fp ) ;

    dcb.sourcefiles[n] = ( uint8_t * ) bgd_strdup( filename );
    dcb.sourcelines[n] = ( uint8_t ** ) lines ;
    dcb.sourcecount[n] = count ;
    return 1 ;
}

/* ---------------------------------------------------------------------- */

int dcb_load( const char * filename )
{
    file * fp ;

    /* check for existence of the DCB FILE */
    if ( !file_exists( filename ) ) return 0 ;

    fp = file_open( filename, "rb0" ) ;
    if ( !fp )
    {
        fprintf( stderr, "ERROR: Runtime error - Could not open file (%s)\n", filename ) ;
        exit( 1 );
    }

    return dcb_load_from( fp, filename, 0 );
}

/* ---------------------------------------------------------------------- */

DCB_VAR * read_and_arrange_varspace( file * fp, int count )
{
    int n, n1;
    DCB_VAR * vars = ( DCB_VAR * ) bgd_calloc( count, sizeof( DCB_VAR ) ) ;

    for ( n = 0; n < count; n++ )
    {
        file_read( fp, &vars[n], sizeof( DCB_VAR ) ) ;
        ARRANGE_DWORD( &vars[n].ID );
        ARRANGE_DWORD( &vars[n].Offset );
        ARRANGE_DWORD( &vars[n].Varspace );
        for ( n1 = 0; n1 < MAX_TYPECHUNKS; n1++ ) ARRANGE_DWORD( &vars[n].Type.Count[n1] );
        ARRANGE_DWORD( &vars[n].Type.Members );
    }

    return vars;
}

//#if __BYTEORDER == __BIG_ENDIAN
static uint32_t fix_endianess_type(uint8_t* dst, DCB_TYPEDEF* var);
static void fix_endianess_data(int* data, DCB_VAR* variables, int count)
{
    for (int i=0; i<count; ++i)
    {
        fix_endianess_type((uint8_t*)data + variables[i].Offset, &variables[i].Type);
    }
}
static uint32_t fix_endianess_types( uint8_t * dst, DCB_TYPEDEF * var, int nvars)
{
    int result = 0;
    int partial;
    DCB_TYPEDEF * _var = var;
    int _nvars = nvars ;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = fix_endianess_type( dst, var );
        result += partial;
        dst += partial;
    }
    return result;
}

static int fix_endianess_vars( void * dst, DCB_VAR * vars, int nvars )
{
    int result = 0;
    int partial;

    // for ( ; nvars > 0; nvars--, var++ )
    // {
    //     partial = fix_endianess_type( dst, &var->Type );
    //     dst += partial;
    //     result += partial;
    // }

    for ( int n=0; n < nvars; ++n)
    {        
        partial = fix_endianess_type((uint8_t*)dst + vars[n].Offset, &vars[n].Type );
        result += partial;
    }

    return result;
}

static inline void SwapBytes(uint8_t* left, uint8_t* right)
{
    uint8_t tmp=*left;
    *left=*right;
    *right=tmp;
}

static uint32_t fix_endianess_type(uint8_t* dst, DCB_TYPEDEF* var)
{

#define SWAP_DWORD(a) { SwapBytes(a, a+3); SwapBytes(a+1, a+2); }
#define SWAP_WORD(a)  { SwapBytes(a, a+1); }
#define SWAP_DWORDS(a,n) { for(int i=0; i<n; ++i) { SWAP_DWORD(a+n*4)}  }
#define SWAP_WORDS(a,n)  { for(int i=0; i<n; ++i) { SWAP_WORD(a+n*2) }  }

    int count  = 1;
    int result = 0;
    int n      = 0;

    for ( ;; )
    {
        switch ( var->BaseType[n] )
        {
            case TYPE_FLOAT:
            case TYPE_INT:
            case TYPE_DWORD:
            case TYPE_POINTER:
                SWAP_DWORDS(dst, count);
                return 4 * count;

            case TYPE_WORD:
            case TYPE_SHORT:
                SWAP_WORDS(dst, count);
                return 2 * count;

            case TYPE_BYTE:
            case TYPE_SBYTE:
            case TYPE_CHAR:
                return count;

            case TYPE_STRING:
                while ( count-- )
                {
                    SWAP_DWORD(dst);
                    dst = dst +4;
                    result += 4;
                }
                return result;

            case TYPE_ARRAY:
                count *= var->Count[n];
                n++;
                continue;

            case TYPE_STRUCT:
                for ( ; count ; count-- )
                {
                    int partial = fix_endianess_vars( dst, dcb.varspace_vars[var->Members], dcb.varspace[var->Members].NVars );
                    dst = (( uint8_t* )dst ) + partial;
                    result += partial;
                }
                break;

            default:
                fprintf( stderr, "ERROR: Runtime error - Could not fix endianess datatype\n" ) ;
                exit( 1 );
                break;
        }
        break;
    }
    return result;
}
//#endif
/* ---------------------------------------------------------------------- */

int dcb_load_from( file * fp, const char * filename, int offset )
{
    log_cb(RETRO_LOG_DEBUG, "dcb_load_from\n");
    unsigned int n ;
    uint32_t size;

    /* Lee el contenido del fichero */

    file_seek( fp, offset, SEEK_SET );
    memset(&dcb, 0, sizeof( DCB_HEADER_DATA ));
    file_read( fp, &dcb, sizeof( DCB_HEADER_DATA ) ) ;    

    ARRANGE_DWORD( &dcb.data.Version );
    ARRANGE_DWORD( &dcb.data.NProcs );
    ARRANGE_DWORD( &dcb.data.NFiles );
    ARRANGE_DWORD( &dcb.data.NID );
    ARRANGE_DWORD( &dcb.data.NStrings );
    ARRANGE_DWORD( &dcb.data.NLocVars );
    ARRANGE_DWORD( &dcb.data.NLocStrings );
    ARRANGE_DWORD( &dcb.data.NGloVars );
    ARRANGE_DWORD( &dcb.data.NVarSpaces );

    ARRANGE_DWORD( &dcb.data.SGlobal );
    ARRANGE_DWORD( &dcb.data.SLocal );
    ARRANGE_DWORD( &dcb.data.SText );

    ARRANGE_DWORD( &dcb.data.NImports );

    ARRANGE_DWORD( &dcb.data.NSourceFiles );

    ARRANGE_DWORD( &dcb.data.NSysProcsCodes );

    ARRANGE_DWORD( &dcb.data.OProcsTab );
    ARRANGE_DWORD( &dcb.data.OID );
    ARRANGE_DWORD( &dcb.data.OStrings );
    ARRANGE_DWORD( &dcb.data.OText );
    ARRANGE_DWORD( &dcb.data.OGlobal );
    ARRANGE_DWORD( &dcb.data.OGloVars );
    ARRANGE_DWORD( &dcb.data.OLocal );
    ARRANGE_DWORD( &dcb.data.OLocVars );
    ARRANGE_DWORD( &dcb.data.OLocStrings );
    ARRANGE_DWORD( &dcb.data.OVarSpaces );
    ARRANGE_DWORD( &dcb.data.OFilesTab );
    ARRANGE_DWORD( &dcb.data.OImports );

    ARRANGE_DWORD( &dcb.data.OSourceFiles );
    ARRANGE_DWORD( &dcb.data.OSysProcsCodes );

    if ( memcmp( dcb.data.Header, DCB_MAGIC, sizeof( DCB_MAGIC ) - 1 ) != 0 || dcb.data.Version < 0x0700 ) return 0 ;

    log_cb(RETRO_LOG_DEBUG, "dcb_load_from: SGlobal=%d SLocal=%d NLocStrings=%d NProcs=%d NVarSpaces=%d\n", dcb.data.SGlobal, dcb.data.SLocal, dcb.data.NLocStrings, dcb.data.NProcs, dcb.data.NVarSpaces);

    globaldata = bgd_calloc( dcb.data.SGlobal + 4, 1 ) ;
    localdata  = bgd_calloc( dcb.data.SLocal + 4, 1 ) ;
    localstr   = ( int * ) bgd_calloc( dcb.data.NLocStrings + 4, sizeof( int ) ) ;
    dcb.proc   = ( DCB_PROC * ) bgd_calloc(( 1 + dcb.data.NProcs ), sizeof( DCB_PROC ) ) ;
    procs      = ( PROCDEF * ) bgd_calloc(( 1 + dcb.data.NProcs ), sizeof( PROCDEF ) ) ;

    procdef_count = dcb.data.NProcs ;
    local_size    = dcb.data.SLocal ;
    local_strings = dcb.data.NLocStrings ;

    /* Recupera las zonas de datos globales */

    file_seek( fp, offset + dcb.data.OGlobal, SEEK_SET ) ;
    file_read( fp, globaldata, dcb.data.SGlobal ) ;         /* **** */

    file_seek( fp, offset + dcb.data.OLocal, SEEK_SET ) ;
    file_read( fp, localdata, dcb.data.SLocal ) ;           /* **** */

    if ( dcb.data.NLocStrings )
    {
        file_seek( fp, offset + dcb.data.OLocStrings, SEEK_SET ) ;
        file_readUint32A( fp, (uint32_t *)localstr, dcb.data.NLocStrings ) ;
    }

    file_seek( fp, offset + dcb.data.OProcsTab, SEEK_SET ) ;

    log_cb(RETRO_LOG_DEBUG, "loop dcb.data.NProcs\n");
    for ( n = 0 ; n < dcb.data.NProcs ; n++ )
    {
        //log_cb(RETRO_LOG_DEBUG, "loop dcb.data.NProcs n=%d\n", n);
        file_read( fp, &dcb.proc[n], sizeof( DCB_PROC_DATA ) ) ;

        ARRANGE_DWORD( &dcb.proc[n].data.ID );
        ARRANGE_DWORD( &dcb.proc[n].data.Flags );
        ARRANGE_DWORD( &dcb.proc[n].data.NParams );

        ARRANGE_DWORD( &dcb.proc[n].data.NPriVars );
        ARRANGE_DWORD( &dcb.proc[n].data.NPriStrings );

        ARRANGE_DWORD( &dcb.proc[n].data.NPubVars );
        ARRANGE_DWORD( &dcb.proc[n].data.NPubStrings );

        ARRANGE_DWORD( &dcb.proc[n].data.NSentences );

        ARRANGE_DWORD( &dcb.proc[n].data.SPrivate );
        ARRANGE_DWORD( &dcb.proc[n].data.SPublic );

        ARRANGE_DWORD( &dcb.proc[n].data.SCode );

        ARRANGE_DWORD( &dcb.proc[n].data.OExitCode );
        ARRANGE_DWORD( &dcb.proc[n].data.OErrorCode );

        ARRANGE_DWORD( &dcb.proc[n].data.OSentences );

        ARRANGE_DWORD( &dcb.proc[n].data.OPriVars );
        ARRANGE_DWORD( &dcb.proc[n].data.OPriStrings );
        ARRANGE_DWORD( &dcb.proc[n].data.OPrivate );

        ARRANGE_DWORD( &dcb.proc[n].data.OPubVars );
        ARRANGE_DWORD( &dcb.proc[n].data.OPubStrings );
        ARRANGE_DWORD( &dcb.proc[n].data.OPublic );

        ARRANGE_DWORD( &dcb.proc[n].data.OCode );
    }

    /* Recupera las cadenas */

    dcb.data.OStrings += offset;
    dcb.data.OText += offset;

    string_load( fp, dcb.data.OStrings, dcb.data.OText, dcb.data.NStrings, dcb.data.SText );

    /* Recupera los ficheros incluídos */

    if ( dcb.data.NFiles )
    {
        log_cb(RETRO_LOG_DEBUG, "loop dcb.data.NFiles\n");
        DCB_FILE dcbfile;
        char fname[__MAX_PATH];

        xfile_init( dcb.data.NFiles );
        file_seek( fp, offset + dcb.data.OFilesTab, SEEK_SET ) ;
        
        for ( n = 0 ; n < dcb.data.NFiles; n++ )
        {
            //log_cb(RETRO_LOG_DEBUG, "loop dcb.data.NFiles n=%d\n", n);
            file_read( fp, &dcbfile, sizeof( DCB_FILE ) ) ;

            ARRANGE_DWORD( &dcbfile.SName );
            ARRANGE_DWORD( &dcbfile.SFile );
            ARRANGE_DWORD( &dcbfile.OFile );

            file_read( fp, &fname, dcbfile.SName ) ;
            file_add_xfile( fp, filename, offset + dcbfile.OFile, fname, dcbfile.SFile ) ;
        }
    }

    /* Recupera los imports */

    if ( dcb.data.NImports )
    {
        log_cb(RETRO_LOG_DEBUG, "NImports\n");
        dcb.imports = ( uint32_t * )bgd_calloc( dcb.data.NImports, sizeof( uint32_t ) ) ;
        file_seek( fp, offset + dcb.data.OImports, SEEK_SET ) ;
        file_readUint32A( fp, dcb.imports, dcb.data.NImports ) ;
    }

    /* Recupera los datos de depurado */

    if ( dcb.data.NID )
    {
        log_cb(RETRO_LOG_DEBUG, "loop dcb.data.NID\n");
        dcb.id = ( DCB_ID * ) bgd_calloc( dcb.data.NID, sizeof( DCB_ID ) ) ;
        file_seek( fp, offset + dcb.data.OID, SEEK_SET ) ;

        for ( n = 0; n < dcb.data.NID; n++ )
        {
            //log_cb(RETRO_LOG_DEBUG, "loop dcb.data.NID n=%d\n", n);
            file_read( fp, &dcb.id[n], sizeof( DCB_ID ) ) ;
            ARRANGE_DWORD( &dcb.id[n].Code );
        }
    }

    if ( dcb.data.NGloVars )
    {
        file_seek( fp, offset + dcb.data.OGloVars, SEEK_SET ) ;
        dcb.glovar = read_and_arrange_varspace( fp, dcb.data.NGloVars );
    }

    if ( dcb.data.NLocVars )
    {
        file_seek( fp, offset + dcb.data.OLocVars, SEEK_SET ) ;
        dcb.locvar = read_and_arrange_varspace( fp, dcb.data.NLocVars );
    }

    if ( dcb.data.NVarSpaces )
    {
        log_cb(RETRO_LOG_DEBUG, "dcb.data.NVarSpaces=%d\n", dcb.data.NVarSpaces);
        dcb.varspace = ( DCB_VARSPACE * ) bgd_calloc( dcb.data.NVarSpaces, sizeof( DCB_VARSPACE ) ) ;
        dcb.varspace_vars = ( DCB_VAR ** ) bgd_calloc( dcb.data.NVarSpaces, sizeof( DCB_VAR * ) ) ;
        file_seek( fp, offset + dcb.data.OVarSpaces, SEEK_SET ) ;

        for ( n = 0; n < dcb.data.NVarSpaces; n++ )
        {
            //log_cb(RETRO_LOG_DEBUG, "read DCB_VARSPACE n=%d\n", n);
            file_read( fp, &dcb.varspace[n], sizeof( DCB_VARSPACE ) ) ;
            ARRANGE_DWORD( &dcb.varspace[n].NVars );
            ARRANGE_DWORD( &dcb.varspace[n].OVars );
        }

        for ( n = 0; n < dcb.data.NVarSpaces; n++ )
        {
            dcb.varspace_vars[n] = 0 ;
            if ( !dcb.varspace[n].NVars ) continue ;
            file_seek( fp, offset + dcb.varspace[n].OVars, SEEK_SET ) ;
            //log_cb(RETRO_LOG_DEBUG, "read read_and_arrange_varspace n=%d\n", n);
            dcb.varspace_vars[n] = read_and_arrange_varspace( fp, dcb.varspace[n].NVars );
        }
    }

    if ( dcb.data.NSourceFiles )
    {
        log_cb(RETRO_LOG_DEBUG, "dcb.data.NSourceFiles\n");
        char fname[__MAX_PATH] ;

        dcb.sourcecount = ( uint32_t * ) bgd_calloc( dcb.data.NSourceFiles, sizeof( uint32_t ) ) ;
        dcb.sourcelines = ( uint8_t *** ) bgd_calloc( dcb.data.NSourceFiles, sizeof( char ** ) ) ;
        dcb.sourcefiles = ( uint8_t ** ) bgd_calloc( dcb.data.NSourceFiles, sizeof( char * ) ) ;
        file_seek( fp, offset + dcb.data.OSourceFiles, SEEK_SET ) ;
        for ( n = 0; n < dcb.data.NSourceFiles; n++ )
        {
            //log_cb(RETRO_LOG_DEBUG, "dcb.data.NSourceFiles n=%d\n", n);
            file_readUint32( fp, &size ) ;
            file_read( fp, fname, size ) ;
            if ( !load_file( fname, n ) ) fprintf( stdout, "WARNING: Runtime warning - file not found (%s)\n", fname ) ;
        }
    }

    /* ZZZZZZZZZZZZZZZZZZZZZZZZZZ */

    /* Recupera los procesos */
    log_cb(RETRO_LOG_DEBUG, "dcb.data.NProcs\n");

    for ( n = 0 ; n < dcb.data.NProcs ; n++ )
    {
        //log_cb(RETRO_LOG_DEBUG, "dcb.data.NProcs n=%d\n", n);
        procs[n].params             = dcb.proc[n].data.NParams ;
        procs[n].string_count       = dcb.proc[n].data.NPriStrings ;
        procs[n].pubstring_count    = dcb.proc[n].data.NPubStrings ;
        procs[n].private_size       = dcb.proc[n].data.SPrivate ;
        procs[n].public_size        = dcb.proc[n].data.SPublic ;
        procs[n].code_size          = dcb.proc[n].data.SCode ;
        procs[n].id                 = dcb.proc[n].data.ID ;
        procs[n].flags              = dcb.proc[n].data.Flags ;
        procs[n].type               = n ;
        procs[n].name               = getid_name( procs[n].id ) ;
        procs[n].breakpoint         = 0;

        if ( dcb.proc[n].data.SPrivate )
        {
            procs[n].pridata = ( int * )bgd_calloc( dcb.proc[n].data.SPrivate, sizeof( char ) ) ; /* El size ya esta calculado en bytes */
            file_seek( fp, offset + dcb.proc[n].data.OPrivate, SEEK_SET ) ;
            file_read( fp, procs[n].pridata, dcb.proc[n].data.SPrivate ) ;      /* *** */
        }

        if ( dcb.proc[n].data.SPublic )
        {
            procs[n].pubdata = ( int * )bgd_calloc( dcb.proc[n].data.SPublic, sizeof( char ) ) ; /* El size ya esta calculado en bytes */
            file_seek( fp, offset + dcb.proc[n].data.OPublic, SEEK_SET ) ;
            file_read( fp, procs[n].pubdata, dcb.proc[n].data.SPublic ) ;       /* *** */
        }

        if ( dcb.proc[n].data.SCode )
        {
            procs[n].code = ( int * ) bgd_calloc( dcb.proc[n].data.SCode, sizeof( char ) ) ; /* El size ya esta calculado en bytes */
            file_seek( fp, offset + dcb.proc[n].data.OCode, SEEK_SET ) ;
            file_readUint32A( fp, (uint32_t *)procs[n].code, dcb.proc[n].data.SCode / sizeof(uint32_t) ) ;

            if ( dcb.proc[n].data.OExitCode )
                procs[n].exitcode = dcb.proc[n].data.OExitCode ;
            else
                procs[n].exitcode = 0 ;

            if ( dcb.proc[n].data.OErrorCode )
                procs[n].errorcode = dcb.proc[n].data.OErrorCode ;
            else
                procs[n].errorcode = 0 ;
        }

        if ( dcb.proc[n].data.NPriStrings )
        {
            procs[n].strings = ( int * )bgd_calloc( dcb.proc[n].data.NPriStrings, sizeof( int ) ) ;
            file_seek( fp, offset + dcb.proc[n].data.OPriStrings, SEEK_SET ) ;
            file_readUint32A( fp, (uint32_t *)procs[n].strings, dcb.proc[n].data.NPriStrings ) ;
        }

        if ( dcb.proc[n].data.NPubStrings )
        {
            procs[n].pubstrings = ( int * )bgd_calloc( dcb.proc[n].data.NPubStrings, sizeof( int ) ) ;
            file_seek( fp, offset + dcb.proc[n].data.OPubStrings, SEEK_SET ) ;
            file_readUint32A( fp, (uint32_t *)procs[n].pubstrings, dcb.proc[n].data.NPubStrings ) ;
        }

        if ( dcb.proc[n].data.NPriVars )
        {
            file_seek( fp, offset + dcb.proc[n].data.OPriVars, SEEK_SET ) ;
            dcb.proc[n].privar = read_and_arrange_varspace( fp, dcb.proc[n].data.NPriVars );            
        }

        if ( dcb.proc[n].data.NPubVars )
        {
            file_seek( fp, offset + dcb.proc[n].data.OPubVars, SEEK_SET ) ;
            dcb.proc[n].pubvar = read_and_arrange_varspace( fp, dcb.proc[n].data.NPubVars );
        }
        #if __BYTEORDER == __BIG_ENDIAN
            fix_endianess_data(procs[n].pridata, dcb.proc[n].privar, dcb.proc[n].data.NPriVars);
            fix_endianess_data(procs[n].pubdata, dcb.proc[n].pubvar, dcb.proc[n].data.NPubVars);
        #endif
    }

    /* Recupero tabla de fixup de sysprocs */

    sysproc_code_ref = bgd_calloc( dcb.data.NSysProcsCodes, sizeof( DCB_SYSPROC_CODE2 ) ) ;
    file_seek( fp, offset + dcb.data.OSysProcsCodes, SEEK_SET ) ;

    log_cb(RETRO_LOG_DEBUG, "dcb.data.NSysProcsCodes\n");
    for ( n = 0; n < dcb.data.NSysProcsCodes; n++ )
    {
        //log_cb(RETRO_LOG_DEBUG, "dcb.data.NSysProcsCodes n=%d\n", n);
        DCB_SYSPROC_CODE sdcb;
        file_read( fp, &sdcb, sizeof( DCB_SYSPROC_CODE ) ) ;

        ARRANGE_DWORD( &sdcb.Id );
        ARRANGE_DWORD( &sdcb.Type );
        ARRANGE_DWORD( &sdcb.Params );
        ARRANGE_DWORD( &sdcb.Code );

        sysproc_code_ref[n].Id = sdcb.Id ;
        sysproc_code_ref[n].Type = sdcb.Type ;
        sysproc_code_ref[n].Params = sdcb.Params ;
        sysproc_code_ref[n].Code = sdcb.Code ;
        sysproc_code_ref[n].ParamTypes = ( uint8_t * ) bgd_calloc( sdcb.Params + 1, sizeof( char ) );
        if ( sdcb.Params ) file_read( fp, sysproc_code_ref[n].ParamTypes, sdcb.Params ) ;
    }

    log_cb(RETRO_LOG_DEBUG, "sysprocs_fixup\n");
    sysprocs_fixup();

    mainproc = procdef_get_by_name( "MAIN" );

#if __BYTEORDER == __BIG_ENDIAN
    fix_endianess_data(globaldata, dcb.glovar, dcb.data.NGloVars);
    fix_endianess_data(localdata, dcb.locvar, dcb.data.NLocVars);
#endif

    log_cb(RETRO_LOG_DEBUG, "done\n");
    return 1 ;
}


/* ---------------------------------------------------------------------- */

char * getid_name( unsigned int code )
{
    unsigned int n ;
    for ( n = 0 ; n < dcb.data.NID ; n++ )
        if ( dcb.id[n].Code == code )
            return (char *)dcb.id[n].Name ;
    return "(?)" ;
}

/* ---------------------------------------------------------------------- */

int getid( char * name )
{
    unsigned int n ;
    for ( n = 0 ; n < dcb.data.NID ; n++ )
        if ( strcmp( (const char *)dcb.id[n].Name, name ) == 0 )
            return dcb.id[n].Code ;
    return -1 ;
}

/* ---------------------------------------------------------------------- */
