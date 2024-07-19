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

/* --------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <SDL.h>

#include "bgdcore.h"
#include "bgdrtm.h"
#include "bgddl.h"
#include "dlvaracc.h"

#include "dcb.h"
#include "instance.h"

#include "files.h"

#include "xstrings.h"
#include "libkey.h"
#include "libgrbase.h"
#include "librender.h"

/* --------------------------------------------------------------------------- */

extern void systext_color( int cfg, int cbg );
extern void systext_puts( GRAPH * map, int x, int y, char * str, int len );

/* --------------------------------------------------------------------------- */

#ifndef _vsnprintf
#define _vsnprintf vsnprintf
#endif

#ifndef _snprintf
#define _snprintf snprintf
#endif

/* --------------------------------------------------------------------------- */

enum {
    PROCESS_ID = 0,
    FATHER,
    BIGBRO,
    SON,
    STATUS
};

enum {
    SHIFTSTATUS = 0
};

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( mod_debug, locals_fixup )[] = {
    { "id"                  , NULL, -1, -1 },
    { "father"              , NULL, -1, -1 },
    { "bigbro"              , NULL, -1, -1 },
    { "son"                 , NULL, -1, -1 },
    { "reserved.status"     , NULL, -1, -1 },
    { NULL                  , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( mod_debug, globals_fixup )[] = {
    { "shift_status"        , NULL, -1, -1 },
    { NULL                  , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

#define MAXTEXT 5000

#define CHARWIDTH 6
#define CHARHEIGHT 8

#define CONSOLE_HISTORY 512
#define CONSOLE_LINES   25
#define CONSOLE_COLUMNS 80
#define COMMAND_HISTORY 128

#define MAX_EXPRESSIONS 128

/* --------------------------------------------------------------------------- */

#define HELPTXT \
    "\25404Process Info\25407\n"                                                       \
    "\25405INSTANCES       \25407  List all running processes\n"                       \
    "\25405GLOBALS         \25407  Show global vars with values\n"                     \
    "\25405LOCALS proc     \25407  Show a process's local vars\n"                      \
    "\25405PRIVATES proc   \25407  Show a process's private vars\n"                    \
    "\25405PUBLICS proc    \25407  Show a process's public vars\n"                     \
    "\n"                                                                         \
    "\25404Execution Commands\25407\n"                                                 \
    "\25405GO              \25407  Continue the execution\n"                           \
    "\25405TRACE           \25407  Execute one instruction and Debug\n"                \
    "\25405STEP            \25407  Execute one instruction/process/function and Debug\n" \
    "\25405NEXTFRAME       \25407  Continue to next frame\n"                           \
    "\25405NEXTPROC        \25407  Continue to next process\n"                         \
    "\n"                                                                         \
    "\25404Breakpoints\25407\n"                                                        \
    "\25405BREAK           \25407  List breakpoints\n"                                 \
    "\25405BREAK proc      \25407  Set a breakpoint on process\n"                      \
    "\25405BREAKALL        \25407  Set breakpoints on all processes\n"                 \
    "\25405BREAKALLTYPES   \25407  Set breakpoints on all processes types\n"           \
    "\25405DELETE proc     \25407  Delete a breakpoint\n"                              \
    "\25405DELETEALL       \25407  Delete all breakpoints on all processess\n"         \
    "\25405DELETEALLTYPES  \25407  Delete all breakpoints on all processess types\n"   \
    "\n"                                                                         \
    "\25404Process Interaction\25407\n"                                                \
    "\25405RUN proc [args] \25407  Run a process\n"                                    \
    "\25405KILL proc       \25407  Kill a process\n"                                   \
    "\25405WAKEUP proc     \25407  Wakeup a process\n"                                 \
    "\25405SLEEP proc      \25407  Sleep a process\n"                                  \
    "\25405FREEZE proc     \25407  Freeze a process\n"                                 \
    "\25405KILLALL proc    \25407  Kill all process with criteria\n"                   \
    "\25405WAKEUPALL proc  \25407  Wakeup all process with criteria\n"                 \
    "\25405SLEEPALL proc   \25407  Sleep all process with criteria\n"                  \
    "\25405FREEZEALL proc  \25407  Freeze all process with criteria\n"                 \
    "\n"                                                                         \
    "\25404Misc\25407\n"                                                               \
    "\25405SHOW expression \25407  Evaluate and show some expression\n"                \
    "\25405SHOW            \25407  List all expressions to show\n"                     \
    "\25405SHOWDEL ExpID   \25407  Delete an expression (by ID, use SHOW)\n"           \
    "\25405SHOWDELALL      \25407  Delete all expression\n"                            \
    "\25405STRINGS         \25407  Show all strings in memory\n"                       \
    "\25405VARS            \25407  Show internals vars\n"                              \
    "\25405QUIT            \25407  Kill the program and exit\n"                        \
    "\n"                                                                         \
    "\25404Keys\25407\n"                                                               \
    "\25405ESC             \25407  Cancel command\n"                                   \
    "\25405UP/DOWN         \25407  Command history navigation\n"                       \
    "\25405PGUP/PGDN       \25407  Page Up/Page Down\n"                                \
    "\25405CTRL+CURSORS    \25407  Console scroll\n"                                   \
    "\25405ALT+CURSORS     \25407  Console window size\n"                              \
    "\25405SHIFT+CURSORS   \25407  List window scroll\n"                               \
    "\n"                                                                         \
    "You can evaluate bgd_free expressions in the console, and you can see/change\n" \
    "local, public and private vars using the '.' operator\n"                    \
    "(pe: 65535.X, MAIN.X, etc.)\n\n"

/* --------------------------------------------------------------------------- */

#define HOTKEYHELP_SIZE 50

#define HOTKEYHELP1  "\25401F1:\25400?\x03\25401F2:\25400Procs\x03\25401F5:\25400Go\x03\25401F7:\25400Trace\x03\25401F8:\25400Step\x03\25401F10:\25400NFrame\x03\25401F11:\25400NProc"
#define HOTKEYHELP2  "\25401F1:\25400?\x03\25401F2:\25400Brief\x03\25401F6:\25400Procs\x03\25401F9:\25400Break"
#define HOTKEYHELP3  "\25401F1:\25400?\x03\25401F2:\25400Brief\x03\25401F3:\25400Loc\x03\25401F4:\25400Pri\x03\25401F5:\25400Pub\x03\25401F6:\25400Types\x03\25401F9:\25400Brk"

/* --------------------------------------------------------------------------- */

#define N_CONSOLE_VARS (sizeof(console_vars)/sizeof(console_vars[0]))

#define CON_DWORD               0x0001
#define CON_DWORD_HEX           0x8001

/* --------------------------------------------------------------------------- */

/* Tipos de token */
#define IDENTIFIER 1
#define STRING     2
#define NUMBER     3
#define OPERATOR   4
#define NOTOKEN    5

/* --------------------------------------------------------------------------- */

static struct {
    int type ;
    DCB_VAR var ;
    double value ;
    void * data ;
    char name[256] ;
} result, lvalue ;

static struct {
    enum { T_ERROR, T_VARIABLE, T_NUMBER, T_CONSTANT, T_STRING } type ;
    char name[128] ;
    double  code ;
} token ;

static const char * token_ptr ;

/* --------------------------------------------------------------------------- */

/* Console contents */
static int console_lines = CONSOLE_LINES ;
static int console_columns = CONSOLE_COLUMNS ;

static char * console[CONSOLE_HISTORY] ;
static int console_initialized = 0 ;
static int console_head = 0 ;
static int console_tail = 0 ;

static char * command[COMMAND_HISTORY] ;
static int command_initialized = 0 ;
static int command_head = 0 ;
static int command_tail = 0 ;
static int command_count = 0 ;

static char * show_expression[MAX_EXPRESSIONS] = { NULL };
static int show_expression_count = 0;
static int console_showcolor = 0xffffff;

static int console_y = 0 ;

/* --------------------------------------------------------------------------- */

static int debug_on_frame = 0;
static int break_on_next_proc = 0;

/* --------------------------------------------------------------------------- */

static struct {
    char * name ;
    void * value ;
    int    type ;
}
console_vars[] = {
    { "SHOW_COLOR",         &console_showcolor,     CON_DWORD_HEX   },
    { "FILES",              &opened_files,          CON_DWORD       },
    { "DEBUG_LEVEL",        &debug,                 CON_DWORD       },
} ;

/* --------------------------------------------------------------------------- */

static int console_scroll_pos = 0 ;
static int console_scroll_lateral_pos = 0 ;
static char console_input[128] ;

/* --------------------------------------------------------------------------- */

static void eval_immediate();
static void eval_value();
static void eval_factor();
static void eval_subexpression();
static char * eval_expression( const char * here, int interactive );

static char * describe_type( DCB_TYPEDEF type, int from );
static char * show_value( DCB_TYPEDEF type, void * data );
static void   show_struct( int num, char * title, int indent, void * data );
static void   show_var( DCB_VAR var, char * name, void * data, char * title, int indent );

static int type_size( DCB_TYPEDEF orig );
static DCB_TYPEDEF reduce_type( DCB_TYPEDEF orig );
static void var2const();

static void console_do( const char * command );

/* --------------------------------------------------------------------------- */

static void console_scroll( int direction ) {
    console_scroll_pos += direction ;
    if ( direction < 0 ) {
        if ( console_scroll_pos < 0 ) console_scroll_pos = 0 ;
    } else {
        if ( console_scroll_pos > CONSOLE_HISTORY ) console_scroll_pos = CONSOLE_HISTORY ;
    }
}

/* --------------------------------------------------------------------------- */

static void console_putline( char * text ) {
    if ( !console_initialized ) {
        memset( console, 0, sizeof( console ) ) ;
        console_initialized = 1 ;
    }

    if ( console[console_tail] ) bgd_free( console[console_tail] ) ;
    console[console_tail] = bgd_strdup( text ) ;

    console_tail++ ;
    if ( console_tail == CONSOLE_HISTORY ) console_tail = 0 ;
    if ( console_tail == console_head ) {
        console_head++ ;
        if ( console_head == CONSOLE_HISTORY ) console_head = 0 ;
    }
}

/* --------------------------------------------------------------------------- */

static void console_printf( const char *fmt, ... ) {
    char text[MAXTEXT], * ptr, * iptr ;

    va_list ap;
    va_start( ap, fmt );
    _vsnprintf( text, sizeof( text ), fmt, ap );
    va_end( ap );
    text[sizeof( text )-1] = 0;

    if ( *text == '[' ) {
        memmove( text + 3, text, strlen( text ) + 1 ) ;
        memmove( text, "\25408", 3 ) ;
        ptr = strchr( text, ']' ) ;
        if ( ptr ) {
            ptr++ ;
            memmove( ptr + 3, ptr, strlen( ptr ) + 1 ) ;
            memmove( ptr, "\25407", 3 ) ;
        }
    }

    iptr = text ;
    ptr  = text ;

    while ( *ptr ) {
        if ( *ptr == '\n' ) {
            *ptr = 0 ;
            console_putline( iptr ) ;
            iptr = ptr + 1 ;
        }
        if ( *ptr == '\254' ) {
            ptr++ ;
            if ( isdigit( *ptr ) ) ptr++ ;
            if ( isdigit( *ptr ) ) ptr++ ;
            continue ;
        }
        ptr++ ;
    }

    if ( ptr > iptr ) console_putline( iptr ) ;
}

/* --------------------------------------------------------------------------- */

static void console_putcommand( const char * commandline ) {
    if ( !command_initialized ) {
        memset( command, 0, sizeof( command ) );
        command_initialized = 1;
    }

    if ( command[command_tail] ) bgd_free( command[command_tail] );
    command[command_tail++] = bgd_strdup( commandline );
    if ( command_tail == COMMAND_HISTORY ) command_tail = 0;
    if ( command_tail == command_head ) {
        if ( ++command_head == COMMAND_HISTORY ) command_head = 0;
    } else
        command_count++;
}

/* --------------------------------------------------------------------------- */

static const char * console_getcommand( int offset ) {
    if ( offset >= 0 || offset < -command_count ) return NULL;

    offset = command_tail + offset;
    while ( offset < 0 ) offset = COMMAND_HISTORY + offset;
    return command[offset];
}

/* --------------------------------------------------------------------------- */

static void console_getkey( int key, int sym ) {
    static int history_offset = 0;
    char buffer[2] ;
    const char * command;

    if ( !key ) {
        if ( sym == SDLK_UP ) {
            command = console_getcommand( --history_offset );
            if ( command == NULL )  history_offset++;
            else                    strncpy( console_input, command, 127 );
        }

        if ( sym == SDLK_DOWN ) {
            if ( history_offset == -1 ) {
                *console_input = 0;
                history_offset++;
            } else {
                command = console_getcommand( ++history_offset );
                if ( command == NULL )  history_offset--;
                else                    strncpy( console_input, command, 127 );
            }
        }
    }

    if ( key == SDLK_BACKSPACE && *console_input ) console_input[strlen( console_input )-1] = 0 ;
    if ( key == SDLK_ESCAPE ) *console_input = 0 ;
    if ( key == SDLK_RETURN ) {

        console_scroll_pos = 0 ;
        console_printf( "\25407> %s", console_input ) ;
        if ( * console_input ) {
            console_putcommand( console_input );
            console_do( console_input ) ;
            *console_input = 0 ;
            history_offset = 0;
        }
    }

    if ( key >= SDLK_SPACE && key <= SDLK_WORLD_95 ) {
        buffer[0] = key ;
        buffer[1] = 0 ;
        strcat( console_input, buffer ) ;
    }
}

/* --------------------------------------------------------------------------- */

static void console_lateral_scroll( int direction ) {
    if ( direction > 0 ) {
        console_scroll_lateral_pos-- ;
        if ( console_scroll_lateral_pos < 0 ) console_scroll_lateral_pos = 0 ;
    } else {
        console_scroll_lateral_pos++ ;
        if ( console_scroll_lateral_pos > MAXTEXT ) console_scroll_lateral_pos = MAXTEXT ;
    }
}

/* --------------------------------------------------------------------------- */

static char * describe_type( DCB_TYPEDEF type, int from ) {
    static char buffer[512] ;
    int i ;

    if ( !from ) buffer[0] = 0 ;

    switch ( type.BaseType[from] ) {
        case TYPE_ARRAY:
            for ( i = from ; type.BaseType[i] == TYPE_ARRAY; i++ ) ;
            describe_type( type, i ) ;
            for ( i = from ; type.BaseType[i] == TYPE_ARRAY; i++ )
                _snprintf( buffer + strlen( buffer ), sizeof( buffer ) - strlen( buffer ), "[%d]", type.Count[i] - 1 ) ;
            break ;

        case TYPE_STRUCT:
            strcat( buffer, "STRUCT" ) ;
            break ;

        case TYPE_DWORD:
            strcat( buffer, "DWORD" ) ;
            break ;

        case TYPE_INT:
            strcat( buffer, "INT" ) ;
            break ;

        case TYPE_SHORT:
            strcat( buffer, "SHORT" ) ;
            break ;

        case TYPE_WORD:
            strcat( buffer, "WORD" ) ;
            break ;

        case TYPE_BYTE:
            strcat( buffer, "BYTE" ) ;
            break ;

        case TYPE_SBYTE:
            strcat( buffer, "SIGNED BYTE" ) ;
            break ;

        case TYPE_CHAR:
            strcat( buffer, "CHAR" ) ;
            break ;

        case TYPE_STRING:
            strcat( buffer, "STRING" ) ;
            break ;

        case TYPE_FLOAT:
            strcat( buffer, "FLOAT" ) ;
            break ;

        case TYPE_POINTER:
            describe_type( type, from + 1 ) ;
            strcat( buffer, " POINTER" ) ;
            break ;
    }

    return buffer ;
}

/* --------------------------------------------------------------------------- */

static char * show_value( DCB_TYPEDEF type, void * data ) {
    static char buffer[512] ;
    char * newbuffer ;
    int subsize;
    unsigned int n ;
    unsigned int count ;

    switch ( type.BaseType[0] ) {
        case TYPE_ARRAY:
            count = type.Count[0];

            type = reduce_type( type ) ;
            subsize = type_size( type ) ;
            if ( type.BaseType[0] == TYPE_STRUCT ) return "" ;
            newbuffer = ( char * ) bgd_malloc( 512 ) ;
            strcpy( newbuffer, "= (" ) ;
            for ( n = 0 ; n < count ; n++ ) {
                if ( n ) strcat( newbuffer, ", " ) ;
                show_value( type, data ) ;
                if ( strlen( newbuffer ) + strlen( buffer ) > 30 ) {
                    strcat( newbuffer, "..." ) ;
                    break ;
                }
                strcat( newbuffer, buffer + 2 ) ;
                data = ( uint8_t * )data + subsize ;
            }
            strcat( newbuffer, ")" ) ;
            strcpy( buffer, newbuffer ) ;
            bgd_free( newbuffer ) ;
            return buffer ;

        case TYPE_STRUCT:
            return "" ;

        case TYPE_STRING:
            _snprintf( buffer, sizeof( buffer ), "= \"%s\"", string_get( *( uint32_t * )data ) ) ;
            return buffer ;

        case TYPE_BYTE:
            _snprintf( buffer,sizeof( buffer ), "= %d", *( uint8_t * )data ) ;
            return buffer ;

        case TYPE_SBYTE:
            _snprintf( buffer, sizeof( buffer ), "= %d", *( int8_t * )data ) ;
            return buffer ;

        case TYPE_CHAR:
            if ( *( uint8_t * )data >= 32 )
                _snprintf( buffer, sizeof( buffer ), "= '%c'", *( uint8_t * )data ) ;
            else
                _snprintf( buffer, sizeof( buffer ), "= '\\x%02X'", *( uint8_t * )data ) ;
            return buffer ;

        case TYPE_FLOAT:
            _snprintf( buffer, sizeof( buffer ), "= %g", *( float * )data ) ;
            return buffer ;

        case TYPE_WORD:
            _snprintf( buffer, sizeof( buffer ), "= %d", *( uint16_t * )data ) ;
            return buffer ;

        case TYPE_DWORD:
            _snprintf( buffer, sizeof( buffer ), "= %ud", *( uint32_t * )data ) ;
            return buffer ;

        case TYPE_SHORT:
            _snprintf( buffer, sizeof( buffer ), "= %d", *( int16_t * )data ) ;
            return buffer ;

        case TYPE_INT:
            _snprintf( buffer, sizeof( buffer ), "= %d", *( int * )data ) ;
            return buffer ;

        case TYPE_POINTER:
            _snprintf( buffer, sizeof( buffer ), "= 0x%08X", *( uint32_t * )data ) ;
            return buffer ;

        default:
            return "?" ;
    }
}

/* --------------------------------------------------------------------------- */

static void show_struct( int num, char * title, int indent, void * data ) {
    int n, count ;
    DCB_VAR * vars ;

    vars = dcb.varspace_vars[num] ;
    count = dcb.varspace[num].NVars ;

    for ( n = 0 ; n < count ; n++ ) {
        show_var( vars[n], 0, data ? ( uint8_t * )data + vars[n].Offset : 0, title, indent ) ;
    }
}

/* --------------------------------------------------------------------------- */

static void show_var( DCB_VAR var, char * name, void * data, char * title, int indent ) {

    if ( !name ) {
        unsigned int code ;

        name = "?" ;
        for ( code = 0 ; code < dcb.data.NID ; code++ ) {
            if ( dcb.id[code].Code == var.ID ) {
                name = ( char * ) dcb.id[code].Name ;
                break ;
            }
        }
    }

    if ( data ) console_printf( "%s%*s %s %s %s\n", title, indent, "", describe_type( var.Type, 0 ), name, show_value( var.Type, data ) ) ;
    else        console_printf( "%s%*s %s %s\n", title, indent, "", describe_type( var.Type, 0 ), name ) ;

    if ( var.Type.BaseType[0] == TYPE_STRUCT ) {
        show_struct( var.Type.Members, title, indent + 3, data ) ;
        console_printf( "%s%*s END", title, indent,"" ) ;
    }
}

/* --------------------------------------------------------------------------- */

/* Very simple tokenizer */

static void get_token() {
    char * ptr ;
    unsigned int n ;

    while ( isspace( *token_ptr ) ) token_ptr++ ;

    if ( !*token_ptr ) {
        token.type = NOTOKEN ;
        return ;
    }

    /* Numbers */

    if ( ISNUM( *token_ptr ) ) {
        const char * ptr;
        double num = 0, dec;
        int base = 10;

        /* Hex/Bin/Octal numbers with the h/b/o sufix */
        ptr = token_ptr;
        while ( ISNUM( *ptr ) || ( *ptr >= 'a' && *ptr <= 'f' ) || ( *ptr >= 'A' && *ptr <= 'F' ) ) ptr++;

        if ( *ptr != 'h' && *ptr != 'H' && *ptr != 'o' && *ptr != 'O' && ( ptr[-1] == 'b' || ptr[-1] == 'B' ) ) ptr--;

        if ( *ptr == 'b' || *ptr == 'B' ) base = 2;
        if ( *ptr == 'h' || *ptr == 'H' ) base = 16;
        if ( *ptr == 'o' || *ptr == 'O' ) base = 8;

        token.code = 0 ; /* for ints values */

        /* Calculate the number value */

        while ( ISNUM( *token_ptr ) || ( base > 10 && ISALNUM( *token_ptr ) ) ) {
            if ( base == 2 && *token_ptr != '0' && *token_ptr != '1' ) break;
            if ( base == 8 && ( *token_ptr < '0' || *token_ptr > '7' ) ) break;
            if ( base == 10 && !ISNUM( *token_ptr ) ) break;
            if ( base == 16 && !ISNUM( *token_ptr ) && ( TOUPPER( *token_ptr ) < 'A' || TOUPPER( *token_ptr ) > 'F' ) ) break;

            if ( ISNUM( *token_ptr ) ) {
                num = num * base + ( *token_ptr - '0' );
                token.code = token.code * base + ( *token_ptr - '0' );
                token_ptr++;
            }
            if ( *token_ptr >= 'a' && *token_ptr <= 'f' && base > 10 ) {
                num = num * base + ( *token_ptr - 'a' + 10 );
                token.code = token.code * base + ( *token_ptr - 'a' + 10 );
                token_ptr++;
            }
            if ( *token_ptr >= 'A' && *token_ptr <= 'F' && base > 10 ) {
                num = num * base + ( *token_ptr - 'A' + 10 );
                token.code = token.code * base + ( *token_ptr - 'A' + 10 );
                token_ptr++;
            }
        }
        token.type = NUMBER;
        token.code = ( float )num;

        /* We have the integer part now - convert to int/float */

        if ( *token_ptr == '.' && base == 10 ) {
            token_ptr++;
            if ( !ISNUM( *token_ptr ) )
                token_ptr--;
            else {
                dec = 1.0 / ( double )base;
                while ( ISNUM( *token_ptr ) || ( base > 100 && ISALNUM( *token_ptr ) ) ) {
                    if ( ISNUM( *token_ptr ) ) num = num + dec * ( *token_ptr++ - '0' );
                    if ( *token_ptr >= 'a' && *token_ptr <= 'f' && base > 10 ) num = num + dec * ( *token_ptr++ - 'a' + 10 );
                    if ( *token_ptr >= 'A' && *token_ptr <= 'F' && base > 10 ) num = num + dec * ( *token_ptr++ - 'A' + 10 );
                    dec /= ( double )base;
                }
                token.type  = NUMBER;
                token.code = ( float )num;
            }
        }

        /* Skip the base sufix */

        if ( base == 16 && ( *token_ptr == 'h' || *token_ptr == 'H' ) ) token_ptr++;
        if ( base == 8  && ( *token_ptr == 'o' || *token_ptr == 'O' ) ) token_ptr++;
        if ( base == 2  && ( *token_ptr == 'b' || *token_ptr == 'B' ) ) token_ptr++;

        _snprintf( token.name, sizeof( token.name ), "%g", token.code ) ;
        return ;
    }

    if ( *token_ptr == '"' || *token_ptr == '\'' ) { /* Cadena */
        char c = *token_ptr++ ;
        token.type = STRING ;
        ptr = token.name;
        while ( *token_ptr && *token_ptr != c ) *ptr++ = *token_ptr++ ;
        if ( *token_ptr == c ) token_ptr++ ;
        *ptr = 0 ;
        return ;
    }

    ptr = token.name ;
    *ptr++ = TOUPPER( *token_ptr ) ;
    if ( ISWORDCHAR( *token_ptr++ ) ) {
        while ( ISWORDCHAR( *token_ptr ) )
            *ptr++ = TOUPPER( *token_ptr++ ) ;
    }
    *ptr = 0 ;

    for ( n = 0 ; n < dcb.data.NID ; n++ ) {
        if ( strcmp( ( char * )dcb.id[n].Name, token.name ) == 0 ) {
            token.type = IDENTIFIER ;
            token.code = dcb.id[n].Code ;
/*            strcpy( token.name, (char *)dcb.id[n].Name ) ; */
            return ;
        }
    }

    token.type = OPERATOR ;
}

/* --------------------------------------------------------------------------- */

static DCB_TYPEDEF reduce_type( DCB_TYPEDEF orig ) {
    int n ;
    for ( n = 0 ; n < MAX_TYPECHUNKS - 1 ; n++ ) {
        orig.BaseType[n] = orig.BaseType[n+1] ;
        orig.Count[n] = orig.Count[n+1] ;
    }
    return orig ;
}

/* --------------------------------------------------------------------------- */

static void var2const() {
    while ( result.type == T_VARIABLE && result.var.Type.BaseType[0] == TYPE_ARRAY )
        result.var.Type = reduce_type( result.var.Type ) ;

    if ( result.type == T_VARIABLE && result.var.Type.BaseType[0] == TYPE_STRING ) {
        result.type = T_STRING ;
        strncpy( result.name, string_get( *( int * )( result.data ) ), 127 ) ;
        result.name[127] = 0 ;
    }

    if ( result.type == T_VARIABLE && result.var.Type.BaseType[0] == TYPE_FLOAT ) {
        result.type = T_CONSTANT ;
        result.value = *( float * )( result.data ) ;
    }

    if ( result.type == T_VARIABLE && ( result.var.Type.BaseType[0] == TYPE_DWORD || result.var.Type.BaseType[0] == TYPE_INT ) ) {
        result.type = T_CONSTANT ;
        result.value = *( int * )( result.data ) ;
    }

    if ( result.type == T_VARIABLE && ( result.var.Type.BaseType[0] == TYPE_WORD || result.var.Type.BaseType[0] == TYPE_SHORT ) ) {
        result.type = T_CONSTANT ;
        result.value = *( int16_t * )( result.data ) ;
    }

    if ( result.type == T_VARIABLE && ( result.var.Type.BaseType[0] == TYPE_BYTE || result.var.Type.BaseType[0] == TYPE_SBYTE ) ) {
        result.type = T_CONSTANT ;
        result.value = *( int8_t * )( result.data ) ;
    }

    if ( result.type == T_VARIABLE && result.var.Type.BaseType[0] == TYPE_CHAR ) {
        result.type = T_STRING ;
        if ( *( uint8_t * )result.data >= 32 )
            _snprintf( result.name, sizeof( result.name ), "%c", *( uint8_t * )result.data ) ;
        else
            _snprintf( result.name, sizeof( result.name ), "\\x%02X", *( uint8_t * )result.data ) ;
    }
}

/* --------------------------------------------------------------------------- */

static int type_size( DCB_TYPEDEF orig ) {
    unsigned int n, total ;

    switch ( orig.BaseType[0] ) {
        case TYPE_ARRAY:
            return orig.Count[0] * type_size( reduce_type( orig ) ) ;

        case TYPE_POINTER:
        case TYPE_STRING:
        case TYPE_DWORD:
        case TYPE_FLOAT:
        case TYPE_INT:
            return 4 ;

        case TYPE_WORD:
        case TYPE_SHORT:
            return 2 ;

        case TYPE_BYTE:
        case TYPE_SBYTE:
        case TYPE_CHAR:
            return 1 ;

        case TYPE_STRUCT:
            total = 0 ;
            for ( n = 0 ; n < dcb.varspace[orig.Members].NVars ; n++ )
                total += type_size( dcb.varspace_vars[orig.Members][n].Type ) ;
            return total ;

        default:
            return 0 ;
    }
}

/* --------------------------------------------------------------------------- */

static void eval_local( DCB_PROC * proc, INSTANCE * i ) {
    unsigned int n ;

    for ( n = 0 ; n < dcb.data.NLocVars ; n++ ) {
        if ( dcb.locvar[n].ID == token.code ) {
            strcpy( result.name, token.name ) ;
            result.type = T_VARIABLE ;
            result.var  = dcb.locvar[n] ;
            result.data = ( uint8_t * )i->locdata + dcb.locvar[n].Offset ;
            get_token() ;
            return ;
        }
    }

    for ( n = 0 ; n < proc->data.NPriVars ; n++ ) {
        if ( proc->privar[n].ID == token.code ) {
            strcpy( result.name, token.name ) ;
            result.type = T_VARIABLE ;
            result.var  = proc->privar[n] ;
            result.data = ( uint8_t * )i->pridata + proc->privar[n].Offset ;
            get_token() ;
            return ;
        }
    }

    for ( n = 0 ; n < proc->data.NPubVars ; n++ ) {
        if ( proc->pubvar[n].ID == token.code ) {
            strcpy( result.name, token.name ) ;
            result.type = T_VARIABLE ;
            result.var  = proc->pubvar[n] ;
            result.data = ( uint8_t * )i->pubdata + proc->pubvar[n].Offset ;
            get_token() ;
            return ;
        }
    }

    console_printf( "\25402Local or private or public variable not found\25407" ) ;
    result.type = T_ERROR ;
}

/* --------------------------------------------------------------------------- */

static void eval_immediate() {
    unsigned int n ;

    if ( token.type == NUMBER ) {
        result.type = T_CONSTANT ;
        result.value = token.code ;
        get_token() ;
        return ;
    }

    if ( token.type == STRING ) {
        result.type = T_STRING ;
        _snprintf( result.name, sizeof( result.name ), "%s", token.name ) ;
        get_token() ;
        return ;
    }

    if ( token.type != IDENTIFIER ) {
        console_printf( "\25402Not a valid expression\25407" ) ;
        result.type = T_ERROR ;
        return ;
    }

    if ( token.name[0] == '(' ) {
        get_token() ;
        eval_subexpression() ;
        if ( token.name[0] != ')' ) {
            console_printf( "\25402Unbalanced parens\25407" ) ;
            result.type = T_ERROR ;
            return ;
        }
        get_token() ;
        return ;
    }

    if ( token.name[0] == '-' ) {
        get_token() ;
        eval_immediate() ;
        var2const() ;
        if ( result.type != T_CONSTANT ) {
            console_printf( "\25402Operand is not a number\25407" ) ;
            result.type = T_ERROR ;
            return ;
        }
        result.value = -result.value ;
        _snprintf( result.name, sizeof( result.name ), "%g", result.value ) ;
        return ;
    }

    for ( n = 0 ; n < dcb.data.NGloVars ; n++ ) {
        if ( dcb.glovar[n].ID == token.code ) {
            strcpy( result.name, token.name ) ;
            result.type = T_VARIABLE ;
            result.var  = dcb.glovar[n] ;
            result.data = ( uint8_t * )globaldata + dcb.glovar[n].Offset ;

            get_token() ;
            return ;
        }
    }

    if ( strcmp( token.name, "MAIN" ) == 0 )
        token.code = dcb.proc[0].data.ID ;

    for ( n = 0 ; n < dcb.data.NProcs ; n++ ) {
        if ( dcb.proc[n].data.ID == token.code ) {
            INSTANCE * i = first_instance ;

            while ( i ) {
                if ( i->proc->type == ( int )n ) break ;
                i = i->next ;
            }
            if ( !i ) {
                console_printf( "\25402No instance of process %s is active\25407", token.name ) ;
                result.type = T_ERROR ;
                return ;
            }

            get_token() ;
            if ( token.name[0] != '.' ) {
                console_printf( "\25402Invalid use of a process name\25407" ) ;
                result.type = T_ERROR ;
                return ;
            }
            get_token() ;
            eval_local( &dcb.proc[n], i ) ;
            return ;
        }
    }

    console_printf( "\25402Variable does not exist (%s)\25407", token.name ) ;
    result.type = T_ERROR ;
    return ;
}

/* --------------------------------------------------------------------------- */

static void eval_value() {
    eval_immediate() ;
    if ( result.type == T_ERROR ) return ;

    for ( ;; ) {
        if ( token.name[0] == '.' ) {
            DCB_VARSPACE * v ;
            DCB_VAR * var ;
            unsigned int n ;

            var2const() ;
            if ( result.type == T_CONSTANT ) {
                INSTANCE * i ;
                i = instance_get(( int )result.value ) ;
                if ( !i ) {
                    result.type = T_ERROR ;
                    console_printf( "\25402Instance %d does not exist\25407", ( int )result.value ) ;
                    return ;
                }
                get_token() ;
                eval_local( &dcb.proc[i->proc->type], i ) ;
                continue;
            }

            if ( result.type != T_VARIABLE
                    || result.var.Type.BaseType[0] != TYPE_STRUCT ) {
                console_printf( "\25402%s is not an struct\25407", result.name );
                result.type = T_ERROR ;
                return ;
            }
            get_token() ;
            if ( token.type != IDENTIFIER ) {
                console_printf( "\25402%s is not a member\25407", token.name ) ;
                result.type = T_ERROR ;
                return ;
            }

            v = &dcb.varspace[result.var.Type.Members] ;
            var = dcb.varspace_vars[result.var.Type.Members] ;
            for ( n = 0 ; n < v->NVars ; n++ ) {
                if ( var[n].ID == token.code )
                    break ;
            }
            if ( n == v->NVars ) {
                console_printf( "\25402%s is not a member\25407", token.name ) ;
                result.type = T_ERROR ;
                return ;
            }

            result.type = T_VARIABLE ;
            result.var = var[n] ;
            result.data = ( uint8_t * )result.data + var[n].Offset ;
            strcat( result.name, "." ) ;
            strcat( result.name, token.name ) ;
            get_token() ;
            continue ;
        }

        if ( token.name[0] == '[' ) {
            DCB_VAR i = result.var ;
            void * i_data = result.data ;
            char name[230] ;

            if ( result.type != T_VARIABLE || result.var.Type.BaseType[0] != TYPE_ARRAY ) {
                console_printf( "\25402%s is not an array\25407", result.name ) ;
                result.type = T_ERROR ;
                return ;
            }

            strcpy( name, result.name ) ;
            get_token() ;
            eval_subexpression() ;
            if ( result.type == T_ERROR ) return ;
            if ( token.name[0] == ']' ) get_token() ;
            var2const() ;

            if ( result.type != T_CONSTANT ) {
                console_printf( "\25402%s is not an integer\25407", result.name ) ;
                result.type = T_ERROR ;
                return ;
            }
            if ( result.value < 0 ) {
                console_printf( "\25402Index (%d) less than zero\25407", result.value ) ;
                result.type = T_ERROR ;
                return ;
            }
            if ( result.value >= i.Type.Count[0] ) {
                console_printf( "\25402Index (%d) out of bounds\25407", result.value ) ;
                result.type = T_ERROR ;
                return ;
            }

            result.type = T_VARIABLE ;
            result.var = i ;
            result.var.Type = reduce_type( i.Type ) ;
            result.data = ( uint8_t * )i_data + ( int )result.value * type_size( result.var.Type ) ;
            _snprintf( result.name, sizeof( result.name ), "%s[%d]", name, ( int )result.value ) ;
            continue ;
        }

        break ;
    }
}

/* --------------------------------------------------------------------------- */

static void eval_factor() {
    double base = 1 ;
    int op = 0 ;

    for ( ;; ) {
        eval_value() ;
        if ( result.type == T_ERROR ) return ;
        if ( !strchr( "*/%", token.name[0] ) && !op ) return ;
        var2const() ;
        if ( result.type != T_CONSTANT ) {
            result.type = T_ERROR ;
            console_printf( "\25402Operand is not a number\25407" ) ;
            return ;
        }
        if ( !op ) op = 1 ;
        if ( op > 1 && !result.value ) {
            result.type = T_ERROR ;
            console_printf( "\25402Divide by zero\25407" ) ;
            return ;
        }
        if ( op == 1 ) base *= result.value ;
        if ( op == 2 ) base /= result.value ;
        if ( op == 3 ) base = ( int )base % ( int )result.value ;
        if ( !strchr( "*/%", token.name[0] ) ) {
            result.type = T_CONSTANT ;
            result.value = base ;
            _snprintf( result.name, sizeof( result.name ), "%g", base ) ;
            return ;
        }
        op = ( token.name[0] == '*' ) ? 1 : ( token.name[0] == '/' ) ? 2 : 3 ;
        get_token() ;
    }
}

/* --------------------------------------------------------------------------- */

static void eval_subexpression() {
    double base = 0 ;
    int op = 0 ;

    for ( ;; ) {
        eval_factor() ;
        if ( result.type == T_ERROR ) return ;
        if ( token.name[0] != '+' && token.name[0] != '-' && !op ) return ;
        var2const() ;
        if ( result.type != T_CONSTANT ) {
            result.type = T_ERROR ;
            console_printf( "\25402Operand is not a number\25407" ) ;
            return ;
        }
        if ( !op ) op = 1 ;
        base += op * result.value ;
        if ( token.name[0] != '+' && token.name[0] != '-' ) {
            result.type = T_CONSTANT ;
            result.value = base ;
            _snprintf( result.name, sizeof( result.name ), "%g", base ) ;
            return ;
        }
        op = ( token.name[0] == '+' ) ? 1 : -1 ;
        get_token() ;
    }
}

/* --------------------------------------------------------------------------- */

static char * eval_expression( const char * here, int interactive ) {
    static char buffer[1088];
    static char part[1024];

    while ( *here == ' ' ) here++;

    token_ptr = here ;
    get_token() ;
    eval_subexpression() ;

    if ( token.type != NOTOKEN && token.name[0] != ',' && token.name[0] != '=' ) {
        if ( result.type != T_ERROR ) {
            console_printf( "\25402Invalid expression\25407" );
            result.type = T_ERROR;
        }
        return 0;
    }

    memset( part, 0, sizeof( part ) );
    strncpy( part, here, token_ptr - here - (( token.type != NOTOKEN ) ? 1 : 0 ) );

    if ( result.type == T_CONSTANT ) {
        _snprintf( buffer, sizeof( buffer ), "%s = %g", part, result.value );
        if ( interactive ) console_printf( "\25407%s", buffer ) ;
    } else if ( result.type == T_STRING ) {
        if ( interactive ) console_printf( "\25407%s = \"%s\"", part, result.name ) ;
    } else if ( result.type == T_VARIABLE ) {
        lvalue = result ;

        if ( token.name[0] == '=' ) {
            if ( lvalue.type != T_VARIABLE ) {
                strcpy( buffer, "Not an lvalue" ) ;
                if ( interactive ) console_printf( "\25402%s\25407", buffer ) ;
                return buffer ;
            }
            get_token() ;
            eval_subexpression() ;
            if ( result.type == T_ERROR ) return "" ;
            var2const() ;

            if (( lvalue.var.Type.BaseType[0] == TYPE_DWORD || lvalue.var.Type.BaseType[0] == TYPE_INT ) && result.type == T_CONSTANT )
                *( int * )( lvalue.data ) = ( int )result.value ;
            else if (( lvalue.var.Type.BaseType[0] == TYPE_WORD || lvalue.var.Type.BaseType[0] == TYPE_SHORT ) && result.type == T_CONSTANT )
                *( uint16_t * )( lvalue.data ) = ( uint16_t )result.value ;
            else if (( lvalue.var.Type.BaseType[0] == TYPE_BYTE || lvalue.var.Type.BaseType[0] == TYPE_SBYTE ) && result.type == T_CONSTANT )
                *( uint8_t * )( lvalue.data ) = ( uint8_t )result.value ;
            else if ( lvalue.var.Type.BaseType[0] == TYPE_CHAR && result.type == T_STRING ) {
                if ( *result.name == '\\' && *( result.name + 1 ) == 'x' )
                    *( uint8_t * )( lvalue.data ) = ( uint8_t )strtol( result.name + 2, NULL, 16 );
                else
                    *( uint8_t * )( lvalue.data ) = ( uint8_t ) * ( result.name );
            } else if ( lvalue.var.Type.BaseType[0] == TYPE_FLOAT && result.type == T_CONSTANT )
                *( float * )( lvalue.data ) = ( float )result.value ;
            else if ( lvalue.var.Type.BaseType[0] == TYPE_STRING && result.type == T_STRING ) {
                string_discard( *( uint32_t * ) lvalue.data ) ;
                *( uint32_t * )( lvalue.data ) = string_new( result.name ) ;
                string_use( *( uint32_t * ) lvalue.data ) ;
            } else {
                strcpy( buffer, "Invalid assignation" ) ;
                if ( interactive ) console_printf( "\25402%s\25407", buffer ) ;
                return buffer ;
            }
        }

        if ( interactive ) {
            show_var( lvalue.var, lvalue.name, lvalue.data, "", 0 ) ;
        } else {
            strcpy( buffer, part ) ;
            strcat( buffer, " " ) ;
            strcat( buffer, show_value( lvalue.var.Type, lvalue.data ) );
        }
    }

    if ( token.name[0] == ',' ) {
        char * temporary = bgd_strdup( buffer );
        int    size = strlen( temporary );

        if ( eval_expression( token_ptr, interactive ) == 0 ) {
            bgd_free( temporary );
            return 0;
        }
        if ( strlen( buffer ) + size < 1020 && !interactive ) {
            memmove( buffer + size + 2, buffer, strlen( buffer ) + 1 );
            memcpy( buffer, temporary, size );
            memcpy( buffer + size, ", ", 2 );
        }
        bgd_free( temporary );
        return buffer;
    }

    return buffer;
}

/* --------------------------------------------------------------------------- */

static void console_instance_dump( INSTANCE * father, int indent ) {
    INSTANCE * i, * next ;
    PROCDEF * proc ;
    DCB_PROC * dcbproc ;
    char buffer[128] ;
    char line[128] ;
    int n, nid, bigbro, son ;

    i = father ;
    if ( !father ) i = first_instance ;

    proc = i->proc ;
    dcbproc = &dcb.proc[proc->type] ;

    nid = LOCDWORD( mod_debug, i, PROCESS_ID ) ;
    if ( dcb.data.NSourceFiles && dcbproc->data.ID )
        sprintf( buffer, "%s", getid_name( dcbproc->data.ID ) ) ;
    else
        sprintf( buffer, "%s", ( proc->type == 0 ) ? "MAIN" : "PROC" ) ;

    line[0] = 0 ;
    for ( n = 0 ; n < indent - 1; n++ ) strncat( line, " \x03 ", sizeof(line) - strlen(line) -1 );

    snprintf( line + strlen( line ), sizeof(line) -strlen(line) , " \x01\x02 %-12.124s ", buffer ) ;

    n = strlen( line ) % 4 ;
    while ( n && ( n++ < 4 ) ) strcat( line, " " ) ;
    sprintf( line + strlen( line ), "%7d", nid ) ;

    if ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) strcat( line, "[W]" );
    switch ( LOCDWORD( mod_debug, i, STATUS ) & ~STATUS_WAITING_MASK ) {
        case STATUS_DEAD        :   strcat( line, "[D]" ) ; break ;
        case STATUS_KILLED      :   strcat( line, "[K]" ) ; break ;
        case STATUS_RUNNING     :   strcat( line, "   " ) ; break ;
        case STATUS_SLEEPING    :   strcat( line, "[S]" ) ; break ;
        case STATUS_FROZEN      :   strcat( line, "[F]" ) ; break ;
    }

    console_printf( "\25407%s", line ) ;

    if ( !( son = LOCDWORD( mod_debug, i, SON ) ) ) return ;

    next = instance_get( son ) ;
    if ( !next ) console_printf( "\25402\12**PANIC**\7 SON %d does not exist\25407", son ) ;

    i = next ;

    while ( i ) {
        proc = i->proc ;
        dcbproc = &dcb.proc[proc->type] ;

        if ( LOCDWORD( mod_debug, i, SON ) ) {
            console_instance_dump( i, indent + 1 ) ;
        } else {
            nid = LOCDWORD( mod_debug, i, PROCESS_ID ) ;
            if ( dcb.data.NSourceFiles && dcbproc->data.ID ) {
                sprintf( buffer, "%s", getid_name( dcbproc->data.ID ) ) ;
            } else {
                sprintf( buffer, "%s", ( proc->type == 0 ) ? "MAIN" : "PROC" ) ;
            }

            line[0] = 0 ;
            for ( n = 0 ; n < indent; n++ ) strncat( line, " \x03 ", sizeof(line) - strlen(line) -1 );

            snprintf( line + strlen( line ), sizeof(line) -strlen(line), " \x01\x02 %-12s ", buffer ) ;

            n = strlen( line ) % 4 ;
            while ( n && ( n++ < 4 ) ) strcat( line, " " ) ;
            sprintf( line + strlen( line ), "%7d", nid ) ;

            if ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) strcat( line, "[W]" );
            switch ( LOCDWORD( mod_debug, i, STATUS ) & ~STATUS_WAITING_MASK ) {
                case STATUS_DEAD        :   strcat( line, "[D]" ) ; break ;
                case STATUS_KILLED      :   strcat( line, "[K]" ) ; break ;
                case STATUS_RUNNING     :   strcat( line, "   " ) ; break ;
                case STATUS_SLEEPING    :   strcat( line, "[S]" ) ; break ;
                case STATUS_FROZEN      :   strcat( line, "[F]" ) ; break ;
            }
            console_printf( "\25407%s", line ) ;
        }

        if ( ( bigbro = LOCDWORD( mod_debug, i, BIGBRO ) ) ) {
            next = instance_get( bigbro ) ;
            if ( !next ) console_printf( "\25402\12**PANIC**\7 BIGBRO %d does not exist\25407", bigbro ) ;
            i = next ;
        } else
            break ;
    }
}

/* --------------------------------------------------------------------------- */
/* Muestra el árbol de instancias */

static void console_instance_dump_all() {
    INSTANCE * i ;
    int father;

    console_printf( "\25404INSTANCES TREE\25407\n\n" );

    for ( i = first_instance ; i ; i = i->next ) {
        if ( !( father = LOCDWORD( mod_debug, i, FATHER )) || !instance_get( father ) ) {
            console_instance_dump( i, 1 ) ;
        }
    }

    console_printf( "\n" ) ;
}

/* --------------------------------------------------------------------------- */

static void console_instance_dump_all_brief() {
    INSTANCE * i ;
    char status[30] ;
    int father;

    console_printf( "\25404INSTANCES BRIEF LIST\25407\n\n" );
    console_printf( "\25404Id         Father     Status        Name\25407\n" );


    for ( i = first_instance ; i ; i = i->next ) {
        status[0] = '\0';
        if ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK )
            strcpy( status, "\25408wait\25407+" );

        switch ( LOCDWORD( mod_debug, i, STATUS ) & ~STATUS_WAITING_MASK ) {
            case STATUS_DEAD        :   strcat( status, "\25406dead\25407    " ) ; break ;
            case STATUS_KILLED      :   strcat( status, "\25402killed\25407  " ) ; break ;
            case STATUS_SLEEPING    :   strcat( status, "\25409sleeping\25407" ) ; break ;
            case STATUS_FROZEN      :   strcat( status, "\25405frozen\25407  " ) ; break ;
            case STATUS_RUNNING     :   strcat( status, "\25412running\25407 " ) ; break ;
        }

        if ( !( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) )
            strcat( status, "     " );


        father = LOCDWORD( mod_debug, i, FATHER );

        console_printf( "%-10d %s%-10d\25407 %s \25415%s\25407\n",
                LOCDWORD( mod_debug, i, PROCESS_ID ),
                instance_get( father ) ? "" : "\25402",
                father,
                status,
                ( dcb.data.NSourceFiles && dcb.proc[i->proc->type].data.ID ) ? getid_name( dcb.proc[i->proc->type].data.ID ) : (( i->proc->type == 0 ) ? "Main" : "proc" )
                      ) ;
    }

    console_printf( "\n" ) ;
}

/* --------------------------------------------------------------------------- */

static int console_list_current = 0;
static int console_list_y_pos[2] = { 0 };
static int console_list_x_pos[2] = { 0 };
static int console_list_y_max[2] = { 0 };
static INSTANCE * console_list_current_instance = NULL;

#define WINDOW_LIST_COLS    40

static void show_list_window() {
    int n;
    int pos = console_list_y_pos[console_list_current] > console_y / CHARHEIGHT - 2 ? console_list_y_pos[console_list_current] - ( console_y / CHARHEIGHT - 2 ) : 0 ;
    int x = ( scrbitmap->width - console_columns * CHARWIDTH ) / 2 + ( console_columns - WINDOW_LIST_COLS ) * CHARWIDTH ;
    int y = 0;

    systext_color( 0x000000, 0xC0C0C0 ) ;
    switch ( console_list_current ) {
        case    0:
            console_list_y_max[0] = procdef_count ;
            systext_puts( scrbitmap, x, y, "\25400\25421PROCESS TYPES", WINDOW_LIST_COLS );
            for ( n = 0 ; pos + n < procdef_count && y < console_y - CHARHEIGHT ; n++ ) {
                y += CHARHEIGHT;
                if ( console_list_y_pos[console_list_current] == pos + n ) {
                    if ( procs[pos + n].breakpoint )
                        systext_puts( scrbitmap, x, y, "\25415\25419", 1 );
                    else
                        systext_puts( scrbitmap, x, y, "\25415\25417", 1 );
                } else {
                    if ( procs[pos + n].breakpoint )
                        systext_puts( scrbitmap, x, y, "\25415\25418", 1 );
                    else
                        systext_puts( scrbitmap, x, y, "\25400\25423", 1 );
                }
                if ( console_list_x_pos[console_list_current] < strlen( procs[pos + n].name ) )
                    systext_puts( scrbitmap, x, y, &procs[pos + n].name[console_list_x_pos[console_list_current]], WINDOW_LIST_COLS );
                else
                    systext_puts( scrbitmap, x, y, "", WINDOW_LIST_COLS );
            }
            break;

        case    1:
        {
            INSTANCE * i ;
            int c ;
            char line[128];
            char status[10];

            console_list_y_max[1] = 0 ;

            systext_puts( scrbitmap, x, y, "\25400\25421INSTANCES", WINDOW_LIST_COLS );

            for ( c = 0, n = 0, i = first_instance ; i ; i = i->next, c++ ) {
                console_list_y_max[1]++ ;

                if ( c < pos || y >= console_y - CHARHEIGHT ) continue ;
                y += CHARHEIGHT;

                if ( console_list_y_pos[console_list_current] == pos + n ) {
                    console_list_current_instance = i;

                    if ( i->breakpoint )
                        systext_puts( scrbitmap, x, y, "\25415\25419", 1 );
                    else
                        systext_puts( scrbitmap, x, y, "\25415\25417", 1 );
                } else {
                    if ( i->breakpoint )
                        systext_puts( scrbitmap, x, y, "\25415\25418", 1 );
                    else
                        systext_puts( scrbitmap, x, y, "\25400\25423", 1 );
                }

                status[0] = '\0';
                if ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) strcat( status, "[W]" );
                switch ( LOCDWORD( mod_debug, i, STATUS ) & ~STATUS_WAITING_MASK ) {
                    case STATUS_DEAD        :   strcat( status, "[D]" ) ; break ;
                    case STATUS_KILLED      :   strcat( status, "[K]" ) ; break ;
                    case STATUS_SLEEPING    :   strcat( status, "[S]" ) ; break ;
                    case STATUS_FROZEN      :   strcat( status, "[F]" ) ; break ;
                    case STATUS_RUNNING     :   strcat( status, "[R]" ) ; break ;
                }

                sprintf( line, "%d %-6.6s %s",
                        LOCDWORD( mod_debug, i, PROCESS_ID ),
                        status,
                        ( dcb.data.NSourceFiles && dcb.proc[i->proc->type].data.ID ) ? getid_name( dcb.proc[i->proc->type].data.ID ) : (( i->proc->type == 0 ) ? "Main" : "proc" )
                       );
                if ( console_list_x_pos[console_list_current] < strlen( line ) )
                    systext_puts( scrbitmap, x, y, &line[console_list_x_pos[console_list_current]], WINDOW_LIST_COLS );
                else
                    systext_puts( scrbitmap, x, y, "", WINDOW_LIST_COLS );

                n++ ;
            }
        }
        break;
    }
}

