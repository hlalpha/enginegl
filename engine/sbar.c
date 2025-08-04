/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sbar.c -- status bar code

#include "quakedef.h"

//#define CUSTOM_HUD


qpic_t *Draw_LoadPicFromWad (char *identifier);


int			hud_size = 0;

qpic_t		*hud_640bl;
qpic_t		*hud_640br;
qpic_t		*hud_640mr;
qpic_t		*hud_640ul;
qpic_t		*hud_640bl_t;
qpic_t		*hud_640br_t;
qpic_t		*hud_640mr_t;
qpic_t		*hud_640ul_t;


/*
===============
Sbar_ShowScores

Tab key down
===============
*/
void Sbar_ShowScores (void)
{
}

/*
===============
Sbar_SizeUp
===============
*/
void Sbar_SizeUp (void)
{
	if ( scr_viewsize.value < 120.0 )
	{
		Cvar_SetValue ("viewsize", scr_viewsize.value + 10.0);
	}
	else if ( --hud_size < 0 )
	{
		hud_size = 0;
	}
}

/*
===============
Sbar_DontShowScores

Tab key up
===============
*/
void Sbar_DontShowScores (void)
{
}

/*
===============
Sbar_SizeDown
===============
*/
void Sbar_SizeDown (void)
{
	if ( ++hud_size > 3 )
	{
		hud_size = 3;
		Cvar_SetValue ("viewsize", scr_viewsize.value - 10.0);
	}
}

/*
===============
Sbar_Changed
===============
*/
void Sbar_Changed (void)
{
}


/*
===============
Sbar_Init
===============
*/
void InitHLHud (void)
{
	// ?
	/*
	void **v0; // esi

	dword_45A818 = 8;
	dword_45A810 = 52;
	dword_45A814 = 44;
	dword_45A81C = 10;

	if ( vid.width >= 640 )
	{
		dword_45A810 = 84;
		dword_45A814 = 88;
	}

	v0 = (void **)&unk_D3B9F0;
	do
		free(*v0++);
	while ( v0 < (void **)&dword_D3BA24 );
	*/

	hud_640bl = Draw_LoadPicFromWad ("640_BL");
	hud_640br = Draw_LoadPicFromWad ("640_BR");
	hud_640mr = Draw_LoadPicFromWad ("640_MR");
	hud_640ul = Draw_LoadPicFromWad ("640_UL");
	hud_640bl_t = Draw_LoadPicFromWad ("640_BL_T");
	hud_640br_t = Draw_LoadPicFromWad ("640_BR_T");
	hud_640mr_t = Draw_LoadPicFromWad ("640_MR_T");
	hud_640ul_t = Draw_LoadPicFromWad ("640_UL_T");
}

/*
===============
Sbar_Init
===============
*/
void Sbar_Init (void)
{
	//memset(&unk_D3B9F0, 0, 52u); // ?

	Cmd_AddCommand ("+showscores", Sbar_ShowScores);
	Cmd_AddCommand ("-showscores", Sbar_DontShowScores);
	
	InitHLHud ();
}


/*
===============
Sbar_Draw
===============
*/
void Sbar_Draw (void)
{
#ifdef CUSTOM_HUD
	if (scr_con_current == vid.height)
		return;		// console is full screen

	scr_copyeverything = 1;

	char msg[100];
	sprintf( msg, "Health: %3d", cl.stats[STAT_HEALTH] );
	Draw_String( 15, vid.height - 45, msg );

	sprintf( msg, "Armor: %3d", (cl.items & IT_INVULNERABILITY) ? 666 : cl.stats[STAT_ARMOR] );
	Draw_String( 15, vid.height - 30, msg );

	if ( cl.stats[STAT_AMMO] > 0 )
	{
		sprintf( msg, "Ammo: %3d", cl.stats[STAT_AMMO] );
		Draw_String( vid.width - 15 - Draw_StringLength( msg ), vid.height - 30, msg );
	}
#else
	int old_width, old_height;
	int health, i;
	float flhealth;

	if (!hud_size)
		return;		// HUD is disabled

	if (scr_con_current == vid.height)
		return;		// console is full screen

	scr_copyeverything = 1;

	glPushMatrix ();

	glViewport (glx, gly, glwidth, glheight);

	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();

	glOrtho (0, 640, 480, 0, -99999, 99999);

	glMatrixMode (GL_MODELVIEW);

	glLoadIdentity ();

	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_BLEND);
	glDisable (GL_ALPHA_TEST);

	old_width = vid.width;
	old_height = vid.height;

	vid.width = 640;
	vid.height = 480;

	glColor4f (1,1,1,1);

	{
		// Draw height hud
		Draw_HudPic (0, vid.height-hud_640bl->height, hud_640bl->width, hud_640bl->height, hud_640bl);

		// Draw top-left panel
		if (hud_size >= 3)
		{
			Draw_HudPic (0, 0, hud_640ul->width, hud_640ul->height, hud_640ul);
		}

		// Draw right panel
		if (hud_size >= 2)
		{
			Draw_HudPic (vid.width-hud_640br->width, vid.height-hud_640br->height, hud_640br->width, hud_640br->height, hud_640br);

			Draw_HudPic (vid.width-hud_640mr->width, vid.height-hud_640br->height-hud_640mr->height+1, hud_640mr->width, hud_640mr->height, hud_640mr);
		}
	}

	glBlendFunc (GL_SRC_ALPHA, GL_ONE);

	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA); // FIXME(SanyaSho): glTexEnvi is deprecated

	glColor4f (1,1,1,0.8);

	glEnable (GL_BLEND);
	glEnable (GL_ALPHA_TEST);

	{
		// Draw health hud background
		Draw_HudPic (0, vid.height-hud_640bl->height, hud_640bl->width, hud_640bl->height, hud_640bl_t);

		// Draw top-left panel background
		if (hud_size >= 3)
		{
			Draw_HudPic (0, 0, hud_640ul->width, hud_640ul->height, hud_640ul_t);
		}

		// Draw right panel background
		if (hud_size >= 2)
		{
			Draw_HudPic (vid.width-hud_640br->width, vid.height-hud_640br->height, hud_640br->width, hud_640br->height, hud_640br_t);

			Draw_HudPic (vid.width-hud_640mr->width, vid.height-hud_640br->height-hud_640mr->height+1, hud_640mr->width, hud_640mr->height, hud_640mr_t);
		}
	}

	glDisable (GL_BLEND);
	glDisable (GL_ALPHA_TEST);

	// Draw health bar
	flhealth = cl.stats[STAT_HEALTH] / 100.0;
	if (flhealth > 1.f)
		flhealth = 1.f;

	health = (int)(flhealth * 150.0);

	for (i=0 ; i<health ; i+=10)
	{
		Draw_FillRGB (i+88, vid.height-hud_640bl->height+84, 8, 8, 255, 156, 39);
	}

	glPopMatrix ();

	vid.width = old_width;
	vid.height = old_height;
#endif
}


/*
==================
Sbar_IntermissionOverlay

==================
*/
void Sbar_IntermissionOverlay (void)
{
}


/*
==================
Sbar_FinaleOverlay

==================
*/
void Sbar_FinaleOverlay (void)
{
}
