/*
 *  BennuGD - Videogame compiler/interpreter
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Copyright (c) 1999 José Luis Cebrián Pagüe
 *  Copyright (c) 2002 Fenix Team
 *  Copyright (c) 2008 Joseba García Echebarria
 *
 */

/*
 * FILE        : ttf.c
 * DESCRIPTION : Truetype support DLL
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>
/* BennuGD stuff */
#include <bgddl.h>
#include <libfont.h>
#include <files.h>
#include <g_pal.h>
#include <xstrings.h>
#include <xctype.h>

#define DEFAULT_GRADIENT_START 31

/* ------------- */

uint32_t gr_rgb16( int r, int g, int b )
{
    int color ;

    /* 16 bits */
    color = (( r >> std_pixel_format16->Rloss ) << std_pixel_format16->Rshift ) |
            (( g >> std_pixel_format16->Gloss ) << std_pixel_format16->Gshift ) |
            (( b >> std_pixel_format16->Bloss ) << std_pixel_format16->Bshift ) ;

    if ( !color ) return 1 ;

    return color ;
}

uint32_t gr_rgb32( int r, int g, int b )
{
	return                 0xff000000   |
			(( r << 16 ) & 0x00ff0000 ) |
			(( g <<  8 ) & 0x0000ff00 ) |
			(( b       ) & 0x000000ff ) ;
}

uint32_t gr_rgba16( int r, int g, int b, int a )
{
    int color = 0;

    /* 16 bits */
    color = (( r >> std_pixel_format16->Rloss ) << std_pixel_format16->Rshift ) |
            (( g >> std_pixel_format16->Gloss ) << std_pixel_format16->Gshift ) |
            (( b >> std_pixel_format16->Bloss ) << std_pixel_format16->Bshift ) ;

    if ( !color ) return 1 ;

    return color ;
}

uint32_t gr_rgba32( int r, int g, int b, int a )
{
	return  (( a << 24 ) & 0xff000000 ) |
			(( r << 16 ) & 0x00ff0000 ) |
			(( g <<  8 ) & 0x0000ff00 ) |
			(( b       ) & 0x000000ff ) ;
}

/* ------------- */


/* These are from SDL_types.h */
typedef unsigned char	Uint8;
typedef unsigned short	Uint16;

/*
 *  FUNCTION : gr_load_ttf
 *
 *  Load a Truetype font, using Freetype. The font can be produced in
 *  1, 8 or 16 bit depth. A 8 bpp font uses the current palette.
 *
 *  PARAMS :
 *		filename		Name of the TTF file
 *		size			Size of the required font (height in pixels)
 *      bpp             Depth of the resulting font (1, 8 or 16)
 *      fg              Foreground color (for 8 or 16 bpp)
 *      bg              Background color (for 8 or 16 bpp)
 *
 *  RETURN VALUE :
 *      ID of the font if succeded or -1 otherwise
 *
 */

static FT_Library freetype;
static int        freetype_initialized = 0;
static FT_Face    face;
static char       facename[2048] = "";
static char *     facedata = 0;

