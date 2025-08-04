#if !defined( QFONT_H )
#define QFONT_H

// Font stuff

#define NUM_GLYPHS 256

typedef struct
{
	short startoffset;
	short charwidth;
} charinfo;

typedef struct
{
	int 		width, height;
	int			rowheight;
	charinfo	fontinfo[ NUM_GLYPHS ];
	byte 		data[4];
} qfont_t;

#endif // QFONT_H
