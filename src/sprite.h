/*  libsprite - Library for reading Ragnarok Online sprite files.
 *  Copyright (C) 2004  Hongli Lai <h.lai@chello.nl>
 *
 *  Modificado por Axel Bayerl para Sprite Viewer 
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** @file sprite.h
 *
 * Reading .SPR (sprite) files.
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#ifdef __cplusplus
	extern "C" {
#endif /* __cplusplus */


#ifndef NULL
	#define NULL ((void *) 0)
#endif

typedef struct {
	unsigned char *data;
	int len;
	int width;
	int height;
} SpriteImage;


typedef struct {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char unused;
} SpritePalette;


typedef struct {
	char *filename;
	unsigned int nimages;
	SpriteImage *images;
	unsigned int palette_size;
	SpritePalette *palette;
} Sprite;


typedef enum {
	/* Developer errors */
	SE_BADARGS,

	/* sprite_new() errors */
	SE_CANTOPEN,
	SE_INVALID,

	/* sprite_to_bmp(), sprite_to_bmp_file() and sprite_to_rgb() errors */
	SE_INDEX,

	/* sprite_to_bmp_file() errors */
	SE_CANTWRITE
} SpriteError;

//Funcion para hacer uso de reverse_palette desde el exterior
SpritePalette * get_pal(SpritePalette *palette,int palette_len);

/* Open sprite file */
Sprite *sprite_open (const char *fname, SpriteError *error);

Sprite *sprite_open_from_data (const unsigned char *data, unsigned int size, SpriteError *error);

/* Change palette of sprite*/
void change_palete(Sprite *sprite, const char *fname, SpriteError *error);

/* Converts a sprite to bitmap file in memory */
void *sprite_to_bmp (Sprite *sprite, int i, int *size, SpriteError *error);

/* Like sprite_to_bmp(), but saves the result to a file */
int sprite_to_bmp_file (Sprite *sprite, int i, const char *writeToFile, SpriteError *error);

/* Converts a sprite to raw RGB data. The rowstride/pitch is 3*width. */
void *sprite_to_rgb (Sprite *sprite, int i, int *size, SpriteError *error);

/* Frees a Sprite* pointer */
void sprite_free (Sprite *sprite);


#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif /* _SPRITE_H_ */
