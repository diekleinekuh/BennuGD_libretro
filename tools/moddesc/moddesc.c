/*
 *  Copyright © 2006-2011 SplinterGU (Fenix/Bennugd)
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

#include <loadlib.h> /* Must be fist include */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include <bgddl.h>
#include <dlvaracc.h>
#include <files_st.h>

/* ----------------------------------------------------------------- */

int onlyfenixexport = 0;
int interactive = 1;
int all = 1, functions = 0, locals = 0, globals = 0, types = 0, constants = 0, hooks_callbacks = 0, dependencies = 0, locals_dep = 0, globals_dep = 0;

/* ----------------------------------------------------------------- */

void describe_type( int type )
{
    switch ( type )
    {
        case TYPE_DWORD:
            printf( "DWORD" ) ;
            break ;
        case TYPE_INT:
            printf( "INT" ) ;
            break ;
        case TYPE_SHORT:
            printf( "SHORT" ) ;
            break ;
        case TYPE_WORD:
            printf( "WORD" ) ;
            break ;
        case TYPE_BYTE:
            printf( "BYTE" ) ;
            break ;
        case TYPE_SBYTE:
            printf( "SIGNED BYTE" ) ;
            break ;
        case TYPE_CHAR:
            printf( "CHAR" ) ;
            break ;
        case TYPE_STRING:
            printf( "STRING" ) ;
            break ;
        case TYPE_FLOAT:
            printf( "FLOAT" ) ;
            break ;
        case TYPE_POINTER:
            printf( "POINTER" ) ;
            break ;
        case TYPE_UNDEFINED:
            printf( "UNDEFINED" );
            break ;
    }
}

/* ----------------------------------------------------------------- */

void * fnc_import( const char * name )
{
    if ( !onlyfenixexport ) printf( "Import %s\n", name );
    return NULL;
}

/* ----------------------------------------------------------------- */

int describe_func( char * name, char * paramtypes, int type, void * func )
{
    int param;
    if ( func ) printf( "Export " );
    describe_type( type );
    printf( " %s(", name );
    param = 0;
    while ( paramtypes && paramtypes[param] )
    {
        switch ( paramtypes[param] )
        {
            case 'I':
                printf( "INTEGER" );
                break;

            case 'B':
                printf( "BYTE" );
                break;

            case 'W':
                printf( "WORD" );
                break;

            case 'S':
                printf( "STRING" );
                break;

            case 'P':
                printf( "POINTER" );
                break;

            case 'F':
                printf( "FLOAT" );
                break;

            case 'V':
                printf( "VARIABLE" );
                param += 2;
                break;

            default:
                printf( "[PARAM ERROR]" ) ;
        }

        if ( paramtypes[param+1] ) printf( ", " );
        param++;
    }
    printf( ")\n" );

    return 0 ;
}

/* ----------------------------------------------------------------- */

static char * modules_exts[] =
{
    ".dll",
    ".dylib",
    ".so",
    NULL
} ;

/* ----------------------------------------------------------------- */

