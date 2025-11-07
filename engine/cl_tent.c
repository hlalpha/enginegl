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
// cl_tent.c -- client side temporary entities

#include "quakedef.h"

TEMPENTITY		gTempEnts[MAX_TEMP_ENTITIES], *gpTempEntFree, *gpTempEntActive;

cvar_t			tracerSpeed = {"tracerspeed", "3000"};
cvar_t			tracerOffset = {"traceroffset", "35"};
cvar_t			tracerLength = {"tracerlength", "1.0"};
cvar_t			tracerRed = {"tracerred", "0.6"};
cvar_t			tracerGreen = {"tracergreen", "0.8"};
cvar_t			tracerBlue = {"tracerblue", "0.1"};
cvar_t			tracerAlpha = {"traceralpha", "0.2"};

sfx_t			*cl_sfx_wizhit;
sfx_t			*cl_sfx_knighthit;
sfx_t			*cl_sfx_tink1;
sfx_t			*cl_sfx_ric1;
sfx_t			*cl_sfx_ric2;
sfx_t			*cl_sfx_ric3;
sfx_t			*cl_sfx_ric4;
sfx_t			*cl_sfx_ric5;
sfx_t			*cl_sfx_r_exp3;
sfx_t			*cl_sfx_r_exp4;
sfx_t			*cl_sfx_r_exp5;
#ifdef QUAKE2
sfx_t			*cl_sfx_imp;
sfx_t			*cl_sfx_rail;
#endif

/*
=================
CL_InitTEnts
=================
*/
void CL_InitTEnts (void)
{
	Cvar_RegisterVariable (&tracerSpeed);
	Cvar_RegisterVariable (&tracerOffset);
	Cvar_RegisterVariable (&tracerLength);
	Cvar_RegisterVariable (&tracerRed);
	Cvar_RegisterVariable (&tracerGreen);
	Cvar_RegisterVariable (&tracerBlue);
	Cvar_RegisterVariable (&tracerAlpha);

	cl_sfx_ric1 = S_PrecacheSound ("weapons/ric1.wav");
	cl_sfx_ric2 = S_PrecacheSound ("weapons/ric2.wav");
	cl_sfx_ric3 = S_PrecacheSound ("weapons/ric3.wav");
	cl_sfx_ric4 = S_PrecacheSound ("weapons/ric4.wav");
	cl_sfx_ric5 = S_PrecacheSound ("weapons/ric5.wav");
	cl_sfx_r_exp3 = S_PrecacheSound ("weapons/explode3.wav");
	cl_sfx_r_exp4 = S_PrecacheSound ("weapons/explode4.wav");
	cl_sfx_r_exp5 = S_PrecacheSound ("weapons/explode5.wav");

	CL_TempEntInit ();
}

/*
=================
ModelFrameCount
=================
*/
int ModelFrameCount (model_t *m)
{
	int			count;
	msprite_t	*psprite;

	count = 1;

	if (m)
	{
		switch (m->type)
		{
		case mod_sprite:
			psprite = (msprite_t *)m->cache.data;
			count = psprite->numframes;
			break;
		case mod_studio:
			count = R_StudioBodyVariations (m);
			break;
		};

		if (count < 1)
			count = 1;
	}

	return count;
}

/*
=================
ParseWaterColor
=================
*/
extern void SetWaterColor (int r, int g, int b);

void ParseWaterColor (void)
{
	int r, g, b;

	r = MSG_ReadByte(); // R
	g = MSG_ReadByte(); // G
	b = MSG_ReadByte(); // B
	MSG_ReadByte();		// A, Unused but set to 1 in HLSDK

	SetWaterColor (r, g, b);
}

TEMPENTITY *CL_TempEntAlloc (vec3_t org, model_t *model);

/*
=================
R_TracerEffect
=================
*/
void R_TracerEffect (vec_t *start, vec_t *end)
{
	vec3_t	temp, vel;
	float	len, scale, life;

	if (tracerSpeed.value <= 0)
		tracerSpeed.value = 3;

	VectorSubtract (end, start, temp);
	len = Length (temp);

	VectorScale (temp, 1.0 / len, temp);

	scale = (rand() % 20) + tracerOffset.value - 10;
	VectorScale (temp, scale, vel);

	VectorAdd (start, vel, start);

	VectorScale (temp, tracerSpeed.value, vel);

	life = len / tracerSpeed.value;
	R_TracerParticles (start, vel, life);

}

