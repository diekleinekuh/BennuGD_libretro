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

#include "bgdrtm.h"

#include "files.h"
#include "xstrings.h"

#include "dlvaracc.h"
#include "regex.h"

/* ----------------------------------------------------------------- */

enum {
    REGEX_REG = 0
};

/* ----------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( mod_regex, globals_fixup) [] =   {
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
                                { "regex_reg", NULL, -1, -1 },
                                { NULL, NULL, -1, -1 }
                                };

/* ----------------------------------------------------------------- */

/** REGEX (STRING pattern, STRING string)
 *  Match a regular expresion to the given string. Fills the
 *  REGEX_REG global variables and returns the character position
 *  of the match or -1 if none found.
 */

static int modregex_regex (INSTANCE * my, int * params)
{
    const char * reg = string_get(params[0]);
    const char * str = string_get(params[1]);
    int result = -1;
    unsigned n;

    struct re_pattern_buffer pb;
    struct re_registers re;
    int start[16];
    int end[16];
    int * regex_reg;

    /* Alloc the pattern resources */

    memset (&pb, 0, sizeof(pb));
    memset (&re, 0, sizeof(re));
    pb.buffer = bgd_malloc(4096);
    pb.allocated = 4096;
    pb.fastmap = bgd_malloc(256);
    pb.regs_allocated = REGS_FIXED;
    re.num_regs = 16;
    re.start = start;
    re.end = end;

    re_syntax_options = RE_SYNTAX_POSIX_MINIMAL_EXTENDED | REG_ICASE;

    /* Match the regex */

    if (re_compile_pattern (reg, strlen(reg), &pb) == 0)
    {
        result = re_search (&pb, str, strlen(str), 0, strlen(str), &re);

        if (result != -1)
        {
            /* Fill the regex_reg global variables */
            regex_reg = (int *) &GLODWORD( mod_regex, REGEX_REG);
            for (n = 0 ; n < 16 && n <= pb.re_nsub ; n++)
            {
                string_discard (regex_reg[n]);
                regex_reg[n] = string_newa (str + re.start[n], re.end[n] - re.start[n]);
                string_use (regex_reg[n]);
            }
        }
    }

    /* Free the resources */
    bgd_free (pb.buffer);
    bgd_free (pb.fastmap);
    string_discard(params[0]);
    string_discard(params[1]);

    return result;
}

/** REGEX_REPLACE (STRING pattern, STRING string, STRING replacement)
 *  Match a regular expresion to the given string. For each
 *  match, substitute it with the given replacement. \0 - \9
 *  escape sequences are accepted in the replacement.
 *  Returns the resulting string. REGEX_REG variables are
 *  filled with information about the first match.
 */

static int modregex_regex_replace (INSTANCE * my, int * params)
{
    const char * reg = string_get(params[0]);
    const char * rep = string_get(params[1]);
    const char * str = string_get(params[2]);

    unsigned reg_len = strlen(reg);
    unsigned str_len = strlen(str);
    unsigned rep_len = strlen(rep);
    char * replacement;
    unsigned replacement_len;
    int fixed_replacement = strchr(rep, '\\') ? 0:1;

    struct re_pattern_buffer pb;
    struct re_registers re;
    int start[16];
    int end[16];

    unsigned startpos = 0;
    unsigned nextpos;
    int regex_filled = 0;

    char * result = 0;
    unsigned result_allocated = 0;
    int result_string = 0;

    unsigned n;
    int * regex_reg;

    /* Alloc a buffer for the resulting string */

    result = bgd_malloc(128);
    result_allocated = 128;
    *result = 0;

    /* Alloc the pattern resources */

    memset (&pb, 0, sizeof(pb));
    memset (&re, 0, sizeof(re));
    pb.buffer = bgd_malloc(4096);
    pb.allocated = 4096;
    pb.used = 0;
    pb.fastmap = bgd_malloc(256);
    pb.translate = NULL;
    pb.fastmap_accurate = 0;
    pb.regs_allocated = REGS_FIXED;
    re.start = start;
    re.end = end;

    re_syntax_options = RE_SYNTAX_POSIX_MINIMAL_EXTENDED;

    /* Run the regex */

    if (re_compile_pattern (reg, reg_len, &pb) == 0)
    {
        startpos = 0;

        while (startpos < str_len)
        {
            nextpos = re_search (&pb, str, str_len, startpos,
                str_len - startpos, &re);
            if ((int)nextpos < 0) break;

            /* Fill the REGEX_REG global variables */

            if (regex_filled == 0)
            {
                regex_filled = 1;
                regex_reg = (int *)&GLODWORD( mod_regex, REGEX_REG);
                for (n = 0 ; n < 16 && n <= pb.re_nsub ; n++)
                {
                    string_discard (regex_reg[n]);
                    regex_reg[n] = string_newa (str + re.start[n], re.end[n] - re.start[n]);
                    string_use (regex_reg[n]);
                }
            }

            /* Prepare the replacement string */

            if (fixed_replacement == 0)
            {
                int total_length = rep_len;
                const char * bptr;
                char *  ptr;

                /* Count the size */

                ptr = strchr(rep, '\\');
                while (ptr)
                {
                    if (ptr[1] >= '0' && ptr[1] <= '9')
                        total_length += re.end[ptr[1]-'0'] - re.start[ptr[1]-'0'] - 2;
                    ptr = strchr(ptr+1, '\\');
                }

                /* Fill the replacement string */

                replacement = bgd_calloc (total_length+1, 1);

                bptr = rep;
                ptr = strchr(rep, '\\');
                while (ptr)
                {
                    if (ptr[1] >= '0' && ptr[1] <= '9')
                    {
                        strncpy (replacement+strlen(replacement), bptr, ptr-bptr);
                        strncpy (replacement+strlen(replacement), str + re.start[ptr[1]-'0'], re.end[ptr[1]-'0'] - re.start[ptr[1]-'0']);
                        bptr = ptr+2;
                    }
                    ptr = strchr (ptr+1, '\\');
                }
                strcat (replacement, bptr);
                replacement_len = strlen(replacement);
            }
            else
            {
                replacement = (char *)rep;
                replacement_len = rep_len;
            }

            /* Fill the resulting string */

            if (result_allocated < strlen(result)+(nextpos-startpos)+1+replacement_len)
            {
                result_allocated += ((nextpos-startpos+1+replacement_len) & ~127) + 128;
                result = bgd_realloc(result, result_allocated);
            }
            result[strlen(result)+(nextpos-startpos)] = 0;
            memcpy (result + strlen(result), str+startpos, nextpos-startpos);
            strcat (result, replacement);

            if (fixed_replacement == 0) bgd_free (replacement);

            /* Continue the search */

            startpos = nextpos+re_match(&pb, str, str_len, nextpos, 0);
            if (startpos <  nextpos) break;
            if (startpos == nextpos) startpos++;
        }
    }

    /* Copy remaining characters */

    nextpos = str_len;
    if (result_allocated < strlen(result)+(nextpos-startpos)+1)
    {
        result_allocated += ((nextpos-startpos+1) & ~127) + 128;
        result = bgd_realloc(result, result_allocated);
    }
    result[strlen(result)+(nextpos-startpos)] = 0;
    memcpy (result + strlen(result), str+startpos, nextpos-startpos);

    /* Free resources */

    bgd_free (pb.buffer);
    bgd_free (pb.fastmap);
    string_discard(params[0]);
    string_discard(params[1]);
    string_discard(params[2]);

    /* Return the new string */

    result_string = string_new(result);
    string_use(result_string);
    bgd_free(result);

    return result_string;
}