void describe_module( char *filename )
{
    void * library = NULL;

    char ** types_def = NULL;
    char ** globals_def = NULL;
    char ** locals_def = NULL;
    DLCONSTANT * constants_def = NULL;
    DLSYSFUNCS * functions_exports = NULL;
    char ** modules_dependency = NULL;

    DLVARFIXUP * globals_fixup = NULL;
    DLVARFIXUP * locals_fixup = NULL;
    HOOK * handler_hooks = NULL;

    void ( * RegisterFunctions )( void *( * )( const char * ), int ( * )( char *, char *, int, void * ) ) = NULL ;
    int misc = 0;

    char soname[1024];
    char * ptr;
    char ** pex;

#if defined( WIN32 )
#define DLLEXT      ".dll"
#elif defined(TARGET_MAC)
#define DLLEXT      ".dylib"
#else
#define DLLEXT      ".so"
#endif

    strncpy ( soname, filename, sizeof ( soname ) );

    for ( ptr = soname; *ptr; ptr++ )
    {
        *ptr = tolower( *ptr );
    }

    pex = modules_exts;
    while ( pex && * pex )
    {
        int nlen = strlen ( soname );
        int elen = strlen ( *pex );
        if ( nlen > elen && strcmp( &soname[nlen - elen], *pex ) == 0 )
        {
            soname[nlen - elen] = '\0' ;
            pex = modules_exts;
        }
        else
        {
            pex++;
        }
    }

    strcat( soname, DLLEXT );

    filename = soname;

    library  = dlibopen( filename ) ;
    if ( !library )
    {
        printf( "ERROR: %s library not found\n", filename ) ;
        return;
    }

    printf( "Module name: %s\n\n", filename );

    if ( all || constants )
    {
        constants_def = ( DLCONSTANT * ) _dlibaddr( library, "constants_def" ) ;
        if ( constants_def )
        {
            printf( "Constants:\n\n" ); fflush( stdout );
            while ( constants_def->name )
            {
                describe_type( constants_def->type ); fflush( stdout );
                printf( " %s = %d\n", constants_def->name, constants_def->code ); fflush( stdout );
                constants_def++ ;
            }
            printf( "\n\n" );
        }
    }

    if ( all || types )
    {
        types_def = ( char ** ) _dlibaddr( library, "types_def" ) ;
        if ( types_def && *types_def )
        {
            printf( "Types:\n\n%s\n\n", *types_def );
        }
    }

    if ( all || globals )
    {
        globals_def = ( char ** ) _dlibaddr( library, "globals_def" ) ;
        if ( globals_def && *globals_def )
        {
            printf( "Globals:\n\n%s\n\n", *globals_def );
        }
    }

    if ( all || locals )
    {
        locals_def  = ( char ** ) _dlibaddr( library, "locals_def" ) ;
        if ( locals_def && *locals_def )
        {
            printf( "Locals:\n\n%s\n\n", *locals_def );
        }
    }

    if ( all || functions )
    {
        functions_exports = ( DLSYSFUNCS * ) _dlibaddr( library, "functions_exports" ) ;
        if ( functions_exports )
        {
            printf( "Functions:\n\n" );
            while ( functions_exports->name )
            {
                describe_func( functions_exports->name, functions_exports->paramtypes, functions_exports->type, NULL );
                functions_exports++;
            }
            printf( "\n\n" );
        }
    }

    if ( all || dependencies )
    {
        modules_dependency = ( char ** ) _dlibaddr( library, "modules_dependency" ) ;
        if ( modules_dependency )
        {
            printf( "Module Dependency:\n\n" );
            while ( *modules_dependency )
            {
                printf( "%s\n", *modules_dependency );
                modules_dependency++;
            }
            printf( "\n\n" );
        }
    }

    if ( all || globals_dep )
    {
        globals_fixup     = ( DLVARFIXUP * ) _dlibaddr( library, "globals_fixup" ) ;
        if ( globals_fixup )
        {
            printf( "Globals vars dependency:\n\n" );
            while ( globals_fixup->var )
            {
                printf( "%s\n", globals_fixup->var );
                globals_fixup++;
            }
            printf( "\n\n" );
        }
    }

    if ( all || locals_dep )
    {
        locals_fixup      = ( DLVARFIXUP * ) _dlibaddr( library, "locals_fixup" ) ;
        if ( locals_fixup )
        {
            printf( "Locals vars dependency:\n\n" );
            while ( locals_fixup->var )
            {
                printf( "%s\n", locals_fixup->var );
                locals_fixup++;
            }
            printf( "\n\n" );
        }
    }

    if ( all || hooks_callbacks )
    {
        #define CallbackHeaders() if (!misc) { misc = 1; printf ("Internals callbacks:\n\n"); }

        if ( _dlibaddr( library, "module_initialize" ) )
        {
            CallbackHeaders(); printf( "module_initialize\n" );
        }
        if ( _dlibaddr( library, "module_finalize" ) )
        {
            CallbackHeaders(); printf( "module_finalize\n" );
        }
        if ( _dlibaddr( library, "instance_create_hook" ) )
        {
            CallbackHeaders(); printf( "instance_create_hook\n" );
        }
        if ( _dlibaddr( library, "instance_destroy_hook" ) )
        {
            CallbackHeaders(); printf( "instance_destroy_hook\n" );
        }
        if ( _dlibaddr( library, "instance_pre_execute_hook" ) )
        {
            CallbackHeaders(); printf( "instance_pre_execute_hook\n" );
        }
        if ( _dlibaddr( library, "instance_pos_execute_hook" ) )
        {
            CallbackHeaders(); printf( "instance_pos_execute_hook\n" );
        }
        if ( _dlibaddr( library, "process_exec_hook" ) )
        {
            CallbackHeaders(); printf( "process_exec_hook\n" );
        }

        if ( misc ) printf( "\n\n" );

        handler_hooks = ( HOOK * ) _dlibaddr( library, "handler_hooks" ) ;
        if ( handler_hooks )
        {
            int i = 0;
            printf( "Hooks at priorities: " );
            while ( handler_hooks->hook )
            {
                printf( "%d", handler_hooks->prio );
                handler_hooks++;
                if ( handler_hooks->hook ) printf( ", " );
                i++;
            }
            printf( "\n\n" );
        }
    }

    if ( all )
    {
        RegisterFunctions = _dlibaddr( library, "RegisterFunctions" ) ;
        if ( RegisterFunctions )
        {
            printf( "Fenix support:\n\n" );
            ( *RegisterFunctions )( fnc_import, describe_func ) ;
        }
    }
}