/*
=================
R_FizzEffect
=================
*/
void R_FizzEffect (entity_t *ent, int modelIndex, int density)
{
	model_t		*entmodel, *pmod;
	int			framecount, i, speed;
	TEMPENTITY	*ptemp;
	vec3_t		org;
	float		width, depth, maxHeight;

	// the entity doesn't have a model
	entmodel = ent->model;
	if (!entmodel || !modelIndex)
		return;

	// get the model
	pmod = cl.model_precache[modelIndex];
	if (!pmod)
		return;

	// calculate the effect params
	width = entmodel->maxs[0] - entmodel->mins[0];
	maxHeight = entmodel->maxs[2] - entmodel->mins[2];
	depth = entmodel->maxs[1] - entmodel->mins[1];

	speed = 3 * (density + 3);

	framecount = ModelFrameCount (pmod);

	for (i=0 ; i<(density+1) ; i++)
	{
		org[0] = entmodel->mins[0] + (rand() % (int)width);
		org[1] = entmodel->mins[1] + (rand() % (int)depth);
		org[2] = entmodel->mins[2];

		ptemp = CL_TempEntAlloc(org, pmod);
		if (!ptemp)
			break;

		ptemp->flags |= FTENT_SINEWAVE;

		ptemp->entity.syncbase = org[0]; // syncbase is used as a origin[0] in CL_TempEntUpdate

		ptemp->entity.baseline.origin[2] = speed + (rand()&31);

		ptemp->die = maxHeight / ptemp->entity.baseline.origin[2] + cl.time;

		ptemp->entity.frame = (rand() % framecount);
	}
}

/*
=================
R_TempSphereModel
=================
*/
void R_TempSphereModel (vec3_t pos, vec_t scale, float life, int count, int modelIndex)
{
	model_t		*pmod;
	int			framecount, i;
	TEMPENTITY	*ptemp;

	if (!modelIndex)
		return;

	// get the model
	pmod = cl.model_precache[modelIndex];
	if (!pmod)
		return;

	// get the frame count of the model
	framecount = ModelFrameCount (pmod);

	for (i=0 ; i<count ; i++)
	{
		// create an entity
		ptemp = CL_TempEntAlloc (pos, pmod);
		if (!ptemp)
			break;

		ptemp->entity.body = (rand() % framecount); // TODO(SanyaSho): Support for Sprites/AliasMDL!

		if ((rand()&255) >= 200)
			ptemp->flags |= FTENT_GRAVITY;
		else
			ptemp->flags |= FTENT_SLOWGRAVITY;

		if ((rand()&255) < 220)
		{
			ptemp->flags |= FTENT_ROTATE;

			ptemp->entity.msg_angles[0][0] = ((rand() & 7) * 2) - 4;
			ptemp->entity.msg_angles[0][1] = ((rand() & 7) * 4) - 4;
			ptemp->entity.msg_angles[0][2] = (rand() & 7) - 4;
		}

		if ((rand()&255) < 100)
			ptemp->flags |= FTENT_SMOKETRAIL;

		ptemp->flags |= (FTENT_COLLIDEWORLD|FTENT_FLICKER);

		ptemp->entity.effects = (i & 31);

		ptemp->entity.rendermode = kRenderModeNormal;

		ptemp->entity.baseline.origin[0] = ((rand()&4095) - 2048) * (1.f/2048.f);
		ptemp->entity.baseline.origin[1] = ((rand()&4095) - 2048) * (1.f/2048.f);
		ptemp->entity.baseline.origin[2] = ((rand()&4095) - 2048) * (1.f/2048.f);

		VectorNormalize (ptemp->entity.baseline.origin);
		VectorScale (ptemp->entity.baseline.origin, scale, ptemp->entity.baseline.origin);

		ptemp->die = cl.time+life;
	}
}