/* --------------------------------------------------------------------------- */

static INSTANCE * findproc( INSTANCE * last, char * action, char * ptr ) {
    INSTANCE * i = NULL;
    char * aptr;
    int procno = 0;
    int n;

    if ( *ptr ) {
        if ( *ptr >= '0' && *ptr <= '9' ) {
            if ( last ) return NULL;
            procno = atoi( ptr );
            for ( i = first_instance ; i ; i = i->next )
                if ( LOCDWORD( mod_debug, i, PROCESS_ID ) == procno )
                    break;
            if ( !i ) {
                console_printf( "\25402Instance %d does not exist\25407", procno );
                return NULL;
            }
        } else {
            aptr = action;
            while ( ISWORDCHAR( *ptr ) ) {
                *aptr++ = TOUPPER( *ptr ); ptr++;
            }
            *aptr = 0;

            for ( n = 0 ; n < ( int )dcb.data.NID ; n++ )
                if ( strcmp( ( char * )dcb.id[n].Name, action ) == 0 )
                    break;
            for ( procno = 0 ; procno < ( int )dcb.data.NProcs ; procno++ )
                if ( dcb.proc[procno].data.ID == dcb.id[n].Code )
                    break;
            if ( procno == ( int )dcb.data.NProcs ) {
                console_printf( "\25402Unknown process %s\25407", action );
                return NULL;
            }
            for ( i = last ? last->next : first_instance ; i ; i = i->next )
                if ( i->proc->type == procno )
                    break;
            if ( !i && !last ) {
                console_printf( "\25402No instance of process %s created\25407", action );
                return NULL;
            }
        }
    }

    return ( i );
}

