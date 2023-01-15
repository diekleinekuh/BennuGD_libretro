/*
 *  Copyright © 2006-2019 SplinterGU (Fenix/Bennugd)
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

#ifndef _LOADLIB_H
#define _LOADLIB_H

/* --------------------------------------------------------------------------- */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#define _GNU_SOURCE
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define __stdcall
#define __dllexport
#define __dllimport
#endif

#include <stdio.h>

/*******************************************************************************
 * COMMON                                                                      *
 *******************************************************************************/

static char * __dliberr = NULL;

/* --------------------------------------------------------------------------- */

static char * dliberror( void )
{
    return __dliberr;
}

/* --------------------------------------------------------------------------- */

#ifndef __STATIC__

/*******************************************************************************
 * MODULAR                                                                     *
 *******************************************************************************/

#ifdef _WIN32
#define dlclose(a)
#endif

/* --------------------------------------------------------------------------- */

typedef struct
{
    char * fname;
    void * hnd;
} dlibhandle ;

/* --------------------------------------------------------------------------- */

static int dlibclose( void * _handle )
{
    dlibhandle * handle = ( dlibhandle * ) _handle;
    dlclose( handle->hnd );
    free( handle->fname );
    free( handle );

    return 0;
}

/* --------------------------------------------------------------------------- */

static void * dlibopen( const char * fname )
{
    char *f;
    char *_fname = NULL;
#ifdef _WIN32
    void * hnd = LoadLibrary( fname );
#else
    void * hnd = dlopen( fname, RTLD_NOW | RTLD_GLOBAL );
#endif

    // Avoid unused warning in compiller
    (void) &dliberror;
    (void) &dlibclose;

    if ( !hnd )
    {
        f = ( char * ) fname + strlen( fname );
        while ( f > fname && f[-1] != '\\' && f[-1] != '/' ) f-- ;

        if ( strncmp( f, "lib", 3 ) )
        {
            if ( ( _fname = malloc( strlen( fname ) + 4 ) ) )
            {
                sprintf( _fname, "%.*slib%s", ( int ) ( f - fname ), fname, f );
#ifdef _WIN32
                hnd = LoadLibrary( _fname );
#else
                hnd = dlopen( _fname, RTLD_NOW | RTLD_GLOBAL );
#endif
            }
        }
    }

    if ( !hnd )
    {
#ifdef _WIN32
        __dliberr = "Could not load library." ;
#else
        __dliberr = dlerror() ;
#endif
        if ( _fname ) free( _fname );
        return NULL;
    }

    dlibhandle * dlib = (dlibhandle*) malloc( sizeof( dlibhandle ) );
    if ( !dlib )
    {
        __dliberr = "Could not load library." ;
        dlclose( hnd );
        if ( _fname ) free( _fname );
        return NULL;
    }

    f = ( char * ) fname + strlen( fname );
    while ( f > fname && f[-1] != '\\' && f[-1] != '/' ) f-- ;
    dlib->fname = strdup( f );
    if ( !dlib->fname )
    {
        __dliberr = "Could not load library." ;
        free( dlib );
        dlclose( hnd );
        if ( _fname ) free( _fname );
        return NULL;
    }

    dlib->hnd = hnd;

    if ( _fname ) free( _fname );

    return ( ( void * ) dlib );
}

/* --------------------------------------------------------------------------- */

static void * dlibaddr( void * _handle, const char * symbol )
{
    dlibhandle * handle = ( dlibhandle * ) _handle;
    char * ptr, * f;

#ifdef _WIN32
    void * addr = GetProcAddress( (HMODULE)handle->hnd, symbol );
    if ( !addr )
    {
        __dliberr = "Symbol not found." ;
        return NULL;
    }
#else
    void * addr = dlsym( handle->hnd, symbol ) ;
    if ( !addr )
    {
        __dliberr = dlerror() ;
        return NULL;
    }

#ifndef TARGET_BEOS
    Dl_info dli;
    dladdr( addr, &dli );

    ptr = ( char * ) dli.dli_fname; f = NULL;
/*
    printf( "dli_fname=%s\n", dli.dli_fname );
    printf( "dli_fbase=%p\n", dli.dli_fbase );
    printf( "dli_sname=%s\n", dli.dli_sname );
    printf( "dli_saddr=%p\n", dli.dli_saddr );
*/

    while ( *ptr )
    {
        if ( *ptr == '/' || *ptr == '\\' ) f = ptr ;
        ptr++;
    }
    if ( f ) ptr = f + 1;

    if ( strcmp( ptr, handle->fname ) )
    {
        if ( strncmp( ptr, "lib", 3 ) || strcmp( ptr + 3, handle->fname ) )
        {
            __dliberr = "Symbol not found." ;
            return NULL;
        }
    }
#endif
#endif

    return addr;
}