/*
=================
R_BreakModel
=================
*/
void R_BreakModel (vec3_t pos, vec3_t size, vec3_t dir, float life, int count, int modelIndex, char flags)
{
	model_t		*pmod;
	int			framecount, type, i;
	TEMPENTITY	*ptemp;

	if (!modelIndex)
		return;

	// limit all possible break types
	type = flags & BREAK_TYPEMASK;

	// get the model
	pmod = cl.model_precache[modelIndex];
	if (!pmod)
		return;

	// get the frame count of the model
	framecount = ModelFrameCount (pmod);

	if (!count)
		count = ((size[0] * size[1] * size[2]) / 27000); // TODO(SanyaSho): define?

	// HACK: Spawn x4 more wooden gibs
	if (type == BREAK_WOOD)
		count *= 4;

	for (i=0;i<count;i++)
	{
		rand();
		rand();
		rand();

		// create an entity
		ptemp = CL_TempEntAlloc (pos, pmod);
		if (!ptemp)
			break;

		if (pmod->type == mod_sprite)
			ptemp->entity.frame = (rand() % framecount);
		else if (pmod->type == mod_studio)
			ptemp->entity.body = (rand() % framecount);

		ptemp->flags |= FTENT_COLLIDEWORLD;

		if ((rand()&255) >= 200)
			ptemp->flags |= FTENT_GRAVITY;
		else
			ptemp->flags |= FTENT_SLOWGRAVITY;

		if ((rand()&255) < 200)
		{
			ptemp->flags |= FTENT_ROTATE;

			ptemp->entity.msg_angles[0][0] = ((rand() & 7) * 2) - 4;
			ptemp->entity.msg_angles[0][1] = ((rand() & 7) * 4) - 4;
			ptemp->entity.msg_angles[0][2] = (rand() & 7) - 4;
		}

		if (((rand()&255) < 100) && (type == BREAK_METAL || (flags & BREAK_SMOKE)))
			ptemp->flags |= FTENT_SMOKETRAIL;

		if (type == BREAK_GLASS || (flags & BREAK_TRANS))
		{
			ptemp->entity.rendermode = kRenderModeTexture;
			ptemp->entity.renderamt = 100;
			ptemp->entity.renderfx = kRenderFXSlowFadeAway;
		}
		else
			ptemp->entity.rendermode = kRenderModeNormal;

		// NOTE(SanyaSho): Using dir[0] here is probably a bug that was fixed later
		ptemp->entity.baseline.origin[0] = ((rand()&4095) - 2048) * dir[0] * (1.f/2048.f);
		ptemp->entity.baseline.origin[1] = ((rand()&4095) - 2048) * dir[0] * (1.f/2048.f);
		ptemp->entity.baseline.origin[2] = (rand()&4095) * dir[0] * (1.f/4096.f);

		VectorScale (ptemp->entity.baseline.origin, Length (dir) * 100, ptemp->entity.baseline.origin);

		ptemp->die = cl.time+life;
	}
}

/*
=================
R_TempModel
=================
*/
void R_TempModel (vec3_t pos, vec3_t dir, float life, int modelindex)
{
	model_t		*pmod;
	int			framecount;
	TEMPENTITY	*ptemp;

	if (!modelindex)
		return;

	// get the model
	pmod = cl.model_precache[modelindex];
	if (!pmod)
		return;

	// get the frame count of the model
	framecount = ModelFrameCount (pmod);

	// create an entity
	ptemp = CL_TempEntAlloc (pos, pmod);
	if (!ptemp)
		return;

	ptemp->flags |= FTENT_GRAVITY;
	VectorCopy (dir, ptemp->entity.baseline.origin);
	ptemp->die = cl.time+life;

	if (pmod->type == mod_sprite)
		ptemp->entity.frame = (rand() % framecount);
	else // FIXME(SanyaSho): Check for mod_studio here!
		ptemp->entity.body = (rand() % framecount);
}