/* --------------------------------------------------------------------------- */

static void console_do( const char * command ) {
    char * ptr, * aptr ;
    char action[256] ;
    unsigned int var ;
    int n, procno = 0;
    PROCDEF * p = NULL ;
    INSTANCE * i = NULL ;

    ptr = ( char * ) command ;
    while ( *ptr && *ptr != ' ' ) ptr++ ;
    strncpy( action, command, ptr - command ) ;
    action[ptr - command] = 0 ;
    while ( *ptr == ' ' ) ptr++ ;
    aptr = action ;
    while ( *aptr ) {
        *aptr = TOUPPER( *aptr );
        aptr++;
    }

    /* Comandos */

    if ( strcmp( action, "HELP" ) == 0 ) {
        console_printf( HELPTXT );
        return;
    }

    if ( strcmp( action, "GO" ) == 0 ) {
        SDL_EnableKeyRepeat( 0, 0 );
        debugger_show_console = 0;
        debugger_trace = 0;
        debugger_step = 0;
        debug_on_frame = 0;
        break_on_next_proc = 0;
        return ;
    }

    if ( strcmp( action, "NEXTFRAME" ) == 0 ) {
        SDL_EnableKeyRepeat( 0, 0 );
        debugger_show_console = 0;
        debugger_trace = 0;
        debugger_step = 0;
        debug_on_frame = 1;
        break_on_next_proc = 0;
        return ;
    }

    if ( strcmp( action, "NEXTPROC" ) == 0 ) {
        SDL_EnableKeyRepeat( 0, 0 );
        debugger_show_console = 0;
        debugger_trace = 0;
        debugger_step = 0;
        debug_on_frame = 0;
        break_on_next_proc = 1;
        return ;
    }

    if ( strcmp( action, "TRACE" ) == 0 ) {
        SDL_EnableKeyRepeat( 0, 0 );
        debugger_show_console = 0;
        debugger_trace = 1;
        debugger_step = 0;
        debug_on_frame = 0;
        break_on_next_proc = 0;
        return ;
    }

    if ( strcmp( action, "STEP" ) == 0 ) {
        SDL_EnableKeyRepeat( 0, 0 );
        debugger_show_console = 0;
        debugger_trace = 0;
        debugger_step = 1;
        debug_on_frame = 0;
        break_on_next_proc = 0;
        return ;
    }

    if ( strcmp( action, "BREAK" ) == 0 ) {
        if ( *ptr ) {
            if ( *ptr >= '0' && *ptr <= '9' ) {
                procno = atoi( ptr );
                for ( i = first_instance ; i ; i = i->next )
                    if ( LOCDWORD( mod_debug, i, PROCESS_ID ) == procno )
                        break;
                if ( !i ) {
                    console_printf( "\25402Instance %d does not exist\25407", procno );
                } else {
                    i->breakpoint = 1;
                    console_printf( "\25407OK" );
                }
            } else {
                aptr = action;
                while ( ISWORDCHAR( *ptr ) ) {
                    *aptr++ = TOUPPER( *ptr ); ptr++;
                }
                *aptr = 0;

                if ( *action ) {
                    p = procdef_get_by_name( action );
                    if ( !p ) {
                        console_printf( "\25402Process type %d does not exist\25407", action );
                    } else {
                        p->breakpoint = 1;
                        console_printf( "\25407OK" );
                    }
                }
            }
        } else {
            int f = 0;
            for ( n = 0 ; n < procdef_count; n++ ) {
                if ( procs[n].breakpoint ) {
                    if ( !f ) {
                        console_printf( "\25404PROCESS TYPE BREAKPOINTS\25407\n\n" );
                        f = 1;
                    }
                    console_printf( "\25407%s\n", procs[n].name );
                }
            }
            if ( f )
                console_printf( "\n" );

            f = 0;
            for ( i = first_instance ; i ; i = i->next ) {
                if ( i->breakpoint ) {
                    if ( !f ) {
                        console_printf( "\25404PROCESS BREAKPOINTS\25407\n\n" );
                        f = 1;
                    }
                    console_printf( "\25407%d", LOCDWORD( mod_debug, i, PROCESS_ID ) );
                }
            }
            if ( f ) console_printf( "\n" );
        }
        return ;
    }

    if ( strcmp( action, "BREAKALL" ) == 0 ) {
        for ( i = first_instance ; i ; i = i->next ) i->breakpoint = 1;
        console_printf( "\25407OK" );
        return ;
    }

    if ( strcmp( action, "BREAKALLTYPES" ) == 0 ) {
        for ( n = 0 ; n < procdef_count; n++ ) procs[n].breakpoint = 1;
        console_printf( "\25407OK" );
        return ;
    }

    if ( strcmp( action, "DELETEALL" ) == 0 ) {
        for ( i = first_instance ; i ; i = i->next ) i->breakpoint = 0;
        console_printf( "\25407OK" );
        return ;
    }

    if ( strcmp( action, "DELETEALLTYPES" ) == 0 ) {
        for ( n = 0 ; n < procdef_count; n++ ) procs[n].breakpoint = 0;
        console_printf( "\25407OK" );
        return ;
    }

    if ( strcmp( action, "DELETE" ) == 0 ) {
        if ( *ptr ) {
            if ( *ptr >= '0' && *ptr <= '9' ) {
                procno = atoi( ptr );
                for ( i = first_instance ; i ; i = i->next )
                    if ( LOCDWORD( mod_debug, i, PROCESS_ID ) == procno )
                        break;
                if ( !i ) {
                    console_printf( "\25402Instance %d does not exist\25407", procno );
                } else {
                    i->breakpoint = 0;
                    console_printf( "\25407OK" );
                }
            } else {
                aptr = action;
                while ( ISWORDCHAR( *ptr ) ) {
                    *aptr++ = TOUPPER( *ptr ); ptr++;
                }
                *aptr = 0;

                p = procdef_get_by_name( action );
                if ( !p ) {
                    console_printf( "\25402Process type %d does not exist\25407", procno );
                } else {
                    p->breakpoint = 0;
                    console_printf( "\25407OK" );
                }
            }
        }
        return ;
    }

    if ( strcmp( action, "STRINGS" ) == 0 ) {
        string_dump(console_printf) ;
        return ;
    }

    if ( strcmp( action, "INSTANCES" ) == 0 ) {
        console_instance_dump_all() ;
        return ;
    }

    if ( strcmp( action, "GLOBALS" ) == 0 ) {
        for ( var = 0 ; var < dcb.data.NGloVars ; var++ ) {
            DCB_VAR * v = &dcb.glovar[var] ;
            show_var( *v, 0, ( uint8_t * )globaldata + v->Offset, "[GLO]", 0 ) ;
        }
        console_printf( "\n" );
        return ;
    }

    if (
        strcmp( action, "LOCALS" ) == 0 ||
        strcmp( action, "PRIVATES" ) == 0 ||
        strcmp( action, "PUBLICS" ) == 0 ) {
        int show_locals = action[0] == 'L';
        int show_public = action[0] == 'P' && action[1] == 'U' ;

        i = findproc( NULL, action, ptr );

        if ( show_locals ) {
            for ( var = 0 ; var < dcb.data.NLocVars ; var++ ) {
                DCB_VAR * v = &dcb.locvar[var] ;
                show_var( *v, 0, i ? ( char* )i->locdata + v->Offset : 0, "[LOC]", 0 ) ;
            }
        } else if ( show_public ) {
            if ( !i ) {
                console_printf( "\25407Use: PUBLICS process" );
                return;
            }
            for ( var = 0 ; var < dcb.proc[i->proc->type].data.NPubVars ; var++ ) {
                DCB_VAR * v = &dcb.proc[i->proc->type].pubvar[var] ;

                /* Unnamed private vars are temporary params and loop
                   counters, and are ignored by this command */
                if (( int )v->ID >= 0 ) {
                    show_var( *v, 0, ( char* )i->pubdata + v->Offset, "[PUB]", 0 ) ;
                }
            }
        } else {
            if ( !i ) {
                console_printf( "\25407Use: PRIVATES process" );
                return;
            }
            for ( var = 0 ; var < dcb.proc[i->proc->type].data.NPriVars ; var++ ) {
                DCB_VAR * v = &dcb.proc[i->proc->type].privar[var] ;

                /* Unnamed private vars are temporary params and loop
                   counters, and are ignored by this command */
                if (( int )v->ID >= 0 ) {
                    show_var( *v, 0, ( char* )i->pridata + v->Offset, "[PRI]", 0 ) ;
                }
            }
        }
        console_printf( "\n" );
        return ;
    }

    if ( strcmp( action, "SHOW" ) == 0 ) {
        if ( *ptr ) {
            char * res = eval_expression( ptr, 0 );

            if ( !res || result.type == T_STRING ) {
                console_printf( "\25402Invalid argument\25407" );
                return ;
            }

            for ( n = 0; n < MAX_EXPRESSIONS; n++ ) {
                if ( show_expression[n] && !strcmp( show_expression[n], ptr ) ) {
                    console_printf( "\25402Already exists\25407" );
                    return ;
                }
            }

            for ( n = 0; n < MAX_EXPRESSIONS; n++ ) {
                if ( !show_expression[n] ) {
                    show_expression[n] = bgd_strdup( ptr );
                    show_expression_count++;
                    console_printf( "\25407OK" );
                    return;
                }
            }

            console_printf( "\25402No more expressions are possibles\25407" );
        } else {
            int nn = 0;
            for ( n = 0; n < MAX_EXPRESSIONS; n++ ) {
                if ( show_expression[n] ) {
                    console_printf( "%d: %s", n + 1, show_expression[n] );
                    nn++;
                }
            }

            if ( !nn ) console_printf( "\25402No expressions availables\25407" );
        }

        return;
    }


    if ( strcmp( action, "SHOWDEL" ) == 0 ) {
        if ( *ptr ) {
            char * p = ptr;

            while( ISNUM( *p ) ) p++;

            if ( ISNUM( *ptr ) ) {
                int pos = atol( ptr ) - 1;
                if ( pos >= 0 && pos < MAX_EXPRESSIONS && show_expression[pos] ) {
                    bgd_free( show_expression[pos] );
                    show_expression[pos] = NULL;
                    show_expression_count--;
                    console_printf( "\25407OK" );
                    return;
                }
            }
        }

        console_printf( "\25402Invalid argument\25407" );

        return;
    }

    if ( strcmp( action, "SHOWDELALL" ) == 0 ) {
        for ( n = 0; n < MAX_EXPRESSIONS; n++ ) {
            if ( show_expression[n] ) {
                bgd_free( show_expression[n] );
                show_expression[n] = NULL;
                show_expression_count--;
            }
        }
        console_printf( "\25407OK" );
        return;
    }

    if ( strcmp( action, "VARS" ) == 0 ) {
        for ( var = 0 ; var < N_CONSOLE_VARS ; var++ ) {
            switch ( console_vars[var].type ) {
                case CON_DWORD:
                    console_printf( "\25407%s = %d\n", console_vars[var].name, *( int * )console_vars[var].value ) ;
                    break;

                case CON_DWORD_HEX:
                    console_printf( "\25407%s = %08Xh\n", console_vars[var].name, *( int * )console_vars[var].value ) ;
                    break;
            }
        }
        console_printf( "\n" );
        return ;
    }

    if ( strcmp( action, "RUN" ) == 0 ) {
        if ( *ptr ) {
            aptr = action;
            while ( ISWORDCHAR( *ptr ) ) {
                *aptr++ = TOUPPER( *ptr ); ptr++;
            }
            *aptr = 0;

            if ( *action ) {
                int i;
                INSTANCE * inst ;
                p = procdef_get_by_name( action );
                if ( p ) {
                    token_ptr = ptr ;
                    console_printf( "\25407%s", ptr );
                    inst = instance_new( p, NULL );

                    for ( i = 0; i < p->params; i++ ) {
                        int type = dcb.proc[p->type].privar[i].Type.BaseType[0];
                        get_token() ;
                        eval_subexpression() ;

                        if ( result.type == T_VARIABLE )
                            var2const();

                        switch ( result.type ) {
                            case T_CONSTANT:
                                switch ( type ) {
                                    case    TYPE_FLOAT:
                                        PRIDWORD( inst, 4*i ) = *( int * ) & result.value ;
                                        break;

                                    case    TYPE_INT:
                                    case    TYPE_DWORD:
                                    case    TYPE_POINTER:
                                    case    TYPE_SHORT:
                                    case    TYPE_WORD:
                                    case    TYPE_BYTE:
                                    case    TYPE_SBYTE:
                                    case    TYPE_CHAR:
                                        PRIDWORD( inst, 4*i ) = ( int ) result.value ;
                                        break;

                                    case    TYPE_STRING:
                                    default:
                                        instance_destroy( inst );
                                        console_printf( "\25402Invalid argument %d\25407", i );
                                        return;
                                }
                                break;

                            case T_STRING:
                                PRIDWORD( inst, 4*i ) = ( int ) string_new( result.name ) ;
                                string_use( PRIDWORD( inst, 4*i ) );
                                break;

                            case T_VARIABLE:
                            default:
                                instance_destroy( inst );
                                console_printf( "\25402Invalid argument %d\25407", i );
                                return;
                        }
                    }
                    console_printf( "\25407Process %s is executed", p->name );
                } else {
                    console_printf( "\25402Process %s not found\25407", action );
                }
                return;
            }
        }
    }

    if (
        strcmp( action, "KILLALL" ) == 0 ||
        strcmp( action, "SLEEPALL" ) == 0 ||
        strcmp( action, "WAKEUPALL" ) == 0 ||
        strcmp( action, "FREEZEALL" ) == 0 ) {
        char    act = *action;
        int     found = 0;
        char    * oaction = bgd_strdup( action );
        char    * optr = ptr;

        i = NULL;
        while (( i = findproc( i, action, ptr ) ) ) {
            found = 1;
            switch ( act ) {
                case 'K':
                    LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_KILLED ;
                    break;

                case 'W':
                    LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_RUNNING ;
                    break ;

                case 'S':
                    LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_SLEEPING ;
                    break ;

                case 'F':
                    LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_FROZEN ;
                    break;
            }
            strcpy( action, oaction );
            ptr = optr;
        }

        if ( oaction ) bgd_free( oaction );
        if ( found ) console_printf( "\25407OK" );
        return ;
    }

    if (
        strcmp( action, "KILL" ) == 0 ||
        strcmp( action, "SLEEP" ) == 0 ||
        strcmp( action, "WAKEUP" ) == 0 ||
        strcmp( action, "FREEZE" ) == 0 ) {
        char act = *action;
        i = findproc( NULL, action, ptr );
        if ( !i ) return;

        switch ( act ) {
            case 'K':
                LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_KILLED ;
                break;

            case 'W':
                LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_RUNNING ;
                break ;

            case 'S':
                LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_SLEEPING ;
                break ;

            case 'F':
                LOCDWORD( mod_debug, i, STATUS ) = ( LOCDWORD( mod_debug, i, STATUS ) & STATUS_WAITING_MASK ) | STATUS_FROZEN ;
                break;
        }
        console_printf( "\25407OK" );
        return ;
    }

    if ( strcmp( action, "QUIT" ) == 0 ) {
        bgdrtm_exit( 1 ) ;
        return ;
    }

    /* Variables del ENGINE */

    for ( var = 0 ; var < N_CONSOLE_VARS ; var++ ) {
        if ( strcmp( console_vars[var].name, action ) == 0 ) {
            switch ( console_vars[var].type ) {
                case CON_DWORD:
                    if ( *ptr ) {
                        while ( *ptr == '=' || *ptr == ' ' ) ptr++;
                        eval_expression( ptr, 0 );
                        if ( result.type != T_ERROR )
                            *( int * )console_vars[var].value = ( int ) result.value ;
                    }
                    console_printf( "\25407%s = %d", console_vars[var].name, *( int * )console_vars[var].value ) ;
                    return ;

                case CON_DWORD_HEX:
                    if ( *ptr ) {
                        while ( *ptr == '=' || *ptr == ' ' ) ptr++;
                        eval_expression( ptr, 0 );
                        if ( result.type != T_ERROR )
                            *( int * )console_vars[var].value = ( int ) result.value ;
                    }
                    console_printf( "\25407%s = %08Xh\n", console_vars[var].name, *( int * )console_vars[var].value ) ;
                    return ;
            }
        }
    }

    /* Expresiones */

    eval_expression( command, 1 ) ;
}