/* --------------------------------------------------------------------------- */

static void * _dlibaddr( void * _handle, const char * symbol )
{
    dlibhandle * handle = ( dlibhandle * ) _handle;
    char * ptr, * f;
    char * sym = (char*)malloc( strlen( handle->fname ) + strlen( symbol ) + 2 );
    if ( !sym )
    {
        __dliberr = "Can't load symbol." ;
        return NULL;
    }

    strcpy( sym, handle->fname );
    ptr = ( char * ) sym; f = NULL;
    while ( *ptr )
    {
        if ( *ptr == '.' ) f = ptr ;
        ptr++;
    }

    if ( f ) *f = '\0';
    strcat( sym, "_" ); strcat( sym, symbol );

    {
        void * addr = dlibaddr( handle, sym );
        free( sym );
        return addr;
    }
}

/* --------------------------------------------------------------------------- */

#else

/*******************************************************************************
 * MONOLITH                                                                    *
 *******************************************************************************/

#include <fake_dl.h>

static int fake_dl_inited = 0;

/* --------------------------------------------------------------------------- */

#define _dlibaddr(a,b)  dlibaddr(a,b)

/* --------------------------------------------------------------------------- */

static int dlibclose( void * handle )
{
    return 0;
}

/* --------------------------------------------------------------------------- */

static void * dlibopen( const char * fname )
{
    __FAKE_DL * fdl = __fake_dl;
    char dlname[32], *p;

    if ( !fake_dl_inited )
    {
        fake_dl_init();
        fake_dl_inited = 1;
    }

    strcpy( dlname, basename( fname ) );

    p = strrchr( dlname, '.' );
    if ( p ) *p = '\0' ;

    while( fdl->dlname )
    {
        if ( !strcmp( dlname, fdl->dlname ) ) return ( void * ) fdl;
        fdl++;
    }

    __dliberr = "Could not load library." ;

    return NULL;
}

/* --------------------------------------------------------------------------- */

static void * dlibaddr( void * handle, const char * symbol )
{
    __FAKE_DL * fdl = ( __FAKE_DL * ) handle;

    if ( !fdl ) return NULL;

    if ( !strcmp( symbol, "constants_def"             ) ) return ( fdl->constants_def );
    if ( !strcmp( symbol, "types_def"                 ) ) return ( fdl->types_def );
    if ( !strcmp( symbol, "globals_def"               ) ) return ( fdl->globals_def );
    if ( !strcmp( symbol, "locals_def"                ) ) return ( fdl->locals_def );
    if ( !strcmp( symbol, "globals_fixup"             ) ) return ( fdl->globals_fixup );
    if ( !strcmp( symbol, "locals_fixup"              ) ) return ( fdl->locals_fixup );
    if ( !strcmp( symbol, "functions_exports"         ) ) return ( fdl->functions_exports );
    if ( !strcmp( symbol, "module_initialize"         ) ) return ( fdl->module_initialize );
    if ( !strcmp( symbol, "module_finalize"           ) ) return ( fdl->module_finalize );
    if ( !strcmp( symbol, "instance_create_hook"      ) ) return ( fdl->instance_create_hook );
    if ( !strcmp( symbol, "instance_destroy_hook"     ) ) return ( fdl->instance_destroy_hook );
    if ( !strcmp( symbol, "instance_pre_execute_hook" ) ) return ( fdl->instance_pre_execute_hook );
    if ( !strcmp( symbol, "instance_pos_execute_hook" ) ) return ( fdl->instance_pos_execute_hook );
    if ( !strcmp( symbol, "process_exec_hook"         ) ) return ( fdl->process_exec_hook );
    if ( !strcmp( symbol, "handler_hooks"             ) ) return ( fdl->handler_hooks );
    if ( !strcmp( symbol, "modules_dependency"        ) ) return ( fdl->modules_dependency );

    return NULL;
}

/* --------------------------------------------------------------------------- */

#endif

/* --------------------------------------------------------------------------- */

#endif