int gr_load_ttf (const char * filename, int size, int bpp, int fg, int bg, int gradient_start)
{
	FONT * font;
	file * fp;
	long   allocated;
	long   readed;
	char * buffer;
	int    id;
	int    i, x, y;
	int    maxyoffset = 0;
	int    error;
	int    ok = 0;

	if(gradient_start<0)
		gradient_start = 0;

	if(bpp>16)
		bpp = 32;
	else if(bpp>8)
		bpp = 16;
	else if(bpp>1)
		bpp = 8;
	else
		bpp = 1;

	/* Calculate the color equivalences */
	static uint32_t equiv[256];

	switch(bpp) {
		case 8: {
			int r1, g1, b1;
			int r2, g2, b2;

			gr_get_rgb (bg, &r1, &g1, &b1);
			gr_get_rgb (fg, &r2, &g2, &b2);
			for (i = 0 ; i < 256 ; i++) {
				equiv[i] = gr_find_nearest_color (
					r1 + ( (i*(r2-r1)) >> 8 ),
					g1 + ( (i*(g2-g1)) >> 8 ),
					b1 + ( (i*(b2-b1)) >> 8 ));
			}
			break;
		} case 16: {
			int r1, g1, b1;
			int r2, g2, b2;

			gr_get_rgb (bg, &r1, &g1, &b1);
			gr_get_rgb (fg, &r2, &g2, &b2);
			for (i = 0 ; i < 256 ; i++) {
				equiv[i] = gr_rgb16 (
					r1 + ( (i*(r2-r1)) >> 8 ),
					g1 + ( (i*(g2-g1)) >> 8 ),
					b1 + ( (i*(b2-b1)) >> 8 ));
			}
			break;
		} case 32: {
			int r1, g1, b1, a1;
			int r2, g2, b2, a2;

			gr_get_rgba (bg, &r1, &g1, &b1, &a1);
			gr_get_rgba (fg, &r2, &g2, &b2, &a2);
			//printf("bg: r%i g%i b%i a%i\n",r1,g1,b1,a1);
			//printf("fg: r%i g%i b%i a%i\n",r2,g2,b2,a2);
			for (i = 0 ; i < 256 ; i++) {
				equiv[i] = gr_rgba32 (
					r1 + ( (i*(r2-r1)) >> 8 ),
					g1 + ( (i*(g2-g1)) >> 8 ),
					b1 + ( (i*(b2-b1)) >> 8 ),
					a1 + ( (i*(a2-a1)) >> 8 ));
				//printf("equiv[%i]: r%i g%i b%i a%i\n",i,r1 + ( (i*(r2-r1)) >> 8 ),g1 + ( (i*(g2-g1)) >> 8 ),b1 + ( (i*(b2-b1)) >> 8 ),a1 + ( (i*(a2-a1)) >> 8 ));
			}
			break;
		}
	}

	/* Open the file */

	fp = file_open (filename, "rb");
	if (!fp)
	{
		//fprintf (stderr, "gr_load_ttf: imposible abrir %s", filename);
		return -1;
	}
	allocated = 4096;
	readed = 0;
	buffer = malloc(allocated);
	if (buffer == NULL)
	{
		//fprintf (stderr, "gr_load_ttf: sin memoria");
		file_close (fp);
		return -1;
	}

	/* Read the entire file into memory */

	for (;;)
	{
		readed += file_read (fp, buffer+readed, allocated-readed);
		if (readed < allocated)
			break;

		allocated += 4096;
		buffer = realloc (buffer, allocated);
		if (buffer == NULL)
		{
			//fprintf (stderr, "gr_load_ttf: sin memoria");
			file_close (fp);
			return -1;
		}
	}
	file_close(fp);

	/* Initialize Freetype */

	if (freetype_initialized == 0)
	{
		error = FT_Init_FreeType(&freetype);
		if (error)
		{
			//fprintf (stderr, "gr_load_ttf: error al inicializar Freetype");
			free (buffer);
			return -1;
		}
		freetype_initialized = 1;
	}

	/* Load the font file */

	if (strncmp (facename, filename, 1024) != 0)
	{
		if (facedata) free(facedata);
		error = FT_New_Memory_Face (freetype, (FT_Byte*)buffer, readed, 0, &face);
		if (error)
		{
			if (error == FT_Err_Unknown_File_Format) {
				//fprintf (stderr, "gr_load_ttf: %s no es una fuente Truetype válida", filename);
			} else {
				//fprintf (stderr,"gr_load_ttf: error al recuperar %s", filename) ;
			}
			return -1;
		}
		strncpy (facename, filename, 1024);
		facedata = buffer;
	}

	/* Create the Fenix font */

	id = gr_font_new (CHARSET_ISO8859, bpp);
	if (id < 0) return -1;
	font = gr_font_get(id);
	// font->bpp = bpp;
	// font->charset = CHARSET_ISO8859;

	/* Retrieve the glyphs */

	FT_Set_Pixel_Sizes (face, 0, size);

	if (FT_Select_Charmap (face, ft_encoding_latin_1) != 0 &&
	    FT_Select_Charmap (face, ft_encoding_unicode) != 0 &&
	    FT_Select_Charmap (face, ft_encoding_none) != 0)

	{
		if (face->num_charmaps > 0)
			FT_Set_Charmap (face, face->charmaps[0]);
	}

	for (i = 0 ; i < 256 ; i++)
	{
		GRAPH * bitmap;
		int width, height;

		/* Render the glyph */

		error = FT_Get_Char_Index (face, i);
		if (!error) continue;

		error = FT_Load_Glyph (face, error, FT_LOAD_RENDER |
			(bpp == 1 ? FT_LOAD_MONOCHROME : 0));
		if (error) continue;

		ok++;

		/* Create the bitmap */

		width  = face->glyph->bitmap.width;
		height = face->glyph->bitmap.rows;
		bitmap = bitmap_new (i, width, height, bpp);
		if (bitmap)
		{
			
			bitmap_add_cpoint (bitmap, 0, 0);

			if (bpp == 1)
			{
				for (y = 0 ; y < height ; y++)
				{
					memcpy ((Uint8 *)bitmap->data + bitmap->pitch*y,
						face->glyph->bitmap.buffer + face->glyph->bitmap.pitch*y,
						bitmap->widthb);
				}
			}
			else if (bpp == 8)
			{
				Uint8 * ptr = (Uint8 *)bitmap->data;
				Uint8 * gld = face->glyph->bitmap.buffer;

				for (y = 0 ; y < height ; y++)
				{
					for (x = 0 ; x < width ; x++)
					{
						if ((unsigned char)gld[x] >= gradient_start)
							ptr[x] = (Uint8)equiv[(unsigned char) gld[x]];
						else
							ptr[x] = 0;
					}

					ptr += bitmap->pitch;
					gld += face->glyph->bitmap.pitch;
				}
			}
			else if (bpp == 16)
			{
				Uint16* ptr = (Uint16*)bitmap->data;
				Uint8 * gld = face->glyph->bitmap.buffer;

				for (y = 0 ; y < height ; y++)
				{
					for (x = 0 ; x < width ; x++)
					{
						if ((unsigned char)gld[x] >= gradient_start)
							ptr[x] = (Uint16)equiv[(Uint8) gld[x]];
						else
							ptr[x] = 0;
					}

					ptr += bitmap->pitch / 2;
					gld += face->glyph->bitmap.pitch;
				}
			}
			else if (bpp == 32)
			{
				uint32_t* ptr = (uint32_t*)bitmap->data;
				Uint8 * gld = face->glyph->bitmap.buffer;

				for (y = 0 ; y < height ; y++)
				{
					for (x = 0 ; x < width ; x++)
					{
						if ((unsigned char)gld[x] >= gradient_start)
							ptr[x] = equiv[(Uint8) gld[x]];
						else
							ptr[x] = 0;
					}

					ptr += bitmap->pitch / 4;
					gld += face->glyph->bitmap.pitch;
				}
			}
		}

		/* Store the glyph metrics in the font */

		font->glyph[i].xoffset  = face->glyph->bitmap_left;
		font->glyph[i].yoffset  = face->glyph->bitmap_top;
		font->glyph[i].xadvance = face->glyph->advance.x >> 6;
		font->glyph[i].yadvance = face->glyph->advance.y >> 6;
		font->glyph[i].bitmap   = bitmap;

		if (maxyoffset < font->glyph[i].yoffset)
		    maxyoffset = font->glyph[i].yoffset;
	}

//	if (!ok)
//		fprintf (stderr, "El tipo de letra %s no contiene caracteres utilizables", filename);

	/* Transform yoffsets */

	for (i = 0 ; i < 256 ; i++)
	{
		if (font->glyph[i].bitmap)
			font->glyph[i].yoffset = maxyoffset - font->glyph[i].yoffset;
	}

	return id ;
}

static int fxi_load_ttf (INSTANCE * my, int * params)
{
	char * text = (char *)string_get (params[0]) ;
	int r = text ? gr_load_ttf (text, params[1], 1, 0, 0, DEFAULT_GRADIENT_START) : 0 ;
	string_discard (params[0]) ;
	return r ;
}

static int fxi_load_ttfaa (INSTANCE * my, int * params)
{
	char * text = (char *)string_get (params[0]) ;
	int r = text ? gr_load_ttf (text, params[1], params[2], params[3], params[4], DEFAULT_GRADIENT_START) : 0 ;
	string_discard (params[0]) ;
	return r ;
}

static int fxi_load_ttfx (INSTANCE * my, int * params)
{
	char * text = (char *)string_get (params[0]) ;
	int r = text ? gr_load_ttf (text, params[1], params[2], params[3], params[4], params[5]) : 0 ;
	string_discard (params[0]) ;
	return r ;
}

#include "mod_ttf_exports.h"