/*
=================
CL_ParseTEnt
=================
*/
void CL_ParseTEnt (void)
{
	int		type;
	vec3_t	pos, endpos, size, dir;
	dlight_t	*dl;
	int		rnd;
	int		colorStart, colorLength;

	int		modelindex, color, count, density, speed, flags, idx, ent;
	float	lifetime;

	type = MSG_ReadByte ();
	switch (type)
	{
	case TE_SPIKE:				// spike hitting wall
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
#ifdef GLTEST
		Test_Spawn (pos);
#else
		R_RunParticleEffect (pos, vec3_origin, 0, 10);
#endif

		if ( rand() % 5 )
			S_StartSound (-1, 0, cl_sfx_tink1, pos, 1, 1);
		else
		{
			rnd = rand() & 3;
			if (rnd == 1)
				S_StartSound (-1, 0, cl_sfx_ric1, pos, 1, 1);
			else if (rnd == 2)
				S_StartSound (-1, 0, cl_sfx_ric2, pos, 1, 1);
			else
				S_StartSound (-1, 0, cl_sfx_ric3, pos, 1, 1);
		}
		break;
	case TE_SUPERSPIKE:			// super spike hitting wall
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_RunParticleEffect (pos, vec3_origin, 0, 20);

		if ( rand() % 5 )
			S_StartSound (-1, 0, cl_sfx_tink1, pos, 1, 1); // TODO(SanyaSho): cl_sfx_tink1 is uninitialized in enginegl.exe
		else
		{
			rnd = rand() & 3;
			if (rnd == 1)
				S_StartSound (-1, 0, cl_sfx_ric1, pos, 1, 1);
			else if (rnd == 2)
				S_StartSound (-1, 0, cl_sfx_ric2, pos, 1, 1);
			else
				S_StartSound (-1, 0, cl_sfx_ric3, pos, 1, 1);
		}
		break;

	case TE_GUNSHOT:			// bullet hitting wall
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_RunParticleEffect (pos, vec3_origin, 0, 20);

		rnd = rand();
		if (rnd < 0x3FFF)
		{
			switch (rnd % 5)
			{
			case 0:
				S_StartSound (-1, 0, cl_sfx_ric1, pos, 1, 1);
				break;
			case 1:
				S_StartSound (-1, 0, cl_sfx_ric2, pos, 1, 1);
				break;
			case 2:
				S_StartSound (-1, 0, cl_sfx_ric3, pos, 1, 1);
				break;
			case 3:
				S_StartSound (-1, 0, cl_sfx_ric4, pos, 1, 1);
				break;
			case 4:
				S_StartSound (-1, 0, cl_sfx_ric5, pos, 1, 1);
				break;
			}
		}

		break;
		
	case TE_EXPLOSION:			// rocket explosion
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		dl = CL_AllocDlight (0);
		VectorCopy (pos, dl->origin);
		dl->radius = 250;
		dl->color[0] = -6;
		dl->color[1] = -6;
		dl->color[2] = -106;
		dl->die = cl.time + 0.5;
		dl->decay = 800;

		rnd = rand() & 3;
		if (rnd == 1)
			S_StartSound (-1, 0, cl_sfx_r_exp4, pos, 1, 1);
		else if (rnd == 2)
			S_StartSound (-1, 0, cl_sfx_r_exp5, pos, 1, 1);
		else
			S_StartSound (-1, 0, cl_sfx_r_exp3, pos, 1, 1);
		break;
		
	case TE_TAREXPLOSION:		// tarbaby explosion
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_BlobExplosion (pos);

		S_StartSound (-1, 0, cl_sfx_r_exp3, pos, 1, 1);
		break;

	case TE_WATERCOLOR:			// water color
		ParseWaterColor();
		break;

	case TE_TRACER:				// bullet tracer effect
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		endpos[0] = MSG_ReadCoord ();
		endpos[1] = MSG_ReadCoord ();
		endpos[2] = MSG_ReadCoord ();
		R_TracerEffect (pos, endpos);
		break;

	case TE_WIZSPIKE:			// spike hitting wall
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_RunParticleEffect (pos, vec3_origin, 20, 30);
		S_StartSound (-1, 0, cl_sfx_wizhit, pos, 1, 1); // NOTE(SanyaSho): cl_sfx_wizhit is uninitialized in enginegl.exe
		break;

	case TE_KNIGHTSPIKE:		// spike hitting wall
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_RunParticleEffect (pos, vec3_origin, 226, 20);
		S_StartSound (-1, 0, cl_sfx_knighthit, pos, 1, 1); // NOTE(SanyaSho): cl_sfx_knighthit is uninitialized in enginegl.exe
		break;

	case TE_SPARKS:				// spark particles
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_SparkStreaks (pos);
		break;

	case TE_LAVASPLASH:	
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_LavaSplash (pos);
		break;

	case TE_TELEPORT:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_TeleportSplash (pos);
		break;

	case TE_EXPLOSION2:			// color mapped explosion
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		colorStart = MSG_ReadByte ();
		colorLength = MSG_ReadByte ();
		R_ParticleExplosion2 (pos, colorStart, colorLength);
		dl = CL_AllocDlight (0);
		VectorCopy (pos, dl->origin);
		dl->radius = 350;
		dl->die = cl.time + 0.5;
		dl->decay = 300;
		S_StartSound (-1, 0, cl_sfx_r_exp3, pos, 1, 1);
		break;

	/*case TE_RESERVED:
		break;*/

#ifdef QUAKE2
	case TE_IMPLOSION:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		S_StartSound (-1, 0, cl_sfx_imp, pos, 1, 1); // NOTE(SanyaSho): cl_sfx_imp is uninitialized in enginegl.exe
		break;

	case TE_RAILTRAIL:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		endpos[0] = MSG_ReadCoord ();
		endpos[1] = MSG_ReadCoord ();
		endpos[2] = MSG_ReadCoord ();
		S_StartSound (-1, 0, cl_sfx_rail, pos, 1, 1); // NOTE(SanyaSho): cl_sfx_rail is uninitialized in enginegl.exe
		S_StartSound (-1, 1, cl_sfx_r_exp3, endpos, 1, 1);
		R_RocketTrail (pos, endpos, 0+128);
		R_ParticleExplosion (endpos);
		dl = CL_AllocDlight (-1);
		VectorCopy (endpos, dl->origin);
		dl->radius = 350;
		dl->die = cl.time + 0.5;
		dl->decay = 300;
		break;
#endif

	case TE_LARGEFUNNEL:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		R_LargeFunnel (pos);
		break;

	case TE_BLOODSTREAM:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		endpos[0] = MSG_ReadCoord ();
		endpos[1] = MSG_ReadCoord ();
		endpos[2] = MSG_ReadCoord ();
		color = MSG_ReadByte ();
		speed = MSG_ReadByte ();
		R_BloodStream (pos, dir, color, speed);
		break;

	case TE_SHOWLINE:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		endpos[0] = MSG_ReadCoord ();
		endpos[1] = MSG_ReadCoord ();
		endpos[2] = MSG_ReadCoord ();
		R_ShowLine (pos, endpos);
		break;

	case TE_BLOOD:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		dir[0] = MSG_ReadCoord ();
		dir[1] = MSG_ReadCoord ();
		dir[2] = MSG_ReadCoord ();
		color = MSG_ReadByte ();
		speed = MSG_ReadByte ();
		R_Blood (pos, dir, color, speed);
		break;

	case TE_GUNSHOTDECAL:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		ent = MSG_ReadShort ();
		idx = MSG_ReadByte ();

		if (ent > MAX_EDICTS)
			Sys_Error ("Decal: entity = %i\n", ent);

		if (r_decals.value)
			R_DecalShoot (Draw_DecalIndex (idx), ent, pos, 0);
		break;

	case TE_FIZZ:
		ent = MSG_ReadShort ();

		if (ent > MAX_EDICTS)
			Sys_Error ("Bubble: entity = %i\n", ent);

		modelindex = MSG_ReadShort ();
		density = MSG_ReadByte ();
		R_FizzEffect (&cl_entities[ent], modelindex, density);
		break;

	case TE_MODEL:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		dir[0] = MSG_ReadCoord ();
		dir[1] = MSG_ReadCoord ();
		dir[2] = MSG_ReadCoord ();
		modelindex = MSG_ReadShort ();
		lifetime = (MSG_ReadByte () * 0.1);
		R_TempModel (pos, dir, lifetime, modelindex);
		break;

	case TE_EXPLODEMODEL:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		dir[0] = MSG_ReadCoord (); // scaling
		modelindex = MSG_ReadShort ();
		count = MSG_ReadShort ();
		lifetime = (MSG_ReadByte () * 0.1);
		R_TempSphereModel (pos, dir[0], lifetime, count, modelindex);
		break;

	case TE_BREAKMODEL:
		pos[0] = MSG_ReadCoord ();
		pos[1] = MSG_ReadCoord ();
		pos[2] = MSG_ReadCoord ();
		size[0] = MSG_ReadCoord ();
		size[1] = MSG_ReadCoord ();
		size[2] = MSG_ReadCoord ();
		dir[0] = MSG_ReadCoord ();
		dir[1] = MSG_ReadCoord ();
		dir[2] = MSG_ReadCoord ();
		modelindex = MSG_ReadShort ();
		count = MSG_ReadByte ();
		lifetime = (MSG_ReadByte () * 0.1);
		flags = MSG_ReadByte ();
		R_BreakModel (pos, size, dir, lifetime, count, modelindex, flags);
		break;

	default:
		Sys_Error ("CL_ParseTEnt: bad type");
	}
}