/** SPLIT (STRING regex, STRING string, STRING POINTER array, INT array_size)
 *  Fills the given array with sections of the given string, using
 *  the given regular expression as separators. Returns the number
 *  of elements filled in the array.
 *
 */

static int modregex_split (INSTANCE * my, int * params)
{
    const char * reg = string_get(params[0]);
    const char * str = string_get(params[1]);
    int * result_array = (int *)params[2];
    int result_array_size = params[3];
    int count = 0;
    int pos, lastpos = 0;

    struct re_pattern_buffer pb;
    struct re_registers re;
    int start[16];
    int end[16];

    /* Alloc the pattern resources */

    memset (&pb, 0, sizeof(pb));
    memset (&re, 0, sizeof(re));
    pb.buffer = bgd_malloc(4096);
    pb.allocated = 4096;
    pb.fastmap = bgd_malloc(256);
    pb.regs_allocated = REGS_FIXED;
    re.num_regs = 16;
    re.start = start;
    re.end = end;

    re_syntax_options = RE_SYNTAX_POSIX_MINIMAL_EXTENDED;

    /* Match the regex */

    if (re_compile_pattern (reg, strlen(reg), &pb) == 0)
    {
        for (;;)
        {
            pos = re_search (&pb, str, strlen(str), lastpos, strlen(str), &re);
            if (pos == -1) break;
            *result_array = string_newa (str + lastpos, pos-lastpos);
            string_use(*result_array);
            result_array++;
            count++;
            result_array_size--;
            if (result_array_size == 0) break;
            lastpos = pos + re_match (&pb, str, strlen(str), pos, 0);
            if (lastpos < pos) break;
            if (lastpos == pos) lastpos++;
        }
        if (result_array_size > 0)
        {
            *result_array = string_new (str + lastpos);
            string_use (*result_array);
            count++;
        }
    }

    /* Free the resources */
    bgd_free (pb.buffer);
    bgd_free (pb.fastmap);
    string_discard(params[0]);
    string_discard(params[1]);

    return count;
}

/** JOIN (STRING separator, STRING POINTER array, INT array_size)
 *  Joins an array of strings, given a separator. Returns the
 *  resulting string.
 */

static int modregex_join (INSTANCE * my, int * params)
{
    const char * sep = string_get(params[0]);
    int * string_array = (int *)params[1];
    int count = params[2] ;
    int total_length = 0;
    int sep_len = strlen(sep);
    int n;
    char * buffer;
    char * ptr;
    int result;

    for (n = 0 ; n < count ; n++)
    {
        total_length += strlen(string_get(string_array[n]));
        if (n < count-1) total_length += sep_len;
    }

    buffer = bgd_malloc(total_length+1);
    ptr = buffer;

    for (n = 0 ; n < count ; n++)
    {
        memcpy (ptr, string_get(string_array[n]), strlen(string_get(string_array[n])));
        ptr += strlen(string_get(string_array[n]));
        if (n < count-1)
        {
            memcpy (ptr, sep, sep_len);
            ptr += sep_len;
        }
    }
    *ptr = 0;
    result = string_new(buffer);
    bgd_free(buffer);
    string_use(result);
    return result;
}

/* ----------------------------------------------------------------- */
/* exports                                                           */
/* ----------------------------------------------------------------- */

#include "mod_regex_exports.h"

/* ----------------------------------------------------------------- */