/* --------------------------------------------------------------------------- */
/* Hotkey for exit (ALT+X)                                                     */
/* --------------------------------------------------------------------------- */

static int force_exit_cb( SDL_keysym k ) {
    debugger_show_console = 0;
    exit_value = 0;
    must_exit = 1 ;
    return 1;
}

/* --------------------------------------------------------------------------- */
/* Hotkeys for activate/deactivate console                                     */
/* --------------------------------------------------------------------------- */

static int console_keyboard_handler_cb( SDL_keysym k ) {
    char cmd[256];

    if ( dcb.data.NSourceFiles ) {
        if (( k.mod & KMOD_LALT ) && k.sym == SDLK_c ) {
            if ( !debugger_show_console ) {
                SDL_EnableKeyRepeat( 250, 50 );
                debugger_show_console = 1;
            } else {
                SDL_EnableKeyRepeat( 0, 0 );
                debugger_show_console = 0;
            }
            return 1;
        }

        if ( debugger_show_console ) {
            if ( k.sym == SDLK_F1 ) {
                console_do( "HELP" );
                return 1;
            }

            if ( k.mod & KMOD_LALT ) {
                if ( k.sym == SDLK_LEFT ) {
                    if ( console_columns > HOTKEYHELP_SIZE ) console_columns-- ;
                    return 1;
                }

                if ( k.sym == SDLK_RIGHT ) {
                    if ( console_columns < scrbitmap->width / CHARWIDTH ) console_columns++ ;
                    return 1;
                }

                if ( k.sym == SDLK_UP ) {
                    if ( console_lines > 10 ) console_lines-- ;
                    return 1;
                }

                if ( k.sym == SDLK_DOWN ) {
                    if ( console_lines < scrbitmap->height / CHARHEIGHT ) console_lines++ ;
                    return 1;
                }
            }

            if ( k.mod & ( KMOD_LSHIFT | KMOD_RSHIFT ) ) {
                if ( k.sym == SDLK_LEFT ) {
                    if ( console_list_x_pos[console_list_current] > 0 ) console_list_x_pos[console_list_current]-- ;
                    return 1;
                }

                if ( k.sym == SDLK_RIGHT ) {
                    console_list_x_pos[console_list_current]++ ;
                    return 1;
                }

                if ( k.sym == SDLK_UP ) {
                    if ( console_list_y_pos[console_list_current] > 0 ) console_list_y_pos[console_list_current]-- ;
                    return 1;
                }

                if ( k.sym == SDLK_DOWN ) {
                    if ( console_list_y_pos[console_list_current] < console_list_y_max[console_list_current] - 1 ) console_list_y_pos[console_list_current]++ ;
                    return 1;
                }

                if ( k.sym == SDLK_F2 ) {
                    console_instance_dump_all_brief();
                    return 1;
                }

                if ( console_list_current_instance && console_list_current == 1 ) {
                    if ( k.sym == SDLK_F3 ) {
                        int id = LOCDWORD( mod_debug, console_list_current_instance, PROCESS_ID ) ;
                        console_printf( "\25404%s (%d) LOCALS\25407\n\n",
                                ( dcb.data.NSourceFiles && dcb.proc[console_list_current_instance->proc->type].data.ID ) ? getid_name( dcb.proc[console_list_current_instance->proc->type].data.ID ) : (( console_list_current_instance->proc->type == 0 ) ? "Main" : "proc" ),id );
                        sprintf( cmd, "LOCALS %d", id ) ;
                        console_do( cmd );
                        return 1;
                    }

                    if ( k.sym == SDLK_F4 ) {
                        int id = LOCDWORD( mod_debug, console_list_current_instance, PROCESS_ID ) ;
                        console_printf( "\25404%s (%d) PRIVATES\25407\n\n",
                                ( dcb.data.NSourceFiles && dcb.proc[console_list_current_instance->proc->type].data.ID ) ? getid_name( dcb.proc[console_list_current_instance->proc->type].data.ID ) : (( console_list_current_instance->proc->type == 0 ) ? "Main" : "proc" ),id );
                        sprintf( cmd, "PRIVATES %d", id ) ;
                        console_do( cmd );
                        return 1;
                    }

                    if ( k.sym == SDLK_F5 ) {
                        int id = LOCDWORD( mod_debug, console_list_current_instance, PROCESS_ID ) ;
                        console_printf( "\25404%s (%d) PUBLICS\25407\n\n",
                                ( dcb.data.NSourceFiles && dcb.proc[console_list_current_instance->proc->type].data.ID ) ? getid_name( dcb.proc[console_list_current_instance->proc->type].data.ID ) : (( console_list_current_instance->proc->type == 0 ) ? "Main" : "proc" ),id );
                        sprintf( cmd, "PUBLICS %d", id ) ;
                        console_do( cmd );
                        return 1;
                    }
                }

                if ( k.sym == SDLK_F6 ) {
                    console_list_current ^= 1;
                    return 1;
                }

                if ( k.sym == SDLK_F9 ) {
                    switch ( console_list_current ) {
                        case    0:
                            procs[console_list_y_pos[console_list_current]].breakpoint = !procs[console_list_y_pos[console_list_current]].breakpoint;
                            break;

                        case    1:
                            if ( console_list_current_instance ) console_list_current_instance->breakpoint = !console_list_current_instance->breakpoint;
                            break;
                    }
                    return 1;
                }
            }

            if ( k.sym == SDLK_F2 ) {
                console_instance_dump_all() ;
                return 1;
            }

            if ( !( k.mod & ( KMOD_LSHIFT | KMOD_RSHIFT ) ) ) {
                if ( k.sym == SDLK_F5 ) {
                    console_do( "GO" );
                    return 1;
                }

                if ( k.sym == SDLK_F7 ) {
                    console_do( "TRACE" );
                    return 1;
                }

                if ( k.sym == SDLK_F8 ) {
                    console_do( "STEP" );
                    return 1;
                }

                if ( k.sym == SDLK_F10 ) {
                    console_do( "NEXTFRAME" );
                    return 1;
                }

                if ( k.sym == SDLK_F11 ) {
                    console_do( "NEXTPROC" );
                    return 1;
                }
            }

            if ( k.sym == SDLK_PAGEUP ) {
                console_scroll( console_lines ) ;
                return 1;
            }

            if ( k.sym == SDLK_PAGEDOWN ) {
                console_scroll( -console_lines ) ;
                return 1;
            }

            if ( k.mod & ( KMOD_RCTRL | KMOD_LCTRL ) ) {
                if ( k.sym == SDLK_LEFT ) {
                    console_lateral_scroll( 1 ) ;
                    return 1;
                }

                if ( k.sym == SDLK_RIGHT ) {
                    console_lateral_scroll( -1 ) ;
                    return 1;
                }

                if ( k.sym == SDLK_UP ) {
                    console_scroll( 1 ) ;
                    return 1;
                }

                if ( k.sym == SDLK_DOWN ) {
                    console_scroll( -1 ) ;
                    return 1;
                }
            }

            if ( !( k.mod & KMOD_LALT ) ) console_getkey( k.unicode, k.sym ) ;
            return 1;
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------- */

static void console_draw( INSTANCE * i, REGION * clip ) {
    int x, y, line, count ;

    if ( break_on_next_proc ) return ;

/*    if ( debug_on_frame ) {
        SDL_EnableKeyRepeat( 250, 50 );
        debug_on_frame = 0;
        debugger_show_console = 1;
    }
*/
    if ( console_columns > scrbitmap->width / CHARWIDTH )
        console_columns = scrbitmap->width / CHARWIDTH ;

    if ( console_lines > ( scrbitmap->height - CHARHEIGHT * 3 ) / CHARHEIGHT )
        console_lines = ( scrbitmap->height - CHARHEIGHT * 3 ) / CHARHEIGHT ;

    if ( debugger_show_console ) {
        if ( console_y < console_lines * CHARHEIGHT ) console_y += CHARHEIGHT ;
        if ( console_y > console_lines * CHARHEIGHT ) console_y = console_lines * CHARHEIGHT ;
    } else {
        if ( console_y > 0 ) console_y -= CHARHEIGHT ;
        if ( console_y < 0 ) console_y = 0 ;
    }

    x = ( scrbitmap->width - console_columns * CHARWIDTH ) / 2 ;
    y = -console_lines * CHARHEIGHT + console_y ;

    int current_show = 0;

    for ( count = 0; count < MAX_EXPRESSIONS; count++ ) {
        if ( show_expression[count] ) {
            char * res = eval_expression( show_expression[count], 0 );

            if ( !res || result.type == T_ERROR ) {
                bgd_free( show_expression[count] );
                show_expression[count] = NULL;
                show_expression_count--;
            } else {
                int ln = strlen( res );
                int rl = ln * CHARWIDTH;
                int xo = ( scrbitmap->width - rl ) / 2;
                int yo = ( ( console_y < 1 ) ? 2 : console_y + CHARHEIGHT * 2 + 1 ) + CHARHEIGHT * current_show;

                systext_color( 0, 0 ) ;
                systext_puts( scrbitmap, xo - 1, yo - 1, res, ln );
                systext_puts( scrbitmap, xo - 1, yo    , res, ln );
                systext_puts( scrbitmap, xo - 1, yo + 1, res, ln );
                systext_puts( scrbitmap, xo    , yo + 1, res, ln );
                systext_puts( scrbitmap, xo + 1, yo + 1, res, ln );
                systext_puts( scrbitmap, xo + 1, yo    , res, ln );
                systext_puts( scrbitmap, xo + 1, yo - 1, res, ln );
                systext_puts( scrbitmap, xo    , yo - 1, res, ln );

                systext_color( console_showcolor, 0 ) ;
                systext_puts( scrbitmap, xo, yo, res, ln );

                gr_mark_rect( xo, yo, rl, CHARHEIGHT);
                current_show++;
            }
        }
    }

    if ( console_y <= 0 ) return ;

    int shiftstatus = GLODWORD( mod_debug, SHIFTSTATUS ) ;

    line = console_tail ;

    for ( count = 0 ; count < console_lines + console_scroll_pos ; count++ ) {
        if ( line == console_head ) break ;
        line-- ;
        if ( line < 0 ) line = CONSOLE_HISTORY - 1 ;
    }
    console_scroll_pos = count - console_lines ;
    if ( console_scroll_pos < 0 ) console_scroll_pos = 0 ;

    for ( count = 0; count < console_lines; count++ ) {
        systext_color( 0xC0C0C0, 0x000020 ) ;
        if ( !console[line] || console_scroll_lateral_pos >= strlen( console[line] ) ) {
            systext_puts( scrbitmap, x, y, "", console_columns ) ;
        } else {
            int pos = 0 ;
            int off = 0;

            do {
                if ( console[line][pos] == '\254' ) {
                    off += 3;
                    systext_puts( scrbitmap, x, y, console[line] + pos, 3 ) ;
                }
                pos++;
            }
            while ( pos - off < console_scroll_lateral_pos ) ;

            if ( console_scroll_lateral_pos + off < strlen( console[line] ) )
                systext_puts( scrbitmap, x, y, console[line] + console_scroll_lateral_pos + off, console_columns ) ;
            else
                systext_puts( scrbitmap, x, y, "", console_columns ) ;
        }

        y += CHARHEIGHT ;
        line++ ;
        if ( line == CONSOLE_HISTORY ) line = 0 ;
    }

    if ( debugger_show_console && trace_sentence != -1 ) {
        if ( dcb.data.Version < 0x0710 ) {
            if ( trace_instance && instance_exists( trace_instance ) && dcb.sourcecount[trace_sentence >> 24] ) {
                console_printf( "\25407[%s(%d):%d]\n\25414%s\25407\n\n",
                        trace_instance->proc->name,
                        LOCDWORD( mod_debug, trace_instance, PROCESS_ID ),
                        trace_sentence & 0xFFFFFF,
                        dcb.sourcelines [trace_sentence >> 24] [( trace_sentence & 0xFFFFFF )-1] ) ;
            }
        } else {
            if ( trace_instance && instance_exists( trace_instance ) && dcb.sourcecount[trace_sentence >> 20] ) {
                console_printf( "\25407[%s(%d):%d]\n\25414%s\25407\n\n",
                        trace_instance->proc->name,
                        LOCDWORD( mod_debug, trace_instance, PROCESS_ID ),
                        trace_sentence & 0xFFFFF,
                        dcb.sourcelines [trace_sentence >> 20] [( trace_sentence & 0xFFFFF )-1] ) ;
            }
        }
//        debug_on_frame = 0;
//        debugger_show_console = 0;
        trace_sentence = -1;
    }

    systext_color( 0xFFFFFF, 0x404040 ) ;
    systext_puts( scrbitmap, x, y, ">", 2 ) ;
    strcat( console_input, "_" ) ;
    systext_puts( scrbitmap, x + CHARWIDTH*2, y, console_input, console_columns - 2 ) ;
    console_input[strlen( console_input )-1] = 0 ;

    if ( shiftstatus & 3 ) {
        show_list_window();
        systext_color( 0x404040, 0x00C0C0 ) ;
        if ( console_list_current )
            systext_puts( scrbitmap, x, y + CHARHEIGHT, HOTKEYHELP3, console_columns ) ;
        else
            systext_puts( scrbitmap, x, y + CHARHEIGHT, HOTKEYHELP2, console_columns ) ;
    } else {
        systext_color( 0x404040, 0x00C0C0 ) ;
        systext_puts( scrbitmap, x, y + CHARHEIGHT, HOTKEYHELP1, console_columns ) ;
    }
}

/* --------------------------------------------------------------------------- */

static int console_info( INSTANCE * i, REGION * clip, int * z, int * drawme ) {

    * drawme = debugger_show_console || show_expression_count || ( console_y > 0 );

    if ( debugger_show_console || ( console_y > 0 ) ) {
        clip->x  = 0;
        clip->y  = 0;
        clip->x2 = scrbitmap->width ;
        clip->y2 = scrbitmap->height ;
    } else if ( show_expression_count ) {
        int count, rl = 0, l;

        for ( count = 0; count < MAX_EXPRESSIONS; count++ ) {
            if ( show_expression[count] ) {
                char * res = eval_expression( show_expression[count], 0 );

                if ( !res ) {
                    bgd_free( show_expression[count] );
                    show_expression[count] = NULL;
                    show_expression_count--;
                } else {
                    if ( ( l = strlen( res ) * CHARWIDTH ) > rl ) rl = l ;
                }
            }
        }

        if ( !show_expression_count ) {
            *drawme = 0;
            return 0;
        }

        clip->x = ( scrbitmap->width - rl ) / 2 ;
        clip->y = ( console_y <= 0 ) ? 0 : ( console_y + CHARHEIGHT ) ;
        clip->x2 = clip->x + rl ;
        clip->y2 = CHARHEIGHT * show_expression_count;
    }

    return * drawme ;
}

/* --------------------------------------------------------------------------- */

static int moddebug_trace( INSTANCE * my, int * params ) {
    debug = params[0];
    return 0 ;
}

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_debug, process_exec_hook )( INSTANCE * r ) {
    if ( break_on_next_proc ) {
        break_on_next_proc = 0;
        debugger_show_console = 1;
    }
}
/* --------------------------------------------------------------------------- */

static void __frame_complete_hook() {
    if ( frame_completed && debug_on_frame ) {
        debug_on_frame = 0;
        debugger_show_console = 1;
    }
}

/* --------------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                               */

void __bgdexport( mod_debug, module_initialize )() {
    if ( dcb.data.NSourceFiles ) {
        hotkey_add( KMOD_LALT, SDLK_x, force_exit_cb );
        hotkey_add( 0,      0, console_keyboard_handler_cb );

        gr_new_object( -2147483647L - 1, ( OBJ_INFO * ) console_info, ( OBJ_DRAW * ) console_draw, ( void * ) 0 );
    }
}

/* --------------------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( mod_debug, handler_hooks )[] = {
    { 1000, __frame_complete_hook },
    {    0, NULL                  }
} ;

/* --------------------------------------------------------------------------- */
/* exports                                                                     */
/* --------------------------------------------------------------------------- */

#include "mod_debug_exports.h"

/* --------------------------------------------------------------------------- */