/*
=================
CL_TempEntInit
=================
*/
void CL_TempEntInit (void)
{
	int		i;

	memset (&gTempEnts, 0, sizeof(gTempEnts));
	for (i=0 ; i<MAX_TEMP_ENTITIES; i++)
		gTempEnts[i].next = &gTempEnts[i+1];
	gTempEnts[MAX_TEMP_ENTITIES-1].next = NULL;

	gpTempEntFree = gTempEnts;
	gpTempEntActive = NULL;
}

/*
=================
CL_TempEntAlloc
=================
*/
TEMPENTITY *CL_TempEntAlloc (vec3_t org, model_t *model)
{
	TEMPENTITY		*ent;
	int				i;

	// no free space to operate or model is invalid
	if (!gpTempEntFree || !model)
	{
		Con_DPrintf ("Out of bubbles!\n");
		return NULL;
	}

	ent = gpTempEntFree; // keep the current temp entity within this scope
	gpTempEntFree = gpTempEntFree->next; // and move the main pointer

	memset (&ent->entity, 0, sizeof(entity_t));

	ent->flags = FTENT_NONE;
	ent->entity.colormap = vid.colormap;
	ent->entity.model = model;
	ent->entity.rendermode = kRenderModeNormal;
	ent->entity.renderfx = kRenderFXNormal;
	ent->die = cl.time+0.75;

	for (i=0 ; i<3 ; i++)
	{
		ent->entity.origin[i] = org[i] + ((rand() % 6) - 3);
		ent->entity.msg_origins[0][i] = (4 * (rand() % 100) - 200); // FIXME(SanyaSho): This is probably not what is in "*origin - 25"
	}

	ent->next = gpTempEntActive;
	gpTempEntActive = ent;

	return ent;
}