/* ----------------------------------------------------------------- */

void help( char *argv[] )
{
    printf( "Usage: %s [options] modulename \n"
            "\n"
            "    -e     On fenix, only report exported functions\n"
            "    -F     only report funcions\n"
            "    -L     only report locals\n"
            "    -G     only report globals\n"
            "    -T     only report types\n"
            "    -C     only report constants\n"
            "    -H     only report hooks/callbacks\n"
            "    -D     only report modules/libs dependencies\n"
            "    -l     only report locals dependencies\n"
            "    -g     only report globals dependencies\n"
            "    -h     This help\n"
            "\n", argv[0] ) ;

#if _WIN32
    if ( interactive )
    {
        printf( "\nPress any key to continue...\n" );
        fflush( stdout );
        char a; scanf( "%c", &a ) ;
    }
#endif
}

/* ----------------------------------------------------------------- */

int main( int argc, char *argv[] )
{
    int i = 1;
    char * modname = NULL;

    printf( "Module Describe v1.1 (Build: %s %s)\n"
            "Copyright (C) 2009 SplinterGU\n"
            "This utility comes with ABSOLUTELY NO WARRANTY.\n"
            "%s -h for details\n\n", __DATE__, __TIME__, argv[0] ) ;

    if ( argc <= 1 )
    {
        help( argv ) ;
        exit( 0 ) ;
    }

    while ( i < argc )
    {
        if ( argv[i][0] == '-' )
        {
            switch ( argv[i][1] )
            {
                case 'e':
                    onlyfenixexport = 1 ;
                    break;

                case 'F':
                    all = 0;
                    functions = 1;
                    break;

                case 'L':
                    all = 0;
                    locals = 1;
                    break;

                case 'G':
                    all = 0;
                    globals = 1;
                    break;

                case 'T':
                    all = 0;
                    types = 1;
                    break;

                case 'C':
                    all = 0;
                    constants = 1;
                    break;

                case 'H':
                    all = 0;
                    hooks_callbacks = 1;
                    break;

                case 'D':
                    all = 0;
                    dependencies = 1;
                    break;

                case 'l':
                    all = 0;
                    locals_dep = 1;
                    break;

                case 'g':
                    all = 0;
                    globals_dep = 1;
                    break;

                default:
                case 'h':
                    help( argv ) ;
                    exit( 0 );
                    break;
            }
        }
        else
        {
            modname = argv[i];
        }
        i++;
    }

    if ( !modname )
    {
        help( argv ) ;
        exit( 0 ) ;
    }

    describe_module( modname );

    exit( 0 );
}

/* ----------------------------------------------------------------- */