/*
=================
CL_TempEntUpdate
=================
*/
extern	cvar_t	sv_gravity;

void CL_TempEntUpdate (void)
{
	static int	gTempEntFrame = 0;
	int			i;
	TEMPENTITY	*pTemp, *pnext, *pprev;
	float		freq, gravity, gravitySlow, life, frametime, proj;
	trace_t		trace;

	// Don't simulate if there's no active tempents
	if (!gpTempEntActive)
		return;

	// Don't simulate while loading
	if (!cl.worldmodel)
		return;

	gTempEntFrame = (gTempEntFrame + 1) & 31;

	pTemp = gpTempEntActive;

	pprev = NULL;

	frametime = cl.time - cl.oldtime;

	freq = cl.time * 0.01;

	gravity = -frametime * sv_gravity.value;
	gravitySlow = gravity * 0.5;

	while (pTemp)
	{
		life = pTemp->die - cl.time;

		pnext = pTemp->next;

		if (life < 0) // delete the dead entity
		{
			pTemp->next = gpTempEntFree;
			gpTempEntFree = pTemp;
			if (!pprev)
				gpTempEntActive = pnext;
			else
				pprev->next = pnext;
		}
		else
		{
			pprev = pTemp;

			VectorCopy (pTemp->entity.origin, pTemp->entity.msg_origins[0]);

			if (pTemp->flags & FTENT_SINEWAVE)
			{
				pTemp->entity.origin[0] = sin (pTemp->entity.syncbase * freq + pTemp->entity.baseline.origin[2]) * 8.0 + pTemp->entity.syncbase;
				// SanyaSho: no origin[1]?
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else
				for (i=0;i<3;i++)
					pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * frametime;

			if (pTemp->flags & FTENT_ROTATE)
				VectorAdd (pTemp->entity.angles, pTemp->entity.msg_angles[0], pTemp->entity.angles);

			if (pTemp->flags & FTENT_COLLIDEWORLD)
			{
				memset (&trace, 0, sizeof(trace));
				trace.fraction = 1.f;
				trace.allsolid = true;

				SV_RecursiveHullCheck (cl.worldmodel->hulls, cl.worldmodel->hulls[0].firstclipnode, 0.f, 1.f, pTemp->entity.msg_origins[0], pTemp->entity.origin, &trace);

				if (trace.fraction != 1.f)
				{
					VectorMA (pTemp->entity.msg_origins[0], trace.fraction * frametime, pTemp->entity.baseline.origin, pTemp->entity.origin);

					proj = DotProduct (pTemp->entity.baseline.origin, trace.plane.normal);
					VectorMA (pTemp->entity.baseline.origin, -proj*2, trace.plane.normal, pTemp->entity.baseline.origin);

					VectorScale (pTemp->entity.baseline.origin, 0.5, pTemp->entity.baseline.origin);
				}
			}

			if ((pTemp->flags & FTENT_FLICKER) && gTempEntFrame == pTemp->entity.effects)
			{
				dlight_t *dl = CL_AllocDlight (0);
				VectorCopy (pTemp->entity.origin, dl->origin);
				dl->radius = 60;
				dl->color[0] = 255;
				dl->color[1] = 120;
				dl->color[2] = 0;
				dl->die = cl.time + 0.01;
			}

			if (pTemp->flags & FTENT_SMOKETRAIL)
				R_RocketTrail (pTemp->entity.msg_origins[0], pTemp->entity.origin, 1);

			if (pTemp->flags & FTENT_GRAVITY)
				pTemp->entity.baseline.origin[2] += gravity;
			else if (pTemp->flags & FTENT_SLOWGRAVITY)
				pTemp->entity.baseline.origin[2] += gravitySlow;

			if (cl_numvisedicts < MAX_VISEDICTS)
				cl_visedicts[cl_numvisedicts++] = &pTemp->entity;
		}

		pTemp = pnext;
	}
}

/*
=================
CL_FxBlend
=================
*/
int CL_FxBlend (entity_t *ent)
{
	int ramt = ent->renderamt;

	switch (ent->renderfx)
	{
	case kRenderFXSlowPulse:		// 1
		ramt = ent->renderamt + 16 * sin(cl.time * 2.f);
		break;

	case kRenderFXFastPulse:		// 2
		ramt = ent->renderamt + 16 * sin(cl.time * 8.f);
		break;

	case kRenderFXSlowWidePulse:	// 3
		ramt = ent->renderamt + 64 * sin(cl.time * 2.f);
		break;

	case kRenderFXFastWidePulse:	// 4
		ramt = ent->renderamt + 64 * sin(cl.time * 8.f);
		break;

	case kRenderFXSlowFadeAway:		// 5
		if (ent->renderamt > 0)
			ent->renderamt -= 1;
		else
			ent->renderamt = 0;
		ramt = ent->renderamt;
		break;

	case kRenderFXFastFadeAway:		// 6
		if (ent->renderamt > 3)
			ent->renderamt -= 4;
		else
			ent->renderamt = 0;
		ramt = ent->renderamt;
		break;

	case kRenderFXSlowBecomeSolid:	// 7
		if (ent->renderamt < 255)
			ent->renderamt += 1;
		else
			ent->renderamt = 255;
		ramt = ent->renderamt;
		break;

	case kRenderFXFastBecomeSolid:	// 8
		if (ent->renderamt < 252)
			ent->renderamt += 4;
		else
			ent->renderamt = 255;
		ramt = ent->renderamt;
		break;

	case kRenderFXSlowStrobe:		// 9
		ramt = 20 * sin(cl.time * 4.f);
		if (ramt < 0)
			ramt = 0;
		else
			ramt = ent->renderamt;
		break;

	case kRenderFXFastStrobe:		// 10
		ramt = 20 * sin(cl.time * 16.f);
		if (ramt < 0)
			ramt = 0;
		else
			ramt = ent->renderamt;
		break;

	case kRenderFXFasterStrobe:		// 11
		ramt = 20 * sin(cl.time * 36.f);
		if (ramt < 0)
			ramt = 0;
		else
			ramt = ent->renderamt;
		break;

	case kRenderFXSlowFlicker:		// 12
		ramt = 20 * (sin(cl.time * 2.f) + sin(cl.time + 17.f));
		if (ramt < 0)
			ramt = 0;
		else
			ramt = ent->renderamt;
		break;

	case kRenderFXFastFlicker:		// 13
		ramt = 20 * (sin(cl.time * 16.f) + sin(cl.time + 23.f));
		if (ramt < 0)
			ramt = 0;
		else
			ramt = ent->renderamt;
		break;
	};

	// Limit color to [0..255]
	if (ramt > 255)
		return 255;
	if (ramt < 0)
		return 0;

	return ramt;
}
